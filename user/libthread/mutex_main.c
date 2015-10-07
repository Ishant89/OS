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
 *	@author Ishant Dawer(idawer)
 *	@bug Malloc is not thread-safe
 */



/*EDIT: MAKE MALLOC SAFE*/
#define DEBUG 0
#include "mutex_private.h"

void debug_mutex_structure()
{
	mutex_thread_object * temp;
	thread_queue * temp_queue;
	SIPRINTF("Begin trace\n");
	for (temp = head_mutex_object; temp != NULL; 
			temp= temp->next_mutex_object)
	{
		SIPRINTF("Mutex id is %u\n",temp->mutex_id);
		thread_queue * queue = temp -> head_queue;
		for (temp_queue = queue; temp_queue!=NULL;
				temp_queue=temp_queue->next_thread_id)
		{
			SIPRINTF("TID is %u\n",temp_queue->thread_id);
		}
	}
	SIPRINTF("End trace\n");
}

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
	if (head_mutex_object == NULL)
		return FAIL;

	if (head_mutex_object -> next_mutex_object == NULL 
			&& head_mutex_object->mutex_id == mutex_id)
	{
		head_mutex_object = NULL;
		return PASS;
	}
	for (temp_obj = head_mutex_object; temp_obj->next_mutex_object != NULL; 
			temp_obj = temp_obj -> next_mutex_object)
	{
		if (temp_obj -> next_mutex_object -> mutex_id == mutex_id)
		{
			temp = temp_obj -> next_mutex_object;
			temp_obj -> next_mutex_object = 
				temp -> next_mutex_object;
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
	ASSERT(head_queue != NULL);
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

void append_thread_id_to_queue(
		thread_queue ** queue_head, 
		thread_queue * last_elem
		)
{
	ASSERT(queue_head != NULL);
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

int check_if_thread_in_queue(thread_queue ** head_queue,unsigned int thread_id)
{
	thread_queue * temp;
	for (temp=*head_queue;temp != NULL;temp=temp->next_thread_id)
	{
		if (temp->thread_id == thread_id)
		{
			return PASS;
		}
	}
	return FAIL;

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
		malloc(sizeof(mutex_thread_object));

	if (mutex_obj == NULL)
		return FAIL;
	mutex_obj -> mutex_id = mutex_id;
	mutex_obj -> head_queue = NULL;
	mutex_obj -> lock = 0;//Change with mutex lock
	
	/* Adding object to the global list of mutex objs*/
	add_mutex_object_list(mutex_obj);

	/*EDIT: TO be removed*/
	//debug_mutex_structure();
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
	SIPRINTF("Entering mutex lock id:%d by tid %d",(unsigned int)mp,gettid());
	/*Deschedule condition */
	int reject = 0;
	/* get the mutex id from the structure */
	unsigned int mutex_id = GET_MUTEX_ID(mp);
	/* Get the thread id of the thread */
	int thread_id = gettid();

	/* Get the mutex object */
	mutex_thread_object * mutex_identifier = 
		get_mutex_object_by_mutex_id(mutex_id);

	if ( mutex_identifier == NULL)
	{
		SIPRINTF("Call mutex init first \n");
		task_vanish(-2);
		return;
	}

	/* Get the head of the thread queue */
	thread_queue * head_queue = mutex_identifier -> head_queue;

	/* Verify if same thread is not issuing mutex_lock again*/
	if (check_if_thread_in_queue(&head_queue,thread_id) == PASS)
	{
		SIPRINTF("Thread already in queue \n");
		return ;
	}

	

	/* Create a thread structure to be added to head_queue */
	thread_queue * new_thread_request = 
		(thread_queue*) malloc(sizeof(thread_queue));
	new_thread_request->thread_id = thread_id;
	new_thread_request->next_thread_id = NULL;
		
	/* EDIT:Change the lock with compare & exchange or
	 * compare value & swap */
	while (compAndXchg((void*)&mutex_identifier->lock,0,1))
	{
		continue;
	}	

	SIPRINTF("Lock is with tid %d",gettid());
	/*Get the number of thread elements */
	int num_elems = get_number_elements_queue(&mutex_identifier -> head_queue);


	/* Set the first tid in the flag queue */
	int if_first_thread_in_queue = 0;
	if (num_elems == 0)
	{
		if_first_thread_in_queue = 1;
	}

	/* Critical section */
	append_thread_id_to_queue(&(mutex_identifier->head_queue),new_thread_request);


	/*Release the lock */
	mutex_identifier->lock = 0;

	SIPRINTF("Lock is just released by tid %d",gettid());
	/*EDIT: */

	debug_mutex_structure();

	if (!if_first_thread_in_queue)
	{
		/*Deschedule if not first element in the queue */
		SIPRINTF("TID is descheduled %d",gettid());
		if(deschedule(&reject) < 0)
		{
			return;
		}
	}
	SIPRINTF("Exiting mutex lock id: %d by tid %d",mutex_id,gettid());
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
	SIPRINTF("Entering mutex unlock id:%d by tid %d",(unsigned int)mp,gettid());
	/* get the mutex id from the structure */
	unsigned int mutex_id = GET_MUTEX_ID(mp);

	/* Get the mutex object */
	mutex_thread_object * mutex_identifier = 
		get_mutex_object_by_mutex_id(mutex_id);

	/* EDIT:Change the lock with compare & exchange or 
	 * compare value & swap */
	if (mutex_identifier == NULL)
	{
		SIPRINTF("Call mutex_init first");
		task_vanish(-2);
		return;
	}


	while (compAndXchg((void *)&mutex_identifier->lock,0,1))
	{
		continue;
	}	

	/* Check if the mutex unlock is not invoked by the thread id other than the 
	 * one keeping the lock */
	SIPRINTF("Lock is with tid %d",gettid());
	if (mutex_identifier -> head_queue -> thread_id != gettid())
	{
		SIPRINTF("Some other thread is trying to unlock ");
		task_vanish(-2);
		return ;
	}	

	/*Object to be removed */
	thread_queue * temp = remove_thread_id_from_queue(&(mutex_identifier->head_queue));

	/* Get the head of the thread queue */
	thread_queue * head_queue = mutex_identifier -> head_queue;

	if (head_queue != NULL)
	{
		SIPRINTF("%d made %d runnable",gettid(),mutex_identifier->head_queue->thread_id);
		while(make_runnable(mutex_identifier->head_queue->thread_id) < 0)
		{
			continue;
		}
	}
	mutex_identifier->lock = 0;

	SIPRINTF("Lock is just released by tid %d",gettid());
	/*EDIT: To be removed */
	debug_mutex_structure();
	/* Free the temp thread queue entry for the popped entry*/
	SIPRINTF("Exiting mutex unlock id:%d by tid %d",mutex_id,gettid());
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
	mutex_thread_object * mutex_identifier = 
		get_mutex_object_by_mutex_id(mutex_id);

	if (mutex_identifier == NULL)
	{
		lprintf("Use mutex init first\n");
		task_vanish(-2);
		return;
	}
	/*Take the lock */
	while (compAndXchg((void*)&mutex_identifier->lock,0,1))
	{
		continue;
	}
	/* Get the lock status from the queue and use compare excahnge */
	int lock = mutex_identifier-> lock;
	
	/* Get the number of elements in the queue */
	int num_elems = get_number_elements_queue(&(mutex_identifier->head_queue));

	/* Either lock is still taken and elements are still there */
	if (lock == 0 || num_elems != 0)
	{
		lprintf("Terminating : Either lock is nt released or elements are there\n");
		/* Change status to macro */
		task_vanish(-2);
		return;
	}

	/* Remove the mutex object by its id*/
	rem_mutex_object_by_mutex_id(mutex_id);

	/*Release the lock */
	mutex_identifier->lock = 0;
	
	/* Mostly head_queue should be null here */
	if (mutex_identifier->head_queue != NULL)
		free(mutex_identifier->head_queue);


	/* Remove the mutex object */
	free(mutex_identifier);
	/*EDIT: To be removed */
	//debug_mutex_structure();
}
