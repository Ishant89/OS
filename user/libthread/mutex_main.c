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
 *	
 *	@author Ishant Dawer(idawer)
 *	@bug Malloc is not thread-safe
 */

/*EDIT: MAKE MALLOC SAFE*/
#include "mutex_private.h"

/** @brief Add mutex objects to top
 *  
 *  This adds mutex object to the top of the mutex 
 *  list 
 *   
 *  @param object mutex object
 *  @return void
 */
void add_mutex_object_list(mutex_thread_object * object)
{
	mutex_thread_object * temp = head_mutex_object;
	head_mutex_object = object;
	object -> next_mutex_object = temp;
}

/** @brief remove mutex objects by mutex id 
 *  
 *  This removes mutex object from the
 *  list 
 *   
 *  @param mutex_id Mutex id 
 *  @return Pass/Fail
 */

int rem_mutex_object_by_mutex_id(unsigned int mutex_id)
{
	mutex_thread_object * temp_obj,*temp; 
	for (temp_obj = head_mutex_object; temp_obj != NULL; temp_obj = temp_obj -> next_mutex_object)
	{
		if (temp_obj -> next_mutex_object -> mutex_id == mutex_id)
		{
			temp = temp_obj -> next_mutex_object;
			temp_obj -> next_mutex_object = temp -> next_mutex_object;
			return PASS;
		}
	}
	return FAIL;
}

/** @brief get the mutex object by mutex id 
 *  
 *  This function is used to find the mutex object by its 
 *  id. 
 *
 *  @param mutex_id Mutex id 
 *  @return mutex_object
 */

mutex_thread_object * get_mutex_object_by_mutex_id(unsigned int mutex_id)
{
	mutex_thread_object * temp = NULL;
	for (temp = head_mutex_object;temp!=NULL;temp = temp->next_mutex_object)
	{
		if (temp -> mutex_id == mutex_id)
		{
			return temp;
		}
	}
	return NULL;
}

/** @brief Get the number of queue elements 
 *  
 *  Find the number of elements in the queue 
 *
 *  @param thread_queue
 *  @return int 
 */

int get_number_elements_queue(thread_queue ** head_queue)
{
	int num_elems = 0 ;
	thread_queue * temp;
	for (temp=*head_queue;temp!=NULL;temp=temp->next_thread_id)
	{
		num_elems++;
	}
	return num_elems;
}
/** @brief append thread id to the queue
 *  
 *  This pushes the thread id to the end
 *  of the queue 
 *
 *  @param queue Queue
 *
 *  @param thread_id last_elem
 *  @return void 
 */

void append_thread_id_to_queue(thread_queue ** queue_head, thread_queue * last_elem)
{
	thread_queue * temp;
	if (*queue_head == NULL)
	{
		*queue_head = last_elem;
		return;
	}
	for (temp=*queue_head; (temp -> next_thread_id) != NULL;temp=temp->next_thread_id)
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

thread_queue * remove_thread_id_from_queue(thread_queue ** queue_head)
{
	thread_queue * temp = NULL;
	if (queue_head != NULL)
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
 *  5. add the mutex_object to the top of the list 
 *
 *  @param mutex_t mutex object
 *  @return int Pass/Fail
 */

int mutex_init(mutex_t * mp)
{
	/*Get the mutex ID from the structure */
	unsigned int mutex_id = GET_MUTEX_ID(mp);
	/* Assign a mutex object */
	mutex_thread_object * mutex_obj = (mutex_thread_object*)
		_malloc(sizeof(mutex_thread_object));

	if (mutex_obj == NULL)
		return FAIL;
	mutex_obj -> mutex_id = mutex_id;
	mutex_obj -> head_queue = NULL;
	mutex_obj -> lock = 1;//Change with mutex lock
	
	/* Adding object to the global list of mutex objs*/
	add_mutex_object_list(mutex_obj);
	return PASS;	
}

/** @brief Mutex lock API 
 *
 *  This function does the following : 
 *
 *  1. This API adds the thread to the end of the waiting queue 
 *	2. If the thread is the only element in the queue, then it should not 
 *	be descheduled 
 *	3. If the thread is not the first element in the queue, it should be 
 *	descheduled
 *	
 *	@param mutex_t* mp 
 *
 *	@return void
 */

void mutex_lock(mutex_t * mp)
{
	/*Deschedule condition */
	int reject = 0;
	/* get the mutex id from the structure */
	unsigned int mutex_id = GET_MUTEX_ID(mp);

	/* Get the thread id of the thread */
	int thread_id = thr_getid();

	/* Get the mutex object */
	mutex_thread_object * mutex_identifier = get_mutex_object_by_mutex_id(mutex_id);

	/* Get the head of the thread queue */
	thread_queue * head_queue = mutex_identifier -> head_queue;

	/*Get the number of thread elements */
	int num_elems = get_number_elements_queue(&head_queue);

	/* Set the first tid in the flag queue */
	int if_first_thread_in_queue = 0;
	if (num_elems == 0)
	{
		if_first_thread_in_queue = 1;
	}

	/* Create a thread structure to be added to head_queue */
	thread_queue * new_thread_request = (thread_queue*) _malloc(sizeof(thread_queue));
	new_thread_request->thread_id = thread_id;
	new_thread_request->next_thread_id = NULL;
		
	/* EDIT:Change the lock with compare & exchange or compare value & swap */
	while (!mutex_identifier->lock)
	{
		continue;
	}	
	/*Take the lock */
	mutex_identifier->lock = 0;

	/* Critical section */
	append_thread_id_to_queue(&head_queue,new_thread_request);

	/*Release the lock */
	mutex_identifier->lock = 1;

	if (!if_first_thread_in_queue)
	{
		/*Deschedule if not first element in the queue */
		if(deschedule(&reject) < 0)
		{
			return;
		}
	}
}


/** @brief Mutex unlock API 
 *  
 *  This API unlocks the thread 
 *  It does : 
 *  1. it removes the top element and checks if the queue is empty
 *  2. if not empty, it removes the top element from the queue
 *  3. Check if the queue is emty or not 
 *  4. if empty, no further make_runnable 
 *	
 *	@param mutex_t 
 *
 *
 */

void mutex_unlock(mutex_t *mp)
{
	/* get the mutex id from the structure */
	unsigned int mutex_id = GET_MUTEX_ID(mp);

	/* Get the mutex object */
	mutex_thread_object * mutex_identifier = get_mutex_object_by_mutex_id(mutex_id);
	/* Get the head of the thread queue */
	thread_queue * head_queue = mutex_identifier -> head_queue;

	/* EDIT:Change the lock with compare & exchange or compare value & swap */
	while (!mutex_identifier->lock)
	{
		continue;
	}	
	/*Take the lock */
	mutex_identifier->lock = 0;

	/*Object to be removed */
	thread_queue * temp = remove_thread_id_from_queue(&head_queue);

	if (head_queue != NULL)
	{
		if(make_runnable(head_queue->thread_id) < 0)
		{
			mutex_identifier->lock = 1;
			return; 
		}
	}
	mutex_identifier->lock = 1;

	/* Free the temp thread queue entry for the popped entry*/
	free(temp);

}

/** @brief mutex destroy
 *  
 *  This clears all the stuff and data structures created by mutex_init
 *
 *  1. Clear the mutex object 
 *  2. If threads are accessing it and lock is there, don't destroy 
 */

void mutex_destroy(mutex_t *mp)
{
	/* get the mutex id from the structure */
	unsigned int mutex_id = GET_MUTEX_ID(mp);

	/* Get the mutex object */
	mutex_thread_object * mutex_identifier = get_mutex_object_by_mutex_id(mutex_id);
	/* Get the head of the thread queue */
	thread_queue * head_queue = mutex_identifier -> head_queue;
	/* Get the lock status from the queue and use compare excahnge */
	int lock = mutex_identifier-> lock;
	
	/* Get the number of elements in the queue */
	int num_elems = get_number_elements_queue(&head_queue);

	if (lock == 0 || num_elems != 0)
	{
		return;
	}

	/* Mostly head_queue should be null here */
	if (head_queue != NULL)
		free(head_queue);

	/* Remove the mutex object */
	free(mutex_identifier);
}
