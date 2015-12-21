/****************************************************************************************
 * Filename:	AMS.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Function declarations for the AMS support service
 * **************************************************************************************/

#ifndef _AGENTS_AMS_H__
#define _AGENTS_AMS_H__

#include <glib.h>
#include <dbus/dbus.h>
#include "../platform-defs.h"
#include "../API/API.h"

/********* BOOTSTRAP FUNCTIONS ***********************/
void AMS_start(DBusConnection*, GMainLoop*, gchar*);
void AMS_end();

/********* FUNCTIONS CALLABLE BY OTHER PLATFORM SERVICES **********/
void AMS_register(AID* id, APError* err);
void AMS_deRegister(char* name, APError* err);
int AMS_agentExists(GString* name);

/********* TEST FUNCTIONS **********************/
void AMS_printDirectory();
#endif

