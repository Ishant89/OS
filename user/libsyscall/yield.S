/** @file yield.S
 *  @brief Stub file for yield
 *
 *  This is a stub library for yield syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl yield

yield:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi
  movl 0x8(%ebp),%esi
  int $YIELD_INT
  pop %esi
  leave 
  ret
  
