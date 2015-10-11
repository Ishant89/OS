/** @file misbehave.S
 *  @brief Stub file for misbehave
 *
 *  This is a stub library for misbehave syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>


.globl misbehave

misbehave:

  /* push esi as it is callee saved */
  push %esi
  /* Get the argument from esp+4 */
  movl 8(%esp),%esi
  movl $0x00,%eax
  int $MISBEHAVE_INT
  pop %esi
  ret
