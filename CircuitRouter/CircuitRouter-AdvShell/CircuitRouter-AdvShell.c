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
#include <sys/times.h>
#include "process.h"
#include "list.h"
#include "../lib/timer.h"

#define BUFSIZE 4096
#define TRUE 1
#define SOLVER_EXEC "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver"
#define CLIENT_EXEC "./CircuitRouter-Client"

int PWD_SIZE = 64;
int child_count = 0;
Node liveProcesses;
char * PIPE_PATH;

void exitRoutine() {
	if (unlink(PIPE_PATH) != 0) {
       	fprintf(stderr, "Error unlinking pipe.\n");
        exit(EXIT_FAILURE);
    }   
    while (child_count);
    printf("\n");
    printAll(liveProcesses);
    exit(0);
}


//Splits a given input in the first '|' found
int split(char* parsedInfo[2], char* buffer) {
    int validCommand = 1;

    char *pid = strtok(buffer, "\\|");
    char *command = strtok(NULL, "\\|");
    char * helper;
    if ((helper = strtok(command, " ")) != NULL && strcmp(helper, "run") != 0) {
    	validCommand = 0;
    	if ((getppid() == atoi(pid)) && (strcmp(helper, "exit") == 0)) {
    		exitRoutine();
    	}
    }

    parsedInfo[0] = pid;
    parsedInfo[1] = strtok(NULL, " ");

    return validCommand;
}


//returns the file descriptor that points to the client that sent a message
int connectToClient(char *info[2]) {
    char* clientPID = info[0];
    //printf("Input file from client (%s): %s\n", clientPID, info[1]); debugging symbols

    //Get the pipe name
    char *CLIENT_PATH = (char *) malloc(sizeof(char) * (strlen("./CircuitRouter-Client") + strlen(clientPID) + 6));
    strcpy(CLIENT_PATH, "./CircuitRouter-Client");
    strcat(CLIENT_PATH, clientPID);
    strcat(CLIENT_PATH, ".pipe");
    
    //creates the pipe
    int out;
    if ((out = open(CLIENT_PATH, O_WRONLY)) < 0) {
        fprintf(stderr, "Error opening client pipe.\n");
        exit(EXIT_FAILURE);   
    }

    free(CLIENT_PATH);
    return out;
}

//Signal interruption handler (SIGINT)
void handleSigint(int sig, siginfo_t *si, void *context) {
    switch(sig) {
        case SIGINT:
            exitRoutine();
        default:
            return;
    }
}

//Signal child handler (SIGCHLD)
void handleChild(int sig, siginfo_t *si, void *context) { 
    switch(sig) {
        case SIGCHLD:
            if (si->si_code == CLD_EXITED) {
                TIMER_T stopTime;
                TIMER_READ(stopTime);
                Process *p = getByPID(si->si_pid, liveProcesses);
                p->finish = stopTime;
                p->status = si->si_status;
                child_count--;
            }
            break;
        default:
            return;
    }
}

//Sets the buffer all to zeros
void clear_buffer(char buf[], int size) {
    for (int i = 0; i < size; i++)
        buf[i] = 0;
}

int main(int argc, char * argv[]) {
    int in, n, pid;
    char buf[BUFSIZE];
    liveProcesses = createNode(NULL); //list of processes running
    
    //Create the SIGCHLD handler
    struct sigaction childHandler;
    childHandler.sa_flags = SA_SIGINFO;
    childHandler.sa_sigaction = handleChild;
    sigaction(SIGCHLD, &childHandler, NULL);   

    //Create the SIGINT handler
    struct sigaction endHandler;
    endHandler.sa_flags = SA_SIGINFO;
    endHandler.sa_sigaction = handleSigint;
    sigaction(SIGINT, &endHandler, NULL);

    //Create the pipe name
    PIPE_PATH = (char *) malloc(sizeof(char) * (strlen(argv[0]) + 6));
    strcpy(PIPE_PATH, argv[0]);
    strcat(PIPE_PATH, ".pipe");

    //Split process to create server on background and input from stdin
    while ((pid = fork()) < 0);
    if (pid > 0)
        execl(CLIENT_EXEC, CLIENT_EXEC, PIPE_PATH, NULL);


    close(STDIN_FILENO);
    
    //Remove existing PIPE_PATH
    if (unlink(PIPE_PATH) != 0 && errno != ENOENT) {
        fprintf(stderr, "Error unlinking pipe.\n");
        exit(-1);
    }

    //open the PIPE_PATH with read mode
    if (mkfifo(PIPE_PATH, 0777) < 0) {
        fprintf(stderr, "Error creating named pipe.\n");
        exit(-1);
    }
    
    //Wait for *at least* 1 connection
    if ((in = open(PIPE_PATH, O_RDONLY)) < 0) {
        fprintf(stderr, "Error accessing pipe.\n");
        exit(-1);
    }

    char* parsedInfo[2];
    while (TRUE) {
        int ok_read = 0;
        char tmp_buf[BUFSIZE];
        
        //Read input to empty buffer
        clear_buffer(buf, BUFSIZE);
        while (!ok_read) {
            clear_buffer(tmp_buf, BUFSIZE);
            n = read(in, tmp_buf, BUFSIZE);
            if ((n < 0 && errno != EINTR) || n == 0) // if there was an error other than a signal or no more connections
                break;
            else if (n > 0) //was able to read something
                ok_read = 1;
            strcat(buf, tmp_buf);
        }
        if (!ok_read)
            break;
        
        //get the input validated
        int validCommand = split(parsedInfo, buf);
        int out = connectToClient(parsedInfo);


        if (!validCommand) {
            char *invalidCommand = "Command not suported.";
            write(out, invalidCommand, strlen(invalidCommand));
            continue;
        }

        
        TIMER_T startTime; //measure start time of process
        TIMER_READ(startTime);
        if ((pid = fork()) == 0) {

            char *args[3];
            char outStr[12];
            sprintf(outStr, "%d", out);
            args[0] = outStr;
            args[1] = parsedInfo[1];
            args[2] = NULL;
            close(2); //bye bye stderr
            dup(out); //if something bad happens, the error is written to the client so it won't stay blocked
            execv(SOLVER_EXEC, args);      
            exit(EXIT_FAILURE);
        }
        else if (pid > 0) {     
            close(out);
            Process *p = createProcess(pid, startTime);
            Node n = createNode(p);
            insert(liveProcesses, n);
            child_count++;
        }
        else {
            perror("Failed to create new process.");
            exit(EXIT_FAILURE);
        }
    }
}
