#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define BUFSIZE 1024

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
        fgets(buf, BUFSIZE, stdin);
        write(out, buf, BUFSIZE);
    }

    close(out);
}