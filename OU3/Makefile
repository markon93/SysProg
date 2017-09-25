CC = gcc
CFLAGS = -g -std=c99 -Wall 

default: mish

mish: mish.o parser.o
	$(CC) $(CFLAGS) -o mish mish.o parser.o

parser.o: parser.h parser.c
	$(CC) $(CFLAGS) -c parser.c

mish.o: parser.h mish.c
	$(CC) $(CFLAGS) -c mish.c

all: 
	mish.o parser.o mish

clean: 
	rm mish *.o *~
