#include "sighant.h"

void sighant(int sig) {
  if (sig == SIGINT) {
      signalReceived = 1;
  }
}
