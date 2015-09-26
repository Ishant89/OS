/** @file print.S
 *  @brief Stub file for print
 *
 *  This is a stub library for print syscall
 *
 *  @author Ishant Dawer (idawer)
 *
 *  @bug No known bugs
 */

#include <syscall_int.h>

.globl print 

print:
	push %ebp /*Save previous frame ptr*/
	mov %esp,%ebp
	push %esi
	lea 0x8(%ebp),%esi
	int $PRINT_INT
	pop %esi
	leave 
	ret
	
