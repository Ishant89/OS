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
//#define DEBUG 0
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

void add_thread_to_semaphore_queue(semaphore_thread_object *sem_object,int thread_id)
{
  /* Get head of the waiting queue */
  sem_thread_queue *temp = sem_object -> head_queue;

  /* Append to end of the queue */
  while(temp != NULL)
  {
    temp = temp -> next_thread_id;
  }

  temp = calloc(1,sizeof(sem_thread_queue));
  temp -> thread_id = thread_id;
  temp -> next_thread_id = NULL;


}

sem_thread_queue *remove_thread_from_start_queue(semaphore_thread_object *sem_object)
{
  sem_thread_queue *temp = sem_object -> head_queue;

  if(temp != NULL)
  {
    sem_object -> head_queue = temp -> next_thread_id;
    return temp;
  }

  return NULL;
}

int check_thread_in_queue(semaphore_thread_object *sem_object,int thread_id)
{
  sem_thread_queue *temp = sem_object -> head_queue;

  while(temp != NULL)
  {
    if(temp -> thread_id == thread_id)
      return 1;

    temp = temp -> next_thread_id;
  }

  return 0;

}

void print_queue(sem_thread_queue *head)
{
  sem_thread_queue *temp = head;

  while(temp != NULL)
  {
    SIPRINTF("Thread id : %d Next Thread : %p",temp -> thread_id,temp -> next_thread_id);
    temp = temp -> next_thread_id;
  }
}

void print_semaphore_object_list()
{
  semaphore_thread_object *temp = head_semaphore_object;

  while(temp != NULL)
  {
    SIPRINTF("-------Semaphore Object %d -------",temp -> semaphore_id);
    print_queue(temp -> head_queue);
    SIPRINTF("Count : %d",temp -> count);
    temp = temp -> next_semaphore_object;
  }
}

int sem_init(sem_t *sem,int count)
{
  SIPRINTF("Sem_init : Entering sem_init by tid %d",gettid());

  unsigned int sem_id = (unsigned int)GET_SEMAPHORE_ID(sem);

  semaphore_thread_object *new_sem = calloc(1,sizeof(semaphore_thread_object));

  if(new_sem == NULL)
    return FAILURE;

  new_sem -> semaphore_id = sem_id;
  new_sem -> head_queue = NULL;
  new_sem -> lock = 0;
  new_sem -> count = count;
  SIPRINTF("Sem_init : Initializing mutex %p by tid %d",
                                          &(new_sem -> mutex_lock),gettid());
  mutex_init(&(new_sem -> mutex_lock));

  add_semaphore_object_to_list(new_sem);
  print_semaphore_object_list();
  SIPRINTF("Sem_init : Leaving sem_init by tid %d",gettid());
  return SUCCESS;
}

void sem_wait(sem_t *sem)
{
  SIPRINTF("Sem_wait : Entering sem_wait by tid %d",gettid());
  /*Deschedule condition */
  int reject = 0;
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

  /* Decrement semaphore count */
  (sem_object -> count)--;
  SIPRINTF("Sem_wait : Decremented count : %d by tid %d",
             sem_object -> count,gettid());

  if(sem_object -> count < 0)
  {
    int thread_id = gettid();
    SIPRINTF("Sem_wait : Adding to queue and deschedule thread %d by tid %d",
             thread_id,gettid());
    if(!check_thread_in_queue(sem_object,thread_id))
    {
      SIPRINTF("Sem_wait : Thread already in queue");
      return ;
    }

    add_thread_to_semaphore_queue(sem_object,thread_id);
    print_semaphore_object_list();
    SIPRINTF("Sem_wait : Release lock with wait %p by tid %d",
              &(sem_object -> mutex_lock),gettid());
    mutex_unlock(&(sem_object -> mutex_lock));

    /*Deschedule*/
    if(deschedule(&reject) < 0)
    {
      SIPRINTF("Sem_wait : Count not deschedule thread");
      return;
    }

    SIPRINTF("Sem_wait : Made runnable after deschedule %d",gettid());
  }

  else
  {
    mutex_unlock(&(sem_object -> mutex_lock));
  }

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

  /* Decrement semaphore count */
  (sem_object -> count)++;
  SIPRINTF("Sem_signa; : Incremented count : %d by tid %d",
            sem_object -> count,gettid());

  if(sem_object -> count <= 0)
  {
    SIPRINTF("Sem_signal : Removing from queue and making runnbale %d",gettid());
    sem_thread_queue *thread = remove_thread_from_start_queue(sem_object);
    int thread_id = thread -> thread_id;
    SIPRINTF("Sem_signal : Removed thread %d",thread_id);
    free(thread);
    print_semaphore_object_list();
    if(thread != NULL)
    {
      SIPRINTF("Sem_signal : Making runnable thread %d",thread_id);
      while(make_runnable(thread_id) < 0)
      {
        continue;
      }

    }
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

  if(sem_object == NULL)
  {
    SIPRINTF("Sem_destroy : Cannot find semaphore object");
    task_vanish(-2);
  }

  remove_semaphore_object_from_list(sem_object);
  free(sem_object);
  print_semaphore_object_list();
  SIPRINTF("Leaving sem_destroy by tid %d",gettid());
}


