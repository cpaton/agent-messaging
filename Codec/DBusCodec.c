/****************************************************************************************
 * Filename:	DBusCodec.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implementation of the DBusCodec that is used for all transport of information
 * internal to the platform and for agent communication.  All functions come in pairs
 * one for encoding and a partner that should be used for decoding it.
 * **************************************************************************************/

#include "DBusCodec.h"
#include "../API/API.h"
#include <string.h>

/************** UTIL FUNCTIONS ****************************************/
/* function that checks to make sure that the current item in a DBus message has the
 * correct type.  It only looks at the argument, it does not move the iterator
 * 
 * iter - an iterator for the dbus message contents
 * type - the type that you expect the next item to be (see the DBus documentation
 * 	for the numbers associated with given types)
 * returns - TRUE if the next item is of the specified type, FALSE otherwise
 */
gboolean checkType(DBusMessageIter* iter, int type) {
	if (dbus_message_iter_get_arg_type(iter) != type)
		return FALSE;
	else
		return TRUE;
}

/********** STANDARD FUNCTIONS **********************************/
/* adds a given parameter to the end of a message, with the name of the parameter
 * first followed by its value
 * 
 * iter - the iterator for the message to which the value should be appended
 * parameter - the name of the parameter to be added
 * value - the value of the parameter
 */
void encodeParameter(DBusMessageIter* iter, GString* parameter, GString* value) {
	dbus_message_iter_append_string(iter, parameter->str);
	if (value == NULL) {
		dbus_message_iter_append_string(iter, "");
	}
	else {
		dbus_message_iter_append_string(iter, value->str);
	}
}

/* retrieves the value of a named parameter from a message. It does not move the
 * iterator past the end of the parameter
 * 
 * iter - iterator for the message
 * parameter - will contain the name of the parameter after it has been decoded
 * value - the value of the parameter that has been decoded
 */
void decodeParameter(DBusMessageIter* iter, GString** parameter, GString** value) {
	char* read;
	read = dbus_message_iter_get_string(iter);
	(*parameter) = g_string_new(read);
	dbus_message_iter_next(iter);
	read = dbus_message_iter_get_string(iter);	
	if (g_strcasecmp(read, "") == 0) {
		(*value) = NULL;
	}
	else {
		(*value) = g_string_new(read);
	}
}

/* adds a string to the end of a messge
 * 
 * iter - the iterator for the message
 * str - the string to be appended
 */
void encodeString(DBusMessageIter* iter, GString* str) {
	if (str == NULL)
		dbus_message_iter_append_string(iter, "");
	else
		dbus_message_iter_append_string(iter, str->str);
}

/* reads a string value from a message, it does not move the iterator onto the next
 * element in the message
 * 
 * iter - the iterator for the message
 * returns - the string read in, if the next item wasn't a string then it returns NULL
 */
GString* decodeString(DBusMessageIter* iter) {
	char* value = dbus_message_iter_get_string(iter);
	GString* gstr;
	if (g_strcasecmp("", value) == 0)
		return NULL;
	else
		gstr = g_string_new(value);
	return gstr;
}


/* adds an array of strings to a message
 * 
 * iter - the iterator for the message
 * array - the strings that should be added to the message, the array must be of GString*
 */
void encodeStringArray(DBusMessageIter* iter, GArray* array) {
	DBusMessageIter arrayIter;
	dbus_message_iter_append_array(iter, &arrayIter, DBUS_TYPE_STRING);
	
	//int retVal = dbus_message_iter_append_uint32(&arrayIter, array->len);
	
	//loop over all items in the array and put them out
	//char** strArray;
	//strArray = g_new(char*, array->len);	
	int i;
	for (i=0; i < array->len; i++) {
		GString* str = g_array_index(array, GString*, i);
		dbus_message_iter_append_string(&arrayIter, str->str);
		//strArray[i] = g_new(char, (str->len)+1);
		//strArray[i] = strcpy(str->str, strArray[i]);		
	}
}

/* gets an array of strings from a message.  When it has finished the iterator 
 * will be pointing to the next element in the message
 * 
 * iter - the iterator for the message
 * returns - an array of strings that has been read off
 */
GArray* decodeStringArray(DBusMessageIter* iter) {
	if (!checkType(iter, DBUS_TYPE_ARRAY)) return NULL;
	
	GArray* array = g_array_new(FALSE, FALSE, sizeof(GString*));
	DBusMessageIter arrayIter;
	int type;
	dbus_message_iter_init_array_iterator(iter, &arrayIter, &type);
	if (type != DBUS_TYPE_STRING) return NULL;	
	
	if (checkType(&arrayIter, DBUS_TYPE_STRING)) {
		//loop over all of the array
		gboolean cont = TRUE;
		while (cont) {
			if (!dbus_message_iter_has_next(&arrayIter)) cont = FALSE;
			char* value = dbus_message_iter_get_string(&arrayIter);
			
			GString* str = g_string_new(value);
			g_array_append_val(array, str);
			dbus_message_iter_next(&arrayIter);		
		}
	}
		
	//move the iterator past the array
	dbus_message_iter_next(iter);

	return array;
}

/*************** ENCODES AND DECODES *********************************/

/********* PLATFORM DESCRIPTION *************************/
/* adds the platform description structure to the end of a message
 * 
 * iter - the iterator for the message
 * desc - the platform description structure to be encoded
 */
void encodePlatformDescription(DBusMessageIter* iter, PlatformDescription* desc) {
	dbus_message_iter_append_string(iter, desc->name->str);
	
	//now encode each of the platform services
	int i;
	for (i=0; i < desc->services->len; i++) {
		PlatformServiceDescription* service = g_array_index(desc->services, PlatformServiceDescription*, i);
		encodeParameter(iter, service->name, service->address);
	}
}

/* reads off a platform description from a message
 * 
 * iter - the iterator for the message
 * returns - a platform description structure for the values read
 */
PlatformDescription* decodePlatformDescription(DBusMessageIter* iter) {
	PlatformDescription* platform = g_new(PlatformDescription, 1);
	InitPlatformDescription(platform);
	
	//read off the platform name
	const char* name;
	if (!checkType(iter, DBUS_TYPE_STRING)) return NULL;
	name = dbus_message_iter_get_string(iter);
	platform->name = g_string_new(name);
	
	//read of the entries for the platform service descriptions		
	while (dbus_message_iter_has_next(iter)) {
		dbus_message_iter_next(iter);				
		PlatformServiceDescription* temp = g_new(PlatformServiceDescription, 1);
		decodeParameter(iter, &(temp->name), &(temp->address));				
		g_array_append_val(platform->services, temp);
	}		
	
	return platform;
}

/****************** AID *************************/
/* adds an agent identifier to a message
 * 
 * iter - the iterator for the message
 * aid - the agent identifier that should be added
 */
void encodeAID(DBusMessageIter* iter, AID* aid) {
	if (aid == NULL) {
		encodeString(iter, NULL);
		GArray* array = g_array_new(FALSE, FALSE, sizeof(GString*));
		encodeStringArray(iter, array);
	}
	else {
		encodeString(iter, aid->name);
		//dbus_message_iter_append_string(iter, aid->name->str);
		encodeStringArray(iter, aid->addresses);
	}
}

/* reads off an AID from a message.  When complete the iterator will be pointing to the
 * next item in the message
 * 
 * iter - the iterator for the message
 * returns - the agent identifier read, if an error occurs it returns NULL
 */
AID* decodeAID(DBusMessageIter* iter) {
	AID* id = g_new(AID, 1);
	AIDInit(id);	
	
	//get the name of the agent
	if (!checkType(iter, DBUS_TYPE_STRING)) return NULL;
	//char* name = dbus_message_iter_get_string(iter);
	//id->name = g_string_new(name);
	id->name = decodeString(iter);
	
	//now deal with the addresses
	dbus_message_iter_next(iter);
	GArray* array = decodeStringArray(iter);
	if (array == NULL) return NULL;
	id->addresses = array;
	
	return id;	
}

/************ REPLIES *******************************/
/* creates the contents of a simple reply message that only has a single string
 * denoting success or not
 * 
 * iter - the iterator for the message
 * message - the string that should be added to the message
 */
void encodeReply(DBusMessageIter* iter, char* message) {
	dbus_message_iter_append_string(iter, message);
}

/* gets the message contained in a simple reply from one of the platform services
 * 
 * iter - the iterator for the message
 * returns - the string read from the reply
 */
GString* decodeReply(DBusMessageIter* iter) {
	GString* str;
	char* reply = dbus_message_iter_get_string(iter);
	str = g_string_new(reply);
	return str;
	
}

/******************* AID ARRAYS *************************************/
/* adds an array of AIDs to a message using the encoding mechanism for arrays
 * of complex types that has been built on top of the DBus sending mechanism
 * 
 * iter - the iterator for the message
 * array - the agent identifiers that are to be added
 */
void encodeAIDArray(DBusMessageIter* iter, GArray* array) {
	//first of all output the length
	dbus_message_iter_append_int32(iter, array->len);
	
	//now output all of the AID's
	int i;
	for (i=0; i<array->len; i++) {
		AID* id = g_array_index(array, AID*, i);
		encodeAID(iter, id);
	}
}

/* reads off an array of agent identifiers from a message, using the protocol for the
 * sending of complex arrays that was built on top of the DBus type system.  Once 
 * complete the iterator is pointing to the element after the array.
 * 
 * iter - the iterator for the message
 * returns - Array of agent identifiers read off from the message
 */
GArray* decodeAIDArray(DBusMessageIter* iter) {
	GArray* array = g_array_new(FALSE, FALSE, sizeof(AID*));
	
	//read in the number of entries
	int number = dbus_message_iter_get_int32(iter);
	dbus_message_iter_next(iter);
	
	//now read in each of the AID's
	int i;
	for (i=0; i<number; i++) {
		AID* id = decodeAID(iter);
		g_array_append_val(array, id);
	}
	
	return array;
}

/* adds an array of DF service entries for an agent to a message using the mechanism
 * for sending arrays of complex types
 * 
 * iter - the iterator for the message
 * array - the array of DF service entries to be added
 */
void encodeDFServiceArray(DBusMessageIter* iter, GArray* array) {
	//first of all output the number of entries
	dbus_message_iter_append_int32(iter, array->len);
	
	//then output each service in turn
	int i;
	for (i=0; i<array->len; i++) {
		DFServiceDescription* service = g_array_index(array, DFServiceDescription*, i);
		encodeDFService(iter, service);
	}
}

/* reads off an array of DF service entries for an agent from a message expecting it
 * to be in the format for complex arrays.  Once complete the iterator points to the 
 * next item in the message
 * 
 * iter - the iterator for the message
 * returns - array of DF service entries read from the message
 */
GArray* decodeDFServiceArray(DBusMessageIter* iter) {
	GArray* results = g_array_new(FALSE, FALSE, sizeof(DFServiceDescription*));
	
	//read of the number of service entries to read
	int number = dbus_message_iter_get_int32(iter);
	dbus_message_iter_next(iter);
	
	//read in each servce entry
	int i;
	for(i=0; i<number; i++) {
		DFServiceDescription* service = decodeDFService(iter);
		g_array_append_val(results, service);
	}
	
	return results;
}


/**************** DF SERVICE DESCRIPTIONS **************************/
/* adds a single DF service entry for an agent to the message
 * 
 * iter - the iterator for the message
 * service - the service information to be added to the message
 */
void encodeDFService(DBusMessageIter* iter, DFServiceDescription* service) {
	//add the name of the service
	encodeString(iter, service->name);
	
	//add the type of the service
	encodeString(iter, service->type);
	
	//add the protocols
	encodeStringArray(iter, service->protocols);
	
	//add the ontologies
	encodeStringArray(iter, service->ontologies);
	
	//add the languages
	encodeStringArray(iter, service->languages);
}

/* reads of a DF service description from a message.  Once complete the iterator
 * points to the next item in the message
 * 
 * iter - the iterator for the message
 * returns - the DF service description read from the message
 */
DFServiceDescription* decodeDFService(DBusMessageIter* iter) {
	DFServiceDescription* value = g_new(DFServiceDescription, 1);
	DFServiceDescriptionInit(value);
	
	//read off the name of the service
	value->name = decodeString(iter);
	dbus_message_iter_next(iter);
	
	//read off the type of the service
	value->type = decodeString(iter);
	dbus_message_iter_next(iter);
	
	//read off the protocols
	value->protocols = decodeStringArray(iter);
	
	//read off the ontologies
	value->ontologies = decodeStringArray(iter);
	
	//read off the languages
	value->languages = decodeStringArray(iter);
	
	return value;
}

/*************** DF ENTRIES *************************************/
/* adds an entire DF entry to a message
 * 
 * iter - the iterator for the message
 * entry - the DF entry that should be added to the message
 */
void encodeDFEntry(DBusMessageIter* iter, AgentDFDescription* entry) {
	//first of all encode the AID
	encodeAID(iter, entry->id);
	
	//encode the protocols
	encodeStringArray(iter, entry->protocols);
	
	//encode the ontologies
	encodeStringArray(iter, entry->ontologies);
	
	//encode the languages
	encodeStringArray(iter, entry->languages);	
	
	//encode the services
	encodeDFServiceArray(iter, entry->services);
}

/* reads of an entire DF entry from a message.  Once complete the iterator
 * points to the next item in the message
 * 
 * iter - the iterator for the message
 * returns - the DF description read from the message
 */
AgentDFDescription* decodeDFEntry(DBusMessageIter* iter) {
	AgentDFDescription* entry = g_new(AgentDFDescription, 1);
	AgentDFDescriptionInit(entry);
	
	//first of all read off the agent identifier
	entry->id = decodeAID(iter);
	
	//read off the protocols
	entry->protocols = decodeStringArray(iter);
	
	//read off the ontologies
	entry->ontologies = decodeStringArray(iter);
	
	//read off the languages
	entry->languages = decodeStringArray(iter);
	
	//read off the services
	entry->services = decodeDFServiceArray(iter);	
	
	return entry;
}

/************* DF ENTRY ARRAYS  ******************************/
/* adds an array of DF entries to a message using the complex array typing mechanism
 * 
 * iter - the iterator for the message
 * array - the DF entries to be added
 */
void encodeDFEntryArray(DBusMessageIter* iter, GArray* array) {
	//output the number of entries
	dbus_message_iter_append_int32(iter, array->len);
	
	//now output each entry
	int i;
	for (i=0; i<array->len; i++) {
		AgentDFDescription* entry = g_array_index(array, AgentDFDescription*, i);
		encodeDFEntry(iter, entry);
	}
}

/* reads off an array of DF entries from a message expecting them to be in the
 * encoding mechanism for complex arrays. Once complete the iterator points
 * to the next item in the message
 * 
 * iter - the iterator for the message
 * returns - array of DF entries read from the message
 */
GArray* decodeDFEntryArray(DBusMessageIter* iter) {
	GArray* array = g_array_new(FALSE, FALSE, sizeof(AgentDFDescription*));
	
	//read in the number of entries
	int number = dbus_message_iter_get_int32(iter);
	dbus_message_iter_next(iter);
	
	//now read in each of the AID's
	int i;
	for (i=0; i<number; i++) {
		AgentDFDescription* entry = decodeDFEntry(iter);
		g_array_append_val(array, entry);
	}
	
	return array;
}

/* adds an envelope to the end of a message
 * 
 * iter - the iterator for the message
 * envelope - the envelop to be added
 */
void encodeACLEnvelope(DBusMessageIter* iter, ACLEnvelope* envelope) {
	//add the from field
	encodeAID(iter, envelope->from);
	
	//add the to fields
	encodeAIDArray(iter, envelope->to);
	
	//add the acl representation
	encodeString(iter, envelope->aclRepresentation);
	
	//add the intended receiver
	encodeAID(iter, envelope->intendedReceiver);
}

/* reads off an envelope from a message. Once complete the iterator points to the next
 * item in the message
 * 
 * iter - the iterator for the message
 * returns - the envelope read
 */
ACLEnvelope* decodeEnvelope(DBusMessageIter* iter) {
	ACLEnvelope* envelope = g_new(ACLEnvelope, 1);
	ACLEnvelopeInit(envelope);
	
	//get the from field
	envelope->from = decodeAID(iter);
	
	//get the to fields
	envelope->to = decodeAIDArray(iter);
	
	//get the acl representation
	envelope->aclRepresentation = decodeString(iter);
	dbus_message_iter_next(iter);
	
	//get the intended receiver
	envelope->intendedReceiver = decodeAID(iter);
	//check to see if there is no intended receiver
	if (envelope->intendedReceiver->name == NULL && envelope->intendedReceiver->addresses->len ==0)
		envelope->intendedReceiver = NULL;
	
	return envelope;
}

/* adds a FIPA-ACL message to a message
 * 
 * iter - the iterator for the message
 * msg - the message to be added
 */
void encodeACLMessage(DBusMessageIter* iter, ACLMessage* msg) {
	//encode the performative
	encodeString(iter, msg->performative);
	
	//encode the sender
	encodeAID(iter, msg->sender);
	
	//encode the receivers
	encodeAIDArray(iter, msg->receivers);
	
	//encode the language, ontology and protocols
	encodeString(iter, msg->language);
	encodeString(iter, msg->ontology);
	encodeString(iter, msg->protocol);
	
	//encode the conversation information
	encodeString(iter, msg->conversationID);
	encodeString(iter, msg->replyWith);
	encodeString(iter, msg->inReplyTo);
	encodeString(iter, msg->replyBy);
	
	//encode the content
	encodeString(iter, msg->content);
}

/* reads off a FIPA-ACL message from a message.  Once complete the iterator points
 * to the next item in the message
 * 
 * iter - the iterator for the message
 * returns - the message read off
 */
ACLMessage* decodeACLMessage(DBusMessageIter* iter) {
	ACLMessage* msg = g_new(ACLMessage, 1);
	ACLMessageInit(msg);
	
	//decode the performative
	msg->performative = decodeString(iter);
	dbus_message_iter_next(iter);
	
	//get the sender
	msg->sender = decodeAID(iter);
	if (msg->sender->name == NULL && msg->sender->addresses->len ==0)
		msg->sender = NULL;
		
	//get the receivers
	msg->receivers = decodeAIDArray(iter);
	
	//get the description of the content
	msg->language = decodeString(iter);
	dbus_message_iter_next(iter);
	msg->ontology = decodeString(iter);
	dbus_message_iter_next(iter);
	msg->protocol = decodeString(iter);
	dbus_message_iter_next(iter);
	
	//get the conversation items
	msg->conversationID = decodeString(iter);
	dbus_message_iter_next(iter);
	msg->replyWith = decodeString(iter);
	dbus_message_iter_next(iter);
	msg->inReplyTo = decodeString(iter);
	dbus_message_iter_next(iter);
	msg->replyBy = decodeString(iter);
	dbus_message_iter_next(iter);
	
	//get the content of the message
	msg->content = decodeString(iter);
	dbus_message_iter_next(iter);
	
	return msg;
}

/* adds an entire agent message to a DBus message
 * 
 * iter - the iterator for the message
 * msg - the agent message to be added
 */
void encodeAgentMessage(DBusMessageIter* iter, AgentMessage* msg) {
	//enocde the envelope
	encodeACLEnvelope(iter, msg->envelope);
	
	//encode the payload
	encodeACLMessage(iter, msg->payload);	
}

/* reads off an agent message from a message. Once complete the iterator points
 * to the next item in the message
 * 
 * iter - the iterator for the message
 * returns - the agent message read
 */
AgentMessage* decodeAgentMessage(DBusMessageIter* iter) {
	AgentMessage* message = g_new(AgentMessage, 1);
	AgentMessageInit(message);
	
	//decode the envelope
	message->envelope = decodeEnvelope(iter);
	
	//decode the payload
	message->payload = decodeACLMessage(iter);
	
	return message;
}
