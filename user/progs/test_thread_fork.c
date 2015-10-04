#include <simics.h>
//#include <thr_internals.h>
#include <thread.h>
#include <syscall.h>
#include <malloc.h>
int main()
{
	lprintf("Parent TID is%d \n",gettid());
	/* Create a page for chil stack frame */
	//void * base = (void*)0xffffc000;
	int reject = 0;
	//int c = new_pages(base,PAGE_SIZE);
	//c=c;
	void * child_stack_high = (void *)(_malloc(PAGE_SIZE)) ;	

	int pid = thread_fork(child_stack_high);
	//  MAGIC_BREAK;

  if(pid)
  {
    lprintf("Hello from parent\n");
	//base = (void*)0xffffa000;
	//c = new_pages(base,PAGE_SIZE);
	child_stack_high = (void *)(_malloc(PAGE_SIZE));

	int pid1 = thread_fork(child_stack_high);
	//  MAGIC_BREAK;
	lprintf("pid of child2 is %d\n",pid1);
	if (!pid)
		lprintf("Hello from child 2");
	int i = 0;
	for (;i< 5000;i++);
	int z = make_runnable(5);
	lprintf("Made runnable %d\n",z);
  }

  else
  {
    lprintf("Hello from child %d\n",pid);
	int k = deschedule(&reject);
	lprintf("Deschedule is %d",k);
  }

	vanish();
  return 0;

}
