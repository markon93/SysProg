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
	int tryChangeDir = chdir(newDirectory);
	if(tryChangeDir != 0){
		perror(newDirectory);
	}
}

int main (int argc, char * argv[]){
	// Create a signal handler
  signal(SIGINT, sighant);

	pid_t parentPid = getpid();
	while(true){
		signalReceived = 0;

	  	// Write a prompt
		fprintf(stderr, "mish%% ");
		int tryFlush = fflush(stderr);
		if(tryFlush != 0){
			perror("");
			exit(1);
		}

		// Scan the command line
		char input[MAXLINELEN + 1];
		if(fgets(input, MAXLINELEN + 1, stdin) == NULL){
			fprintf(stderr,"Error reading the command.\n");
			continue;
		}

		// Parse the command line
		command commandLine[MAXCOMMANDS + 1];
		int numberOfCommands = parse(input, commandLine);

		// Enter pressed: new prompt
		if (numberOfCommands == 0){
			continue;
		}

		// Shared memory variable for connecting child pid to command number
		int *childPids = mmap(NULL, numberOfCommands*sizeof(int), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);

		// Get command
		command c = commandLine[0];

		// Check whether the command is external or internal
		// Internal = 0; external = 1.
		if(strcmp(c.argv[0], "echo") == 0|| strcmp(c.argv[0], "cd") == 0){
			c.internal = 0;
		}
		else{
			c.internal = 1;
		}

		// Execute internal command
		if(c.internal == 0){
			// echo command
			if(strcmp(c.argv[0], "echo") == 0){
				echo(c.argc, c.argv);
				continue;
			}

			// cd command
			else if(strcmp(c.argv[0], "cd") == 0){
				cd(c.argv[1]);
				continue;
			}
		}

		else {
			// Create pipes
			int fd[numberOfCommands - 1][2];
			for(int i = 0; i < numberOfCommands - 1; i++){
				if(pipe(fd[i]) != 0){
					perror("pipe");
					exit(1);
				}
				printf("Pipe fds %d %d\n", fd[i][0], fd[i][1]);
			}

			// Create child processes
			for(int i = 0; i < numberOfCommands; i++){
				int pid = fork();

				if(pid < 0){
					perror("fork");
					exit(1);
				}

				else if (pid == 0){
					break;
				}

				else{
					childPids[i] = pid;
				}
			}

			if(getpid() == parentPid){
				for(int p = 0; p < numberOfCommands - 1; p++){
					printf("Close fds %d %d\n", fd[p][0], fd[p][1]);
					close(fd[p][0]);
					close(fd[p][1]);
				}
				for(int j = 0; j < numberOfCommands; j++){
					printf("Parent waiting...\n");
					waitpid(childPids[j], NULL, 0);
				}
			}




			// Execute all the child processes
			if(getpid() != parentPid){
				printf("Child %d ready.\n",getpid());

				int j;
				for(j = 0; j < numberOfCommands; j++){
					if(getpid() == childPids[j]){
						printf("Child %d will do command %d.\n", getpid(), j + 1);
						c = commandLine[j];
						break;
					}
				}

				if(c.outfile != NULL){
					if(redirect(c.outfile, 0, STDIN_FILENO) == -1){
						perror(c.outfile);
					}
					else{printf("Child %d opens outfile...\n", getpid());}
				}

				if(c.infile != NULL){
					if(redirect(c.infile, 1, STDOUT_FILENO) == -1){
						perror(c.outfile);
					}
					else{printf("Child %d opens outfile...\n", getpid());}
				}

				if(numberOfCommands > 1){
					if(j == 0){
						dupPipe(fd[j], WRITE_END, STDOUT_FILENO);
						fprintf(stderr,"Child %d duping write.\n", getpid());
					}

					else if(j == numberOfCommands - 1){
						dupPipe(fd[j - 1], READ_END, STDIN_FILENO);
						fprintf(stderr,"Child %d duping read.\n", getpid());
					}
					else{
						fprintf(stderr,"Child %d duping read and write.\n", getpid());
						dupPipe(fd[j - 1], READ_END, STDIN_FILENO);
						dupPipe(fd[j], WRITE_END, STDOUT_FILENO);
					}
				}
				printf("Child %d execing...\n", getpid());
				execvp(c.argv[0], c.argv);
				perror("exec");
				return -1;
			}

			for(int k = 0; k < numberOfCommands - 1; k++){
				close(fd[k][0]);
				close(fd[k][1]);
			}

			// Avsluta alla barnprocesser på signal ctrl+c
			if(getpid() == parentPid && signalReceived == 1 &&
				childPids != NULL){
				for (int i = 0; i < numberOfCommands; i++){
					kill(childPids[i], SIGKILL);
				}
			}
		}
	}
  	printf("\n");
  	return 0;
}
