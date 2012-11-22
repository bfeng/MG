
#include "OpenCL_compiler.h"


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
  
  //#-4 Return call-able kernel entry
  openCL_kernel = clCreateKernel(openCL_program, "superKernel", &ERR);
  
  return;
            
}
