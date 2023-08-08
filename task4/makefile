all: myshell

myshell: myshell.o LineParser.o
	gcc -m32 -Wall -g -o myshell myshell.o LineParser.o
	
LineParser.o: LineParser.c
	gcc -m32 -Wall -g -c -o LineParser.o LineParser.c
myshell.o: task4.c
	gcc -m32 -Wall -g -c -o myshell.o task4.c
	
	
   
.PHONY:

clean:
	rm -f *.o task1b


