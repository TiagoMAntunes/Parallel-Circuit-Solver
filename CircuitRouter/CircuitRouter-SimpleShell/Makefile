CFLAGS = -Wall -g -std=gnu99
TARGET = CircuitRouter-SimpleShell
SOURCES=$(wildcard *.c)
OBJS = $(SOURCES:%.c=%.o)
CC = gcc
LDFLAGS=-lm

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS) 

list.o: list.c list.h Process.h
Process.o: Process.c Process.h ../lib/timer.h
CircuitRouter-SimpleShell.o: CircuitRouter-SimpleShell.c ../lib/timer.h list.h Process.h 

$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo Cleaning...
	rm -f $(OBJS) $(TARGET)
