/** @file exec.S
 *  @brief Stub file for exec
 *
 *  This is a stub library for the exec syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl exec

exec:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi /* Save esi as it is callee saved */
  /* Get the pointer to the argc and 
   * argv arguments
   */
  lea 0x8(%ebp),%esi
  movl $0x00,%eax
  int $EXEC_INT
  pop %esi
  leave 
  ret
