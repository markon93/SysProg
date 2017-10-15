#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int signalReceived;

#ifndef _SIGHANT_
#define _SIGHANT_

void sighant(int signum);

#endif
