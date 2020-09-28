#ifndef LISTA_H
#define LISTA_H

typedef struct Process {
  char name[30];
  int t0; 
  int simTime;
  int deadline;
  int finishedTime;
  int timePast;
  int startTime;
  int index;
  int paused;

} Process;

typedef struct List {
  Process * info;
  int numProcess;
} List;

#endif