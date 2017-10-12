#include "checkComLine.h"

/* Main */
int main(int argc, char* argv[]){
	
	int mode = checkGivenFlags(argc, argv);
	command* c = getCommand(argc, argv, mode);
	
	free(c);
	return 0;
}