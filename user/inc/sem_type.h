/** @file sem_type.h
 *  @brief This file defines the type for semaphores.
 */

#ifndef _SEM_TYPE_H
#define _SEM_TYPE_H

#include <cond.h>
#include <mutex.h>

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
typedef struct sem {
  cond_t cv;
  int count;
  unsigned int max_count;
  mutex_t mutex_lock;
} sem_t;

#endif /* _SEM_TYPE_H */
