/****************************************************************************************
 * Filename:	tests.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declarations of functions used for testing the platforms capabilities
 * **************************************************************************************/

#ifndef __TEST_H__
#define __TEST_H__

#include <dbus/dbus.h>
#include <glib.h>
#include "test-agents.h"
#include "test-utils.h"

void runTests(char** argv);

//util fns
DBusConnection* getConnection();

void AIDTest();
void ACLTest();
void sendTestMessage(DBusConnection* cn_conn, gchar* service, gchar* path, gchar* method);
void dfSearch();

#endif
