/* This is a header file for an implementation of the dataype queue using as basis a linked list.

Author: Marko Nygård, oi12mnd@cs.umu.se
12.10.2017
*/

#include "list.h"

#ifndef _QUEUE_
#define _QUEUE_

typedef list queue;

/* Creates a new queue.
	- returns: A pointer to the new queue.
*/
queue* q_create();

void queue_setMemHandler(queue *q, memFreeFunc *f);


/* Adds an element to the end of a queue.
	- element: The element to add to the queue.
	- q: The queue to add the element to.
*/
void q_enqueue(queue* q, void* element);

/* Removes the first element from the queue.
	- q: The queue.
*/
void q_dequeue(queue* q);

/* Checks if a queue is empty.
	- q: The queue.
	- returns: true if the q is empty, false otherwise.
*/
bool q_isEmpty(queue* q);

/* Looks up the value of the first element in the queue.
	- q: The queue.
	- returns: Pointer to the element.
*/
void* q_peek(queue* q);

/* Deletes all of the elements in a queue and removes it from memory.
	- q: The queue.
	- returns: 0 if successful, -1 otherwise.
*/
void q_free(queue* q);

#endif
