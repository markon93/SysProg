#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
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
pthread_mutex_t m;

int isRegularFile(const char *path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void* traverse(void* com){
	command* c = (command*) com;
	pthread_mutex_lock(&m);

	// Pick a directory from the directory queue
	char* dir = q_peek(c -> dirQueue);
	printf("q = %s\n", (char*)q_peek(c -> dirQueue));
	q_dequeue(c -> dirQueue);

	// If the queue is not empty, let the other threads continue, else the
	// first thread must look for subdirectories before continuing.
	bool isUnlocked = false;
	if(!q_isEmpty(c -> dirQueue)){
		isUnlocked = true;
		pthread_mutex_unlock(&m);
	}

	// Read the files in the directory
	// If the file is a directory, put it in the queue
	struct dirent *pDirent;
	DIR* d = opendir(dir);
	if(d == NULL){
		printf(":::%s:::\n", dir);
		perror("");
		exit(5);
	}

	// Copy the name of the searched file, otherwise strcpy sets it
	// to NULL for some reason.
	char* name = malloc(strlen(c->name)*sizeof(c->name));
	name = strcpy(name, c->name);
	dir = strcat(dir, "/");

	char* dirCopy = malloc(strlen(dir)*sizeof(dir));

	int i = 0;
	while((pDirent = readdir(d)) != NULL){
		if(strcmp(pDirent -> d_name, ".") && strcmp(pDirent -> d_name, "..")){
			char* fullPath = strcpy(dirCopy, dir);
			fullPath = strcat(fullPath, pDirent -> d_name);

			char* pathCopy = malloc(strlen(fullPath)*sizeof(fullPath));
			char* path = strcpy(pathCopy, fullPath);

			// Add new directories to queue if there are any
			if(!isRegularFile(fullPath)){
				q_enqueue(c -> dirQueue, path);
			}
			pathCopy = NULL;

			// If the filename is in the current directory, print out the path.
			if(!strcmp(name, pDirent -> d_name)){
					printf("\n%s\n",fullPath);
			}
		}
		i++;
	}
	closedir(d);

	if(!isUnlocked){
		pthread_mutex_unlock(&m);
	}
	free(name);
	free(dirCopy);
	return NULL;
}









/* Create a specified number of threads */
void createThreads(command* c){
	for(int i = 0; i < c -> nrthr - 1; i ++){
		pthread_t thread;
		pthread_create(&thread, NULL, traverse, c);
		void* status;
		pthread_join(thread, &status);
	}
}



/* Main */
int main(int argc, char* argv[]){
	int mode = checkGivenFlags(argc, argv);
	command* c = getCommand(argc, argv, mode);


/*
	printf("t: %c\n", c -> type);
	printf("nrt: %d\n", c -> nrthr);
	printf("start 0: %s \n", (char*)q_peek(c -> dirQueue));
	printf("name: %s \n",c -> name);
	printf("nStarts: %d\n\n\n", c -> nStarts);
*/

	createThreads(c);

	printf("\n\n\n");
	int rr = 0;
	while(!q_isEmpty(c -> dirQueue)){
		printf("q(%d) = %s\n",rr, (char*)q_peek(c -> dirQueue));
		q_dequeue(c -> dirQueue);
		rr++;
	}

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
