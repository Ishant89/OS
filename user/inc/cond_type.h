/** @file cond_type.h
 *  @brief This file defines the type for condition variables.
 */

#ifndef _COND_TYPE_H
#define _COND_TYPE_H

/** @brief Struct for waiting thread queue */
typedef struct wait_thread_queue
{
   	int thread_id;
	struct wait_thread_queue * next_wait_thread;
} wait_thread_queue;

/** @brief Stuct for conditional variable */
typedef struct cond {
	mutex_t cond_lock;/*Lock for the cond objects */
	wait_thread_queue * head_queue;/*Head of the wait queue */
	mutex_t * mutex_object; /* Associated mutex object */
} cond_t;

#endif /* _COND_TYPE_H */
