/****************************************************************************************
 * Filename:	Agent.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Core implementaion of the API for management of the general abilites of agents
 * All user agents should make use of the code here.  Provides support for
 * implementing all of the appropraite conversation protocols that must be used
 * when communicating with the platform support services and also the complete
 * bootstrap process that must be followed in order to connect to the platform. The
 * functions here are used to hide the implementation details of the platform from 
 * the agent developer.
 * **************************************************************************************/

#include "agent.h"
#include "../util.h"
#include "../DBus/DBus-utils.h"
#include "../Codec/codecs.h"
#include "API.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function required by the D-Bus protocol but is not used in this apllication
 */
void agentUnregFunction(DBusConnection* conn, void* user_data) {}

/* Called by agents to register a function that is to be called whenever a message is
 * received
 * 
 * agent - the agent configuration object for this agent
 * fn - pointer to the function that must be called when a message is received
 */
void AP_registerMessageReceiverCallback(AgentConfiguration* agent, MessageReceiver fn) {
	agent->callbackFunction = fn;
}

/* Called when a user agent no longer wants a function to be called whenever a
 * message is received.
 * 
 * agent - configuration structure for the agent
 */
void AP_unregisterMessageReceiverCallback(AgentConfiguration* agent) {
	agent->callbackFunction = NULL;
}

/* Handles all messages received over the DBus message bus that are for the 
 * management of the agent, this messages are only sent by the platform itself and
 * not other agents, and are used to exert some management control over the agent.
 * These messages are also extensively in the testing of the platform.
 * 
 * All parameters are filled in by the underlying DBus code
 */
DBusHandlerResult agentManagementHandler(DBusConnection* connection, DBusMessage *msg, void *userData) {
	AgentConfiguration* agent = (AgentConfiguration*)userData;
	
	const char* method = dbus_message_get_member(msg);
	if (g_strcasecmp(MSG_PING, method) == 0) {
		//just output that we have received the message
		g_message("MANAGEMENT: Ping message received from %s", dbus_message_get_sender(msg));
	}
	else if (g_strcasecmp(MSG_TERMINATE, method) == 0) {
		g_main_quit(agent->mainLoop);
	}	
	else {
		g_message("MANAGEMENT: Unknown method called (%s)", method);
	}	
	return DBUS_HANDLER_RESULT_HANDLED;
}

/* handles the receipt of an agent message over the transport bus.  This is called for
 * every agent message receiveced.  This method calls a registered callback function
 * or adds the message to the queue appropriately for the wishes of the agent developer.
 * 
 * agent - the configuration object managed by the API for the agent for whom the message
 * 	was sent
 * msg - the DBus message object that was received over the bus
 */
void handleReceivedMessage(AgentConfiguration* agent, DBusMessage* msg) {
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	
	//use the DBus codec to retrieve the content of the message
	AgentMessage* message = decodeAgentMessage(&iter);
	
	//check to see if the callback function should be called
	if (agent->callbackFunction != NULL) {
		(*agent->callbackFunction)(agent, message);
	}
	else {
		//add the message to the end of the queue
		g_message("Message received and added to queue");
		agent->messageQueue = g_list_append(agent->messageQueue, msg);
		
		//GString* gstr = AgentMessageToString(message);
		//g_message("Message is %s", gstr->str);
	}	
}

/* used to handle all messages sent to this agent over the transport bus, it makes sure
 * that the appropriate handler is called depending on the type of message received/
 * 
 * All parameters and return values are filled in and handled by the DBus code, see
 * their documentation for more information
 */
DBusHandlerResult agentMessageHandler(DBusConnection* connection, DBusMessage *msg, void *userData) {
	AgentConfiguration* agent = (AgentConfiguration*)userData;
	
	const char* method = dbus_message_get_member(msg);
	if (g_strcasecmp(MTS_MSG, method) == 0) {		
		handleReceivedMessage(agent, msg);
	}	
	else {
		g_message("Unknown method called (%s)", method);
	}	
	return DBUS_HANDLER_RESULT_HANDLED;
}

/* used during bootstrap, this interrogates the AMS running on the agent platform
 * for a description of the platform services so that this information can be added
 * to the agents configuration strucuture.  It implements the agents side of the
 * get-description conversation protocol.
 * 
 * agent - the agents configuration strucutre that is making the request, before using
 * 	this function a connection must have already been made to the DBus and 
 * 	the platform service must be available.
 * err - the structure used to report any errors
 */
void getPlatformDescription(AgentConfiguration* agent, APError* err) {
	DBusError error;
	dbus_error_init(&error);
	
	//create a new method call that will be sent to the AMS to get the platform description
	DBusMessage* msg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	AMS_SERVICE_PATH, PLATFORM_SERVICE, MSG_GET_DESCRIPTION);
	DBusMessage* reply;
	
	//send the message and wait for a reply from the AMS
	reply = dbus_connection_send_with_reply_and_block(agent->connection, msg, WAIT_TIME, &error);
	if (reply == NULL) {
		APSetError(err, ERROR_COULD_NOT_CONTACT_PLATFORM);
		return;
	}
	
	//get the content of the message
	PlatformDescription* platform;
	DBusMessageIter iter;
	dbus_message_iter_init(reply, &iter);
	platform = decodePlatformDescription(&iter);
	
	if (platform == NULL) {
		APSetError(err, ERROR_INVALID_REPLY);
		return;
	}
	
	//now extract the required information to fill in our configuration
	agent->platformName = platform->name;
	int i;
	for (i=0; i<platform->services->len; i++) {
		PlatformServiceDescription* service = g_array_index(platform->services, PlatformServiceDescription*, i);
		if (g_strcasecmp(service->name->str, MTS_NAME) == 0) {
			agent->MTSAddress = service->address;
		}
		else if (g_strcasecmp(service->name->str, AMS_NAME) == 0) {
			agent->AMSAddress = service->address;
		}
		else if (g_strcasecmp(service->name->str, DF_NAME) == 0) {
			agent->DFAddress = service->address;
		}			
	}		
	
	//output the information that we have obtained to the log
	g_message("Platform Name\t:\t %s", agent->platformName->str);	
	g_message("MTS found at \t:\t %s", agent->MTSAddress->str);
	g_message("AMS found at \t:\t %s", agent->AMSAddress->str);
	g_message("DF found at \t:\t %s", agent->DFAddress->str);
}

/* used during bootstrap to register this agent with the AMS support service. Implements
 * the agent end of the AMS register conversation protocol.
 * 
 * agent - the agents configuration structure, before calling this function the connection
 * 	to the transport bus must have been obtained and the platform description retrieved
 * err - structure used to report any errors
 */
void registerAgent(AgentConfiguration* agent, APError* err) {
	DBusError error;
	dbus_error_init(&error);
	
	//create a new method call
	DBusMessage* msg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	AMS_SERVICE_PATH, PLATFORM_SERVICE, MSG_AMS_REGISTER);
	DBusMessage* reply;
	
	//build the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	encodeAID(&iter, agent->identifier);	
	
	//send the register request and wait for a reply from the AMS
	reply = dbus_connection_send_with_reply_and_block(agent->connection, msg, WAIT_TIME, &error);
	if (reply == NULL) {
		APSetError(err, ERROR_COULD_NOT_CONTACT_PLATFORM);
		return;
	}
	
	//check the reply to make sure that it was successful
	DBusMessageIter replyIter;
	dbus_message_iter_init(reply, &replyIter);
	GString* returnVal = decodeReply(&replyIter);
	if (g_strcasecmp(returnVal->str, RETURN_OK) !=0) {
		APSetError(err, returnVal->str);
		return;
	}
}

/* peforms the required set up to enable the agent to receive messages from the 
 * transport bus
 * 
 * agent - agents configuration structure that must have the connection to the transport
 * 	bus set
 * err - strucuture used to report errors
 */
void setUpMessageListner(AgentConfiguration* agent, APError* err) {
	//set up the listeners for agent messages
	DBusObjectPathVTable vTable;
	vTable.unregister_function = agentUnregFunction;
	vTable.message_function = agentMessageHandler;
	if (!dbus_connection_register_object_path(agent->connection, MESSAGE_PATH_ARRAY, 
		&vTable, agent)) {
		APSetError(err, ERROR_MESSAGE_LISTENER);	
	}
	
	//set up the listener for management calls
	DBusObjectPathVTable vTable2;
	vTable2.unregister_function = agentUnregFunction;
	vTable2.message_function = agentManagementHandler;
	if (!dbus_connection_register_object_path(agent->connection, MANAGEMENT_PATH_ARRAY, 
		&vTable2, agent)) {
		APSetError(err, ERROR_MESSAGE_LISTENER);	
	}	
}

/* This performs the basic boot strapping for an agent that will connect it to the running
 * platform and obtain information required to allow it send and receive messages
 * and interact with the platform services
 * 
 * agentName - the name that the agent wishes to be known by
 * err - the error structure that should be used for an errors
 * return - the initialised agent configuration
 */
AgentConfiguration* AP_newAgent(char* agentName, APError* err) {
	g_log_set_handler(NULL,  G_LOG_LEVEL_MASK, myLogHandler, NULL);	
	
	AgentConfiguration* agent = g_new(AgentConfiguration, 1);
	AgentConfigurationInit(agent);
	
	//connect to the session DBus
	DBusConnection* conn = getDBusConnection();
	if (conn == NULL) {
		APSetError(err, ERROR_CONNECTION_UNSUCCESSFUL);
		return NULL;
	}
	agent->connection = conn;	
	
	//check to see if the platform service can be found
	DBusError error;
	dbus_error_init(&error);
	if (!dbus_bus_name_has_owner(conn, PLATFORM_SERVICE, &error)) {
		APSetError(err, ERROR_PLATFORM_NOT_FOUND);
		return NULL;
	}
	
	//now attempt to get the platform description from the well known AMS service
	g_message("Obtaining platform description");
	getPlatformDescription(agent, err);
	if (APErrorIsSet(*err)) return NULL;
	
	//acquire our service on the message bus
	GString* serviceName = g_string_new(SERVICE_START);
	g_string_sprintfa(serviceName, "%s", agentName);
	if (!getService(conn, serviceName->str)) {
		g_string_free(serviceName, TRUE);
		APSetError(err, ERROR_SERVICE_NOT_ACQUIRED);
		return NULL;
	}
	else {
		g_message("Acquired service %s", serviceName->str);
	}
		
	//now build this agents identifier object	
	AID* id = g_new(AID, 1);
	AIDInit(id);
	GString* name = g_string_new(agentName);
	g_string_sprintfa(name, "@%s", agent->platformName->str);
	id->name = name;
	GString address;
	address = buildTransportAddress(serviceName->str, MESSAGE_PATH, MTS_MSG);
	GString* temp = g_string_new(address.str);
	g_array_append_val(id->addresses, temp);
	agent->identifier = id;
	
	GString* tt = AIDToString(*id);
	g_message("My identifier is\n%s", tt->str);
	g_string_free(tt, TRUE);	
	
	//now we need to register with the AMS on the platform
	g_message("Registering with AMS...");
	registerAgent(agent, err);
	if (APErrorIsSet(*err)) {
		g_message("Registration with AMS failed");
		return NULL;
	}
	else {
		g_message("Registration with AMS suceeded");
	}
	
	//now we need to set up the listeners to receive messages
	setUpMessageListner(agent, err);
	if (!APErrorIsSet(*err)) g_message("Listening on %s", MESSAGE_PATH);
	
	//set the reaming fields for the agent configuration
	agent->baseService = getBaseService(agent->connection);
	agent->mainLoop = g_main_loop_new(NULL, FALSE);
	agent->DFEntry->id = agent->identifier;
	
	return agent;
}

/* must be called when the agent no longer wishes to use the services offered by the
 * platform.  Performs all of the required deregistrations and disconnection from the
 * transport bus releasing all resources is was associated with on the platform
 * 
 * agent - agents configuration structure that was created when the agent bootstrapped
 * err - structure used to report all errrors
 */
void deRegisterAgent(AgentConfiguration* agent, APError* err) {
	DBusError error;
	dbus_error_init(&error);
	
	//create a new method call
	DBusMessage* msg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	AMS_SERVICE_PATH, PLATFORM_SERVICE, MSG_AMS_DEREGISTER);
	DBusMessage* reply;
	
	//build the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init_append(msg, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, agent->identifier->name->str);
	
	reply = dbus_connection_send_with_reply_and_block(agent->connection, msg, WAIT_TIME, &error);
	if (reply == NULL) {
		APSetError(err, ERROR_COULD_NOT_CONTACT_PLATFORM);
		return;
	}
	
	//check the reply to make sure that it was successful
	DBusMessageIter replyIter;
	dbus_message_iter_init(reply, &replyIter);
	GString* returnVal = decodeReply(&replyIter);
	if (g_strcasecmp(returnVal->str, RETURN_OK) !=0) {
		APSetError(err, returnVal->str);
		return;
	}
}

/* removes the agents entry from the DF support service
 * 
 * agent - agent configuration structure
 * err - structure used to report all errors
 */
void deRegisterAgentFromDF(AgentConfiguration* agent, APError* err) {
	DBusError error;
	dbus_error_init(&error);
	
	//create a new method call
	DBusMessage* msg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	DF_SERVICE_PATH, PLATFORM_SERVICE, MSG_DF_DEREGISTER);
	DBusMessage* reply;
	
	//build the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init_append(msg, &iter);
	encodeString(&iter, agent->identifier->name);
	//dbus_message_iter_append_string(&iter, agent->identifier->name->str);
	
	reply = dbus_connection_send_with_reply_and_block(agent->connection, msg, WAIT_TIME, &error);
	if (reply == NULL) {
		APSetError(err, ERROR_COULD_NOT_CONTACT_PLATFORM);
		return;
	}
	
	//check the reply to make sure that it was successful
	DBusMessageIter replyIter;
	dbus_message_iter_init(reply, &replyIter);
	GString* returnVal = decodeReply(&replyIter);
	if (g_strcasecmp(returnVal->str, RETURN_OK) !=0) {
		APSetError(err, returnVal->str);
		return;
	}
}

/* Performs all of the required clean up for the agent when it wishes to terminate
 * 
 * agent - the configuration for this agent
 * err - the error structure that should be used to store the errors
 */
void AP_finish(AgentConfiguration* agent, APError* err) {
	//we need to deregister from the DF
	g_message("De-Registering from the DF");
	deRegisterAgentFromDF(agent, err);
	if (APErrorIsSet(*err)) {
		g_message("Unable to de-register from DF - %s", err->message->str);
		APErrorReInit(err);
	}
	else {
		g_message("De-Registration from DF successful");
	}
	
	//we need to deregister from the AMS	
	g_message("De-Registering from the AMS");
	deRegisterAgent(agent, err);
	if (APErrorIsSet(*err)) {
		g_message("Unable to de-register from AMS - %s", err->message->str);
	}
	else {
		g_message("De-Registration from AMS successful");
	}
	
	//disconnect from the DBus
	dbus_connection_close(agent->connection);
}

/* Modifies the agents entry in the AMS, implementing the agent end of the AMS
 * modify conversation protocol.
 * 
 * agent - the agents configuration
 * err - the error structure that should be filled to hold any errors
 */
void AP_modifyAMSEntry(AgentConfiguration* agent, APError* err) {
	DBusError error;
	dbus_error_init(&error);
	
	//create a new method call
	DBusMessage* msg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	AMS_SERVICE_PATH, PLATFORM_SERVICE, MSG_AMS_MODIFY);
	DBusMessage* reply;
	
	//build the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	encodeAID(&iter, agent->identifier);
	
	reply = dbus_connection_send_with_reply_and_block(agent->connection, msg, WAIT_TIME, &error);
	if (reply == NULL) {
		APSetError(err, ERROR_COULD_NOT_CONTACT_PLATFORM);
		return;
	}
	
	//check the reply to make sure that it was successful
	DBusMessageIter replyIter;
	dbus_message_iter_init(reply, &replyIter);
	GString* returnVal = decodeReply(&replyIter);
	if (g_strcasecmp(returnVal->str, RETURN_OK) !=0) {
		APSetError(err, returnVal->str);
		return;
	}	
}

/* searches the AMS for an agent with the given name
 * 
 * agent - the configuration strucutre for the agent performing the search
 * name - the agent that you want to look for
 * err - the error structure that should be filled to hold any errors
 */
GArray* AP_searchAMS(AgentConfiguration* agent, char* name, APError* err) {
	GArray* results;
	
	DBusError error;
	dbus_error_init(&error);
	
	//check to see if we need to append the platform name to the name given
	GString* agentName = g_string_new(name);
	if (strstr(name, "@") == NULL) g_string_sprintfa(agentName, "@%s", agent->platformName->str);
	
	//create a new method call
	DBusMessage* msg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	AMS_SERVICE_PATH, PLATFORM_SERVICE, MSG_AMS_SEARCH);
	DBusMessage* reply;
	
	//build the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init_append(msg, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, agentName->str);
	
	reply = dbus_connection_send_with_reply_and_block(agent->connection, msg, WAIT_TIME, &error);
	if (reply == NULL) {
		APSetError(err, ERROR_COULD_NOT_CONTACT_PLATFORM);
		return results;
	}
	
	//get the reply
	DBusMessageIter replyIter;
	dbus_message_iter_init(reply, &replyIter);	
	results = decodeAIDArray(&replyIter);	
	
	return results;
}

/* Registers the agents df service descriptions with the DF service
 * 
 * agent - the agent configuration object 
 * err - the structure that should be filled in to hold any errors
 */
void AP_registerWithDF(AgentConfiguration* agent, APError* err) {
	DBusError error;
	dbus_error_init(&error);
	
	//make this agent the owner of the DF entry
	agent->DFEntry->id = agent->identifier;
	
	//create a new method call
	DBusMessage* msg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	DF_SERVICE_PATH, PLATFORM_SERVICE, MSG_DF_REGISTER);
	DBusMessage* reply;
	
	//build the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	encodeDFEntry(&iter, agent->DFEntry);
	
	reply = dbus_connection_send_with_reply_and_block(agent->connection, msg, WAIT_TIME, &error);
	if (reply == NULL) {
		APSetError(err, ERROR_COULD_NOT_CONTACT_PLATFORM);
	}
	
	//check the reply to make sure that it was successful
	DBusMessageIter replyIter;
	dbus_message_iter_init(reply, &replyIter);
	GString* returnVal = decodeReply(&replyIter);
	if (g_strcasecmp(returnVal->str, RETURN_OK) !=0) {
		APSetError(err, returnVal->str);
		return;
	}	
}

/* modifes an agents entry in the DF
 * 
 * agent - the configuration for the agent that is being managaed by the API
 * err - the structure that should be used to fill in for errors
 */
void AP_modifyDFEntry(AgentConfiguration* agent, APError* err) {
	DBusError error;
	dbus_error_init(&error);
	
	//create a new method call
	DBusMessage* msg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	DF_SERVICE_PATH, PLATFORM_SERVICE, MSG_DF_MODIFY);
	DBusMessage* reply;
	
	//build the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	encodeDFEntry(&iter, agent->DFEntry);
	
	reply = dbus_connection_send_with_reply_and_block(agent->connection, msg, WAIT_TIME, &error);
	if (reply == NULL) {
		APSetError(err, ERROR_COULD_NOT_CONTACT_PLATFORM);
	}
	
	//check the reply to make sure that it was successful
	DBusMessageIter replyIter;
	dbus_message_iter_init(reply, &replyIter);
	GString* returnVal = decodeReply(&replyIter);
	if (g_strcasecmp(returnVal->str, RETURN_OK) !=0) {
		APSetError(err, returnVal->str);
		return;
	}
}

/* searches the DF for entries that match the given template
 * 
 * template - the template for the search
 * err  - the structure that should be used to report errors
 * return - the array containing the matches
 */
GArray* AP_searchDF(AgentConfiguration* agent, AgentDFDescription* template, APError* err) {
	DBusError error;
	dbus_error_init(&error);
	
	//create a new method call
	DBusMessage* msg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	DF_SERVICE_PATH, PLATFORM_SERVICE, MSG_DF_SEARCH);
	DBusMessage* reply;
	
	//build the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	encodeDFEntry(&iter, template);
	
	reply = dbus_connection_send_with_reply_and_block(agent->connection, msg, WAIT_TIME, &error);
	if (reply == NULL) {
		APSetError(err, ERROR_COULD_NOT_CONTACT_PLATFORM);
	}
	
	DBusMessageIter replyIter;
	dbus_message_iter_init(reply, &replyIter);
	GArray* results = decodeDFEntryArray(&replyIter);
	return results;	
}

/* used only within the API to build an envelope strucutre for a given message that
 * an agent wishes to send
 * 
 * msg - the message for which an envelope is to be built
 * returns - the envelope for the message
 */
ACLEnvelope* buildEnvelope(ACLMessage* msg) {
	ACLEnvelope* envelope = g_new(ACLEnvelope, 1);
	ACLEnvelopeInit(envelope);
	
	//set the from field
	ACLEnvelopeSetFrom(envelope, msg->sender);
	
	//set all of the receivers
	int i;
	for (i=0; i<msg->receivers->len; i++) {
		AID* id = g_array_index(msg->receivers, AID*, i);
		ACLEnvelopeAddTo(envelope, id);
	}
	
	//set the representation
	ACLEnvelopeSetACLRepresentation(envelope, DBUS_ACL_REPRESENTATION);
	
	return envelope;
}

/* called to send an agent message over the transport bus to other agents.  It
 * implements the agent end of the MTS send conversation protocol
 * 
 * agent - sending agents configuration strucutre
 * msg - the FIPA-ACL message that is to be sent
 * err - structure used to hold any errors
 */
void AP_send(AgentConfiguration* agent, ACLMessage* msg, APError* err) {
	//check to make sure that there is at least one recipient for this message
	if (msg->receivers->len == 0) {
		APSetError(err, ERROR_MUST_HAVE_RECEIVER);
		return;
	}
	
	//make sure that the message has a performative
	if (msg->performative == NULL) {
		APSetError(err, ERROR_PERFORMATIVE_REQUIRED);
		return;		
	}
	if (g_strcasecmp(msg->performative->str, "") == 0) {
		APSetError(err, ERROR_PERFORMATIVE_REQUIRED);
		return;		
	}
		
	//set the sender to be this agent, overriding anything set by the user
	ACLMessageSetSender(msg, agent->identifier);
	
	//build the envelope that will be used for this message
	ACLEnvelope* envelope = buildEnvelope(msg);
	
	//build the complete message
	AgentMessage* message = g_new(AgentMessage, 1);
	AgentMessageInit(message);
	message->envelope = envelope;
	message->payload = msg;
	
	//now print the message to the screen
	GString* temp = AgentMessageToString(message);
	g_message("Message is \n%s", temp->str);
	g_string_free(temp, TRUE);
	
	//build the message that will be sent to the MTS service
	DBusError error;
	dbus_error_init(&error);
	
	//create a new method call
	DBusMessage* DBusMsg = dbus_message_new_method_call(PLATFORM_SERVICE, 
	 	MTS_SERVICE_PATH, PLATFORM_SERVICE, MTS_MSG);
	
	//build the content of the message
	DBusMessageIter iter;
	dbus_message_iter_init(DBusMsg, &iter);
	encodeAgentMessage(&iter, message);
	
	//send the message without expecting a reply
	dbus_message_set_no_reply(DBusMsg, TRUE);
	dbus_connection_send(agent->connection, DBusMsg, NULL);
	dbus_connection_flush(agent->connection);
}

/* optionally called after an agent has performed all initialisation and wishes to wait
 * until it receives a message, it is a standard GMainLoop so the agent can set
 * up its own timers and other event handlers
 * 
 * agent - agent configurations strucutre
 */
void AP_agentSleep(AgentConfiguration* agent) {
	g_main_loop_run(agent->mainLoop);
}

/* used for the SWIG implementation to convert a GString into a standard char* that
 * is more likely to be properly understood by the programming language using
 * the interface
 * 
 * gstr - the string to convert
 * returns - the char* part of the GString
 */
char* gstrToString(GString* gstr) {
	return gstr->str;
}
