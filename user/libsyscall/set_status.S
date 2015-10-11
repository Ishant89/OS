/** @file set_status.S
 *  @brief Stub file for set_status
 *
 *  This is a stub library for set_status syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>


.globl set_status

set_status:

  /* push esi as it is callee saved */
  push %esi
  /* Get the argument from esp+4 */
  movl 8(%esp),%esi
  movl $0x00,%eax
  int $SET_STATUS_INT
  pop %esi
  ret
