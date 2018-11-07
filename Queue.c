#include <stdlib.h>
#include <stdio.h>
#include "FIFO_TEST.h"



Queue *q_init() {
	Queue *q = (Queue*)malloc(sizeof(Queue));
	if(q == NULL){
		printf("Memory Allocation Error");
		return NULL;
	}
	q->head = q->tail = NULL;
	q->size = 0;
	return q;
}

void q_enq(Queue *q, void *p) {


	QNode *NewNode = (QNode *) malloc(sizeof(QNode));
	NewNode->data = p;
	NewNode->next = NULL;

	if(NewNode == NULL){
		printf("Fatal Error- Can't add to lsit\n");
		return;
	}

	if(q->size == 0){
		q->head = NewNode;
		q->tail = NewNode;
		q->size++;
	}

	q->tail->next = NewNode;
	q->tail = NewNode;
	q->size++;

}


void *q_deq(Queue *q) {
	void *ret = NULL;
	if (q->size == 0) {
		printf("Cannot remove from an empty Queue\n");
		return ret;
	}
	QNode *pr = q->head;
	ret = pr->data;
	q->head = pr->next;
	if (pr == q->tail)
		q->tail = NULL;
	free(pr);
	q->size--;
	return ret;
}
