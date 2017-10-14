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
  int nStarts;
  queue* dirQueue;
} command;

/* Prints a string containing a common error message. */
void standardErrorMessage();

/* Handles a variety of errors that may arise when calling mfind.
- errno: An integer value corresponding to the type of error that was made. The error codes are not sorted in any particular order.
*/
void errorHandler(int errno);

/* Checks the validity of the user input.
	- argc: The number of arguments
	- argv: string array with the arguments
	- returns: int value depending on which flags have been given:
		* 0 if no flags are given.
		* 1 if only type is given.
		* 2 if only nrthreads are given.
		* 3 if both type and nrthreads are given.
If any errors occur, they are passed to the error handler.
*/
int checkGivenFlags(int argc, char* argv[]);

/*	Checks wheter a given number of arguments is valid for a specified command line setup.
	- argc: The number of arguments
	- mode: an integer value corresponding to the command line setup. See checkGivenFlags for interpretation.
	- minArgs: The minimum number of arguments required
		for the given the mode (e.g. 3 for mode 0)
*/
int checkNumberArguments(int argc, int minArgs, int mode);

/* Checks whether a user supplied type has been correctly specified.
	- type: The user supplied type argument. Must be one of the following:
		* 'd' (directory)
		* 'f' (file)
		* 'l' (link)
	- returns: The type as a char, if it is valid. Invalid types are passed to the error handler.
*/
char checkType(char* type);

/* Checks whether a user supplied number of threads is valid.
	- nrthr: The user supplied argument.
	- returns: The number of threads, if correctly specified (i.e. a positive and non-zero integer.) Any error is passed to the error handler.
*/
int checkNrthr(char* nrthr);

/* Puts a correctly formatted command line into a command structure.
	- argc: The number of arguments.
	- argv: Vector with the arguments.
	- mode: An integer value as specified in checkGivenFlags.
*/
command* getCommand(int argc, char* argv[], int mode);

#endif
