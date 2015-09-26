/** @file gettid.S
 *  @brief Stub file for gettid
 *
 *  This is a stub library for gettid syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl gettid

gettid:
  push %ebp /*Save previous frame ptr*/
  mov %esp,%ebp
  int $GETTID_INT
  leave 
  ret
  
