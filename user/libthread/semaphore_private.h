/** @file semaphore_private.h
 *  
 *  @brief Semaphore library 
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

/** @brief Thread queue structure 
 *  
 *  This contains : 
 *  1. Thread id (list of thread ids in the waiting queue
 *  2. One in the top should always be running 
 *  3. Others should be waiting. 
 */

#include <malloc.h>
#include <syscall.h>
#include <sem_type.h>
#include <thread.h>
#include <contracts.h>
#include <mutex_type.h>
/*EDIT:To be removed*/
#include <simics.h>
#include <mutex.h>


#define SUCCESS 0
#define FAILURE -1
#define GET_SEMAPHORE_ID(sem) (unsigned int)(&(sem -> semaphore_id))


/** @brief queue for waiting threads 
 *  
 *  This contains : 
 *  1. thread id of the waiting thread. 
 *  2. pointer to the next thread in the queue.  
 *
 */
typedef struct sem_thread_queue 
{
  int thread_id;
  struct sem_thread_queue * next_thread_id;
} sem_thread_queue;


/** @brief Semaphore thread objects 
 *  
 *  This contains : 
 *  1. Semaphore id to identify semaphore object 
 *  2. Head to track the list of waiting 
 *  thread ID queues 
 *  3. Pointer to next semaphore oject 
 *  4. Lock for the semaphore object object
 *  5. Initialized count for the semaphore object
 *
 */


typedef struct semaphore_thread_object
{
  unsigned int semaphore_id;
  sem_thread_queue * head_queue;
  struct semaphore_thread_object * next_semaphore_object;
  int lock;
  int count;
  mutex_t mutex_lock;
} semaphore_thread_object;


/** @brief List of mutex objects */

semaphore_thread_object * head_semaphore_object = NULL;

int compAndXchg(void *,int,int);

