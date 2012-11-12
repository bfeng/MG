.phony: all clean

CC=gcc

objects=main.o

all: MG
	echo all: make complete

MG: $(objects)
	$(CC) -o $@ $+

clean:
	rm *.h.gch
	rm $(objects)

%.o:%.c
	$(CC) -c $+

main.o: main.c headers.h
