#include "checkComLine.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/* Main */
int main(int argc, char* argv[]){
	
	int mode = checkGivenFlags(argc, argv);
	command* c = getCommand(argc, argv, mode);
	
	printf("t: %c\n", c -> type);
	printf("nrt: %d\n", c -> nrthr);
	printf("start 0: %s \n",c -> start[0]);
	printf("name: %s \n",c -> name);


	free(c);
	return 0;
}