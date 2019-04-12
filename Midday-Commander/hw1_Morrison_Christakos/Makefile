all: mc0 mc1

mc0.o: mc0.c
	gcc -c mc0.c -o mc0.o

mc0: mc0.o
	gcc mc0.o -o mc0

mc1.o: mc1.c
	gcc -c mc1.c -o mc1.o

mc1: mc1.o
	gcc mc1.o -o mc1

clean:
	rm -f *.o mc0 mc1