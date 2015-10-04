/** @file mutex_private.h
 *  
 *  @brief Mutex library 
 *
 *  This file implements the mutex functions for the 
 *  thread library 
 *
 *  1. mutex_init 
 *  2. mutex_destroy
 *  3. mutex_lock
 *  4. mutex_unlock
 *	
 *	@author Ishant Dawer(idawer)
 *	@bug No known bugs
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
#include <mutex_type.h>
#include <thread.h>
#include <contracts.h>
/*EDIT:To be removed*/
#include<simics.h>
#include<thr_internals.h>

/** @brief Pass/Fail */
#define PASS 0
#define FAIL -1

/** @brief Get the mutex id from the structure */

#define GET_MUTEX_ID(mp) ((unsigned int)(&(mp->mutex_id)))

typedef struct thread_queue 
{
	int thread_id;
	struct thread_queue * next_thread_id;
} thread_queue;

/** @brief Mutex thread objects 
 *  
 *  This contains : 
 *  1. Mutex id to identify mutex object 
 *  2. Head to track the list of waiting 
 *  thread ID queues 
 *  3. Pointer to next element 
 *
 *  4. Lock for that mutex object
 */


typedef struct mutex_thread_object
{
	unsigned int mutex_id;
	thread_queue * head_queue;
	struct mutex_thread_object * next_mutex_object;
	int lock;
} mutex_thread_object;


/** @brief List of mutex objects */

mutex_thread_object * head_mutex_object = NULL;

int compAndXchg(void *,int,int);

<<<<<<< HEAD
/* Mutex API */
int mutex_init(mutex_t * mp);
void mutex_lock(mutex_t * mp);
void mutex_unlock(mutex_t *mp);



=======
/* Functions */

int mutex_init( mutex_t *mp );
void mutex_destroy( mutex_t *mp );
void mutex_lock( mutex_t *mp );
void mutex_unlock( mutex_t *mp );
>>>>>>> bb0f14c617eb03c27dc0c188bac41adf457a5ffe
