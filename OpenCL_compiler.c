
#include "OpenCL_compiler.h"

void check_err_c(cl_int);
void program_build_info(cl_program, cl_device_id);
void openCL_compiler(char * filename, cl_context context, cl_device_id * devices)
{
  //#-1 Read file
  printf("Compiling kernel source: %s ...\n", filename);
  FILE *fp;
  char path[256] = "";
  char *source_str;
  size_t source_size;
  
  strcat(path, filename);
  fp = fopen(path, "r");
  if (!fp) 
  {
    printf("Failed to load file %s.\n", filename);
    return;
  }
  
  source_str = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);
  
  //#-2 Create with context
  cl_int ERR;
  openCL_program = clCreateProgramWithSource(context, 1, 
            (const char **)&source_str, (const size_t *)&source_size, &ERR);
  switch (ERR)
  {
    case CL_SUCCESS:
        break;
    case CL_INVALID_CONTEXT:
      printf("error during compiling: invalid context.\n");
      break;
    case CL_INVALID_VALUE:
      printf("error during compiling: invalid input value.\n");
      break;
    case CL_OUT_OF_RESOURCES:
      printf("error during compiling: resources error.\n");
      break;
    case CL_OUT_OF_HOST_MEMORY:
      printf("error during compiling: host memory out.\n");
      break;      
    default:
      printf("you cant be here.");           
  }
  
  
  
  //#-3 Compile and Link
  ERR = clBuildProgram(openCL_program, 1, devices, NULL, NULL, NULL);
  check_err_c(ERR);
  program_build_info(openCL_program, *devices);
  //#-4 Return call-able kernel entry
  openCL_kernel = clCreateKernel(openCL_program, "superKernel", &ERR);
  check_err_c(ERR);  
  if (ERR != CL_SUCCESS)
  {
  
  }
  
  
  return;
            
}

void check_err_c(cl_int err)
{
  if(err != CL_SUCCESS)
  {
    printf("#Error relating to compiling kernel!!\n");
    switch (err)
    {
      case CL_SUCCESS:
        break;
      case CL_INVALID_PROGRAM:
        printf("invalid program.\n");
        break;
      case CL_INVALID_PROGRAM_EXECUTABLE:
        printf("no successfully built executable for program.\n");
        break;
      case CL_INVALID_KERNEL_NAME:
        printf("invalid kernel name\n");
        break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("error during compiling: host memory out.\n");
        break;
      case CL_INVALID_KERNEL_DEFINITION:
        printf("invalid kernel defination.\n");
        break;
      case CL_INVALID_VALUE:
        printf("kernel name is NULL.\n");
        break;
      case CL_OUT_OF_RESOURCES:
        printf("out off resources.\n");
        break;
      case CL_INVALID_DEVICE:
        printf("invalid device.\n");
        break;
      case CL_COMPILER_NOT_AVAILABLE:
        printf("compiler not available.\n");
        break;
      case CL_BUILD_PROGRAM_FAILURE:
        printf("build program failure.\n");
        break;

        
      default:
        printf("you cant be here.");           
    }
  }
}

void program_build_info(cl_program program, cl_device_id device)
{
  printf("############################################################\n");
  printf("#               OpenCL Compiler Scripts:                   #\n");
  printf("############################################################\n");
  cl_int ERR;
  size_t * ret_size_t = (size_t *) malloc(sizeof(size_t));
  char * ret_char = (char *) malloc (sizeof(char) * 10000000);
  ERR = clGetProgramBuildInfo(openCL_program, device, CL_PROGRAM_BUILD_LOG, sizeof(char)*10000000, (void *) ret_char, NULL);
  if(ERR != CL_SUCCESS)
  {
    printf("getprograminfo ERR\n");
    switch(ERR)
    {
      case CL_INVALID_PROGRAM:
        printf("invalid program object\n");
        break;
      case CL_INVALID_PROGRAM_EXECUTABLE:
        printf("invalid program executable\n");
        break;   
      case CL_INVALID_VALUE:
        printf("invalid value\n");
        break;            
      case CL_OUT_OF_HOST_MEMORY:
        printf("out of host memory\n");
        break;     
      case CL_OUT_OF_RESOURCES:
        printf("failure to allocate resources\n");
        break;                 
      default:
        printf("Other problem\n");
      break;
    }
  }
  printf("%s\n", ret_char);
  
}
