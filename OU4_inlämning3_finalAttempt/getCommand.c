#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"
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
int checkGivenFlags(int argc, char* argv[], command c){
	while ((opt = getopt(argc, argv, "t:p:")) != -1) {
		if(opt == 't'){
			char type = checkType(opt);
			c -> type = type;
		}
		else if(opt == 'p'){
			int nrthr = checkNrthr(opt);
			c -> nrthr = nrthr;
		}

		else{
			fprintf(stderr,"Invalid flag.\n");
			standardErrorMessage();
			exit(5);
		}
	}
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

	char* validTypes = "afld";
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

	int nStarts = argc - 2;
	command* c = malloc(sizeof(*c) + (nStarts)*sizeof(char*));

	// Place the correctly formatted command in a struct.
	c -> nrthr = nrthr;
	c -> name = argv[argc - 1];
	c -> nStarts = nStarts;
	queue* dirQueue = q_create();
	for (int i = 0; i < nStarts; i++){
		 q_enqueue(dirQueue, argv[argc - nStarts - 1 + i]);
	}
	c -> dirQueue = dirQueue;

	return c;
}
