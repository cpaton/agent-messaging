%module agentPlatform
//%include "typemaps.i"
%{
	#include "../platform-defs.h"
	//#include "../API/API.h"
%}

/********** PLATFORM DEFINITIONS **************************/
extern void bootstrapPlatform();

/****************** USER AGENT DEFS ************************/
extern AgentConfiguration* AP_newAgent(char* INPUT, APError* INPUT);
extern void AP_finish(AgentConfiguration*, APError*);
extern void AP_agentSleep(AgentConfiguration*);
extern void AP_modifyAMSEntry(AgentConfiguration*, APError*);
extern GArray* AP_searchAMS(AgentConfiguration*, char*, APError*);
extern void AP_registerWithDF(AgentConfiguration*, APError*);
extern void AP_modifyDFEntry(AgentConfiguration*, APError*);
extern GArray* AP_searchDF(AgentConfiguration*, AgentDFDescription*, APError*);
extern void AP_send(AgentConfiguration*, ACLMessage*, APError*);
extern void AP_registerMessageReceiverCallback(AgentConfiguration*, MessageReceiver);
extern void AP_unregisterMessageReceiverCallback(AgentConfiguration*);

/******************* ACL ENVELOPE DEFS *********************/
extern AID* ACLEnvelopeGetFrom(ACLEnvelope*);
extern GArray* ACLEnvelopeGetTo(ACLEnvelope*);
extern GString* ACLEnvelopeGetACLRepresentation(ACLEnvelope*);
extern AID* ACLEnvelopeGetIntendedReceiver(ACLEnvelope*);
extern int ACLEnvelopeHasIntendedReceiver(ACLEnvelope*);
extern GString* ACLEnvelopeToString(ACLEnvelope*);

/***************** ACL MESSAGE DEFS ************************/
extern GString* ACLMessageGetPerformative(ACLMessage*);
extern AID* ACLMessageGetSender(ACLMessage*);
extern GArray* ACLMessageGetReceivers(ACLMessage*);
extern GArray* ACLMessageGetReplyTo(ACLMessage*);
extern GString* ACLMessageGetLanguage(ACLMessage*);
extern GString* ACLMessageGetEncoding(ACLMessage*);
extern GString* ACLMessageGetOntology(ACLMessage*);
extern GString* ACLMessageGetProtocol(ACLMessage*);
extern GString* ACLMessageGetConversationID(ACLMessage*);
extern GString* ACLMessageGetReplyWith(ACLMessage*);
extern GString* ACLMessageGetInReplyTo(ACLMessage*);
extern GString* ACLMessageGetReplyBy(ACLMessage*);
extern GString* ACLMessageGetContent(ACLMessage*);
extern GString* ACLMessageToString(ACLMessage*);
extern ACLMessage* ACLMessageNew(char*);
extern ACLMessage* ACLMessageCreateReply(ACLMessage*);
extern GString* AgentMessageToString(AgentMessage*);
extern ACLMessage* SACLMessageSetPerformative(ACLMessage*, char*);
extern ACLMessage* SACLMessageSetSender(ACLMessage*, AID*);
extern ACLMessage* SACLMessageAddReceiver(ACLMessage*, AID*);
extern ACLMessage* SACLMessageAddReplyTo(ACLMessage*, AID*);
extern ACLMessage* SACLMessageSetLanguage(ACLMessage*, char*);
extern ACLMessage* SACLMessageSetEncoding(ACLMessage*, char*);
extern ACLMessage* SACLMessageSetOntology(ACLMessage*, char*);
extern ACLMessage* SACLMessageSetProtocol(ACLMessage*, char*);
extern ACLMessage* SACLMessageSetConversationID(ACLMessage*, char*);
extern ACLMessage* SACLMessageSetReplyWith(ACLMessage*, char*);
extern ACLMessage* SACLMessageSetInReplyTo(ACLMessage*, char*);
extern ACLMessage* SACLMessageSetReplyBy(ACLMessage*, char*);
extern ACLMessage* SACLMessageSetContent(ACLMessage*, char*);

/***************** AGENT IDENTIFIER DEFS ********************************/
extern void AIDInit(AID*);
extern AID* AIDNew();
extern void AIDReInit(AID*);
extern AID* SAIDSetName(AID*, char*);
extern AID* SAIDAddAddress(AID*, char*) ;
extern GString* SAIDGetName(AID*);
extern GArray* SAIDGetAddresses(AID*);
extern GString* SAIDToString(AID*);

/******************* ERROR DEFS ******************************/
extern APError* APNewError();
extern int SAPErrorIsSet(APError*);
extern GString* SAPErrorGetError(APError*);

/******************* DF API DEFS ********************************************/
extern GString* AgentDFDescriptionToString(AgentDFDescription*);
extern GString* DFServiceDescriptionToString(DFServiceDescription*);
extern AgentDFDescription* DFDescNew();
extern AID* DFDescGetAID(AgentDFDescription*);
extern GArray* DFDescGetServices(AgentDFDescription*);
extern GArray* DFDescGetLanguages(AgentDFDescription*);
extern GArray* DFDescGetOntologies(AgentDFDescription*);
extern GArray* DFDescGetProtocols(AgentDFDescription*);
extern AgentDFDescription* SDFDescAddProtocol(AgentDFDescription*, char*);
extern AgentDFDescription* SDFDescAddOntology(AgentDFDescription*, char*);
extern AgentDFDescription* SDFDescAddLanguage(AgentDFDescription*, char*);
extern AgentDFDescription* SDFDescAddService(AgentDFDescription*, DFServiceDescription*);
extern DFServiceDescription* DFServiceDescriptionNew();
extern GString* ServiceDescGetName(DFServiceDescription*);
extern GString* ServiceDescGetType(DFServiceDescription*);
extern GArray* ServiceDescGetProtocols(DFServiceDescription*);
extern GArray* ServiceDescGetOntologies(DFServiceDescription*);
extern GArray* ServiceDescGetLanguages(DFServiceDescription*);
extern DFServiceDescription* SServiceDescAddProtocol(DFServiceDescription*, char*);
extern DFServiceDescription* SServiceDescAddOntology(DFServiceDescription*, char*);
extern DFServiceDescription* SServiceDescAddLanguage(DFServiceDescription*, char*);

/**************** C ARTEFACTS DEFS ************************/
extern char* gstrToString(GString*);


