typedef struct Process {
  char name[30];
  int t0;
  int simTime;
  int deadline;
  int timesPaused;

} Process;

typedef struct List {
  Process * info;
  int numProcess;
  List * next;
} List;
