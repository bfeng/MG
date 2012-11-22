//#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable

typedef struct JobDescription{
  int JobID;
  int JobType;
  int numThreads;
  int params; //OpenCL can only accept fixed array, and running kernel can never know new buffer
} JobDescription;
typedef struct JobDescription *JobPointer; 
typedef struct QueueRecord {
  JobDescription *Array; 
  int Capacity;                 
  int Rear;
  int Front;
  int ReadLock;
} QueueRecord;
typedef struct QueueRecord *Queue;


__kernel void superKernel(__global struct QueueRecord * incomming, 
                          __global struct JobDescription * incomming_array, 
                          __global struct QueueRecord * result, 
                          __global struct JobDescription * result_array, 
                          const numJobsPerWarp)
{
  int warp_size = 32;
  
  __local JobDescription currentJobs[32];
  
  int threadID = get_local_id(0) % warp_size;
  int warpID = get_local_id(0) / warp_size;
  
  printf("I got threadID = %d, warpID = %d\n", threadID, warpID);
  int i;
  for (i = 0; i < numJobsPerWarp; atom_inc(i))
  {
    if(threadID = 0) printf("I am manager in this warp");
    
    if(threadID < (currentJobs[warpID].numThreads)) printf("execute job");
    
    if(threadID = 0) printf("I also enqueue result\n");
  }
  
}







//////////////////////////////////
////  SuperKernel helpers
//////////////////////////////////
int d_IsEmpty(volatile Queue Q) {
  volatile int *rear = &(Q->Rear);
  return (*rear+1)%Q->Capacity == Q->Front;
}

int d_IsFull(volatile Queue Q) {
  volatile int *front = &(Q->Front);
  return (Q->Rear+2)%Q->Capacity == *front;
}
void getLock(volatile Queue Q)
{
  while(atom_cmpxchg(&(Q->ReadLock), 0, 1) != 0);
}

void releaseLock(volatile Queue Q)
{
  atom_xchg(&(Q->ReadLock),0);
}
