/** @file semaphore_main.c
 *  
 *  @brief Semaphore library 
 *
 *  This file implements the semaphore functions for the 
 *  thread library 
 *
 *  This file implements the semaphore functions for the 
 *  thread library 
 *
 *  1. sem_init 
 *  2. sem_destroy
 *  3. sem_wait
 *  4. sem_signal
 *  
 *  @author Shelton Dsozua(sdsouza)
 *  @bug No known bugs
 */

/*EDIT: MAKE MALLOC SAFE*/
#define DEBUG 0
#include "semaphore_private.h"


int sem_init(sem_t *sem,int count)
{
  SIPRINTF("Sem_init : Entering sem_init by tid %d",gettid());


  sem_t *new_sem = sem;

  if(new_sem == NULL)
    return FAILURE;

  new_sem -> count = count;
  new_sem -> max_count = count;
  SIPRINTF("Sem_init : Initializing mutex %p by tid %d",
                                          &(new_sem -> mutex_lock),gettid());
  if (mutex_init(&(new_sem -> mutex_lock)) < 0)
  {
	  return FAILURE;
  }
  if (cond_init(&(new_sem -> cv)) < 0)
  {
	  return FAILURE;
  }

  SIPRINTF("Sem_init : Leaving sem_init by tid %d",gettid());
  return SUCCESS;
}

void sem_wait(sem_t *sem)
{
  SIPRINTF("Sem_wait : Entering sem_wait by tid %d",gettid());
  sem_t *sem_object;
  sem_object = sem;


  if(sem_object == NULL)
  {
    SIPRINTF("Sem_wait : Cannot find semaphore object");
    task_vanish(-2);
  }

  SIPRINTF("Sem_wait : Acquire lock %p by tid %d",
           &(sem_object -> mutex_lock),gettid());
  /* Acquire exclusive access */
  mutex_lock(&(sem_object -> mutex_lock));


  while(sem_object -> count <= 0)
  {
    
    SIPRINTF("Sem_wait : Going into cond_wait %p by tid %d",
              &(sem_object -> mutex_lock),gettid());

    cond_wait(&(sem_object -> cv),&(sem_object -> mutex_lock));

    SIPRINTF("Sem_wait : Made runnable after cond wait %d",gettid());
  }

  /* Decrement semaphore count */
  (sem_object -> count)--;
  SIPRINTF("Sem_wait : Decremented count : %d by tid %d",
             sem_object -> count,gettid());

  mutex_unlock(&(sem_object -> mutex_lock));

  SIPRINTF("Leaving sem_wait by tid %d",gettid());
}

void sem_signal(sem_t *sem)
{
  SIPRINTF("Sem_signal : Entering sem_signal by tid %d",gettid());
  sem_t *sem_object;
  sem_object = sem;

  if(sem_object == NULL)
  {
    SIPRINTF("Sem_signal : Cannot find semaphore object");
    task_vanish(-2);
  }

  SIPRINTF("Sem_signal : Acquire lock by tid %d",gettid());
  mutex_lock(&(sem_object -> mutex_lock));

  if(sem_object -> count <= 0)
  {
    /* Increment semaphore count */
    (sem_object -> count)++;
    SIPRINTF("Sem_signa; : Incremented count : %d by tid %d",
             sem_object -> count,gettid());
    SIPRINTF("Sem_signal : Signalling and making runnable %d",gettid());
    cond_signal(&(sem_object -> cv));
  }

  else if(sem_object -> count < sem_object -> max_count)
    /* Increment semaphore count */
    (sem_object -> count)++;

   SIPRINTF("Sem_signal : Release lock by tid %d",gettid());
   mutex_unlock(&(sem_object -> mutex_lock));
   SIPRINTF("Leaving sem_signal by tid %d",gettid());
}

void sem_destroy(sem_t *sem)
{
  SIPRINTF("Entering sem_destroy by tid %d",gettid());
  sem_t *sem_object;
  sem_object = sem;

  /* Check ig semaphore object exists */
  if(sem_object == NULL)
  {
    SIPRINTF("Sem_destroy : Cannot find semaphore object");
    task_vanish(-2);
  }

  mutex_destroy(&(sem_object -> mutex_lock));
  cond_destroy(&(sem_object -> cv));
  SIPRINTF("Leaving sem_destroy by tid %d",gettid());
}


