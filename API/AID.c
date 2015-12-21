/****************************************************************************************
 * Filename:	AID.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implementation of the agent-identifier strucutre required to be used by the platform
 * for unique identification of agents during conversations.  Functions are provided
 * for creation and manipulation of the internal strucutre used.
 * **************************************************************************************/

#include "AID.h"
#include "../platform-defs.h"
#include "../util.h"

/* Initialises the given id structure by setting all elements to their initial values.  The value
 * passed in must not be null
 * 
 * id - the agnet-identifier structure that needs initialising
 */
void AIDInit(AID* id) {
	id->name = NULL;	
	//initialise the arrays to hold nothing originally
	id->addresses = g_array_new(FALSE, FALSE, sizeof(GString*));
}

/* creates a new agent-identifier structure and initialises the values.  Should be called
 * whenever a new agent-identifier is required
 * 
 * returns - a newly allocated agent identifier that should be freed
 */
AID* AIDNew() {
	AID* id = g_new(AID, 1);
	AIDInit(id);
	return id;
}

/* Frees all of the memory in the given agent-identifier, the value passed in cannot be
 * null
 * 
 * id - the agent-identifier to be freed
 */
void AIDFree(AID id) {
	if (id.name != NULL) g_string_free(id.name, TRUE);
	g_array_free(id.addresses, TRUE);
}

/* Allows a given agent-identifier structure to be re-used by freeing all memory and
 * reinitialising it - the value passed in cannot be null
 * 
 * id - the agent-identifier that you want to use again
 */
void AIDReInit(AID* id) {
	AIDFree((*id));
	AIDInit(id);
}

//getter functions
GString AIDGetName(AID id) { return (*id.name); }
GArray AIDGetAddresses(AID id) { return (*id.addresses); }

//setter functions
void AIDSetName(AID* id, char* name) {
	id->name = g_string_new(name);
}

void AIDAddAddress(AID* id, char* address) {
	GString* temp = g_string_new(address);
	g_array_append_val(id->addresses, temp);
}

/* Coverts a given agent-identifier into a lisp like string format
 * 
 * id - the agent-indentifier that you wish converting that cannot be null
 */
GString* AIDToString(AID id) {
	GString* gstr = g_string_new("(agent-identifier");
		
	//add the name
	g_string_sprintfa(gstr, " :name");
	if (id.name != NULL)
		g_string_sprintfa(gstr, " %s", id.name->str);	
	
	//add the addresses
	
	g_string_sprintfa(gstr, " :addresses");
	if (id.addresses->len > 0) {
		g_string_sprintfa(gstr, " (");
		appendStringArray(&gstr, id.addresses);
		g_string_sprintfa(gstr, ")");
	}	
	
	g_string_sprintfa(gstr, ")");
	return gstr;
}

/* clones and agent-identifier strucutre so that all the information remains
 * but it is now in a new memory space
 * 
 * id - the strucutre that you want copying
 * returns - the new structure with the same information but in a new memory space
 */
AID* AIDClone(AID id) {
	AID* newAID = g_new(AID, 1);
	AIDInit(newAID);
	
	//first of all copy the name across
	GString* temp;
	if (id.name != NULL) {
		temp = g_string_new(id.name->str);
		newAID->name = temp;
	}
	
	//copy all of the addresses accross
	int i;
	for (i=0; i < id.addresses->len; i++) {
		temp = g_array_index(id.addresses, GString*, i);
		AIDAddAddress(newAID, temp->str);
	}
	
	return newAID;
}

/************ SWIG IMPLEMENTATION ARTEFACTS *************************/
/* wrapper functions written for the above getters and setters that remove the pasing
 * by reference that is not support by SWIG for complex data types.  Ideally these
 * would have been re written above but this was not possible due to time constraints
 */
AID* SAIDSetName(AID* id, char* name) {
	AIDSetName(id, name);
	return id;
}

AID* SAIDAddAddress(AID* id, char* address) {
	AIDAddAddress(id, address);
	return id;
}

GString* SAIDGetName(AID* id) {
	return id->name;
}

GArray* SAIDGetAddresses(AID* id) {
	return id->addresses;
}

GString* SAIDToString(AID* id) {
	GString* gstr;
	gstr = AIDToString(*id);	
	return gstr;
}




