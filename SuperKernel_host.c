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

static void check_err(cl_int);

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
  CreateQueues(THE_NUMBER_OF_JOBS, context, command_queue);
  
  
  
  
  
  
  
  
  
  
  DisposeQueues();
  
  
  
  
}



void *main_IncomingJobsManager()
{
//The thread should get job descriptions some how and Enqueue them
//  into the queue in params
//    --eventually this should get jobs from an external application
//      but will probably just be hardcoded at first or a parameter

  Queue d_newJobs = (Queue) p;

  // Hard code for testing
  int HC_JobType = 2; // hard code the job type for sleeps
  int HC_JobID;
  int HC_numThreads = 32;
  int HC_jobs = NUMBER_OF_JOBS;
  //  int HC_matrixWidth = 32;
  //int HC_matrixSize = HC_matrixWidth * HC_matrixWidth;



  printf("Starting IncomingJobs Manager\n");

  void * d_sleep_time = moveToCuda(&SLEEP_TIME, sizeof(int));

  int i;
  for(i=0;i<HC_jobs;i++){
    HC_JobID = i;
    // launch queue jobs
    // malloc the host structure
    JobDescription *h_JobDescription = (JobDescription *) malloc(size);

    // set the values to the host structure
    h_JobDescription->JobType = HC_JobType;
    h_JobDescription->JobID = HC_JobID;

    h_JobDescription->params = d_sleep_time; 

    h_JobDescription->numThreads = HC_numThreads;

    //
    EnqueueJob(h_JobDescription, command_queue);
    //printf("Finished EnqueueJob # %d\n", HC_JobID);

    // free the local memory
    free(h_JobDescription);
  }
  printf("Finished Incoming Jobs Manager\n");
  return 0;
}

static void check_err(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error in SuperKernel_host!!\n");
    printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
  }
}
