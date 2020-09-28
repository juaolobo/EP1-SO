#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

Queue *  initQ() {
	Queue * queue = malloc(sizeof(Queue));
	queue->start = NULL;
	queue->end = NULL;
	queue->nItems = 0;

	return queue;
}

int size(Queue *queue) {
	return queue->nItems;
}

int queueEmpty(Queue * queue) {
	if (queue != NULL)
		return queue->start == NULL;
	return 1;
}

void insertQueue(Queue *queue, int index, int time) {

	QNode * new = malloc(sizeof(QNode));
	QNode * aux = queue->start, * aux_prev;

	new->index = index;
	new->timeRemaining = time;
	new->next = NULL;

	if (!queueEmpty(queue)){

		while (aux != NULL && aux->timeRemaining <= time){
			aux_prev = aux;
			aux = aux->next;
		}

		if (aux == queue->start){ // caso 1 : new deve ser o inicio da fila
			new->next = aux;
			queue->start = new;
			return;
		}

		if (aux != NULL) { // caso 2 : new esta no meio da fila 
			new->next = aux->next;
			aux->next = new;
		}

		else { // caso 3 : new deve ser o ultimo da fila
			aux_prev->next = new;
		}

	}
	else 
		queue->start = new;

}

int removeQueue(Queue * queue) {

	int topIndex = queue->start->index;
	QNode * aux = queue->start;
	queue->start = queue->start->next;
	free(aux);
	queue->nItems--;

	return topIndex;
}

void printQ(Queue * queue) {
	QNode * aux = queue->start;

	while( aux != NULL){
		printf("%d\n", aux->index);
		aux = aux->next;
	}
}

void freeQ(Queue * queue) {
	QNode * aux = queue->start;
	while(aux != NULL) {
		aux = queue->start->next;
		free(queue->start);
		queue->start = aux;
	}

	free(queue);
}