/** @file autostack.c
 *  
 *  @brief Implementation for automatic stack growth
 *   
 *  This file is resposible for registering a handler which
 *  will be used to grow the main thread stack automatically
 *  if required in case of a page fault.
 *
 *  @author Ishant(idawer) & Shelton(sdsouza)
 *
 *  @bug No known bugs
 */

#include <autostack.h> 
#include <thr_internals.h>
#include <syscall.h>
#include <stdlib.h>

/** @brief Automatic stack growth handler
 *  
 *  This function is called when an exception is encountered in 
 *  a single-threaded application. It take cares of only page faults
 *  realted to automatic stack growth.
 *
 *  @param arg Arguments 
 *  @param ureg register list
 *  @return Void
 */

void autostack_handler_t(void *arg, ureg_t *ureg)
{
  /* Get the fault reason */
  char *fault_reason = get_fault_reason(ureg -> cause);

  /* Vanish the task if the stack pointer when the fault occured was greater than 
   * the current stack_low or the cause was not a pagefault.
   */
  if((ureg -> esp > (unsigned int)stack_low_ptr) || 
    (ureg -> cause != SWEXN_CAUSE_PAGEFAULT))

  {

    panic("\n-------------Autostack handler only takes cares of page faults-- \
          ------------ \
          \n Cause for failure : %s \
          \n Faulting address : 0x%x \
          \n Instruction pointer at fault location : 0x%x", 
          fault_reason, ureg -> cr2, ureg -> eip);
    task_vanish(ureg -> error_code);

  }

  /* Calculate the required size for which the stack has to be grown further */
  unsigned long extend_stack_size = GET_EXTENDED_STACK_BASE(stack_low_ptr,
                                                            ureg->esp);

  /* Get the new stack low */
  stack_low_ptr = GET_STACK_LOW_ADDRESS((unsigned int)stack_low_ptr,
                                                      extend_stack_size);

  /* Allocate the extended memory using new pages */
  if(new_pages(stack_low_ptr,extend_stack_size) < 0)
  {
    panic("New Pages error");
	  task_vanish(KILL_STATUS);
    return;
  }

  if (swexn(autostack_addr_high, autostack_handler_t , NULL, ureg) < 0)
  {
    panic("Cannot register SWEXN handler\n");
	  task_vanish(KILL_STATUS);
    return;
  }

  return;
}

/** @brief Install autostack handler
 *  
 *  This function installs the autostack handler 
 *  This should be called from crt0.c
 *  This will register the handler using swexn system call.
 *
 *  @param stack_high
 *  @param stack_low
 *  @return Void
 */

void
install_autostack(void *stack_high, void *stack_low)
{
  stack_high_ptr = stack_high;
  stack_low_ptr = stack_low;

  /* Using _malloc as the thr_init is not yet called */
  autostack_addr_low = _malloc(EXCEPTION_STACK_SIZE);
  autostack_addr_high = GET_STACK_HIGH((unsigned int)autostack_addr_low,
                                       EXCEPTION_STACK_SIZE);

  /* Install handler */
  if (swexn(autostack_addr_high, autostack_handler_t , NULL, NULL) < 0)
  {
    panic("Cannot register SWEXN handler\n");
    return;
  }

}

/** @brief deregister automatic stack growth handler 
 *   
 *  This function deregisters the handler for automatic stack
 *  growth.
 *
 * @param None
 * @return None
 */

 void deregister_autostack_handler()
 {

  /* De-registering the previous handler */
  if (swexn(NULL,NULL,NULL,NULL) < 0)
  {
    panic("Unable to deregister previous handler ");
    return;
  }

  /* Free the exception stack allocated for automatic stack growth */
  free(autostack_addr_low);

 }

/** @brief helper function for panic
 *   
 *  This function is a helper function for panic.
 *
 * @param exception number
 * @return pointer to a string containig the reson for fault
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




