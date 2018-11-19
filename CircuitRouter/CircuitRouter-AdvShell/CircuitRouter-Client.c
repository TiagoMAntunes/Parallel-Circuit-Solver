#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    int out;
    char buf[BUFSIZE];

    if (argc != 2) {
        printf("Usage: ./CircuitRouter-Client <server pipe path>\n");
        exit(0);
    }

    if ((out = open(argv[1], O_WRONLY)) < 0) 
        exit(-1);

    while (TRUE) {
        getMessage(buf, BUFSIZE);
        write(out, buf, BUFSIZE);
    }

    close(out);
}