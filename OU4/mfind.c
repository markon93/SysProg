#include "checkComLine.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "queue.h"

/* Add starting directories to queue */
void addStartingDirectories(queue* q, char* dirs[],
	int nStarts){
	for(int i = 0; i < nStarts; i++){
		q_enqueue(q,dirs[i]);
	}
}

/*
2. Läs filerna i mappen, om filen är en mapp lägg till den i datastrukturen.
3. Om namnet, och ifall filtypen är angedd, matchar skriv ut sökvägen. Ni
kan även lägga till alla sådana sökvägar i en lista och skriva ut dessa på
slutet. Detta har för- och nackdelar.
4. Inkrementera någon sorts räknare för varje mapp som tråden stötte på. Se
[viktigt].
5. Återgå till att plocka en mapp från datastrukturen
*/
void* threadFunc(void* command){
	// 1. Plocka en mapp från er datastruktur
	printf("Thread %d is on the lose!\n", *(int*) args);
	return NULL;
}

/* Create a specified number of threads */
void createThreads(int nrthr){
	for(int i = 0; i < nrthr - 1; i ++){
		pthread_t thread;
		pthread_create(&thread, NULL, threadFunc, (void*)&i);
		void* status;
		pthread_join(thread, &status);
	}
}



/* Main */
int main(int argc, char* argv[]){
	queue* dirQueue = q_create();

	int mode = checkGivenFlags(argc, argv);
	command* c = getCommand(argc, argv, mode);

	printf("t: %c\n", c -> type);
	printf("nrt: %d\n", c -> nrthr);
	printf("start 0: %s \n",c -> start[0]);
	printf("name: %s \n",c -> name);
	printf("nStarts: %d\n\n\n", c -> nStarts);

	addStartingDirectories(dirQueue, c -> start, c -> nStarts);

	createThreads(c -> nrthr);

	free(c);
	q_free(dirQueue);
	return 0;
}

/*

int i = 0;
while(!q_isEmpty(dirQueue)){
	printf("q(%d) = %s\n",i, (char*)q_peek(dirQueue));
	q_dequeue(dirQueue);
	i++;
}

*/
