#include "SuperKernel_host.h"

#ifndef STDIO_INCLUDED
#define STDIO_INCLUDED
#include <stdio.h>
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

static void check_err(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error in SuperKernel_host!!\n");
    printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
  }
}
