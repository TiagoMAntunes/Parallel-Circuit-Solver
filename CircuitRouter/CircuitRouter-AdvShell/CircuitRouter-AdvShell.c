#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include "process.h"
#include "list.h"

#define BUFSIZE 4096
#define TRUE 1

int PWD_SIZE = 64;
Node liveProcesses, deadProcesses;

int split(char* parsedInfo[2], char* buffer) {
    int validCommand = 1;

    char *pid = strtok(buffer, "\\|");
    char *command = strtok(NULL, "\\|");
    char * helper;
    if ((helper = strtok(command, " ")) != NULL && strcmp(helper, "run") != 0)
        validCommand = 0;

    parsedInfo[0] = pid;
    parsedInfo[1] = strtok(NULL, " ");

    return validCommand;
}


int connectToClient(char *info[2]) {
    //creating pipe to talk to client
    char* clientPID = info[0];
    printf("Input file from client (%s): %s\n", clientPID, info[1]);

    char *CLIENT_PATH = (char *) malloc(sizeof(char) * (strlen("./CircuitRouter-Client") + strlen(clientPID) + 6));
    strcpy(CLIENT_PATH, "./CircuitRouter-Client");
    strcat(CLIENT_PATH, clientPID);
    strcat(CLIENT_PATH, ".pipe");

    int out;
    if ((out = open(CLIENT_PATH, O_WRONLY)) < 0) {
        fprintf(stderr, "Error opening client pipe.\n");
        exit(EXIT_FAILURE);   
    }

    free(CLIENT_PATH);
    return out;
}

void handleChild(int sig, siginfo_t *si, void *context) { 
    switch(sig) {
        case SIGCHLD:
   //         Process *p = createProcess((int) si->si_pid, si->si_status,  si->si_stime, si->si_utime);
   //         Node n = createNode(p)
            insert(deadProcesses, createNode(createProcess((int) si->si_pid, si->si_status,  si->si_stime, si->si_utime)));
            break;
        default:
            return;
    }
}

void clear_buffer(char buf[], int size) {
    for (int i = 0; i < size; i++)
        buf[i] = 0;
}

int main(int argc, char * argv[]) {
    int in, n, pid;
    char buf[BUFSIZE];
    liveProcesses = createNode(NULL); //list of processes running
    deadProcesses = createNode(NULL); //list of processes that have finished
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handleChild;

    //Create the pipe name
    char * PIPE_PATH = (char *) malloc(sizeof(char) * (strlen(argv[0]) + 6));
    strcpy(PIPE_PATH, argv[0]);
    strcat(PIPE_PATH, ".pipe");


    if (unlink(PIPE_PATH) != 0 && errno != ENOENT) {
        fprintf(stderr, "Error unlinking pipe.\n");
        exit(-1);
    }

    //open the PIPE_PATH with read mode
    if (mkfifo(PIPE_PATH, 0777) < 0) {
        fprintf(stderr, "Error creating named pipe.\n");
        exit(-1);
    }
   
    if ((in = open(PIPE_PATH, O_RDONLY)) < 0) {
        fprintf(stderr, "Error accessing pipe.\n");
        exit(-1);
    }

    char* parsedInfo[2];
    while (TRUE) {
        int ok_read = 0;
        char tmp_buf[BUFSIZE];
        clear_buffer(buf, BUFSIZE);
        while (!ok_read) {
            clear_buffer(tmp_buf, BUFSIZE);
            n = read(in, tmp_buf, BUFSIZE);
            if ((n < 0 && errno != EINTR) || n == 0)
                break;
            else if (n > 0)
                ok_read = 1;
            strcat(buf, tmp_buf);
        }
        if (!ok_read)
            break;
        
        int validCommand = split(parsedInfo, buf);
        int out = connectToClient(parsedInfo);


        if (!validCommand) {
            char *invalidCommand = "Command not suported.";
            write(out, invalidCommand, strlen(invalidCommand));
            continue;
        }
        
        if ((pid = fork()) == 0) {

            char *args[3];
            char outStr[12];
            sprintf(outStr, "%d", out);
            args[0] = outStr;
            args[1] = parsedInfo[1];
            args[2] = NULL;

            execv("../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver", args);          
            exit(EXIT_FAILURE);
        }
        else if (pid > 0) {
            sigaction(SIGCHLD, &sa, NULL);        
            close(out);
        //    sleep(10);                    o sitio final disto nao é aqui, é so para testar o tempo
        //    printAll(deadProcesses);
        }
        else {
            perror("Failed to create new process.");
            exit(EXIT_FAILURE);
        }
    }

    close(in);
    if (unlink(PIPE_PATH) != 0) {
        fprintf(stderr, "Error unlinking pipe.\n");
        exit(EXIT_FAILURE);
    }   
    return 0;
}
