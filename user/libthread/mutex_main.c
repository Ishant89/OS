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
#define DEBUG_CRITICAL 0
#include "mutex_private.h"

void debug_mutex_structure()
{
	mutex_t * temp;
	thread_queue * temp_queue;
	SIPRINTF("Begin trace\n");
	for (temp = head_mutex_object; temp != NULL; 
			temp= temp->next_mutex_object)
	{
		SIPRINTF("Mutex id is %x",temp->mutex_id);
		thread_queue * queue = temp -> head_queue;
		for (temp_queue = queue; temp_queue!=NULL;
				temp_queue=temp_queue->next_thread_id)
		{
			SIPRINTF("TID is %u",temp_queue->thread_id);
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
void add_mutex_object_list(mutex_t * object)
{
	mutex_t * temp = head_mutex_object;
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
	mutex_t * temp_obj,*temp; 
	if (head_mutex_object == NULL)
		return FAIL;

	if (head_mutex_object->mutex_id == mutex_id)
	{
		head_mutex_object = head_mutex_object -> next_mutex_object;
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

mutex_t * get_mutex_object_by_mutex_id(unsigned int mutex_id)
{
	mutex_t * temp = NULL;
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
	SIPRINTF("Entering mutex init by id: %d with params: %x",
			gettid(),(unsigned int)mp);
	/*Get the mutex ID from the structure */
	unsigned int mutex_id = GET_MUTEX_ID(mp);
	/* Assign a mutex object */
	mutex_t * mutex_obj = mp;

	mutex_obj -> mutex_id = mutex_id;
	mutex_obj -> head_queue = NULL;
	mutex_obj -> lock_owner = -1;
	mutex_obj -> lock = 0;//Change with mutex lock
	mutex_obj -> unlock_flag = 0;//Change with mutex lock
	
	/* Adding object to the global list of mutex objs*/
	//add_mutex_object_list(mutex_obj);

	/*EDIT: TO be removed*/
	SIPRINTF("Exiting mutex init by id: %d with params: %x",
			gettid(),(unsigned int)mp);
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
	SIPRINTF("Entering mutex lock id:%x by tid %d",(unsigned int)mp,gettid());
	//int reject=0;
	//thread_queue * head_queue;
	/* get the mutex id from the structure */
	//unsigned int mutex_id = GET_MUTEX_ID(mp);
	/* Get the thread id of the thread */
	int thread_id = gettid();

	/* Get the mutex object */
	mutex_t * mutex_identifier = mp;
	//	get_mutex_object_by_mutex_id(mutex_id);

	if ( mutex_identifier == NULL)
	{
		SIPRINTF("Call mutex init first \n");
		task_vanish(-2);
		return;
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
		yield(mutex_identifier->lock_owner);
	}
	/* EDIT:Change the lock with compare & exchange or
	 * compare value & swap */
	while (compAndXchg((void*)&mutex_identifier->lock,0,1))
	{
		yield(mutex_identifier->lock_owner);
	}	

	SIPRINTF("Lock is with tid %d",gettid());
	/*Get the number of thread elements */
	/*int num_elems = get_number_elements_queue(&mutex_identifier -> head_queue);
*/
	/* Get the head of the thread queue */

	
	//head_queue = mutex_identifier -> head_queue;

	/* Verify if same thread is not issuing mutex_lock again*/
	/*
	if (check_if_thread_in_queue(&head_queue,thread_id) == PASS ||
			mutex_identifier-> lock_owner == thread_id)
	{
		SIPRINTF("Thread already in queue \n");
		return ;
	}
*/
	/* Set the first tid in the flag queue */
	if (mutex_identifier->lock_owner == -1)
	{
		mutex_identifier-> lock_owner = thread_id;
		/*Release the lock */
		mutex_identifier->lock = 0;
		ISPRINTF("Lock owned by tid : %d for %p",thread_id,mp);
		return;
	} 
	
	/* Critical section */
	append_thread_id_to_queue(&(mutex_identifier->head_queue),
		&new_thread_request);
	
	/*Release the lock */
	mutex_identifier->lock = 0;

	SIPRINTF("Lock is just released by tid %d",gettid());
	/*EDIT: */


	/*Deschedule if not first element in the queue */
	ISPRINTF("TIDd is descheduled %d for %p and lo: %d",gettid(),mp
			,mutex_identifier-> lock_owner);
	//reject = mutex_identifier->lock_owner == thread_id;
	while(new_thread_request.reject != 1 || mutex_identifier-> lock_owner != thread_id)
	{
		deschedule(&new_thread_request.reject);
	}

	/*while (compAndXchg((void*)&mutex_identifier->lock,0,1))
	{
		continue;
	}*/	
	SIPRINTF("Waking up from  mutex lock id: %p by tid %d",mp,gettid());
	/*Object to be removed */
/*	
	while (compAndXchg((void*)&mutex_identifier->lock,0,1))
			
	{
		continue;
	}
	remove_thread_id_from_queue(
			&(mutex_identifier->head_queue));
*/
	/*Release the lock */
//	mutex_identifier->lock = 0;
	ISPRINTF("TIDr is rescheduled %d for %p and lo:%d",gettid(),mp
			,mutex_identifier-> lock_owner);
	//while(mutex_identifier->lock_owner!=thread_id);
	SIPRINTF("Exiting mutex lock id: %p by tid %d",mp,gettid());
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
	SIPRINTF("Entering mutex unlock id:%x by tid %d",(unsigned int)mp,gettid());
	/* get the mutex id from the structure */
	//unsigned int mutex_id = GET_MUTEX_ID(mp);
	unsigned int head_id;
	thread_queue * temp_queue;

	/* Get the mutex object */
	mutex_t * mutex_identifier = mp;
	//	get_mutex_object_by_mutex_id(mutex_id);

	/* EDIT:Change the lock with compare & exchange or 
	 * compare value & swap */
	if (mutex_identifier == NULL)
	{
		SIPRINTF("Call mutex_init first");
		task_vanish(-2);
		return;
	}

	/* Registering interest for queue lock */	

	mutex_identifier -> unlock_flag++;
	while (compAndXchg((void *)&mutex_identifier->lock,0,1))
	{
		yield(mutex_identifier->lock_owner);
	}	

	/* Check if the mutex unlock is not invoked by the thread id other than the 
	 * one keeping the lock */
	SIPRINTF("Lock is with tid %d",gettid());
	/*if (mutex_identifier -> head_queue -> thread_id != gettid())
	{
		SIPRINTF("Some other thread is trying to unlock ");
		task_vanish(-2);
		return ;
	}*/	

	if (mutex_identifier->head_queue != NULL)
	{
		head_id = mutex_identifier->head_queue->thread_id;
		temp_queue= remove_thread_id_from_queue(
			&(mutex_identifier->head_queue));
		ISPRINTF("%d made %d runnable for %p",gettid(),head_id,mp);
		mutex_identifier-> lock_owner = head_id;
		temp_queue->reject = 1;
		make_runnable(head_id) ;
	} else 
	{
		mutex_identifier->lock_owner = -1;
	}
	/*Deregister interest*/
	mutex_identifier -> unlock_flag--;
	mutex_identifier->lock = 0;

	SIPRINTF("Lock is just released by tid %d",gettid());
	/*EDIT: To be removed */
	/* Free the temp thread queue entry for the popped entry*/
	SIPRINTF("Exiting mutex unlock id:%p by tid %d",mp,gettid());
	
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
	SIPRINTF("Entering mutex destroy id:%x by tid %d",(unsigned int)mp,gettid());
	/* get the mutex id from the structure */
	//unsigned int mutex_id = GET_MUTEX_ID(mp);

	/* Get the mutex object */
	mutex_t * mutex_identifier = mp ;
	//	get_mutex_object_by_mutex_id(mutex_id);

	if (mutex_identifier == NULL)
	{
		lprintf("Use mutex init first\n");
		task_vanish(-2);
		return;
	}
	/*Take the lock */
/*	while (compAndXchg((void*)&mutex_identifier->lock,0,1))
	{
		continue;
	}*/

	/* Either lock is still taken and elements are still there */
	if (mutex_identifier-> lock_owner !=-1)
	{
		lprintf("Terminating : Either lock is nt released or elements are there\n");
		/* Change status to macro */
		task_vanish(-2);
		return;
	}

	/* Remove the mutex object by its id*/
	//rem_mutex_object_by_mutex_id(mutex_id);

	/*Release the lock */
	mutex_identifier->lock = 0;

	/*EDIT: To be removed */
	SIPRINTF("Exiting mutex destroy id:%x by tid %d",(unsigned int)mp,gettid());
}
