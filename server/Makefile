all: mycloudserver

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csapp.o: csapp.h csapp.c
	gcc -c csapp.h csapp.c
	clear

mycloudserver: mycloudserver.cpp csapp.o
	g++ -Wall -std=c++11 -g mycloudserver.cpp csapp.o -lpthread -o mycloudserver

clean:
	rm -rf *.gch *.o *dSYM mycloudserver
