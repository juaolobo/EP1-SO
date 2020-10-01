#include <stdio.h>
#include <stdlib.h>
#include "output.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"
#include "lista.h"

#define MAX 100

pthread_mutex_t mutexVector[MAX];

int threadAmount = 0;

void * thread(void *process);
int firstComeFirstServed(List * processList, char * fileName, int descriptive);
int shortestRemainingTime(List * processList, char * fileName, int descriptive);
int roundRobin(List * processList, char * fileName, int descriptive);
void flushQueue(Queue * q, List * processList, int *contextChanges, pthread_t * tid, double timePast); 
void freeList(List *processList);

int main(int argc, char **argv) {
  FILE * inputFile;
  List processList[MAX];
  int descriptive = 0;

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
    firstComeFirstServed(processList, argv[3], descriptive);
    
  if (atoi(argv[2]) == 2)
    shortestRemainingTime(processList, argv[3], descriptive);

  if (atoi(argv[2]) == 3)
    roundRobin(processList, argv[3], descriptive);

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
  printf("comecei a contar na thread %d\n", threadProcess->index + 1);

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

    if ((timePast - timePast_last) > 1){
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
  printf("THREAD %d finalizada\n", threadProcess->index + 1);
  return NULL;
} 

int firstComeFirstServed(List * processList, char * fileName, int descriptive) {
  FILE * outputFile;
  pthread_t tid[MAX];
  int timePast = 0;
  int contextChanges = 0;
  int i = 0;
  int j = 1;
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

        i++;
      }

      else {
        if (j < processList->numProcess && timePast >= processList[j].info->t0) {
          j++;
          contextChanges++;
        }
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

int shortestRemainingTime(List * processList, char * fileName, int descriptive) {
  FILE * outputFile;
  pthread_t tid[MAX];
  int timePast = 0;
  int contextChanges = processList->numProcess - 1;
  int i = 0;

  Queue *q = initQ();

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

        i++;
      }

      else if (threadAmount == 1) {
        int currentTimeLeft = (processList[i - 1].info->simTime - processList[i - 1].info->timePast);

        if (currentTimeLeft > 0) { 
          if (i > 0 && currentTimeLeft > processList[i].info->simTime) {
            contextChanges++;
            processList[i - 1].info->paused = 1;
            pthread_mutex_lock(&mutexVector[i - 1]);
            insertQueue(q, i - 1, currentTimeLeft, SORTED);
            processList[i].info->startTime = timePast;

            if (pthread_create(&tid[i], NULL, thread, processList[i].info)) {
              printf("Erro ao tentar criar as threads \n");
              exit(1);
            }
          }

          else {
            insertQueue(q, i, processList[i].info->simTime, SORTED);
            printf("NU MUDAMO\n");
          }
          i++;
        }
      }
    }
    
    // não chegou processo, mas não tem nada rodando então tem que ver se tem algo na fila
    else
      flushQueue(q, processList, &contextChanges, tid, timePast);

    sleep(1);

    timePast = timePast +1;
  }

  while (!queueEmpty(q)) {
    flushQueue(q, processList, &contextChanges, tid, timePast);
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

  return 1;
}

int roundRobin(List * processList, char * fileName, int descriptive) {
  printf("Robin Hood\n");
  FILE * outputFile;
  pthread_t tid[MAX];
  int quantum = 2;
  int timePast = 0;
  int contextChanges = 0;
  int finishedProcesses = 0, lastArrived = 0;
  Queue * q = initQ();

  outputFile = fopen(fileName, "w");
  int index;
  int finishedSum = 0;
  while(finishedProcesses < processList->numProcess) {

    // descobre o ultimo processo que chegou
    while(lastArrived < processList->numProcess && processList[lastArrived].info->t0 <= timePast) {
      lastArrived++;
    }
    printf("%d\n", lastArrived);
    finishedSum = 0;
    for (int p = lastArrived - 1; p >= 0; p--){

      if (processList[p].info->simTime == processList[p].info->timePast){
        printf("%s %d %d\n",processList[p].info->name, processList[p].info->simTime, processList[p].info->timePast);
        printf("%d %d\n",processList[p].info->finishedTime, processList[p].info->startTime);
        finishedSum++;
      }
      else
        insertQueue(q, p, 0, NORMAL);
    } 
    finishedProcesses = finishedSum; 
    printf("finished %d\n", finishedProcesses);
    if (threadAmount == 1) {    
      // printf("index %d\n", index);
      if ((processList[index].info->timePast) % quantum == 0) {
        contextChanges++;
        pthread_mutex_lock(&mutexVector[index]);
        processList[index].info->paused = 1;
        threadAmount--;
      }
    }

    // printf("to entre %d\n", threadAmount);
    if (threadAmount == 0) {
      if (!queueEmpty(q)) {
        index = removeQueue(q);
        if (processList[index].info->paused) {
          contextChanges++;
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
    }
    sleep(1);
    timePast++;
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

void flushQueue(Queue * q, List * processList, int * contextChanges, pthread_t * tid, double timePast) {
  if (!queueEmpty(q) && threadAmount == 0) {
    printf("VAMO DESCARREGAR ESSA FILA DESPAUSANDO O PESSOAR\n");

    int index = removeQueue(q);

    if (processList[index].info->paused) {
      contextChanges++;
      pthread_mutex_unlock(&mutexVector[index]);
    }

    else {
      printf("ESSA GALERA NEM RODO AINDA\n");
      printf("%f\n", timePast);
      processList[index].info->startTime = timePast;
      if (pthread_create(&tid[index], NULL, thread, processList[index].info)) {
        printf("Erro ao tentar criar as threads \n");
        exit(1);
      }
    }
    threadAmount++;
  }
}