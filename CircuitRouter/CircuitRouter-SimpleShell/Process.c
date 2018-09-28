#include "Process.h"
#include <time.h>
#include <stdlib.h>

Process * createProcess(int pid) {
    Process * p = malloc(sizeof(struct process));
    p->pid = pid;
    p->status = -1;
    p->start = -1;
    p->finish = -1;
    return p;
}

void freeProcess(Process * p) {
    free(p);
}

int processTime(Process * p) {
    return p->finish - p->start;
}

int status(Process * p) {
    return p->status;
}

int getPid(Process * p) {
    return p->pid;
}