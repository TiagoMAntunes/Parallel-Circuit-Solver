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

int main(int argc, char * argv[]) {
    char **args, *buf;

    buf = malloc(sizeof(char) * 10000);
    args = calloc(MAXARGS+1, sizeof(char *));
    liveProcesses = createNode(NULL); //list of processes running
    deadProcesses = createNode(NULL); //list of processes that have finished
    if (argc > 1) {
        sscanf(argv[1], "%d", &MAXCHILDREN);
    } else 
        MAXCHILDREN = 0;

    while(1) {
        readLineArguments(args, MAXARGS, buf, 10000);
        if (*args != NULL && !strcmp(args[0], "exit"))
            break;
        else if (*args != NULL && !strcmp(args[0], "run"))  {
            args[0] = "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";
            manageProcesses(args);
        } else 
            printf("Invalid arguments\n");
    }

    //after exit, we must finish all tasks and free memory
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

void manageProcesses(char ** args) {
    int pid, state;
    //printf("%d\n", currentProcesses); //debug only
    if (!MAXCHILDREN || (MAXCHILDREN && (currentProcesses < MAXCHILDREN))) { //can start right away
        newProcess(args);   
    } else if (currentProcesses >= MAXCHILDREN) { //need to wait for a process to finish
        pid = wait(&state);
        Node new = updateStatus(state, pid, liveProcesses);
        currentProcesses--;
        insert(deadProcesses, new);        
        removeByPID(pid, liveProcesses);
        newProcess(args);
    }
}

void newProcess(char ** args) {
    int pid;
    pid = fork();
    if (pid < 0) {
        abort();
    }
    else if (pid == 0) { //child process executes a new seq-solver
        //printf("Creating process with file %s\n", filename);
        execv(args[0], args);
	    abort();
    } else {
        Process * p = createProcess(pid); //creates new process and adds it to the list
        insert(liveProcesses, createNode(p));
        currentProcesses++;
    }
}


Node updateStatus(int state, int pid, Node h) {
    int status = -1;

    if (WIFEXITED(state) && (WEXITSTATUS(state) == 0)) {
        status = 0;
    }

    Node new = createNode(getByPID(pid, liveProcesses));
    new->item->status = status;

    return new;
}
