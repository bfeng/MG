#include "SuperKernel_host.h"

#ifndef STDIO_INCLUDED
#define STDIO_INCLUDED
#include <stdio.h>
#endif
#ifndef STDLIB_INCLUDED
#define STDLIB_INCLUDED
#include <stdlib.h>
#endif

#include <pthread.h>

#include "QueueJobs.h"
#include "OpenCL_compiler.h"
#include "OpenCL_launcher.h"
#include "OpenCL_debugger.h"

static void check_err(cl_int);
pthread_t start_IncomingJobsManager();
void *main_IncomingJobsManager();
pthread_t start_ResultsManager();
void *main_ResultsManager();


void SuperKernel_init(int warps, int blocks, int num_job_per_warp, int sleep_time, cl_context context)
{
  printf("Warps: %d\n", warps);
  printf("Blocks: %d\n", blocks);
  printf("Number of jobs per warp: %d\n", num_job_per_warp);
  printf("Sleep time: %d\n", sleep_time);

  //#-1 Inputs
  // printf("#####################################\n");
  // printf("# Please give inputs to SuperKernel #\n");
  // printf("#####################################\n");

  // printf("\nPlease enter the warps number >>>");
  // scanf("%d", &THE_warps);
  // printf("\nPlease enter the blocks number >>>");
  // scanf("%d", &THE_blocks);
  // printf("\nPlease enter the number of jobs per warp >>>");
  // scanf("%d", &THE_numJobsPerWarp);
  // printf("\nPlease enter the sleep time (sec) >>>");
  // scanf("%d", &THE_SLEEP_TIME);

  THE_warps = warps;
  THE_blocks = blocks;
  THE_numJobsPerWarp = num_job_per_warp;
  THE_SLEEP_TIME = sleep_time;

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
  
  cl_mem d_sleeptime = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), 
                             NULL, &ERR);
  check_err(ERR);
  clEnqueueWriteBuffer(command_queue, d_sleeptime, CL_TRUE, 0, sizeof(int), &THE_SLEEP_TIME, 0, NULL, NULL);
  
                             

  //#-4 Compile OpenCL Kernel program
  
  //char filename[256] = "SuperKernel_device.cl";
  char filename[256] = "OpenCL_sleep.cl";
  openCL_compiler(filename, context, &devices[0]);

  //#-5 Run kernel
  /*
  OpenCL_launcher_struct * l_struct = (OpenCL_launcher_struct *) malloc (sizeof(OpenCL_launcher_struct));

  //##########################################
  //# input for EnqueueNDRangeKernel         #
  //##########################################
  l_struct->command_queue = NULL;
  l_struct->kernel = openCL_kernel;
  l_struct->work_dim = (cl_int)1;
  l_struct->global_work_offset = (size_t)NULL;
  l_struct->global_work_size = (size_t)32*THE_warps*THE_blocks;
  l_struct->local_work_size = (size_t)32*THE_warps;

  openCL_launcher(context, devices[0],
                     l_struct,
                     &d_newJobs,
                     &d_newJobs_array,
                     &d_finishedJobs,
                     &d_finishedJobs_array,
                     &d_debug,
                     &THE_numJobsPerWarp);

  */
  
  //#-5b test debugger run with sleep
  //sleep_command_queue = clCreateCommandQueue(context, devices[0], 0, &ERR);
  check_err_easy(ERR);
  ERR = clSetKernelArg(openCL_kernel, 0, sizeof(cl_mem), (void *)&d_sleeptime);
  ERR = clSetKernelArg(openCL_kernel, 1, sizeof(cl_mem), (void *)&d_debug);
  
  size_t global_work_size_d = THE_warps;
  //THE_blocks * 48 * THE_warps;
  size_t local_work_size = THE_numJobsPerWarp;
  //48 * THE_warps;
  int ii = 0;
  
  //for (ii;ii<THE_warps;ii++)
  //{
    ERR = clEnqueueNDRangeKernel( //list of arguments
                              command_queue,
                              openCL_kernel,
                              1,
                              NULL,
                              &global_work_size_d,
                              &local_work_size,
                              0, NULL, NULL);
    check_err(ERR);
  //}
  
  /*
  for (ii;ii<3;ii++)
  {
  ERR = clEnqueueTask(command_queue, openCL_kernel, 0, NULL, NULL);
  }
  */
  
  ERR = clFinish(command_queue);
  //check_err(ERR);
  //#-6 Create debug pthread
  openCL_debugger(context, devices[0], d_debug);

  //#-7 create pthreads to handle QueueJobs

  pthread_mutex_init(&memcpyLock, NULL);

  //pthread_t IncomingJobManager = start_IncomingJobsManager();
  //pthread_t ResultsManager = start_ResultsManager();

  //pthread_join(IncomingJobManager, NULL);
  //pthread_join(ResultsManager, NULL);
  pthread_join(P_Debugger, NULL);

  //#-8 Finish and recycle
  //printf("Both managers have finished\n");
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
    
    switch(err)
    {
      case CL_INVALID_MEM_OBJECT:
        printf("Memroy Object Invalid\n");
        break;
      case CL_OUT_OF_RESOURCES:
        printf("failure to allocate OpenCL resources on the device\n");
        break;
      case CL_INVALID_GLOBAL_OFFSET:
        printf("global offset\n");
        break;
      case CL_INVALID_WORK_GROUP_SIZE:
        printf("invalid work group size\n");
        break;
      case CL_INVALID_WORK_ITEM_SIZE:
        printf("work item size\n");
        break;
      case CL_INVALID_WORK_DIMENSION:
        printf("work demension\n");
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
  }
}
