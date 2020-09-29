#include <stdio.h>
#include <stdlib.h>
#include "output.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "queue.h"

#define MAX 100
pthread_mutex_t mutexVector[MAX];
int threadAmount = 0;
int finishedProcess[MAX];

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
 
  return 1;
}

void * thread(void *process) {
  Process * threadProcess = process;
  time_t startingTime, time2;
  long int operation; 
  int timePast = 0, timePast_last = 0, delay;
  int waspaused = 0, total;

  time(&startingTime);
  printf("comecei a contarm na thread %d\n", threadProcess->index + 1);
  while (timePast < threadProcess->simTime) {
    timePast_last = timePast;
    pthread_mutex_lock(&mutexVector[threadProcess->index]);
    operation++;
    pthread_mutex_unlock(&mutexVector[threadProcess->index]);

    if (waspaused == 0){
      timePast = difftime(time(NULL), startingTime);
    }
    else {
      timePast = difftime(time(NULL), startingTime) - difftime(time2, startingTime);
    }

    if ((timePast - timePast_last) > 1){
      waspaused = 1;
      time(&time2); 
      delay = difftime(time2, startingTime);
      timePast_last = timePast = timePast - delay;
    }
    else 
      threadProcess->timePast = timePast;
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
  int contextChanges = processList->numProcess - 1;
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
        i++;
      }
    }
    printf("to sleepando\n");
    sleep(1);
    timePast = timePast + 1;
  }

  for (int i = 0; i < processList->numProcess; i++) {
    if(pthread_join(tid[i], NULL)) {
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
  // time(&startingTime);

  while (i < processList->numProcess) {
    if (timePast >= processList[i].info->t0) {
      if (threadAmount == 0) {
        
        threadAmount++;
        
        printf("criamo %d\n", i+1);
        
        processList[i].info->startTime = timePast;
        
        if (pthread_create(&tid[i], NULL, thread, processList[i].info)) {
          printf("Erro ao tentar criar as threads \n");
          exit(1);
        }
        i++;
      }
      else if (threadAmount == 1) {
        printf("VAMO VER SE MUDA\n");

        int currentTimeLeft = (processList[i - 1].info->simTime - processList[i - 1].info->timePast);

        if (currentTimeLeft > 0) { // bug estranho do tempo 
        
          if (i > 0 && currentTimeLeft > processList[i].info->simTime) {

            contextChanges++;
            
            printf("Mudamos de %s para %s\n", processList[i - 1].info->name, processList[i].info->name);
            processList[i - 1].info->paused = 1;
            pthread_mutex_lock(&mutexVector[i - 1]);
            insertQueue(q, i - 1, currentTimeLeft);
            processList[i].info->startTime = timePast;

            if (pthread_create(&tid[i], NULL, thread, processList[i].info)) {
              printf("Erro ao tentar criar as threads \n");
              exit(1);
            }
          }

          else {
            insertQueue(q, i, processList[i].info->simTime);
            printf("NU MUDAMO\n");
          }
          i++;
        }
      }
    }
    
    // não chegou processo, mas não tem nada rodando então tem que ver se tem algo na fila
    else
      flushQueue(q, processList, &contextChanges, tid, timePast);

    printf("to sleepando\n");
    sleep(1);

    timePast = timePast +1;
  }

  while (!queueEmpty(q)) {
    printf("%f\n", timePast);
    flushQueue(q, processList, &contextChanges, tid, timePast);
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

int roundRobin(List * processList, char * fileName, int descriptive) {
  printf("Robin Hood");

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