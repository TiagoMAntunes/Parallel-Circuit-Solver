#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define TRUE 1
#define BUFSIZE 4096

int getMessage(char buf[], int size) {
    int i;
    char c;

    //write pid to array
    i = snprintf(buf, size-1, "%d", getpid());
    buf[i++] = '|';
    while ((c = getchar()) != '\n' && c != EOF && i < size-1) {
        buf[i++] = c;
    }
    if (c ==EOF)
        exit(-1);
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

    snprintf(pid, 10, "%d", getpid());
    char * SELF_PATH = (char *) malloc(sizeof(char) * (strlen(argv[0]) + strlen(pid) + 6));
    strcpy(SELF_PATH, argv[0]);
    strcat(SELF_PATH, pid);
    strcat(SELF_PATH, ".pipe");
    
    if (mkfifo(SELF_PATH, 0777) < 0) {
        fprintf(stderr, "Error creating named pipe.\n");
        exit(-1);
    }


    if ((out = open(argv[1], O_WRONLY)) < 0) {
        fprintf(stderr, "Error opening.\n");
        exit(-1);
    }


    while (TRUE) {
        getMessage(outbuf, BUFSIZE);
        write(out, outbuf, BUFSIZE);
        while((in = open(SELF_PATH, O_RDONLY)) < 0)
            ;
        read(in, inbuf, BUFSIZE);
        printf("%s\n", inbuf);
    }

    close(out);
}