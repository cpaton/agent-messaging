/****************************************************************************************
 * Filename:	MTS.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implementation of the interaction layer that is responsible for routing all agent
 * messages over the platform to the correct recipients
 * **************************************************************************************/

#include "MTS.h"
#include "../API/API.h"
#include "../DBus/DBus-utils.h"
#include "../platform-defs.h"
#include "../Codec/codecs.h"
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Function put here cos it is required by D-Bus but never used by this application */
void unregFunction(DBusConnection* conn, void* user_data) {
	g_message("Unregister function called");
}

/* gets an address from the agent-identifier given that the interaction layer knows how
 * to transport to, this enables multiple protocols to be used for transportation of messages
 * at present it only understands the dbus protocol
 * 
 * id - the agent identifier to search for a transport address
 * returns - the string representing the address, if one cannot be found then NULL
 */
GString* getTransportableAddress(AID* id) {
	if (id == NULL) return NULL;	
	if (id->addresses->len == 0) return NULL;
	
	GString* dbusProtocol = g_string_new(DBUS_PROTOCOL_NAME);	
	int i;
	for (i=0; id->addresses->len; i++) {
		GString* gstr = g_array_index(id->addresses, GString*, i);
		if (g_ascii_strncasecmp(gstr->str, dbusProtocol->str, dbusProtocol->len) == 0)
			return gstr;
	}
	return NULL;
}

/* utlitity function for interaction with the transport bus that creates the shell of an
 * DBus message that can be sent over the transport bus.  The method call is built
 * from a transport address from an agent identifier that uses the DBus protocol
 * 
 * address - string in the correct transport address format for DBus
 * returns - DBus message created for the method call that should be filled with
 * 	content
 */
DBusMessage* generateMethodCall(GString* address) {
	//we need to deal with the address to decide how to deal with the message call
	gchar** addressParts = g_strsplit(address->str, ":", 5);
	
	//create a new method call
	DBusMessage* msg = dbus_message_new_method_call(addressParts[1], 
	 	addressParts[2], addressParts[1], addressParts[3]);
	 	
	g_strfreev(addressParts);
	 
	return msg;
}

/* used to deliver a message to an agent after the initial processing has been completed
 * the intended receiver field must be set, as this is used to determine the end point
 * 
 * message - the message that is to be delivered to the agent
 */
void deliverMessage(AgentMessage* message) {
	//get the address of the agent that this should be sent to
	GString* address = getTransportableAddress(message->envelope->intendedReceiver);	
	
	//check to see if we have a dbus address
	if (address == NULL) {
		//we need to perform an AMS lookup to find the address of this agent
		
		//check to make sure that we have a fully qualified name
		GString* agentName = g_string_new(message->envelope->intendedReceiver->name->str);
		if (strstr(agentName->str, "@")  == NULL) 
			g_string_sprintfa(agentName, "@%s", thePlatform.name->str);			 
		
		int index = AMS_agentExists(agentName);	
		if (index != -1) {
			AID* id = g_array_index(theAMS.agentDirectory, AID*, index);
			
			//check to make sure that we now have an address			
			address = getTransportableAddress(id);
			if (address == NULL) {
				//we cannot find the transport address for this agent that we know of
				GString* gstr = AIDToString((*message->envelope->intendedReceiver));
				g_message("MTS: unable to deliver message (could not find a transport address) to %s", gstr->str);
				g_string_free(gstr, TRUE);
				return;
			}
		}
		else {
			//we cannot find the transport address for this agent that we know of
			GString* gstr = AIDToString((*message->envelope->intendedReceiver));
			g_message("MTS: unable to deliver message (could not find an AMS entry for the agent) to %s", gstr->str);
			g_string_free(gstr, TRUE);
			return;
		}
	}
	
	//now go ahead an deliver the message
	//This is where we would decide what MTP to use to transport it	
	g_message("MTS: Delivering message to %s", address->str);	
	
	DBusMessage* msg = generateMethodCall(address);	
	
	//put the envelope and the message into this method call
	DBusMessageIter iter;
	dbus_message_iter_init_append(msg, &iter);
	encodeAgentMessage(&iter, message);
	
	//now send the message to the agent
	dbus_connection_send(theMTS.configuration->connection, msg, NULL);
	dbus_connection_flush(theMTS.configuration->connection);
}

/* handler for all requests made by agents to get the interaction layer to deliver a message
 * to the agents on the platform.  Performs all initial processing to determine the recipients
 * 
 * msg - the message that was sent over the transport bus to the interaction layer
 */
void MTS_handleMessage(DBusMessage* msg) {
	DBusMessageIter iter;
	dbus_message_iter_init(msg, &iter);
	
	AgentMessage* message = decodeAgentMessage(&iter);
	
	//output who the message was sent by
	g_message("MTS: message sent by %s", message->envelope->from->name->str);
	
	/*//now print out the message
	GString* temp = AgentMessageToString(message);
	g_message("Message read as \n%s", temp->str);
	g_string_free(temp, TRUE);*/
		
	//we need to deliver the message to all of the intended recipients
	int i;
	for (i=0; i<message->envelope->to->len; i++) {
		//set the intended receiver field
		AID* id = g_array_index(message->envelope->to, AID*, i);
		message->envelope->intendedReceiver = id;
		
		//now attempt to deliver the message
		deliverMessage(message);
	}	
}

/* Called by the underlying D-Bus stuff when a message is received that is meant
 * for the MTS object path.  This will be all of the ACLMessages and envelopes sent
 * by agents running on the platform.  No user data is passed into this function
 */
DBusHandlerResult MTSMessageHandler(DBusConnection* connection, DBusMessage *msg, void *userData) {
//check what the member is that is being called
	const char* method = dbus_message_get_member(msg);
	if (g_ascii_strcasecmp(MSG_PING, method) == 0) {
		//just output that we have received the message
		g_message("MTS: Ping message received from %s", dbus_message_get_sender(msg));
	}
	if (g_ascii_strcasecmp(MTS_MSG, method) == 0) {
		//just output that we have received the message
		g_message("MTS: Received route request from %s", dbus_message_get_sender(msg));
		MTS_handleMessage(msg);
	}
	else {
		g_message("MTS: Unknown method called (%s)", method);
	}	
	return DBUS_HANDLER_RESULT_HANDLED;
}

/* Called during bootstrap to set up the MTS service.  It sets up the listener function
 * that is called whenever a message is sent to the MTS service.  The platform 
 * structure must have been set before this function is called.
 * 
 * conn - the platforms connection to the D-Bus session bus
 * mainLoop - the GLib main loop that is being used by the platform
 * baseService - the base service for the platforms connection to the D-Bus
 */
void MTS_start(DBusConnection* conn, GMainLoop* mainLoop, gchar* baseService) {
	DBusError error;
	dbus_error_init(&error);	
	dbus_connection_ref(conn);

	//GString* tttt = g_string_new(baseService);
		
	//set up the configuration information for the MTS service	
	theMTS.configuration = g_new(AgentConfiguration, 1);
	AgentConfigurationInit(theMTS.configuration);
	theMTS.configuration->connection = conn;
	theMTS.configuration->mainLoop = mainLoop;	
	theMTS.configuration->baseService = g_string_new(baseService);
	
	//set up this services agent identifier
	AID* id = g_new(AID, 1);
	AIDInit(id);	
	GString* temp = g_string_new(MTS_NAME);
	g_string_sprintfa(temp, "@%s", thePlatform.name->str);
	AIDSetName(id, temp->str);
	GString address = buildTransportAddress(thePlatform.service->str, MTS_SERVICE_PATH, "msg");
	AIDAddAddress(id, address.str);
	theMTS.configuration->identifier = id;
	
	//set up this services description
	theMTS.description = g_new(PlatformServiceDescription, 1);
	theMTS.description->name = g_string_new(MTS_NAME);
	theMTS.description->address = g_string_new(address.str);
	
	//set up the listener path for MTS messages
	DBusObjectPathVTable vTable;
	vTable.unregister_function = unregFunction;
	vTable.message_function = MTSMessageHandler;
	if (!dbus_connection_register_object_path(theMTS.configuration->connection, MTS_SERVICE_PATH, 
		&vTable, NULL)) {
		g_error("Unable to register the handler for the MTS service");
		exit(1);		
	}
	else {
		g_message("MTS: Listening on %s", MTS_SERVICE_PATH);
	}
}

/* Called when the platform has been told to terminate.  It should just disconnect
 * its connection to the D-Bus
 */
void MTS_end() {
	//disconnect from the bus
	g_message("MTS Disconnecting from the bus");
	dbus_connection_unref(theMTS.configuration->connection);
}

