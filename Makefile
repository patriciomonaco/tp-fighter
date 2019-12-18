all : Server Client

CPPFLAGS = -g -I.

Server : Server.c functions.h definitions.h functions.o
	cc -g -I. functions.o Server.c -o Server

Client : Client.c functions.h definitions.h functions.o
	cc -g -I. functions.o definitions.h Client.c -o Client
functions.o : functions.c functions.h
	cc -c functions.c

clean :
	rm *.o Client Server
