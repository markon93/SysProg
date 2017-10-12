CC = gcc
CFLAGS = -g -std=c99 -Wall

default: mfind

mfind: mfind.o checkComLine.o
	$(CC) $(CFLAGS) -o mfind mfind.o checkComLine.o

mfind.o: mfind.c checkComLine.h
	$(CC) $(CFLAGS) -c mfind.c 

checkComline.o: checkComLine.h checkComLine.c
	$(CC) $(CFLAGS) -c checkComLine.c 

all: mfind.o checkComLine.o

clean:
	$(RM) mfind *.o *~