#include "QueueJobs.h"
#ifndef STDIO_INCLUDED
#define STDIO_INCLUDED
#include <stdio.h>
#endif
#ifndef STDLIB_INCLUDED
#define STDLIB_INCLUDED
#include <stdlib.h>
#endif

static void check_err(cl_int);

//Constructor and Destructor


void CreateQueues(int MaxElements, cl_context context, cl_command_queue command_queue) {
  Queue Q = (Queue) malloc (sizeof(struct QueueRecord));
  Q->Capacity = MaxElements;
  Q->Front = 1;
  Q->Rear = 0;
  Q->ReadLock = 0;
  
  cl_int ERR;
  d_newJobs = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(struct QueueRecord), 
                             NULL, &ERR);
  check_err(ERR);
  d_newJobs_array = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                                   sizeof(struct JobDescription) * MaxElements, 
                                   NULL, &ERR);
  check_err(ERR);
  d_finishedJobs = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(struct QueueRecord), 
                                  NULL, &ERR);
  check_err(ERR);
  d_finishedJobs_array = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
                                        sizeof(struct JobDescription) * MaxElements, 
                                        NULL, &ERR);
  check_err(ERR);

  
  clEnqueueWriteBuffer(command_queue, d_newJobs, CL_TRUE, 0, sizeof(Q), &Q, 0, NULL, NULL);
  clEnqueueWriteBuffer(command_queue, d_finishedJobs, CL_TRUE, 0, sizeof(Q), &Q, 0, NULL, NULL);
  free(Q);

}

static void check_err(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error creating buffer!!\n");
    printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
  }
}




void DisposeQueues() {
   cl_int ERR;
   clReleaseMemObject(d_newJobs);
   check_err(ERR);
   clReleaseMemObject(d_finishedJobs);
   check_err(ERR);
   clReleaseMemObject(d_newJobs_array);
   check_err(ERR);
   clReleaseMemObject(d_finishedJobs_array);
   check_err(ERR);
   
}







//Host Helper Functions

int h_IsEmpty(Queue Q) {
  return (Q->Rear+1)%Q->Capacity == Q->Front;
}

int h_IsFull(Queue Q) {
  return (Q->Rear+2)%Q->Capacity == Q->Front;
}

void *movePointer(void *p, int n){
   char * ret = (char *) p;
   return ((void *)(ret+n));
}

/* error handler

void printAnyErrors()
{
  cudaError e = cudaGetLastError();
  if(e!=cudaSuccess){
    printf("CUDA Error: %s\n", cudaGetErrorString( e ) );
  }
}

*/
