#include "../lib/timer.h" 

#ifndef __PROCESS_H
#define __PROCESS_H

typedef struct process {
    int pid, status;
    TIMER_T start, finish;
} Process;

Process * createProcess(int pid);
void freeProcess(Process * p);
float processTime(Process * p);
int status(Process * p);
int getPid(Process * p);
void printProcess(Process * p);

#endif