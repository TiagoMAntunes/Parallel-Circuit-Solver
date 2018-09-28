#include <time.h> 

#ifndef __PROCESS_H
#define __PROCESS_H

typedef struct process {
    int pid, status;
    time_t start, finish;
} Process;

Process * createProcess(int pid);
void freeProcess(Process * p);
int processTime(Process * p);
int status(Process * p);
int getPid(Process * p);

#endif