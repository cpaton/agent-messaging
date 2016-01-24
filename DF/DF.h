/****************************************************************************************
 * Filename:	DF.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declarartion of the functions used to implement the DF support service
 * **************************************************************************************/

#ifndef __DF_DF_H__
#define __DF_DF_H__

#include <glib.h>
#include <dbus/dbus.h>
#include "../platform-defs.h"

void DF_start(DBusConnection*, GMainLoop*, gchar*);
void DF_end();

void DF_registerEntry(AgentDFDescription* entry, APError* error);
int DF_entryExists(GString* name);
void DF_printDirectory();
GArray* DF_search(AgentDFDescription* template, APError* error);
gboolean matches(AgentDFDescription* entry, AgentDFDescription* template);

#endif
