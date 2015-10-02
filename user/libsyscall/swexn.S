/** @file swexn.S
 *  @brief Stub file for installing swexn handler
 *
 *  This is a stub library for swexn syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl swexn

swexn:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi
  lea 0x8(%ebp),%esi
  int $SWEXN_INT
  pop %esi
  leave 
  ret
  