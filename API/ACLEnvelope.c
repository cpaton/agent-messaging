/****************************************************************************************
 * Filename:	ACLEnvelope.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implementation of the envelope structure used by the interaction layer for making
 * decisions about where to route an agent message to
 * **************************************************************************************/

#include "ACLEnvelope.h"
#include "../platform-defs.h"
#include "API.h"

/************* SETTER METHODS **************************/
void ACLEnvelopeSetFrom(ACLEnvelope* msg, AID* id) {
	msg->from = id;
}

void ACLEnvelopeAddTo(ACLEnvelope* msg, AID* id) {
	g_array_append_val(msg->to, id);
}

void ACLEnvelopeSetACLRepresentation(ACLEnvelope* msg, char* value) {
	msg->aclRepresentation = g_string_new(value);
}

void ACLEnvelopeSetIntendedReceiver(ACLEnvelope* msg, AID* id) {
	msg->intendedReceiver = id;
}

/***************** GETTER METHODS ****************************/
AID* ACLEnvelopeGetFrom(ACLEnvelope* msg) { return msg->from; }
GArray* ACLEnvelopeGetTo(ACLEnvelope* msg) { return msg->to; }
GString* ACLEnvelopeGetACLRepresentation(ACLEnvelope* msg) { return msg->aclRepresentation; }
AID* ACLEnvelopeGetIntendedReceiver(ACLEnvelope* msg) { return msg->intendedReceiver; }
int ACLEnvelopeHasIntendedReceiver(ACLEnvelope* msg) { 
	if (msg->intendedReceiver == NULL) 
		return FALSE;
	else
		return TRUE;
}

/***************** UTILITY FUNCTIONS **************************/

/* Converts an envelope structure into a string representation.  This is only designed
 * to be used for debugging and testing purposes to print the envelope to the screen
 * it does not have strict enough structure to be used for transportation and parsing
 * at the other end.  The envelope is transformed in a LISP like string.
 * 
 * msg - the envelope that you want to convert to a string
 * returns - a GString object that thas the representation of the envelope
 */
GString* ACLEnvelopeToString(ACLEnvelope* msg) {
	GString* gstr = g_string_new("(\n");
	GString* temp;
	
	if (msg->from != NULL) {
		temp = AIDToString((*msg->from));
		g_string_sprintfa(gstr, ":from %s\n", temp->str);
		g_string_free(temp, TRUE);
	}
	
	if (msg->to->len > 0) {
		int i;
		g_string_sprintfa(gstr, ":to ");
		for (i=0; i<msg->to->len; i++) {
			AID* id = g_array_index(msg->to, AID*, i);
			temp = AIDToString(*id);
			g_string_sprintfa(gstr, " %s", temp->str);
			g_string_free(temp, TRUE);
		}
		g_string_sprintfa(gstr, "\n");
	}
	
	if (msg->aclRepresentation != NULL) g_string_sprintfa(gstr, ":acl-representation %s\n", msg->aclRepresentation->str);
	
	if (msg->intendedReceiver != NULL) {
		temp = AIDToString((*msg->intendedReceiver));
		g_string_sprintfa(gstr, ":intended-receiver %s\n", temp->str);
		g_string_free(temp, TRUE);
	}
	
	g_string_sprintfa(gstr, ")");
	return gstr;
}
