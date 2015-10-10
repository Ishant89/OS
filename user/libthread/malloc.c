/*
 * these functions should be thread safe.
 * It is up to you to rewrite them
 * to make them thread safe.
 *
 */

#include <stdlib.h>
#include <types.h>
#include <stddef.h>
#include <mutex.h>
#include <syscall.h>
#include <thr_internals.h>



void *malloc(size_t __size)
{
	/* Check and take the lock */
	mutex_lock(&alloc_lock);
	void * ret_val = _malloc(__size);
	/* Release the locj */
	mutex_unlock(&alloc_lock);
	return ret_val;
}

void *calloc(size_t __nelt, size_t __eltsize)
{
  	mutex_lock(&alloc_lock);
	void * ret_val = _calloc(__nelt, __eltsize);
	mutex_unlock(&alloc_lock);
	return ret_val;
}

void *realloc(void *__buf, size_t __new_size)
{
  	mutex_lock(&alloc_lock);
	void * ret_val = _realloc(__buf, __new_size);
	mutex_unlock(&alloc_lock);
	return ret_val;
}

void free(void *__buf)
{
  	mutex_lock(&alloc_lock);
	_free(__buf); 
	mutex_unlock(&alloc_lock);
	return;
}
