CFLAGS=-g -Wall -pedantic

CircuitRouter-SeqSolver: coordinate.o grid.o CircuitRouter-SeqSolver.o maze.o router.o lib/list.o lib/pair.o lib/queue.o lib/vector.o
	gcc $(CFLAGS) coordinate.o grid.o CircuitRouter-SeqSolver.o maze.o router.o lib/list.o lib/pair.o lib/queue.o lib/vector.o -lm -o CircuitRouter-SeqSolver

CircuitRouter-SeqSolver.o: CircuitRouter-SeqSolver.c maze.h router.h lib/timer.h lib/types.h lib/list.h
	gcc $(CFLAGS) -c CircuitRouter-SeqSolver.c -o CircuitRouter-SeqSolver.o

maze.o: maze.c maze.h lib/queue.h lib/pair.h lib/types.h lib/vector.h lib/list.h grid.h coordinate.h
	gcc $(CFLAGS) -c maze.c -o maze.o

router.o: router.c router.h coordinate.h grid.h lib/queue.h lib/vector.h
	gcc $(CFLAGS) -c router.c -o router.o

list.o: lib/list.c lib/list.h lib/types.h
	gcc $(CFLAGS) -c lib/list.c -o lib/list.o

pair.o: lib/pair.c lib/memory.h lib/pair.h
	gcc $(CFLAGS) -c lib/pair.c -o lib/pair.o

queue.o: lib/queue.c lib/types.h
	gcc $(CFLAGS) -c lib/queue.c -o lib/queue.o

vector.o: lib/vector.c lib/types.h lib/utility.h lib/vector.h
	gcc $(CFLAGS) -c lib/vector.c -o lib/vector.o

coordinate.o: coordinate.c coordinate.h lib/pair.h lib/types.h
	gcc $(CFLAGS) -c coordinate.c -o coordinate.o
	
grid.o: grid.c grid.h lib/types.h lib/vector.h
	gcc $(CFLAGS) -c grid.c -o grid.o

clean: 
	rm -f *.o lib/*.o CircuitRouter-SeqSolver