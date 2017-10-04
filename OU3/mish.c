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
#include "execute.h"

int signalReceived = 0;
/* Skriv ut alla element i en sträng-array utom det första.
   - argc: antalet argument
   - argv: sträng-array med argumenten
*/
void echo(int argc, char* argv[]){
	for(int i = 1; i < argc; i++){
			printf("%s ", argv[i]);
	}
	printf("\n");
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
			signalReceived = 0;

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
				printf("in=%s\n",input);
				perror("");
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
				// Kommandot 'echo'
				if(strcmp(c.argv[0], "echo") == 0){
					echo(c.argc, c.argv);
					continue;
				}

				// Kommandot cd
				else if(strcmp(c.argv[0], "cd") == 0){
					cd(c.argv[1]);
					continue;
				}
			}

////////////////////////////////////
			else {
/*				for(int i = 0; i < numberOfCommands){
					int pid = fork();
					if (pid == 0){
						break;
					}
				}

				for(int i = 0; i < numberOfCommands - 1; i++){

					if(pipe())
				}*/
				for (int i = 0; i < numberOfCommands - 1; i++){
					int fd[2];
					if(pipe(fd) != 0){
						perror("");
					}
				}

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
			}
		}
	}

  printf("\n");
	//free(commands);
  return 0;
}
