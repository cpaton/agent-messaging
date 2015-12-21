/****************************************************************************************
 * Filename:	test-agents.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implemenation of agents used to test the platform
 * **************************************************************************************/

#include "test-agents.h"
#include "../API/API.h"
#include <stdio.h>

/* function registered with the API that is used as the callback function when a message
 * is received, it simply echoes the message received to the terminal window
 * 
 * data - the agent configuration structure
 * message - the message that was received
 */
void callbackFn(void* data, AgentMessage* message) {
	AgentConfiguration* agent = (AgentConfiguration*)data;
	
	//simply print the message
	GString* gstr = AgentMessageToString(message);
	g_message("Received message\n%s", gstr->str);
	g_string_free(gstr, TRUE);
	
	//make the agent exit
	g_main_loop_quit(agent->mainLoop);
}

/* simple agent that just boots up and then terminates - used to test the bootstrap process
 * 
 * name - the name that the agent should use
 */
void bootAgent(char* name) {
	APError error;
	APErrorInit(&error);
	
	AgentConfiguration* myAgent = AP_newAgent(name, &error);	
	if (APErrorIsSet(error)) {
		g_message("Unable to bootstrap agent - %s", error.message->str);
		APErrorFree(&error);
		return;
	}
	else {
		g_message("Agent bootstrapped successfully");
	}	
	
	g_message("Finishing Agent...");
	AP_finish(myAgent, &error);
}

/* simple agent that just boots up and then modifies its AMS entry
 * 
 * name - the name that the agent should use
 */
void AMSModifyAgent(char* name) {
	APError error;
	APErrorInit(&error);
	
	AgentConfiguration* myAgent = AP_newAgent(name, &error);	
	if (APErrorIsSet(error)) {
		g_message("Unable to bootstrap agent - %s", error.message->str);
		APErrorFree(&error);
		return;
	}
	else {
		g_message("Agent bootstrapped successfully");
	}	
	
/******************** AMS MODIFY ************************************************/
	//modify the AMS entry
	AIDAddAddress(myAgent->identifier, "newAddress");
	GString* str = AIDToString((*myAgent->identifier));
	g_message("New AID : %s", str->str);
	g_string_free(str, TRUE);
	AP_modifyAMSEntry(myAgent, &error);
	if (APErrorIsSet(error)) {
		g_message("Unable to modify AMS entry gent - %s", error.message->str);
		APErrorReInit(&error);
	}
	else {
		g_message("AMS entry modified");
	}	
	
	
	g_message("Finishing Agent...");
	AP_finish(myAgent, &error);
}

/* simple agent that just boots up and then  searches for a server agent
 * 
 * name - the name that the agent should use
 */
void AMSSearchAgent(char* name) {
	APError error;
	APErrorInit(&error);
	
	AgentConfiguration* myAgent = AP_newAgent(name, &error);	
	if (APErrorIsSet(error)) {
		g_message("Unable to bootstrap agent - %s", error.message->str);
		APErrorFree(&error);
		return;
	}
	else {
		g_message("Agent bootstrapped successfully");
	}	
	
	/************************** AMS SEARCH *************************************/
	//search the AMS for an entry
	GArray* results = AP_searchAMS(myAgent, "server", &error);
	if (APErrorIsSet(error)) {
		g_message("AMS serach failed - %s", error.message->str);
		APErrorReInit(&error);
	}
	else {
		g_message("AMS search ok");
		int i;
		for (i=0; i<results->len; i++) {
			AID* id = g_array_index(results, AID*, i);
			GString* str = AIDToString(*id);
			g_message(str->str);
			g_string_free(str, TRUE);
		}
	}

	
	g_message("Finishing Agent...");
	AP_finish(myAgent, &error);
}

/* simple agent that just boots up registers an entry with the DF
 * 
 * name - the name that the agent should use
 */
void DFRegAgent(char* name) {
	APError error;
	APErrorInit(&error);
	
	AgentConfiguration* myAgent = AP_newAgent(name, &error);	
	if (APErrorIsSet(error)) {
		g_message("Unable to bootstrap agent - %s", error.message->str);
		APErrorFree(&error);
		return;
	}
	else {
		g_message("Agent bootstrapped successfully");
	}
	
	/**************************** DF REGISTER *****************************************/
	//test the DF stuff
	DFDescAddLanguage(myAgent->DFEntry, "fipa-sl0");
	DFDescAddLanguage(myAgent->DFEntry, "fipa-sl1");
	DFDescAddLanguage(myAgent->DFEntry, "fipa-sl2");
	DFDescAddOntology(myAgent->DFEntry, "ap-management");
	DFDescAddProtocol(myAgent->DFEntry, "fipa-request");
	
	//add a service entry
	DFServiceDescription service;
	DFServiceDescriptionInit(&service);
	service.name = g_string_new("testService");
	service.type = g_string_new("test");
	ServiceDescAddProtocol(&service, "p");
	ServiceDescAddLanguage(&service, "l");
	ServiceDescAddOntology(&service, "o");
	//DFDescAddService(myAgent->DFEntry, &service);
	
	DFServiceDescription service2;
	DFServiceDescriptionInit(&service2);
	service2.name = g_string_new("testService2");
	service2.type = g_string_new("test");
	ServiceDescAddProtocol(&service2, "p2");
	ServiceDescAddLanguage(&service2, "l2");
	ServiceDescAddOntology(&service2, "o2");
	//DFDescAddService(myAgent->DFEntry, &service2);
	
	//print out the entry
	GString* temp = AgentDFDescriptionToString(myAgent->DFEntry);
	g_message("My DF entry is");
	g_message("%s", temp->str);
	g_string_free(temp, TRUE);
	
	//register the entry with the DF
	g_message("Registering entry with the DF");
	AP_registerWithDF(myAgent, &error);
	if (APErrorIsSet(error)) {
		g_message("DF registration failed - %s", error.message->str);
		APErrorReInit(&error);
	}
	else {
		g_message("DF registration suceeded");
	}

	// deliberately bypass the deregistration to allow printing of the directory
	//g_message("Finishing Agent...");
	//AP_finish(myAgent, &error);
}

/* simple agent that just boots up registers an entry with the DF and then modifies it
 * 
 * name - the name that the agent should use
 */
void DFModifyAgent(char* name) {
	APError error;
	APErrorInit(&error);
	
	AgentConfiguration* myAgent = AP_newAgent(name, &error);	
	if (APErrorIsSet(error)) {
		g_message("Unable to bootstrap agent - %s", error.message->str);
		APErrorFree(&error);
		return;
	}
	else {
		g_message("Agent bootstrapped successfully");
	}
	
	/**************************** DF REGISTER *****************************************/
	//test the DF stuff
	DFDescAddLanguage(myAgent->DFEntry, "fipa-sl0");
	DFDescAddLanguage(myAgent->DFEntry, "fipa-sl1");
	DFDescAddLanguage(myAgent->DFEntry, "fipa-sl2");
	DFDescAddOntology(myAgent->DFEntry, "ap-management");
	DFDescAddProtocol(myAgent->DFEntry, "fipa-request");
	
	//add a service entry
	DFServiceDescription service;
	DFServiceDescriptionInit(&service);
	service.name = g_string_new("testService");
	service.type = g_string_new("test");
	ServiceDescAddProtocol(&service, "p");
	ServiceDescAddLanguage(&service, "l");
	ServiceDescAddOntology(&service, "o");
	//DFDescAddService(myAgent->DFEntry, &service);
	
	DFServiceDescription service2;
	DFServiceDescriptionInit(&service2);
	service2.name = g_string_new("testService2");
	service2.type = g_string_new("test");
	ServiceDescAddProtocol(&service2, "p2");
	ServiceDescAddLanguage(&service2, "l2");
	ServiceDescAddOntology(&service2, "o2");
	//DFDescAddService(myAgent->DFEntry, &service2);
	
	//print out the entry
	GString* temp = AgentDFDescriptionToString(myAgent->DFEntry);
	g_message("My DF entry is");
	g_message("%s", temp->str);
	g_string_free(temp, TRUE);
	
	//register the entry with the DF
	g_message("Registering entry with the DF");
	AP_registerWithDF(myAgent, &error);
	if (APErrorIsSet(error)) {
		g_message("DF registration failed - %s", error.message->str);
		APErrorReInit(&error);
	}
	else {
		g_message("DF registration suceeded");
	}
	
	/*************************** DF MODIFY ******************************************/
	//change the entry
	DFDescAddService(myAgent->DFEntry, &service2);
	AP_modifyDFEntry(myAgent, &error);

	// deliberately bypass the deregistration to allow printing of the directory
	//g_message("Finishing Agent...");
	//AP_finish(myAgent, &error);
}

/* simple agent that just boots up then searches the DF for some servers
 * 
 * name - the name that the agent should use
 */
void DFSearchAgent(char* name) {
	APError error;
	APErrorInit(&error);
	
	AgentConfiguration* myAgent = AP_newAgent(name, &error);	
	if (APErrorIsSet(error)) {
		g_message("Unable to bootstrap agent - %s", error.message->str);
		APErrorFree(&error);
		return;
	}
	else {
		g_message("Agent bootstrapped successfully");
	}	
	
	/****************************** DF SEARCH **************************************/
	//now perform a search for an entry
	AgentDFDescription template;
	AgentDFDescriptionInit(&template);
	AID id;
	AIDInit(&id);
	id.name = g_string_new("test@rpc-ma0cap");
	//template.id = &id;
	DFDescAddOntology(&template, "test-server");
	GArray* results = AP_searchDF(myAgent, &template, &error);
	g_message("Search produced %d results", results->len);
	int i;
	for (i=0; i<results->len; i++) {
		AgentDFDescription* entry = g_array_index(results, AgentDFDescription*, i);
		GString* temp = AgentDFDescriptionToString(entry);
		g_message("%s", temp->str);
		g_string_free(temp, TRUE);
	}

	
	g_message("Finishing Agent...");
	AP_finish(myAgent, &error);
}

void agent(char* name) {
	APError error;
	APErrorInit(&error);
/********************** BOOTSTRAP *****************************************/
	AgentConfiguration* myAgent = AP_newAgent(name, &error);	
	if (APErrorIsSet(error)) {
		g_message("Unable to bootstrap agent - %s", error.message->str);
		APErrorFree(&error);
		return;
	}
	else {
		g_message("Agent bootstrapped successfully");
	}	
	
/**************************** ACL MESSAGE TEST ***********************************/
	//create a new ACLMessage
	ACLMessage* msg = ACLMessageNew(ACL_QUERY_IF);
	
	//set some of the fields		
	AID* receiver = g_new(AID, 1);
	AIDInit(receiver);
	AIDSetName(receiver, "server1");
	ACLMessageAddReceiver(msg, receiver);
	
	AID* receiver2 = g_new(AID, 1);
	AIDInit(receiver2);
	AIDSetName(receiver2, "server2");
	ACLMessageAddReceiver(msg, receiver2);
	
	ACLMessageAddReplyTo(msg, receiver);
	
	ACLMessageSetContent(msg, "ping");
	ACLMessageSetEncoding(msg, "std.string");
	ACLMessageSetLanguage(msg, "string");
	ACLMessageSetOntology(msg, "ap-tests");
	
	//now send the message
	g_message("Sending the message");
	AP_send(myAgent, msg, &error);
	if (APErrorIsSet(error)) {
		g_message("Message sending failed - %s", error.message->str);
		APErrorReInit(&error);
	}
	else {
		g_message("Message sent");
	}
	
	//register the function that will be used to handle messages
	AP_registerMessageReceiverCallback(myAgent, callbackFn);
	
	//put the agent to sleep waiting for messages
	g_message("Agent sleeping...");
	AP_agentSleep(myAgent);
	
	//put the agent to sleep waiting for messages
	//g_message("Agent sleeping...");
	//g_main_loop_run(myAgent->mainLoop);
	
/**************** DEREGISTRATION *****************************************/	
	g_message("Finishing Agent...");
	AP_finish(myAgent, &error);
}

/* callback function registered with the API by server agents used in some of the tests,
 * it simply echoes the message received to the screen and sends a reply saying
 * i'm here
 * 
 * data - agent configuration structure
 * message - the message that was sent to the server
 */
void serverCallbackFn(void* data, AgentMessage* message) {
	AgentConfiguration* agent = (AgentConfiguration*)data;
	
	//simply reply to the message
	GString* gstr = AgentMessageToString(message);
	g_message("Received message\n%s\nSending reply", gstr->str);
	g_string_free(gstr, TRUE);
	ACLMessage* reply = ACLMessageCreateReply(message->payload);
	ACLMessageSetContent(reply, "im here");
	ACLMessageSetPerformative(reply, ACL_INFORM);
	
	APError error;
	APErrorInit(&error);
	AP_send(agent, reply, &error);
}

/* implementation of the simple server agents that are used for the testing of some
 * of the functionality.  It bootstraps adds a DF entry and then waits to receive
 * messages
 * 
 * name - the name that the agent should use
 */
void serverAgent(char* name) {
	APError error;
	APErrorInit(&error);
	AgentConfiguration* myAgent = AP_newAgent(name, &error);	
	if (APErrorIsSet(error)) {
		g_message("Unable to bootstrap agent - %s", error.message->str);
		APErrorFree(&error);
		return;
	}
	else {
		g_message("Agent bootstrapped successfully");
	}	
	
	//add a DF entry for this agent
	DFDescAddOntology(myAgent->DFEntry, "test-server");
	DFDescAddProtocol(myAgent->DFEntry, "fipa-request");
	g_message("Registering entry with the DF");
	AP_registerWithDF(myAgent, &error);
	if (APErrorIsSet(error)) {
		g_message("DF registration failed - %s", error.message->str);
		APErrorReInit(&error);
	}
	else {
		g_message("DF registration suceeded");
	}
	
	//register the function that will be used to handle messages
	AP_registerMessageReceiverCallback(myAgent, serverCallbackFn);
	
	//put the agent to sleep waiting for messages
	g_message("Agent sleeping...");
	AP_agentSleep(myAgent);
	
	g_message("Finishing Agent...");
	AP_finish(myAgent, &error);
}
