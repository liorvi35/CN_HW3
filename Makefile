# macros - for more dynamic Makefile
CC = gcc # compiler
FLAGS = -Wall -g # compilation flags

# for not creating 'all' and 'clean' files
.PHONY: all clean 

# final targets
all: Receiver Sender

# (linkage) making executables from objects:
Receiver: Receiver.o
	$(CC) $(FLAGS) -o Receiver Receiver.o

Sender: Sender.o
	$(CC) $(FLAGS) -o Sender Sender.o  

# (compilation) making object files from '.c' and '.h' files: 
Receiver.o: Receiver.c myLib.h
	$(CC) $(FLAGS) -c Receiver.c myLib.h


Sender.o: Sender.c myLib.h
	$(CC) $(FLAGS) -c Sender.c myLib.h

# delete all files that created after 'make' command (not deleting '.c' files)
clean:
	rm -f *.o Receiver Sender
