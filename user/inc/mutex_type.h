/** @file mutex_type.h
 *  @brief This file defines the type for mutexes.
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H

typedef struct thread_queue 
{
	int thread_id;
	int reject;
	struct thread_queue * next_thread_id;
} thread_queue;

/** @brief Mutex thread objects 
 *  
 *  This contains : 
 *  1. Mutex id to identify mutex object dd_to
 *  2. Head to track the list of waiting 
 *  thread ID queues 
 *  3. Pointer to next element 
 *
 *  4. Lock for that mutex object
 */

typedef struct mutex {
	unsigned int mutex_id;
	thread_queue * head_queue;
	struct mutex * next_mutex_object;
	int lock;
	int lock_owner;
	int unlock_flag;
} mutex_t;

#endif /* _MUTEX_TYPE_H */
