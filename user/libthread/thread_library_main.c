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
#define DEBUG 0
#include <thr_internals.h>
#include "thr_private.h"
#include "mm_new_pages.h"
#include <thread.h>
#include <syscall.h>
#include <stdlib.h>
#include <simics.h>


int thr_init(unsigned int size)
{
	mm_init_new_pages(size);
	mutex_init(&(tcb_lock));
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
	void * tcb_mem = new_pages_malloc();

	if(tcb_mem == NULL)
	{
		SIPRINTF("Exiting thr_create with error : Could not allocate TCB and stack for thread\n");
		return THREAD_NOT_CREATED;
	}

	tcb name = (tcb) tcb_mem;
	/* EDIT: name-> sp, name->func, name->arg and name->waiter to be decided */
	name->kid = gettid();
	name->tid = name->kid;
	name->waiter = -1;
	mutex_init(&(name -> private_lock));
	cond_init(&(name -> exit_cond));
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
	
	//void * stack_tcb_mem = calloc(1,(stack_size + TCB_SIZE + STACK_BUFFER));

	void * stack_tcb_mem = new_pages_malloc();

	if(stack_tcb_mem == NULL)
	{
		SIPRINTF("Exiting thr_create with error : Could not allocate TCB and stack for thread\n");
		return THREAD_NOT_CREATED;
	}

	/* TCB done */
	register tcb name = (tcb) stack_tcb_mem;
	name -> sp = (void*)((unsigned int)stack_tcb_mem + stack_size + TCB_SIZE);
	name -> func = handler;
	name -> arg = arg;
	name -> waiter = -1;
	mutex_init(&(name -> private_lock));
	cond_init(&(name -> exit_cond));
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
	    //SIPRINTF("In thr_create : Child TCB found status %d",!status);
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
	mutex_lock(&tcb_lock);
	tcb temp = tcb_head;

	while(temp != NULL)
	{
		SIPRINTF("TCB: %p Kid: %d Tid: %d",temp,temp->tid,temp->kid);
		temp = temp->next;
	}

	mutex_unlock(&tcb_lock);
}

int thr_join( int tid, void **statusp)
{
	SIPRINTF("Entring join with tid: %d",tid);

	tcb child = get_tcb_from_tid(tid);

	if(child == NULL)
	{
		SIPRINTF("Exiting join with error : Cannot find child TCB %d in join",tid);
		return THREAD_NOT_CREATED;
	}

	mutex_lock(&(child -> private_lock));

	if(isDone(child))
	{
		if(statusp != NULL)
		 *statusp = child -> exit_status;
		remove_tcb_from_list(child);
		mutex_unlock(&(child -> private_lock));
	  free_child_data_structures(child);
		SIPRINTF("Exiting join  with success without wait and tid: %d",tid);
		return 0;
	}

	else
	{
		child -> waiter = gettid();

		while(!isDone(child))
		{
			cond_wait(&(child -> exit_cond),&(child -> private_lock));
		}

		if(statusp != NULL)
		 *statusp = child -> exit_status;
		remove_tcb_from_list(child);
		mutex_unlock(&(child -> private_lock));
		free_child_data_structures(child);
		SIPRINTF("Exiting join with success with wait and tid: %d",tid);
		return 0;

	}

}

void thr_exit( void *status )
{

	tcb current = get_tcb_from_kid(gettid());

	SIPRINTF("Entring exit with tid: %d",current->tid);

	mutex_lock(&(current -> private_lock));

	current -> exit_status = status;

	if(current -> waiter != -1)
	{
		setDone(current);
		cond_signal(&(current -> exit_cond));
	}

	else
		setDone(current);

	SIPRINTF("Exiting thr_exit with tid is : %d",current -> tid);

	mutex_unlock(&(current -> private_lock));

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
	mutex_lock(&tcb_lock);
	tcb temp = tcb_head;
	tcb_head = entry;
	entry-> next = temp;
	mutex_unlock(&tcb_lock);
}

void remove_tcb_from_list(tcb entry)
{
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

	mutex_unlock(&tcb_lock);
		
}

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
	return TCB_NOT_FOUND;
}

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
	return TCB_NOT_FOUND;
}

int check_if_pid_exists_tcb(int tid)
{
	mutex_lock(&tcb_lock);
	tcb temp = tcb_head;
	while(temp != NULL)
	{

		if(temp -> tid == tid)
		{
			mutex_unlock(&tcb_lock);
			return 1;
		}

		else
			temp = temp -> next;
	}

	mutex_unlock(&tcb_lock);
	return 0;
}

void free_child_data_structures(tcb child)
{
	mutex_destroy(&(child -> private_lock));
	cond_destroy(&(child -> exit_cond));
	free_pages(child);
}


