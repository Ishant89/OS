/** @file new_pages.S
 *  @brief Stub file for new_pages
 *
 *  This is a stub library for new_pages syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl new_pages

new_pages:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  push %esi
  lea 0x8(%ebp),%esi
  int $NEW_PAGES_INT
  pop %esi
  leave 
  ret
  
