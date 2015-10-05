/** @file halt.S
 *  @brief Stub file for halt
 *
 *  This is a stub library for the halt syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl halt

halt:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  int $HALT_INT
  leave
  ret