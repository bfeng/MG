#include <stdio.h>

#define MAX_RESOURCES 7

typedef struct {
  int id;
  int resources;
  int start;
  int priority;
} Job;

void scheduling(Job *jobs, int size)
{
  int available = 0;
  int i = 0;
  for(;i<size;i++)
  {
    jobs[i].start = available;
    available += jobs[i].resources;
  }
}

void printf_jobs(Job *jobs, int size)
{
  int i = 0;
  for(;i<size;i++)
  {
    printf("Job %d at %d -> %d\n", jobs[i].id, jobs[i].start, jobs[i].start+jobs[i].resources);
  }
}

int main()
{
  Job job1 = {1, 2, 0, 1};
  Job job2 = {2, 1, 0, 0};
  Job job3 = {3, 1, 0, 3};
  Job job4 = {4, 3, 0, 4};
  Job jobs[] = {job1, job2, job3, job4};
  printf("Before:\n");
  printf_jobs(jobs, 4);
  scheduling(jobs, 4);
  printf("After:\n");
  printf_jobs(jobs, 4);
}
