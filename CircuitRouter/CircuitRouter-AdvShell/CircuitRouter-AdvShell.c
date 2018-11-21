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


int main(int argc, char * argv[]) {
    int in, n;
    char buf[BUFSIZE];

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
        n = read(in, buf, BUFSIZE);
        if (n <= 0) break;

        int validCommand = split(parsedInfo, buf);
        printf("Valid Command? %s\n", (validCommand ==1 ? "yes" : "no"));
        printf("PID: %s\nComman: %s\n", parsedInfo[0], parsedInfo[1]);
    }

    close(in);
    if (unlink(PIPE_PATH) != 0) {
        fprintf(stderr, "Error unlinking pipe.\n");
        exit(-1);
    }   
    return 0;
}
