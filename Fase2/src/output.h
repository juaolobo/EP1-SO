#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdio.h>
#include "lista.h"

int getCPUID();
void printArrival(Process * process);
float getCPU(int id);
void printCPUConsumption();
void printCPUDeparture(Process * process);
void printDeparture(Process * process, int time);
void printContextChanges(int contextChanges);
int writeFile(Process * finishedProcess, FILE * outputFile);

#endif
