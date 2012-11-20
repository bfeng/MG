#include "QueueJobs.h"
#ifndef STDIO_INCLUDED
#define STDIO_INCLUDED
#include <stdio.h>
#endif
#ifndef STDLIB_INCLUDED
#define STDLIB_INCLUDED
#include <stdlib.h>
#endif

void check_err(cl_int);

//Constructor and Destructor


void CreateQueues(int MaxElements, cl_context context) {
  Queue Q = (Queue) malloc (sizeof(struct QueueRecord));
  Q->Capacity = MaxElements;
  Q->Front = 1;
  Q->Rear = 0;
  Q->ReadLock = 0;
  
  cl_int ERR;
  d_newJobs = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(struct QueueRecord), 
                             NULL, ERR);
  check_err(ERR);
  d_newJobs_array = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                                   sizeof(struct JobDescription) * MaxElements, 
                                   NULL, ERR);
  check_err(ERR);
  d_finishedJobs = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(struct QueueRecord), 
                                  NULL, ERR);
  check_err(ERR);
  d_finishedJobs_array = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
                                        sizeof(struct JobDescription) * MaxElements, 
                                        NULL, ERR);
  check_err(ERR);

  
  clEnqueueWriteBuffer

  cudaSafeMemcpy(d_Q, Q, sizeof(struct QueueRecord), 
                 cudaMemcpyHostToDevice, stream_dataIn, 
                 "Copying initial queue to device");
  free(Q);

}

void check_err(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error creating buffer!!\n");
    printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
  }
}




void DisposeQueue(Queue d_Q) {
  Queue h_Q = (Queue) malloc(sizeof(struct QueueRecord));
  cudaSafeMemcpy(h_Q, d_Q, sizeof(struct QueueRecord), 
                 cudaMemcpyDeviceToHost, stream_dataIn,
                 "DisposeQueue, Copying Queue to get array pointer");
  cudaFree(h_Q->Array);
  free(h_Q);
  cudaFree(d_Q);
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
