/** @file sleep.S
 *  @brief Stub file for sleep
 *
 *  This is a stub library for sleep syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl sleep 

sleep:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi
  movl 0x8(%ebp),%esi
  int $SLEEP_INT
  pop %esi
  leave 
  ret
  
