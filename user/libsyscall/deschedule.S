/** @file deschedule.S
 *  @brief Stub file for deschedule
 *
 *  This is a stub library for deschedule syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl deschedule 

deschedule:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi
  movl 0x8(%ebp),%esi
  int $DESCHEDULE_INT
  pop %esi
  leave 
  ret
  