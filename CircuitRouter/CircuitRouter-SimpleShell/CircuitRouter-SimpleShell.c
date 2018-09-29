#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Process.h"
#include "list.h"

void manageProcesses(char * filename);
void newProcess(char * filename);

int MAXCHILDREN, currentProcesses = 0, count = 0;
Node liveProcesses, deadProcesses;

int main(int argc, char * argv[]) {
    char * input  = calloc(10000, sizeof(char)); //TODO: Make adaptive function to fit any size of input 
    char * command, *filename;

    liveProcesses = createNode(NULL); //list of processes running
    deadProcesses = createNode(NULL); //list of processes that have finished
    if (argc > 1) {
        sscanf(argv[1], "%d", &MAXCHILDREN);
    } else 
        MAXCHILDREN = 0;

    while(1) {
        fgets(input, 10000, stdin);
        command = strtok(input, " \n\0");
        if (!strcmp(command, "run")) {
            filename = strtok(NULL, " \n");
            manageProcesses(filename);
        } else if (!strcmp(command, "exit"))
            break;
        else {
            printf("Invalid command\n");
        }
    }

    //after exit, we must finish all tasks and free memory
    int pid, state;
    while(next(liveProcesses) != NULL) {
        pid = wait(&state);
        insert(deadProcesses, createNode(getByPID(pid, liveProcesses)));
        removeByPID(pid, liveProcesses);
    }

    freeAll(liveProcesses);
    freeAll(deadProcesses);
    free(input);
}

void manageProcesses(char * filename) {
    int pid, state;
    //printf("%d\n", currentProcesses); //debug only

    if (!MAXCHILDREN || MAXCHILDREN && currentProcesses < MAXCHILDREN) { //can start right away
        newProcess(filename);
    } else if (currentProcesses >= MAXCHILDREN) { //need to wait for a process to finish
        pid = wait(&state);
        currentProcesses--;
        insert(deadProcesses, createNode(getByPID(pid, liveProcesses)));
        removeByPID(pid, liveProcesses);
        newProcess(filename);
    }
}

void newProcess(char * filename) {
    int pid, state;
    pid = fork();
    if (pid < 0) {
        abort();
    }
    else if (pid == 0) { //child process executes a new seq-solver
        //printf("Creating process with file %s\n", filename);
        execl("../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver", filename,0);
    } else {
        Process * p = createProcess(pid); //creates new process and adds it to the list
        insert(liveProcesses, createNode(p));
        currentProcesses++;
        count++; 
    }
}
