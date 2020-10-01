#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printArrival(Process * process) {
  fprintf(stderr, "%s %d %d %d\n", process->name, process->t0, process->simTime, process->deadline);
}

int getCPUID() {

    FILE* procFile = fopen("/proc/self/stat", "r");
    char fileRead[10000];

    fread(fileRead, sizeof(char), 10000, procFile);

    fclose(procFile);

    char* CPUIDstr = strtok(fileRead, " ");
    
    // o número da cpu utilizada se encontra no campo 39 do arquivo
    // splita a string em varios pedaços separados por espaço até o objetivo
    for (int i = 1; i < 38; i++)
        CPUIDstr = strtok(NULL, " ");

    CPUIDstr = strtok(NULL, " ");
    int CPUID = atoi(CPUIDstr);

    return CPUID;
}

void printCPUDeparture(Process * process){
  // fprintf(stderr, "to be continued\n");

  int CPUID = getCPUID();
}

void printDeparture(Process * process, int time){
  fprintf(stderr, "O %s acabou de ser executado\n", process->name);	
  fprintf(stderr, "%s %d %d\n", process->name, time, (time - process->t0));
}

void printContextChanges(int contextChanges) {
  fprintf(stderr, "Ocorrereu uma mudança de contexto. TOTAL : %d\n", contextChanges);

}

int writeFile(Process * finishedProcess, FILE * outputFile) {

  if (outputFile == NULL) {
    printf("Erro ao criar o arquivo\n");
    exit(1);
  }
  printf("%s %d %d %d\n", finishedProcess->name, finishedProcess->finishedTime, (finishedProcess->finishedTime - finishedProcess->startTime), finishedProcess->timePast);
  fprintf(outputFile, "%s %d %d\n", finishedProcess->name, finishedProcess->finishedTime, (finishedProcess->finishedTime - finishedProcess->startTime));

  return 0;
}