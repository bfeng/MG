#ifndef OPENCL_HEADERS_INCLUDED
#define OPENCL_HEADERS_INCLUDED
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif
#endif



int THE_warps;
int THE_blocks;
int THE_numJobsPerWarp;
//just sleep, just as cuda
int THE_SLEEP_TIME;

int THE_NUMBER_OF_JOBS;

cl_command_queue command_queue;

extern cl_device_id *devices;

pthread_mutex_t memcpyLock;






void SuperKernel_init(int warps, int blocks, int num_job_per_warp, int sleep_time, cl_context context);
