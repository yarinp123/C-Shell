all: mypipeline

mypipeline: mypipeline.o
	gcc -m32 -Wall -g -o mypipeline mypipeline.o
	
mypipeline.o: task2.c
	gcc -m32 -Wall -g -c -o mypipeline.o task2.c

clean:
	rm -f *.o mypipeline
