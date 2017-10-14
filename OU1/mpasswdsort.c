/* Detta program sorterar användarnamn och användar-id givet en fil med
användardata. Datat ska vara på formen:
username:password:UID:GID:GECOS:directory:shell
Fälten password och GECOS kan lämnas tomma, UID och GID måste vara positiva
heltal.
- Input: fil eller filnamn med användardata (en användare per rad).
- Output: sorterad användarlista (på formen UID:username), vilken skrivs ut på
stdout.

Författare: Marko Nygård, oi12mnd@c.umu.se
08-09-2017

Uppdatering 29-09-2017: bugg-fixar
*/
#include "list.h"
#include <string.h>
#include "errno.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct userInfo{
  int uid;
  char* username;
} userInfo;

#define NUMBER_OF_FIELDS 7
#define MAX_LINE_LENGTH 1024

// Om denna variabel förändras innebär det att ett fel i indatat har inträffat.
int exitStatus = 0;

/*
  Kontrollerar att en rad är på korrekt format
  - line: raden som ska kontrolleras
  - numberOfFields: antalet fält i raden
  - lineNumber: radens nummer i filen
  - returnerar: sant om raden har korrekt format, falskt annars
*/
bool checkLineFormat(char line[], int numberOfFields, int lineNumber){
  if (strlen(line) == 1){
    fprintf(stderr, "Line %d: <BLANKLINE> detected.\n", lineNumber);
    exitStatus = 1;
    return false;
  }
  if (numberOfFields != NUMBER_OF_FIELDS){
    fprintf(stderr, "Line %d: Wrong format: %s", lineNumber, line);
    exitStatus = 1;
    return false;
  }
  return true;
}

/*
  Kontrollera att användarnamnet har giltig längd (1-32 tecken)
  - name: användarnamnet
  - lineNumber: radens nummer i filen
  - returnerar: sant om namnet har giltig längd, falskt annars
*/
bool checkUsernameLength(char username[], int lineNumber){
  int len = strlen(username);
  if(len > 32){
    fprintf(stderr, "Line %d: Invalid username length: "
    "%d characters detected.\n", lineNumber, len);
    exitStatus = 1;
    return false;
  }
  return true;
}

/*
  Kontrollerar att ett id-nummer är på rätt format.
  - id: sträng med id-numret
  - lineNumber: radens nummer i filen
  - type: sträng med id-typen (måste vara UID eller GID)
  - returnerar: id som int om detta är på rätt form.
*/
int checkId(char id[], int lineNumber, char type[]){
  //Specialfall: id = 0
  int len = strlen(id);
  if(len == 1 && id[0] == '0'){
    return 0;
  }

  char* idP;
  int intPart = (int) strtol(id, &idP, 10);
  if(*idP != '\0'){
    fprintf(stderr, "Line %d: %s must be an integer. Found: %s\n",
    lineNumber, type, id);
    exitStatus = 1;
    return -1;
  }

  if (intPart < 0){
    fprintf(stderr, "Line %d: %s must be positive. Found: %d\n",
    lineNumber, type, intPart);
    exitStatus = 1;
    return -1;
  }

  return intPart;
}

/*
Kontrollerar att inga obligatoriska fält är tomma.
- data: array med fälten
- lineNumber: radnummer
- returnerar: sant om inga fält är tomma, falskt annars
*/
bool checkEmptyFields(char data[NUMBER_OF_FIELDS][MAX_LINE_LENGTH],
  int lineNumber){
  for(int i = 0; i < NUMBER_OF_FIELDS; i++){
    int len = strlen(data[i]);
    char* emptyField;
    if(len == 0 && (i != 1 && i != 4)){
      switch(i){
        case 0:
          emptyField = "username";
          break;
        case 2:
          emptyField = "UID";
          break;
        case 3:
          emptyField = "GID";
          break;
        case 5:
          emptyField = "directory";
          break;
      }
      fprintf(stderr, "Line %d: The %s field can not be empty.\n",
      lineNumber, emptyField);
      exitStatus = 1;
      return false;
    }
    else if(len == 1 && i == 6){
      emptyField = "shell";
      fprintf(stderr, "Line %d: The %s field can not be empty.\n",
      lineNumber, emptyField);
      exitStatus = 1;
      return false;
    }
  }
  return true;
}

/*
Delar upp en sträng på korrekt format i substrängar med data om
användarnamn, id osv.
- line: raden som ska delas upp.
- lineNumber: radens nummer i filen
- correctIds: lista med korrekt formaterad användardata
*/
void splitLine(char line[], int lineNumber, list* correctIds){
  // Dela upp i delsträngar
  char data[NUMBER_OF_FIELDS][MAX_LINE_LENGTH];
  memset(data, 0, sizeof(data));

  int len = strlen(line);
  int numberOfColons = 0, index = 0;
  for(int i = 0; i < len; i++){
    if(line[i] == ':'){
      numberOfColons++;
      index = 0;
    }
    else{
      data[numberOfColons][index] = line[i];
      index++;
    }
  }

  // Kontrollera korrektheten på strängarna
  bool correct = true;
  correct = checkLineFormat(line, numberOfColons + 1, lineNumber);

  if(correct){
    correct = checkEmptyFields(data, lineNumber);
  }

  if(correct){
    correct = checkUsernameLength(data[0], lineNumber);
  }
  int uid, gid;
  if(correct){
    uid = checkId(data[2], lineNumber, "UID");
    gid = checkId(data[3], lineNumber, "GID");
    if(uid < 0 || gid < 0){
      correct = false;
    }
  }

  // Lägg in korrekt formaterade användarnamn och UIDN i lista.
  // Sortera enligt växande UID vid insättningen.
  char* username = malloc(sizeof(username));
  if(username == NULL){
	  perror("");
	  exit(1);
  }

  userInfo* info = malloc(sizeof(info) + sizeof(username));
  if(info == NULL){
  	perror("");
    exit(1);
  }
  info -> username = NULL;

  if(correct){
	  userInfo* temp;
	  if(uid >= 0 && gid >= 0){
		strcpy(username, data[0]);

		info -> uid = uid;
		info -> username = username;

		node* pos = list_first(correctIds);
		while(!list_end(correctIds, pos)){
		  temp = list_getValue(correctIds, pos);
		  if(info -> uid < temp -> uid){
		    break;
		  }
		  pos = list_next(correctIds, pos);
		}
		list_insert(correctIds, pos, info);
	  }
  }

  if(info -> username == NULL){
  	free(info);
	  free(username);
  }

  else{
	  info = NULL;
	  free(info);
  }
  memset(data, 0, sizeof(data));
}

/*
  Kontrollera input från kommandoraden.
  - argc: antalet argument
  - argv: vektor med argumenten som strängar
  - returnerar: pekare till fil från kommandoraden
*/
FILE* checkArguments(int argc, char* argv[]){
  FILE* f;
  if(argc == 1){
    f = stdin;
  }
  else if(argc == 2){
    f = fopen(argv[1], "r");
    if(f == NULL){
      perror("");
      exit(1);
    }
  }
  else{
    fprintf(stderr, "Number of arguments must be 1 or 2.\n");
    exit(1);
  }
  return f;
}

/*
  Skriv ut listans innehåll.
  - list: pekare till listan.
*/
void displayInfo(list* l){
  node* pos = list_first(l);
  userInfo* info;
  while(!list_end(l, pos)){
    info = list_getValue(l, pos);
    printf("%d:%s\n", info -> uid, info -> username);
    pos = list_next(l, pos);
  }
}

/*
  Tar bort alla användarnamn ur listan.
  - l: pekare till listan
*/
void removeNames(list* l){
  node* pos = list_first(l);
  userInfo* removeName;
  while(!list_end(l, pos)){
    removeName = list_getValue(l, pos);
    free(removeName -> username);
    pos = list_next(l, pos);
  }
}

/* Main */
int main(int argc, char* argv[]){

  FILE* f = checkArguments(argc, argv);
  char l[MAX_LINE_LENGTH];
  int lineNumber = 1;
  list* ids = list_create();

  while(fgets(l, sizeof(l), f) != NULL){
    splitLine(l, lineNumber, ids);
    lineNumber++;
  }
  displayInfo(ids);

  removeNames(ids);
  list_free(ids);
  fclose(f);

  return exitStatus;
}
