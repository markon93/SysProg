#include "sighant.h"

void sighant(int signum) {
    if (signum == SIGINT) {
        fprintf(stderr, "En SIGINT signal\n");
        exit(1);
    } 
    else {
        fprintf(stderr, "Okänt fel\n");
    }
    return;
}

