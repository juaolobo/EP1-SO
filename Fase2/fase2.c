#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/getcpu.h>

#define MAX 100
#define OUTPUT_FILE

pthread_mutex_t mutex1;

int writeFile(Process * finishedProcess, char * fileName, int time) {
  FILE * outputFile;

  outputFile = fopen(fileName, "w");

  if (outputFile == NULL) {
    printf("Erro ao criar o arquivo\n");
    exit(1);
  }

  finishedProcess->timesPaused = 0;
  fprintf(outputFile, "%s %d %d\n", finishedProcess->name, time, (time - finishedProcess->t0));

  return 0;
}

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

  writeFile(process, OUTPUT_FILE, total_time)

  return NULL;

} 

void printArrival(Process * process) {
  fprintf(stderr, "%s %d %d %d\n", process->name, process->t0, process->simTime, process->deadline);
}

int getCPUID() {

}

float getCPU(int id){

}

void printCPUConsumption() {
  fprintf(stderr, "to be continued\n");
}

void printCPUDeparture(Process * process){
  fprintf(stderr, "to be continued\n");
}

void printDeparture(process * Process){
  fprintf(stderr, "O %s acabou de ser executado\n", process->name);
  fprintf(stderr, "%s %d %d\n", process->name, time, (time - process->t0));
}

void printContextChanges(int contextChanges) {
  fprintf(stderr, "Ocorrereu uma mudança de contexto. TOTAL : %d\n", contextChanges);
}

int firstComeFirstServed(List * processList, char * fileName, int descriptive) {

  printf("filinha\n");
  pthread_t tid[MAX];
  time_t begin;
  double cur_time;
  int contextChanges = processList->n - 1;

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
      processList[i].info = currentProcess;
      processList->numProcess++;
    }
  }

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
 
  return 1;
}