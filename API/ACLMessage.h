/****************************************************************************************
 * Filename:	ACLMessage.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declarations of the functions used to create and manipulate FIPA-ACL messages
 * that are used in for agent communication on the platform.
 * **************************************************************************************/

#ifndef __ACLMESSAGE_H__
#define __ACLMESSAGE_H__

#include <glib.h>
#include "../platform-defs.h"

//setter methods
void ACLMessageSetPerformative(ACLMessage* msg, char* performative);
void ACLMessageSetSender(ACLMessage* msg, AID* sender);
void ACLMessageAddReceiver(ACLMessage* msg, AID* receiver);
void ACLMessageAddReplyTo(ACLMessage* msg, AID* replyTo);
void ACLMessageSetLanguage(ACLMessage* msg, char* language);
void ACLMessageSetEncoding(ACLMessage* msg, char* encoding);
void ACLMessageSetOntology(ACLMessage* msg, char* ontology);
void ACLMessageSetProtocol(ACLMessage* msg, char* protocol);
void ACLMessageSetConversationID(ACLMessage* msg, char* conversationID);
void ACLMessageSetReplyWith(ACLMessage* msg, char* replyWith);
void ACLMessageSetInReplyTo(ACLMessage* msg, char* inReplyTo);
void ACLMessageSetReplyBy(ACLMessage* msg, char* replyBy);
void ACLMessageSetContent(ACLMessage* msg, char* content);

//getter methods
GString* ACLMessageGetPerformative(ACLMessage* msg);
AID* ACLMessageGetSender(ACLMessage* msg);
GArray* ACLMessageGetReceivers(ACLMessage* msg);
GArray* ACLMessageGetReplyTo(ACLMessage* msg);
GString* ACLMessageGetLanguage(ACLMessage* msg);
GString* ACLMessageGetEncoding(ACLMessage* msg);
GString* ACLMessageGetOntology(ACLMessage* msg);
GString* ACLMessageGetProtocol(ACLMessage* msg);
GString* ACLMessageGetConversationID(ACLMessage* msg);
GString* ACLMessageGetReplyWith(ACLMessage* msg);
GString* ACLMessageGetInReplyTo(ACLMessage* msg);
GString* ACLMessageGetReplyBy(ACLMessage* msg);
GString* ACLMessageGetContent(ACLMessage* msg);

//utility functions
GString* ACLMessageToString(ACLMessage* msg);
ACLMessage* ACLMessageNew(char* performative);
ACLMessage* ACLMessageCreateReply(ACLMessage* msg);
GString* AgentMessageToString(AgentMessage* msg);

/************* FUNCTIONS FOR SWIG IMPL ***************************************/
ACLMessage* SACLMessageSetPerformative(ACLMessage* msg, char* performative);
ACLMessage* SACLMessageSetSender(ACLMessage* msg, AID* sender);
ACLMessage* SACLMessageAddReceiver(ACLMessage* msg, AID* receiver);
ACLMessage* SACLMessageAddReplyTo(ACLMessage* msg, AID* replyTo);
ACLMessage* SACLMessageSetLanguage(ACLMessage* msg, char* language);
ACLMessage* SACLMessageSetEncoding(ACLMessage* msg, char* encoding);
ACLMessage* SACLMessageSetOntology(ACLMessage* msg, char* ontology);
ACLMessage* SACLMessageSetProtocol(ACLMessage* msg, char* protocol);
ACLMessage* SACLMessageSetConversationID(ACLMessage* msg, char* conversationID);
ACLMessage* SACLMessageSetReplyWith(ACLMessage* msg, char* replyWith);
ACLMessage* SACLMessageSetInReplyTo(ACLMessage* msg, char* inReplyTo);
ACLMessage* SACLMessageSetReplyBy(ACLMessage* msg, char* replyBy);
ACLMessage* SACLMessageSetContent(ACLMessage* msg, char* content);



#endif
