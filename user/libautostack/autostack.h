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

#include <syscall.h>

#define NEW_PAGES_ERROR -1

#define SWEXN_INSTALL_ERROR -2

#define EXCEPTION_STACK_SIZE 4096

#define EXTEND_PAGE_SIZE 4096

#define GET_STACK_LOW_ADDRESS(val1,val2) (void *)(val1 - val2)

#define GET_STACK_HIGH(val1,val2) (void *)(val1 + val2)

#define GET_EXTENDED_STACK_BASE(val1,val2) (((((unsigned long)val1) - ((unsigned long)val2))/EXTEND_PAGE_SIZE) * EXTEND_PAGE_SIZE) + EXTEND_PAGE_SIZE

/* Error number */
int err_num;


void resume_thread(ureg_t *ureg);