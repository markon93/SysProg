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
    lstat(path, &path_stat);     // THIS SHOULD BE LSTAT!!
    return S_ISREG(path_stat.st_mode);
}

void* traverse(void* com){
	command* c = (command*) com;
	int numDirs = 0;

  // COPY THE NAME OF THE FILE, OTHERWISE STRCPY SETS IT TO NULL FOR SOME REASON.
//  char* name = malloc(strlen(c->name)*sizeof(c->name));
//  name = strcpy(name, c->name);
	while(true){
		pthread_mutex_lock(&m);

		// Pick a directory from the directory queue
		char* dir;
		if(!q_isEmpty(c -> dirQueue)){
			dir = q_peek(c -> dirQueue);
		}

		else{
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

    char* dirCopy = malloc(strlen(dir)*sizeof(dir) + 1);
    dirCopy = strcat(dir, "/");

		while((pDirent = readdir(d)) != NULL){
			if(strcmp(pDirent -> d_name, ".") &&
      strcmp(pDirent -> d_name, "..")){

        char* fullPath = malloc(strlen(dirCopy)*sizeof(dirCopy) + strlen(pDirent ->d_name)*sizeof(pDirent ->d_name));

        fullPath = strcat(dirCopy, pDirent -> d_name);
        printf("FULL PATH: %s\n",fullPath);
        printf("DIR_COPY: %s\n",dirCopy);

				// Add new directories to queue if there are any
				if(!isRegularFile(fullPath)){
					q_enqueue(c -> dirQueue, fullPath);
				}

				// If the filename is in the current directory, print out the path.
				if(!strcmp(c -> name, pDirent -> d_name)){
						if((isRegularFile(fullPath) && (c -> type == 'f')) ||
							(!isRegularFile(fullPath) && (c -> type == 'd'))){

							printf("FOUND YA!!!");
							printf("\n%s\n",fullPath);
						}
				}
        //memset(fullPath,0,sizeof(fullPath));
        //free(fullPath);
      }

		}
		closedir(d);

		if(!isUnlocked){
			pthread_mutex_unlock(&m);
		}
	}
	printf("Thread: %lu Reads: %d\n", pthread_self(), numDirs);
//  free(name);
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
