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

void add_semaphore_object_to_list(semaphore_thread_object *new_sem)
{
  semaphore_thread_object *oldHead = head_semaphore_object;
  head_semaphore_object = new_sem;
  new_sem -> next_semaphore_object = oldHead;
}

void remove_semaphore_object_from_list(semaphore_thread_object *new_sem)
{
  semaphore_thread_object *temp = head_semaphore_object;
  semaphore_thread_object *prev = NULL;

  while(temp != new_sem && temp)
  {
    prev = temp;
    temp = temp -> next_semaphore_object;
  }

  if(prev)
    prev -> next_semaphore_object = temp -> next_semaphore_object;
  else
    head_semaphore_object = temp -> next_semaphore_object;
}

semaphore_thread_object *get_semaphore_object_from_sem_id(unsigned int sem_id)
{
  semaphore_thread_object *temp = head_semaphore_object;

  while(temp != NULL)
  {
    if(temp -> semaphore_id == sem_id)
      return temp;

    temp = temp -> next_semaphore_object;
  }

  return NULL;
}


int sem_init(sem_t *sem,int count)
{
  SIPRINTF("Sem_init : Entering sem_init by tid %d",gettid());

  unsigned int sem_id = (unsigned int)GET_SEMAPHORE_ID(sem);

  semaphore_thread_object *new_sem = calloc(1,sizeof(semaphore_thread_object));

  if(new_sem == NULL)
    return FAILURE;

  new_sem -> semaphore_id = sem_id;
  new_sem -> count = count;
  SIPRINTF("Sem_init : Initializing mutex %p by tid %d",
                                          &(new_sem -> mutex_lock),gettid());
  mutex_init(&(new_sem -> mutex_lock));
  cond_init(&(new_sem -> cv));

  add_semaphore_object_to_list(new_sem);

  SIPRINTF("Sem_init : Leaving sem_init by tid %d",gettid());
  return SUCCESS;
}

void sem_wait(sem_t *sem)
{
  SIPRINTF("Sem_wait : Entering sem_wait by tid %d",gettid());
  semaphore_thread_object *sem_object;
  sem_object = get_semaphore_object_from_sem_id(GET_SEMAPHORE_ID(sem));

  SIPRINTF("Sem_wait : Sem_object %d by tid %d",
               sem_object -> semaphore_id,gettid());

  if(sem_object == NULL)
  {
    SIPRINTF("Sem_wait : Cannot find semaphore object");
    task_vanish(-2);
  }

  SIPRINTF("Sem_wait : Acquire lock %p by tid %d",
           &(sem_object -> mutex_lock),gettid());
  /* Acquire exclusive access */
  mutex_lock(&(sem_object -> mutex_lock));


  if(sem_object -> count <= 0)
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
  semaphore_thread_object *sem_object;
  sem_object = get_semaphore_object_from_sem_id(GET_SEMAPHORE_ID(sem));
  SIPRINTF("Sem_signal : Sem_object %d by tid %d",
           sem_object -> semaphore_id,gettid());

  if(sem_object == NULL)
  {
    SIPRINTF("Sem_signal : Cannot find semaphore object");
    task_vanish(-2);
  }

  SIPRINTF("Sem_signal : Acquire lock by tid %d",gettid());
  mutex_lock(&(sem_object -> mutex_lock));

  if(sem_object -> count <= 0)
  {
    /* Decrement semaphore count */
    (sem_object -> count)++;
    SIPRINTF("Sem_signa; : Incremented count : %d by tid %d",
             sem_object -> count,gettid());
    SIPRINTF("Sem_signal : Signalling and making runnable %d",gettid());
    cond_signal(&(sem_object -> cv));
  }
   SIPRINTF("Sem_signal : Release lock by tid %d",gettid());
   mutex_unlock(&(sem_object -> mutex_lock));
   SIPRINTF("Leaving sem_signal by tid %d",gettid());
}

void sem_destroy(sem_t *sem)
{
  SIPRINTF("Entering sem_destroy by tid %d",gettid());
  semaphore_thread_object *sem_object;
  sem_object = get_semaphore_object_from_sem_id(sem -> semaphore_id);

  /* Check ig semaphore object exists */
  if(sem_object == NULL)
  {
    SIPRINTF("Sem_destroy : Cannot find semaphore object");
    task_vanish(-2);
  }

  remove_semaphore_object_from_list(sem_object);
  mutex_destroy(&(sem_object -> mutex_lock));
  cond_destroy(&(sem_object -> cv));
  free(sem_object);
  SIPRINTF("Leaving sem_destroy by tid %d",gettid());
}


