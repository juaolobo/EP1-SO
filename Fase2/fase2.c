#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

#define MAX 100

int writeFile(Process * finishedProcess, char * fileName, int time) {
  FILE * outputFile;

  outputFile = fopen(fileName, "w");

  if (outputFile == NULL) {
    printf("Erro ao criar o arquivo");
    exit(1);
  }
  finishedProcess->timesPaused = 0;
  fprintf(outputFile, "%s %d %d\n%d\n", finishedProcess->name, time, (time - finishedProcess->t0), finishedProcess->timesPaused);

  return 0;
}

int firstComeFirstServed(List * processList, char * fileName, int descrip) {
  printf("filinha");
  writeFile(processList[0].info, fileName, 12);

  return 1;
}

int shortestRemainingTime(List * processList, char * fileName, int descrip) {
  printf("tempo restante");
  return 1;
}

int roundRobin(List * processList, char * fileName, int descrip) {
  printf("Robin Hood");

  return 1;
}


int main(int argc, char **argv) {
  FILE * inputFile;
  List processList[MAX];
  int descrip = 0;

  inputFile = fopen(argv[1], "r");

  if (inputFile == NULL) {
    printf("Erro ao abrir o arquivo");
    exit(1);
  }

  for (int i = 0; !feof(inputFile); i++) {
    Process * currentProcess = malloc(sizeof(Process));
    if (fscanf(inputFile, "%s %d %d %d", currentProcess->name, &currentProcess->t0, &currentProcess->simTime, &currentProcess->deadline)) {
      processList[i].info = currentProcess;
    }
  }

  // for (int j = 0; j < 6; j++)
  //   printf("%s\n", processList[j].info->name);

  if (argv[4] != NULL)
    descrip = 1;

  if (atoi(argv[2]) == 1)
    firstComeFirstServed(processList, argv[3], descrip);
    
  if (atoi(argv[2]) == 2)
    shortestRemainingTime(processList, argv[3], descrip);

  if (atoi(argv[2]) == 3)
    roundRobin(processList, argv[3], descrip);

  
 
  return 1;
}