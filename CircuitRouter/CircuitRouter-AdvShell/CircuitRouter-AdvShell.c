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
int alive = 1;

void exitRoutine() {
	if (unlink(PIPE_PATH) != 0) {
       	fprintf(stderr, "Error unlinking pipe.\n");
        exit(EXIT_FAILURE);
    }   
    while (child_count) ;
    printAll(liveProcesses);
    freeAll(liveProcesses);
    kill(getppid(), SIGINT);
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

    //Get the pipe name
    char *CLIENT_PATH = (char *) malloc(sizeof(char) * (strlen("./CircuitRouter-Client") + strlen(clientPID) + 6));
    if (CLIENT_PATH == NULL) {
        fprintf(stderr, "Error with memory allocation.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(CLIENT_PATH, "./CircuitRouter-Client");
    strcat(CLIENT_PATH, clientPID);
    strcat(CLIENT_PATH, ".pipe");
    
    //creates the pipe
    int out;    
    if ((out = open(CLIENT_PATH, O_WRONLY)) < 0) {
        kill(atoi(clientPID), SIGINT);
        fprintf(stderr, "Error opening client pipe.\n");
        exit(EXIT_FAILURE);   
    }

    free(CLIENT_PATH);
    return out;
}

//Signal interruption handler (SIGINT)
void handleSigint(int sig) {
    switch(sig) {
        case SIGINT:
            alive = 0;
        default:
            return;
    }
}

//Signal child handler (SIGCHLD)
void handleChild(int sig) { 
    int pid, status;
    switch(sig) {
        case SIGCHLD:
            while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
                TIMER_T stopTime;
                TIMER_READ(stopTime);
                Process *p = getByPID(pid, liveProcesses);
                p->finish = stopTime;
                p->status = status;
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
    int MAXCHILDREN = 0;

    if (argv[1] != NULL) {
        MAXCHILDREN = atoi(argv[1]);
    }

    
    //Create the SIGCHLD handler
    struct sigaction childHandler;
    childHandler.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    childHandler.sa_handler = handleChild;
    if (sigaction(SIGCHLD, &childHandler, NULL) != 0) {
        fprintf(stderr, "Error installing sigaction.\n");
        exit(EXIT_FAILURE);
    }   

    //Create the SIGINT handler
    struct sigaction endHandler;
    endHandler.sa_handler = handleSigint;
    if (sigaction(SIGINT, &endHandler, NULL) !=0) {
        fprintf(stderr, "Error installing sigaction.\n");
        exit(EXIT_FAILURE);
    }

    //Create the pipe name
    PIPE_PATH = (char *) malloc(sizeof(char) * (strlen(argv[0]) + 6));
    if (PIPE_PATH == NULL) {
        fprintf(stderr, "Error with memory allocation.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(PIPE_PATH, argv[0]);
    strcat(PIPE_PATH, ".pipe");


    //Split process to create server on background and input from stdin
    while ((pid = fork()) < 0);
    if (pid > 0) {
        execl(CLIENT_EXEC, CLIENT_EXEC, PIPE_PATH, NULL);
        fprintf(stderr, "Error with execl.\n");
        exit(EXIT_FAILURE);
    }


    int closeFlag;
    while ((closeFlag = close(STDIN_FILENO)) == EINTR) ;
    if (closeFlag != 0) {
        fprintf(stderr, "Error closing file descriptor.\n");
        exit(EXIT_FAILURE); 
    }
    
    //Remove existing PIPE_PATH
    if (unlink(PIPE_PATH) != 0 && errno != ENOENT) {
        fprintf(stderr, "Error unlinking pipe.\n");
        exit(EXIT_FAILURE);
    }

    //open the PIPE_PATH with read mode
    if (mkfifo(PIPE_PATH, 0777) < 0) {
        fprintf(stderr, "Error creating named pipe.\n");
        exit(EXIT_FAILURE);
    }
    
    //Wait for *at least* 1 connection
    if ((in = open(PIPE_PATH, O_RDONLY)) < 0) {
        fprintf(stderr, "Error accessing pipe.\n");
        exit(EXIT_FAILURE);
    }

    char* parsedInfo[2];
    while (alive) {
        int ok_read = 0;
        char tmp_buf[BUFSIZE];
        
        //Read input to empty buffer
        clear_buffer(buf, BUFSIZE);
        while (!ok_read) {
            clear_buffer(tmp_buf, BUFSIZE);
            n = read(in, tmp_buf, BUFSIZE);
            if ((n < 0 && errno != EINTR) || alive != 1 || n == 0) // if there was an error other than a signal (except SIGINT) or no more connections
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
            if (write(out, invalidCommand, strlen(invalidCommand)) < 0) {
                fprintf(stderr, "Error writing to pipe.\n");
                exit(EXIT_FAILURE);
            }

            continue;
        }

        
        // Set mask to block SIGCHLD
        sigset_t new_mask;
        sigset_t old_mask;
        if (sigemptyset(&new_mask) != 0) {
            fprintf(stderr, "Error with sigemptyset.\n");
            exit(EXIT_FAILURE);
        }

        if (sigaddset(&new_mask, SIGCHLD) != 0) {
            fprintf(stderr,"Error with sigaddset.\n");
            exit(EXIT_FAILURE);
        }


        // waits for SIGCHLD, which causes child_count to drecrease;
        if (MAXCHILDREN) {
            while (child_count >= MAXCHILDREN);
        }

        TIMER_T startTime; //measure start time of process
        if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
            fprintf(stderr, "Error with sigprocmask.\n");
            exit(EXIT_FAILURE);
        }
        
        TIMER_READ(startTime); 

        if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
            fprintf(stderr, "Error with sigprocmask.\n");
            exit(EXIT_FAILURE);
        }
        if ((pid = fork()) == 0) {

            char *args[3];
            char outStr[12];
            if (sprintf(outStr, "%d", out) < 0) {
                fprintf(stderr, "Error with sprintf.\n");
                exit(EXIT_FAILURE);
            }
            args[0] = outStr;
            args[1] = parsedInfo[1];
            args[2] = NULL;
            while ((closeFlag = close(2)) == EINTR) ; //bye bye stderr
            if (closeFlag != 0) {
                fprintf(stderr, "Error closing file descriptor.\n");
                exit(EXIT_FAILURE);
            }
            if (dup(out) < 0) { //if something bad happens, the error is written to the client so it won't stay blocked
                fprintf(stderr, "Error duplicating file descriptor.\n");
                exit(EXIT_FAILURE);
            } 
            execv(SOLVER_EXEC, args);
            fprintf(stderr, "Error with execv.\n");      
            exit(EXIT_FAILURE);
        }
        else if (pid > 0) {     
            while((closeFlag = close(out)) == EINTR) ;
            if (closeFlag != 0) {
                fprintf(stderr, "Error closing file descriptor.\n");
                exit(EXIT_FAILURE);
            }

            

            Process *p = createProcess(pid, startTime);
            Node n = createNode(p);
            
            if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
                fprintf(stderr, "Error with sigprocmask.\n");
                exit(EXIT_FAILURE);
            }

            insert(liveProcesses, n);
            child_count++;

            if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
                fprintf(stderr, "Error with sigprocmask.\n");
                exit(EXIT_FAILURE);
            }
        }
        else {
            perror("Failed to create new process.");
            exit(EXIT_FAILURE);
        }
    }
    printf("\n");
    exitRoutine();
}
