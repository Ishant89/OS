/** @file readfile.S
 *  @brief Read the file 
 *
 *  This is a stub library for readfile syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */


#include <syscall_int.h>
.globl readfile

readfile:
  push %ebp /*Save previous frame ptr*/
  movl %esp,%ebp
  push %esi
  lea 0x8(%ebp),%esi
  int $READFILE_INT
  pop %esi
  leave 
  ret