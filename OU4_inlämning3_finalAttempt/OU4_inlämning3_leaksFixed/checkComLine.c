#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include "queue.h"
#include "checkComLine.h"

/* Prints a string containing a common error message. */
void standardErrorMessage(){
	fprintf(stderr,"Usage: mfind [-t type] "
		"[-p nrthr] start1 [start2 ...] name\n");
}

/* Puts a correctly formatted command line into a command structure.
	- argc: The number of arguments.
	- argv: Vector with the arguments.
	- mode: An integer value as specified in checkGivenFlags.
*/
command* getCommand(int argc, char* argv[]){
	command* c;
	c = malloc(sizeof(*c));
	c -> type = 'a';
	c -> nrthr = 1;
	int flag;
	while((flag = getopt(argc, argv, "p:t:")) != -1){
		switch(flag){

			// Type specified
			case 't':

				if(strlen(optarg) == 1){
					if(!strcmp(optarg, "d")||!strcmp(optarg, "l")||!strcmp(optarg, "f")){
						c -> type = *(char*) optarg;
					}
					else{
						fprintf(stderr, "Type must be either 'd' (directory), 'l'"
						" (symbolic link) or 'f' (file)\n");
						exit(1);
					}
				}
				else{
					standardErrorMessage();
					exit(1);
				}
				break;

			// Number of threads specified
			case 'p':;
				char* p;
				int intPart = strtol(optarg, &p, 10);
				if(intPart > 0 && *p == '\0'){
					c -> nrthr = intPart;
				}
				else{
					fprintf(stderr, "Number of threads must be an integer value "
						"larger than 0.\n");
					exit(1);
				}
				break;
			case '?':
				standardErrorMessage();
				exit(1);
				break;
			default:
				standardErrorMessage();
				exit(1);
		}
	}
	queue* dirQueue = q_create();
	queue_setMemHandler(dirQueue, free);

	for (int i = optind; i < argc - 1; i++){
		 char* temp = malloc(strlen(argv[i]) + 1);
		 strcpy(temp, argv[i]);
		 q_enqueue(dirQueue, (void*)temp);
	}
	c -> dirQueue = dirQueue;
	c -> name = argv[argc - 1];
	return c;
}
