/** @file rwlock_main.c
 *  
 *  @brief Rwlock library 
 *
 *  This file implements the mutex functions for the 
 *  thread library 
 *
 *  1. rwlock_init
 *  2. rwlock_destroy 
 *  3. rwlock_lock
 *  4. rwlock_unlock
 *  5. rwlock_downgrade
 *	
 *	@author Ishant Dawer(idawer) & Shelton D'souza(sdsouza)
 *	@bug No known bugs
 */

#include "rwlock_private.h"

/** @brief Add rwlock objects to top
 *  
 *  This adds rwlock object to the top of the rwlock 
 *  list 
 *   
 *  @param object rwlock object
 *  @return void
 */
void add_rwlock_object_list(rwlock_thread_object * object)
{
	rwlock_thread_object * temp = head_rwlock_object;
	head_rwlock_object = object;
	object -> next_rwlock_object = temp;
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

void add_thread_id_to_queue(
		thread_queue_rwlock ** queue_head, 
		thread_queue_rwlock * last_elem
		)
{
	ASSERT(queue_head != NULL);
	thread_queue_rwlock * temp;
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

/** @brief remove rwlock objects by rwlock id 
 *  
 *  This removes rwlock object from the
 *  list 
 *   
 *  @param rwlock_id Mutex id 
 *  @return Pass/Fail
 */

int rem_rwlock_object_by_rwlock_id(unsigned int rwlock_id)
{
    rwlock_thread_object * temp_obj,*temp; 
    if (head_rwlock_object == NULL)
        return FAIL;

    if (head_rwlock_object->rwlock_id == rwlock_id)
    {
        head_rwlock_object = head_rwlock_object -> next_rwlock_object;
        return PASS;
    }
    for (temp_obj = head_rwlock_object; temp_obj->next_rwlock_object != NULL; 
            temp_obj = temp_obj -> next_rwlock_object)
    {
        if (temp_obj -> next_rwlock_object -> rwlock_id == rwlock_id)
        {
            temp = temp_obj -> next_rwlock_object;
            temp_obj -> next_rwlock_object = 
                temp -> next_rwlock_object;
            return PASS;
        }
    }
    return FAIL;
}


/** @brief get the rwlock object by rwlock id 
 *  
 *  This function is used to find the rwlock object by its 
 *  id. 
 *
 *  @param rwlock_id Mutex id 
 *  @return rwlock_object
 */

rwlock_thread_object * get_rwlock_object_by_rwlock_id(unsigned int rwlock_id)
{
	rwlock_thread_object * temp = NULL;
	for (temp = head_rwlock_object;temp!=NULL;temp = temp->next_rwlock_object)
	{
		if (temp -> rwlock_id == rwlock_id)
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

thread_queue_rwlock * remove_thread_from_queue_rwlock(
		thread_queue_rwlock ** head_queue,
		unsigned int thread_id)
{
	thread_queue_rwlock * temp=*head_queue;
	thread_queue_rwlock * ret_obj=NULL;
	if (temp->thread_id == thread_id)
	{
		ret_obj = temp;
		*head_queue = temp->next_thread_id;
		return temp;
	}
	for (; (temp -> next_thread_id) != NULL;
			temp=temp->next_thread_id)
	{
		if (temp->next_thread_id->thread_id == thread_id)
			break;
	}	
	ret_obj = temp->next_thread_id;
	temp -> next_thread_id = temp->next_thread_id->next_thread_id;
	return ret_obj;
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

thread_queue_rwlock *check_if_thread_in_queue_rwlock(
		thread_queue_rwlock ** head_queue,
		unsigned int thread_id)
{
	thread_queue_rwlock * temp=NULL;
	for (temp=*head_queue;temp != NULL;temp=temp->next_thread_id)
	{
		if (temp->thread_id == thread_id)
		{
			return temp;
		}
	}
	return temp;

}

/** @brief Init rwlock structure
 *  
 *  This does following : 
 *  1. It creates a new rwlock object 
 *  2. It adds it to the global list
 *
 *  @param rwlock 
 *  @return int 
 */

int rwlock_init(rwlock_t * rwlock)
{
	/* Get the rwlock ID */
	unsigned int rwlock_id = GET_RWLOCK_ID(rwlock);

	/* Create a new object */
	rwlock_thread_object * rwlock_obj = (rwlock_thread_object*)
		malloc(sizeof(rwlock_thread_object));

	if (rwlock_obj == NULL)
	{
		SIPRINTF("Unable to alloc");
		return FAIL;
	}

	/* Populate the fields */

	rwlock_obj -> rwlock_id = rwlock_id;
	if (mutex_init(&(rwlock_obj-> global_lock)) < 0)
	{
		SIPRINTF("Unable to init  mutex");
		return FAIL;
	}

	if (cond_init(&(rwlock_obj-> read_cond)) < 0)
	{
		SIPRINTF("Unable to init  read cond");
		return FAIL;
	}
	
	if (cond_init(&(rwlock_obj-> write_cond)) < 0)
	{
		SIPRINTF("Unable to init  write cond");
		return FAIL;
	}

	rwlock_obj-> readcnt = 0;
	rwlock_obj-> writecnt = 0;
	rwlock_obj-> next_rwlock_object = NULL;
	rwlock_obj->head_queue = NULL;
	rwlock_obj->downgrade_id = -1;
	/* Add to the global list */
	
	add_rwlock_object_list(rwlock_obj);
	return PASS;
}

/** @brief handle reader request
 *  
 *  This does : 
 *  1. 
 *  @param rwlock_identifier
 */

void reader_request_handle(rwlock_thread_object * rwlock_identifier)
{
	/* Take the lock */
	mutex_lock(&(rwlock_identifier->global_lock));

	SIPRINTF("Reader came");
	while (rwlock_identifier->writecnt > 0 && 
			rwlock_identifier->downgrade_id==-1)
	{
		cond_wait(&(rwlock_identifier->read_cond),
				&(rwlock_identifier->global_lock));
	}
	/*Increment the readcount */
	rwlock_identifier->readcnt++;
	/* Unlock */
	thread_queue_rwlock * new_thread_request = (thread_queue_rwlock*)malloc(sizeof(thread_queue_rwlock));
	new_thread_request -> thread_id = gettid();
	new_thread_request -> type = READ;
	new_thread_request -> next_thread_id = NULL;


	add_thread_id_to_queue(&(rwlock_identifier->head_queue),new_thread_request);
	mutex_unlock(&(rwlock_identifier->global_lock));
}
/** @brief handle writeer request
 *  
 *  This does : 
 *  1. 
 *  @param rwlock_identifier
 */

void writer_request_handle(rwlock_thread_object * rwlock_identifier)
{
	/* Take the lock */
	mutex_lock(&(rwlock_identifier->global_lock));
	SIPRINTF("Writer came");
	while (rwlock_identifier->writecnt > 0 || 
			rwlock_identifier->readcnt > 0 
			)
	{
		cond_wait(&(rwlock_identifier->write_cond),
				&(rwlock_identifier->global_lock));
	}
	rwlock_identifier->writecnt++;
	thread_queue_rwlock * new_thread_request = (thread_queue_rwlock*)malloc(sizeof(thread_queue_rwlock));
	new_thread_request -> thread_id = gettid();
	new_thread_request -> type = WRITE;
	new_thread_request -> next_thread_id = NULL;


	add_thread_id_to_queue(&(rwlock_identifier->head_queue),new_thread_request);
/* Unlock */
	mutex_unlock(&(rwlock_identifier->global_lock));
}

/** @brief rwlock lock 
 *  
 *  This does : 
 *  1. Acquire the lock 
 *  2. Checks if writers are pending 
 *  3. If yes, puts itslef into wait
 *  4. 
 *
 *  @param rwlock
 *  @param type
 *	
 *	@return void
 */

void rwlock_lock(rwlock_t * rwlock, int type)
{
	SIPRINTF("Entering rwlock id:%d by tid %d and type: %d",
			(unsigned int)rwlock,gettid(),type);
	/* get the rwlock id from the structure */
	unsigned int rwlock_id = GET_RWLOCK_ID(rwlock);

	/* Get the rwlock object */
	rwlock_thread_object * rwlock_identifier = 
		get_rwlock_object_by_rwlock_id(rwlock_id);

	if ( rwlock_identifier == NULL)
	{
		SIPRINTF("Call rwlock init first \n");
		task_vanish(-2);
	}

	/* Check what is the type */

	switch(type)
	{
		case READ:
			reader_request_handle(rwlock_identifier);
			break;

		case WRITE:
			writer_request_handle(rwlock_identifier);
			break;
	}

}




/** @brief rwlock unlock 
 *  
 *  This does : 
 *  1. Acquire the lock 
 *  2. Checks if writers are pending 
 *  3. If yes, puts itslef into wait
 *  4. 
 *
 *  @param rwlock
 *  @param type
 *	
 *	@return void
 */

void rwlock_unlock(rwlock_t * rwlock)
{
	SIPRINTF("Entering rwlock id:%d by tid %d ",
			(unsigned int)rwlock,gettid());
	/* get the rwlock id from the structure */
	unsigned int rwlock_id = GET_RWLOCK_ID(rwlock);
	/* Get the thread id of the thread */
	int thread_id = gettid();

	/* Get the rwlock object */
	rwlock_thread_object * rwlock_identifier = 
		get_rwlock_object_by_rwlock_id(rwlock_id);

	if ( rwlock_identifier == NULL)
	{
		SIPRINTF("Call rwlock init first \n");
		task_vanish(-2);
	}
	
	/* Check if the request is valid */
	thread_queue_rwlock * thread = check_if_thread_in_queue_rwlock(
			&rwlock_identifier->head_queue,thread_id);

	if (thread == NULL)
	{
		SIPRINTF("Thread ID does not exist");
		task_vanish(-2);
	}
	
	/* Take the lock */
	mutex_lock(&(rwlock_identifier->global_lock));

	/* Signal with bias towards writes */
    
    if (thread -> type == WRITE)	
	{
		rwlock_identifier-> writecnt--;
		if (rwlock_identifier->downgrade_id == thread->thread_id)
			rwlock_identifier->downgrade_id = -1;
	}
	else 
		rwlock_identifier-> readcnt--;

	if (rwlock_identifier -> writecnt > 0)
	{
		cond_signal(&(rwlock_identifier -> write_cond));
	} else if (rwlock_identifier -> readcnt > 0)
	{
		cond_broadcast(&(rwlock_identifier->read_cond));
	}

	/* Remove the thread id from the queue */
	thread_queue_rwlock * temp = remove_thread_from_queue_rwlock(
			&(rwlock_identifier->head_queue),thread_id);

	/*Release the lock */

	mutex_unlock(&(rwlock_identifier->global_lock));

	/* Free the resource */
	free(temp);
}

/** @brief Rwlock downgrade 
 *  
 *  This does : 
 */


void rwlock_downgrade(rwlock_t * rwlock)
{
	SIPRINTF("Entering rwlock downgrade id:%d by tid %d ",
		(unsigned int)rwlock,gettid());
	/* get the rwlock id from the structure */
	unsigned int rwlock_id = GET_RWLOCK_ID(rwlock);
	/* Get the thread id of the thread */
	int thread_id = gettid();

	/* Get the rwlock object */
	rwlock_thread_object * rwlock_identifier = 
		get_rwlock_object_by_rwlock_id(rwlock_id);

	if ( rwlock_identifier == NULL)
	{
		SIPRINTF("Call rwlock init first \n");
		task_vanish(-2);
	}
	
	/* Check if the request is valid */
	thread_queue_rwlock * thread = check_if_thread_in_queue_rwlock(
			&rwlock_identifier->head_queue,thread_id);

	if (thread == NULL)
	{
		SIPRINTF("Thread ID does not exist");
		task_vanish(-2);
	}

	/* Check if flag is already set */
	if (rwlock_identifier->downgrade_id != -1)
	{
		SIPRINTF("Already set downgrade");
		return;
	}

	/* Check the type */

	if (thread -> type == WRITE)
	{
		rwlock_identifier->downgrade_id = gettid();
		cond_broadcast(&(rwlock_identifier->read_cond));
	} else 
	{
		SIPRINTF("Reader cannot downgrade ");
		return;
	}
}

/** @brief rwlock destroy 
 */

void rwlock_destroy(rwlock_t * rwlock)
{
	/* get the rwlock id from the structure */
	unsigned int rwlock_id = GET_RWLOCK_ID(rwlock);

	/* Get the rwlock object */
	rwlock_thread_object * rwlock_identifier = 
		get_rwlock_object_by_rwlock_id(rwlock_id);

	/* Check if the queue is empty or not */
	if (rwlock_identifier -> head_queue != NULL)
	{
		SIPRINTF("Threads are still in rwlock queue ");
		task_vanish(-2);
	}

	/* Remove the rwlock object from the list */
	rem_rwlock_object_by_rwlock_id(rwlock_id);
	/* Free the rwlock_object */

	free(rwlock_identifier);

}
