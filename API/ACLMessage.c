/****************************************************************************************
 * Filename:	ACLMessage.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implementation of the functions required to manipulate a FIPA-ACL mesage
 * used by agents to build the messages that they wish to send on the platform
 * **************************************************************************************/

#include "ACLMessage.h"
#include "API.h"

/********************* SETTER METHODS ******************************/
void ACLMessageSetPerformative(ACLMessage* msg, char* performative) {	
	msg->performative = g_string_new(performative);	
}

void ACLMessageSetSender(ACLMessage* msg, AID* sender) {
	msg->sender = sender;
}

void ACLMessageAddReceiver(ACLMessage* msg, AID* receiver) {
	g_array_append_val(msg->receivers, receiver);
}

void ACLMessageAddReplyTo(ACLMessage* msg, AID* replyTo) {	
	g_array_append_val(msg->replyTo, replyTo);	
}

void ACLMessageSetLanguage(ACLMessage* msg, char* language) {
	msg->language = g_string_new(language);
}

void ACLMessageSetEncoding(ACLMessage* msg, char* encoding) {
	msg->encoding = g_string_new(encoding);
}

void ACLMessageSetOntology(ACLMessage* msg, char* ontology) {
	msg->ontology = g_string_new(ontology);
}

void ACLMessageSetProtocol(ACLMessage* msg, char* protocol) {
	msg->protocol = g_string_new(protocol);
}

void ACLMessageSetConversationID(ACLMessage* msg, char* conversationID) {
	msg->conversationID = g_string_new(conversationID);
}

void ACLMessageSetReplyWith(ACLMessage* msg, char* replyWith) {
	msg->replyWith = g_string_new(replyWith);
}

void ACLMessageSetInReplyTo(ACLMessage* msg, char* inReplyTo) {
	msg->inReplyTo = g_string_new(inReplyTo);
}

void ACLMessageSetReplyBy(ACLMessage* msg, char* replyBy) {
	msg->replyBy = g_string_new(replyBy);
}

void ACLMessageSetContent(ACLMessage* msg, char* content) {
	msg->content = g_string_new(content);
}

/***************** GETTER METHODS *********************************/
GString* ACLMessageGetPerformative(ACLMessage* msg) { return msg->performative; }
AID* ACLMessageGetSender(ACLMessage* msg) { return msg->sender; }
GArray* ACLMessageGetReceivers(ACLMessage* msg) { return msg->receivers; }
GArray* ACLMessageGetReplyTo(ACLMessage* msg) { return msg->replyTo; }
GString* ACLMessageGetLanguage(ACLMessage* msg) { return msg->language; }
GString* ACLMessageGetEncoding(ACLMessage* msg) { return msg->encoding; }
GString* ACLMessageGetOntology(ACLMessage* msg) { return msg->ontology; }
GString* ACLMessageGetProtocol(ACLMessage* msg) { return msg->protocol; }
GString* ACLMessageGetConversationID(ACLMessage* msg) { return msg->conversationID; }
GString* ACLMessageGetReplyWith(ACLMessage* msg) { return msg->replyWith; }
GString* ACLMessageGetInReplyTo(ACLMessage* msg) { return msg->inReplyTo; }
GString* ACLMessageGetReplyBy(ACLMessage* msg) { return msg->replyBy; }
GString* ACLMessageGetContent(ACLMessage* msg) { return msg->content; }

/************* UTILITY FUNCTIONS **********************************/

/* Converts a FIPA-ACL message into a LISP like string.  Only designed to be used
 * for debugging and testing to print a structure to the screen or log, it does not have 
 * a strict enough structure to be used for transportation using some string codec
 * 
 * msg - the message that you want converted to a string
 * returns - a GString object that contains the string representation of the message
 */
GString* ACLMessageToString(ACLMessage* msg) {
	GString* buffer = g_string_new("(");
	
	//performative
	if (msg->performative != NULL) g_string_sprintfa(buffer, "%s", msg->performative->str);
	g_string_sprintfa(buffer, "\n");
	
	//sender
	if (msg->sender != NULL) {
		GString* temp = AIDToString((*msg->sender));
		g_string_sprintfa(buffer, "\t:sender %s\n", temp->str);		
	}
	
	//receivers
	if (msg->receivers->len != 0) {
		g_string_sprintfa(buffer, "\t:receiver (set ");
		int i;
		for (i=0; i < msg->receivers->len; i++) {
			AID* temp = g_array_index(msg->receivers, AID*, i);
			GString* tempStr = AIDToString(*temp);
			g_string_sprintfa(buffer, "%s ", tempStr->str);
		}
		g_string_sprintfa(buffer, ")\n");		
	}
	
	//replyTo
	if (msg->replyTo->len != 0) {
		g_string_sprintfa(buffer, "\t:reply-to (set ");
		int i;
		for (i=0; i < msg->replyTo->len; i++) {
			AID* temp = g_array_index(msg->replyTo, AID*, i);
			GString* tempStr = AIDToString(*temp);
			g_string_sprintfa(buffer, "%s ", tempStr->str);
		}
		g_string_sprintfa(buffer, ")\n");		
	}	
	
	//content
	if (msg->content != NULL) g_string_sprintfa(buffer, "\t:content %s\n", msg->content->str);
	
	//language
	if (msg->language != NULL) g_string_sprintfa(buffer, "\t:language %s\n", msg->language->str);
	
	//encoding
	if (msg->encoding != NULL) g_string_sprintfa(buffer, "\t:encoding %s\n", msg->encoding->str);
	
	//ontology
	if (msg->ontology != NULL) g_string_sprintfa(buffer, "\t:ontology %s\n", msg->ontology->str);
	
	//protocol
	if (msg->protocol != NULL) g_string_sprintfa(buffer, "\t:protocol %s\n", msg->protocol->str);
	
	//conversationID
	if (msg->conversationID != NULL) g_string_sprintfa(buffer, "\t:conversation-id %s\n", msg->conversationID->str);
	
	//replyWith
	if (msg->replyWith != NULL) g_string_sprintfa(buffer, "\t:reply-with %s\n", msg->replyWith->str);
	
	//inReplyTo
	if (msg->inReplyTo != NULL) g_string_sprintfa(buffer, "\t:in-reply-to %s\n", msg->inReplyTo->str);
	
	//replyBy
	if (msg->replyBy != NULL) g_string_sprintfa(buffer, "\t:reply-by %s\n", msg->replyBy->str);
	
	g_string_sprintfa(buffer, ")");	
	return buffer;
}

/* Creates and initialises a new ACL message with the given performative
 * 
 * performative - the performative that you wish the message to have. Constants have
 * 	been set up for all of the performatives given in FIPA0037
 * returns - a newly allocated ACL message structure thats needs to be freed when
 * 	finished with
 */
ACLMessage* ACLMessageNew(char* performative) {
	ACLMessage** msg = g_new(ACLMessage*, 1);
	(*msg) = g_new(ACLMessage, 1);
	ACLMessageInit(*msg);
	ACLMessageSetPerformative(*msg, performative);
	return *msg;
}

/* Creates a reply to a given ACL message.  It sets the receiver, reply-to, converation
 * id, protocol, lanaguage and ontology fields to the appropriate values, this function 
 * should be used if you wish to reply to a given message instead of building a new 
 * one from scratch.
 * 
 * msg - the message that you wish to reply to
 * returns - a newly allocated ACL message that contains the reply message, this should
 * be freed when finshed with
 */
ACLMessage* ACLMessageCreateReply(ACLMessage* msg) {
	ACLMessage* reply = g_new(ACLMessage, 1);
	ACLMessageInit(reply);
	
	//set the receiver to be the sender
	if (msg->replyTo->len >0) {
		int i;
		for (i=0; i<msg->replyTo->len; i++) {
			AID* id = g_array_index(msg->replyTo, AID*, i);			
			ACLMessageAddReceiver(reply, id);	
		}
	}		
	else 
		ACLMessageAddReceiver(reply, msg->sender);
	
	//set the language to be the saem
	if (msg->language != NULL) ACLMessageSetLanguage(reply, msg->language->str);
	
	//set the protocol to be the same
	if (msg->protocol != NULL) ACLMessageSetProtocol(reply, msg->protocol->str);
	
	//set the ontology to be the same
	if (msg->ontology != NULL) ACLMessageSetOntology(reply, msg->ontology->str);
	
	//set the conversation id to be the same
	if (msg->conversationID != NULL) ACLMessageSetConversationID(reply, msg->conversationID->str);
	
	//set the in reply to
	if (msg->replyWith != NULL) ACLMessageSetInReplyTo(reply, msg->replyWith->str);

	return reply;
}

/* Converts an entire agent message to a LISP like string representation.  An agent
 * message consists of both an envelope and a FIPA-ACL message. 
 * 
 * message - the agent message that you want converted to a string
 * returns - string representation of the message
 */
GString* AgentMessageToString(AgentMessage* message) {
	GString* gstr = g_string_new("");
	GString* temp;
	
	if (message->envelope != NULL) {
		temp = ACLEnvelopeToString(message->envelope);
		g_string_sprintfa(gstr, "%s\n", temp->str);
		g_string_free(temp, TRUE);
	}
	
	if (message->payload != NULL) {
		temp = ACLMessageToString(message->payload);
		g_string_sprintfa(gstr, "%s\n", temp->str);
		g_string_free(temp, TRUE);
	}
	
	return gstr;
}

/************** SWIG IMPLEMENTATION ************************************/
/*SWIG implementations of the setter methods that remove the call by reference
 * Ideally the above functions would be changed although this was not possible
 * witin the time constraints of the project as the new code could have been thoroughly
 * tested.
 */
ACLMessage* SACLMessageSetPerformative(ACLMessage* msg, char* performative) {
	ACLMessageSetPerformative(msg, performative);
	return msg;
}

ACLMessage* SACLMessageSetSender(ACLMessage* msg, AID* sender) {
	ACLMessageSetSender(msg, sender);
	return msg;
}

ACLMessage* SACLMessageAddReceiver(ACLMessage* msg, AID* receiver) {
	ACLMessageAddReceiver(msg, receiver);
	return msg;
}

ACLMessage* SACLMessageAddReplyTo(ACLMessage* msg, AID* replyTo) {
	ACLMessageAddReplyTo(msg, replyTo);
	return msg;
}

ACLMessage* SACLMessageSetLanguage(ACLMessage* msg, char* language) {
	ACLMessageSetLanguage(msg, language);
	return msg;
}

ACLMessage* SACLMessageSetEncoding(ACLMessage* msg, char* encoding) {
	ACLMessageSetEncoding(msg, encoding);
	return msg;
}

ACLMessage* SACLMessageSetOntology(ACLMessage* msg, char* ontology) {
	ACLMessageSetOntology(msg, ontology);
	return msg;
}

ACLMessage* SACLMessageSetProtocol(ACLMessage* msg, char* protocol) {
	ACLMessageSetProtocol(msg, protocol);
	return msg;
}

ACLMessage* SACLMessageSetConversationID(ACLMessage* msg, char* conversationID) {
	ACLMessageSetConversationID(msg, conversationID);
	return msg;
}

ACLMessage* SACLMessageSetReplyWith(ACLMessage* msg, char* replyWith)  {
	ACLMessageSetReplyWith(msg, replyWith);
	return msg;
}

ACLMessage* SACLMessageSetInReplyTo(ACLMessage* msg, char* inReplyTo)  {
	ACLMessageSetInReplyTo(msg, inReplyTo);
	return msg;
}

ACLMessage* SACLMessageSetReplyBy(ACLMessage* msg, char* replyBy)  {
	ACLMessageSetReplyBy(msg, replyBy);
	return msg;
}

ACLMessage* SACLMessageSetContent(ACLMessage* msg, char* content)  {
	ACLMessageSetContent(msg, content);
	return msg;
}

