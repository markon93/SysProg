/*	mish.c: An implementation of a simple shell. There are two internal
	command implementations, echo and cd, which behave similarly as the
	internal commands in a unix shell. The shell can also handle the
	execution of external commands, redirecting their input and output,
	and using pipes for communication between them.

	Author: Marko Nygård (oi12mnd)
	Laboration 3 Systemnära Programmering HT17
*/

#define _GNU_SOURCE
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include "sighant.h"
#include "execute.h"

int signalReceived = 0;

/* Echo: Displays all of its arguments to stdout
   - argc: the number of arguments
   - argv: string array with the arguments
*/
void echo(int argc, char* argv[]){
	for(int i = 1; i < argc; i++){
			printf("%s ", argv[i]);
	}
	printf("\n");
}

/* Change the current working directory
   - newDirectory: the directory to change to
*/
void cd(char* newDirectory){
	if(chdir(newDirectory) != 0){
		perror(newDirectory);
	}
}

/* Write out a prompt */
void writePrompt(){
	fprintf(stderr, "mish%% ");
	if(fflush(stderr) != 0){
		perror("");
		exit(1);
	}
}

/* Get the full command */
int getCommands(command commandLine[]){
	int numberOfCommands = -1;

	// Scan the command line
	char input[MAXLINELEN + 1];
	if(fgets(input, MAXLINELEN + 1, stdin) == NULL){
		fprintf(stderr,"Error reading the command.\n");
	}

	else{
		// Parse the command line
		numberOfCommands = parse(input, commandLine);
	}

	return numberOfCommands;
}

/* Check if it is an internal command */
bool isInternal(char* firstCommand){
	if(!strcmp(firstCommand, "echo") || !strcmp(firstCommand, "cd")){
		return true;
	}
	else{
		return false;
	}
}

/* Execute internal commands, i.e. cd and echo */
void executeInternal(int numberOfCommands, command commandLine[]){
	command c = commandLine[0];
	if(!strcmp(c.argv[0],"echo")){
		echo(c.argc, c.argv);
	}
	else if(!strcmp(c.argv[0], "cd")){
		cd(c.argv[1]);
	}
}

/* Create a number of pipes */
void createPipes(){

}

/* Execute external commands */
void executeExternal(int numberOfCommands, command commandLine[]){

	createPipes();

}



/* Execute commands given by the user. */
void executeCommands(int numberOfCommands, command commandLine[]){
	if (numberOfCommands > 0){
			if(isInternal(commandLine[0].argv[0])){
				executeInternal(numberOfCommands, commandLine);
			}

			else{
				executeExternal(numberOfCommands, commandLine);
			}
	}



	// Enter pressed -> new prompt.
	else{
		return;
	}
}


int main(int argc, char* argv[]){
	// Create a signal handler
  signal(SIGINT, sighant);

	// Main loop
	while(true){
		signalReceived = 0;
		writePrompt();
		command commandLine[MAXCOMMANDS + 1];
		int numberOfCommands = getCommands(commandLine);
		if (numberOfCommands == -1){
			exit(1);
		}
		else{
			executeCommands(numberOfCommands, commandLine);
		}
	}

	return 0;
}
