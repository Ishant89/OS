/** @file autostack.c
 *  
 *  @brief Implementation for automatic stack growth
 *   
 *  This file is resposible for registering a handler which
 *  will be used to grow the main thread stack automatically
 *  if required in case of a page fault.
 *
 *  @author Ishant & Shelton
 *
 *  @bug 
 */

#include <autostack.h>
#include <thr_internals.h>
#include <simics.h>
#include <syscall.h>
#include <stdlib.h>

void * stack_high_ptr;
void * stack_low_ptr;

void * exception_stack_high;

void handler_t(void *arg, ureg_t *ureg)
{
  lprintf("In swexn Handler!!\n");
  lprintf("CR2: %x  Low ptr: %p EIP: %x Reason: %d\n",ureg->cr2,stack_low_ptr,ureg->eip,ureg->cause);

  if(ureg -> cause != 14)
    task_vanish(-2);

  unsigned long extend_stack_size = GET_EXTENDED_STACK_BASE(stack_low_ptr,ureg->cr2);

  lprintf("Stack size: %ld",extend_stack_size);

  stack_low_ptr = GET_STACK_LOW_ADDRESS((unsigned int)stack_low_ptr,extend_stack_size);

  if(new_pages(stack_low_ptr,extend_stack_size) < 0)
  {
    lprintf("New pages error\n");
    err_num = NEW_PAGES_ERROR;
    return;
  }

  if (swexn(exception_stack_high, handler_t , NULL, ureg) < 0)
  {
    lprintf("Cannot register SWEXN handler\n");
    err_num = SWEXN_INSTALL_ERROR;
    return;
  }
  //resume_thread(ureg);
  return;
}

void
install_autostack(void *stack_high, void *stack_low)
{
  stack_high_ptr = stack_high;
  stack_low_ptr = stack_low;

  /*void * exception_stack_base = GET_STACK_LOW_ADDRESS((unsigned int)stack_low,20 * EXCEPTION_STACK_SIZE);

  if(new_pages(exception_stack_base,20 * EXCEPTION_STACK_SIZE) < 0)
  {
    lprintf("New pages error\n");
    err_num = NEW_PAGES_ERROR;
    return;
  }

  //void *arg = stack_low;
  //ureg_t * newreg = _calloc(1,sizeof(ureg_t));
  if (swexn(stack_low, handler_t , NULL, NULL) < 0)
  {
    lprintf("Cannot register SWEXN handler\n");
    err_num = SWEXN_INSTALL_ERROR;
    return;
  }

  stack_high_ptr = stack_low;
  stack_low_ptr = exception_stack_base;*/

  void * exception_stack_base= _malloc(EXCEPTION_STACK_SIZE);

  exception_stack_high = GET_STACK_HIGH((unsigned int)exception_stack_base,EXCEPTION_STACK_SIZE);

  if (swexn(exception_stack_high, handler_t , NULL, NULL) < 0)
  {
    lprintf("Cannot register SWEXN handler\n");
    err_num = SWEXN_INSTALL_ERROR;
    return;
  }



}




