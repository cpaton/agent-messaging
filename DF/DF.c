/****************************************************************************************
 * Filename:	DF.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implementation DF support service for the platform. Includes all functionality for
 * the publicly accessible functions and maintenance of the registry that the DF is
 * responsible
 * **************************************************************************************/

#include "DF.h"
#include "../platform-defs.h"
#include "../util.h"
#include "../DBus/DBus-utils.h"
#include "../API/API.h"
#include "../Codec/codecs.h"
#include "../AMS/AMS.h"
#include <stdlib.h>

/* Function required by the D-Bus protocol but is not used in this apllication
 */
void DFUnregFunction(DBusConnection* conn, void* user_data) {
	g_message("DF Unregister function called");
}

/* handles search requests from agents.  The reply is sent within this function depending
 * on the results of the search
 * 
 * msg - the message that was sent from an agent to the serach listener that contains
 * 	the search criteria
 */
void DFHandleSearch(DBusMessage* msg) {
	DBusMessageIter iter;
	DBusMessage* reply;
	dbus_message_iter_init(msg, &iter);
	
	AgentDFDescription* template = decodeDFEntry(&iter);
	GString* gstr = AgentDFDescriptionToString(template);
	g_message("DF: template read as :");
	g_message("%s", gstr->str);
	g_string_free(gstr, TRUE);
	
	//perform the search
	APError error;
	APErrorInit(&error);
	GArray* matches = DF_search(template, &error);
	
	//send back the reply to the user	
	reply = dbus_message_new_method_return(msg);
	DBusMessageIter replyIter;
	dbus_message_iter_init_append(reply, &replyIter);
	encodeDFEntryArray(&replyIter, matches);
	
	//send the reply back
	dbus_connection_send(theAMS.configuration->connection, reply, NULL);
	dbus_connection_flush(theAMS.configuration->connection);	
}

/* handles modify requests from agents.  The reply is built appropraitely and sent within
 * this function
 * 
 * msg - the DBus message that contains the modification request
 */
void DFhandleModify(DBusMessage* msg) {
	DBusMessageIter iter;
	DBusMessage* reply;
	dbus_message_iter_init(msg, &iter);
	
	AgentDFDescription* entry = decodeDFEntry(&iter);
	GString* gstr = AgentDFDescriptionToString(entry);
	g_message("DF: DF entry read as :");
	g_message("%s", gstr->str);
	g_string_free(gstr, TRUE);
	
	//check to make sure that the entry exists
	GString* retVal;
	int index = DF_entryExists(entry->id->name);
	if (index == -1) {
		retVal = g_string_new(ERROR_ENTRY_NOT_FOUND);
	}
	else {
		//remove the old entry
		g_array_remove_index(theDF.agentDirectory, index);
		
		//now add the entry to the directory
		APError error;
		APErrorInit(&error);
		DF_registerEntry(entry, &error);
		if (APErrorIsSet(error)) {
			retVal = g_string_new(error.message->str);
			APErrorFree(&error);
		}
		else {
			retVal = g_string_new(RETURN_OK);
		}		
	}
	
	//build the reply to the message
	reply = dbus_message_new_method_return(msg);
	DBusMessageIter replyIter;
	dbus_message_iter_init_append(reply, &replyIter);
	encodeReply(&replyIter, retVal->str);
	
	//send the reply back
	dbus_connection_send(theAMS.configuration->connection, reply, NULL);
	dbus_connection_flush(theAMS.configuration->connection);	
}

/* handles a de-registration request from an agent, and sends an appropriate reply
 * 
 * msg - the message containing the de-registration request
 */
void DFhandleDeRegister(DBusMessage* msg) {
	DBusMessageIter iter;
	DBusMessage* reply;
	dbus_message_iter_init(msg, &iter);
	
	//get the name of the agent
	GString* name = decodeString(&iter);
	g_message("DF: de-registering %s", name->str);
	
	//check to make sure that the entry exists
	GString* retVal;
	int index = DF_entryExists(name);
	if (index == -1) {
		retVal = g_string_new(ERROR_ENTRY_NOT_FOUND);
	}
	else {
		g_array_remove_index(theDF.agentDirectory, index);
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
}

/* handles a register request from an agent and builds and sends an appropraite
 * reply and updates the DFs registry
 * 
 * msg - DBus message containing the register request
 */
void DFhandleRegister(DBusMessage* msg) {
	DBusMessageIter iter;
	DBusMessage* reply;
	dbus_message_iter_init(msg, &iter);
	
	AgentDFDescription* entry = decodeDFEntry(&iter);
	GString* gstr = AgentDFDescriptionToString(entry);
	g_message("DF: DF entry read as :");
	g_message("%s", gstr->str);
	g_string_free(gstr, TRUE);
	
	//now add the entry to the directory
	APError error;
	APErrorInit(&error);
	DF_registerEntry(entry, &error);
	GString* retVal;
	if (APErrorIsSet(error)) {
		retVal = g_string_new(error.message->str);
		APErrorFree(&error);
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
	
	g_string_free(retVal, TRUE);	
}

/* This function is called whenever a message is sent to the AMS service that is running
 * on the platform. No user data is passed into this function
 */
DBusHandlerResult DFMessageHandler(DBusConnection* connection, DBusMessage *msg, void *userData) {
	//check what the member is that is being called
	const char* method = dbus_message_get_member(msg);
	if (g_ascii_strcasecmp(MSG_PING, method) == 0) {
		//just output that we have received the message
		g_message("DF: Ping message received from %s", dbus_message_get_sender(msg));
	}
	else if (g_ascii_strcasecmp(MSG_PRINT_AGENT_DIRECTORY, method) == 0) {
		//just output that we have received the message
		g_message("DF: print directory request received from %s", dbus_message_get_sender(msg));
		DF_printDirectory();
	}
	else if (g_ascii_strcasecmp(MSG_DF_REGISTER, method) == 0) {
		//just output that we have received the message
		g_message("DF: register request received from %s", dbus_message_get_sender(msg));
		DFhandleRegister(msg);
	}
	else if (g_ascii_strcasecmp(MSG_DF_SEARCH, method) == 0) {
		//just output that we have received the message
		g_message("DF: search request received from %s", dbus_message_get_sender(msg));
		DFHandleSearch(msg);
	}
	else if (g_ascii_strcasecmp(MSG_DF_MODIFY, method) == 0) {
		//just output that we have received the message
		g_message("DF: modify request received from %s", dbus_message_get_sender(msg));
		DFhandleModify(msg);
	}
	else if (g_ascii_strcasecmp(MSG_DF_DEREGISTER, method) == 0) {
		//just output that we have received the message
		g_message("DF: de-register request received from %s", dbus_message_get_sender(msg));
		DFhandleDeRegister(msg);
	}	
	else {
		g_message("DF: Unknown method called (%s)", method);
	}	
	return DBUS_HANDLER_RESULT_HANDLED;
}

/* Called during bootstrapping of the platform.  It sets up the object path that is used
 * to receive all messages for the MTS service
 * 
 * conn - the platforms connection to the session DBus
 * mainLoop - the platforms GLib main loop
 * baseSerivce - the platforms base service on teh D-Bus session bus
 */
void DF_start(DBusConnection* conn, GMainLoop* mainLoop, gchar* baseService) {
	DBusError error;
	dbus_error_init(&error);

	//set up the AMS configuration
	theDF.configuration = g_new(AgentConfiguration, 1);
	AgentConfigurationInit(theDF.configuration);
	theDF.configuration->connection = conn;
	theDF.configuration->mainLoop = mainLoop;
	theDF.configuration->baseService = g_string_new(baseService);
	
	//set up this services agent identifier
	AID* id = g_new(AID, 1);
	AIDInit(id);	
	GString* temp = g_string_new(DF_NAME);
	g_string_sprintfa(temp, "@%s", thePlatform.name->str);
	AIDSetName(id, temp->str);
	GString address = buildTransportAddress(thePlatform.service->str, DF_SERVICE_PATH, "msg");
	AIDAddAddress(id, address.str);
	theDF.configuration->identifier = id;
	
	//set up this services description
	theDF.description = g_new(PlatformServiceDescription, 1);
	theDF.description->name = g_string_new(DF_NAME);
	theDF.description->address = g_string_new(address.str);
	
	//now set up a handler that will listen to all messages on a given path
	DBusObjectPathVTable vTable;
	vTable.unregister_function = DFUnregFunction;
	vTable.message_function = DFMessageHandler;
	if (!dbus_connection_register_object_path(theDF.configuration->connection, DF_SERVICE_PATH, 
		&vTable, NULL)) {
		g_error("Unable to register the handler for the AMS service");
		exit(1);		
	}
	else {
		g_message("DF: Listening on %s", DF_SERVICE_PATH);
	}	
	
	//initialise the agent directory
	theDF.agentDirectory = g_array_new(FALSE, FALSE, sizeof(AgentDFDescription*));
}

/* Called when the platform has been told to terminate.  Disconnects the AMS from
 * the D-Bus
 */
void DF_end() {
	g_message("DF: disconnecting from the DBus");
	dbus_connection_unref(theDF.configuration->connection);
	g_string_free(theDF.configuration->baseService, TRUE);
}

/* adds an entry to the DFs database
 * 
 * entry - the DF entry to be added to the databse
 * error - structure used to report all errors
 */
void DF_registerEntry(AgentDFDescription* entry, APError* error) {
	//check to make sure that the agent is registered with the AMS
	if (AMS_agentExists(entry->id->name) == -1) {
		APSetError(error, ERROR_UNAUTHORISED_AMS);
		return;
	}
	
	//check to see if this agent already has an entry
	if (DF_entryExists(entry->id->name) != -1) {
		APSetError(error, ERROR_DUPLICATE);
		return;
	}
	
	//add the entry to the agent directory
	g_array_append_val(theDF.agentDirectory, entry);
}

/* searhces the database to see if an entry exists for an agent with a given name
 * 
 * name - the name of the agent whose entry you are searching for
 * returns - the index into the array where the entry can be found, otherwise -1
 */
int DF_entryExists(GString* name) {
	int i;
	for (i=0; i<theDF.agentDirectory->len; i++) {
		AgentDFDescription* entry = g_array_index(theDF.agentDirectory, AgentDFDescription*, i);
		//check to see if the names match
		if (g_ascii_strcasecmp(entry->id->name->str, name->str) == 0)
			return i;
	}
	return -1;
}

/* used for testing and debuggin purposes - outputs the contents of the DF registry 
 * to the default log
 */
void DF_printDirectory() {	
	g_message("There are %d entries in the DF directory", theDF.agentDirectory->len);
	int i;
	for (i=0; i<theDF.agentDirectory->len; i++) {
		AgentDFDescription* entry = g_array_index(theDF.agentDirectory, AgentDFDescription*, i);
		GString* temp = AgentDFDescriptionToString(entry);
		g_message("%s", temp->str);
		g_string_free(temp, TRUE);
	}
}

/* used to form part of the search semantics and compares two strings to see if they
 * match
 * 
 * entry - the string of the existing entry in the database that you are trying to test
 * 	for a match
 * template - the string that you are searching for
 * partialMatch - set it to TRUE if you accept a partial string match as a match
 * returns - TRUE if the strings match, FALSE otherwise
 */
gboolean matchString(GString* entry, GString* template, gboolean partialMatch) {
	//if there is no template then there is a match
	if (template == NULL) return TRUE;
	
	//if there is a template but no entry then there is no match
	if (entry == NULL) return FALSE;
	
	//now compare the strings
	if (g_ascii_strcasecmp(entry->str, template->str) == 0)
		return TRUE;
	else if (partialMatch) {
		//check to see if the start of the strings match
		if (g_ascii_strncasecmp(entry->str, template->str, template->len) == 0)
			return TRUE;		
		else
			return FALSE;
	}
	else 
		return FALSE;
}

/* used to form part of the search semantics for the DF, checks to if an array
 * of strings matches a given value
 * 
 * entry - the array of strings in which you are searching for a partial match
 * template - the array of strings for which you are trying to match
 * returns - TRUE if every string in the template has a matching element in the entry,
 * 	FALSE otherwise
 */
gboolean matchStringArray(GArray* entry, GArray* template) {
	//if the template is empty then we have a match
	if (template->len == 0) return TRUE;
	
	//the database must have at least as many entries as the template
	if (entry->len < template->len) return FALSE;
	
	//now check to make sure that each template entry is in the database
	int i;
	for (i=0; i<template->len; i++) {
		GString* lookFor = g_array_index(template, GString*, i);
		int j;
		gboolean found = FALSE;
		for (j=0; j<entry->len; j++) {
			GString* check = g_array_index(entry, GString*, j);
			if (matchString(check, lookFor, FALSE)) {
				found = TRUE;
			}
		}
		//check to make sure that the entry was found
		if (!found) return FALSE;
	}
	
	return TRUE;
}

/* used to perform part of the DF search semantics.  determines whether two agent
 * identifiers match.
 * 
 * entry - the AID from the database in which you are looking for a match
 * template - the AID that you are attempting to match to
 * returns - TRUE if the template matches the entry, FALSE otherwise
 */
gboolean matchAID(AID* entry, AID* template) {
	//if there is no template then there is a match
	if (template == NULL) return TRUE;
	
	//if there is a template but no entry then there is no match
	if (entry == NULL) return FALSE;
	
	//now check to see if they match
	if (matchString(entry->name, template->name, TRUE) && matchStringArray(entry->addresses, template->addresses))
		return TRUE;
	else
		return FALSE;
}

/* forms part of the search semantics for the DF, checks to see a service entry from
 * the database matches a given search
 * 
 * entry - the service entry currently in the database
 * template - the service entry that you are trying to find a match for
 * returns - TRUE if the template matches the database entry, FALSE otherwise
 */
gboolean matchService(DFServiceDescription* entry, DFServiceDescription* template) {
	//if there is no template then there is a match
	if (template == NULL) return TRUE;
	
	//if there is a template but no entry then there is no match
	if (entry == NULL) return FALSE;
	
	//now check to see if they match
	if (matchString(entry->name, template->name, FALSE) && matchString(entry->type, template->type, FALSE) && matchStringArray(entry->protocols, template->protocols) && matchStringArray(entry->ontologies, template->ontologies) && matchStringArray(entry->languages, template->languages))
		return TRUE;
	else
		return FALSE;
}

/* used to form part of the search semantics for the DF, checks to see if an array of
 * service entries matches an array of entries in the database
 * 
 * entry - the service array found in the database
 * template - the service array for which you are looking for a match
 * returns - TRUE if every service in the template has a match in the database entry,
 * 	FALSE otherwise
 */
gboolean matchServices(GArray* entry, GArray* template) {
//if the template is empty then we have a match
	if (template->len == 0) return TRUE;
	
	//the database must have at least as many entries as the template
	if (entry->len < template->len) return FALSE;
	
	//now check to make sure that each template entry is in the database
	int i;
	for (i=0; i<template->len; i++) {
		DFServiceDescription* lookFor = g_array_index(template, DFServiceDescription*, i);
		int j;
		gboolean found = FALSE;
		for (j=0; j<entry->len; j++) {
			DFServiceDescription* check = g_array_index(entry, DFServiceDescription*, j);
			if (matchService(check, lookFor)) {
				found = TRUE;
			}
		}
		//check to make sure that the entry was found
		if (!found) return FALSE;
	}
	
	return TRUE;
}

/* used to form part of the DF search semantics for the DF, checks to see if an entry
 * in the database matches the search criteria
 * 
 * entry - the entry in the database
 * template - the search criteria
 * returns - TRUE if every specified element in the template has a matching element
 * 	in the database entry, FALSE otherwise
 */
gboolean matches(AgentDFDescription* entry, AgentDFDescription* template) {
	if (matchAID(entry->id, template->id) && matchServices(entry->services, template->services) && matchStringArray(entry->protocols, template->protocols) && matchStringArray(entry->ontologies, template->ontologies) && matchStringArray(entry->languages, template->languages))
		return TRUE;
	else
		return FALSE;
}

/* searhces the DF registry for a given entry that matches the template
 * 
 * template - the search criteria
 * error - structure used to report any errors
 * return - array of entries in the database that met the criteria
 */
GArray* DF_search(AgentDFDescription* template, APError* error) {
	GArray* results = g_array_new(FALSE, FALSE, sizeof(AgentDFDescription*));
	
	//loop over all entries
	int i;
	for (i=0; i<theDF.agentDirectory->len; i++) {
		AgentDFDescription* entry = g_array_index(theDF.agentDirectory, AgentDFDescription*, i);
		if (matches(entry, template))
			g_array_append_val(results, entry);
	}
	
	return results;
}
