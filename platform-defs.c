/****************************************************************************************
 * Filename:	platform-defs.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implementation of all of the core platform structures that are used extnsively
 * through the platform.  Functions are given for creation and manipluation of these
 * strucutres
 * **************************************************************************************/

#include "platform-defs.h"
#include "API/API.h"

/* initialises the platform description strucutre that is maintained by the AMS when
 * the platform is bootstrapped
 * 
 * desc - the description structure that is to be initialised, it must have been allocated
 * 	before calling this function
 */
void InitPlatformDescription(PlatformDescription* desc) {
	desc->name =NULL;
	desc->services = g_array_new(FALSE, FALSE, sizeof(PlatformServiceDescription*));
}

/* initialises a DF service description structure
 * 
 * desc - DF description object to be initialised which must have been previously allocated
 */
void DFServiceDescriptionInit(DFServiceDescription* desc) {
	desc->name = NULL;
	desc->type = NULL;
	desc->protocols = g_array_new(FALSE, FALSE, sizeof(GString*));
	desc->ontologies = g_array_new(FALSE, FALSE, sizeof(GString*));
	desc->languages = g_array_new(FALSE, FALSE, sizeof(GString*));
}

/* initialises a DF entry that is used within the DF registry
 * 
 * desc - the description object to be initialised that must have been previously allocated
 */
void AgentDFDescriptionInit(AgentDFDescription* desc) {
	desc->id = NULL;
	desc->services = g_array_new(FALSE, FALSE, sizeof(DFServiceDescription*));
	desc->protocols = g_array_new(FALSE, FALSE, sizeof(GString*));
	desc->ontologies = g_array_new(FALSE, FALSE, sizeof(GString*));
	desc->languages = g_array_new(FALSE, FALSE, sizeof(GString*));
}

/* initialises an agent configuration strucutre that is required by all agents and platform
 * services
 * 
 * config - structure that needs initialising which must have been allocated previously
 */
void AgentConfigurationInit(AgentConfiguration* config) {
	config->connection = NULL;
	config->mainLoop = NULL;
	config->baseService = NULL;
	config->identifier = NULL;
	config->messageQueue = NULL;
	config->MTSAddress = NULL;
	config->AMSAddress = NULL;
	config->DFAddress = NULL;
	config->platformName = NULL;
	config->DFEntry = g_new(AgentDFDescription,1);
	AgentDFDescriptionInit(config->DFEntry);
	config->conversationIDCounter = 0;
	config->callbackFunction = NULL;
}

//setter functions
void AgentConfigurationSetID(AgentConfiguration* config, AID* id) {
	config->identifier = id;	
}

void AgentConfigurationSetMainLoop(AgentConfiguration* config, GMainLoop* loop) {
	config->mainLoop = loop;
}
void AgentConfigurationSetConnection(AgentConfiguration* config, DBusConnection* conn) {
	config->connection = conn;
}

void AgentConfigurationSetBaseService(AgentConfiguration* config, gchar* service) {
	config->baseService = g_string_new(service);
}

/* Initialises an ACL message structure that is used for agent communication
 * 
 * msg - structure that should be initialised that must have been allocated previously
 */
void ACLMessageInit(ACLMessage* msg) {
	msg->performative = NULL;
	msg->sender = NULL;
	msg->receivers = g_array_new(FALSE, FALSE, sizeof(AID*));
	msg->replyTo = g_array_new(FALSE, FALSE, sizeof(AID*));
	msg->content = NULL;
	msg->language = NULL;
	msg->encoding = NULL;
	msg->ontology = NULL;
	msg->protocol = NULL;
	msg->conversationID = NULL;
	msg->replyWith = NULL;
	msg->inReplyTo = NULL;
	msg->replyBy = NULL;
}

/* reclaims memory used in a FIPA-ACL message
 * 
 * msg - the message that should be freed
 */
void ACLMessageFree(ACLMessage msg) {		
	g_array_free(msg.receivers, TRUE);
	g_array_free(msg.replyTo, TRUE);
	
	if (msg.sender !=  NULL) AIDFree(*msg.sender);
	
	//free all of the strings
	if (msg.performative !=  NULL) g_string_free(msg.performative, TRUE);
	if (msg.content !=  NULL) g_string_free(msg.content, TRUE);
	if (msg.language !=  NULL) g_string_free(msg.language, TRUE);
	if (msg.encoding !=  NULL) g_string_free(msg.encoding, TRUE);
	if (msg.ontology !=  NULL) g_string_free(msg.ontology, TRUE);
	if (msg.protocol !=  NULL) g_string_free(msg.protocol, TRUE);
	if (msg.conversationID !=  NULL) g_string_free(msg.conversationID, TRUE);
	if (msg.replyWith !=  NULL) g_string_free(msg.replyWith, TRUE);
	if (msg.inReplyTo !=  NULL) g_string_free(msg.inReplyTo, TRUE);
	if (msg.replyBy !=  NULL) g_string_free(msg.replyBy, TRUE);
}

/* initiailises a envelope that is used by the interaction layer for transporting messages
 * to the correct agents
 * 
 * envelope - the structure that should be initialised which have been previously allocated
 */
void ACLEnvelopeInit(ACLEnvelope* envelope) {
	envelope->to = g_array_new(FALSE, FALSE, sizeof(AID*));
	envelope->from = NULL;
	envelope->aclRepresentation = NULL;
	envelope->intendedReceiver = NULL;
}

/* initialises and agent message structure that is used for all agent messages sent to
 * the interaction layer
 * 
 * message - the structure to be initialised that must have been previously allocated
 */
void AgentMessageInit(AgentMessage* message) {
	message->envelope = NULL;
	message->payload = NULL;
}

