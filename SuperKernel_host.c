#include "SuperKernel_host.h"

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
#include "QueueJobs.h"
#include "OpenCL_compiler.h"
#include "OpenCL_launcher.h"

static void check_err(cl_int);
pthread_t start_IncomingJobsManager();
void *main_IncomingJobsManager();
pthread_t start_ResultsManager();
void *main_ResultsManager();


void SuperKernel_init(cl_context context)
{
  //#-1 Inputs
  printf("#####################################\n");
  printf("# Please give inputs to SuperKernel #\n");
  printf("#####################################\n");

  printf("\nPlease enter the warps number >>>");
  scanf("%d", &THE_warps);
  printf("\nPlease enter the blocks number >>>");
  scanf("%d", &THE_blocks);
  printf("\nPlease enter the number of jobs per warp >>>");
  scanf("%d", &THE_numJobsPerWarp);
  printf("\nPlease enter the sleep time (sec) >>>");
  scanf("%d", &THE_SLEEP_TIME);
  
  THE_NUMBER_OF_JOBS = THE_warps * THE_blocks * THE_numJobsPerWarp;
  
  //#-2 Create CommandQueue
  //##########################################
  //# currently only single cpu is supported #
  //##########################################
  cl_int ERR;
  command_queue = clCreateCommandQueue(context, devices[0], 0, &ERR);
  check_err(ERR);
  
  //#-3 Create QueueJobs
  CreateQueues(256000, context, command_queue);
  
  //#-4 Compile OpenCL Kernel program
  char filename[256] = "SuperKernel_device.cl";
  openCL_compiler(filename, context, &devices[0]);
  
  //#-5 Run kernel
  OpenCL_launcher_struct * l_struct = (OpenCL_launcher_struct *) malloc (sizeof(OpenCL_launcher_struct));
  
  
  //#-6 create pthreads to handle QueueJobs
  
  pthread_mutex_init(&memcpyLock, NULL);
  
  pthread_t IncomingJobManager = start_IncomingJobsManager();
  pthread_t ResultsManager = start_ResultsManager();
  
  pthread_join(IncomingJobManager, NULL);
  pthread_join(ResultsManager, NULL);
  

  //#-7 Finish and recycle
  printf("Both managers have finished\n");
  printf("Destorying Queues...\n");
    
  DisposeQueues();
  
  pthread_mutex_destroy(&memcpyLock);
  
  
}


pthread_t start_IncomingJobsManager()
{
//This should do any initializing that the incoming jobs
//  manager will need and then launch a thread running
//  main_IncomingJobsManager(  ), returning that thread

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_t thread1;
  
  int rc;
  rc = pthread_create( &thread1, &attr, main_IncomingJobsManager, NULL);
  if (rc) {
         printf("ERROR; return code from pthread_create is %d\n", rc);
         exit(-1);
         }
         
  pthread_attr_destroy(&attr);

  return thread1;
}
void *main_IncomingJobsManager()
{
//The thread should get job descriptions some how and Enqueue them
//  into the queue in params
//    --eventually this should get jobs from an external application
//      but will probably just be hardcoded at first or a parameter


  int HC_JobType = 2;
  int HC_JobID;
  int HC_numThreads = 32;
  int HC_jobs = 1;

  printf("Starting IncomingJobs Manager\n");

  int i;
  for(i=0;i<HC_jobs;i++){
    HC_JobID = i;

    JobDescription *h_JobDescription;
    h_JobDescription = (JobDescription *) malloc(sizeof(JobDescription));
    
    h_JobDescription->JobType = HC_JobType;
    h_JobDescription->JobID = HC_JobID;
    //even hard coded
    h_JobDescription->params = THE_SLEEP_TIME; 
    h_JobDescription->numThreads = HC_numThreads;

    EnqueueJob(h_JobDescription, command_queue, memcpyLock);

    free(h_JobDescription);
  }
  printf("Finished Incoming Jobs Manager\n");
  return 0;
}


pthread_t start_ResultsManager()
{
//This should do any initializing that the results manager will
//  need and then launch a thread running main_ResultsManager,
//  returning this thread

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_t thread2;
  pthread_create( &thread2, NULL, main_ResultsManager, NULL);
  return thread2;
}
void *main_ResultsManager()
{
//The thread should read results from the queue in param and
//  print something about them to the screen.
//    --eventually this should return the result to the application
//      that requested the work.
  printf("Starting ResultsManager\n"); 

  int HC_jobs = THE_NUMBER_OF_JOBS;
  int i;
  JobDescription *currentJob;
  
  for(i=0;i<HC_jobs;i++){
  
    currentJob = FrontAndDequeueResult(command_queue, memcpyLock);
    
  }
  return 0;
}




void check_err(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error in SuperKernel_host!!\n");
    printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
  }
}
