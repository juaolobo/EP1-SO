#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdio.h>
#include "lista.h"

int getCPUID();
void printArrival(Process * process);
void printCPUArrival(Process * process);
void printCPUDeparture(Process * process);
void printDeparture(Process * process);
void printContextChanges(int contextChanges);
int writeFile(Process * finishedProcess, FILE * outputFile);

#endif
