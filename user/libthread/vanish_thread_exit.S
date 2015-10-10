 /** @file vanish_thread_exit.S 
 *	 
 *	 @brief Vanish system call plus releasing the lock
 *
 *	 This file helps in killing the child and releasing the child
 *	 lock just before the vanish system call so that vanish system call
 *	 and releasing the lock happens in a single function call 
 *	 Reasons to do this thing:
 *	 1. If lock is released and a function containing vanish system call
 *	 is called, then it is possible that stack is freed and allocated to 
 *	 another child and the following vanish system call will corrupt the new
 *	 child stack 
 *	 2. Our solution will place the lock releasing instr and vanish system call
 *	 instruction in the same function such that no stack operations will happen 
 *	 while doing this.
 *	  
 *	@author Ishant (idawer) & Shelton(sdsouza)	
 *  @bug No known bugs
 */

#include <syscall_int.h>
.globl vanish_thread_exit

vanish_thread_exit:
  movl 0x4(%esp),%ecx /*Get the lock address*/
  movl $0x0,(%ecx)/*Release the lock */
  int $VANISH_INT /*vanish system call*/
   
