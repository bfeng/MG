#include <stdio.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */
#include <time.h>

const int size = 20000;
const int times = 100000;

void vector_add(int* a, int* b, int* c, int n)
{
  int i;
  for(i = 0;i<n;i++)
  {
    c[i] = a[i] + b[i];
  }
}

void test_on_host(int* a, int* b, int* c, int n)
{
  struct timespec start, end;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

  int i;
  for(i = 0;i<times;i++)
  {
    vector_add(a, b, c, size);
  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

  printf("[CPU] Time spent:%lu.%lu s.\n", end.tv_sec-start.tv_sec, end.tv_nsec-start.tv_nsec);
}

void test_on_kernel(int* a, int* b, int* c, int n)
{
  // Load the kernel source code into the array source_str
  FILE *fp;
  char *source_str;
  size_t source_size;

  fp = fopen("vector_add_kernel.cl", "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    return;
  }
  source_str = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose( fp );

  // Get platform and device information
  cl_platform_id platform_id = NULL;
  cl_device_id device_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
      &device_id, &ret_num_devices);
  printf("%d \n", (int)ret_num_devices);

  // Create an OpenCL context
  cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

  // Create a command queue
  cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

  // Create a program from the kernel source
  cl_program program = clCreateProgramWithSource(context, 1,
      (const char **)&source_str, (const size_t *)&source_size, &ret);

  // Build the program
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

  // Create the OpenCL kernel
  cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);

  // Create memory buffers on the device for each vector
  cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
      size * sizeof(int), NULL, &ret);
  cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
      size * sizeof(int), NULL, &ret);
  cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
      size * sizeof(int), NULL, &ret);

  // Copy array a and b to their respective memory buffers
  ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
      size * sizeof(int), a, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
      size * sizeof(int), b, 0, NULL, NULL);

  struct timespec start, end;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

  int i;
  for(i = 0;i<times;i++)
  {
    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = size; // Process the entire lists
    size_t local_item_size = 64; // Process one item at a time
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
        &global_item_size, &local_item_size, 0, NULL, NULL);

  }

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

  printf("[GPU] Time spent:%lu.%lu s.\n", end.tv_sec-start.tv_sec, end.tv_nsec-start.tv_nsec);

  // Read the memory buffer C on the device to the local variable C
  c = (int*)malloc(sizeof(int)*size);
  ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
      size * sizeof(int), c, 0, NULL, NULL);

  // clean up
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(a_mem_obj);
  ret = clReleaseMemObject(b_mem_obj);
  ret = clReleaseMemObject(c_mem_obj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);
}

int main()
{
  int a[size];
  int b[size];
  int c[size];

  int i;
  for(i = 0;i<size;i++)
  {
    a[i] = b[i] = i;
  }

  test_on_host(a,b,c,size);
  test_on_kernel(a,b,c,size);
  return 0;
}
