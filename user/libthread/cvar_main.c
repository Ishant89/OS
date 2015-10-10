/** @file cvar_main.c
 *  
 *  @brief Conditional variables implementation 
 *   
 *  This file implements conditional variables and defines 
 *  following functions : 
 *  1. cond_init()
 *  2. cond_destroy()
 *  3. cond_wait()
 *  4. cond_signal()
 *  5. cond_broadcast()
 *
 *  @author Ishant Dawer(idawer) & Shelton D'Souza (sdsouza)
 *
 *  @bug No known bugs 
 */

#include "cvar_private.h"


/** @brief Check if thread is already in queue
 *  
 *  This iterates over the queue and check if thread 
 *  already exists in the queue 
 *
 *  @param head_queue address of head queue 
 *  @param thread_id Id of the thread
 *
 *  @return Pass/Fail 
 */

static int check_if_thread_in_wait_queue(
		wait_thread_queue ** head_queue,
		unsigned int thread_id
		)
{
	wait_thread_queue * temp;
	for (temp=*head_queue;temp != NULL;temp=temp->next_wait_thread)
	{
		if (temp->thread_id == thread_id)
		{
			return PASS;
		}
	}
	return FAIL;

}
/** @brief append thread id to the queue
 *  
 *  This pushes the thread id to the end
 *  of the queue 
 *
 *  @param queue_head address of head of the queue
 *
 *  @param last_elem last queue element
 *  @return void 
 */

static void append_thread_id_to_wait_queue(
		wait_thread_queue ** queue_head, 
		wait_thread_queue * last_elem
		)
{
	wait_thread_queue * temp;
	if (*queue_head == NULL)
	{
		*queue_head = last_elem;
		return;
	}
	for (temp=*queue_head; (temp -> next_wait_thread) != NULL;
			temp=temp->next_wait_thread)
	{
		continue;
	}
	temp -> next_wait_thread = last_elem;
	last_elem -> next_wait_thread = NULL;
}

/** @brief remove thread id from the top of the queue
 *  
 *  This pushes the thread id from the top of the queue
 *  of the queue and returns the element if success
 *
 *  @param queue_head address of the head of the queue
 *
 *  @return int Success 
 */

static wait_thread_queue * remove_thread_id_from_wait_queue(
		wait_thread_queue ** queue_head)
{
	wait_thread_queue * temp = NULL;
	if (*queue_head != NULL)
	{
		temp = *queue_head;
		*queue_head = temp->next_wait_thread;
	}
	return temp;
}

/** @brief Cond Init 
 *  
 *  This implements following :
 *  1. Sets the waiting thread queue to NULL
 *  2. Sets the associated mutex object to NULL
 *  3. Initilaize the internal mutex lock used as a queue lock
 *
 *  @param cv Condvar ID 
 *  @return PASS/FAIL
 */

int cond_init(cond_t * cv)
{
	/* Creating the cond object */
	cond_t * cond_obj = cv;

	if (cond_obj == NULL)
	{
		panic("Unable to allocate cond_object");
		return FAIL;
	}
	cond_obj -> head_queue = NULL;
	cond_obj-> mutex_object = NULL;
	if (mutex_init(&(cond_obj->cond_lock)) < 0)
	{
		panic("Unable to allocate lock");
		return FAIL;
	}
	return PASS;
}


/** @brief Cond wait 
 *  
 *  This function does the  following : 
 *  1. It takes the queue lock 
 *  2. It checks if the condvar is associated with mutex object (mp)
 *  3. If yes, it checks if the mapping is correct
 *  4. If no, sets the associated mutex object
 *  5. Checks if thread is already in wait queue
 *  6. Appends to the queue if not there already
 *  7. Release the global lock and queue lock
 *  8. Deschedule itself 
 */

void cond_wait(cond_t * cv, mutex_t*mp)
{
	/* Reject condition */
	int reject = 0;

	/* Get the thread id  */
	int thread_id = gettid();

	/* Get the condvar object */
	cond_t * cond_obj = cv;
	if (cond_obj == NULL)
	{
		panic("Cond var object not initialized");
		task_vanish(KILL_STATUS);
	}

	/*Take the queue lock*/
	mutex_lock(&(cond_obj->cond_lock));	

	if (cond_obj -> mutex_object == NULL)
	{
		/* binding the global mutex to condvar */
		cond_obj -> mutex_object = mp;
	} else if (cond_obj -> mutex_object != mp)
	{
		panic("Illegal mapping of mp with cv");
		/*Release the queue lock*/
		mutex_unlock(&(cond_obj -> cond_lock));
		return;
	}

	/* Check if thread is already waiting */
	if (!check_if_thread_in_wait_queue(&cond_obj->head_queue,thread_id))
	{
		panic("Thread already waiting");
		task_vanish(KILL_STATUS);
	}

	/* Create the thread id structure */
	wait_thread_queue  new_thread_request; 
		
	new_thread_request.thread_id = thread_id;
	new_thread_request.next_wait_thread = NULL;

	/*Append to the queue */

	append_thread_id_to_wait_queue(&(cond_obj->head_queue),
			&new_thread_request);

	/*Release the lock */
	mutex_unlock(&(cond_obj->cond_lock));

	/* unlocking the global mutex */
	mutex_unlock(mp);

	/*Deschedule */
	if(deschedule(&reject) < 0)
	{
		return;
	}
	/* Take the global lock */
	mutex_lock(mp);
}


/** @brief Cond signal 
 *  
 *  This function is used to signal the thread in 
 *  wait state 
 *  It does following : 
 *  1. It takes the internal lock 
 *  2. It removes the thread ID from the front 
 *  3. If the queue of the threads is null after popping,
 *  it removes the associated mutex from the sturct 
 *  4. It makes the thread id runnable popped from 
 *  the queue 
 *  5. It claims the resources requested by threadID
 *
 *  @param cv 
 *  @return void
 */

void cond_signal(cond_t * cv )
{
	/* Get the condvar object */
	cond_t * cond_obj = cv;

	if (cond_obj == NULL)
	{
		panic("Cond var object not initialized");
		task_vanish(KILL_STATUS);
	}

	/* Take the lock */
	
	mutex_lock(&(cond_obj->cond_lock));

	if (cond_obj -> head_queue == NULL)
	{
		cond_obj->mutex_object = NULL;
		mutex_unlock(&(cond_obj->cond_lock));
		return;
	}

	/*Pop the first element from the queue*/

	wait_thread_queue * pop_elem = remove_thread_id_from_wait_queue(
			&(cond_obj->head_queue));

	/* If return elem is null, list is empty & reset the associated  
	 * mutex */
	if (pop_elem -> next_wait_thread  == NULL)
	{
		cond_obj->mutex_object = NULL;
	}
	
	/* Make runnable */
	while (make_runnable(pop_elem->thread_id) < 0)
	{
		continue;
	}
	
	/*Release the lock */
	mutex_unlock(&(cond_obj->cond_lock));
}

/** @brief Cond broadcast 
 *  
 *  This function is used to broadcast to all the threads in 
 *  wait state 
 *  It does following : 
 *  1. It takes the internal lock 
 *  2. It removes the thread ID from the front 
 *  3. If the queue of the threads is null after popping,
 *  it removes the associated mutex from the sturct 
 *  4. It makes the thread id runnable popped from 
 *  the queue 
 *  5. It claims the resources requested by threadID
 *
 *  @param cv 
 *  @return void
 */

void cond_broadcast(cond_t * cv )
{
	/* Get the condvar object */
	cond_t * cond_obj = cv;

	if (cond_obj == NULL)
	{
		panic("Cond var object not initialized");
		task_vanish(KILL_STATUS);
	}
	

	/* Take the lock */
	mutex_lock(&(cond_obj->cond_lock));

	if (cond_obj -> head_queue == NULL)
	{
		cond_obj-> mutex_object = NULL;
		mutex_unlock(&(cond_obj->cond_lock));
		return;
	}

	/*Pop the first element from the queue*/

	wait_thread_queue * pop_elem;
   	while((pop_elem 
				= remove_thread_id_from_wait_queue(&(cond_obj->head_queue)))
			!=NULL)
	{
		/* Make runnable */
		while (make_runnable(pop_elem->thread_id) < 0)
		{
			continue;
		}
	}
	/* Resetting the mutex_object */
	cond_obj-> mutex_object = NULL;
	/*Release the lock */
	mutex_unlock(&(cond_obj->cond_lock));
}


/** @brief condvar destroy
 *  
 *  This clears all the stuff and data structures created by cond_init
 *
 *  1. Clear the condvar object 
 *  2. If threads are accessing it and lock is there, don't destroy 
 */

void cond_destroy(cond_t * cv)
{
	/* Get the condvar object */
	cond_t * cond_obj = cv;

	if (cond_obj == NULL)
	{
		panic("Cond var object not initialized");
		task_vanish(KILL_STATUS);
	}
	/*Take the lock */
	mutex_lock(&(cond_obj->cond_lock));

	/* Either lock is still taken and elements are still there */
	if (cond_obj->head_queue != NULL)
	{
		panic("Terminating : Either lock is nt released or elements are there\n");
		/* Change status to macro */
		task_vanish(KILL_STATUS);
	}
	mutex_unlock(&(cond_obj->cond_lock));
	/* Destroy the gloabl mutex */
	mutex_destroy(&(cond_obj->cond_lock));
}
