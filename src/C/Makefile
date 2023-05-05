CC = gcc
FLAGS = -Wall -g
TARGETS = Sender Receiver

.PHONY: all clean

all: $(TARGETS)

Sender: Sender.c myHeader.h
	$(CC) $(FLAGS) -c $^
	$(CC) $(FLAGS) -o $@ $@.o


Receiver: Receiver.c myHeader.h
	$(CC) $(FLAGS) -c $^
	$(CC) $(FLAGS) -o $@ $@.o


clean:
	rm -f *.o *.h.gch $(TARGETS)