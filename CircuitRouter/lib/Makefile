CLAGS = -Wall -g -std=gnu99
SOURCES = $(wildcard *.c)
OBJS = $(SOURCES:%.c=%.o)
CC = gcc

all: $(OBJS)

commandlinereader.o: commandlinereader.c commandlinereader.h
list.o: list.c list.h types.h
pair.o: pair.c pair.h 
queue.o: queue.c queue.h types.h
vector.o: vector.c vector.h utility.h



$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	echo Cleaning ${PWD}
	rm -rf *.o *.h.gch
	
