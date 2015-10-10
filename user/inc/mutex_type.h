/** @file mutex_type.h
 *  @brief This file defines the type for mutexes.
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H
/** @brief waiting thread queue struct */
typedef struct thread_queue 
{
	/* Waiting thread id */
	int thread_id;
	/* Reject flag (for deschedule cond) */
	int reject;
	/* Next entry*/
	struct thread_queue * next_thread_id;
} thread_queue;

/** @brief Mutex thread objects 
 *  
 *  This contains : 
 *  1. Mutex id to identify mutex object dd_to
 *  2. Head to track the list of waiting 
 *  thread ID queues 
 */

typedef struct mutex {
	/* Head of the waiting queue */
	thread_queue * head_queue;
	/* Queue lock */
	int lock;
	/* thread id of the lock owner*/
	int lock_owner;
	/* Unlock flag for thread in mutex_unlock*/
	int unlock_flag;
} mutex_t;

#endif /* _MUTEX_TYPE_H */
