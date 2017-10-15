/* mish.h: header file for an implementation of a shell.
	Course: Systemnära Programmering HT2017
	Author: Marko Nygård(oi12mnd)
	Date: 6.10.2017
*/

#define _GNU_SOURCE
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include "sighant.h"
#include "execute.h"

#ifndef _MISH_
#define _MISH_

/* Echo: Displays all of its arguments to stdout
   - argc: the number of arguments
   - argv: string array with the arguments
*/
void echo(int argc, char*argv[]);

/* Change the current working directory
   - newDirectory: the directory to change to
*/
void cd(char* newDirectory);

#endif


