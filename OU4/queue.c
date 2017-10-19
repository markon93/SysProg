/* */
#include "queue.h"
#include <stdio.h>

/*  */
queue* q_create(){
	return list_create();
}

/* */
void q_dequeue(queue* q){
  node* p = list_first(q);
	list_remove(q, p);
}

/* */
void q_enqueue(queue* q, void* element){
	node* n = list_first(q);

	//while(!list_end(q,n)){
	//	n = list_next(q,n);
	//}
	list_insert(q, n, element);
}

/* */
bool q_isEmpty(queue* q){
	return list_isEmpty(q);
}

/* */
void* q_peek(queue* q){
	return list_getValue(q, list_first(q));
}

/* */
void q_free(queue* q){
	list_free(q);
}
