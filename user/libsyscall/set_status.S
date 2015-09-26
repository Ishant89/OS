/* @file set_status.S 
 *
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
