ROOT = ../Build/

AMS_OBJS = ${addprefix AMS/, AMS.o}
CODEC_OBJS = ${addprefix Codec/, DBusCodec.o}
DBUS_OBJS = ${addprefix DBus/, DBus-utils.o}
DF_OBJS = ${addprefix DF/, DF.o}
MTS_OBJS = ${addprefix MTS/, MTS.o}
API_OBJS = ${addprefix API/, ACLEnvelope.o ACLMessage.o AID.o APError.o DFAPI.o platform.o agent.o}
TEST_OBJS = ${addprefix Tests/, test-agents.o test-utils.o tests.o}
ROOT_OBJS = platform-defs.o util.o

OBJS = *.o ${addprefix $(ROOT), $(AMS_OBJS) $(CODEC_OBJS) $(DBUS_OBJS) $(DF_OBJS) $(MTS_OBJS) $(API_OBJS) $(TEST_OBJS) $(ROOT_OBJS)}

LIBS = -lglib-2.0 -ldbus-glib-1

CFLAGS = `pkg-config --cflags glib-2.0` `pkg-config --cflags dbus-glib-1` -DDBUS_API_SUBJECT_TO_CHANGE

all: Java

Java: cat.txt
	swig -java -outdir ./Java agentPlatform.i
	gcc -c *.c $(CFLAGS)
	ld -shared -o libagentPlatform.so $(OBJS) $(LIBS)
	javac ./Java/*.java
	
cat.txt:
	echo here
	
JavaClean:
	rm Java/*.class
	