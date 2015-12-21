/****************************************************************************************
 * Filename:	platform-defs.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declarations of the core structures that are used throughout the platform
 * **************************************************************************************/

#ifndef __PLATFORM_DEFS_H__
#define __PLATFORM_DEFS_H__

#include <glib.h>
#include <dbus/dbus.h>

/******************************************************************************
 * ******************** SERVICE DECLARATIONS ************************
 * ****************************************************************************/
/* definitions of the well known names for the aspects of the transport addresses used
 * in DBus for the platform components
 */
  
#define PLATFORM_SERVICE "uk.ac.bath.cs.CAP"
#define SERVICE_START "ap."
 
#define MTS_SERVICE_PATH "/ap/MTS"
#define MTS_NAME "MTS"
static const char* MTS_SERVICE_PATH_ARRAY[] = {"ap", "MTS", NULL};

#define DF_SERVICE_PATH "/ap/DF"
#define DF_NAME "DF"
static const char* DF_SERVICE_PATH_ARRAY[] = {"ap", "DF", NULL};

#define AMS_SERVICE_PATH "/ap/AMS"
#define AMS_NAME "AMS"
static const char* AMS_SERVICE_PATH_ARRAY[] = {"ap", "AMS", NULL};

#define TERMINATE_PATH "/ap/terminate"
static const char* TERMINATE_PATH_ARRAY[] = {"ap", "terminate", NULL};

#define MESSAGE_PATH "/ap/msg"
static const char* MESSAGE_PATH_ARRAY[] = {"ap", "msg", NULL};

#define MANAGEMENT_PATH "/ap/management"
static const char* MANAGEMENT_PATH_ARRAY[] = {"ap", "management", NULL};

#define DBUS_PROTOCOL_NAME "dbus"
#define DBUS_ACL_REPRESENTATION "dbus-acl"

/***************************************************************************************
 * ************* AGENT IDENTIFIER STRUCTURE ********************************
 * **************************************************************************************/
struct stAID {
	GString* name;
	GArray* addresses;
};
typedef struct stAID AID;

/***************************************************************************************
 * ********************* ERROR STRUCTURE **************************************
 * **************************************************************************************/
 struct stAPError {
 	GString* message;
 	int number;
};
typedef struct stAPError APError;

/***************************************************************************************
 * ***************** PLATFORM AGENT CONFIGURATIONS ********************
 * *************************************************************************************/
 
  /*************** DF SERVICE DESCRIPTION *******************/
struct stDFServiceDescription {
	GString* name;
	GString* type;
	GArray* protocols;
	GArray* ontologies;
	GArray* languages;
};
typedef struct stDFServiceDescription DFServiceDescription;
void DFServiceDescriptionInit(DFServiceDescription* desc);

/*************** AGENT DF SERVICE DESCRIPTION *******************/
struct stAgentDFDescription {
	AID* id;
	GArray* services;
	GArray* protocols;
	GArray* ontologies;
	GArray* languages;
};
typedef struct stAgentDFDescription AgentDFDescription;
void AgentDFDescriptionInit(AgentDFDescription* desc);
 
 /***************************************************************************************
 * ******************** AGENT COMMUNICATION *********************************
 * **************************************************************************************/

//define all of the performatives that can appear in an ACL message as specified
//in http://www.fipa.org/specs/FIPA00037
#define ACL_ACCEPT_PROPOSAL "accept-proposal"
#define ACL_AGREE "agree"
#define ACL_CANCEL "cancel"
#define ACL_CALL_FOR_PROPOSAL "cfp"
#define ACL_CONFIRM "confirm"
#define ACL_DISCONFIRM "disconfirm"
#define ACL_FAILURE "failure"
#define ACL_INFORM "inform"
#define ACL_INFORM_IF "inform-if"
#define ACL_INFOEM_REF "inform-ref"
#define ACL_NOT_UNDERSTOOD "not-understood"
#define ACL_PROPAGATE "propagate"
#define ACL_PROPOSE "propose"
#define ACL_PROXY "proxy"
#define ACL_QUERY_IF "query-if"
#define ACL_QUERY_REF "query-ref"
#define ACL_REFUSE "refuse"
#define ACL_REJECT_PROPOSAL "reject-proposal"
#define ACL_REQUEST "request"
#define ACL_REQUEST_WHEN "request-when"
#define ACL_WHENEVER "request-whenever"
#define ACL_SUBSCRIBE "subscribe"

//define the structure that will be used to represent an ACL message within the
//platform
struct stACLMessage {
	GString* performative;	
	AID* sender;
	GArray* receivers;
	GArray* replyTo;
	GString* language;
	GString* encoding;
	GString* ontology;
	GString* protocol;
	GString* conversationID;
	GString* replyWith;
	GString* inReplyTo;
	GString* replyBy;
	GString* content;		
};
typedef struct stACLMessage ACLMessage;
void ACLMessageInit(ACLMessage* msg);
void ACLMessageFree(ACLMessage msg);

//define the structre that will be used to represent the FIPA Enevelope
struct FIPAACLEnvelope {
	GArray* to;
	AID* from;
	GString* aclRepresentation;
	AID* intendedReceiver;
} ;
typedef struct FIPAACLEnvelope ACLEnvelope;
void ACLEnvelopeInit(ACLEnvelope* envelope);

struct stAgentMessage {
	ACLEnvelope* envelope;
	ACLMessage* payload;
};
typedef struct stAgentMessage AgentMessage;
void AgentMessageInit(AgentMessage* message);

typedef void (*MessageReceiver)(void*, AgentMessage*);

/***************************************************************************************
 * ********************* AGENT CONFIGURATION*********************************
 * **************************************************************************************/
 
struct stAgentConfig{
	DBusConnection* connection;
	GMainLoop* mainLoop;
	GString* baseService;
	AID* identifier;
	GList* messageQueue;
	GString* MTSAddress;
	GString* AMSAddress;
	GString* DFAddress;
	GString* platformName;
	AgentDFDescription* DFEntry;
	int conversationIDCounter;
	MessageReceiver callbackFunction;
	//void (*callbackFn) (void*, AgentMessage*);
};
typedef struct stAgentConfig AgentConfiguration;
void AgentConfigurationInit(AgentConfiguration* config);
void AgentConfigurationSetID(AgentConfiguration* config, AID* id);
void AgentConfigurationSetMainLoop(AgentConfiguration* config, GMainLoop* loop);
void AgentConfigurationSetConnection(AgentConfiguration* config, DBusConnection* conn);
void AgentConfigurationSetBaseService(AgentConfiguration* config, gchar* service);
 
/***************************************************************************************
 * ********************* PLATFORM DESCRIPTION********************************
 * **************************************************************************************/

struct stPlatformServiceDescription {
	GString* name;
	GString* address;
};
typedef struct stPlatformServiceDescription PlatformServiceDescription;
 
struct stPlatformDescription {
	GString* name;
	GArray* services;
};
typedef struct stPlatformDescription PlatformDescription;
void InitPlatformDescription(PlatformDescription* desc);

/***************************************************************************************
 * ***************************************** MTS **************************************
 * **************************************************************************************/
struct stMTSConfig {
	AgentConfiguration* configuration;
	PlatformServiceDescription* description;
};
typedef struct stMTSConfig MTSConfiguration;
extern MTSConfiguration theMTS;

/***************************************************************************************
 * ********************************** DF ***********************************************
 * **************************************************************************************/
struct stDFConfig {
	AgentConfiguration* configuration;
	PlatformServiceDescription* description;
	GArray* agentDirectory;
};
typedef struct stDFConfig DFConfiguration;
extern DFConfiguration theDF;

/***************************************************************************************
 * ********************************** AMS *********************************************
 * **************************************************************************************/
struct stAMSConfig {
	AgentConfiguration* configuration;
	GArray* agentDirectory;
	PlatformServiceDescription* description;
	PlatformDescription* platformDescription;
};
typedef struct stAMSConfig AMSConfiguration;
extern AMSConfiguration theAMS;

/***************************************************************************************
 * ********************** THE PLATFORM *******************************************
 * **************************************************************************************/
struct stPlatform {
	GString* name;
	GString* service;
};
typedef struct stPlatform Platform;
extern Platform thePlatform;

/***************************************************************************************
 * ************** METHOD CALL DECLARATIONS ********************************
 * *************************************************************************************/
 
 /* these form the last part of a transport address on DBus and are used to aid the
  * platform services to handle messages they receive, the name of the method call
  * is used in a switch statement to call the appropriate handler
  */
 
//STANDARD CALLS
#define MSG_PING "ping"
#define MSG_TERMINATE "terminate"
#define MSG_PRINT_AGENT_DIRECTORY "printDirectory"

//AMS specific
#define MSG_GET_DESCRIPTION "getDescription"
#define MSG_AMS_REGISTER "register"
#define MSG_AMS_DEREGISTER "deregister"
#define MSG_AMS_MODIFY "modify"
#define MSG_AMS_SEARCH "search"

//DF specific
#define MSG_DF_REGISTER "register"
#define MSG_DF_DEREGISTER "deregister"
#define MSG_DF_MODIFY "modify"
#define MSG_DF_SEARCH "search"

//MTS specific
#define MTS_MSG "agentMessage"

//time to wait for a reply
#define WAIT_TIME 5000


#endif
