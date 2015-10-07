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

/** @brief Add condvar object to the global list 
 *  
 *  This adds cond var object to the list 
 *  @param cv condvar id
 *
 *  @return Pass/Fail
 */

//#define DEBUG 0
#define DEBUG_CRITICAL 0
#include "cvar_private.h"

void debug_cond_structure()
{
	cond_var_object * temp;
	wait_thread_queue * temp_queue;
	ISPRINTF("Begin trace\n");
	for (temp = head_cond_object; temp != NULL; 
			temp= temp->next_cond_object)
	{
		ISPRINTF("Cond id is %u",temp->cond_id);
		wait_thread_queue * queue = temp -> head_queue;
		for (temp_queue = queue; temp_queue!=NULL;
				temp_queue=temp_queue->next_wait_thread)
		{
			ISPRINTF("TID is %u",temp_queue->thread_id);
		}
	}
	ISPRINTF("End trace\n");
}

void add_cond_object_list(cond_var_object * object)
{
	cond_var_object * temp = head_cond_object;
	head_cond_object = object;
	object -> next_cond_object = temp;
}
/** @brief get condvar object by id 
 *  
 *  Get the condvar object by its id 
 *  @param cv Condvar id 
 *
 *  @return object cond var object 
 */
cond_var_object * get_cond_obj_by_id(unsigned int cv)
{
	cond_var_object * temp = NULL;
	for (temp = head_cond_object;temp!=NULL;temp = temp->next_cond_object)
	{
		if (temp -> cond_id == cv)
		{
			return temp;
		}
	}
	return NULL;
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

int check_if_thread_in_wait_queue(wait_thread_queue ** head_queue,unsigned int thread_id)
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
 *  @param queue Queue
 *
 *  @param thread_id last_elem
 *  @return void 
 */

void append_thread_id_to_wait_queue(
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

/** @brief remove cond objects by cond id 
 *  
 *  This removes cond object from the
 *  list 
 *   
 *  @param cond_id cond id 
 *  @return Pass/Fail
 */

int rem_cond_object_by_cond_id(unsigned int cond_id)
{
	cond_var_object * temp_obj,*temp; 
	if (head_cond_object == NULL)
		return FAIL;

	if ( head_cond_object->cond_id == cond_id)
	{
		head_cond_object = head_cond_object -> next_cond_object;
		return PASS;
	}
	for (temp_obj = head_cond_object; temp_obj->next_cond_object != NULL; 
			temp_obj = temp_obj -> next_cond_object)
	{
		if (temp_obj -> next_cond_object -> cond_id == cond_id)
		{
			temp = temp_obj -> next_cond_object;
			temp_obj -> next_cond_object = 
				temp -> next_cond_object;
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

wait_thread_queue * remove_thread_id_from_wait_queue(wait_thread_queue ** queue_head)
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
 *  1. It takes cv variable and uses it as an id 
 *  2. It checks if ID already exists in the global list 
 *  3. If not returns Failure 
 *  4. Create a new object such that lock is available, next cond obj
 *  is NULL, head_queue is empty and outer mutex is not mapped yet
 *  5. Adds to the list successfully 
 *
 *  @param cv Condvar ID 
 *  @return PASS/FAIL
 */

int cond_init(cond_t * cv)
{
	SIPRINTF("Entering cond_init with cond object id : %d by tid: %d",(unsigned int)cv,gettid());

	unsigned int cond_id = GET_COND_ID(cv);
	/* Creating the cond object */
	cond_var_object * cond_obj = (cond_var_object*) malloc(COND_OBJ_SIZE);

	if (cond_obj == NULL)
	{
		SIPRINTF("Unable to allocate cond_object");
		return FAIL;
	}
	cond_obj-> cond_id = cond_id;
	//cond_obj -> cond_lock = LOCK_AVAILABLE;
	cond_obj -> head_queue = NULL;
	cond_obj-> mutex_object = NULL;
	cond_obj -> next_cond_object = NULL;
	if (mutex_init(&(cond_obj->cond_lock)) < 0)
	{
		SIPRINTF("Unable to allocate lock");
		task_vanish(-2);
	}
	
	/* Check if cond object is already init and reinit again */
	cond_var_object * existing_obj = get_cond_obj_by_id(cond_id);

	if (existing_obj != NULL && existing_obj-> head_queue != NULL)
	{
		SIPRINTF("Already objects exist and some threads are using it");
		return FAIL;
	}

	/*Adding the object to the global list */
	
	add_cond_object_list(cond_obj);
	SIPRINTF("Exiting cond_init with cond object id : %d by tid: %d",(unsigned int)cv,gettid());

	return PASS;
}


/** @brief Cond wait 
 *  
 *  This function does the  following : 
 *  1. It checks if invoking thread id is already in the 
 *  queue 
 *  2. If yes, it throws an error 
 *  3. If no, it creates a new thread_id structure 
 *  4. Adds it to the list atomically (with internal lock
 *  5. Unlocks the global lock 
 *  6. Deschedule it 
 *  7. After deschedule it will try to acquire the lock again
 */

void cond_wait(cond_t * cv, mutex_t*mp)
{
	SIPRINTF("Entering cond_wait with cv %d and mp %d by tid: %d",
			(unsigned int) cv,(unsigned int) mp,gettid());
	/* Reject condition */
	int reject = 0;
	/* Condv id */
	unsigned int cond_id = GET_COND_ID(cv);

	/* Get the thread id  */
	int thread_id = gettid();

	/* Get the condvar object */
	cond_var_object * cond_obj = get_cond_obj_by_id(cond_id);

	if (cond_obj == NULL)
	{
		SIPRINTF("Cond var object not initialized");
		task_vanish(-2);
	}

	/* Check if thread is already waiting */
	if (!check_if_thread_in_wait_queue(&cond_obj->head_queue,thread_id))
	{
		SIPRINTF("Thread already waiting");
		task_vanish(-2);
	}

	/* Create the thread id structure */
	wait_thread_queue * new_thread_request = 
		(wait_thread_queue*) malloc(sizeof(wait_thread_queue));
	new_thread_request->thread_id = thread_id;
	new_thread_request->next_wait_thread = NULL;

	/* Take the lock */
/*
	while (compAndXchg((void *)&cond_obj->cond_lock,0,1))
	{
		continue;
	}
*/
	mutex_lock(&(cond_obj->cond_lock));	
	SIPRINTF("Lock is with tid %d",gettid());
	/*Append to the queue */

	append_thread_id_to_wait_queue(&(cond_obj->head_queue),
			new_thread_request);

	/* binding the global mutex to condvar */
	cond_obj -> mutex_object = mp;

	/* unlocking the global mutex */
	mutex_unlock(mp);

	/*Release the lock */
	//cond_obj->cond_lock = 0;
	mutex_unlock(&(cond_obj->cond_lock));
	
	SIPRINTF("Lock is just released by tid %d",gettid());
	/*EDIT: */

	//debug_cond_structure();

	/*Deschedule */
	SIPRINTF("TID is descheduled %d",gettid());
	if(deschedule(&reject) < 0)
	{
		return;
	}
	SIPRINTF("Exiting cond_wait with cv %d and mp %d by tid: %d",
			(unsigned int) cv,(unsigned int) mp,gettid());
	
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
	SIPRINTF("Entering cond_signal with condv %d by tid: %d",
			(unsigned int) cv,gettid());
	/* Condv id */
	unsigned int cond_id = GET_COND_ID(cv);

	/* Get the thread id  */
	int thread_id = gettid();

	/* Get the condvar object */
	cond_var_object * cond_obj = get_cond_obj_by_id(cond_id);

	if (cond_obj == NULL)
	{
		SIPRINTF("Cond var object not initialized");
		task_vanish(-2);
	}

	/* Take the lock */

	/*while (compAndXchg((void *)&cond_obj->cond_lock,0,1))
	{
		continue;
	}*/

	mutex_lock(&(cond_obj->cond_lock));

	if (cond_obj -> mutex_object == NULL)
	{
		SIPRINTF("None of the mutex objects bound");
		/*Release the lock */
		mutex_unlock(&(cond_obj->cond_lock));
		//cond_obj->cond_lock = 0;
		return;
	}

	if (cond_obj -> head_queue == NULL)
	{
		SIPRINTF("Queue is empty");
		/*Release the lock */
		mutex_unlock(&(cond_obj->cond_lock));
		//cond_obj->cond_lock = 0;
		return;
	}
	
	SIPRINTF("Lock is with tid %d",gettid());
	/*Pop the first element from the queue*/

	debug_cond_structure();
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
	SIPRINTF("Made runnable %d by %d",pop_elem->thread_id,thread_id);
	
	/*Release the lock */
	mutex_unlock(&(cond_obj->cond_lock));
	//cond_obj->cond_lock = 0;
    	
	free(pop_elem);
	SIPRINTF("Lock is just released by tid %d",gettid());
	/*EDIT: */

	debug_cond_structure();

	SIPRINTF("Exiting cond_signal with cv %d by tid: %d", 
			(unsigned int) cv,gettid());

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
	SIPRINTF("Entering cond_broadcast with condv %d by tid: %d",
			(unsigned int) cv,gettid());
	/* Condv id */
	unsigned int cond_id = GET_COND_ID(cv);

	/* Get the thread id  */
	int thread_id = gettid();

	/* Get the condvar object */
	cond_var_object * cond_obj = get_cond_obj_by_id(cond_id);

	if (cond_obj == NULL)
	{
		SIPRINTF("Cond var object not initialized");
		task_vanish(-2);
	}

	/* Take the lock */
/*
	while (compAndXchg((void *)&cond_obj->cond_lock,0,1))
	{
		continue;
	}
*/
	mutex_lock(&(cond_obj->cond_lock));
	if (cond_obj -> mutex_object == NULL)
	{
		SIPRINTF("None of the mutex objects found");
		/*Release the lock */
		mutex_unlock(&(cond_obj->cond_lock));
		//cond_obj->cond_lock = 0;
		return;
	}

	if (cond_obj -> head_queue == NULL)
	{
		SIPRINTF("Queue is empty");
		/*Release the lock */
		mutex_unlock(&(cond_obj->cond_lock));
		//cond_obj->cond_lock = 0;
		return;
	}
	
	SIPRINTF("Lock is with tid %d",gettid());
	/*Pop the first element from the queue*/

	wait_thread_queue * pop_elem;
	debug_cond_structure();
   	while((pop_elem 
				= remove_thread_id_from_wait_queue(&(cond_obj->head_queue)))
			!=NULL)
	{
		/* Make runnable */
		while (make_runnable(pop_elem->thread_id) < 0)
		{
			continue;
		}
		SIPRINTF("Made runnable %d by %d",pop_elem->thread_id,thread_id);
		free(pop_elem);
	}
	/*Release the lock */
	mutex_unlock(&(cond_obj->cond_lock));
	//cond_obj->cond_lock = 0;
	
	SIPRINTF("Lock is just released by tid %d",gettid());
	/*EDIT: */

	debug_cond_structure();

	SIPRINTF("Exiting cond_broadcast with cv %d by tid: %d", 
			(unsigned int) cv,gettid());

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

	SIPRINTF("Entering cond_destroy with condv %d by tid: %d",
			(unsigned int) cv,gettid());
	/* Condv id */
	unsigned int cond_id = GET_COND_ID(cv);


	/* Get the condvar object */
	cond_var_object * cond_obj = get_cond_obj_by_id(cond_id);

	if (cond_obj == NULL)
	{
		SIPRINTF("Cond var object not initialized");
		task_vanish(-2);
	}
	/*Take the lock */
	/*while (compAndXchg((void*)&cond_obj->cond_lock,0,1))
	{
		continue;
	}*/
	mutex_lock(&(cond_obj->cond_lock));
	/* Get the lock status from the queue and use compare excahnge */
	//int lock = cond_obj -> cond_lock;
	

	/* Either lock is still taken and elements are still there */
	//if (lock == 0 || cond_obj->head_queue != NULL)
	if (cond_obj->head_queue != NULL)
	{
		SIPRINTF("Terminating : Either lock is nt released or elements are there\n");
		/* Change status to macro */
		task_vanish(-2);
	}

	/* Remove the mutex object by its id*/
	rem_cond_object_by_cond_id(cond_id);

	/*Release the lock */
	mutex_unlock(&(cond_obj->cond_lock));
	//cond_obj->cond_lock = 0;
	
	/* Mostly head_queue should be null here */
	if (cond_obj->head_queue != NULL)
		free(cond_obj->head_queue);

	/* Destroy the mutex */

	mutex_destroy(&(cond_obj->cond_lock));
	/* Remove the mutex object */
	free(cond_obj);
	/*EDIT: To be removed */
	//debug_cond_structure();
	SIPRINTF("Exiting cond_destroy with condv %d by tid: %d",
			(unsigned int) cv,gettid());
}









