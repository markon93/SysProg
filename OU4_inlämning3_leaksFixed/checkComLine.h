/* This is a header file for an input handler for the program mfind.c.

Author: Marko Nygård, oi12mnd@cs.umu.se
12.10.2017
*/

#ifndef CHECK_COM_LINE
#define CHECK_COM_LINE

#include "queue.h"
#define NUM_VALID_TYPE_FLAGS 3

/* Store command line options*/
typedef struct command{
  char type;
  int nrthr;
  char* name;
  queue* dirQueue;
} command;

/* Prints a string containing a common error message. */
void standardErrorMessage();

/* Puts a correctly formatted command line into a command structure.
	- argc: The number of arguments.
	- argv: Vector with the arguments.
	- mode: An integer value as specified in checkGivenFlags.
*/
command* getCommand(int argc, char* argv[]);

#endif
