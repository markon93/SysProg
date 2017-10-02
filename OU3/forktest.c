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

int main(int argc, char * argv[]){
printf("pre-fork\n");

pid_t parentp = getpid();

pid_t p = fork();

int s;
waitpid(parentp, &s, 0);

fprintf(stderr,"post-fork, getpid = %d\n", getpid());
fprintf(stderr,"post-fork, p = %d\n", p);

printf("\n");
sleep(1);


}
