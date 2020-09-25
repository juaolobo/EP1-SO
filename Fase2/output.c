#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

void printArrival(Process * process) {
  fprintf(stderr, "%s %d %d %d\n", process->name, process->t0, process->simTime, process->deadline);
}

int getCPUID() {
	return 0;
}

float getCPU(int id){
	return 0;
}

void printCPUConsumption() {
  fprintf(stderr, "to be continued\n");
}

void printCPUDeparture(Process * process){
  fprintf(stderr, "to be continued\n");
}

void printDeparture(Process * process, int time){
  fprintf(stderr, "O %s acabou de ser executado\n", process->name);	
  fprintf(stderr, "%s %d %d\n", process->name, time, (time - process->t0));
}

void printContextChanges(int contextChanges) {
  fprintf(stderr, "Ocorrereu uma mudança de contexto. TOTAL : %d\n", contextChanges);

}

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