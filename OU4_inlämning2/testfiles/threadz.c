#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct testStruct {
	int a;
	int b;
} tS;

pthread_mutex_t m;
void* printstuff(void* stuff){

	tS* TS = (tS*) stuff;

	pthread_mutex_lock(&m);
	int X = TS -> a;
	int Y = TS -> b;

	fprintf(stdout, "%d\n",X*Y);
	pthread_mutex_unlock(&m);
	return NULL;
}


int main(){
	
	int x = 3;
	int y = 9;


	tS* ttss = malloc(sizeof(ttss));
	ttss -> a = x;
	ttss -> b = y;

	void* status1;
	void* status2;

	pthread_t t1, t2;


	pthread_create(&t1, NULL, printstuff, (void*)ttss);
	pthread_create(&t2, NULL, printstuff, (void*)ttss);

	pthread_join(t1, &status1);
	pthread_join(t2, &status2);
	
	free(ttss);

	return 0;

}