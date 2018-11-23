#include "process.h"
#include <time.h> 
#include <stdlib.h>
#include <stdio.h>

/*******************************************************************************
 * createProcess
 *******************************************************************************
*/
Process * createProcess(int pid, int status, clock_t sysTime, clock_t usrTime) {
    Process * p = malloc(sizeof(struct process));
    p->pid = pid;
    p->status = status;
    p->duration = sysTime;
    return p;
}

/*******************************************************************************
 * freeProcess
 *******************************************************************************
*/
void freeProcess(Process * p) {
    free(p);
}

/*******************************************************************************
 * processTime
 *******************************************************************************
*/
float processTime(Process * p) {
    return p->duration;// / CLOCKS_PER_SEC;
}

/*******************************************************************************
 * status
 *******************************************************************************
*/
int status(Process * p) {
    return p->status;
}

/*******************************************************************************
 * getPid
 *******************************************************************************
*/
int getPid(Process * p) {
    return p->pid;
}

/*******************************************************************************
 * printProcess
 *******************************************************************************
*/
void printProcess(Process * p) {
    printf("CHILD EXITED (PID=%d; return %s; %.0f)\n", getPid(p), 
                                    (status(p) == 0 ? "OK" : "NOK"), processTime(p));
}