#include <stdio.h>
#include <stdlib.h>
#include "output.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "queue.h"

#define MAX 100
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
int threadAmount = 0;
int finishedProcess[MAX];

void * thread(void *process);
int firstComeFirstServed(List * processList, char * fileName, int descriptive);
int shortestRemainingTime(List * processList, char * fileName, int descriptive);
int roundRobin(List * processList, char * fileName, int descriptive);
void freeList(List *processList);

int main(int argc, char **argv) {
  FILE * inputFile;
  List processList[MAX];
  int descriptive = 0;

  pthread_mutex_init(&mutex1, NULL);
  pthread_mutex_init(&mutex2, NULL);
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
    }

  }

  fclose(inputFile);

  processList->numProcess = processList->numProcess - 1; // quando o fscanf falha, o n é acrescentado ainda

  if (argv[4] != NULL)
    descriptive = 1;

  if (atoi(argv[2]) == 1)
    firstComeFirstServed(processList, argv[3], descriptive);
    
  if (atoi(argv[2]) == 2)
    shortestRemainingTime(processList, argv[3], descriptive);

  if (atoi(argv[2]) == 3)
    roundRobin(processList, argv[3], descriptive);

  pthread_mutex_destroy(&mutex1);  
  freeList(processList);
 
  return 1;
}

void * thread(void *process) {
  long int operation; 
  time_t startingTime;
  double timePast = 0;
  Process * threadProcess = process;

  time(&startingTime);

  while (timePast < threadProcess->simTime) {
    pthread_mutex_lock(&mutex1);
    operation++;
    pthread_mutex_unlock(&mutex1);
    timePast = difftime(time(NULL), startingTime);
    threadProcess->timePast = timePast;
  }

  threadProcess->finishedTime = timePast + threadProcess->t0;
  pthread_mutex_lock(&mutex2);
  threadAmount--;
  pthread_mutex_unlock(&mutex2);
  return NULL;
} 

int firstComeFirstServed(List * processList, char * fileName, int descriptive) {
  FILE * outputFile;
  pthread_t tid[MAX];
  time_t startingTime;
  double timePast = 0;
  int contextChanges = processList->numProcess - 1;
  int i = 0;

  outputFile = fopen(fileName, "w");
  time(&startingTime);
  
  while (i < processList->numProcess) {
    if (timePast == processList[i].info->t0) {
      if(pthread_create(&tid[i], NULL, thread, processList[i].info)) {
        printf("Erro ao tentar criar as threads \n");
        exit(1);
      }
      i++;
    }
    timePast = difftime(time(0), startingTime);
  }

  for (int i = 0; i < processList->numProcess; i++){
    if(pthread_join(tid[i], NULL)) {
      printf("Erro ao juntar as threads\n");
      exit(1);
    }
    writeFile(processList[i].info, outputFile);
  }

  fprintf(outputFile, "%d", contextChanges);
  fclose(outputFile);

  return 1;
}

int shortestRemainingTime(List * processList, char * fileName, int descriptive) {
  printf("tempo restante");
  // Queue * queue = NULL;
  // Queue * aux = NULL;
  FILE * outputFile;
  pthread_t tid[MAX];
  time_t startingTime;
  double timePast = 0;
  int contextChanges = 0;
  int i = 0;
  int maxTimeIndex = 0;
  Queue *q = initQ();

  outputFile = fopen(fileName, "w");
  time(&startingTime);

  while (i < processList->numProcess) {
    if (timePast == processList[i].info->t0) {

      if (threadAmount == 8) {
        for (int j = 0; j < i; j++) // acha o processo com maior tempo de execução faltante que está rodando
          if (timePast < processList[j].info->finishedTime) 
            if ((processList[j].info->simTime - processList[j].info->timePast) > (processList[maxTimeIndex].info->simTime - processList[maxTimeIndex].info->timePast))
              maxTimeIndex = j;
        
        if ((processList[maxTimeIndex].info->simTime - processList[maxTimeIndex].info->timePast) > processList[i].info->simTime) { 
          pthread_kill(tid[maxTimeIndex], SIGSTOP);
          contextChanges++;

          insertQueue(q, maxTimeIndex, processList[maxTimeIndex].info->simTime - processList[maxTimeIndex].info->timePast);

          // aux = malloc(sizeof(aux));
          // aux->index = maxTimeIndex;
          // aux->next = NULL;

          // if (queue != NULL) {
          //   aux->next = queue;
          //   queue = aux;
          // }

          // else {
          //   queue = aux;
          // }

          // aux = NULL;
          threadAmount--;
        }
      }

      if (pthread_create(&tid[i], NULL, thread, processList[i].info)) {
        printf("Erro ao tentar criar as threads \n");
        exit(1);
      }

      else
        threadAmount++;

      i++;

    }

    else {
      if (threadAmount < 8) {
        if (!queueEmpty(q)) {
          int index = removeQueue(q);
        // aux = queue;
        // queue = aux->next;
        // aux->next = NULL;

          if (pthread_kill(tid[index], SIGCONT)) {
            printf("Erro ao tentar criar as threads \n");
            exit(1);
          }
        }
        
        threadAmount++;
      }
    }

    timePast = difftime(time(0), startingTime);
  }


  for (int i = 0; i < processList->numProcess; i++) {
    if(pthread_join(tid[i], NULL)) {
      printf("Erro ao juntar as threads\n");
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
