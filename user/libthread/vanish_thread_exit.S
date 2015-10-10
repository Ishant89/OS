 /** @file vanish.S 
 *
 *
 */

#include <syscall_int.h>
#include <simics.h>
.globl vanish_thread_exit

vanish_thread_exit:
  movl 0x4(%esp),%ecx
  movl $0x0,(%ecx)
  int $VANISH_INT
   
