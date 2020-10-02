#ifndef QUEUE_H
#define QUEUE_H

#define SORTED 1
#define NORMAL 0

typedef struct QNode {
	int index;
	int timeRemaining;
	struct QNode * next;
} QNode;

typedef struct Queue {
	QNode * start;
	QNode * end;
	int nItems;
} Queue;

Queue * initQ();
int topQueue(Queue * queue);
int queueEmpty(Queue * queue);
int size(Queue * queue);
int find(Queue * queue, int index);
int queueFull(Queue * queue);
void insertQueue(Queue * queue, int index, int time, int sort);
int removeQueue(Queue * queue);
void freeQueue(Queue * queue);

#endif