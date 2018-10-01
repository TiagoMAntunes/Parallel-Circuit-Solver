#include "Process.h"
#include "../lib/timer.h"
#include <stdlib.h>
#include <stdio.h>

Process * createProcess(int pid) {
    Process * p = malloc(sizeof(struct process));
    p->pid = pid;
    p->status = -1;
    return p;
}

void freeProcess(Process * p) {
    free(p);
}

float processTime(Process * p) {
    return TIMER_DIFF_SECONDS(p->start, p->finish);
}

int status(Process * p) {
    return p->status;
}

int getPid(Process * p) {
    return p->pid;
}

void printProcess(Process * p) {
    printf("CHILD EXITED (PID=%d; return %s)\n", getPid(p), 
                                    (status(p) == 0 ? "OK" : "NOK"));
}