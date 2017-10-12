#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#define NUM_VALID_TYPE_FLAGS 3

// Command line arguments
typedef struct command{
	char type;
	int nrthr;
	char* name;
	char* start[];
} command;

// Prints out a common error message
void standardErrorMessage(){
	fprintf(stderr,"Usage: mfind [-t type] "
		"[-p nrthr] start1 [start2 ...] name\n");
}

// Prints a relevant error message
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

/*	Checks wheter a given number of arguments is valid
	- argc: The number of arguments
	- flagValue: The return value of a certain mode
	- minArgs: The minimum number of arguments required 
		corresponding to the mode
*/
int checkNumberArguments(int argc, int minArgs, int flagValue){
	if(argc < minArgs){
		int errno = 1;
		errorHandler(errno);
	}
	return flagValue;
}

/* Checks the validity of the user input.
	- argc: The number of arguments
	- argv: string array with the arguments
	- returns: int value depending on which flags have been given:
		* 0 if no flags are given
		* 1 if only type is given
		* 2 if only nrthreads are given
		* 3 if both type and nrthreads are given
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


/* 
	Check if the type is correctly specified. The error 
	handler function is invoced if the type is not valid.
	- type: The user supplied type
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

/**/
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

/**/
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
	for (int i = 0; i < nStarts; i++){
		c -> start[i] = argv[argc - nStarts - 1 + i];
	}
	return c;
}

/* Main */
int main(int argc, char* argv[]){
	
	int mode = checkGivenFlags(argc, argv);
	command* c = getCommand(argc, argv, mode);
	
	free(c);
	return 0;
}