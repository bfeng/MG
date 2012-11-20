#ifndef OPENCL_HEADERS_INCLUDED
#define OPENCL_HEADERS_INCLUDED
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif
#endif



//Data types
//To keep consistency with CUDA code
typedef struct JobDescription{
  int JobID;
  int JobType;
  int numThreads;
  void *params;
};
typedef struct JobDescription *JobPointer; 

typedef struct QueueRecord {
  struct JobDescription *Array; 
  int Capacity;                 
  int Rear;
  int Front;
  int ReadLock;
};
typedef struct QueueRecord *Queue;

cl_mem d_newJobs;
cl_mem d_finishedJobs;
cl_mem d_newJobs_array;
cl_mem d_finishedJobs_array;
