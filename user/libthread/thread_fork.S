/* @file thread_fork.S
 *  @brief Stub file for thread_fork
 *
 *  This is a stub library for thread_fork syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>
#include <simics.h>
.globl thread_fork

/** @brief Thread fork function 
 *  
 *  This includes a function call to thread_fork (system call to create a 
 *	new thread. 
 *	Before this call, there is just one stack which belongs to
 *	a single thread which poses the problem mentioned as follows:
 *	
 *	Problem: After this call, two threads will share the same stack
 *
 *	Solution (using below assembly): 
 *	1. Point the child stack pointer to a different memory allocated using 
 *	new_pages
 *	2. Push the return address,from where this fn was called, to the new child stack 
 *	3. Move ebp up by 160 bytes above the stack pointer (buffer size) 
 *	4. Separating child and parent's ebp prevents corruption of data in 
 *	respective contexts
 *	5. Upon return, both parent and child will return to same eip but on 
 *	different stacks.
 *
 *	It does the following operations:
 *	1. It takes in child stack pointer and return address where child will 
 *	return first
 *	2. Move new child stack addreess to child stack pointer 
 *	3. Move ebp to stack pointer and shift up by 160 bytes
 *	4. Push return address to the stack pointer location
 *	5. It does the system call 
 *	6. If ret val is 0 (child), then ret address is popped from the child stack 
 *	otherwise parent stack
 *	
 *
 *
 *	@param return_address return address from where fork was called
 *	@param child_stack pointer 
 *	@return void 
 */

thread_fork:
  movl (%esp),%edx /*Return address*/
  movl 0x4(%esp),%ecx /*Child stack pointer*/
  movl %edx,0x4(%ecx)/*Saving ret address on new stack*/
  movl %ecx,(%ecx)
  push %ebp
  movl %esp,%ebp
  int $THREAD_FORK_INT
  cmpl $0x0,%eax
  jne label
  movl %ecx,%esp/*Loading child stack pointer*/
  popl %ebp /*Load esp value into ebp*/
  lea 0x40(%ebp),%ebp
  ret
  label: 
		leave
		ret
