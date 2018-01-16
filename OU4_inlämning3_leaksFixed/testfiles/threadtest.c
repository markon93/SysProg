#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* printStuff(void* argp){
	fprintf(stdout, "This is a function that prints %s!\n", (char*)argp);
	return NULL;
}

void* mult(void* x){
	int prod = (*(int*)x)*(*(int*)x);
	void* p = &prod;
	thr_exit(p);
}

int main(void){
	pthread_t printThread;
	char* y = "stuff";
	int threadTest = pthread_create(&printThread, NULL, printStuff, y);
	pthread_join(printThread, NULL);

	pthread_t prodT;
	int a = 5;
 
	int t2 = pthread_create(&prodT, NULL, mult, &a);
	pthread_join(prodT, NULL);

	void* pa = mult(&a); 
	printf("The product is %d\n", *(int*)pa);
	exit(0);
}

