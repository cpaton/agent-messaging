/****************************************************************************************
 * Filename:	DBusCodec.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declarations of fuctions used to help with interaction with the DBus system
 * **************************************************************************************/

#ifndef __DBUS_DBUS_UTILS_H__
#define __DBUS_DBUS_UTILS_H__

#include <glib.h>
#include <dbus/dbus.h>

GString buildTransportAddress(char* service, char* path, char* method);
gboolean getService(DBusConnection* conn, char* serviceName);
GString* getBaseService(DBusConnection* conn);
DBusConnection* getDBusConnection();

#endif
