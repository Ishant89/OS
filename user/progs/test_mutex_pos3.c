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
	mutex_t mp,mp1;
	lprintf("Init for mp %p",&mp);
	 mutex_init(&mp);
	lprintf("Init for mp %p",&mp1);
	 mutex_init(&mp1);
	lprintf("lock for mp %p",&mp);
  mutex_lock(&mp);
	lprintf("lock for mp %p",&mp1);
  mutex_lock(&mp1);
	lprintf("unlock for mp %p",&mp);
  mutex_unlock(&mp);
	lprintf("unlock for mp %p",&mp1);
  mutex_unlock(&mp1);
	lprintf("destroy for mp %p",&mp);
  mutex_destroy(&mp);
  lprintf("Done!!!!\n");
	//while(1);
	return 0;
}
