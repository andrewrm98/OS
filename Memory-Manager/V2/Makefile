# Project 4 Makefile
# Peter Christakos
# Andrew Morrison
CC = gcc
CFLAGS = -Wall
OBJS = manager.o

all: manager 

manager: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o manager

manager.o: manager.c
	$(CC) $(CFLAGS) -c manager.c

clean:
	rm -f *~ *.o manager