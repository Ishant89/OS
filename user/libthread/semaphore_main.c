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
 *  @author Shelton Dsozua(sdsouza) & Ishant Dawer(idawer)
 *  @bug No known bugs
 */

#include "semaphore_private.h"
/** @brief Initialize semaphore structure
 *  
 *  1. It defines the count and max_count 
 *  of the threads
 *  2. It inits the mutex and condvar
 *
 *  @param sem sem object
 *  @param count number of threads (in parallel)
 *  @return int PASS/FAIL
 */

int sem_init(sem_t *sem,int count)
{
  sem_t *new_sem = sem;

  if(new_sem == NULL)
    return FAILURE;
  new_sem -> count = count;
  new_sem -> max_count = count;
  if (mutex_init(&(new_sem -> mutex_lock)) < 0)
  {
	  return FAILURE;
  }
  if (cond_init(&(new_sem -> cv)) < 0)
  {
	  return FAILURE;
  }
  return SUCCESS;
}
/** @brief Sem wait lib call
 *  
 *  1. Checks for valid semaphore object
 *  2. takes the mutex lock 
 *  3. Checks if the count is <=0
 *  4. If yes, puts itself into conditional wait
 *  5. else decrements the count
 *
 *  @param sem semaphore object
 */

void sem_wait(sem_t *sem)
{
  sem_t *sem_object;
  sem_object = sem;

  if(sem_object == NULL)
  {
    panic("Sem_wait : Cannot find semaphore object");
    task_vanish(KILL_STATUS);
  }
  /* Acquire exclusive access */
  mutex_lock(&(sem_object -> mutex_lock));
  while(sem_object -> count <= 0)
  {
    cond_wait(&(sem_object -> cv),&(sem_object -> mutex_lock));
  }

  /* Decrement semaphore count */
  (sem_object -> count)--;
  mutex_unlock(&(sem_object -> mutex_lock));
}

/** @brief Sem signal lib call
 *  
 *  1. Checks for valid semaphore object
 *  2. takes the mutex lock 
 *  3. Checks if the count is <=0
 *  4. If yes,signals the elements into cond_wait queue
 *  5. and increments the count
 *  6. else increments the count if count is less than max_count
 *
 *  @param sem semaphore object
 */
void sem_signal(sem_t *sem)
{
  sem_t *sem_object;
  sem_object = sem;

  if(sem_object == NULL)
  {
    task_vanish(-2);
  }

  mutex_lock(&(sem_object -> mutex_lock));

  if(sem_object -> count <= 0)
  {
    /* Increment semaphore count */
    (sem_object -> count)++;
    cond_signal(&(sem_object -> cv));
  }

  else if(sem_object -> count < sem_object -> max_count)
    (sem_object -> count)++;
   mutex_unlock(&(sem_object -> mutex_lock));
}

/** @brief Destroy sem object 
 *  
 *  1. Checks if sem object is null
 *  2. Destroy mutex lock 
 *  3. Destroy condobject 
 *  
 *  @param sem semaphore object
 */
void sem_destroy(sem_t *sem)
{
  sem_t *sem_object;
  sem_object = sem;

  /* Check ig semaphore object exists */
  if(sem_object == NULL)
  {
    panic("Sem_destroy : Cannot find semaphore object");
    task_vanish(KILL_STATUS);
  }

  mutex_destroy(&(sem_object -> mutex_lock));
  cond_destroy(&(sem_object -> cv));
}


