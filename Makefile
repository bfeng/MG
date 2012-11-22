.phony: all clean test

CC=gcc
SOURCES=main.c sys_info.c SuperKernel_host.c QueueJobs.c OpenCL_compiler.c
objects=main.o sys_info.o SuperKernel_host.o QueueJobs.o OpenCL_compiler.o
params=-I/usr/local/cuda/include -L/usr/lib64/ -lOpenCL

all: MG
	echo all: make complete

MG: $(objects)
	$(CC) -o $@ $+ $(params)

clean:
	rm $(objects)
	rm *.h.gch

%.o:%.c
	$(CC) -c $+ $(params)

#
# The following list will list out all objects
# which are generated while compiling.
# We firstly compile the objects is to make
# sure each part of the program is okey to use
# for the main program.
# It is easier to debug the code.
# 
main.o: main.c headers.h

sys_info.o: sys_info.c sys_info.h

SuperKernel_host.o: SuperKernel_host.c SuperKernel_host.h

OpenCL_compiler.o: OpenCL_compiler.c OpenCL_compiler.h

test:
	###############################################
	##### Compiler must be gcc-4.4 or g++-4.4 #####
	###############################################
	##### Now "make all" has the same effect  #####
	###############################################
	gcc -o MG $(SOURCES) -I/usr/local/cuda/include -L/usr/lib64/ -lOpenCL
