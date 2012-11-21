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
struct JobDescription{
  int JobID;
  int JobType;
  int numThreads;
  int params; //OpenCL can only accept fixed array, and running kernel can never know new buffer
};
typedef struct JobDescription *JobPointer; 

struct QueueRecord {
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







void CreateQueues(int, cl_context, cl_command_queue);
void DisposeQueues();
void EnqueueJob(JobDescription *, cl_command_queue);
JobDescription * FrontAndDequeueResult(cl_command_queue);
