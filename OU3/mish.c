#define _POSIX_SOURCE
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include "sighant.h"

int signalReceived = 0;
/* Skriv ut alla element i en sträng-array utom det första.
   - argc: antalet argument
   - argv: sträng-array med argumenten
*/
void echo(int argc, char* argv[], char* outfile){
	int i;
	FILE* out;
	if(outfile == NULL){
		out = stdout;
	}
	else{
		out = fopen(outfile, "w");
	}

	if(out == NULL){
		perror("");
		exit(1);
	}

	for(i = 1; i < argc; i++){
			fprintf(out, "%s ", argv[i]);
	}
	printf("\n");
}

void infiniteLoop(void){
	while(true)
  		sleep(5);
}

/* Byt katalog
   - newDirectory: katalogen att byta till
*/
void cd(char* newDirectory){
	int tryChangeDir = chdir(newDirectory);
	if(tryChangeDir != 0){
		perror(newDirectory);
	}
}

int main (int argc, char * argv[]){
  	signal(SIGINT, sighant);
		pid_t parentPid = getpid();
		while(true){

  		char cwd[1024];

	  	// Skriv ut prompt
			fprintf(stderr, "mish%% ");
			int tryFlush = fflush(stderr);
			if(tryFlush != 0){
				perror("");
				exit(1);
			}



			// Scanna in kommandorad
			char input[MAXLINELEN + 1];
			if(fgets(input, MAXLINELEN + 1, stdin) == NULL){
				printf("\n");
				return(0);
			}

			// Parsa kommandoraden
			command commandLine[MAXCOMMANDS + 1];
			int numberOfCommands = parse(input, commandLine);
			pid_t commandPids[numberOfCommands + 1];

			// Enter (tomt kommando): gör inget
			if (numberOfCommands == 0){
				continue;
			}

			// Fastställ kommando
			command c = commandLine[0];

			// Kolla om kommandot är internt eller externt
			// Internt = 0; externt = 1.
			if(strcmp(c.argv[0], "echo") == 0|| strcmp(c.argv[0], "cd") == 0){
				c.internal = 0;
			}
			else{
				c.internal = 1;
			}

			// Exekvera internt kommando
			if(c.internal == 0){
				printf("INTERNAL\n");
				// Kommandot 'echo'
				if(strcmp(c.argv[0], "echo") == 0){
					echo(c.argc, c.argv, c.outfile);
				}

				// Kommandot cd
				else if(strcmp(c.argv[0], "cd") == 0){
					cd(c.argv[1]);
				}

				// Skriv ut current directory
				else if(strcmp(c.argv[0], "cwd") == 0){
					printf("Current directory: %s\n", getcwd(cwd, sizeof(cwd)));
				}
			}

////////////////////////////////////
			else {
				pid_t pid;
				for (int i = 0; i < numberOfCommands; i++){
					pid = fork();
					fprintf(stderr,"pid = %d\n", pid);
					printf("En fork hände!\n");
					// Fel vid forkningen
					if(pid < 0){
						perror("");
						exit(1);
					}

					else if(pid == 0){
						fprintf(stderr,"Barnet gör något\n");
						fflush(stderr);
						system("ls");

						break;
					}

					else{

						printf("Föräldern gör något\n");
						commandPids[i] = pid;
						int s;
						waitpid(pid, &s, 0);
					}


				}


/*
				for(int i = 0; i < argc; i++){
					commandPids[i] = 1;
				}

				char exec[1024];
				if(c.argv[0][0] == '.' && c.argv[0][1] == '/'){
					for(int j = 0; j < c.argc; j++){
						strcat(exec, c.argv[j]);
						strcat(exec, " ");
					}
					printf("%s\n", exec);
					int status = system(exec);
					if(memset(exec, 0, sizeof(exec)) == NULL){
    				perror("");
  				}
				}
				else{
					fprintf(stderr, "Unknown command.\n");
					continue;
				}
*/

		// Avsluta alla barnprocesser på signal ctrl+c
		if(getpid() == parentPid && signalReceived == 1 &&
	commandPids != NULL){
			for (int i = 0; i < numberOfCommands; i++){
					printf("PiD = %d\n",commandPids[i]);
					kill(commandPids[i], SIGKILL);
			}
		}


	}
sleep(1);
		//for (int i = 0; i < numberOfCommands; i++){
		//	printf("time to die, pid = %d\n",commandPids[i]);
		//	kill(commandPids[i], SIGINT);
		//}

		// Om barnprocesserna ej avslutades, döda dem.

	}

  printf("\n");
	//free(commands);
  return 0;
}
