/*
 * these functions should be thread safe.
 * It is up to you to rewrite them
 * to make them thread safe.
 *
 */

#include <stdlib.h>
#include <types.h>
#include <stddef.h>
#include <compAndXchg.h>

/*EDIT */
#define DEBUG 0
#include<simics.h>
#include<syscall.h>
#include<thr_internals.h>

/* Lock for malloc */
int alloc_lock = 0 ;

void *malloc(size_t __size)
{
	/* Check and take the lock */
  while (compAndXchg((void *)&alloc_lock,0,1))
  {
	  continue;
  }
  SIPRINTF("Entering malloc by tid %d request size %d",gettid(),__size);
  void * ret_val = _malloc(__size);
  /* Release the locj */
  alloc_lock = 0 ;
  SIPRINTF("Exiting malloc by tid %d and mem  %p",gettid(),ret_val);
  return ret_val;
}

void *calloc(size_t __nelt, size_t __eltsize)
{
  while (compAndXchg((void *)&alloc_lock,0,1))
  {
	  continue;
  }
  SIPRINTF("Entering calloc by tid %d request size %d",gettid(),__nelt);
  void * ret_val = _calloc(__nelt, __eltsize);
  /* Release the locj */
  alloc_lock = 0 ;
  SIPRINTF("Exiting calloc by tid %d and mem  %p",gettid(),ret_val);
  return ret_val;
}

void *realloc(void *__buf, size_t __new_size)
{
  while (compAndXchg((void *)&alloc_lock,0,1))
  {
	  continue;
  }
  SIPRINTF("Entering realloc by tid %d request size %d",gettid(),__new_size);
  void * ret_val = _realloc(__buf, __new_size);
  /* Release the locj */
  alloc_lock = 0 ;
  SIPRINTF("Exiting realloc by tid %d and mem  %p",gettid(),ret_val);
  return ret_val;
}

void free(void *__buf)
{
  while (compAndXchg((void *)&alloc_lock,0,1))
  {
	  continue;
  }
  SIPRINTF("Entering free by tid %d mem %p",gettid(),__buf);
  _free(__buf); 
  /* Release the locj */
  alloc_lock = 0 ;
  SIPRINTF("Exiting free by tid %d mem %p",gettid(),__buf);
	return;
}
