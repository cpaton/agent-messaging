/****************************************************************************************
 * Filename:	MTS.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declaration of the functions that implement the interaction layer funcionality within
 * the agent platform
 * **************************************************************************************/

#ifndef __MTS__MTS__
#define __MTS__MTS__

#include <dbus/dbus.h>
#include <glib.h>
#include <dbus/dbus-glib.h>
#include "../platform-defs.h"

void MTS_start(DBusConnection*, GMainLoop*, gchar*);
void MTS_end();

#endif
