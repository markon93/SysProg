#ifndef CHECK_COM_LINE
#define CHECK_COM_LINE

#define NUM_VALID_TYPE_FLAGS 3

// Command line arguments
typedef struct command{
  char type;
  int nrthr;
  char* name;
  char* start[];
} command;

void standardErrorMessage();

void errorHandler(int errno);

int checkNumberArguments(int argc, int minArgs, int flagValue);

int checkGivenFlags(int argc, char* argv[]);

char checkType(char* type);

int checkNrthr(char* nrthr);

command* getCommand(int argc, char* argv[], int mode);

#endif