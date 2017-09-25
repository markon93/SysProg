CC = gcc
CFLAGS = -g -std=c99 -Wall

default: mpasswdsort

mpasswdsort: list.o mpasswdsort.o
	$(CC) $(CFLAGS) -o mpasswdsort list.o mpasswdsort.o

list.o: list.h list.c
	$(CC) $(CFLAGS) -c list.c

mpasswdsort.o: list.h mpasswdsort.c
	$(CC) $(CFLAGS) -c mpasswdsort.c

all: mpasswdsort.o list.o mpasswdsort

clean:
	$(RM) mpasswdsort *.o *~
