
#include "OpenCL_launcher.h"
#include <pthread.h>

void * kernel_launcher(void *);
void check_err(cl_int);
static void check_err_easy(cl_int);

void openCL_launcher(cl_context k_context, cl_device_id device, 
                     OpenCL_launcher_struct l_struct, 
                     cl_mem * in,
                     cl_mem * in_array,
                     cl_mem * out,
                     cl_mem * out_array,
                     int * numJobsPerWarp)
{
  //Kernel is in Opencl_compiler.h 
  //context is in superkernel_host.c
  //(above) To generate command queue
  
  //#-1 set argument
  cl_int ERR_arg;
  ERR_arg = clSetKernelArg(l_struct.kernel, 0, sizeof(cl_mem), (void *)in);
  ERR_arg = clSetKernelArg(l_struct.kernel, 1, sizeof(cl_mem), (void *)in_array);
  ERR_arg = clSetKernelArg(l_struct.kernel, 2, sizeof(cl_mem), (void *)out);
  ERR_arg = clSetKernelArg(l_struct.kernel, 3, sizeof(cl_mem), (void *)out_array);
  ERR_arg = clSetKernelArg(l_struct.kernel, 4, sizeof(int), (void *)numJobsPerWarp);
  
  //#-2 create individual command_queue
  cl_int ERR;
  k_command_queue = clCreateCommandQueue(k_context, device, 0, &ERR);
  check_err_easy(ERR);
  
  l_struct.command_queue = k_command_queue;
  
  //#-3 create kernel launch pthread with a new cl_command_queue
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_t thread3;
  
  int rc;
  rc = pthread_create( &thread3, &attr, kernel_launcher, (void *) &l_struct);
  if (rc) {
         printf("ERROR; return code from pthread_create is %d\n", rc);
         exit(-1);
         }
         
  pthread_attr_destroy(&attr);
    
}


void * kernel_launcher(void * l_struct)
{
  cl_int ERR;
  
  //to-do
  ERR = clEnqueueNDRangeKernel( //list of arguments
                              ((OpenCL_launcher_struct*)l_struct) -> command_queue,
                              ((OpenCL_launcher_struct*)l_struct) -> kernel,
                              ((OpenCL_launcher_struct*)l_struct) -> work_dim,
                              &(((OpenCL_launcher_struct*)l_struct) -> global_work_offset),
                              &(((OpenCL_launcher_struct*)l_struct) -> global_work_size),
                              &(((OpenCL_launcher_struct*)l_struct) -> local_work_size),
                              0, NULL, NULL);
  check_err(ERR);
  ERR = clFinish( ((OpenCL_launcher_struct*)l_struct) -> command_queue );
  check_err(ERR);
  
}

void check_err(cl_int err)
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
static void check_err_easy(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error in OpenCL_launcher!!\n");
    printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
  }
}
