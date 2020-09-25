#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 100

pthread_mutex_t mutex1;

void * thread(void *process) {

  long int i; 
  time_t begin;
  double total_time = 0;
  Process * thread_process = process;
  time(&begin);


  while (total_time < thread_process->simTime) {
    pthread_mutex_lock(&mutex1);
    i++;
    pthread_mutex_unlock(&mutex1);
    total_time = difftime(time(NULL), begin);
  }

  return NULL;

} 

int firstComeFirstServed(List * processList, char * fileName, int descriptive) {

  printf("filinha\n");
  pthread_t tid[MAX];
  time_t begin;
  double cur_time = 0;
  int contextChanges = processList->numProcess - 1;

  time(&begin);
  int i = 0;
  while (i < processList->numProcess) {
    if (cur_time >= processList[i].info->t0){
      if(pthread_create(&tid[i], NULL, thread, processList[i].info)){
        printf("Erro ao tentar criar as threads \n");
        exit(1);
      }
      i++;
    }
    cur_time = difftime(time(0), begin);
  }

  for (int i = 0; i < processList->numProcess; i++){
    if(pthread_join(tid[i], NULL)) {
      printf("Erro ao juntar as threads\n");
      exit(1);
    }
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

void freeList(List *processList) {
  for (int i = 0; i < processList->numProcess+1; i++)

  // é preciso da um free a mais pq o loop do fscanf aloca uma celula a mais quando o feof é atingindo
    free(processList[i].info);
}

int main(int argc, char **argv) {

  FILE * inputFile;
  List processList[MAX];
  int descriptive = 0;

  pthread_mutex_init(&mutex1, NULL);
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