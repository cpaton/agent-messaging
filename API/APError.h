/****************************************************************************************
 * Filename:	APError.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declaration of the functions used to create and manipulate error structures used
 * by the API.  Also contains the definitions of the error messages used by the
 * platform.
 * **************************************************************************************/

#ifndef __API_APERROR_H__
#define __API_APERROR_H__

#include "../platform-defs.h"

//creation and deletion
void APErrorInit(APError* err);
APError* APNewError();
void APErrorFree(APError* err);
void APErrorReInit(APError* err);

//utilities
gboolean APErrorIsSet(APError err);
void APSetError(APError* err, char* number);

/*************** SWIG ARTEFACTS ************************************/
int SAPErrorIsSet(APError* err);
GString* SAPErrorGetError(APError* err);

/**************************************************************************************
 * ********************* ERROR DEFINITION **************************************
 * ************************************************************************************/
#define ERROR_NULL "Null pointer sent"
#define ERROR_REQUIRED_FIELD_MISSING "Required Field Missing"
#define ERROR_CONNECTION_UNSUCCESSFUL "Unable to connect to the D-Bus"
#define ERROR_PLATFORM_NOT_FOUND "The platform could not be found"
#define ERROR_COULD_NOT_CONTACT_PLATFORM "The platform could not be contacted"
#define ERROR_INVALID_REPLY "Invalid format of reply message"
#define ERROR_SERVICE_NOT_ACQUIRED "Could not acquire agent service"
#define ERROR_INVALID_AGENT_NAME "Invalid agent name"
#define ERROR_DUPLICATE_AGENT "Agent already exists with that name"
#define ERROR_MESSAGE_LISTENER "Unable to register agent message listener"
#define ERROR_AGENT_NOT_FOUND "No agent with that name was found"
#define ERROR_AGENT_DOES_NOT_EXIST "The given agent does not exist"
#define ERROR_UNAUTHORISED_AMS "You don't have the authority to perform that action - not registered with AMS"
#define ERROR_DUPLICATE "Duplicate entries are not allowed"
#define ERROR_ENTRY_NOT_FOUND "No entry was found for that agent"

#define ERROR_MUST_HAVE_RECEIVER "Message must have at least on receiver"
#define ERROR_PERFORMATIVE_REQUIRED "Performative required"

#define RETURN_OK "ok"

#endif
