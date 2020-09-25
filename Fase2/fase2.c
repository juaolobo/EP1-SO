#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
#include <time.h>
#include <pthread.h>

#define MAX 100

pthread_mutex_t mutex1;

int writeFile(Process * finishedProcess, char * fileName, int time) {
  FILE * outputFile;

  outputFile = fopen(fileName, "w");

  if (outputFile == NULL) {
    printf("Erro ao criar o arquivo\n");
    exit(1);
  }
  finishedProcess->timesPaused = 0;
  fprintf(outputFile, "%s %d %d\n%d\n", finishedProcess->name, time, (time - finishedProcess->t0), finishedProcess->timesPaused);

  return 0;
}

void * thread(void *process) {

  long int i; 
  clock_t begin;
  double total_time;
  Process * thread_process = process;
  begin = clock();

  printf("%s %d %d %d %d\n", thread_process->name, thread_process->t0, thread_process->simTime, thread_process->deadline, thread_process->timesPaused);

  while (total_time <= thread_process->simTime) {
    pthread_mutex_lock(&mutex1);
    i++;
    pthread_mutex_unlock(&mutex1);
    total_time = (double)(clock() - begin) / CLOCKS_PER_SEC;
  }

  return NULL;

} 

int firstComeFirstServed(List * processList, char * fileName, int descriptive) {

  printf("filinha\n");
  pthread_t tid[MAX];
  clock_t begin;

  for (int i = 0; i < processList->n; i++){
    if(pthread_create(&tid[i], NULL, thread, processList[i].info)){
      printf("Erro ao tentar criar as threads \n");
      exit(1);
    }
  }

  begin = clock();
  for(int i = 0; i < processList->n; i++){
    if(pthread_join(tid[i], NULL)) {
      printf("Erro ao joinar a thread\n");
      exit(1);
    }
    total_time = (double)(clock() - begin) / CLOCKS_PER_SEC;
  }
  return 1;
}

int shortestRemainingTime(List * processList, char * fileName, int descriptive) {
  printf("tempo restante");
  return 1;
}

int roundRobin(List * processList, char * fileName, int descriptive) {
  printf("Robin Hood");

  return 1;
}


int main(int argc, char **argv) {

  FILE * inputFile;
  List processList[MAX];
  processList->n = 0;
  int descriptive = 0;

  inputFile = fopen(argv[1], "r");

  if (inputFile == NULL) {
    printf("Erro ao abrir o arquivo");
    exit(1);
  }

  for (int i = 0; !feof(inputFile); i++) {
    Process * currentProcess = malloc(sizeof(Process));
    if (fscanf(inputFile, "%s %d %d %d", currentProcess->name, &currentProcess->t0, &currentProcess->simTime, &currentProcess->deadline)) {
      processList[i].info = currentProcess;
      processList->n++;
    }
  }
  processList->n = processList->n -1; // quando o fscanf falha, o n é acrescentado ainda
  printf("%d\n", processList->n);

  if (argv[4] != NULL)
    descriptive = 1;

  if (atoi(argv[2]) == 1)
    firstComeFirstServed(processList, argv[3], descriptive);
    
  if (atoi(argv[2]) == 2)
    shortestRemainingTime(processList, argv[3], descriptive);

  if (atoi(argv[2]) == 3)
    roundRobin(processList, argv[3], descriptive);

  
 
  return 1;
}