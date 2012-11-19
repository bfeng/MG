#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#define NUM_OpenCL_MAX 2
#define NUM_GPU_MAX 4

cl_int ret_clGetPlatformIDs;
cl_platform_id * platforms;
cl_uint num_entries_plat;
cl_uint *num_platforms;


cl_int ret_clGetDeviceIDs;
cl_device_id *devices;
cl_uint num_entries_dev;
cl_uint *num_devices;

cl_int ret_CreateContext_SINGLE;
//use devices[0];
cl_context context_SINGLE;

cl_int ret_CreateContext_MULTIPLE;
//use devices;
cl_context context_MULTIPLE;







void sys_init();

void sys_destory();
