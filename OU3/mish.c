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

	// Shared memory variable for connecting child pid to command number
	int * commandIndex = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);

	pid_t parentPid = getpid();
	while(true){
		signalReceived = 0;
		*commandIndex = 0;

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
			perror("");
			return(0);
		}

		// Parse the command line
		command commandLine[MAXCOMMANDS + 1];
		int numberOfCommands = parse(input, commandLine);

		// Enter pressed: new prompt
		if (numberOfCommands == 0){
			continue;
		}

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

////////////////////////////////////
		else {
			pid_t childPids[numberOfCommands + 1];
			// Create pipes
			int fd[2];
			for(int i = 0; i < numberOfCommands - 1; i++){
				if(pipe(fd) != 0){
					perror("pipe");
					exit(1);					
				}
			}

			// Create child processes
			for(int i = 0; i < numberOfCommands; i++){
				int pid = fork();

				if(pid < 0){
					perror("fork");
					exit(1);					
				}				

				else if (pid == 0){
					*commandIndex = *commandIndex + 1;	
					fprintf(stderr,"Child %d: cI = %d\n", getpid(), *commandIndex);
					break;
				}

				else{
					childPids[i] = pid;					
				}
			}
		
			// Execute all the child processes
			if(getpid() != parentPid){
				printf("Child %d ready.\n",getpid());
				command c = commandLine[*commandIndex - 1];
			
				if(c.outfile != NULL){
					if(redirect(c.outfile, 0, STDIN_FILENO) == -1){
						perror(c.outfile);
					}
				}

				if(c.infile != NULL){
					if(redirect(c.infile, 1, STDOUT_FILENO) == -1){
						perror(c.outfile);
					}
				}
				
				if(numberOfCommands > 1){
					if(*commandIndex == 1){
						dupPipe(fd, WRITE_END, STDOUT_FILENO);
						fprintf(stderr,"Child %d duping write.\n", getpid());
					}

					else if(*commandIndex == numberOfCommands){
						dupPipe(fd, READ_END, STDIN_FILENO);
						fprintf(stderr,"Child %d duping read.\n", getpid());
					}
					else{
						dupPipe(fd, READ_END, STDIN_FILENO);
						dupPipe(fd, WRITE_END, STDOUT_FILENO);
					}
				}
				fprintf(stderr,"Child %d done.\n", getpid());
				if(execvp(c.argv[0], c.argv) < 0){
					perror("exec");
					return -1;
				}	
				fprintf(stderr,"Child %d done.\n", getpid());			
				return 0;			
			}
				
			if(getpid() == parentPid){	
				for(int j = 0; j < numberOfCommands; j++){
					int s;						
					waitpid(childPids[j], &s, 0);					
				}
			}

//			for(int j = 3; j < 2*numberOfCommands; j++){
//				close(j);
//			}

	/*

				bool killP = false;
				pid_t pid;
				for (int i = 0; i < numberOfCommands; i++){
					command c = commandLine[i];

					fprintf(stderr,"pid = %d\n", getpid());
					printf("En fork hände!\n");
					pid = fork();

					// Fel vid forkningen
					if(pid < 0){
						perror("");
						exit(1);
					}

					else if(pid == 0){

						if(c.outfile != NULL){
					  	int fdOut = redirect(c.outfile, 0, STDIN_FILENO);
					  }

						if(c.infile != NULL){
							int fdIn = redirect(c.infile, 1, STDOUT_FILENO);
						}

						fprintf(stderr,"Barnet gör något\n");
						fflush(stderr);

						execvp(c.argv[0], c.argv);
						break;
					}

					else{

						printf("Föräldern gör något\n");
						commandPids[i] = pid;
						int s;
						waitpid(pid, &s, 0);
					}
					if (signalReceived == 1){
						killP = true;
						break;
					}

				}

		// Barnprocessen är klar
		if (getpid() != parentPid){
			return 0;
		}

		// Avsluta alla barnprocesser på signal ctrl+c
		if(getpid() == parentPid && killP &&
	commandPids != NULL){
			for (int i = 0; i < numberOfCommands; i++){
					printf("Control C pressed.\n");
					kill(pid, SIGKILL);
			}
		}
	}

		// Avsluta alla barnprocesser
		if (getpid() == parentPid){
			for (int i = 0; i < numberOfCommands; i++){
				printf("time to die, pid = %d\n",commandPids[i]);
				kill(commandPids[i], SIGKILL);
			}*/
		}
	}

  printf("\n");
  //free(childPids);
  return 0;
}
