#include "OpenCL_debugger.h"
#include <pthread.h>

void * kernel_debugger(void * pt);
static void check_err3(cl_int err);

void openCL_debugger(cl_context debugger_context, cl_device_id debug_device, cl_mem debug_mem)
{
  //#-1 Create debug command queue

  cl_int ERR;
  cl_command_queue debugger_command_queue = clCreateCommandQueue(debugger_context, debug_device, 0, &ERR);
  OpenCL_debugger_struct * pt = (OpenCL_debugger_struct *) malloc(sizeof(OpenCL_debugger_struct));
  pt->context = debugger_context;
  pt->device = debug_device;
  pt->command_queue = debugger_command_queue;
  pt->mem = debug_mem;
  
  
  
  //#-2 Create Thread to read the cl_mem
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_t thread4;
  int rc;
  rc = pthread_create( &thread4, &attr, kernel_debugger, (void *)pt);
  if (rc) {
         printf("ERROR; return code from pthread 4 _create is %d\n", rc);
         exit(-1);
         }
         
  pthread_attr_destroy(&attr);
  
}

void *kernel_debugger(void * pt)
{
  cl_int ERR;
  size_t size = sizeof(char)*1024;
  char * ret_ptr = (char *)malloc(sizeof(char)*1024);
  
  ERR = clEnqueueReadBuffer(((OpenCL_debugger_struct*)pt)->command_queue,
                      ((OpenCL_debugger_struct*)pt)->mem,
                      CL_TRUE,
                      0,
                      size,
                      ret_ptr, 0, NULL, NULL);
  check_err3(ERR);
  
  ERR = clFinish(((OpenCL_debugger_struct*)pt)->command_queue);
  check_err3(ERR);
  if(*ret_ptr!= '1')
  { 
    ERR = clEnqueueReadBuffer(((OpenCL_debugger_struct*)pt)->command_queue,
                      ((OpenCL_debugger_struct*)pt)->mem,
                      CL_TRUE,
                      0,
                      size,
                      ret_ptr, 0, NULL, NULL);
    check_err3(ERR);
    ERR = clFinish(((OpenCL_debugger_struct*)pt)->command_queue);
    check_err3(ERR);
  } else if (*ret_ptr == '1')
  {
    printf("########################\n");
    printf("#activate debug memory!#\n");
    printf("########################\n");
  }
  
  return 0;
  
}
static void check_err3(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error relating to buffer read or write!!(debugger)\n");
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
    
    
  }//else printf("SUCCESS\n");
}


