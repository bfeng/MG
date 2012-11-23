#ifndef OPENCL_HEADERS_INCLUDED
#define OPENCL_HEADERS_INCLUDED
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif
#endif

#ifndef STDIO_INCLUDED
#define STDIO_INCLUDED
#include <stdio.h>
#endif
#ifndef STDLIB_INCLUDED
#define STDLIB_INCLUDED
#include <stdlib.h>
#endif
#ifndef PTHREAD_INCLUDED
#define PTHREAD_INCLUDED
#include <string.h>
#endif

#define WARP_SIZE 32

typedef struct OpenCL_launcher_struct{
//additional arguments

cl_command_queue command_queue;//not from superkernel
cl_kernel kernel;
cl_uint work_dim;
size_t global_work_offset;
size_t global_work_size;
size_t local_work_size;
//events ignored for now

} OpenCL_launcher_struct;



cl_command_queue k_command_queue;

//void openCL_launcher();
