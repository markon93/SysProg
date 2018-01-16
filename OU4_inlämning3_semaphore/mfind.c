#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include "queue.h"
#include "checkComLine.h"

pthread_mutex_t m;
sem_t sem;
int waitingThreads = 0, threadsDone = 0;
bool allDone = false;

/* This function applies the bredth-first search algorithm with specified
directories as the starting queue. A directory is picked from the queue, and
new subdirectories are added to the queue as the threads traverse the file
tree. This goes on until it is certain that all directories have been traversed.
- com: A structure with all parameters and flags.
- returns: The NULL pointer, output is printed to stdout.
*/
void* traverse(void* com){
  command* c = (command*) com;
  int numDirs = 0;

  while(true){
    pthread_mutex_lock(&m);
    bool empty = q_isEmpty(c -> dirQueue);
    pthread_mutex_unlock(&m);
    if(empty){
      waitingThreads++;
      if(waitingThreads == c -> nrthr){
        allDone = true;
        for(int i = 0; i < c->nrthr; i++){
          sem_post(&sem);
        }
        break;
      }

      sem_wait(&sem);
      if(allDone){
        break;
      }
      else{
        waitingThreads--;
      }
    }

    else{

      pthread_mutex_lock(&m);
      char* dir  = (char*)malloc(strlen(q_peek(c->dirQueue)) + 1);
      strcpy(dir, (char*) q_peek(c -> dirQueue));
      q_dequeue(c -> dirQueue);
      pthread_mutex_unlock(&m);

      // Read the files in the directory
      // If the file is a directory, put it in the queue
      struct dirent *pDirent;

      DIR* d = opendir(dir);
      numDirs++;
      if(d == NULL){
        perror(dir);
        free(dir);
        continue;
      }

      char* dirWithSlash = (char*)malloc(strlen(dir) + 2);

      strcpy(dirWithSlash, dir);

      free(dir);

      dirWithSlash = strcat(dirWithSlash,"/");

      while((pDirent = readdir(d)) != NULL){
        if(strcmp(pDirent -> d_name, ".") &&
        strcmp(pDirent -> d_name, "..")){

          char* fullPath = (char*)malloc(strlen(dirWithSlash)+
          strlen(pDirent -> d_name) + 1);

          strcpy(fullPath, dirWithSlash);
          fullPath = strcat(fullPath, pDirent -> d_name);

          char* fullPathCopy = malloc(strlen(fullPath)+1);
          strcpy(fullPathCopy, fullPath);

          free(fullPath);

          struct stat path_stat;
          lstat(fullPathCopy, &path_stat);

          // If the filename is in the current directory, print out the path.
          if(!strcmp(c -> name, pDirent -> d_name)){
            if((S_ISREG(path_stat.st_mode) && (c -> type == 'f')) ||
               (S_ISDIR(path_stat.st_mode) && (c -> type == 'd'))||
               (S_ISLNK(path_stat.st_mode) && (c -> type == 'l'))||
               (c -> type == 'a')) {
                 printf("%s\n",fullPathCopy);
             }
           }

          // Add new directories to queue if there are any
          if(S_ISDIR(path_stat.st_mode)){
            if((access(fullPathCopy, R_OK) == 0)){
              pthread_mutex_lock(&m);
              q_enqueue(c -> dirQueue, fullPathCopy);
              pthread_mutex_unlock(&m);
              sem_post(&sem);
            }
            else{
              fprintf(stderr, "%s: Permission denied.\n", fullPathCopy);
              free(fullPathCopy);
            }
          }
          else{
            free(fullPathCopy);
          }
        }
      }
      closedir(d);
      free(dirWithSlash);
    }
  }
  printf("Thread: %lu Reads: %d\n", pthread_self(), numDirs);
  return NULL;
}

/* Create a specified number of threads */
void createThreads(command* c){
  pthread_t threads[c -> nrthr - 1];
  for(int i = 0; i < c -> nrthr - 1; i ++){
    pthread_create(&threads[i], NULL, traverse, c);
  }
  traverse(c);

  for(int i = 0; i < c -> nrthr - 1; i ++){
    if(pthread_join(threads[i], NULL) != 0)
    perror("");
  }
}

/* Main */
int main(int argc, char* argv[]){


  command* c = getCommand(argc, argv);
  sem_init(&sem, 0, c->nrthr+1);

  createThreads(c);
  q_free(c -> dirQueue);
  free(c);
  return 0;
}
