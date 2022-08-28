CC=gcc
CFLAGS=-c -g -std=gnu11 -O
LIBS= -lSDL_bgi -l SDL2 -pthread -lm

all: cp

cp: CarPark.o Queue.o PQ.o CPSimulator.o Car.h Queue.h PQ.h CarPark.h
	$(CC) Queue.o CarPark.o PQ.o CPSimulator.o -o cp $(LIBS)

Queue.o: Queue.c Queue.h
	$(CC) $(CFLAGS) Queue.c

PQ.o: PQ.c PQ.h
	$(CC) $(CFLAGS) PQ.c

CPSimulator.o: CPSimulator.c
	$(CC) $(CFLAGS) CPSimulator.c

clean:
	rm cp Queue.o PQ.o CPSimulator.o

