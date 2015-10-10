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

//#define DEBUG 0
#include <autostack.h>
#include <thr_internals.h>
#include <simics.h>
#include <syscall.h>
#include <stdlib.h>


void autostack_handler_t(void *arg, ureg_t *ureg)
{
  SIPRINTF("In swexn Handler!!\n");
  SIPRINTF("CR2: %x  Low ptr: %p EIP: %x Reason: %d Sp: %x Error code: %d\n",ureg->cr2,stack_low_ptr,ureg->eip,ureg->cause,ureg->esp,ureg->error_code);

  char *fault_reason = get_fault_reason(ureg -> cause);

  if((ureg -> esp > (unsigned int)stack_low_ptr) || (ureg -> cause != SWEXN_CAUSE_PAGEFAULT))
  {

    panic("\n-------------Autostack handler only takes cares of page faults-------------- \
          \n Cause for failure : %s \
          \n Faulting address : 0x%x \
          \n Instruction pointer at fault location : 0x%x", 
          fault_reason, ureg -> cr2, ureg -> eip);
    task_vanish(ureg -> error_code);
  }

  unsigned long extend_stack_size = GET_EXTENDED_STACK_BASE(stack_low_ptr,ureg->esp);

  SIPRINTF("Stack size: %ld",extend_stack_size);

  stack_low_ptr = GET_STACK_LOW_ADDRESS((unsigned int)stack_low_ptr,extend_stack_size);

  if(new_pages(stack_low_ptr,extend_stack_size) < 0)
  {
    panic("New Pages error");
    err_num = NEW_PAGES_ERROR;
	  task_vanish(err_num);
    return;
  }

  if (swexn(autostack_addr_high, autostack_handler_t , NULL, ureg) < 0)
  {
    SIPRINTF("Cannot register SWEXN handler\n");
    err_num = SWEXN_INSTALL_ERROR;
	  task_vanish(err_num);
    return;
  }
  return;
}

void
install_autostack(void *stack_high, void *stack_low)
{
  stack_high_ptr = stack_high;
  stack_low_ptr = stack_low;

  
  autostack_addr_low = _malloc(EXCEPTION_STACK_SIZE);

  autostack_addr_high = GET_STACK_HIGH((unsigned int)autostack_addr_low,EXCEPTION_STACK_SIZE);

  if (swexn(autostack_addr_high, autostack_handler_t , NULL, NULL) < 0)
  {
    SIPRINTF("Cannot register SWEXN handler\n");
    err_num = SWEXN_INSTALL_ERROR;
    return;
  }

}

/** @brief deregister automatic stack growth handler 
 *   
 *  This function deregisters the handler for automatic stack
 * growth.
 *
 * @param 
 */

 void deregister_autostack_handler()
 {

 
  /* De-registering the previous handler */
  if (swexn(NULL,NULL,NULL,NULL)<0)
  {
    SIPRINTF("Unable to deregister previous handler ");
    return;
  }

  /* Free the exception stack for allocated for automatic stack growth */
  free(autostack_addr_low);

  SIPRINTF("Handler deregistered successfully");

 }

/** @brief helper function for panic
 *   
 *  This function is a helper function for panic.
 *
 * @param 
 */
char *get_fault_reason(int exception_number)
{
  char *buf;
  switch(exception_number)
  {
    case SWEXN_CAUSE_DIVIDE :  
      buf = "DIVIDE ERROR";
      break;
    case SWEXN_CAUSE_DEBUG :
      buf = "DEBUG ERROR";
      break;
    case SWEXN_CAUSE_BREAKPOINT : 
      buf = "BREAKPOINT ERROR";
      break;
    case SWEXN_CAUSE_OVERFLOW :   
      buf = "OVERFLOW ERROR";
      break;
    case SWEXN_CAUSE_BOUNDCHECK :  
      buf = "BOUNDCHECK ERROR";
      break;
    case SWEXN_CAUSE_OPCODE :   
      buf = "OPCODE ERROR";
      break;
    case SWEXN_CAUSE_NOFPU :  
      buf = "FPU MISSING"; 
      break;
    case SWEXN_CAUSE_SEGFAULT :
      buf = "SEGMENTATION FAULT";
      break;
    case SWEXN_CAUSE_STACKFAULT :
      buf = "STACK FAULT"; 
      break;
    case SWEXN_CAUSE_PROTFAULT :  
      buf = "PROTECTION FAULT";  
      break;
    case SWEXN_CAUSE_PAGEFAULT :  
      buf = "PAGE FAULT";
      break;
    case SWEXN_CAUSE_FPUFAULT :   
      buf = "FPU FAULT";
      break;
    case SWEXN_CAUSE_ALIGNFAULT : 
      buf = "ALIGNMENT ERROR";
      break;
    case SWEXN_CAUSE_SIMDFAULT :  
      buf = "SIMD FAULT";
      break;
    default :
      buf = "UNKNOWN";
      break;
  }

  return buf;

}




