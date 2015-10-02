/** @file yield.S
 *  @brief Stub file for resume_thread
 *
 *  This is a stub library for  syscall
 *
 *  @author Shelton Dsouza (sdsouza)
 *
 *  @bug No known bugs
 */

#include <simics.h>

.globl resume_thread

resume_thread:
  mov %ebp,%esi
  mov 4(%esp),%eax
  mov 32(%eax),%ebp
  mov 40(%eax),%ebx
  mov 44(%eax),%edx
  mov 48(%eax),%ecx
  mov 72(%eax),%esp
  mov 84(%esi),%edi
  push %edi
  mov 80(%esi),%edi
  push %edi
  mov 76(%esi),%edi
  push %edi
  iret
