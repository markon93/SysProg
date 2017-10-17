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

pthread_mutex_t m;

/* Checks if a given path is a regular file or not. */
int isRegularFile(const char *path){
    struct stat path_stat;
    lstat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

void* traverse(void* com){
	command* c = (command*) com;
	int numDirs = 0;

	while(true){
		pthread_mutex_lock(&m);

		// Pick a directory from the directory queue
		char* dir;
		if(!q_isEmpty(c -> dirQueue)){
			dir = q_peek(c -> dirQueue);
		}
		 else if(q_isEmpty(c -> dirQueue)){
     	 	printf("Empty\n");
      		pthread_mutex_unlock(&m);
     	 	break;
    	}
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
		numDirs++;
		if(d == NULL){
			printf(":::%s:::\n", dir);
			perror("");
			exit(5);
		}

    char* dirWithSlash = (char*)malloc(strlen(dir)*sizeof(dir) + 1);
    strcpy(dirWithSlash, dir);

    dirWithSlash = strcat(dirWithSlash,"/");

		while((pDirent = readdir(d)) != NULL){
			if(strcmp(pDirent -> d_name, ".") &&
      strcmp(pDirent -> d_name, "..")){

        char* fullPath = (char*)malloc(strlen(dirWithSlash)*sizeof(dirWithSlash)+
      strlen(pDirent -> d_name)*sizeof(pDirent -> d_name));
        strcpy(fullPath, dirWithSlash);
        fullPath = strcat(fullPath, pDirent -> d_name);

        char* fullPathCopy = malloc(strlen(fullPath)*sizeof(fullPath));
        strcpy(fullPathCopy, fullPath);

		struct stat path_stat;
    	lstat(fullPath, &path_stat);
    	
		// Add new directories to queue if there are any
		if(S_ISDIR(path_stat.st_mode) && 
			 (access(fullPath, R_OK) == 0)){
			q_enqueue(c -> dirQueue, fullPathCopy);
 	   	}

		// If the filename is in the current directory, print out the path.
		if(!strcmp(c -> name, pDirent -> d_name)){
				if((S_ISREG(path_stat.st_mode) && (c -> type == 'f')) ||
					(S_ISDIR(path_stat.st_mode) && (c -> type == 'd'))||
					(S_ISLNK(path_stat.st_mode) && (c -> type == 'l'))){
					printf("\n%s\n",fullPathCopy);
				}
		}
        free(fullPath);
      }
		}
		closedir(d);
    free(dirWithSlash);
    
    if(!isUnlocked){
		pthread_mutex_unlock(&m);
	}

  }
	printf("Thread: %lu Reads: %d\n", pthread_self(), numDirs);
  return NULL;
}

/* Create a specified number of threads */
void createThreads(command* c){
	pthread_t threads[c -> nrthr - 1];
	void* status[c -> nrthr - 1];

	for(int i = 0; i < c -> nrthr - 1; i ++){
		pthread_create(&threads[i], NULL, traverse, c);
	}

	for(int i = 0; i < c -> nrthr - 1; i ++){
		pthread_join(threads[i], &status[i]);
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
