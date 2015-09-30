/** @file vanish.S 
 *
 */

#include <syscall_int.h>
#include <simics.h>
.globl vanish

vanish:
  //call sim_breakpoint
  int $VANISH_INT
  ret
