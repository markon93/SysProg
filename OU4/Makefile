CC = gcc
CFLAGS = -g -std=c99 -Wall

default: mfind

mfind: mfind.o
	$(CC) $(CFLAGS) -o mfind mfind.o

mpasswdsort.o: mfind.c
	$(CC) $(CFLAGS) -c mfind.c

all: mfind.o

clean:
	$(RM) mfind *.o *~