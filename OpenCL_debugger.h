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
#include <pthread.h>
#endif

typedef struct OpenCL_debugger_struct{

cl_command_queue command_queue;//not from superkernel
cl_context context;
cl_device_id device;
cl_mem mem;


} OpenCL_debugger_struct;

void openCL_debugger(cl_context debugger_context, cl_device_id debug_device, cl_mem debug_mem);
