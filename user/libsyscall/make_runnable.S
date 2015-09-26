/** @file make_runnable.S
 *  @brief Stub file for make_runnable
 *
 *  This is a stub library for make_runnable syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl make_runnable 

make_runnable:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi
  movl 0x8(%ebp),%esi
  int $MAKE_RUNNABLE_INT
  pop %esi
  leave 
  ret
  
