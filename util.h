#ifndef __UTIL__
#define __UTIL__

#include <glib.h>
#include "platform-defs.h"
#include <unistd.h>

void myLogHandler(const gchar *domain, GLogLevelFlags level, const gchar* msg, 
	gpointer userData);
	
GString* getMachineName();
GString* stringArrayToString(GArray* array);
void appendStringArray(GString** str, GArray* array);

#endif
