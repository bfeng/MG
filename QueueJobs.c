#include "QueueJobs.h"
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


void EnqueueJob(JobDescription * h_jobDescription, cl_command_queue command_queue)
{
  
  Queue h_Q = (Queue) malloc(sizeof(struct QueueRecord));
  clEnqueueReadBuffer(command_queue, d_newJobs, CL_TRUE, 0, sizeof(h_Q), &h_Q, 0, NULL, NULL);
  while(h_IsFull(h_Q))
  {
    pthread_yield();
    clEnqueueReadBuffer(command_queue, d_newJobs, CL_TRUE, 0, 
                        sizeof(h_Q), &h_Q, 0, NULL, NULL);
  }
  
  h_Q->Rear = (h_Q->Rear+1)%(h_Q->Capacity);
  clEnqueueWriteBuffer(command_queue, d_newJobs, CL_TRUE, sizeof(JobDescription) * h_Q->Rear, 
                       sizeof(JobDescription), h_jobDescription, 0, NULL, NULL);
  
  clEnqueueWriteBuffer(command_queue, d_newJobs, CL_TRUE, 4+sizeof(JobDescription*), 
                       sizeof(int), &h_Q->Rear, 0, NULL, NULL);
                       
  free(h_Q);
}

JobDescription * FrontAndDequeueResult(cl_command_queue command_queue)
{
  Queue h_Q = (Queue) malloc(sizeof(struct QueueRecord));
  clEnqueueReadBuffer(command_queue, d_finishedJobs, CL_TRUE, 0, sizeof(h_Q), 
                      &h_Q, 0, NULL, NULL);
  while(h_IsEmpty(h_Q)){
    pthread_yield();
    clEnqueueReadBuffer(command_queue, d_finishedJobs, CL_TRUE, 0, sizeof(h_Q), 
                        &h_Q, 0, NULL, NULL);
  }
  
  JobDescription *result = (JobDescription *) malloc(sizeof(JobDescription));
  clEnqueueReadBuffer(command_queue, d_finishedJobs, CL_TRUE, 
                      sizeof(JobDescription) * h_Q->Front, 
                      sizeof(JobDescription), result, 0, NULL, NULL);
  
  h_Q->Front = (h_Q->Front+1)%(h_Q->Capacity);
  clEnqueueWriteBuffer(command_queue, d_finishedJobs, CL_TRUE, 8+sizeof(JobDescription*), 
                       sizeof(int), &h_Q->Rear, 0, NULL, NULL);
  free(h_Q);
  return result;
  
}




//Helper Functions

int h_IsEmpty(Queue Q) {
  return (Q->Rear+1)%Q->Capacity == Q->Front;
}

int h_IsFull(Queue Q) {
  return (Q->Rear+2)%Q->Capacity == Q->Front;
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
