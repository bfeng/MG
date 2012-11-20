#include "sys_info.h"



//Data types
//To keep consistency with CUDA code
struct JobDescription{
  int JobID;
  int JobType;
  int numThreads;
  void *params;
};
typedef struct JobDescription *JobPointer; 

struct QueueRecord {
  struct JobDescription *Array; 
  int Capacity;                 
  int Rear;
  int Front;
  int ReadLock;
};
typedef struct QueueRecord *Queue;

