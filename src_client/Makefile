#Makefile for the client side

# compiler flags for GTK usage
GTKINC = `pkg-config --cflags gtk+-2.0`
GTKLIBS = `pkg-config --libs gtk+-2.0`

# Variables
CFLAGS = -ansi -std=c99 -Wall $(GTKINC) -c
LFLAGS = -ansi -std=c99 -Wall $(GTKLIBS)


# Variables
#CFLAGS = -ansi -std=c99 -Wall -c
#LFLAGS = -ansi -std=c99 -Wall

# Default target
all: Client
############ Generate object files ##############

# Target for Client.o
Client.o: Client.c Client.h ClientGui.h Constants.h
	gcc -c $(CFLAGS) Client.c -o ../bin/Client.o

# Target for ClientModuleTest.o
ClientModuleTest.o: Client.c Client.h Constants.h
	gcc -c -ansi -std=c99 -Wall -c Client.c -DCLIENTMODULETEST -o ../bin/ClientModuleTest.o

# Target for ClientGui.o
ClientGui.o: ClientGui.c Client.h ../FileIO.h ClientGui.h
	gcc -c $(CFLAGS) ClientGui.c -o ../bin/ClientGui.o

# Target for FileIO.o
FileIO.o: ../FileIO.h ../FileIO.c
	gcc -c $(CFLAGS) ../FileIO.c -o ../bin/FileIO.o

############ Generate the executable ############

# Target for Client
Client: Client.o ClientGui.o FileIO.o
	cd ../bin; gcc $(LFLAGS) Client.o ClientGui.o FileIO.o -o Client

# Target for ClientModuleTest
ClientModuleTest: ClientModuleTest.o FileIO.o
	cd ../bin; gcc -ansi -std=c99 -Wall -DCLIENTMODULETEST ClientModuleTest.o FileIO.o -o ClientModuleTest
###########   Others ################
# Target for clean-up
clean:
	cd ../bin; rm -rf *.o; rm -rf Client; rm -rf ClientModuleTest;

# Target to run the program
test:
	make
	./../bin/Client bondi 4450
