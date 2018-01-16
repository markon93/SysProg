/*
  Detta är en implementation av datatypen enkellänkad lista. Dess gränsyta
  följer definitionen given på kursen datastrukturer och algoritmer.
  Författare: Marko Nygård, oi12mnd@cs.umu.se
  08-09-2017
*/
#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef __MEMFREEDATAFUNC
#define __MEMFREEDATAFUNC
typedef void memFreeFunc(void*);
#endif

typedef struct link{
  void* value;
  struct link* next;
} node;

typedef struct list{
  node* head;
  memFreeFunc *freeFunc;
} list;

/*
  Skapar en ny lista med ett tomt element.
  - returnerar: en pekare till den nya listan.
*/
list* list_create(void);

void list_setMemHandler(list *l, memFreeFunc *f);
/*
  Gå till början av listan.
  - returnerar: pekare till det första noden i listan.
*/
node* list_first(list* l);

/*
  Kontrollerar om en lista är tom.
  - l: pekare till listan
  - returnerar: sant om listan är tom, falskt annars.
*/
bool list_isEmpty(list* l);

/*
  Lägger till ett element till listan.
  - l: pekare till listan
  - pos: positionen där noden ska läggas till
  - value: pekare till värdet som ska sättas in
  - returnerar: pekare till den tillagda noden
*/
node* list_insert(list* l, node* n, void* val);

/*
  Kontrollerar om det närvarande elementet är det sista i en lista.
  - l: pekare till listan
  - pos: pekare till nod i listan
  - returnerar: sant om pos är sista noden i listan, falskt annars.
*/
bool list_end(list* l, node* n);

/*
  Hittar nästa element i en lista givet närvarande position.
  - l: pekare till listan
  - pos: pekare till nuvarande nod
  - returnerar: pekare till nästa nod
*/
node* list_next(list* l, node* n);

/*
   Hämtar värdet från en nod.
  - l: pekare till listan
  - pos: pekare till noden vars värde efterfrågas
  - returnerar: värdet som lagras i noden.
*/
void* list_getValue(list* l, node* n);

/*
  Tar bort en nod från listan.
  -l: pekare till listan
  -pos: pekare till noden
  -returnerar: pekare till nästa nod
*/
node* list_remove(list* l, node* n);

/*
  Tömmer hela listan.
  - l: pekare till listan
  - returnerar: pekare till listan
*/
list* list_empty(list* l);

/*
  Avallokera allt data i listan.
  - l: pekare till listan
*/
void list_free(list* l);

#endif
