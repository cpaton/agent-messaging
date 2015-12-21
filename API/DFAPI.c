/****************************************************************************************
 * Filename:	DFAPI.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Functions to create and manipulate the ontology objects used in conversations
 * with the DF support services.  These structures must be used in conversations
 * with the DF.
 * **************************************************************************************/

#include "DFAPI.h"
#include "API.h"
#include "../util.h"

/***************************************************************************************
 * ****************** AGENT DF DESCRIPTIONS **********************************
 * **************************************************************************************/

/* converts a DF ddescription into a LISP like string that can be used for debugging
 * and testing.
 * 
 * desc - the DF description you want converting into a string
 * returns - GString for the string representation
 */
GString* AgentDFDescriptionToString(AgentDFDescription* desc) {
	GString* gstr = g_string_new("(df-agent-description");
	GString* temp;
	int i;
	
	//the name of the agnet
	g_string_sprintfa(gstr, " :name");
	if (desc->id != NULL) {
		temp = AIDToString(*desc->id);
		g_string_sprintfa(gstr, " %s", temp->str);
		g_string_free(temp, TRUE);
	}
	
	//the agents services
	g_string_sprintfa(gstr, " :services");
	for (i=0; i<desc->services->len; i++) {
		DFServiceDescription* service = g_array_index(desc->services, DFServiceDescription*, i);
		temp = DFServiceDescriptionToString(service);
		g_string_sprintfa(gstr, " %s", temp->str);
		g_string_free(temp, TRUE);
	}
	
	//the protocols
	g_string_sprintfa(gstr, " :protocols ");
	appendStringArray(&gstr, desc->protocols);	
	
	//the ontologies
	g_string_sprintfa(gstr, " :ontologies ");
	appendStringArray(&gstr, desc->ontologies);	
	
	//the languages
	g_string_sprintfa(gstr, " :languages ");
	appendStringArray(&gstr, desc->languages);		
	
	g_string_sprintfa(gstr, ")");
	return gstr;
}

/* creates a new DF description and initialises is so that its values can be set
 * 
 * returns - a newly allocated DF description that should be freed
 */
AgentDFDescription* DFDescNew() {
	AgentDFDescription* dfDesc = g_new(AgentDFDescription, 1);
	AgentDFDescriptionInit(dfDesc);
	return dfDesc;
}

//Getters
AID* DFDescGetAID(AgentDFDescription* desc) {
	return desc->id;
}
	
GArray* DFDescGetServices(AgentDFDescription* desc) {
	return desc->services;
}

GArray* DFDescGetLanguages(AgentDFDescription* desc) {
	return desc->languages;
}

GArray* DFDescGetOntologies(AgentDFDescription* desc) {
	return desc->ontologies;
}

GArray* DFDescGetProtocols(AgentDFDescription* desc) {
	return desc->protocols;
}

//setters
void DFDescAddProtocol(AgentDFDescription* desc, char* value) {
	GString* val = g_string_new(value);
	g_array_append_val(desc->protocols, val);
}

void DFDescAddOntology(AgentDFDescription* desc, char* value) {
	GString* val = g_string_new(value);
	g_array_append_val(desc->ontologies, val);
}

void DFDescAddLanguage(AgentDFDescription* desc, char* value) {
	GString* val = g_string_new(value);
	g_array_append_val(desc->languages, val);
}

void DFDescAddService(AgentDFDescription* desc, DFServiceDescription* value) {
	g_array_append_val(desc->services, value);
}

AgentDFDescription* SDFDescAddProtocol(AgentDFDescription* desc, char* value) {
	DFDescAddProtocol(desc, value);
	return desc;
}

AgentDFDescription* SDFDescAddOntology(AgentDFDescription* desc, char* value) {
	DFDescAddOntology(desc, value);
	return desc;
}

AgentDFDescription* SDFDescAddLanguage(AgentDFDescription* desc, char* value)  {
	DFDescAddLanguage(desc, value);
	return desc;
}

AgentDFDescription* SDFDescAddService(AgentDFDescription* desc, DFServiceDescription* value) {
	DFDescAddService(desc, value);
	return desc;
}

/***************************************************************************************
 * **************** DF SERVICE DESCRIPTIONS **********************************
 * **************************************************************************************/

/* converts a service description to a LISP like string representation that can be used
 * for testing and debugging
 * 
 * service - the entry that should be converted into a string
 * returns - GString for the string representation
 */
GString* DFServiceDescriptionToString(DFServiceDescription* service) {
	GString* gstr = g_string_new("(service-description");
	
	//the name of the service
	g_string_sprintfa(gstr, " :name");
	if (service->name != NULL) g_string_sprintfa(gstr, " %s", service->name->str);
	
	//the type of the service
	g_string_sprintfa(gstr, " :type");
	if (service->type != NULL) g_string_sprintfa(gstr, " %s", service->type->str);
	
	//the protocols
	g_string_sprintfa(gstr, " :protocols ");
	appendStringArray(&gstr, service->protocols);	
	
	//the ontologies
	g_string_sprintfa(gstr, " :ontologies ");
	appendStringArray(&gstr, service->ontologies);	
	
	//the languages
	g_string_sprintfa(gstr, " :languages ");
	appendStringArray(&gstr, service->languages);		
	
	g_string_sprintfa(gstr, ")");
	return gstr;
}

/* creates and initialises a new DF service description that can be used
 * 
 * returns - a newly allocated DF service description that should be freed
 */
DFServiceDescription* DFServiceDescriptionNew() {
	DFServiceDescription* service = g_new(DFServiceDescription, 1);
	DFServiceDescriptionInit(service);
	return service;	
}

//getters
GString* ServiceDescGetName(DFServiceDescription* desc) {
	return desc->name;
}

GString* ServiceDescGetType(DFServiceDescription* desc) {
	return desc->type;
}

GArray* ServiceDescGetProtocols(DFServiceDescription* desc) {
	return desc->protocols;
}

GArray* ServiceDescGetOntologies(DFServiceDescription* desc) {
	return desc->ontologies;
}

GArray* ServiceDescGetLanguages(DFServiceDescription* desc) {
	return desc->languages;
}

//setters
void ServiceDescAddProtocol(DFServiceDescription* desc, char* value) {
	GString* val = g_string_new(value);
	g_array_append_val(desc->protocols, val);
}

void ServiceDescAddOntology(DFServiceDescription* desc, char* value) {
	GString* val = g_string_new(value);
	g_array_append_val(desc->ontologies, val);
}

void ServiceDescAddLanguage(DFServiceDescription* desc, char* value) {
	GString* val = g_string_new(value);
	g_array_append_val(desc->languages, val);
}

DFServiceDescription* SServiceDescAddProtocol(DFServiceDescription* desc, char* value) {
	ServiceDescAddProtocol(desc, value);
	return desc;
}

DFServiceDescription* SServiceDescAddOntology(DFServiceDescription* desc, char* value) {
	ServiceDescAddOntology(desc, value);
	return desc;
}

DFServiceDescription* SServiceDescAddLanguage(DFServiceDescription* desc, char* value) {
	ServiceDescAddLanguage(desc, value);
	return desc;
}

