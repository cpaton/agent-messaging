/****************************************************************************************
 * Filename:	DBus-utils.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Functions used to make using the Dbus system easier
 * **************************************************************************************/

#include "DBus-utils.h"
#include "../platform-defs.h"
#include <glib.h>
#include <dbus/dbus-glib.h>

/* Function that builds a valid transport address for a service hanging of
 * the DBus
 * service - the name of the service that it has acquired on dbus
 * path - the path that the message should be sent to
 * method - the method that should be called at the given path
 * return - the fully qualified transport address that can be used in an agentID struct
 */
GString buildTransportAddress(char* service, char* path, char* method) {
	GString* temp = g_string_new(DBUS_PROTOCOL_NAME);
	g_string_sprintfa(temp, ":%s:%s:%s", service, path, method);
	return *temp;
}

/* function that attempts to acquire a service for a given connection
 * serviceName - the name of the service that you wish to acquire
 * return - true if the service was acquired false otherwise
 */
gboolean getService(DBusConnection* conn, char* serviceName) {
	DBusError error;
	dbus_error_init(&error);
	int retVal;
	retVal = dbus_bus_request_name(conn, serviceName, DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);
	if (retVal == -1) {		
		dbus_error_free(&error);
		return FALSE;
	}
	else {
		return TRUE;
	}
}

/* Called to get a connection to the session DBus.  The environment variable
 * DBUS_SESSION_BUS_ADDRESS must be set before this funcion can be used
 * the connection is integrated with GLib
 * return - the connection object
 */
DBusConnection* getDBusConnection() {
	DBusError error;
	DBusConnection* conn;
	dbus_error_init(&error);	
	
	GError* gError;
	gError = NULL;
		
	//connect to the session bus and integrate it with a GLib
	conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
	dbus_connection_setup_with_g_main(&conn, &gError);
	if (conn == NULL) {
		//we were unable to connect to the session bus
		g_error("Unable to connect to the session bus %s", gError->message);
		g_error_free(gError);
		return NULL;		
	}	
	
	/*//connect to the session bus
	conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if (dbus_error_is_set(&error)) {
		//we were unable to connect to the session bus
		g_error("Unable to connect to the session bus %s", error.message);
		dbus_error_free(&error);
		return NULL;		
	}	*/
	
	return conn;
}

/* Determines the base service for the associated connection.  Before calling this
 * the connection must have been made to the DBus
 * conn - the connection to the message bus
 * return - the base service for the connection
 */
GString* getBaseService(DBusConnection* conn) {
	const char* baseService;
	baseService = dbus_bus_get_unique_name(conn);
	GString* str = g_string_new((char*)baseService);
	return str;	
}
