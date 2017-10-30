/* mish.h: header file for an implementation of a shell.
	Course: Systemnära Programmering HT2017
	Author: Marko Nygård(oi12mnd)
	Date: 6.10.2017
  Update: 30.10.2017
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

#ifndef _MISH_
#define _MISH_

/* Echo: Displays all of its arguments to stdout
   - argc: the number of arguments
   - argv: string array with the arguments
*/
void echo(int argc, char*argv[]);

/* Change the current working directory
   - newDirectory: the directory to change to
*/
void cd(char* newDirectory);

/* Write out a prompt */
void writePrompt();

/* Scan and parse all of the commands supplied by the user.
	- commandLine: Empty vector of proper length to hold the commands
	- returns: The number of commands if OK, -1 on error
*/
int getCommands(command commandLine[]);

/* Check if the user supplied an internal command.
	- firstCommand: The first of the user commands.
	- returns: true if it is an internal command, false otherwise.
*/
bool isInternal(char* firstCommand);

/* Execute internal commands, i.e. cd and echo.
	- numberOfCommands: The number of commands.
	- commandLine: Vector with all the commands.
*/
void executeInternal(int numberOfCommands, command commandLine[]);

/* Create a specified number of pipes.
	- numberOfPipes: The number of pipes to create.
	- pipeArray: An empty n-by-2 array which will hold the pipe ends, i.e.
	the file descriptors of the read and write ends of all the pipes.
*/
void createPipes(int numberOfPipes, int pipeArray[][2]);

/* Redirect input and output between child processes. The first child dups its
write end, the last child its read end, and the children in between both their
read end and write end to make the chain work. When the duping is done, close
all file descriptors not in use.
- childNumber: The index of the child (i.e. 0 for the first child etc).
- numberOfChildren: The total number of children.
- pipeArray: 2d array with the pipes.
*/
void dupChild(int childNumber, int numberOfChildren, int pipeArray[][2]);

/* Create a number of child processes
	- numberOfChildren: The total number of children to create.
	- childPids: Empty array to hold the pids of the children.
	- commandLine: Vector with all of the commands.
	- pipeArray: 2d vector with all the pipes.
*/
void createChildren(int numberOfChildren, pid_t childPids[],
  command commandLine[], int pipeArray[][2]);

/* Execute external commands.
  - numberOfCommands: The total number of commands.
	- commandLine: Vector with all the commands.
*/
void executeExternal(int numberOfCommands, command commandLine[]);

/* Execute commands given by the user.
	- numberOfCommands: The total number of commands.
	- commandLine: Vector with all of the commands.
*/
void executeCommands(int numberOfCommands, command commandLine[]);


#endif
