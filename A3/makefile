all: checkprime pfact

pfact: pfact.o
		gcc -Wall -g -o pfact pfact.o -lm

checkprime: checkprime.o
		gcc -Wall -g -o checkprime checkprime.o

checkprime.o :
		gcc -Wall -g -c checkprime.c 

pfact.o:
		gcc -Wall -g -c pfact.c
