/** @file set_term_color.S
 *  @brief Stub file for set term color
 *
 *  This is a stub library for set_term_color syscall
 *
 *  @author Ishant Dawer (idawer)
 *
 *  @bug No known bugs
 */


#include <syscall_int.h>
.globl set_term_color

set_term_color:
	push %ebp /*Save previous frame ptr*/
	movl %esp,%ebp
	push %esi
	movl 0x8(%ebp),%esi
	int $SET_TERM_COLOR_INT
	pop %esi
	leave 
	ret

