#include <time.h> 

#ifndef __PROCESS_H
#define __PROCESS_H

typedef struct process {
    int pid, status;
    clock_t duration;
} Process;

/*******************************************************************************
 * createProcess
 * Input: pid
 * Output: Pointer to process with id equal to pid
 *******************************************************************************
*/
Process * createProcess(int pid, int status, clock_t sysTime, clock_t usrTime);

/*******************************************************************************
 * freeProcess
 * Input: Pointer to process to free
 * Output: void
 *******************************************************************************
*/
void freeProcess(Process * p);

/*******************************************************************************
 * processTime
 * Input: Pointer to process
 * Output: Number of seconds that went by since the process started running 
 * untill its father realized it had finished
 *******************************************************************************
*/
float processTime(Process * p);

/*******************************************************************************
 * status
 * Input: Pointer to process
 * Output: Process termination status
 *******************************************************************************
*/
int status(Process * p);

/*******************************************************************************
 * getPid
 * Input: Pointer to process
 * Output: Process id
 *******************************************************************************
*/
int getPid(Process * p);

/*******************************************************************************
 * printProcess
 * Input: Pointer to process
 * Output: void
 * Prints the project pointed by p in the following format: 
 * "CHILD EXITED (PID=p->pid; return OK / NOK)" - according to exit status 
 *******************************************************************************
*/
void printProcess(Process * p);

#endif