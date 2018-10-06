#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Process.h"
#include "list.h"
#include "../lib/commandlinereader.h"

#define MAXARGS 8

void manageProcesses(char ** args);
void newProcess(char ** args);
Node updateStatus(int state, int pid, Node h);

int MAXCHILDREN, currentProcesses = 0;
Node liveProcesses, deadProcesses;

/*******************************************************************************
 * main
 *******************************************************************************
*/
int main(int argc, char * argv[]) {
    char **args, *buf;

    buf = malloc(sizeof(char) * 10000);
    args = calloc(MAXARGS+1, sizeof(char *));
    liveProcesses = createNode(NULL);   //List of processes running
    deadProcesses = createNode(NULL);   //List of processes that have finished
    if (argc > 1) {
        sscanf(argv[1], "%d", &MAXCHILDREN);
    } else 
        MAXCHILDREN = 0;

    while(1) {
        readLineArguments(args, MAXARGS, buf, 10000);
        if (!strcmp(args[0], "exit"))
            break;
        else if (!strcmp(args[0], "run"))  {
            args[0] = "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";
            manageProcesses(args);
        } else 
            printf("Invalid arguments\n");
    }

    //After exit, we must finish all tasks and free memory
    int pid, state;
    while(next(liveProcesses) != NULL) {
        pid = wait(&state);

        Node new = updateStatus(state, pid, liveProcesses);
        
        insert(deadProcesses, new);
        removeByPID(pid, liveProcesses);
    }

    printAll(deadProcesses);
    printf("END.\n");

    freeAll(liveProcesses);
    freeAll(deadProcesses);
    free(buf);
    free(args);	 
}

/*******************************************************************************
 * manageProcesses
 *******************************************************************************
*/
void manageProcesses(char ** args) {
    int pid, state;
    //Can start right away
    if (!MAXCHILDREN || (MAXCHILDREN && (currentProcesses < MAXCHILDREN)))
        newProcess(args);   

    else if (currentProcesses >= MAXCHILDREN) {           //Need to wait for  
        pid = wait(&state);                               //a process to finish
        Node new = updateStatus(state, pid, liveProcesses);
        currentProcesses--;
        insert(deadProcesses, new);        
        removeByPID(pid, liveProcesses);
        newProcess(args);
    }
}

/*******************************************************************************
 * newProcess
 *******************************************************************************
*/
void newProcess(char ** args) {
    int pid;
    pid = fork();
    if (pid < 0) 
        abort();

    else if (pid == 0) {            //Child process executes a new seq-solver
        execv(args[0], args);
	    abort();
    } else {
        Process * p = createProcess(pid);           //Creates new process and 
        insert(liveProcesses, createNode(p));       //adds it to the list
        currentProcesses++;
    }
}

/*******************************************************************************
 * updateStatus
 *******************************************************************************
*/
Node updateStatus(int state, int pid, Node h) {
    int status = -1;            //NOK

    if (WIFEXITED(state))       //If process exited properly, status is OK
        status = 0;

    Node new = createNode(getByPID(pid, liveProcesses));
    new->item->status = status;

    return new;
}
