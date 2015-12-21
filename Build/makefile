ROOT = 
SOURCE_ROOT = ../

AMS_OBJS = ${addprefix AMS/, AMS.o}
CODEC_OBJS = ${addprefix Codec/, DBusCodec.o}
DBUS_OBJS = ${addprefix DBus/, DBus-utils.o}
DF_OBJS = ${addprefix DF/, DF.o}
MTS_OBJS = ${addprefix MTS/, MTS.o}
API_OBJS = ${addprefix API/, ACLEnvelope.o ACLMessage.o AID.o APError.o DFAPI.o platform.o agent.o}
TEST_OBJS = ${addprefix Tests/, test-agents.o test-utils.o tests.o}
ROOT_OBJS = platform-defs.o util.o main.o

DIRS = AMS API Codec DBus DF MTS Tests

OBJS = ${addprefix $(ROOT), $(AMS_OBJS) $(CODEC_OBJS) $(DBUS_OBJS) $(DF_OBJS) $(MTS_OBJS) $(API_OBJS) $(TEST_OBJS) $(ROOT_OBJS)}
#OBJS = ${addprefix $(ROOT), $(ROOT_OBJS) $(AMS_OBJS)}

LIBS = `pkg-config --libs glib-2.0` `pkg-config --libs dbus-glib-1`
CC = gcc
CFLAGS = `pkg-config --cflags glib-2.0` `pkg-config --cflags dbus-glib-1` -DDBUS_API_SUBJECT_TO_CHANGE

all: Platform
	@echo Build Complete

Platform: $(OBJS)
	$(CC) -oPlatform $(OBJS) $(CFLAGS) $(LIBS)
	
%.o : 
	$(CC) -c $(SOURCE_ROOT)$(@D)/$(*F).c -o$(ROOT)$(@D)/$(*F).o $(CFLAGS)

clean: 
	rm $(OBJS)
	rm Platform

dirs: 
	mkdir $(DIRS)
	