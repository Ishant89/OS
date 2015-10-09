/** @file cond_type.h
 *  @brief This file defines the type for condition variables.
 */

#ifndef _COND_TYPE_H
#define _COND_TYPE_H

/** @brief Struct for waiting thread queue */
/*typedef struct wait_thread_queue
{
   	int thread_id;
	struct wait_thread_queue * next_wait_thread;
} wait_thread_queue;
*/
typedef struct cond {
  /* fill this in */
	unsigned int cond_id;
} cond_t;

#endif /* _COND_TYPE_H */
