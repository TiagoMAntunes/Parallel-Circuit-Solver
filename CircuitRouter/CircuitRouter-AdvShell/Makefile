CFLAGS = -g -Wall -std=gnu99
SOURCES = CircuitRouter-AdvShell.c
OBJS = $(SOURCES:%.c=%.o)
CC = gcc
LDFLAGS = -lm
ADVANCED = CircuitRouter-AdvShell
CLIENT = CircuitRouter-Client

all: CircuitRouter-AdvShell CircuitRouter-Client

CircuitRouter-AdvShell: CircuitRouter-AdvShell.o
CircuitRouter-Client: CircuitRouter-Client.o

CircuitRouter-AdvShell.o: CircuitRouter-AdvShell.c
CircuitRouter-Client.o: CircuitRouter-Client.c
$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o $(CLIENT) $(ADVANCED) -- --* *.pipe
