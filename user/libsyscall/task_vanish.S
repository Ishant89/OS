/** @file task_vanish.S
 *  @brief Stub file for task_vanish
 *
 *  This is a stub library for task_vanish syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl task_vanish

task_vanish:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi /* Save esi as it is calledd saved */
  movl 8(%ebp),%esi /* Get the status argument */
  movl $0x00,%eax
  int $TASK_VANISH_INT
  pop %esi
  leave 
  ret