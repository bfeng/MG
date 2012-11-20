.phony: all clean test

CC=gcc
SOURCES=main.c sys_info.c SuperKernel_host.c
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

test:
	###############################################
	##### Compiler must be gcc-4.4 or g++-4.4 #####
	###############################################
	gcc -o MG $(SOURCES) -I/usr/local/cuda/include -L/usr/lib64/ -lOpenCL


