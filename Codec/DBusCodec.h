/****************************************************************************************
 * Filename:	DBusCodec.h
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Declaration of the DBus codec that is used by default for all transportation of 
 * data internal to the platform.  All functions occur in pairs one for encoding and
 * one for decoding.
 * **************************************************************************************/

#ifndef __DBUSCODEC_H__
#define __DBUSCODEC_H__

#include <dbus/dbus.h>
#include "../platform-defs.h"

void encodePlatformDescription(DBusMessageIter* iter, PlatformDescription* desc);
PlatformDescription* decodePlatformDescription(DBusMessageIter* iter);

void encodeAID(DBusMessageIter* iter, AID* aid);
AID* decodeAID(DBusMessageIter* iter);

void encodeReply(DBusMessageIter* iter, char* message);
GString* decodeReply(DBusMessageIter* iter);

void encodeAIDArray(DBusMessageIter* iter, GArray* array);
GArray* decodeAIDArray(DBusMessageIter* iter);

void encodeDFEntry(DBusMessageIter* iter, AgentDFDescription* entry);
AgentDFDescription* decodeDFEntry(DBusMessageIter* iter);

void encodeDFService(DBusMessageIter* iter, DFServiceDescription* service);
DFServiceDescription* decodeDFService(DBusMessageIter* iter);

void encodeString(DBusMessageIter* iter, GString* str);
GString* decodeString(DBusMessageIter* iter);

void encodeDFEntryArray(DBusMessageIter* iter, GArray* array);
GArray* decodeDFEntryArray(DBusMessageIter* iter);

void encodeAgentMessage(DBusMessageIter* iter, AgentMessage* msg);
AgentMessage* decodeAgentMessage(DBusMessageIter* iter);


#endif
