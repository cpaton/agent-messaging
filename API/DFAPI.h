/****************************************************************************************
 * Filename:	DFAPI.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declarations of functions that manipulate the ontology objects used in conversations
 * with the DF
 * **************************************************************************************/

#ifndef __API_DF_H__
#define __API_DF_H__

#include "../platform-defs.h"

/************************* UTILITY FNS *****************************************/
GString* AgentDFDescriptionToString(AgentDFDescription* desc);
GString* DFServiceDescriptionToString(DFServiceDescription* service);

/********************* AGENT DF DESCRIPTIONS ******************************/
AgentDFDescription* DFDescNew();

AID* DFDescGetAID(AgentDFDescription* desc);
GArray* DFDescGetServices(AgentDFDescription* desc);
GArray* DFDescGetLanguages(AgentDFDescription* desc);
GArray* DFDescGetOntologies(AgentDFDescription* desc);
GArray* DFDescGetProtocols(AgentDFDescription* desc);

void DFDescAddProtocol(AgentDFDescription* desc, char* value);
void DFDescAddOntology(AgentDFDescription* desc, char* value);
void DFDescAddLanguage(AgentDFDescription* desc, char* value);
void DFDescAddService(AgentDFDescription* desc, DFServiceDescription* value);

/**************** SWIG SETTERS **********************************/
AgentDFDescription* SDFDescAddProtocol(AgentDFDescription* desc, char* value);
AgentDFDescription* SDFDescAddOntology(AgentDFDescription* desc, char* value);
AgentDFDescription* SDFDescAddLanguage(AgentDFDescription* desc, char* value);
AgentDFDescription* SDFDescAddService(AgentDFDescription* desc, DFServiceDescription* value);

/********************* DF SERVICE DESCRIPTION ****************************/
DFServiceDescription* DFServiceDescriptionNew();

GString* ServiceDescGetName(DFServiceDescription* desc);
GString* ServiceDescGetType(DFServiceDescription* desc);
GArray* ServiceDescGetProtocols(DFServiceDescription* desc);
GArray* ServiceDescGetOntologies(DFServiceDescription* desc);
GArray* ServiceDescGetLanguages(DFServiceDescription* desc);

void ServiceDescAddProtocol(DFServiceDescription* desc, char* value);
void ServiceDescAddOntology(DFServiceDescription* desc, char* value);
void ServiceDescAddLanguage(DFServiceDescription* desc, char* value);

/****************** SWIG SETTERS **********************************************/
DFServiceDescription* SServiceDescAddProtocol(DFServiceDescription* desc, char* value);
DFServiceDescription* SServiceDescAddOntology(DFServiceDescription* desc, char* value);
DFServiceDescription* SServiceDescAddLanguage(DFServiceDescription* desc, char* value);

#endif
