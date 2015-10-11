/** @file mutex_main.c
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
 *  Note: This design will change for M:N implementation	
 *	@author Ishant Dawer(idawer) & Shelton (sdsouza)
 *	@bug No knwown bugs
 */

#include "mutex_private.h"

/** @brief append thread id to the queue
 *  
 *  This pushes the thread id to the end
 *  of the queue 
 *  This is used to maintain queues for the
 *  waiting threads blocked on lock 
 *  @param queue_head Top of the queue
 *
 *  @param queue_elem last_elem
 *  @return void 
 */

static void append_thread_id_to_queue(
		thread_queue ** queue_head, 
		thread_queue * last_elem
		)
{
	thread_queue * temp;
	if (*queue_head == NULL)
	{
		*queue_head = last_elem;
		return;
	}
	for (temp=*queue_head; (temp -> next_thread_id) != NULL;
			temp=temp->next_thread_id)
	{
		continue;
	}
	temp -> next_thread_id = last_elem;
	last_elem -> next_thread_id= NULL;
}


/** @brief remove thread id from the top of the queue
 *  
 *  This pushes the thread id from the top of the queue
 *  of the queue and returns the element if success
 *
 *  @param queue Queue
 *
 *  @return int Success 
 */

static thread_queue * remove_thread_id_from_queue(thread_queue ** queue_head)
{
	thread_queue * temp = NULL;
	if (*queue_head != NULL)
	{
		temp = *queue_head;
		*queue_head = temp->next_thread_id;
	}
	return temp;
}

/** @brief Thread init function 
 *  
 *  This API does the following : 
 *  1. Creates mutex object 
 *  2. Mutex object is mutex_t int
 *  3. head_queue is kept empty
 *  4. lock is available 
 *  5. Unlock_flag is used to give preference to thread in 
 *  mutex_unlock 
 *  @param mutex_t mutex object
 *  @return int Pass/Fail
 */

int mutex_init(mutex_t * mp)
{
	/* Assign a mutex object */
	mutex_t * mutex_obj = mp;
/* Queues of threads blocked on lock*/
	mutex_obj -> head_queue = NULL;
	/* Thread ID currently owning the lock*/
	mutex_obj -> lock_owner = -1;
	/* Thread queue lock */
	mutex_obj -> lock = 0;
	/* 
	 * Thread in mutex_unlock sets this so that
	 * they get the queue lock in bounded time
	 */
	mutex_obj -> unlock_flag = 0;
	return PASS;	
}

/** @brief Mutex lock API 
 *
 *  Progress : If lock is availble, thread takes the lock
 *  and enters critical section
 *  
 *  Mutual Exclusion: Lock helps in mutual exclusion 
 *  
 *  Bounded Waiting: Thread queue is FIFO and threads are 
 *  made runnable on that basis
 *
 *  This function does the following : 
 *  1. It first checks if unlock_flag is set or not
 *  2. If set, it yields to the lock_owner so that lock_owner
 *  gets the queue lock asap and makes other in the queue runnable
 *  3. If unset, it checks for the lock 
 *  4. If available, it takes it and sets the owner to itself
 *	5. Else, it tries to get the queue lock and inserts itelf into 
 *	the queue once the queue lock is attained 
 *	6. After entering into the lock, it gets descheduled
 *  
 *  Note: deschedule has a condition which will be explained below	
 *	@param mutex_t* mp 
 *
 *	@return void
 */

void mutex_lock(mutex_t * mp)
{
	/* Get the thread id of the thread */
	int thread_id = gettid();

	/* Get the mutex object */
	mutex_t * mutex_identifier = mp;

	if (mutex_identifier == NULL)
	{
		panic(" Not a valid pointer\n");
		task_vanish(KILL_STATUS);
	}

	/* Create a thread structure to be added to head_queue */
	thread_queue  new_thread_request; 
	
	new_thread_request.thread_id = thread_id;
	new_thread_request.next_thread_id = NULL;
	new_thread_request.reject = 0;
		
	/* If unlock flag is set, don't let other threads to contend 
	 * for queue lock */
	while(mutex_identifier->unlock_flag)
	{
		/* Yield to thread owning the lock */
		yield(mutex_identifier->lock_owner);
	}
	/* Take the lock */
	while (compAndXchg((void*)&mutex_identifier->lock,0,1))
	{
		/* Yield to thread owning the lock */
		yield(mutex_identifier->lock_owner);
	}	

	/* Set the first tid in the flag queue */
	if (mutex_identifier->lock_owner == -1)
	{
		/* Lock is available */
		mutex_identifier-> lock_owner = thread_id;
		/*Release the lock */
		mutex_identifier->lock = 0;
		return;
	} 
	
	/* Critical section */
	/* Append thread to the queue */
	append_thread_id_to_queue(&(mutex_identifier->head_queue),
		&new_thread_request);
	
	/*Release the lock */
	mutex_identifier->lock = 0;

	/*
	 * Every thread request has a reject flag which is unset 
	 * by default. It helps to prevent race between thread in mutex_lock
	 * about to deschedule and another thread in mutex_unlock about to make 
	 * current thread runnable. 
	 *
	 * Race scenario: Thread is not descheduled but 
	 * other thread is trying to make it runnable which will fail and
	 * current thread will deschedule. 
	 * This will lead to deadlock. E.g.:
	 * 1. Thread A is about to deschedule
	 * 2. Thread B made A runnable 
	 * 3. But failed as Thread A was not descheduled
	 * 4. Thread A gets descheduled 
	 * 5. No one to wake up thread A
	 * 
	 * Problem solved: If thread(thread B) in unlock sets the reject flag, 
	 * this will lead to current(thread A) not getting descheduled. 
	 * This can lead to another race where make_runnable by thread B 
	 * (next instruction after the reject flag is set,
	 * can awake  the same thread A when it is trying to take the lock for 
	 * different mutex but it is supposed to be descheduled 
	 * as per expectation. 
	 * This is prevented by checking that current thread 
	 * id should be the owner of the lock otherwise it gets stuck 
	 * in the following loop.
	 * 
	 * For any thread to exit the following loop:
	 * 1.set the reject flag, 
	 * 2.Make_runnable should succeed or fail
	 * (should not create spurious wake)
	 * 3.set the lock_owner to itself (i.e thread A)
	 * all above things should be done by thread B in mutex_unlock
	 *
	 * Checks : If reject flag is set, don't deschedule but move in loop if 
	 * lock owner is not the current thread.
	 */
	while(new_thread_request.reject != 1 || 
			mutex_identifier-> lock_owner != thread_id)
	{
		deschedule(&new_thread_request.reject);
	}
}


/** @brief Mutex unlock API 
 *  
 *  This API unlocks the thread 
 *  It does : 
 *  1. It registers the interest for queue lock so that it gets the lock 
 *  in bounded time 
 *  2. After getting the queue lock, it checks if queue is null
 *  3. If yes, it removes the element from head queue,sets the reject flag
 *  for that thread removed from the head queue and makes it runnable
 *  and sets the lock_owner to new thread (made runnable) 
 *  4. Else, releases the lock and setting the lock_owner to -1
 *	
 *	@param mutex_t mp
 *
 *
 */

void mutex_unlock(mutex_t *mp)
{
	unsigned int head_id;
	thread_queue * temp_queue;

	/* Get the mutex object */
	mutex_t * mutex_identifier = mp;

	if (mutex_identifier -> lock_owner == -1)
	{
		panic("Call mutex_init first");
		task_vanish(KILL_STATUS);
	}

	/* Registering interest for queue lock */	

	mutex_identifier -> unlock_flag++;
	/* Take the lock */
	while (compAndXchg((void *)&mutex_identifier->lock,0,1))
	{
		yield(mutex_identifier->lock_owner);
	}	

	/* Check if the mutex unlock is not invoked by the thread 
	 * id other than the one keeping the lock */

	if (mutex_identifier->head_queue != NULL)
	{
		head_id = mutex_identifier->head_queue->thread_id;

		temp_queue= remove_thread_id_from_queue(
			&(mutex_identifier->head_queue));
		
		/* transfer the lock ownership*/
		mutex_identifier-> lock_owner = head_id;
		/* Set the reject flag */
		temp_queue->reject = 1;
		/* Make runnable waiting thread*/
		make_runnable(head_id) ;
	} else 
	{
		/* Release the lock */
		mutex_identifier->lock_owner = -1;
	}
	/*Deregister interest*/
	mutex_identifier -> unlock_flag--;
	/*Release the lock */
	mutex_identifier->lock = 0;
}

/** @brief mutex destroy
 *  
 *  This clears all the stuff and data structures created by mutex_init
 *
 *  1. If threads are accessing it and lock is there, don't destroy 
 *  
 *  @param mp mutex_object
 */

void mutex_destroy(mutex_t *mp)
{

	/* Get the mutex object */
	mutex_t * mutex_identifier = mp ;

	if (mutex_identifier == NULL)
	{
		panic("Use mutex init first\n");
		return;
	}
    	/* Either lock is still taken and elements are still there */
	if (mutex_identifier-> lock_owner !=-1 ||
			mutex_identifier-> head_queue != NULL)
	{
		panic("Terminating : Either lock is nt \
				released or elements are there\n");
		/* Change status to macro */
		task_vanish(KILL_STATUS);
	}
}
