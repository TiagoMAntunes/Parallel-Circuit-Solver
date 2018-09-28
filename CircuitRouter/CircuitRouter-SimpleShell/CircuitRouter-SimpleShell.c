#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "CircuitRouter-SimpleShell.h"
#include "Process.h"

int MAXCHILDREN, currentProcesses = 0, count = 10;
Process * processList;

int main(int argc, char * argv[]) {
    char * input  = calloc(10000, sizeof(char)); //TODO: Make adaptive function to fit any size of input 
    char * command, *filename;
    processList = malloc(sizeof(Process) * 10); //starts at 10

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
}

void manageProcesses(char * filename) {
    int pid, state;
    if (!MAXCHILDREN || MAXCHILDREN && currentProcesses < MAXCHILDREN) {
        newProcess(filename);
    } else if (currentProcesses >= MAXCHILDREN) {
        pid = wait(&state);
        currentProcesses--;
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
    else if (pid == 0) {
        printf("Creating process with file %s\n", filename);
        Process * p = createProcess(pid);
        execl("../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver", filename,0);
    } else
        currentProcesses++;
}