#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 4096
#define TRUE 1
int PWD_SIZE = 64;

int split(char* parsedInfo[2], char* buffer) {
    int validCommand = 1;

    char *pid = strtok(buffer, "\\|");
    char *command = strtok(NULL, "\\|");

    if (strcmp(strtok(command, " "), "run") != 0)
        validCommand = 0;

    parsedInfo[0] = pid;
    parsedInfo[1] = command;

    return validCommand;
}


int main() {
    int in, n;
    char buf[BUFSIZE];

    char * PIPE_PATH = (char *) malloc(sizeof(char) * PWD_SIZE);

    //get the current directory
    while (getcwd(PIPE_PATH, PWD_SIZE) == NULL) {
        switch (errno) {
            case ERANGE:
                PWD_SIZE = PWD_SIZE * 2;
                PIPE_PATH = realloc(PIPE_PATH, PWD_SIZE * sizeof(char));
                break;
            default:
                exit(-1);
        }
    }

    //get the PIPE_PATH name
    int PIPE_PATH_SIZE = strlen(PIPE_PATH);
    if (PWD_SIZE <  PIPE_PATH_SIZE + 6)
        if (realloc(PIPE_PATH, (PIPE_PATH_SIZE+6) * sizeof(char)) == NULL)
            exit(-1);  
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
        n = read(in, buf, BUFSIZE);

        int validCommand = split(parsedInfo, buf);
        printf("Valid Command? %s\n", (validCommand ==1 ? "yes" : "no"));
        printf("PID: %s\nComman: %s\n", parsedInfo[0], parsedInfo[1]);

        if (n <= 0) break;
    }

    close(in);
    if (unlink(PIPE_PATH) != 0) {
        fprintf(stderr, "Error unlinking pipe.\n");
        exit(-1);
    }   
    return 0;
}