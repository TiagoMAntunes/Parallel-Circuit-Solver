CFLAGS = -g -Wall -std=gnu99
SOURCES = CircuitRouter-AdvShell.c
OBJS = $(SOURCES:%.c=%.o)
CC = gcc
LDFLAGS = -lm
ADVANCED = CircuitRouter-AdvShell
CLIENT = CircuitRouter-Client

all: CircuitRouter-AdvShell CircuitRouter-Client

CircuitRouter-AdvShell: CircuitRouter-AdvShell.o process.o list.o ../lib/commandlinereader.o
CircuitRouter-Client: CircuitRouter-Client.o

CircuitRouter-AdvShell.o: CircuitRouter-AdvShell.c
CircuitRouter-Client.o: CircuitRouter-Client.c
process.o: process.c process.h
list.o: list.c list.h
../lib/commandlinereader.o: ../lib/commandlinereader.c ../lib/commandlinereader.h	
$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o $(CLIENT) $(ADVANCED) -- --* *.pipe
