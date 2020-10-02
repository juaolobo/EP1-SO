#define _GNU_SOURCE    
#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

void printArrival(Process * process) {
  fprintf(stderr, "%s %d %d %d\n", process->name, process->t0, process->simTime, process->deadline);
}

void printCPUArrival(Process * process) {
  int CPUID = sched_getcpu();
  fprintf(stderr, "%s começou a ocupar a CPU %d\n", process->name, CPUID);
}

void printCPUDeparture(Process * process){
  // fprintf(stderr, "to be continued\n");

  int CPUID = sched_getcpu();
  fprintf(stderr, "%s liberando a CPU %d\n", process->name, CPUID);
}

void printDeparture(Process * process){
  fprintf(stderr, "O %s acabou de ser executado\n", process->name);	
  fprintf(stderr, "%s %d %d\n", process->name, process->finishedTime, (process->finishedTime - process->startTime));
}

void printContextChanges(int contextChanges) {
  fprintf(stderr, "Ocorreram um total de %d mudanças de contexto.\n", contextChanges);
}

int writeFile(Process * finishedProcess, FILE * outputFile) {

  if (outputFile == NULL) {
    printf("Erro ao criar o arquivo\n");
    exit(1);
  }
  fprintf(outputFile, "%s %d %d\n", finishedProcess->name, finishedProcess->finishedTime, (finishedProcess->finishedTime - finishedProcess->startTime));

  return 0;
}