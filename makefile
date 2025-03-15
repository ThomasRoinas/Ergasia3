# Makefile for server-client

# instructions for make: (command line / terminal)
# 1. type make-server
# 2. type ./server-server
# 3. type make clean to remove the object files and the executable

all: server-client 

server: server-client.o
	cc -o server-client server-client.o

server.o : server-client.c server-client.h
	cc -c -o server-client.o server-client.c

clean :
	rm *.o server-client
