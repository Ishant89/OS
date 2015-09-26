/** @file remove_pages.S
 *  @brief Stub file for remove_pages
 *
 *  This is a stub library for remove_pages syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl remove_pages

remove_pages:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi
  movl 0x8(%ebp),%esi
  int $REMOVE_PAGES_INT
  pop %esi
  leave 
  ret
  
