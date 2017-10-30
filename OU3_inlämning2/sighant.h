/* Header file for a simple signal handler.
  Author: Marko Nygård, oi12mnd@cs.umu.se
  Date: 6.10.2017
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Used to determine whether a signal is received or not
extern int signalReceived;

#ifndef _SIGHANT_
#define _SIGHANT_

/* Sets the sig variable to 1 whenever a SIGINT signal is received.
  - sig: Integer corresponding to the signal type.
*/
void sighant(int sig);

#endif
