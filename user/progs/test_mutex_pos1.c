#include <simics.h>
//#include <thr_internals.h>
#include <thread.h>
#include <syscall.h>
#include <malloc.h>
#include<mutex.h>
#include<simics.h>

void * hello(void * tid)
{
	lprintf("Hello from child : %d\n",(int)tid);
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
	mutex_t mp;
	int status = mutex_init(&mp);
  lprintf("status is %d\n",status);
  mutex_lock(&mp);
  lprintf("Done!!!!\n");
	//while(1);
	return 0;
}
