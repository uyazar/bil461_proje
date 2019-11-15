#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = server.o request.o bil461.o client.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: server client output.cgi
	-mkdir -p public
	-cp output.cgi favicon.ico home.html public

server: server.o request.o bil461.o
	$(CC) $(CFLAGS) -o server server.o request.o bil461.o $(LIBS)

client: client.o bil461.o
	$(CC) $(CFLAGS) -o client client.o bil461.o $(LIBS)

output.cgi: output.c
	$(CC) $(CFLAGS) -o output.cgi output.c $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $< $(LIBS)

clean:
	-rm -f $(OBJS) server client output.cgi
	-rm -rf public
