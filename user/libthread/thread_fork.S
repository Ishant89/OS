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

thread_fork:
  movl (%esp),%edx /*Return address*/
  movl 0x4(%esp),%ecx /*Child stack pointer*/
  movl %edx,0x4(%ecx)
  movl %ecx,(%ecx)
  push %ebp
  movl %esp,%ebp
  int $THREAD_FORK_INT
  cmpl $0x0,%eax
  jne label
  movl %ecx,%esp
  popl %ebp
  ret
  label: 
		leave
		ret
