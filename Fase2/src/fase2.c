#include <stdio.h>
#include <stdlib.h>
#include "output.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"
#include "lista.h"
#include <sched.h>

#define MAX 100

pthread_mutex_t mutexVector[MAX];

int threadAmount = 0;
int descriptive = 0;

void * thread(void *process);
int firstComeFirstServed(List * processList, char * fileName);
int shortestRemainingTime(List * processList, char * fileName);
int roundRobin(List * processList, char * fileName);
void flushQueue(int index, List * processList, pthread_t * tid, double timePast); 
void freeList(List *processList);

int main(int argc, char **argv) {
  FILE * inputFile;
  List processList[MAX];

  processList->numProcess = 0;
  inputFile = fopen(argv[1], "r");


  if (inputFile == NULL) {
    printf("Erro ao abrir o arquivo");
    exit(1);
  }

  for (int i = 0; !feof(inputFile); i++) {
    Process * currentProcess = malloc(sizeof(Process));
    if (fscanf(inputFile, "%s %d %d %d", currentProcess->name, &currentProcess->t0, &currentProcess->simTime, &currentProcess->deadline)) {
      processList->numProcess++;
      processList[i].info = currentProcess;
      processList[i].info->index = i;
      processList[i].info->timePast = 0;
      processList[i].info->paused = 0;      
    }
  }

  fclose(inputFile);

  processList->numProcess = processList->numProcess - 1; // quando o fscanf falha, o n é acrescentado ainda

  for (int i = 0; i < processList->numProcess; i++) {
    pthread_mutex_init(&mutexVector[i], NULL);
  }

  if (argv[4] != NULL)
    descriptive = 1;

  if (atoi(argv[2]) == 1)
    firstComeFirstServed(processList, argv[3]);
    
  if (atoi(argv[2]) == 2)
    shortestRemainingTime(processList, argv[3]);

  if (atoi(argv[2]) == 3)
    roundRobin(processList, argv[3]);

  freeList(processList);

  for (int i = 0; i < processList->numProcess; i++) {
    pthread_mutex_destroy(&mutexVector[i]);
  }
 
  return 0;
}

void * thread(void *process) {
  Process * threadProcess = process;
  time_t startingTime;
  long int operation; 
  int timePast = 0, timePast_last = 0, delay = 0;
  int waspaused = 0, total;

  time(&startingTime);
  if (descriptive) {
    printCPUArrival(threadProcess);
    printArrival(threadProcess);
  }

  while (timePast < threadProcess->simTime) {
    timePast_last = timePast;
    pthread_mutex_lock(&mutexVector[threadProcess->index]);
    operation++;
    pthread_mutex_unlock(&mutexVector[threadProcess->index]);

    if (waspaused == 0){
      timePast = difftime(time(NULL), startingTime);
    }
    else {
      timePast = difftime(time(NULL), startingTime) - delay;
    }

    if ((timePast - timePast_last) > 1) {
      waspaused = 1;
      delay = timePast - timePast_last;
      timePast_last = timePast = timePast - timePast_last;
      threadProcess->timePast = timePast;

    }
    else {
      threadProcess->timePast = timePast;
    }
  }

  total = difftime(time(NULL), startingTime);
  threadProcess->finishedTime = threadProcess->startTime + total;
  threadAmount--;
  if (descriptive) {
    printCPUDeparture(threadProcess);
    printDeparture(threadProcess);
  }

  return NULL;
} 

int firstComeFirstServed(List * processList, char * fileName) {
  FILE * outputFile;
  pthread_t tid[MAX];
  int timePast = 0;
  int contextChanges = 0;
  int i = 0;
  outputFile = fopen(fileName, "w");

  while (i < processList->numProcess) {
    if (timePast >= processList[i].info->t0) {
      if (threadAmount == 0) {
        threadAmount++;
        processList[i].info->startTime = timePast;

        if (pthread_create(&tid[i], NULL, thread, processList[i].info)) {
          printf("Erro ao tentar criar as threads \n");
          exit(1);
        }

        if (i > 0 && processList[i].info->t0 <= processList[i - 1].info->finishedTime)
          contextChanges++;

        i++;
      }
    }

    sleep(1);
    timePast = timePast + 1;
  }

  for (int i = 0; i < processList->numProcess; i++) {
    if (pthread_join(tid[i], NULL)) {
      printf("Erro ao entrar na thread\n");
      exit(1);
    }

    writeFile(processList[i].info, outputFile);
  }

  fprintf(outputFile, "%d", contextChanges);
  fclose(outputFile);

  return 1;
}

int shortestRemainingTime(List * processList, char * fileName) {
  FILE * outputFile;
  pthread_t tid[MAX];
  int timePast = 0;
  int contextChanges = 0;
  int i = 0;
  int index = 0, lastindex = -1;

  Queue *q = initQ();

  outputFile = fopen(fileName, "w");

  while (i < processList->numProcess) {

    while (processList[i].info->t0 == timePast) {
      insertQueue(q, i, processList[i].info->simTime, SORTED);
      i++;
    }

    if (threadAmount == 0) {
      if (!queueEmpty(q)) {
        lastindex = index;
        index = removeQueue(q);
        threadAmount++;

        if (processList[index].info->paused){
          pthread_mutex_unlock(&mutexVector[index]);
          processList[index].info->paused = 0;
        } 

        else {

          processList[index].info->startTime = timePast;
          if (pthread_create(&tid[index], NULL, thread, processList[index].info)) {
            printf("Erro ao tentar criar as threads \n");
            exit(1);
          }
        }
        
        if (processList[index].info->startTime == processList[lastindex].info->finishedTime){
          contextChanges++;
        }
      }
    }

    if (threadAmount == 1) {
      if (!queueEmpty(q)) {
        int currentTimeLeft = (processList[index].info->simTime - processList[index].info->timePast);
        int top = topQueue(q);
        if (processList[top].info->t0 == timePast) { // detecta preempção na chegada
          if (processList[top].info->simTime < currentTimeLeft) {
            contextChanges++;
            top = removeQueue(q);
            pthread_mutex_lock(&mutexVector[index]);
            insertQueue(q, index, currentTimeLeft, SORTED);
            processList[index].info->paused = 1;
            processList[top].info->startTime = timePast;
            if (pthread_create(&tid[top], NULL, thread, processList[top].info)) {
              printf("Erro ao tentar criar as threads aqui\n");
              exit(1);
            }
            index = top;
          }
        }
      }
    }
    
    sleep(1);
    timePast = timePast + 1;
  }

  while (!queueEmpty(q)) {

    if (threadAmount == 0) {
      lastindex = index;
      index = removeQueue(q);
      if (processList[index].info->paused) {
        pthread_mutex_unlock(&mutexVector[index]);
      }

      else {
        processList[index].info->startTime = timePast;
        if (pthread_create(&tid[index], NULL, thread, processList[index].info)) {
          printf("Erro ao tentar criar as threads \n");
          exit(1);
        }
      }
      threadAmount++;
      contextChanges++;
    }
    sleep(1);
    timePast = timePast + 1;
  }

  freeQueue(q);

  for (int i = 0; i < processList->numProcess; i++) {
    if (pthread_join(tid[i], NULL)) {
      printf("Erro ao entrar na thread\n");
      exit(1);
    }
    writeFile(processList[i].info, outputFile);
  }

  fprintf(outputFile, "%d", contextChanges);
  fclose(outputFile);
  printContextChanges(contextChanges);
  return 1;
}

int roundRobin(List * processList, char * fileName) {
  FILE * outputFile;
  pthread_t tid[MAX];
  int quantum = 3;
  int timePast = 0;
  int contextChanges = 0;
  int finishedProcesses = 0, lastArrived = 0;
  int lastindex = -1;
  Queue * q = initQ();

  outputFile = fopen(fileName, "w");
  int index;
  int finishedSum = 0;
  while(finishedProcesses < processList->numProcess) {

    while(lastArrived < processList->numProcess && processList[lastArrived].info->t0 <= timePast) 
      lastArrived++;

    // printf("%d\n", lastArrived);
    finishedSum = 0;


    for (int p = lastArrived - 1; p >= 0; p--) {

      printf("%d %d %d\n", processList[p].info->simTime, processList[p].info->timePast, processList[p].info->finishedTime);
      if (processList[p].info->simTime <= processList[p].info->timePast) {
        finishedSum++;
      }
      else
        if (p == index) {
          if (find(q, p) == -1 && processList[p].info->simTime - processList[p].info->timePast > 0 && processList[p].info->timePast%quantum == 0)
            insertQueue(q, p, 0, NORMAL);
        }

        else if (find(q, p) == -1)
          insertQueue(q, p, 0, NORMAL);


    }   

    printf("fila ");
    printQ(q);
    printf("\n");

    finishedProcesses = finishedSum; 
    if (threadAmount == 1) {    
      lastindex = index;
      if (processList[index].info->timePast != processList[index].info->simTime && (processList[index].info->timePast) % quantum == 0) {
        pthread_mutex_lock(&mutexVector[index]);
        processList[index].info->paused = 1;
        threadAmount--;
      }

    }
    if (threadAmount == 0) {
      if (!queueEmpty(q)) {
        index = removeQueue(q);
        if (processList[index].info->paused) {
          if (index != lastindex){
            contextChanges++;
          }
          pthread_mutex_unlock(&mutexVector[index]);
          processList[index].info->paused = 0;
        }

        else {
          processList[index].info->startTime = timePast;

          if (pthread_create(&tid[index], NULL, thread, processList[index].info)) {
            printf("Erro ao tentar criar as threads \n");
            exit(1);
          }

          if (processList[index].info->t0 == processList[lastindex].info->finishedTime){
            contextChanges++;
          }

          else if (processList[lastindex].info->paused) {
            contextChanges++;
          }
          
        }
        threadAmount++;
      }
    }
    sleep(1);
    timePast++;
    printf("finished %d\n", finishedProcesses);
  }

  freeQueue(q);

  for (int i = 0; i < processList->numProcess; i++) {

    if (pthread_join(tid[i], NULL)) {
      printf("Erro ao entrar na thread\n");
      exit(1);
    }
    writeFile(processList[i].info, outputFile);
  }


  fprintf(outputFile, "%d", contextChanges);
  fclose(outputFile);

  return 1;
}

void freeList(List *processList) {
  for (int i = 0; i < processList->numProcess+1; i++)

  // é preciso da um free a mais pq o loop do fscanf aloca uma celula a mais quando o feof é atingindo
  free(processList[i].info);
}

void flushQueue(int index, List * processList, pthread_t * tid, double timePast) {

  if (processList[index].info->paused) {
    pthread_mutex_unlock(&mutexVector[index]);
  }

  else {
    processList[index].info->startTime = timePast;
    if (pthread_create(&tid[index], NULL, thread, processList[index].info)) {
      printf("Erro ao tentar criar as threads \n");
      exit(1);
    }
  }
  threadAmount++;
}