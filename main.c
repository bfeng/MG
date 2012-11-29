#include <stdlib.h>
#include <stdio.h>

#include "headers.h"

int main(int argc, char ** argv)
{
  if (argc != 5)
  {
    printf("Input error\n");
    return -1;
  }

  int warps = atoi(argv[1]);
  int blocks = atoi(argv[2]);
  int num_job_per_warp = atoi(argv[3]);
  int sleep_time = atoi(argv[4]);

  sys_init();

  user_context();

  if(context_mode == 1)SuperKernel_init(warps, blocks, num_job_per_warp, sleep_time, context_SINGLE);
  //else if(context_mode == 2)SuperKernel_init(context_MULTIPLE);
  //else{printf("Error in context_mode!");return 99;}




  sys_destory();
  return 0;
}
