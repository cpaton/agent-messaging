/****************************************************************************************
 * Filename:	platform.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * API functions to allow the platform to be started from any program.  Also includes
 * all of the components that make up the platform
 * **************************************************************************************/

#include "platform.h"
#include "../platform-defs.h"
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include "../util.h"
#include "API.h"

/**********************************************************************************
 * *********** DECLARATIONS OF PLATFORM COMPONENTS ***********
 * ********************************************************************************/
Platform thePlatform; /* single configuration object for the platform */
MTSConfiguration theMTS; /* the one and only interaction layer */
AMSConfiguration theAMS; /* the one and only AMD */
DFConfiguration theDF; /* the one and only DF */

/* required by DBus but never used within this application */
void PlatformUnregFunction(DBusConnection* conn, void* user_data) {
	g_message("Platform Unregister function called");
}

/* handler for messages that are sent to the platform service.  At present no messages
 * should be sent directly to the service but to the individual platform services instead
 * 
 * all paraemters and return value are filled in by the underlying DBus code, see
 * their documentation for more information
 */
DBusHandlerResult allMessageHandler(DBusConnection* connection, DBusMessage* msg, void *userData) { 
	//just print out who sent the message
	//g_message("Platform Filter : Received message from %s", dbus_message_get_sender(msg));
	
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED; 
}

/* handles calls to the terminate path for the platform.  This listener is used to terminate
 * the platform when it is no longer needed
 * 
 * all paraemters and return value are filled in by the underlying DBus code, see their
 * documentation for more information
 */
DBusHandlerResult TerminateHandler(DBusConnection* connection, DBusMessage *msg, void *userData) {
	//we have been told to terminate so we shall do so
	g_message("Platform : Sent terminate message from %s", dbus_message_get_sender(msg));		
	GMainLoop* mainLoop = (GMainLoop*)userData;
	g_main_quit(mainLoop);	
	return DBUS_HANDLER_RESULT_HANDLED;
}

/* called to bootstrap the platform.  It connects to the DBus and then starts up the
 * interaction layer, AMS and DF and all of the listeners for these services so
 * that any messages are handled.  The platform then goes to sleep waiting for
 * requests from agents
 */
void bootstrapPlatform() {
	//set up the default handler to just print messages to the terminal window	
	g_log_set_handler(NULL,  G_LOG_LEVEL_MASK, myLogHandler, NULL);	
	
	//connect the platform to the session bus
	GError* gError;	
	DBusError error;
	DBusConnection* conn;
	GMainLoop* mainLoop  = g_main_loop_new(NULL, FALSE);
			
	//perform initialisation
	dbus_error_init(&error);
	gError = NULL;
		
	//connect to the session bus and integrate it with a GLib
	conn = dbus_bus_get_with_g_main(DBUS_BUS_SESSION, &gError);
	if (conn == NULL) {
		//we were unable to connect to the session bus
		g_error("Unable to connect to the session bus %s", gError->message);
		g_error_free(gError);
		exit(1);		
	}
	else {
		g_message("Connection to D-Bus successful");
	}		
	
	//get the base service of this connection
	const char* baseService;
	baseService = dbus_bus_get_base_service(conn);
	g_message("Platform base service is %s", baseService);
	
	//acquire the platform service
	int retVal;
	retVal = dbus_bus_acquire_service(conn, PLATFORM_SERVICE, 1, &error);
	if (retVal == -1) {
		g_error("Platform unable to acquire the service %s (%s)", PLATFORM_SERVICE, error.message);
		dbus_error_free(&error);
		exit(1);
	}
	else {
		g_message("Plaform acquired the service %s", PLATFORM_SERVICE);
	}
	
	//add the filter that will listen for all messages
	if (!dbus_connection_add_filter(conn, allMessageHandler, NULL, NULL)) {
		g_error("Unable to add the message filter for that will capture all messages");
		exit(1);
	}
	
	//add the handler telling us to terminate
	DBusObjectPathVTable termVTable;
	termVTable.unregister_function = PlatformUnregFunction;
	termVTable.message_function = TerminateHandler;
	if (!dbus_connection_register_object_path(conn, TERMINATE_PATH_ARRAY, 
		&termVTable, mainLoop)) {
		g_error("Unable to register the handler for the terminating the Platform");
		exit(1);		
	}
	else {
		g_message("Platform registered handler for %s", TERMINATE_PATH);
	}
	
	//set up the global description of the platform
	thePlatform.name = getMachineName();
	thePlatform.service = g_string_new(PLATFORM_SERVICE);
		
	//start up the MTS and AMS
	MTS_start(conn, mainLoop, (char *)baseService);	
	AMS_start(conn, mainLoop, (char *)baseService);
	DF_start(conn, mainLoop, (char *)baseService);
	
	//set up the platform description
	theAMS.platformDescription = g_new(PlatformDescription, 1);
	InitPlatformDescription(theAMS.platformDescription);
	theAMS.platformDescription->name = getMachineName();
	//theAMS.platformDescription->services = g_array_new(FALSE, FALSE, sizeof(PlatformServiceDescription*));
	g_array_append_val(theAMS.platformDescription->services, theAMS.description);
	g_array_append_val(theAMS.platformDescription->services, theMTS.description);
	g_array_append_val(theAMS.platformDescription->services, theDF.description);	
	
	//register all of the platform services with the agent directory in the AMS
	AMS_register(theAMS.configuration->identifier, NULL);
	AMS_register(theMTS.configuration->identifier, NULL);
	AMS_register(theDF.configuration->identifier, NULL);	
	
	//now that all the handlers are registered we can start the main loop with the 
	//help of GLib
	g_message("Platform sleeping...");
	g_main_loop_run(mainLoop);
	g_message("Platform Terminating...");
	
	//perform all of the required clean up
	MTS_end();	
	AMS_end();
	DF_end();
}
