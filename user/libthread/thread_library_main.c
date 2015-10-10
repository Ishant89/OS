/** @file thread_library_main.c
 *  
 *  @brief Thread library API definitions 
 *   
 *  This file contains definitions of the user thread library 
 *  APIs
 *  1. thr_init()
 *  2. thr_create()
 *  3. thr_join()
 *  4. thr_exit()
 *  5. thr_getid()
 *  6. thr_yield()
 *	
 *	@author Ishant(idawer) & Shelton(sdsouza)
 *
 *	@bug  No bugs found
 */

#include <thr_internals.h>/*Internal macros */
#include "thr_private.h" /* TCB structure */
#include "mm_new_pages.h" /* New pages alloc */
#include <thread.h> /* Thread lib functions */
#include <syscall.h>/* Syscall stubs */
#include <stdlib.h> /*Std lib*/
#include<autostack.h> /*Auto stack handler*/
#include<thread_crash_handler.h> /* Thread crash handler*/

/** @brief Initializes the thread library
 *
 *
 * This function initializes the thread library. It does the following
 * functions :
 *
 * 1. Initiaize the mutexes for malloc,free and the new pages allocator.
 * 2. Deregister automatic stack growth.
 * 3. Allocate the tcb for the parent and insert it into the tcb global list.
 * 4. Switch from single-threaded to multi-threaded env
 *
 *  @param size
 *
 *  @return int Error code
 */

int thr_init(unsigned int size)
{
	/* Init malloc lock */
	mutex_init(&alloc_lock);
	mm_init_new_pages(size);
	/* Deregister autostack handler */
	deregister_autostack_handler();
	mutex_init(&(tcb_lock));
	
	/* Allocate parent TCB */
	void * tcb_mem = new_pages_malloc();

	if(tcb_mem == NULL)
	{
		panic("Exiting thr_create with error : \
				Could not allocate TCB and stack for thread\n");
		return FAIL;
	}

	/* Setting the stack size for threads */
	stack_size = size;	
	tcb parent = (tcb) tcb_mem;
	parent->kid = gettid(); /* Kernel thrd id */
	parent->tid = parent->kid;/*for 1:1, kid and thread id same*/
	
	/* 
	 * Child thread will make join runnable using waiter value
	 * This will have thread id of the process to 
	 * join (by def :-1) 
	 */
	parent->waiter = -1;

	/* Parent's crash handler stack */
	parent -> crash_handler_sp = (void*)((unsigned int)tcb_mem + 
			TCB_SIZE + STACK_BUFFER + CRASH_HANDLER_STACK_SIZE);
	
	/* Install thread crash handler for the parent */
	if(install_thread_crash_handler(parent -> crash_handler_sp) < 0)
	{
		panic("Unable to install thread crash handler");
		return FAIL;
	}
	insert_tcb_list(parent);
	return PASS;
}

/** @brief Create a thread
 *
 * This function creates a thread . It does the following
 * functions :
 *
 *  1. Allocate tcb and stack for the new thread.
 *  2. Initialize mutex and cond var for the new thread.
 *  3. Install the thread crash handler for the new thread.
 *  4. Stall the child's execution till the parent inserts its tcb
 *     into the list.
 *
 *  @param func  function pointer
 *  @param arg Argument pointer
 *
 *  @return int pid of creted child else error code.
 */


int thr_create( func handler, void * arg )
{
	/* Create stack for the thread */
	void * stack_tcb_mem = new_pages_malloc();

	if(stack_tcb_mem == NULL)
	{
		panic("Exiting thr_create with error : \
				Could not allocate TCB and stack for thread\n");
		return FAIL;
	}
	
	/* TCB details 
	 * TCB pointer is saved in register because we want to avoid local 
	 * thread (parent) variables access in child context.
	 * and the following tcb is for newly thread created as follows
	 */
	register tcb name = (tcb) stack_tcb_mem;
	name -> sp = (void*)((unsigned int)stack_tcb_mem + stack_size + TCB_SIZE);
	name -> func = handler;
	name -> creator_tid= gettid();
	name -> arg = arg;
	name -> waiter = -1;
	name -> crash_handler_sp = (void*)((unsigned int)stack_tcb_mem + 
			TCB_SIZE + STACK_BUFFER + stack_size + CRASH_HANDLER_STACK_SIZE);
	/*Create kernel thread */
	int pid = thread_fork(name->sp);
	/* If child call the handler */
	if (!pid)
	{
		/* Install the thread crash handler */
		if (install_thread_crash_handler(name -> crash_handler_sp) < 0)
		{
			panic("Unable to install thread crash handler");
			return FAIL;
		}
		int my_pid = gettid();
		int status;
		while(!(status = check_if_pid_exists_tcb(my_pid)))
		{
			/* Yield to parent till child tcb is in the tcb list*/
			yield(name->creator_tid);
		}
		/* exec child handler */
		int result = (int)name->func(name->arg);
		/* Terminating child after above handler RETURNS with or wo
		 * doing thr_exit in the handler
		 */
		thr_exit((void *)result);
	}

	name->kid = pid;
	name->tid = pid;
	insert_tcb_list(name);
	return pid;
}

/** @brief thread join 
 *  
 *  This function is invoked to wait for child until child 
 *  exits 
 *  It does following things : 
 *  1. Gets the tcb from the global list 
 *  2. Checks the child info from the tcb 
 *  3. If child is done, it reclaims the child 
 *  stack, tcb and exception stack area
 *  4. If child not done,it waits for child to exit(vanish)
 *  
 *  @param tid Thread id to wait for 
 *  @param statusp set status set by child upon termination
 *
 *  @return int thread id of the child
 */
int thr_join( int tid, void **statusp)
{
	/*deschedule condition */
	int reject =0;

	/* get tcb*/
	tcb child = get_tcb_from_tid(tid);

	if(child == NULL)
	{
		panic("Exiting join with error : Cannot find child TCB %d in join",tid);
		return FAIL;
	}

	while(compAndXchg((void *)&(child -> private_lock),0,1))
	{
		/* Yield to the child if lock between child and parent is 
		 * used by child */
		yield(tid);
	}

	/* Check if child is done or not */
	if(isDone(child))
	{
		/* Setting statusp if not null*/
		if(statusp != NULL)
			*statusp = child -> exit_status;
		remove_tcb_from_list(child);
		/* Release the lock*/
		child->private_lock = 0;
		/* Free the child memory structures*/
		free_child_data_structures(child);
		return PASS;
	}

	child -> waiter = gettid();
	child->private_lock = 0;
	if(deschedule(&reject) < 0)
	{
		return FAIL;
	}
/* Take the lock */
	while(compAndXchg((void *)&(child -> private_lock),0,1))
	{
		yield(tid);
	}
	if(statusp != NULL)
		*statusp = child -> exit_status;
	/* Remove tcb from the list*/
	remove_tcb_from_list(child);
	/* Release the lock */
	child->private_lock = 0;
	free_child_data_structures(child);
	return PASS;
}
/** @brief Thread exit 
 *  
 *  This function is invoked by the thread upon termination
 *  This function does following : 
 *  1. It first tries to get the lock shared between creator 
 *  and itself 
 *  2. It checks if there is a waiter from the tcb 
 *  3. It makes the waiter runnable 
 *  4. If no waiter, it sets a 'DONE' flag to notify waiter to 
 *  take back its resources
 *
 *  @param status Status upon exit 
 *
 *  @return void 
 */
void thr_exit( void *status )
{

	tcb current = get_tcb_from_kid(gettid());

/*Take the lock */
	while(compAndXchg((void *)&(current -> private_lock),0,1));

	current -> exit_status = status;
/*check for waiter */
	if(current -> waiter != -1)
	{
		/* Set the flag */
		setDone(current);
		/* Make runnable the join thread 
		 * To avoid race between deschedule in join and 
		 * following instruction, it is done in a spin 
		 */
		while (make_runnable(current->waiter) < 0)
		{
			/* Yield to join thread */
			yield(current->waiter);
		}
	}
	else
		setDone(current);
	/* Following vanish stub is little different from 
	 * the usual vanish system call.
	 * This is done to avoid another function call from the child
	 * after the lock is released as there is a race between join thread 
	 * (which will reclaim child stack) and child thread to call vanish.
	 * If join thread succeeds, stack is taken back and child will execute
	 * usual vanish system call on an illegal stack.
	 * To avoid this scenario:
	 * we call the following function which includes following:
	 * 1. Release the lock
	 * 2. Call vanish system call using INT
	 *
	 * Advantage: System call to vanish on corrupt stack is avoided
	 * Refer more in the documentation
	 */
	vanish_thread_exit(&(current->private_lock));
}

/** @brief thread yield
 *  
 *  This function yields to another thread
 *  It does following:
 *  1. If tid is -1; yield to unspecified thread
 *  2. else, yield to with thread as tid
 *  
 *  @param tid thread's tid to be yielded
 *  @return int status 0 if success,-1 on failure
 */
int thr_yield( int tid )
{
	if(tid == -1)
		yield(tid);
	else
	{
		tcb thread_yield = get_tcb_from_tid(tid);
		if(thread_yield == NULL)
			return FAIL;
		if (yield(thread_yield -> kid) < 0)
			return FAIL;
	}
	return PASS;
}
/** @brief get the user thread id 
 *  
 *  This function gets the current thread id in execution
 *  It does following:
 *  1. In 1:1 user thread library,thread ids are same as user
 *  ids
 *
 *  @param void 
 *  @return int current thread id
 */
int thr_getid( void )
{
	int kid = gettid();
	tcb current = get_tcb_from_kid(kid);
	return current -> tid;
}

/** @brief Insert tcb to the linked list
 *  
 *  This function is used to append the tcb in the
 *  front of the list
 *
 *  @param entry TCB structure
 *
 */

void insert_tcb_list(tcb entry)
{
	/* Take the lock */
	mutex_lock(&tcb_lock);
	tcb temp = tcb_head;
	tcb_head = entry;
	entry-> next = temp;
	/* Release the lock*/
	mutex_unlock(&tcb_lock);
}

/** @brief remove tcb from the linked list
 *  
 *  This function is used to remove the tcb from
 *  the list
 *
 *  @param entry TCB structure
 *
 */

void remove_tcb_from_list(tcb entry)
{
	/* Take the lock */
	mutex_lock(&tcb_lock);
	tcb temp = tcb_head;
	tcb prev = NULL;
	while(temp != entry && temp)
	{
		prev = temp;
		temp = temp ->next;
	}

	if(prev)
		prev -> next = temp -> next;
	else
		tcb_head = temp -> next;

	/* Release the lock*/
	mutex_unlock(&tcb_lock);
		
}

/** @brief get tcb from the linked list
 *  
 *  This function is used to get the tcb from
 *  the list by user thread id
 *
 *  @param pid user thread id
 *  @return tcb tcb structure
 *
 */

tcb get_tcb_from_tid(int pid)
{
	mutex_lock(&tcb_lock);
	tcb temp = tcb_head;
	while(temp != NULL)
	{

		if(temp -> tid == pid)
		{
			mutex_unlock(&tcb_lock);
			return temp;
		}

		else
			temp = temp -> next;
	}

	mutex_unlock(&tcb_lock);
	return NULL;
}

/** @brief get tcb from the linked list
 *  
 *  This function is used to get the tcb from
 *  the list by kernel thread id
 *
 *  @param kid kernel thread id
 *  @return tcb tcb structure
 *
 */

tcb get_tcb_from_kid(int kid)
{
	mutex_lock(&tcb_lock);
	tcb temp = tcb_head;
	while(temp != NULL)
	{

		if(temp -> kid == kid)
		{
			mutex_unlock(&tcb_lock);
			return temp;
		}

		else
			temp = temp -> next;
	}

	mutex_unlock(&tcb_lock);
	return NULL;
}

/** @brief check if pid's tcb exists
 *  
 *  This function is used to check if the tcb for
 *  a pid exists in the tcb list 
 *
 *  @param tid user thread id
 *  @return int Fail/Success
 *
 */

int check_if_pid_exists_tcb(int tid)
{
	mutex_lock(&tcb_lock);
	tcb temp = tcb_head;
	while(temp != NULL)
	{

		if(temp -> tid == tid)
		{
			mutex_unlock(&tcb_lock);
			return RETVAL_1;
		}

		else
			temp = temp -> next;
	}

	mutex_unlock(&tcb_lock);
	return RETVAL_0;
}

/** @brief frees the child data structures
 *  
 *
 *  This function is used to free the child tcb
 *   child structures:
 *   1.  Child stack
 *   2. Exception stack 
 *   3. tcb 
 *  @param tcb tcb
 *
 */
void free_child_data_structures(tcb child)
{
	free_pages(child);
}
