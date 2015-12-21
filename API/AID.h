/****************************************************************************************
 * Filename:	AID.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declarations of functions for creating and manipulating agent -identifier structures
 * **************************************************************************************/

#ifndef __AID_H__
#define __AID_H__

#include <glib.h>
#include "../platform-defs.h"

/************************ CREATION ********************************/
void AIDInit(AID* id);
AID* AIDNew();
void AIDFree(AID id);
void AIDReInit(AID* id);

/*********************** GETTERS **********************************/
GString AIDGetName(AID id);
GArray AIDGetAddresses(AID id);

/********************** SETTERS **********************************/
void AIDSetName(AID* id, char* name);
void AIDAddAddress(AID* id, char* address) ;

/****************** UTILITIES *************************************/
GString* AIDToString(AID id);
AID* AIDClone(AID);

/**************** SWIG ARTEFACTS *****************************/
AID* SAIDSetName(AID* id, char* name);
AID* SAIDAddAddress(AID* id, char* address);
GString* SAIDGetName(AID* id);
GArray* SAIDGetAddresses(AID* id);
GString* SAIDToString(AID* id);

#endif
