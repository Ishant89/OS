/** @file vanish.S
 *  @brief Stub file for vanish
 *
 *  This is a stub library for vanish syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>
.globl vanish

vanish:
  int $VANISH_INT
