#include <syscall.h>
#include <simics.h>
#include <stdarg.h>
#include <assert.h>
#include<thread.h>
#include<stdlib.h>
void* worker(void * id)
{

	int * pc = 0;
	*pc = 10;
	thr_exit(NULL);
	return NULL;
}
int main()
{
	thr_init(4096);
	int pid = thr_create(worker,(void*)10);
	thr_join(pid,NULL);
	return 0;
}

