#Makefile for the server side

# Variables
CFLAGS = -ansi -std=c99 -Wall -c
LFLAGS = -ansi -std=c99 -Wall

# Default target
all: Server

############ Generate object files ##############

# Target for Server.o
Server.o: Server.c Server.h ../FileIO.h
	gcc -c $(CFLAGS) Server.c -o ../bin/Server.o

# Target for FileIO.o
FileIO.o: ../FileIO.h ../FileIO.c
	gcc -c $(CFLAGS) ../FileIO.c -o ../bin/FileIO.o


############ Generate the executable ############

# Target for Server
Server: Server.o FileIO.o
	cd ../bin; gcc $(LFLAGS) Server.o FileIO.o -o Server

###########   Others ################
# Target for clean-up
clean:
	cd ../bin; rm -rf *.o; rm -rf Server

# Target to run the program
test:
	make
	cd ../bin; ./Server 4450
