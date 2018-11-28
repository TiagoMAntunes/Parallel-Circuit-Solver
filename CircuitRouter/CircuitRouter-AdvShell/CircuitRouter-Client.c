#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define TRUE 1
#define BUFSIZE 4096


char * SELF_PATH;

//On CTRL+C, unlink pipe and free memory.
void handleInterrupt(int sig) {
    unlink(SELF_PATH);
    free(SELF_PATH);
    exit(0);
}

void displayHeader() {
    printf(">>> ");
}

void displayResult(char * array) {
    if (!strcmp(array, "Circuit Solved"))
        printf("%s%s\n", KGRN, array);
    else
        printf("%s%s\n", KRED, array);
    printf("%s", KNRM);
}

int getMessage(char buf[], int size) {
    int i, j;
    char c;

    //write pid to array
    i = snprintf(buf, size-1, "%d", getpid());
    if (i < 0) {
        fprintf(stderr, "Error with snprintf.\n");
        exit(EXIT_FAILURE);
    }

    buf[i++] = '|';
    j = i;
    
    while (i == j)
        while ((c = getchar()) != '\n' && c != EOF && i < size-1) {
            buf[i++] = c;
        }
    
    if (c == EOF)
        exit(EXIT_FAILURE);
    buf[i] = '\0';
    return i;
}



int main(int argc, char* argv[]) {
    int out, in;
    char outbuf[BUFSIZE], inbuf[BUFSIZE], pid[10];

    if (argc != 2) {
        printf("Usage: ./CircuitRouter-Client <server pipe path>\n");
        exit(0);
    }

    if (snprintf(pid, 10, "%d", getpid()) < 0) {
        fprintf(stderr, "Error with snprintf.\n");
        exit(EXIT_FAILURE);
    }

    SELF_PATH = (char *) malloc(sizeof(char) * (strlen(argv[0]) + strlen(pid) + 6));
    if (SELF_PATH == NULL) {
        fprintf(stderr, "Error with memory allocation.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(SELF_PATH, argv[0]);
    strcat(SELF_PATH, pid);
    strcat(SELF_PATH, ".pipe");
    
    if (unlink(SELF_PATH) != 0 && errno != ENOENT) {
        fprintf(stderr, "Error unlinking pipe.\n");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(SELF_PATH, 0777) < 0) {
        fprintf(stderr, "Error creating named pipe.\n");
        exit(EXIT_FAILURE);
    }


    if ((out = open(argv[1], O_WRONLY)) < 0) {
        fprintf(stderr, "Error opening.\n");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, handleInterrupt) == SIG_ERR) {
        fprintf(stderr, "Error installing signal.\n");
        exit(EXIT_FAILURE);
    }
    int readSize;
    while (TRUE) {
        displayHeader();
        getMessage(outbuf, BUFSIZE);
        if (write(out, outbuf, BUFSIZE) < 0) {
            fprintf(stderr, "Error writing to pipe.\n");
            exit(EXIT_FAILURE);
        }
        while((in = open(SELF_PATH, O_RDONLY)) < 0)
            ;
        readSize = read(in, inbuf, BUFSIZE-1);
        inbuf[readSize] = '\0';
        displayResult(inbuf);
    }
}