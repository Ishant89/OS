#include<cond.h>
#include<simics.h>
#include<thread.h>
#include<stdlib.h>

int i = 0;

int status = 10;

int number = 8;
int buf[8];

cond_t cv;
mutex_t mp;
void * consumer (void * arg)
{
	mutex_lock(&mp);
	while ( i <= 0)
	{
		cond_wait(&cv,&mp);
	}
	lprintf("Consumer %d consumed object %d",thr_getid(),i);
	i--;
	mutex_unlock(&mp);
	thr_exit(&status);
	return NULL;
}

void * producer (void * arg)
{
	while (number)
	{
		mutex_lock(&mp);
		if (i < 2)
		{
			i++;
			number--;
			cond_signal(&cv);
			lprintf("Producer %d produced object %d",thr_getid(),i);
		}
		
		mutex_unlock(&mp);
	}
	thr_exit(&status);
	return NULL;
}

int main () 
{
	int status = cond_init(&cv);
	int number1 = number;
	if ( status < 0)
	{
		lprintf(" Init error ");
		return -1;
	}
	int st2 = mutex_init(&mp);
	if ( st2 < 0)
	{
		lprintf(" mutex Init error ");
		return -1;
	}
	int st1 = thr_init(4096);
	if ( st1 < 0)
	{
		lprintf(" thr init error ");
		return -1;
	}
	int j ;

	int prod_id = thr_create(producer,(void*)10);
	for (j=0; j < number1; j++)
	{
		buf[j] = thr_create(consumer,(void*) j);

	}
	for (j=0;j<number1;j++)
	{
		thr_join(buf[j],NULL);

	}
	thr_join(prod_id,NULL);

	mutex_destroy(&mp);
	cond_destroy(&cv);
	return 0;
}

