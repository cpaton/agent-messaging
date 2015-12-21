/****************************************************************************************
 * Filename:	agent.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declarations of the bulk of the API that provides functions for interacting with
 * the platform services and support for starting and ending an agent correctly.
 * **************************************************************************************/

#ifndef _API_AGENT_H__
#define _API_AGENT_H__

#include <glib.h>
#include "../platform-defs.h"
#include "API.h"

/****************** STARTING AND FINISHING ************************/
AgentConfiguration* AP_newAgent(char* agentName, APError* err);
void AP_finish(AgentConfiguration* agent, APError* err);

/****************** AMS FUNCTIONS ************************************/
void AP_modifyAMSEntry(AgentConfiguration* agent, APError* err);
GArray* AP_searchAMS(AgentConfiguration* agent, char* name, APError* error);

/******************* DF FUNCTIONS ************************************/
void AP_registerWithDF(AgentConfiguration* agent, APError* err);
void AP_modifyDFEntry(AgentConfiguration* agent, APError* err);
GArray* AP_searchDF(AgentConfiguration* agent, AgentDFDescription* template, APError* err);

/****************** MTS FUNCTIONS **********************************/
void AP_send(AgentConfiguration* agent, ACLMessage* msg, APError* err);

/***************** UTILITIES ******************************************/
void AP_registerMessageReceiverCallback(AgentConfiguration* agent, MessageReceiver fn);
void AP_unregisterMessageReceiverCallback(AgentConfiguration* agent);
void AP_agentSleep(AgentConfiguration* agent);

/****************** UTILITIES FOR SWIG ****************************/
char* gstrToString(GString* gstr);

#endif
