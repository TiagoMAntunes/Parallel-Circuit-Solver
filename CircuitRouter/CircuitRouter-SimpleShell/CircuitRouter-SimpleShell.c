#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "CircuitRouter-SimpleShell.h"

int MAXCHILDREN, currentProcesses = 0;

int main(int argc, char * argv) {
    char * input  = calloc(10000, sizeof(char)); //TODO: Make adaptive function to fit any size of input 
    char * command, *filename;

    if (argc >= 1) {
        sscanf(argv, "%d", &MAXCHILDREN);
    } else 
        MAXCHILDREN = 0;

    while(strcmp("exit\n", input)) {
        fgets(input, 10000, stdin);
        command = strtok(input, " ");
        if (!strcmp(command, "run")) {
            filename = strtok(NULL, " \n");
            newProcess(filename);
        } else {
            printf("Invalid command\n");
        }
    }
}

void newProcess(char * filename) {
    int pid, estado;
    if (!MAXCHILDREN || MAXCHILDREN && currentProcesses <= MAXCHILDREN) {
        pid = fork();
        printf("%d\n", pid);
        if (pid < 0) {
            abort();
        }
        else if (pid == 0) {
            printf("Creating process with file %s\n", filename);
            execl("../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver", filename,0);
        } else {
            currentProcesses++;
        }
    }
}