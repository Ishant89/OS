/** @file vanish.S 
 *
 *
 */

#include <syscall_int.h>
#include <simics.h>
.globl vanish

vanish:
  int $VANISH_INT
  ret
