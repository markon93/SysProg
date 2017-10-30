CC = gcc
CFLAGS = -g -std=gnu11 -Wall

default: mish

mish: mish.o parser.o sighant.o execute.o
	$(CC) $(CFLAGS) -o mish mish.o parser.o sighant.o execute.o

parser.o: parser.h parser.c
	$(CC) $(CFLAGS) -c parser.c

sighant.o: sighant.h sighant.c
	$(CC) $(CFLAGS) -c sighant.c

mish.o: parser.h mish.c sighant.h mish.h
	$(CC) $(CFLAGS) -c mish.c

execute.o: execute.h execute.c
	$(CC) $(CFLAGS) -c execute.c

all:
	mish.o parser.o sighant.o execute.o mish

clean:
	rm mish *.o *~
