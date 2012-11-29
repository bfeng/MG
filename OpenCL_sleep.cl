//#pragma OPENCL EXTENSION cl_intel_printf : enable

__kernel void superKernel(__global int * seconds_input, __global char * debugger, __global int * numberjobs)
{
  //debugger only 1024 * sizeof(char)
  float AddPerUs = 14846398.5;
  float adds = (*seconds_input)*AddPerUs;
  
  int t = get_global_id(0);
  int loop = *numberjobs;
  int loop_temp;
  for(loop_temp = 0; loop_temp < loop; loop_temp++)
  {
  int temp=0;
  while(temp<adds)
  {
    temp++;
  }
  }
  char enable;
  enable = '1';
  char test;
  test = 'b';
  debugger[0] = enable;
  debugger[1] = test;
  //constant char *p = "this is a test string\n";
  //cl_intel_printf("%s\n", p);
  
}


