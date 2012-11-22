.phony: all clean test

CC=gcc
SOURCES=main.c sys_info.c SuperKernel_host.c QueueJobs.c OpenCL_compiler.c
objects=main.o

all: MG
	echo all: make complete

MG: $(objects)
	$(CC) -o $@ $+

clean:
	rm $(objects)
	rm *.h.gch

%.o:%.c
	$(CC) -c $+

main.o: main.c headers.h

test:
	###############################################
	##### Compiler must be gcc-4.4 or g++-4.4 #####
	###############################################
	gcc -o MG $(SOURCES) -I/usr/local/cuda/include -L/usr/lib64/ -lOpenCL


