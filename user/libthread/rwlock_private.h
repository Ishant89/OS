/** @file rwlock_private.h
 *  
 *  @brief Rwlock library 
 *
 *  This file implements the mutex functions for the 
 *  thread library 
 *
 *  1. rwlock_init
 *  2. rwlock_destroy 
 *  3. rwlock_lock
 *  4. rwlock_unlock
 *  5. rwlock_downgrade
 *	
 *	@author Ishant Dawer(idawer) & Shelton D'souza(sdsouza)
 *	@bug No known bugs
 */



#include <malloc.h>
#include <syscall.h>
#include <rwlock_type.h>
#include <thread.h>
#include <contracts.h>
/*EDIT:To be removed*/
#include<simics.h>
#include<thr_internals.h>
#include<cond.h>
#include<mutex.h>

/** @brief Pass/Fail */
#define PASS 0
#define FAIL -1

#define READ 0
#define WRITE 1
/** @brief Get the mutex id from the structure */

#define GET_RWLOCK_ID(mp) ((unsigned int)(&(mp->rwlock_id)))

typedef struct thread_queue_rwlock 
{
	int thread_id;
	int type;
	struct thread_queue_rwlock * next_thread_id;
} thread_queue_rwlock;

/** @brief RWLOCK objects 
 *  
 *  This contains : 
 *  1. Mutex id to identify mutex object 
 *  2. Head to track the list of waiting 
 *  thread ID queues 
 *  3. Pointer to next element 
 *
 *  4. Lock for that mutex object
 */


typedef struct rwlock_thread_object
{
	unsigned int rwlock_id;
	mutex_t global_lock;
	cond_t read_cond;
	cond_t write_cond;
	unsigned int readcnt;
	unsigned int writecnt;
	int downgrade_id;
	thread_queue_rwlock * head_queue;
	struct rwlock_thread_object * next_rwlock_object;
} rwlock_thread_object;


/** @brief List of rwlock objects */

rwlock_thread_object * head_rwlock_object = NULL;

