#include <simics.h>
//#include <thr_internals.h>
#include <thread.h>
#include <syscall.h>
#include <malloc.h>
#include<mutex.h>
#include<simics.h>

mutex_t mp;
int  id; 

void * hello(void * tid)
{
  lprintf("Hello from child : \n");
  mutex_lock(&mp);

  id = thr_getid();
  lprintf("Thread id from child is %d\n",id);
  
  
  mutex_unlock(&mp);
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
	int thr_status = thr_create(hello,NULL);
	if (thr_status < 0)
	{
		lprintf(" Error in thread create status");
		return -1;
	}
  sleep(10);
  mutex_lock(&mp);

  id = gettid();
  lprintf("Thread id from parent is %d\n",id);
  
  
  mutex_unlock(&mp);

  int status1;
  int join_status = thr_join(thr_status,(void**)&status1);
	if (join_status < 0)
	{
		lprintf(" Error in thread join status");
		return -1;
	}
  
	mutex_destroy(&mp);
  lprintf("Done!!!!\n");
	while(1);
	return 0;
}
