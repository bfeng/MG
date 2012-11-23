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

#define MAX_SOURCE_SIZE (0x100000)

//data struct
cl_kernel openCL_kernel;
cl_program openCL_program;

void openCL_compiler(char *, cl_context, cl_device_id *);
