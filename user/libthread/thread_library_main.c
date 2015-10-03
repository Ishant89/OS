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
 *	@author Ishant & Shelton
 *
 *	@bug 
 */

#include<thr_internals.h>
#include "thr_private.h"
#include <thread.h>
#include<syscall.h>
#include<stdlib.h>
#include<simics.h>


int thr_init(unsigned int size)
{
	int flag;
	/* Stack Size */
	/* Check if size is multiple of PAGE_SIZE*/ 
	if ((size % WORD_SIZE)==0)
	{
		stack_size = size;
		flag = SUCCESS;
	}

	else
		flag = ERROR;

	tcb_lock = 0;
	/* Allocate parent TCB */
	void * tcb_mem = _calloc(1,TCB_SIZE);

	tcb name = (tcb) tcb_mem;
	/* EDIT: name-> sp, name->func, name->arg and name->waiter to be decided */
	name->kid = gettid();
	name->tid = name->kid;
	name->waiter = -1;
	name->children = NULL;
	name->lock_available = 0;
	insert_tcb_list(name);
	return flag;
}

/** @brief thr_create 
 *  
 *  This function creates the stack using new pages 
 *  and uses thread_fork system call to issue a new 
 *  task thread 
 *
 *  1. Currently using TID same as Kernel Issued ID
 *  
 *  @param func  function pointer 
 *  @param arg Argument pointer 
 *
 *  @return int Error code
 */
typedef void *(*func)(void*) ;

int thr_create( func handler, void * arg )
{
	SIPRINTF("Entering thr create");
	/* Create stack for the thread */
	/*EDIT: Replace 1 with macro */
	
	void * stack_tcb_mem = _calloc(1,(stack_size + TCB_SIZE + STACK_BUFFER));

	if(stack_tcb_mem == NULL)
	{
		SIPRINTF("Exiting thr_create with error : Could not allocate TCB and stack for thread\n");
		return THREAD_NOT_CREATED;
	}

	/* TCB done */
	register tcb name = (tcb) stack_tcb_mem;
	name->sp = (void*)((unsigned int)stack_tcb_mem + stack_size + TCB_SIZE);
	name->func = handler;
	name->arg = arg;
	name->waiter = -1;
	name->lock_available = 0;

	/*tcb current = get_tcb_from_kid(gettid());
	print_tcb_list();
	push_children(&(current -> children),name);
	*/

	/*Create kernel thread */
	int pid = thread_fork(name->sp);
	/* If child call the handler */
	if (!pid)
	{
		int my_pid = gettid();
		SIPRINTF("In thr_create and in child :%d",my_pid);
		int status;
		while(!(status = check_if_pid_exists_tcb(my_pid)))
		{
	    SIPRINTF("In thr_create : Child TCB found status %d",!status);
			yield(-1);
		}
		int result = (int)name->func(name->arg);
		thr_exit((void *)result);
	}

	name->kid = pid;
	name->tid = pid;
	insert_tcb_list(name);
	print_tcb_list();
	SIPRINTF("Exting thr_creat with Success : Done creating thread: %d",pid);
	return pid;
}

void print_tcb_list()
{
	while(compAndXchg((void *)&(tcb_lock),0,1));
	tcb temp = tcb_head;

	while(temp != NULL)
	{
		SIPRINTF("TCB: %p Kid: %d Tid: %d",temp,temp->tid,temp->kid);
		temp = temp->next;
	}

	tcb_lock = 0;
}

int thr_join( int tid, void **statusp)
{
	SIPRINTF("Entring join with tid: %d",tid);

	int reject = 0;

	tcb child = get_tcb_from_tid(tid);

	if(child == NULL)
	{
		SIPRINTF("Exiting join with error : Cannot find child TCB %d in join",tid);
		return THREAD_NOT_CREATED;
	}

	while(compAndXchg((void *)&(child -> lock_available),0,1));

	if(isDone(child))
	{
		if(statusp != NULL)
		 *statusp = child -> exit_status;
		remove_tcb_from_list(child);
		print_tcb_list();
		child -> lock_available = 0;
		_free(child);
		SIPRINTF("Exiting join  with success without wait and tid: %d",tid);
		return 0;
	}

	else
	{
		child -> waiter = gettid();
		child -> lock_available = 0;

		if(deschedule(&reject) < 0)
			SIPRINTF("Cannot reschdule parent!\n");

		while(compAndXchg((void *)&(child -> lock_available),0,1));
		if(statusp != NULL)
		 *statusp = child -> exit_status;
		remove_tcb_from_list(child);
		print_tcb_list();
		child -> lock_available = 0;
		_free(child);
		SIPRINTF("Exiting join with success with wait and tid: %d",tid);
		return 0;

	}

}

void thr_exit( void *status )
{

	tcb current = get_tcb_from_kid(gettid());

	SIPRINTF("Entring exit with tid: %d",current->tid);

	while(compAndXchg((void *)&(current -> lock_available),0,1));

	current -> exit_status = status;

	if(current -> waiter != -1)
	{
		while(make_runnable(current -> waiter) < 0);
		SIPRINTF("Made runnable %d by %d",current -> waiter,current -> tid);
	}

	else
		setDone(current);

	SIPRINTF("Exiting thr_exit with tid is : %d",current -> tid);

	current -> lock_available = 0;

	vanish();
}

int thr_yield( int tid )
{
	if(tid == -1)
		yield(tid);
	else
	{
		tcb thread_yield = get_tcb_from_tid(tid);
		if(thread_yield == NULL)
			return THREAD_NOT_CREATED;
		if (yield(thread_yield -> kid) < 0)
			return THREAD_NOT_CREATED;
	}

	return 0;
}

int thr_getid( void )
{
	int kid = gettid();
	tcb current = get_tcb_from_kid(kid);
	return current -> tid;
}

void insert_tcb_list(tcb entry)
{
	while(compAndXchg((void *)&(tcb_lock),0,1));
	tcb temp = tcb_head;
	tcb_head = entry;
	entry-> next = temp;
	tcb_lock = 0;
}

void remove_tcb_from_list(tcb entry)
{
	while(compAndXchg((void *)&(tcb_lock),0,1));
	tcb temp = tcb_head;
	tcb prev = NULL;
	while(temp != entry && temp)
	{
		prev = temp;
		temp = temp ->next;
	}

	if(prev)
		prev -> next = temp;
	else
		tcb_head = temp -> next;

	tcb_lock = 0;
		
}

tcb get_tcb_from_tid(int pid)
{
	while(compAndXchg((void *)&(tcb_lock),0,1));
	tcb temp = tcb_head;
	while(temp != NULL)
	{

		if(temp -> tid == pid)
		{
			tcb_lock = 0;
			return temp;
		}

		else
			temp = temp -> next;
	}

	tcb_lock = 0;
	return TCB_NOT_FOUND;
}

tcb get_tcb_from_kid(int kid)
{
	while(compAndXchg((void *)&(tcb_lock),0,1));
	tcb temp = tcb_head;
	while(temp != NULL)
	{

		if(temp -> kid == kid)
		{
			tcb_lock = 0;
			return temp;
		}

		else
			temp = temp -> next;
	}

	tcb_lock = 0;
	return TCB_NOT_FOUND;
}

int check_if_pid_exists_tcb(int tid)
{
	while(compAndXchg((void *)&(tcb_lock),0,1));
	tcb temp = tcb_head;
	SIPRINTF("Searching TCB from Head :%p",temp);
	while(temp != NULL)
	{

		SIPRINTF("Temp id: %d True id %d",temp->tid,tid);
		if(temp -> tid == tid)
		{
			tcb_lock = 0;
			return 1;
		}

		else
			temp = temp -> next;
	}

	tcb_lock = 0;
	return 0;
}

void push_children(children_list **head,tcb child)
{
	children_list *tl = _calloc(1,sizeof(children_list));
	tl -> next = *head;
	tl -> name = child;
	*head = tl;
}

void remove_children(children_list **head,tcb child)
{
	children_list *temp = *head;
	children_list *prev = NULL;
	while(temp -> name != child && temp)
	{
		prev = temp;
		temp = temp -> next;
	}

	if(prev)
		prev -> next = temp;
	else
		*head = NULL;

	free_child_thread_list(temp);
}


void free_child_thread_list(children_list * head)
{
	children_list * next;
	while(head)
	{
		next = head -> next;
		_free(head);
		head = next;
	}
}

void freeThread(tcb thread)
{
	if (thread->waiter != -1) {
    SIPRINTF("Thread %p exits while waiting on something\n", thread);
  }

  free_child_thread_list(thread->children);
  _free(thread);
}
