/** @file get_ticks.S
 *  @brief Stub file for get_ticks
 *
 *  This is a stub library for get_ticks syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl get_ticks

get_ticks:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  int $GET_TICKS_INT
  leave 
  ret
  
