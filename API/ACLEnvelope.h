/****************************************************************************************
 * Filename:	ACLEnvelope.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declarations of the getter and setter methods for the envelope structure used
 * by the interaction layer for message delivery.
 * **************************************************************************************/

#ifndef __API_ACLENVELOPE_H__
#define __API_ACLENVELOPE_H__

#include "API.h"

//setter methods
void ACLEnvelopeSetFrom(ACLEnvelope* msg, AID* id);
void ACLEnvelopeAddTo(ACLEnvelope* msg, AID* id);
void ACLEnvelopeSetACLRepresentation(ACLEnvelope* msg, char* value);
void ACLEnvelopeSetIntendedReceiver(ACLEnvelope* msg, AID* id);

//getter methods
AID* ACLEnvelopeGetFrom(ACLEnvelope* msg);
GArray* ACLEnvelopeGetTo(ACLEnvelope* msg);
GString* ACLEnvelopeGetACLRepresentation(ACLEnvelope* msg);
AID* ACLEnvelopeGetIntendedReceiver(ACLEnvelope* msg);
int ACLEnvelopeHasIntendedReceiver(ACLEnvelope* msg);

//utility functions
GString* ACLEnvelopeToString(ACLEnvelope* msg);

#endif
