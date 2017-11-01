/* Simple signal handling function.
   Author: Marko Nygård, oi12mnd@cs.umu.se
   Date: 6.10.2017
 */

#include "sighant.h"

/* Sets the sig variable to 1 whenever a SIGINT signal is received.
  - sig: Integer corresponding to the signal type.
*/
void sighant(int sig) {
  if (sig == SIGINT) {
    signalReceived = 1;
  }
}
