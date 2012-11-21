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
//#include <unistd.h>
static void check_err(cl_int);
static void check_err2(cl_int);
int h_IsEmpty(Queue);
int h_IsFull(Queue);
void OpenCLSafeMemcpy(int mode, cl_mem device_mem, size_t offset, size_t size, void *ptr,  cl_command_queue a_command_queue, pthread_mutex_t memcpyLock);
//Constructor and Destructor


void CreateQueues(int MaxElements, cl_context context, cl_command_queue command_queue) {
  Queue Q = (Queue) malloc (sizeof(struct QueueRecord));
  Q->Capacity = MaxElements;
  printf("MAX = %d\n", Q->Capacity);
  Q->Front = 1;
  Q->Rear = 0;
  Q->ReadLock = 0;
  
  cl_int ERR;
  d_newJobs = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(QueueRecord), 
                             NULL, &ERR);
  check_err(ERR);
  d_newJobs_array = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                                   sizeof(JobDescription) * MaxElements, 
                                   NULL, &ERR);
  check_err(ERR);
  d_finishedJobs = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(QueueRecord), 
                                  NULL, &ERR);
  check_err(ERR);
  d_finishedJobs_array = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
                                        sizeof(JobDescription) * MaxElements, 
                                        NULL, &ERR);
  check_err(ERR);

  
  clEnqueueWriteBuffer(command_queue, d_newJobs, CL_TRUE, 0, sizeof(QueueRecord), Q, 0, NULL, NULL);
  clEnqueueWriteBuffer(command_queue, d_finishedJobs, CL_TRUE, 0, sizeof(QueueRecord), Q, 0, NULL, NULL);
  ERR = clFinish(command_queue);
  check_err(ERR);
  
  //debug
  /*
  Queue Qt = (Queue) malloc (sizeof(struct QueueRecord));
  clEnqueueReadBuffer(command_queue, d_newJobs, CL_TRUE, 0, sizeof(QueueRecord), Qt, 0, NULL, NULL); 
  ERR = clFinish(command_queue);
  check_err(ERR);
  printf("MAX = %d return\n", Qt->Capacity);
  */
  
  free(Q);

}

static void check_err(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error relating to buffer create or desotry!!\n");
    printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
    switch(err)
    {
      case CL_INVALID_MEM_OBJECT:
        printf("Memroy Object Invalid\n");
        break;
      case CL_OUT_OF_RESOURCES:
        printf("failure to allocate OpenCL resources on the device\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("Out Of Host Memory\n");
        break;
      case CL_INVALID_COMMAND_QUEUE:
        printf("invalid command queue\n");
        break;
      default:
        printf("Impossible Error.\n");
        break;
    }
    
  }
}

static void check_err2(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error relating to buffer read or write!!\n");
    printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
    switch(err)
    {
      case CL_INVALID_MEM_OBJECT:
        printf("Memroy Object Invalid\n");
        break;
      case CL_OUT_OF_RESOURCES:
        printf("failure to allocate OpenCL resources on the device\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("Out Of Host Memory\n");
        break;
      case CL_INVALID_COMMAND_QUEUE:
        printf("invalid command queue\n");
        break;
      case CL_INVALID_CONTEXT:
        printf("invalid context\n");
        break;
      case CL_INVALID_VALUE:
        printf("invalid value\n");
        break;
      case CL_INVALID_EVENT_WAIT_LIST:
        printf("invalid event list\n");
        break;
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        printf("mem allocation failure\n");
        break;
      case CL_INVALID_OPERATION:
        printf("invalid operation\n");
        break;
      default:
        printf("Impossible Error.\n");
    }
    
    
  }//else printf("SUCCESS");
}


void DisposeQueues() {
   cl_int ERR;
   ERR = clReleaseMemObject(d_newJobs);
   check_err(ERR);
   ERR = clReleaseMemObject(d_finishedJobs);
   check_err(ERR);
   ERR = clReleaseMemObject(d_newJobs_array);
   check_err(ERR);
   ERR = clReleaseMemObject(d_finishedJobs_array);
   check_err(ERR);
   
}
//////////////////////////////////////////////////
//  Host access device buffer opencl
//////////////////////////////////////////////////

void EnqueueJob(JobDescription * h_JobDescription, cl_command_queue command_queue, pthread_mutex_t memcpyLock)
{
  
  Queue h_Q = (Queue) malloc(sizeof(struct QueueRecord));
  if(h_Q == NULL)
  printf("ERROR allocating memory in EnqueueJob!\n");
  //1
  OpenCLSafeMemcpy(0, d_newJobs, 0, sizeof(QueueRecord), h_Q, command_queue, memcpyLock);
  //printf("h_Q->capacity = %d\n", h_Q->Capacity);
  
  while(h_IsFull(h_Q))
  {
    pthread_yield();
    
    //2
    OpenCLSafeMemcpy(0, d_newJobs, 0, sizeof(QueueRecord), h_Q, command_queue, memcpyLock);
  }
  
  h_Q->Rear = (h_Q->Rear+1)%(h_Q->Capacity);
  //3
  if(h_JobDescription == NULL) printf("NULLLLLLLLL");
  printf("%lu, %lu", sizeof(JobDescription) * h_Q->Rear, sizeof(JobDescription));

  
  OpenCLSafeMemcpy(1, d_newJobs_array, sizeof(JobDescription) * h_Q->Rear, sizeof(JobDescription), h_JobDescription, command_queue, memcpyLock);


  //4
  OpenCLSafeMemcpy(1, d_newJobs, 4+sizeof(JobDescription*), sizeof(int), &h_Q->Rear, command_queue, memcpyLock);  
    

    //sleep(1);
    //printf("\n%d\n", h_Q->Rear);                     
    //free(h_Q);
    
}

JobDescription * FrontAndDequeueResult(cl_command_queue command_queue, pthread_mutex_t memcpyLock)
{
  Queue h_Q = (Queue) malloc(sizeof(struct QueueRecord));
  //5
  OpenCLSafeMemcpy(0, d_finishedJobs, 0, sizeof(h_Q), h_Q, command_queue, memcpyLock);

    printf("HERE IS RESULT: %d\n", h_Q->Rear);
  
  while(h_IsEmpty(h_Q)){
  
    pthread_yield();
    
    //6
    OpenCLSafeMemcpy(0, d_finishedJobs, 0, sizeof(h_Q), h_Q, command_queue, memcpyLock);
  }
  
  
  JobDescription *result = (JobDescription *) malloc(sizeof(JobDescription));
  //7
  OpenCLSafeMemcpy(1, d_finishedJobs, sizeof(JobDescription) * h_Q->Front, sizeof(JobDescription), result, command_queue, memcpyLock);
  
  h_Q->Front = (h_Q->Front+1)%(h_Q->Capacity);
  //8
  OpenCLSafeMemcpy(1, d_finishedJobs, 8+sizeof(JobDescription*), sizeof(int), &h_Q->Rear, command_queue, memcpyLock);

  //free(h_Q);
  return result;
  
}




//Helper Functions

int h_IsEmpty(Queue Q) {
  return (Q->Rear+1)%Q->Capacity == Q->Front;
}

int h_IsFull(Queue Q) {
  return (Q->Rear+2)%Q->Capacity == Q->Front;
}

void OpenCLSafeMemcpy(int mode, cl_mem device_mem, size_t offset, size_t size, void *ptr, cl_command_queue a_command_queue, pthread_mutex_t memcpyLock)
{ //READ=mode(0)
  //WRITE=mode(1)
  //pthread_mutex_lock(&memcpyLock);
  cl_int ERR1;
  
  printf("%lu, %lu", offset, size);
  
  if(mode == 0)
  {
    ERR1 = clEnqueueReadBuffer(a_command_queue, device_mem, CL_TRUE, offset, size, ptr, 0, NULL, NULL);
    
    
    check_err2(ERR1);
  }
  else if(mode == 1)
  {
    ERR1 = clEnqueueWriteBuffer(a_command_queue, device_mem, CL_TRUE, offset, size, ptr, 0, NULL, NULL);

    
    check_err2(ERR1);
    
  }
  else printf("Check code calling safe memeory copy!");

  ERR1 = clFinish(a_command_queue);
  check_err2(ERR1);
  
  //pthread_mutex_unlock(&memcpyLock);

}

/* error handler

void printAnyErrors()
{
  cudaError e = cudaGetLastError();
  if(e!=cudaSuccess){
    printf("CUDA Error: %s\n", cudaGetErrorString( e ) );
  }
}
printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
printf("@@@@  Debug 1                  @@@@\n");
printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
*/
