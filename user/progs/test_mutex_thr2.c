#include <simics.h>
//#include <thr_internals.h>
#include <thread.h>
#include <syscall.h>
#include <malloc.h>
#include<mutex.h>
#include<simics.h>

#define MAX_THREADS 50
mutex_t mp;
int  id=0; 

void * hello(void * tid)
{
  //lprintf("Hello from child : \n");
  //mutex_lock(&mp);

  id++;
  lprintf("Thread id from child %d : %d \n",thr_getid(),id);
 
  
  //mutex_unlock(&mp);
return tid;
}

int main()
{
	//int pid = 10;
/*  int pid;
	thr_init(PAGE_SIZE);	
	int i = 0 ;
	for (;i < 5;i++)
  {
		pid = thr_create(hello,(void*)i);
    thr_join(pid,NULL);
    lprintf("Joined thread with tid:%d\n",pid);
  }
*/
	misbehave(3);
	int thread_ids[MAX_THREADS];
	int i = 0;
	int init_status = thr_init(PAGE_SIZE);	
	if (init_status < 0)
	{
		lprintf(" Error in init status");
		return -1;
	}
	int mutex_init_status = mutex_init(&mp);
	if (mutex_init_status < 0)
	{
		lprintf(" Error in mutex_init status");
		return -1;
	}
	int thr_status,status1,join_status;
	for (;i<MAX_THREADS;i++)
	{
		thr_status = thr_create(hello,NULL);
		if (thr_status < 0)
		{
			lprintf(" Error in thread create status");
			return -1;
		}
		thread_ids[i]=thr_status;
	}
	//while(id< 50);
	//task_vanish(0);
	i=0;
	for (;i<MAX_THREADS;i++)
	{
	  join_status = thr_join(thread_ids[i],(void**)&status1);
		if (join_status < 0)
		{
			lprintf(" Error in thread join status");
			return -1;
		}
	}

	mutex_destroy(&mp);
	lprintf("Count is %d\n",id);
  lprintf("Done!!!!\n");
	//while(1);
	return 0;
}
