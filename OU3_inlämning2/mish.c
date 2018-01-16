/*	mish.c: An implementation of a simple shell. There are two internal
	command implementations, echo and cd, which behave similarly as the
	internal commands in a unix shell. The shell can also handle the
	execution of external commands, redirecting their input and output,
	and using pipes for communication between them.

	Author: Marko Nygård (oi12mnd)
	Laboration 3 Systemnära Programmering HT17
	Date: 6.10.2017
	Update: Split main into more functions + everything working properly.
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

/* Write out a prompt */
void writePrompt(){
	fprintf(stderr, "mish%% ");
	if(fflush(stderr) != 0){
		perror("");
		exit(1);
	}
}

/* Scan and parse all of the commands supplied by the user.
	- commandLine: Empty vector of proper length to hold the commands
	- returns: The number of commands if OK, -1 on error
*/
int getCommands(command commandLine[]){
	int numberOfCommands = -1;

	// Scan and parse the command line
	char input[MAXLINELEN + 1];
	if(fgets(input, MAXLINELEN + 1, stdin) != NULL){
		numberOfCommands = parse(input, commandLine);
	}

	// The input is NULL after terminating mish with ctrl+D; a new line makes
	// it look a little nicer.
	else{
		fprintf(stderr,"\n");
	}

	return numberOfCommands;
}

/* Check if the user supplied an internal command.
	- firstCommand: The first of the user commands.
	- returns: true if it is an internal command, false otherwise.
*/
bool isInternal(char* firstCommand){
	if(!strcmp(firstCommand, "echo") || !strcmp(firstCommand, "cd")){
		return true;
	}
	else{
		return false;
	}
}

/* Execute internal commands, i.e. cd and echo.
	- numberOfCommands: The number of commands.
	- commandLine: Vector with all the commands.
*/
void executeInternal(int numberOfCommands, command commandLine[]){
	command c = commandLine[0];
	if(!strcmp(c.argv[0],"echo")){
		echo(c.argc, c.argv);
	}
	else if(!strcmp(c.argv[0], "cd")){
		cd(c.argv[1]);
	}
}

/* Echo: Displays all of its arguments to stdout
   - argc: the number of arguments
   - argv: string array with the arguments
*/
void echo(int argc, char* argv[]){
	for(int i = 1; i < argc; i++){
		printf("%s", argv[i]);
		if(i != argc - 1){
			printf(" ");
		}
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

/* Create a specified number of pipes.
	- numberOfPipes: The number of pipes to create.
	- pipeArray: An empty n-by-2 array which will hold the pipe ends, i.e.
	the file descriptors of the read and write ends of all the pipes.
*/
void createPipes(int numberOfPipes, int pipeArray[][2]){
	for(int i = 0; i < numberOfPipes; i++){
		if(pipe(pipeArray[i]) != 0){
			perror("pipe");
			exit(1);
		}
	}
}

/* Create a number of child processes
	- numberOfChildren: The total number of children to create.
	- childPids: Empty array to hold the pids of the children.
	- commandLine: Vector with all of the commands.
	- pipeArray: 2d vector with all the pipes.
*/
void createChildren(int numberOfChildren, pid_t childPids[],
	command commandLine[], int pipeArray[][2]){

	for (int i = 0; i < numberOfChildren; i++){
		pid_t pid = fork();
		if(pid < 0){
			perror("fork");
			exit(1);
		}
		else if (pid == 0){

			command c = commandLine[i];

			// Redirect input if needed
			if(c.infile != NULL){
				if(redirect(c.infile, 0, STDIN_FILENO) == -1){
					perror(c.infile);
					exit(1);
				}
			}

			// Redirect output if needed
			if(c.outfile != NULL){
				if(redirect(c.outfile, 1, STDOUT_FILENO) == -1){
					perror(c.outfile);
					exit(1);
				}
			}

			// Redirect I/O between children if needed
			if(numberOfChildren > 1){
				dupChild(i, numberOfChildren, pipeArray);
			}

			// Execute commands
			execvp(c.argv[0], c.argv);
			perror("exec");
			exit(1);
		}
		else{
			// Store child pids in array
			childPids[i] = pid;
		}
	}
}

/* Redirect input and output between child processes. The first child dups its
write end, the last child its read end, and the children in between both their
read end and write end to make the chain work. When the duping is done, close
all file descriptors not in use.
- childNumber: The index of the child (i.e. 0 for the first child etc).
- numberOfChildren: The total number of children.
- pipeArray: 2d array with the pipes.
*/
void dupChild(int childNumber, int numberOfChildren, int pipeArray[][2]){
	if(childNumber == 0){
		dupPipe(pipeArray[childNumber], WRITE_END, STDOUT_FILENO);
	}

	else if(childNumber == numberOfChildren - 1){
		dupPipe(pipeArray[childNumber - 1], READ_END, STDIN_FILENO);
	}
	else{
		dupPipe(pipeArray[childNumber - 1], READ_END, STDIN_FILENO);
		dupPipe(pipeArray[childNumber], WRITE_END, STDOUT_FILENO);
	}

	for(int i = 0; i < numberOfChildren - 1; i++){
		close(pipeArray[i][0]);
		close(pipeArray[i][1]);
	}
}

/* Execute external commands.
	- numberOfCommands: The total number of commands.
	- commandLine: Vector with all the commands.
*/
void executeExternal(int numberOfCommands, command commandLine[]){
	int numberOfPipes = numberOfCommands - 1;
	int pipeArray[numberOfPipes][2];
	pid_t childPids[numberOfCommands];

	createPipes(numberOfPipes, pipeArray);
	createChildren(numberOfCommands, childPids, commandLine, pipeArray);

	for(int i = 0; i < numberOfPipes; i++){
		close(pipeArray[i][0]);
		close(pipeArray[i][1]);
	}

	for(int i = 0; i < numberOfCommands; i++){
		waitpid(childPids[i], NULL, 0);
	}

	// End all child processes on signal ctrl+c
	if(signalReceived == 1 && childPids != NULL){
		for (int i = 0; i < numberOfCommands; i++){
			kill(childPids[i], SIGKILL);
		}
	}
}

/* Execute commands given by the user.
	- numberOfCommands: The total number of commands.
	- commandLine: Vector with all of the commands.
*/
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

/* Main */
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
