all: mcput mcget mcdel mclist 

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csapp.o: csapp.h csapp.c
	gcc -c csapp.h csapp.c
	clear

mycloudclient: mycloudclient.cpp csapp.o
	g++ -Wall -std=c++11 -g mycloudclient.cpp csapp.o -lpthread -o mycloudclient

mcput: mcput.cpp csapp.o
	g++ -Wall -std=c++11 -g mcput.cpp csapp.o -lpthread -o mcput

mcget: mcget.cpp csapp.o
	g++ -Wall -std=c++11 -g mcget.cpp csapp.o -lpthread -o mcget

mcdel: mcdel.cpp csapp.o
	g++ -Wall -std=c++11 -g mcdel.cpp csapp.o -lpthread -o mcdel

mclist: mclist.cpp csapp.o
	g++ -Wall -std=c++11 -g mclist.cpp csapp.o -lpthread -o mclist


clean:
	rm -rf *.gch *.o *dSYM mycloudclient mcput mcget mcdel mclist
