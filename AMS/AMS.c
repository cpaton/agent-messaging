/****************************************************************************************
 * Filename:	AMS.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Functionality for the implementation of the AMS support service.  All of the publicly
 * accessible functions over the transport bus are defined here as is the initialisation
 * and exit code.
 * **************************************************************************************/

#include "AMS.h"
#include "../platform-defs.h"
#include "../util.h"
#include "../API/API.h"
#include "../DBus/DBus-utils.h"
#include "../Codec/DBusCodec.h"
#include <stdlib.h>
#include <string.h>

/* Function required by the D-Bus protocol but is not used in this apllication
 */
void AMSUnregFunction(DBusConnection* conn, void* user_data) {
	g_message("AMS Unregister function called");
}

/* Checks an agent name to see if it ends in  @ followed by the name of the 
 * platform that the AMS assigned.
 * 
 * returns TRUE if the name ends in the platform name false otherwise
 */
gboolean checkName(GString* name) {
	//the name must end in the platform name
	GString* toFind = g_string_new("@");
	g_string_sprintfa(toFind, "%s", thePlatform.name->str);
	
	if (strstr(name->str, toFind->str) == NULL) return FALSE;
	return TRUE;
}

/* Performs all of the required operations to handle a request from an agent to register
 * itself on the platform.  It sends the reply depending on whether the registration
 * was successful or not
 * 
 * msg: the DBusMessage object that holds the request message sent by the agent
 * that must implement the AMS register conversation protocol using the DBusCodec
 */
void handleRegister(DBusMessage* msg) {
	DBusMessage* reply;
	
	//get the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	AID* id = decodeAID(&iter);	
	GString* temp = AIDToString(*id);
	g_message("AMS: identifier read as %s", temp->str);
	g_string_free(temp, TRUE);
	
	//now perform the registration
	APError error;
	APErrorInit(&error);
	AMS_register(id, &error);
	GString* retVal;
	if (APErrorIsSet(error)) {
		retVal = g_string_new(error.message->str);
		g_warning("AMS: Error registering agent %s", error.message->str);
	}
	else {
		retVal = g_string_new(RETURN_OK);
		g_message("AMS: registration succeeded");
	}
	
	//build the reply to the message
	reply = dbus_message_new_method_return(msg);
	DBusMessageIter replyIter;
	dbus_message_iter_init_append(reply, &replyIter);
	encodeReply(&replyIter, retVal->str);
	
	//send the reply back
	dbus_connection_send(theAMS.configuration->connection, reply, NULL);
	dbus_connection_flush(theAMS.configuration->connection);	
	
	g_string_free(retVal, TRUE);
}

/* Performs all of the required operations to handle a modify request from an agent
 * It sends the reply depending on the successful completion of the modifcation or not.
 * 
 * msg: the DBusMessage object that holds the request message sent by the agent
 * that must implement the AMS modify conversation protocol using the DBusCodec
 */
void handleModify(DBusMessage* msg) {
	DBusMessage* reply;
	
	//get the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	AID* id = decodeAID(&iter);	
	GString* temp = AIDToString(*id);
	g_message("AMS: identifier read as %s", temp->str);
	g_string_free(temp, TRUE);
	
	//now perform the modify
	
	//check to make sure that the agent exists
	int index = AMS_agentExists(id->name);
	GString* retVal;
	if (index == -1) {
		retVal = g_string_new(ERROR_AGENT_DOES_NOT_EXIST);
		g_message("AMS: Agent not found in registry");
	}
	else {
		g_array_remove_index(theAMS.agentDirectory, index);
		g_array_append_val(theAMS.agentDirectory, id);
		retVal = g_string_new(RETURN_OK);
		g_message("AMS: Modify complete");
	}
	
	//build the reply to the message
	reply = dbus_message_new_method_return(msg);
	DBusMessageIter replyIter;
	dbus_message_iter_init_append(reply, &replyIter);
	encodeReply(&replyIter, retVal->str);
	
	//send the reply back
	dbus_connection_send(theAMS.configuration->connection, reply, NULL);
	dbus_connection_flush(theAMS.configuration->connection);	
	
	g_string_free(retVal, TRUE);
	//AMS_printDirectory();
}

/* Performs all of the required operations to handle a search request from an agent
 * It sends the reply depending on the results of the search.
 * 
 * msg: the DBusMessage object that holds the request message sent by the agent
 * that must implement the AMS search conversation protocol using the DBusCodec
 */
void handleSearch(DBusMessage* msg) {
	DBusMessage* reply;
	
	//get the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	char* name;
	dbus_message_iter_get_basic(&iter, &name);
	g_message("AMS: looking for agent with name %s", name);
	
	//now perform the search	
	GString* str = g_string_new(name);
	int index = AMS_agentExists(str);
	GArray* results = g_array_new(FALSE, FALSE, sizeof(AID*));
	if (index != -1) {
		AID* id = g_array_index(theAMS.agentDirectory, AID*, index);
		g_array_append_val(results, id);
	}
	
	//build the reply to the message
	reply = dbus_message_new_method_return(msg);
	DBusMessageIter replyIter;
	dbus_message_iter_init_append(reply, &replyIter);
	g_message("AMS: found %d matches", results->len);
	encodeAIDArray(&replyIter, results);
	
	//send the reply back
	dbus_connection_send(theAMS.configuration->connection, reply, NULL);
	dbus_connection_flush(theAMS.configuration->connection);	
}

/* Performs all of the required operations to handle a de-register request from an agent
 * It sends the reply depending on the results of the operation.
 * 
 * msg: the DBusMessage object that holds the request message sent by the agent
 * that must implement the AMS de-register conversation protocol using the DBusCodec
 */
void handleDeRegister(DBusMessage* msg) {
	DBusMessage* reply;
	reply = dbus_message_new_method_return(msg);
	APError error;
	APErrorInit(&error);
	
	//get the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	char* agentName;
	dbus_message_iter_get_basic(&iter, &agentName);
	g_message("AMS: agent to de-register is %s", agentName);

	//now de register the agent from the platform	
	AMS_deRegister(agentName, &error);
	GString* retVal;
	if (APErrorIsSet(error)) {
		retVal = g_string_new(error.message->str);
	}
	else {
		retVal = g_string_new(RETURN_OK);
	}
	
	//build the reply to the message
	reply = dbus_message_new_method_return(msg);
	DBusMessageIter replyIter;
	dbus_message_iter_init_append(reply, &replyIter);
	encodeReply(&replyIter, retVal->str);
	
	//send the reply back
	dbus_connection_send(theAMS.configuration->connection, reply, NULL);
	dbus_connection_flush(theAMS.configuration->connection);	
	
	//g_string_free(retVal, TRUE);
	//APErrorFree(&error);
}

/* Handles a request from an agent during its bootstrap for the service root for the platform
 * It sends a reply back to the agent containing service descriptions of the AMS, DF
 * and interaction layer.  The reply conforms to the AMS get-description conversation
 * protocol.
 * 
 * msg: the DBusMessage object that holds the request message sent by the agent
 * that must implement the AMS get-description conversation protocol using the DBusCodec
 */
void sendDescription(DBusMessage* msg) {
	//create the reply to the message that has been sent
	DBusMessage* reply;
	reply = dbus_message_new_method_return(msg);

	//build up the content of the message
	DBusMessageIter args;
	dbus_message_iter_init_append(reply,&args);	
	encodePlatformDescription(&args, theAMS.platformDescription);
	
	//send the reply	
	dbus_connection_send(theAMS.configuration->connection, reply, NULL);
	dbus_connection_flush(theAMS.configuration->connection);
}

/* This function is called whenever a message is sent to the AMS service that is running
 * on the platform. No user data is passed into this function it is all handled by the
 * DBus code internally.  The function calls the appropriate handler for the type 
 * of message that it has received based on the member field of the message
 * 
 * returns: values depend on the implementation of the DBus code see their documentation
 * for appropriate values
 */
DBusHandlerResult AMSMessageHandler(DBusConnection* connection, DBusMessage *msg, void *userData) {
	//check what the member is that is being called
	const char* method = dbus_message_get_member(msg);
	if (g_ascii_strcasecmp(MSG_PING, method) == 0) {
		//just output that we have received the message
		g_message("AMS: Ping message received from %s", dbus_message_get_sender(msg));
	}
	else if (g_ascii_strcasecmp(MSG_PRINT_AGENT_DIRECTORY, method) == 0) {
		//print out the directory to the screen
		AMS_printDirectory();
	}
	else if (g_ascii_strcasecmp(MSG_AMS_DEREGISTER, method) == 0) {
		//print out the directory to the screen
		g_message("AMS: received de-register request from %s", dbus_message_get_sender(msg));
		handleDeRegister(msg);
	}		
	else if (g_ascii_strcasecmp(MSG_GET_DESCRIPTION, method) == 0) {
		g_message("AMS: received request for platform description from %s", dbus_message_get_sender(msg));
		sendDescription(msg);
	}
	else if (g_ascii_strcasecmp(MSG_AMS_REGISTER, method) == 0) {
		g_message("AMS: received register request from %s", dbus_message_get_sender(msg));
		handleRegister(msg);
	}
	else if (g_ascii_strcasecmp(MSG_AMS_MODIFY, method) == 0) {
		g_message("AMS: received modify request from %s", dbus_message_get_sender(msg));
		handleModify(msg);
	}
	else if (g_ascii_strcasecmp(MSG_AMS_SEARCH, method) == 0) {
		g_message("AMS: received search request from %s", dbus_message_get_sender(msg));
		handleSearch(msg);
	}			
	else {
		g_message("AMS: Unknown method called (%s)", method);
	}	
	return DBUS_HANDLER_RESULT_HANDLED;
}

/* Called during bootstrapping of the platform and initialises the AMS into the state
 * where it is ready to carry out its responsibilities.  It ensures that it is configured properly
 * by setting up its configuration strucutre, its AID database and listeners on the 
 * DBus message bus
 * 
 * conn - the platforms connection to the session DBus
 * mainLoop - the platforms GLib main loop
 * baseSerivce - the platforms base service on the D-Bus session bus assigned to it
 * 	when it connected.
 */
void AMS_start(DBusConnection* conn, GMainLoop* mainLoop, gchar* baseService) {
	DBusError error;
	dbus_error_init(&error);
	dbus_connection_ref(conn);

	//set up the AMS configuration;
	theAMS.configuration = g_new(AgentConfiguration, 1);
	AgentConfigurationInit(theAMS.configuration);
	theAMS.configuration->connection = conn;
	theAMS.configuration->mainLoop = mainLoop;
	theAMS.configuration->baseService = g_string_new(baseService);
	
	//initialise the white pages registry
	theAMS.agentDirectory = g_array_new(FALSE, FALSE, sizeof(AID*));
	
	//set up this services agent identifier
	AID* id = g_new(AID, 1);
	AIDInit(id);	
	GString* temp = g_string_new(AMS_NAME);
	g_string_sprintfa(temp, "@%s", thePlatform.name->str);
	AIDSetName(id, temp->str);
	GString address = buildTransportAddress(thePlatform.service->str, AMS_SERVICE_PATH, "msg");
	AIDAddAddress(id, address.str);
	theAMS.configuration->identifier = id;
	
	//set up this services description
	theAMS.description = g_new(PlatformServiceDescription, 1);
	theAMS.description->name = g_string_new(AMS_NAME);
	theAMS.description->address = g_string_new(address.str);
	
	//now set up a handler that will listen to all messages on a given path
	DBusObjectPathVTable vTable;
	vTable.unregister_function = AMSUnregFunction;
	vTable.message_function = AMSMessageHandler;
	if (!dbus_connection_register_object_path(theAMS.configuration->connection, AMS_SERVICE_PATH, 
		&vTable, NULL)) {
		g_error("Unable to register the handler for the AMS service");
		exit(1);		
	}
	else {
		g_message("AMS: Listening on %s", AMS_SERVICE_PATH);
	}
}

/* Called when the platform has been told to terminate.  Disconnects the AMS from
 * the D-Bus
 */
void AMS_end() {
	g_message("AMS disconnecting from the DBus");
	dbus_connection_unref(theAMS.configuration->connection);
	g_string_free(theAMS.configuration->baseService, TRUE);
}

/* adds an entry into the AMS registry that is maintained by the AMS, any problems are 
 * reported into the error structure
 * 
 * id - the agent identifier that needs to be added
 * err - the error struture that should be used to report errors
 */
void AMS_register(AID* id, APError* err) {
	//check to make sure that we have been passed a identifiuer
	if (id == NULL) {
		APSetError(err, ERROR_NULL);
		return;
	}
	
	//check to make sure that all the fields have been filled in
	if (id->name == NULL || id->addresses->len == 0) {
		APSetError(err, ERROR_REQUIRED_FIELD_MISSING);
		return;
	}
	
	//check to make sure that the name is formatted correctly
	if (!checkName(id->name)) {
		APSetError(err, ERROR_INVALID_AGENT_NAME);
		return;
	}
	
	//check to make sure that the doesnot already exist
	if (AMS_agentExists(id->name) != -1) {
		APSetError(err, ERROR_DUPLICATE_AGENT);
		return;
	}
	
	//add the identifier to the registry
	g_array_append_val(theAMS.agentDirectory, id);
}

/* prints out the current status of the agent directory to the log which by default is just
 * the terminal window - used for testing and debugging purposes
 */
void AMS_printDirectory() {
	g_message("There are %d entries in the directory", theAMS.agentDirectory->len);
	int i;
	for (i=0; i < theAMS.agentDirectory->len; i++) {
		AID* id = g_array_index(theAMS.agentDirectory, AID*, i);
		GString* temp = AIDToString(*id);
		g_message("%s", temp->str);
		g_string_free(temp, TRUE);
	}
}

/* Looks in the agent directory to see an agent with that name already exists
 * 
 * name - the name of the agent that is being looked for
 * return - the index into the array if the element is found -1 otherwise
 */
int AMS_agentExists(GString* name) {
	if (name == NULL) return -1;
	int i;
	for (i=0; i < theAMS.agentDirectory->len; i++) {
		AID* id = g_array_index(theAMS.agentDirectory, AID*, i);
		if (g_ascii_strcasecmp(id->name->str, name->str) == 0) return i;
	}
	return -1;
}

/* Removes the given agent from the agent directory
 *
 * name - the full name of the agent that you want to remove
 * err - the structure that should be used to fill in the error
 */
void AMS_deRegister(char* name, APError* err) {
	GString* temp = g_string_new(name);	
	int retVal = AMS_agentExists(temp);
	if (retVal == -1) {
		APSetError(err, ERROR_AGENT_NOT_FOUND);
	}
	else {
		g_array_remove_index(theAMS.agentDirectory, retVal);
	}
}
