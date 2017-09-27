CC = gcc
CFLAGS = -g -std=c99 -Wall 

default: mish

mish: mish.o parser.o sighant.o
	$(CC) $(CFLAGS) -o mish mish.o parser.o sighant.o

parser.o: parser.h parser.c
	$(CC) $(CFLAGS) -c parser.c

sighant.o: sighant.h sighant.c
	$(CC) $(CFLAGS) -c sighant.c

mish.o: parser.h mish.c sighant.h
	$(CC) $(CFLAGS) -c mish.c

all: 
	mish.o parser.o sighant.o mish 

clean: 
	rm mish *.o *~
