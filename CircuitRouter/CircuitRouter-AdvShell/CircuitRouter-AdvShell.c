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
#include "../lib/commandlinereader.h"

#define BUFSIZE 4096
#define TRUE 1
#define COMMAND_EXIT "exit"
#define COMMAND_RUN "run"
#define MAXARGS 2

int PWD_SIZE = 64;
Node liveProcesses;
int countChildren;

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
            if (si->si_code == CLD_EXITED) {
                TIMER_T stopTime;
                TIMER_READ(stopTime);
                Process *p = getByPID(si->si_pid, liveProcesses);
                p->finish = stopTime;
                p->status = si->si_status;
                //bloquear interrupcoes
                countChildren--;
            }
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
    int in, n, pid, fromClient = 1;
    char buf[BUFSIZE];
    liveProcesses = createNode(NULL); //list of processes running
    countChildren = 0;
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

    fd_set fdset, tempset;
    FD_ZERO(&fdset);                   // clears set of fd's to read from
    FD_SET(STDIN_FILENO, &fdset);      // set the bit for stdin
    FD_SET(in, &fdset);                // set the bit for in
    int maxfd = STDIN_FILENO > in ? STDIN_FILENO + 1 : in + 1; 
    while (TRUE) {
        tempset = fdset;               // reset tempset
        int selectVal = select(maxfd, &tempset, NULL, NULL, NULL);
        if (selectVal == EINTR) {
            continue;
        } 

        else if (selectVal == EBADF || selectVal == EINVAL || selectVal == ENOMEM) {
            printf("shit happened\n");
            exit(1);
        }

        else if (selectVal > 0) {

            int out;
            char tmp_buf[BUFSIZE];
            clear_buffer(buf, BUFSIZE);

            // going to read from client's pipe

            if (FD_ISSET(in, &tempset)) {   
                int ok_read = 0;
                fromClient = 1;  
                while (!ok_read) {
                    clear_buffer(tmp_buf, BUFSIZE);
                    n = read(in, tmp_buf, BUFSIZE-strlen(buf));
                    if ((n < 0 && errno != EINTR) || n == 0)
                        break;
                    else if (n > 0)
                        ok_read = 1;
                    strcat(buf, tmp_buf);
                }


                int validCommand = split(parsedInfo, buf);
                out = connectToClient(parsedInfo);

                if (!validCommand) {
                    char *invalidCommand = "Command not suported.";
                    write(out, invalidCommand, strlen(invalidCommand));
                    continue;
                }
            }

            // going to read from server's stdin
            else if (FD_ISSET(STDIN_FILENO, &tempset)) { 
                int numArgs = readLineArguments(parsedInfo, MAXARGS+1, buf, BUFSIZE);

                if (numArgs == 0) {
                    continue;
                }
                else if (numArgs > 0 && strcmp(parsedInfo[0], COMMAND_EXIT) != 0 && strcmp(parsedInfo[0], COMMAND_RUN) != 0) {
                    printf("Unknown command. Try again.\n");
                    continue;
                }
                else if (numArgs < 0 || (numArgs > 0 && (strcmp(parsedInfo[0], COMMAND_EXIT) == 0))) {
                    printf("CircuitRouter-SimpleShell will exit.\n--\n");
                    break;
                }
        
                else 
                    fromClient = 0;   
            } 

            TIMER_T startTime;
            TIMER_READ(startTime);
            if ((pid = fork()) == 0) {
                char *args[3];
                char outStr[12];
                if (!fromClient) {
                    out = STDOUT_FILENO;
                }
                sprintf(outStr, "%d", out);
                args[0] = outStr;
                args[1] = parsedInfo[1];
                args[2] = NULL;
                close(2);
                dup(out);
                execv("../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver", args);          
                exit(EXIT_FAILURE);
            }
            else if (pid > 0) {
                sigaction(SIGCHLD, &sa, NULL);        
                close(out);
                Process *p = createProcess(pid, startTime);
                Node n = createNode(p);
                insert(liveProcesses, n);
                countChildren++;
            }
            else {
                perror("Failed to create new process.");
                exit(EXIT_FAILURE);
            }
        }
    }

    close(in);
    if (unlink(PIPE_PATH) != 0) {
        fprintf(stderr, "Error unlinking pipe.\n");
        exit(EXIT_FAILURE);
    }  

    while (countChildren != 0) ;    //wait for all children to finish
    printAll(liveProcesses);
    return 0;
}
