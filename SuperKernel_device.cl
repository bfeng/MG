//#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable




typedef struct JobDescription{
  int JobID;
  int JobType;
  int numThreads;
  int params; //OpenCL can only accept fixed array, and running kernel can never know new buffer
} JobDescription;
//typedef JobDescription *JobPointer; 
typedef struct QueueRecord {
  JobDescription *Array; 
  int Capacity;                 
  int Rear;
  int Front;
  int ReadLock;
} QueueRecord;
//typedef QueueRecord * Queue;

//void releaseLock(QueueRecord * Q);
//void getLock(QueueRecord * Q);
//int d_IsFull(QueueRecord * Q);
//int d_IsEmpty(QueueRecord * Q);
//void EnqueueResult(JobDescription * JobDone, QueueRecord * Q, JobDescription *);
//void FrontAndDequeueJob(QueueRecord * Q, JobDescription * Job);

__kernel void superKernel(__global QueueRecord * incomming, 
                          __global JobDescription * incomming_array, 
                          __global QueueRecord * result, 
                          __global JobDescription * result_array, 
                          int numJobsPerWarp,
                          __global char * debug)
{
  __private int warp_size = 32;
  
  __local JobDescription currentJobs[32];
  
  __private int threadID = get_local_id(0) % warp_size;
  __private int warpID = get_local_id(0) / warp_size;
  

  __private int i;
  for (i = 0; i < numJobsPerWarp; i++)
  {
    if(threadID == 0) //FrontAndDequeueJob(incomming, &currentJobs[warpID]);
    {
      while(incomming->ReadLock != 0){}
      if(incomming->ReadLock== 0) incomming->ReadLock = 1;
      
      
      __private int count = 0;
      while( (incomming->Rear+1) % incomming->Capacity == incomming->Front )count++;

      currentJobs[warpID] = incomming_array[incomming->Front];

      incomming->Front = (incomming->Front+1)%(incomming->Capacity);

      incomming->ReadLock = 0;
    }
    
    if(threadID < (currentJobs[warpID].numThreads))
    {
      currentJobs[warpID].params = 1;
    
    }
    
    if(threadID == 0)
    {
      while(result->ReadLock != 0){}
      if(result->ReadLock== 0) result->ReadLock = 1;
      
      __private int count = 0;
      while( (result->Rear+1) % result->Capacity == result->Front )count++;
      
      __private int temp = (result->Rear + 1)%(result->Capacity);

      result_array[temp] = currentJobs[warpID];

      result->Rear = temp;
      
      result->ReadLock = 0;
    }
  }
  
}



