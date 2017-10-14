#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dirent.h>
#include "queue.h"
#include "checkComLine.h"

/*
3. Om namnet, och ifall filtypen är angedd, matchar skriv ut sökvägen. Ni
kan även lägga till alla sådana sökvägar i en lista och skriva ut dessa på
slutet. Detta har för- och nackdelar.
4. Inkrementera någon sorts räknare för varje mapp som tråden stötte på. Se
[viktigt].
5. Återgå till att plocka en mapp från datastrukturen
*/
void* threadFunc(void* com){
	command* c = (command*) com;

	// Pick a directory from the directory queue
	char* dir = q_peek(c -> dirQueue);
	q_dequeue(c -> dirQueue);
	fprintf(stdout,"%s\n",dir);

	// 2. Läs filerna i mappen, om filen är en mapp lägg till den i datastrukturen
	struct dirent *pDirent;
	DIR* d = opendir(dir);
	if(d == NULL){
		perror("opendir");
		exit(5);
	}

	int i = 0;
	while((pDirent = readdir(d)) != NULL){
		//if(strcmp(pDirent -> d_name, ))
		q_enqueue(c -> dirQueue, pDirent -> d_name);
		i++;
	}
	closedir(d);



	return NULL;
}









/* Create a specified number of threads */
void createThreads(command* c){
	for(int i = 0; i < c -> nrthr - 1; i ++){
		pthread_t thread;
		pthread_create(&thread, NULL, threadFunc, c);
		void* status;
		pthread_join(thread, &status);
	}
}



/* Main */
int main(int argc, char* argv[]){
	int mode = checkGivenFlags(argc, argv);
	command* c = getCommand(argc, argv, mode);

	printf("t: %c\n", c -> type);
	printf("nrt: %d\n", c -> nrthr);
	printf("start 0: %s \n", (char*)q_peek(c -> dirQueue));
	printf("name: %s \n",c -> name);
	printf("nStarts: %d\n\n\n", c -> nStarts);

	createThreads(c);



	q_free(c -> dirQueue);
	free(c);
	return 0;
}

/*

int rr = 0;
while(!q_isEmpty(c -> dirQueue)){
	printf("q(%d) = %s\n",rr, (char*)q_peek(c -> dirQueue));
	q_dequeue(c -> dirQueue);
	rr++;
}

*/
