#ifndef FILA_h
#define FILA_h

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

Queue * initQ(Queue * queue);
int queueEmpty(Queue * queue);
int size(Queue * queue);
int queueFull(Queue * queue);
void insertQueue(Queue * queue, int index, int time);
int removeQueue(Queue * queue);

#endif