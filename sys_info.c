#include "sys_info.h"

#include <stdio.h>

void sys_init()
{ 
  printf("Initialize OpenCL context ... \n");
  
  //#-1 get platforms
  num_entries_plat = NUM_OpenCL_MAX;
  printf("%d ", NUM_OpenCL_MAX);
  platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_entries_plat);
  num_platforms = (cl_uint*) malloc(sizeof(cl_uint));
  
  ret_clGetPlatformIDs = clGetPlatformIDs(num_entries_plat, platforms, num_platforms);
  printf("DEBUG1\n"); 
  switch (ret_clGetPlatformIDs)
  {
    case CL_SUCCESS:
      printf("%d Platform Acquired!\n", *num_platforms);
      if((int)*num_platforms==2)
      {
        printf("NOT SUPPORTING Multi-platforms, please terminate this program\n");
        while(1){}
      }      
      break;
    case CL_INVALID_VALUE:
      printf("No Platform found! Or check NUM_GPU_MAX value.\n");
      break;
    case CL_OUT_OF_HOST_MEMORY:
      printf("Host out of memory.\n");
      break;
    default:
      printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
  }
  
  //#-2 get devices
  num_entries_dev = NUM_GPU_MAX;
  devices = (cl_device_id*) malloc(sizeof(cl_device_type)*num_entries_dev);
  num_devices = (cl_uint*) malloc(sizeof(cl_uint));
    
  ret_clGetDeviceIDs = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, num_entries_dev,
                                       devices, num_devices);
  
    switch (ret_clGetDeviceIDs)
  {
    case CL_SUCCESS:
      printf("%d GPU-device Acquired!\n", *num_devices);
      break;
    case CL_DEVICE_NOT_FOUND:
      printf("No GPU-device found!\n");
      break;
    case CL_OUT_OF_HOST_MEMORY:
      printf("Host out of memory.\n");
      break;
    default:
      printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
  }
  
  int i;
  for (i = 0; i < (int)*num_devices; i++)
  {
    printf("  # - %d - :\n", i);
    char buffer[1024];
    cl_uint buf_uint;
    cl_ulong buf_ulong;
    clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
    printf("  DEVICE_NAME = %s\n", buffer);
    clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
    printf("  DEVICE_VENDOR = %s\n", buffer);
    clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
    printf("  DRIVER_VERSION = %s\n", buffer);
    clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint),
                    &buf_uint, NULL);
    printf("  DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
    clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), 
                    &buf_uint, NULL);
    printf("  DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
    clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), 
                    &buf_ulong, NULL);
    printf("  DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
    
  }
  
  //#-3 get context
  
  context_SINGLE = clCreateContext(NULL, 1, &devices[0], NULL, NULL, 
                                   &ret_CreateContext_SINGLE);
  switch (ret_CreateContext_SINGLE)
  {
    case CL_SUCCESS:
      printf("Context for SINGLE GPU processing environment created!\n");
      break;
    case CL_DEVICE_NOT_AVAILABLE:
      printf("No GPU-device available right now!\n");
      break;
    case CL_OUT_OF_HOST_MEMORY:
      printf("Host out of memory.\n");
      break;
    default:
      printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
  }
  
  if((int)*num_devices > 1)
  {
    context_SINGLE = clCreateContext(NULL, *num_devices, devices, NULL, NULL, 
                                   &ret_CreateContext_MULTIPLE);
    switch (ret_CreateContext_MULTIPLE)
    {
      case CL_SUCCESS:
        printf("Context for %d GPUs processing environment created!\n", *num_devices);
        break;
      case CL_DEVICE_NOT_AVAILABLE:
        printf("No GPU-device available right now!\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("Host out of memory.\n");
        break;
      default:
        printf("#PLEASE ASK THE CODE WRITTER# if you see this.\n");
    }
  }
  
  
}

void user_context()
{
  context_mode = 1;
  if((int)*num_devices > 1)
  {
    printf("You have multiple GPU devices available on your machine.\n");
    printf("(1) Single GPU mode .\n");
    printf("(2) Multiple GPU mode .\n");
    printf("Please enter the context to run >>>");
    scanf("%d", &context_mode);
    if(context_mode == 1)return;
    else if(context_mode == 2)return;
    else
    {
      printf(" @@@ Sorry! Wrong input! Context is set to single GPU mode! @@@\n");
      context_mode = 2;
    }
  }
  
  return;  
}


void sys_destory()
{
  //#-1 release devices
  //int i;
  //for (i = 0; i < (int)*num_devices; i++) clReleaseDevice(devices[i]);
  
  //#-2 release contexts
  clReleaseContext(context_SINGLE);
  if((int)*num_devices > 1)clReleaseContext(context_MULTIPLE);
}
