#include<rwlock.h>
#include<simics.h>
#include<thread.h>
#include<stdlib.h>
#include<syscall.h>
#define NUM_READER_THREADS 10
#define NUM_WRITER_THREADS 2




rwlock_t rw;

void * reader(void * arg)
{
//	sleep(1);
	rwlock_lock(&rw,RWLOCK_READ);
	lprintf("Reader read : %d",thr_getid());
	rwlock_unlock(&rw);
	return NULL;
}

void * writer(void * arg)
{
	rwlock_lock(&rw,RWLOCK_WRITE);
	lprintf("Writer wrote: %d",thr_getid());
	rwlock_unlock(&rw);
	return NULL;
}

int main () 
{
	lprintf("Starting the threads");
	int buf [NUM_READER_THREADS];
	int buf1 [NUM_WRITER_THREADS];
	int st1 = thr_init(4096);
	if ( st1 < 0)
	{
		lprintf(" thr init error ");
		return -1;
	}
	int status = rwlock_init(&rw);
	if ( status < 0)
	{
		lprintf(" Init error ");
		return -1;
	}
	
	
	int j ;
	for (j=0; j < (NUM_READER_THREADS + NUM_WRITER_THREADS); j++)
	{
		if(j == 1 || j == 2)
		{
			buf1[j-1] = thr_create(writer,(void*)j);
		}
		buf[j] = thr_create(reader,(void*) j);

	}
	for (j=0;j<NUM_READER_THREADS;j++)
	{
		thr_join(buf[j],NULL);

	}
	for (j=0;j<NUM_WRITER_THREADS;j++)
	{
		thr_join(buf1[j],NULL);

	}
	rwlock_destroy(&rw);
	return 0;
}

