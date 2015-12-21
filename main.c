/****************************************************************************************
 * Filename:	main.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Driver program for the agent platform that was used for testing the funcionality
 * it takes a command line arguement which is passed to the test driver to determine
 * which test should be run.  If no arguement is given then it bootstraps the platform
 * **************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include "API/API.h"
#include "Tests/tests.h"
#include "util.h"

int main(int argc, char** argv) {
	//set up the log file
	g_log_set_handler(NULL,  G_LOG_LEVEL_MASK, myLogHandler, NULL);	
	
	//set the environment variable for the connection to the D-Bus
	FILE* fle;
	fle = fopen("./address.txt", "r");
	if (fle != NULL) {
		char address[255];
		fscanf(fle, "%s", address);
		setenv("DBUS_SESSION_BUS_ADDRESS",address, 0);
	}
	
	//now determine what test we should run
	if (argc >= 2) {
		runTests(argv);		
	}	
	else {		
		//there is no test specified so just run the normal platform
		bootstrapPlatform();		
	}
	
	return EXIT_SUCCESS;
}
