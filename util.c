/****************************************************************************************
 * Filename:	util.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Utility functions that are used throughout the implementation, used to perform
 * some common tasks required in multiple places.
 * **************************************************************************************/

#include "util.h"
#include <stdio.h>
#include "platform-defs.h"

/* sets the default log handler for all messages written by the entire platform and API
 * it simply echoes messages to the terminal
 * 
 * see the GLib doucmentation for more information about the parameters sent
 * to this function
 */
void myLogHandler(const gchar *domain, GLogLevelFlags level, const gchar* msg, gpointer userData) {
	//for now just print the messages to the console
	printf("%s\n", msg);
}

/* gets the name of the machine on which the code is running
 * 
 * returns - the fully qualified domain name on which the program is running
 */
GString* getMachineName() {
	gchar hostName[255];
	gchar domainName[255];
	int retVal;
	
	retVal = gethostname(hostName, 255);
	if (retVal != 0) { 
		g_error("Unable to determine the host name received error %d", retVal);
		return NULL;
	}
	retVal = getdomainname(domainName, 255);	
	if (retVal != 0) { 
		g_error("Unable to determine the host name received error %d", retVal);
		return NULL;
	}
	
	GString* str = g_string_new(hostName);
	//check to see whether we hav received (none)
	if (g_strcasecmp(domainName, "(none)") != 0)
		g_string_sprintfa(str, ".%s", domainName);
	return str; 	
}

/* converts and array of strings into a single space serparated string
 * 
 * array - the array of strings to be converted
 * returns - a single containing all of the elements of the array separated by a space
 */
GString* stringArrayToString(GArray* array) {
	int i;
	GString* str = g_string_new("");
	for (i=0; i<array->len; i++) {
		GString* temp = g_array_index(array, GString*, i);
		g_string_sprintfa(str, "%s ", temp->str);
	}
	return str;
}

/* adds the contents of an array of strings to the end of a string
 * 
 * str - the string to which the array should be appended
 * array - the array of strings that should be appended to the string
 */
void appendStringArray(GString** str, GArray* array) {
	GString* temp = stringArrayToString(array);
	g_string_sprintfa((*str), "%s", temp->str);
	g_string_free(temp, TRUE);
}
