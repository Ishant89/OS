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

	/* Allocate parent TCB */
	void * tcb_mem = _calloc(1,TCB_SIZE);

	tcb name = (tcb) tcb_mem;
	/* EDIT: name-> sp, name->func, name->arg and name->waiter to be decided */
	name->kid = gettid();
	name->tid = name->kid;
	name->waiter = -1;
	name->children = NULL;
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
	/* Create stack for the thread */
	/*EDIT: Replace 1 with macro */
	
	void * stack_tcb_mem = _calloc(1,(stack_size + TCB_SIZE + STACK_BUFFER));

	/* TCB done */
	register tcb name = (tcb) stack_tcb_mem;
	name->sp = (void*)((unsigned int)stack_tcb_mem + stack_size + TCB_SIZE);
	name->func = handler;
	name->arg = arg;
	name->waiter = -1;
	tcb current = get_tcb_from_kid(gettid());
	print_tcb_list();
	push_children(&(current -> children),name);
	/*Create kernel thread */
	int pid = thread_fork(name->sp);
	/* If child call the handler */
	if (!pid)
	{
		int tid = (int)name->func(name->arg);
		thr_exit((void *)tid);
	}

	name->kid = pid;
	name->tid = pid;
	insert_tcb_list(name);
	print_tcb_list();
	return pid;
}

void print_tcb_list()
{
	tcb temp = tcb_head;
	while(temp != NULL)
	{
		lprintf("TCB: %p Kid: %d Tid: %d",temp,temp->tid,temp->kid);
		temp = temp->next;
	}
}

int thr_join( int tid, void **statusp)
{
	int reject = 0;
	tcb child = get_tcb_from_tid(tid);

	if(child == NULL)
		return THREAD_NOT_CREATED;

	if(isDone(child))
	{
		if(statusp != NULL)
		 *statusp = child -> exit_status;
		remove_tcb_from_list(child);
		_free(child);
		return 0;
	}

	else
	{
		child -> waiter = gettid();

		if(deschedule(&reject) < 0)
			lprintf("Cannot reschdule parent!\n");

		if(statusp != NULL)
		 *statusp = child -> exit_status;
		remove_tcb_from_list(child);
		_free(child);
		return 0;

	}

}

void thr_exit( void *status )
{
	tcb current = get_tcb_from_kid(gettid());

	current -> exit_status = status;

	if(current -> waiter != -1)
	{
		if(make_runnable(current -> waiter) < 0)
			lprintf("Cannot run parent again\n");
	}

	else
		setDone(current);

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
	tcb temp = tcb_head;
	tcb_head = entry;
	entry-> next = temp;
}

void remove_tcb_from_list(tcb entry)
{
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
		
}

tcb get_tcb_from_tid(int pid)
{
	tcb temp = tcb_head;
	while(temp != NULL)
	{

		if(temp -> tid == pid)
			return temp;

		else
			temp = temp -> next;
	}

	return TCB_NOT_FOUND;
}

tcb get_tcb_from_kid(int kid)
{
	tcb temp = tcb_head;
	while(temp != NULL)
	{

		if(temp -> kid == kid)
			return temp;

		else
			temp = temp -> next;
	}

	return TCB_NOT_FOUND;
}

void push_children(children_list **head,tcb child)
{
	children_list *tl = _calloc(1,sizeof(children_list));
	tl -> next = *head;
	tl -> name = child;
	*head = tl;
}

/*void remove_children(children_list **head,tcb child)
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
}*/


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
    lprintf("Thread %p exits while waiting on something\n", thread);
  }

  free_child_thread_list(thread->children);
  _free(thread);
}
