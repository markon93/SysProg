#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "checkComLine.h"

/* Prints a string containing a common error message. */
void standardErrorMessage(){
	fprintf(stderr,"Usage: mfind [-t type] "
		"[-p nrthr] start1 [start2 ...] name\n");
}

/* Handles a variety of errors that may arise when calling mfind.
- errno: An integer value corresponding to the type of error that was made. The error codes are not sorted in any particular order.
*/
void errorHandler(int errno){
	if(errno == 1){
		fprintf(stderr, "Too few arguments.\n");
		standardErrorMessage();
		exit(1);
	}

	else if(errno == 2){
		fprintf(stderr,"Invalid type specified. "
		"Must be either 'd'(directory), 'f'(file) "
		"or 'l'(link).\n");
		exit(2);
	}

	else if (errno == 3){
		standardErrorMessage();
		exit(3);
	}

	else if(errno == 4){
		fprintf(stderr, "Invalid number of threads "
			"specified. Must be a non-zero positive "
			"integer.\n");
		exit(4);
	}
}

/* Checks the validity of the user input.
	- argc: The number of arguments
	- argv: string array with the arguments
	- returns: int value depending on which flags have been given:
		* 0 if no flags are given.
		* 1 if only type is given.
		* 2 if only nrthreads are given.
		* 3 if both type and nrthreads are given.
If any errors occur, they are passed to the error handler.
*/
int checkGivenFlags(int argc, char* argv[]){
	int errno;
	int mode;
	bool typeGiven = false, nrthreadsGiven = false;
	if(argc >= 3){
		for(int i = 0; i < argc; i++){
			if(!strcmp(argv[i], "-t")){
				if(i == 1){
					typeGiven = true;
				}
				else{
					errno = 3;
					errorHandler(errno);
				}
			}
			else if (!strcmp(argv[i], "-p")){
				if((!typeGiven && i == 1) ||
					(typeGiven && i == 3)){
					nrthreadsGiven = true;
				}
				else{
					errno = 3;
					errorHandler(errno);
				}
			}
		}

		// No flags given
		if(!typeGiven && !nrthreadsGiven){
			mode = checkNumberArguments(argc, 3, 0);
		}

		// Only -t given
		else if(typeGiven && !nrthreadsGiven){
			mode = checkNumberArguments(argc, 5, 1);
		}

		// Only -t given
		else if(!typeGiven && nrthreadsGiven){
			mode = checkNumberArguments(argc, 5, 2);
		}

		// Both flags given
		else {
			mode = checkNumberArguments(argc, 7, 3);
		}
	}
	else{
		errno = 1;
		errorHandler(errno);
	}
	return mode;
}

/*	Checks wheter a given number of arguments is valid for a specified command line setup.
	- argc: The number of arguments
	- mode: an integer value corresponding to the command line setup. See checkGivenFlags for interpretation.
	- minArgs: The minimum number of arguments required
		for the given the mode (e.g. 3 for mode 0)
*/
int checkNumberArguments(int argc, int minArgs, int mode){
	if(argc < minArgs){
		int errno = 1;
		errorHandler(errno);
	}
	return mode;
}

/* Checks whether a user supplied type has been correctly specified.
	- type: The user supplied type argument. Must be one of the following:
		* 'd' (directory)
		* 'f' (file)
		* 'l' (link)
	- returns: The type as a char, if it is valid. Invalid types are passed to the error handler.
*/
char checkType(char* type){
	int errno;
	if(strlen(type) != 1){
		errno = 2;
		errorHandler(errno);
	}

	char* validTypes = "fld";
	bool validType = false;
	for(int i = 0; i < NUM_VALID_TYPE_FLAGS; i++){
		if(strchr(type, validTypes[i])){
			validType = true;
			break;
		}
	}

	if(!validType){
		errno = 2;
		errorHandler(errno);
	}
	return type[0];
}

/* Checks whether a user supplied number of threads is valid.
	- nrthr: The user supplied argument.
	- returns: The number of threads, if correctly specified (i.e. a positive and non-zero integer.) Any error is passed to the error handler.
*/
int checkNrthr(char* nrthr){

	int errno;
  	char* p;

  	int intPart = (int) strtol(nrthr, &p, 10);
  	if(*p != '\0' || intPart <= 0){
    	errno = 4;
    	errorHandler(errno);
  	}
  	return intPart;
}

/* Puts a correctly formatted command line into a command structure.
	- argc: The number of arguments.
	- argv: Vector with the arguments.
	- mode: An integer value as specified in checkGivenFlags.
*/
command* getCommand(int argc, char* argv[], int mode){
	// Standard values, no flags given
	command* c;
	int nStarts = argc - 2;
	char t = 'f';
	int nrthr = 1;

	// Type specified
	if(mode == 1){
		nStarts = argc - 4;
		t = checkType(argv[2]);
	}

	// Number of threads specified
	else if(mode == 2){
		nStarts = argc - 4;
		nrthr = checkNrthr(argv[2]);
	}

	// Type and number of threads specified
	else if(mode == 3){
		nStarts = argc - 6;
		t = checkType(argv[2]);
		nrthr = checkNrthr(argv[4]);
	}

	// Place the correctly formatted command in a struct.
	c = malloc(sizeof(*c) + (nStarts)*sizeof(char*));
	c -> type = t;
	c -> nrthr = nrthr;
	c -> name = argv[argc - 1];
	c -> nStarts = nStarts;
	for (int i = 0; i < nStarts; i++){
		c -> start[i] = argv[argc - nStarts - 1 + i];
	}
	return c;
}
