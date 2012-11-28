#include <stdio.h>

#include "headers.h"

int main(int argc, char ** argv)
{
  sys_init();
    
  user_context();
/*  
  if(context_mode == 1)SuperKernel_init(context_SINGLE);
  else if(context_mode == 2)SuperKernel_init(context_MULTIPLE);
  else{printf("Error in context_mode!");return 99;}
  
  */
  

  sys_destory();
  return 0;
}
