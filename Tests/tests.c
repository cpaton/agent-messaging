/****************************************************************************************
 * Filename:	tests.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implemenation of functions used to test the platform. Includes implementations of the
 * tests that do not require the use of agents, and the test driver program that is used
 * to determine which test to run
 * **************************************************************************************/

#include "tests.h"

#include <glib.h>
//#include "../FIPA/AID.h"
//#include "../Lang/ACL/ACLMessage.h"
//#include "../Lang/ACL/ACL-defs.h"
#include <dbus/dbus.h>
#include "../platform-defs.h"
#include "../API/API.h"
#include "../util.h"
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <string.h>

/* test-driver function that calls the appropraite test for the given arguments
 * 
 * argv - copy of the command line arguments that are used to determine which test
 * 	to run
 */
void runTests(char** argv) {
	//we need to run the approprate test
	if (strcmp(argv[1], "aid") == 0) {
		printf("********* Running the AID Tests **********\n");
		AIDTest();
		printf("********* Finished the AID Tests **********\n");
	}
	else if (strcmp(argv[1], "acl") == 0) {
		printf("********* Running the ACL Tests **********\n");
		ACLTest();
		printf("********* Finished the ACL Tests **********\n");
	}
	else if (strcmp(argv[1], "pings") == 0) {
		printf("********* Running Ping The Platform Services Tests **********\n");
		DBusConnection* conn = getConnection();
		sendTestMessage(conn, PLATFORM_SERVICE, MTS_SERVICE_PATH, MSG_PING);
		sendTestMessage(conn, PLATFORM_SERVICE, AMS_SERVICE_PATH, MSG_PING);
		sendTestMessage(conn, PLATFORM_SERVICE, DF_SERVICE_PATH, MSG_PING);
		dbus_connection_unref(conn);
		printf("********* Finished Ping The Platform Services Tests **********\n");
	}
	else if (strcmp(argv[1], "ping") == 0) {
		printf("********* Running the Ping Boot Tests **********\n");
		DBusConnection* conn = getConnection();
		GString* service = g_string_new("ap.");
		g_string_sprintfa(service, argv[2]);
		sendTestMessage(conn, service->str, MANAGEMENT_PATH, MSG_PING);			
		dbus_connection_unref(conn);
		printf("********* Finished the Ping Boot Tests **********\n");
	}
	else if (strcmp(argv[1], "term") == 0) {
		printf("********* Running the Terminate Tests **********\n");
		DBusConnection* conn = getConnection();
		GString* service = g_string_new("ap.");
		g_string_sprintfa(service, argv[2]);
		sendTestMessage(conn, service->str, MANAGEMENT_PATH, MSG_TERMINATE);			
		dbus_connection_unref(conn);
		printf("********* Finished the Terminate Tests **********\n");
	}
	else if (strcmp(argv[1], "amsmsg") == 0) {
		printf("********* Running the AMS msg Tests **********\n");
		DBusConnection* conn = getConnection();
		sendTestMessage(conn, PLATFORM_SERVICE, AMS_SERVICE_PATH, MSG_PING);
		sendTestMessage(conn, PLATFORM_SERVICE, AMS_SERVICE_PATH, MSG_PRINT_AGENT_DIRECTORY);
		sendTestMessage(conn, PLATFORM_SERVICE, AMS_SERVICE_PATH, MSG_GET_DESCRIPTION);
		dbus_connection_unref(conn);
		printf("********* Finished the AMS msg Tests **********\n");
	}
	else if (strcmp(argv[1], "dfmsg") == 0) {
		printf("********* Running the AMS msg Tests **********\n");
		DBusConnection* conn = getConnection();
		sendTestMessage(conn, PLATFORM_SERVICE, DF_SERVICE_PATH, MSG_PING);
		sendTestMessage(conn, PLATFORM_SERVICE, DF_SERVICE_PATH, MSG_PRINT_AGENT_DIRECTORY);
		sendTestMessage(conn, PLATFORM_SERVICE, DF_SERVICE_PATH, MSG_DF_REGISTER);
		sendTestMessage(conn, PLATFORM_SERVICE, DF_SERVICE_PATH, MSG_DF_DEREGISTER);
		sendTestMessage(conn, PLATFORM_SERVICE, DF_SERVICE_PATH, MSG_DF_MODIFY);
		sendTestMessage(conn, PLATFORM_SERVICE, DF_SERVICE_PATH, MSG_DF_SEARCH);
		dbus_connection_unref(conn);
		printf("********* Finished the AMS msg Tests **********\n");
	}
	else if (strcmp(argv[1], "terminate") == 0) {
		printf("********* Running the Terminate Tests **********\n");
		DBusConnection* conn = getConnection();
		sendTestMessage(conn, PLATFORM_SERVICE, TERMINATE_PATH, MSG_PING);
		dbus_connection_unref(conn);
		printf("********* Finished the Terminate Tests **********\n");
	}
	else if (strcmp(argv[1], "printdir") == 0) {
		printf("********* Running the Print Directory Tests **********\n");
		DBusConnection* conn = getConnection();
		sendTestMessage(conn, PLATFORM_SERVICE, AMS_SERVICE_PATH, MSG_PRINT_AGENT_DIRECTORY);
		sendTestMessage(conn, PLATFORM_SERVICE, DF_SERVICE_PATH, MSG_PRINT_AGENT_DIRECTORY);
		dbus_connection_unref(conn);
		printf("********* Finished the Print Directory Tests **********\n");
	}		
	else if (strcmp(argv[1], "temp") == 0) {
		GString* str = getMachineName();
		printf("The host name of this machine is : %s\n", str->str);
	}
	else if (strcmp(argv[1], "boot") == 0) {
		printf("********* Running the bootstrap tests **********\n");
		bootAgent(argv[2]);
		printf("********* Finished the boostrap tests **********\n");
	}		
	else if (strcmp(argv[1], "amsmod") == 0) {
		printf("********* Running the AMS modify tests **********\n");
		AMSModifyAgent("AMSMod");
		printf("********* Finished the AMS modify tests **********\n");
	}		
	else if (strcmp(argv[1], "amssearch") == 0) {
		printf("********* Running the AMS search tests **********\n");
		AMSSearchAgent("AMSSearcher");
		printf("********* Finished the AMS search tests **********\n");
	}
	else if (strcmp(argv[1], "dfreg") == 0) {
		printf("********* Running the DF registration tests **********\n");
		DFRegAgent("DFReg");
		printf("********* Finished the DF registration tests **********\n");
	}		
	else if (strcmp(argv[1], "dfmod") == 0) {
		printf("********* Running the DF modify tests **********\n");
		DFModifyAgent("DFModder");
		printf("********* Finished the DF modify tests **********\n");
	}
	else if (strcmp(argv[1], "dfsearch") == 0) {
		printf("********* Running the DF search tests **********\n");
		DFSearchAgent("DFSearcher");
		printf("********* Finished the DF search tests **********\n");
	}		
	else if (strcmp(argv[1], "agent") == 0) {
		printf("********* Running the Agent **********\n");
		agent(argv[2]);
		printf("********* Finished the Agent **********\n");
	}		
	else if (strcmp(argv[1], "server") == 0) {
		printf("********* Running the server **********\n");
		serverAgent(argv[2]);
		printf("********* Finished the server **********\n");
	}
	else if (strcmp(argv[1], "dfsearch") == 0) {
		printf("********* Running the DF Search Test **********\n");
		dfSearch();
		printf("********* Finished the DF Search Test **********\n");
	}		
	else {
		g_warning("Unknown test to perform. Doing nothing");
	}		
}

/* utility function used to get a connection to the DBus message bus that can be used
 * by the test programs
 * 
 * returns - a connection to the transport bus
 */
DBusConnection* getConnection() {	
	DBusError error;
	DBusConnection* conn;
	dbus_error_init(&error);		
	//connect to the session bus and integrate it with a GLib
	conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if (dbus_error_is_set(&error)) {
		//we were unable to connect to the session bus
		g_error("Unable to connect to the session bus %s", error.message);
		dbus_error_free(&error);
		exit(1);		
	}
	else {
		g_message("Connection successful");
	}		
	
	return conn;
	
}

/* tests the AID structure implementation to make sure that it works correctly using
 * the functions offered by the API for manipulating these structures
 */
void AIDTest() {
	//create a new AID
	AID id;	
	
	g_message("Initialising the AID struct");
	AIDInit(&id);
	
	//add some information in the AID	
	AIDSetName(&id, "craig@rpc-ma0cap");		
	AIDAddAddress(&id, ":1.2");
	AIDAddAddress(&id, "DBUS::1.3");	
	
	GString* str = AIDToString(id);
	g_message(str->str);
	g_string_free(str, TRUE);
	
	//clone the structure and try printing it again
	g_message("Cloning the structure");
	AID* temp = AIDClone(id);
	g_message("Printing out the cloned structure");
	str = AIDToString(*temp);
	g_message(str->str);
	g_string_free(str, TRUE);
	
	g_message("Freeing the AID struct");
	AIDFree(id);	
}

/* tests the ACL structure implementation to make sure that it works correctly using
 * the functions offered by the API for manipulating these structures
 */
void ACLTest() {
	//create a new ACLMessage
	ACLMessage* msg = ACLMessageNew(ACL_REQUEST);
	
	//set some of the fields	
	AID* sender = g_new(AID, 1);
	AIDInit(sender);
	AIDSetName(sender, "AgentA");
	ACLMessageSetSender(msg, sender);	
	
	AID* receiver = g_new(AID, 1);
	AIDInit(receiver);
	AIDSetName(receiver, "AgentB");
	ACLMessageAddReceiver(msg, receiver);
	ACLMessageAddReplyTo(msg, receiver);
	
	ACLMessageSetContent(msg, "(start ?x tv)");
	ACLMessageSetEncoding(msg, "fipa.sl.std.string");
	ACLMessageSetLanguage(msg, "fipa.sl");
	ACLMessageSetOntology(msg, "FIPAAgentManagement");
	
	//print the message to the screen
	GString* temp = ACLMessageToString(msg);
	g_message(temp->str);
	g_string_free(temp, TRUE);
	
	//create a reply to the message
	ACLMessage* reply = ACLMessageCreateReply(msg);
	temp = ACLMessageToString(reply);
	g_message(temp->str);
	g_string_free(temp, TRUE);
	
	
	ACLMessageFree(*msg);
	g_free(msg);
}

/* sends an empty message to a service running on the platform - used to demonstrate
 * that a process has connected to the transport bus correctly and that it is listening
 * for messages
 * 
 * cn_conn - connection to the transport bus
 * service - the name of the service on the transport bus to which the message is to be sent
 * path - the path on ther service that the mesage is to be sent
 * method - the name of the method to which the message is to be sent
 */
void sendTestMessage(DBusConnection* cn_conn, gchar* service, gchar* path, gchar* method) {
	g_message("Sending test message to service %s with path %s and method %s", service, path, method);
	DBusMessage* msg = dbus_message_new_method_call(service, path, service, method);
	if (!msg) {
		g_error("Unable to create message exiting");
		exit(1);
	}
	dbus_message_set_no_reply(msg, TRUE);
	
	//the message content
	
	//now send the message without expecting a reply	
	g_message("Sending the test message");
	if (!dbus_connection_send(cn_conn, msg, NULL)) {
		g_error("Failed to send the message due to memory failure");
		exit(1);
	}
	dbus_connection_flush(cn_conn);
	g_message("Message sent");	
}

/* used to test the search semantics employed by the DF service to ensure that it
 * returns the correct results
 */
void dfSearch() {
	/***************** THE DATABASE ********************/
	//build the default entry in the list
	AgentDFDescription database;
	AgentDFDescriptionInit(&database);
	DFDescAddLanguage(&database, "fipa-sl0");
	DFDescAddLanguage(&database, "fipa-sl1");
	DFDescAddLanguage(&database, "fipa-sl2");
	DFDescAddOntology(&database, "ap-management");
	DFDescAddProtocol(&database, "fipa-request");
	
	//the AID
	AID id;
	AIDInit(&id);
	id.name = g_string_new("agentA@rpc-ma0cap");
	AIDAddAddress(&id, "ap.agentA");
	database.id = &id;
	
	//add a service entry
	DFServiceDescription service;
	DFServiceDescriptionInit(&service);
	service.name = g_string_new("testService");
	service.type = g_string_new("test");
	ServiceDescAddProtocol(&service, "p");
	ServiceDescAddLanguage(&service, "l");
	ServiceDescAddOntology(&service, "o");
	DFDescAddService(&database, &service);
	
	DFServiceDescription service2;
	DFServiceDescriptionInit(&service2);
	service2.name = g_string_new("testService2");
	service2.type = g_string_new("test");
	ServiceDescAddProtocol(&service2, "p2");
	ServiceDescAddLanguage(&service2, "l2");
	ServiceDescAddOntology(&service2, "o2");
	DFDescAddService(&database, &service2);
	
	//print out the entry
	GString* temp = AgentDFDescriptionToString(&database);
	g_message("Database is");
	g_message("%s", temp->str);
	g_string_free(temp, TRUE);
	
	
	
	/************ THE TEMPLATE ****************************/
	AgentDFDescription template;
	AgentDFDescriptionInit(&template);
	DFDescAddLanguage(&template, "fipa-sl0");
	DFDescAddOntology(&template, "ap-management");
	DFDescAddProtocol(&template, "fipa-request");
	
	//the AID
	AID id2;
	AIDInit(&id2);
	id2.name = g_string_new("agentA");
	template.id = &id2;
	
	//the services
	DFServiceDescription service3;
	DFServiceDescriptionInit(&service3);
	//service3.name = g_string_new("testService2");
	//service3.type = g_string_new("test");
	//ServiceDescAddProtocol(&service3, "p2");
	//ServiceDescAddLanguage(&service3, "l2");
	ServiceDescAddOntology(&service3, "o3");
	DFDescAddService(&template, &service3);
	
	//print out the entry
	temp = AgentDFDescriptionToString(&template);
	g_message("\n\nTemplate is");
	g_message("%s", temp->str);
	g_string_free(temp, TRUE);
	
	if (matches(&database, &template))
		g_message("\n\nMATCHED");
	else
		g_message("\n\nNO MATCH");
	
}


