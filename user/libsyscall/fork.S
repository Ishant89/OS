/** @file fork.S
 *  @brief Stub file for fork
 *
 *  This is a stub library for the fork syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl fork

fork:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  movl $0x00,%eax
  int $FORK_INT
  leave 
  ret