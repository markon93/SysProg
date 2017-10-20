/*
  Detta är en implementation av datatypen enkellänkad lista. Dess
  gränsyta följer definitionen given på kursen datastrukturer och algoritmer.
  Författare: Marko Nygård, oi12mnd@cs.umu.se
  08-09-2017
*/
#include "list.h"
/*
  Skapar en ny lista med ett tomt element.
  - returnerar: en pekare till den nya listan.
*/
list* list_create(){
  list* created_list = malloc(sizeof(list));
  if(created_list == NULL){
    perror("");
  }
  created_list -> head = malloc(sizeof(node));
  if(created_list -> head == NULL){
    perror("");
  }
  created_list -> head -> value = NULL;
  created_list -> head -> next = NULL;
  return created_list;
}

/*
  Gå till början av listan.
  - returnerar: pekare till det första noden i listan.
*/
node* list_first(list* l){
  return l -> head;
}

/*
  Kontrollerar om en lista är tom.
  - l: pekare till listan
  - returnerar: sant om listan är tom, falskt annars.
*/
bool list_isEmpty(list* l){
    return(l -> head -> next == NULL);
}

/*
  Lägger till ett element till listan.
  - l: pekare till listan
  - pos: positionen där noden ska läggas till
  - value: pekare till värdet som ska sättas in
  - returnerar: pekare till den tillagda noden
*/
node* list_insert(list* l, node* pos, void* value){
  node* new = malloc(sizeof(node));
  if(new == NULL){
    perror("");
  }
  new -> value = value;
  new -> next = pos -> next;
  pos -> next = new;
  return pos;
}

/*
  Kontrollerar om det närvarande elementet är det sista i en lista.
  - l: pekare till listan
  - pos: pekare till nod i listan
  - returnerar: sant om pos är sista noden i listan, falskt annars.
*/
bool list_end(list* l, node* pos){
  return (pos -> next == NULL);
}

/*
  Hittar nästa element i en lista givet närvarande position.
  - l: pekare till listan
  - pos: pekare till nuvarande nod
  - returnerar: pekare till nästa nod
*/
node* list_next(list* l, node* pos){
  if (pos -> next != NULL){
    return pos -> next;
  }
  else{
    return NULL;
  }
}

/*
   Hämtar värdet från en nod.
  - l: pekare till listan
  - pos: pekare till noden vars värde efterfrågas
  - returnerar: värdet som lagras i noden.
*/
void* list_getValue(list* l, node* pos){
  return pos -> next -> value;
}

/*
  Tar bort en nod från listan.
  -l: pekare till listan
  -pos: pekare till noden
  -returnerar: pekare till nästa nod
*/
node* list_remove(list* l, node* pos){
  node* n = pos -> next;
  pos -> next = pos -> next -> next;
  n -> next = NULL;
  free(n);
  return pos;
}

/*
  Tömmer hela listan.
  - l: pekare till listan
  - returnerar: pekare till listan
*/
list* list_empty(list* l){
  node* n = list_first(l);
  while(!list_isEmpty(l)){
    list_remove(l, n);
  }
  return l;
}

/*
  Avallokera allt data i listan.
  - l: pekare till listan
*/
void list_free(list* l){
  node* n = list_first(l);
  while (!list_isEmpty(l)){
    list_remove(l, n);
  }
  free(l -> head);
  free(l);
}
