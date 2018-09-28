#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "CircuitRouter-SimpleShell.h"
#include "Process.h"
#include "list.h"

int MAXCHILDREN, currentProcesses = 0, count = 0;

Node liveProcesses, deadProcesses;

int main(int argc, char * argv[]) {
    char * input  = calloc(10000, sizeof(char)); //TODO: Make adaptive function to fit any size of input 
    char * command, *filename;

    liveProcesses = createNode(NULL);
    deadProcesses = createNode(NULL);
    if (argc > 1) {
        sscanf(argv[1], "%d", &MAXCHILDREN);
    } else 
        MAXCHILDREN = 0;

    while(strcmp("exit\n", input)) {
        fgets(input, 10000, stdin);
        command = strtok(input, " ");
        if (!strcmp(command, "run")) {
            filename = strtok(NULL, " \n");
            manageProcesses(filename);
        } else {
            printf("Invalid command\n");
        }
    }
    int pid, state;
    while(liveProcesses != NULL) {
        pid = wait(&state);
        insert(deadProcesses, getByPID(pid, liveProcesses));
        removeByPID(pid, liveProcesses);
    }
    printf("Live processes:\n");
    print(liveProcesses);
    printf("Dead processes:\n");
    print(deadProcesses);
    free(input);
}

void manageProcesses(char * filename) {
    int pid, state;
    if (!MAXCHILDREN || MAXCHILDREN && currentProcesses < MAXCHILDREN) {
        newProcess(filename);
    } else if (currentProcesses >= MAXCHILDREN) { //need to wait for a process to finish
        printf("Waiting for a process...");
        pid = wait(&state);
        currentProcesses--;
        insert(deadProcesses, getByPID(pid, liveProcesses));
        removeByPID(pid, liveProcesses);
        newProcess(filename);
    }
}

void newProcess(char * filename) {
    int pid, state;
    pid = fork();
    printf("%d\n", pid);
    if (pid < 0) {
        abort();
    }
    else if (pid == 0) { //child process executes a new seq-solver
        printf("Creating process with file %s\n", filename);
        execl("../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver", filename,0);
    } else {
        Process * p = createProcess(pid); //creates new process and adds it to the list
        insert(liveProcesses, createNode(p));
        currentProcesses++;
        count++; 
    }
}
