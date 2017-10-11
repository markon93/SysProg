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


// Get type if it is specified
/*char checkType(command* c, ){
	
	if(!strcmp(argv[1], "-t")){

		int errno; 
		// Not enough arguments
		if(argc < 5){
			errno = 1;
			errorHandler(errno);
			exit(2);
		}

		// Invalid flag length (should be 1)
		if(strlen(argv[2]) != 1){
			errno = 2;
			errorHandler(errno);
			exit(3);
		}

		// Invalid flag symbol
		char validFlags[NUM_VALID_TYPE_FLAGS] = {'d', 'f', 'l'};
		for(int i = 0; i < NUM_VALID_TYPE_FLAGS; i++){
			if (*argv[2] == validFlags[i]){
				printf("Type specified: %s\n", argv[2]);
				return *argv[2];
			}
		}

		errno = 2;
		errorHandler(errno);
		exit(3);

	} 
	else{
		return 'f';
	}
}
*/

/*	Check which command and flags were used

*/
void getCommand(int argc, char* argv[]){
	
}


int main(int argc, char* argv[]){
	printf("Welcome!\n");
	int mode = checkGivenFlags(argc, argv);
	fprintf(stdout, "Mode: %d\n", mode);

	command* c = malloc(sizeof(*c) + (argc - 2)*sizeof(char*));

	if(mode == 0){
		c -> type = 'f';
		c -> nrthr = 1;
		c -> name = argv[argc - 1];
		int numStarts = argc - 2;
		for (int i = 0; i < numStarts; i++){
			c -> start[i] = argv[i + 1];
		}
	}

	printf("%d\n", c -> nrthr);
	printf("%c\n", c -> type);
	printf("%s\n", c -> name);
	for(int j = 0; j < argc - 2; j++){
		printf("%s\n", c -> start[j]);
	}

	free(c);
	return 0;
}