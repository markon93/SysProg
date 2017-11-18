/* This is an implementation of the queue datatype using a single
linked list.

Author: Marko Nygård, oi12mnd@cs.umu.se
12.10.2017
*/
#include "queue.h"
#include <stdio.h>

/* Creates a new queue.
	- returns: A pointer to the new queue.
*/
queue* q_create(){
	return list_create();
}

/* Removes the first element from the queue.
	- q: The queue.
*/
void q_dequeue(queue* q){
  node* p = list_first(q);
	list_remove(q, p);
}

/* Adds an element to the end of a queue.
	- element: The element to add to the queue.
	- q: The queue to add the element to.
*/
void q_enqueue(queue* q, void* element){
	node* n = list_first(q);

	while(!list_end(q,n)){
		n = list_next(q,n);
	}
	list_insert(q, n, element);
}

/* Checks if a queue is empty.
	- q: The queue.
	- returns: true if the q is empty, false otherwise.
*/
bool q_isEmpty(queue* q){
	return list_isEmpty(q);
}

/* Looks up the value of the first element in the queue.
	- q: The queue.
	- returns: Pointer to the element.
*/
void* q_peek(queue* q){
	return list_getValue(q, list_first(q));
}

/* Deletes all of the elements in a queue and removes it from memory.
	- q: The queue.
	- returns: 0 if successful, -1 otherwise.
*/
void q_free(queue* q){
	list_free(q);
}
