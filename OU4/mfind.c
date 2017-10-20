#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "queue.h"
#include "checkComLine.h"

pthread_mutex_t m;
pthread_mutex_t mWait;
pthread_cond_t cond;

int waitingThreads = 0, threadsDone = 0;
/**/
void* traverse(void* com){
  command* c = (command*) com;
  int numDirs = 0;

  while(true){
    pthread_mutex_lock(&m);

    if(q_isEmpty(c -> dirQueue)){
      waitingThreads++;
      pthread_mutex_unlock(&m);
      if(waitingThreads == c -> nrthr){
        pthread_cond_broadcast(&cond);
      }

      else{
        pthread_cond_wait(&cond, &mWait);
        pthread_mutex_unlock(&mWait);
      }

      if(waitingThreads == c -> nrthr){
        break;
      }
      else{
        waitingThreads--;
        continue;
      }
    }
    else{
      // Pick a directory from the directory queue
      char* dir = q_peek(c -> dirQueue);

      q_dequeue(c -> dirQueue);
      pthread_mutex_unlock(&m);
      // Read the files in the directory
      // If the file is a directory, put it in the queue
      struct dirent *pDirent;
      DIR* d = opendir(dir);
      numDirs++;
      if(d == NULL){
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
            pthread_cond_signal(&cond);
          }

          // If the filename is in the current directory, print out the path.
          if(!strcmp(c -> name, pDirent -> d_name)){
            if((S_ISREG(path_stat.st_mode) && (c -> type == 'f')) ||
            (S_ISDIR(path_stat.st_mode) && (c -> type == 'd'))||
            (S_ISLNK(path_stat.st_mode) && (c -> type == 'l'))||
            (c -> type == 'a')) {
              printf("%s\n",fullPathCopy);
            }
          }
          free(fullPath);
        }
      }
      closedir(d);
      free(dirWithSlash);
    }
  }
  printf("Thread: %lu Reads: %d\n", pthread_self(), numDirs);
  threadsDone++;
  return NULL;
}

/* Create a specified number of threads */
void createThreads(command* c){
  pthread_t threads[c -> nrthr - 1];
  for(int i = 0; i < c -> nrthr - 1; i ++){
    pthread_create(&threads[i], NULL, traverse, c);
  }
  traverse(c);

  while(threadsDone != c -> nrthr){
    sleep(0.1);
    pthread_cond_signal(&cond);
  }

  for(int i = 0; i < c -> nrthr - 1; i ++){
    if(pthread_join(threads[i], NULL) != 0)
    perror("");
  }
}

/* Main */
int main(int argc, char* argv[]){
  int mode = checkGivenFlags(argc, argv);
  command* c = getCommand(argc, argv, mode);

  createThreads(c);
  q_free(c -> dirQueue);
  free(c);

  return 0;
}
