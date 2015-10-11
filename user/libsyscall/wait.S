/** @file wait.S
 *  @brief Stub file for wait
 *
 *  This is a stub library for the wait syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl wait

wait:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi /* Save esi as it is called saved */
  movl 8(%ebp),%esi /* Get the status pointer */
  movl $0x00,%eax
  int $WAIT_INT
  pop %esi
  leave 
  ret