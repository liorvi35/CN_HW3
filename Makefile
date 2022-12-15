CC = gcc
FLAGS = -Wall -g

.PHONY: all clean

all: Receiver Sender

Receiver: Receiver.o
	$(CC) $(FLAGS) -o Receiver Receiver.o

Sender: Sender.o
	$(CC) $(FLAGS) -o Sender Sender.o  

Receiver.o: Receiver.c myLib.h
	$(CC) $(FLAGS) -c Receiver.c myLib.h

Sender.o: Sender.c myLib.h
	$(CC) $(FLAGS) -c Sender.c myLib.h

clean:
	rm -f *.o Receiver Sender