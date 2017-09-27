#include "parser.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "sighant.h"
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
		printf("Number of commands: %d\n",numberOfCommands);
		
		// Fastställ kommando
		command c = commandLine[0];
		
		// Enter (tomt kommando): gör inget
		if (c.argv == NULL){
			continue;
		}
		
		else if (strcmp(c.argv[0], "inf") == 0){
			infiniteLoop();
		}
		
		// Kommandot 'echo'
		else if(strcmp(c.argv[0], "echo") == 0){
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
		
		else {
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
		}
	}
	
    printf("\n"); 
    return 0;
}
