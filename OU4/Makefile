CC = gcc -pthread
CFLAGS = -g -std=c99 -Wall

default: mfind

mfind: mfind.o checkComLine.o queue.o list.o
	$(CC) $(CFLAGS) -o mfind mfind.o checkComLine.o queue.o list.o

mfind.o: mfind.c checkComLine.h queue.h
	$(CC) $(CFLAGS) -c mfind.c

checkComline.o: checkComLine.h checkComLine.c
	$(CC) $(CFLAGS) -c checkComLine.c

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

all: mfind.o checkComLine.o queue.o list.o

clean:
	$(RM) mfind *.o *~
