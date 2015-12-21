/****************************************************************************************
 * Filename:	APError.c
 * Author:		Craig Paton
 * Date:			Apr 2004
 * 
 * Implementation of the APError structure that is used to hold error information
 * for calls made to the API.  Constants for the error messages used can be
 * found in the header file.
 * **************************************************************************************/

#include "APError.h"

/* Initialises the error structure
 * 
 * err - the error that needs initialising
 */
void APErrorInit(APError* err) {
	if (err == NULL) err = g_new(APError, 1);
	err->message = NULL;
	err->number = -1;
}

/* Frees and error strucure
 * 
 * err - the error that you no longer wish to use
 */
void APErrorFree(APError* err) {
	if (err->message == NULL) g_string_free(err->message, TRUE);	
}

/* Re initialisers an error so that it can be used again
 * 
 * err - the error that you want to use again
 */
void APErrorReInit(APError* err) {
	APErrorFree(err);
	APErrorInit(err);
}

/* checks to see if an error has been set
 * 
 * err - the error you wish to check
 * returns - TRUE if the structure has an error message FALSE otherwise
 */
gboolean APErrorIsSet(APError err) {
	if (err.number != -1) return TRUE;
	return FALSE;
}

/* sets an error structure to contain the message for the associated error
 * 
 * err - the structure that should be used to hold the error message
 * number - the message
 */
void APSetError(APError* err, char* number) {
	if (err != NULL) {
		err->number = 1;
		err->message = g_string_new(number);
	}
}

/* creates new error structure that can be used to for calls to the API functions
 * 
 * returns - a newly allocated and initialised error structure
 */
APError* APNewError() {
	APError* error = g_new(APError, 1);
	APErrorInit(error);
	return error;
}

/**************** SWIG IMPLEMENTATION ATREFACTS **********************/
/* wrapper functions for some of the functionality above that removes the call
 * by reference which SWIG cannot cope with
 */
int SAPErrorIsSet(APError* err) {
	if (APErrorIsSet(*err))
		return 1;
	else
		return 0;
}

GString* SAPErrorGetError(APError* err) {
	return err->message;
}
