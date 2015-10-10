/** @file thread_crash_handler.c 
 *  
 *  @brief handle thread crash 
 *
 *  This installs handler to handle thread crash 
 *
 *  @author Ishant & Shelton
 *  @bug No known bugs
 */
#define DEBUG 0
#include <syscall.h>
#include <malloc.h>

/*EDIT:Simics.h */
#include <simics.h>
#include <thr_internals.h>
#include <thread_crash_handler.h>
#include<syscall.h>
#include<malloc.h>

/*EDIT:Simics.h */
#include<simics.h>
#include<thr_internals.h>

#define EXCEPTION_STACK_SIZE 256
#define FAIL -1
#define PASS 0

/** @brief thread crash handler 
 *  
 *  This function handles the exception 
 *
 *  @param arg Arguments 
 *  @param ureg register list
 */

void thread_crash_handler(void * arg,ureg_t * ureg)
{
	char *fault_reason = get_fault_reason(ureg -> cause);

	panic("\n-------------Thread %d Crashed-------------- \
          \n Cause for failure : %s \
          \n Faulting address : 0x%x \
          \n Instruction pointer at fault location : 0x%x", 
          gettid(),fault_reason, ureg -> cr2, ureg -> eip);
	
	task_vanish(ureg -> error_code);
}

/** @brief Install thread crash handler 
 *  
 *  This function installs the thread crash handler 
 *  This should be called from thr_init
 *  This will deregister the previous handler and install
 *  a new one.
 */

int install_thread_crash_handler(void *exception_stack)
{

	/* Defining blank new regeister set */
	SIPRINTF("Entering install thread crash handler");
	ureg_t * newureg = NULL;

	/*Allocating an exception stack */
	void * exception_stack = malloc(EXCEPTION_STACK_SIZE);

	SIPRINTF("stack : %p",exception_stack);
	if (exception_stack == NULL)
	{
		SIPRINTF("Unable to allocate stack ");
		return FAIL;
	}

	/* De-registering the previous handler */
	if (swexn(NULL,NULL,NULL,newureg)<0)
	{
		SIPRINTF("Unable to deregister previous handler ");
		return FAIL;
	}
	SIPRINTF("Handler deregistered successfully");
	/* Installing the handler */

	if (swexn(exception_stack,thread_crash_handler,NULL,newureg)<0)
	{
		SIPRINTF("Unable to register handler ");
		return FAIL;
	}
	SIPRINTF("New handler registered successfully");
	SIPRINTF("Exiting install thread crash handler");
	return PASS;
}
