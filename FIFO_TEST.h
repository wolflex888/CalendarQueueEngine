#ifndef FIFO_QUEUE_
#define FIFO_QUEUE_


typedef struct queueNode {
	void *data;
	//double ts;
	//double ts_end;
	struct queueNode *next;
} QNode;


typedef struct {
	QNode *head;
	QNode *tail;
	int size;
} Queue;


Queue *q_init();
void q_enq(Queue *q, void *p);
void *q_deq(Queue *q);

#endif
