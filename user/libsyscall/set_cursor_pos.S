 /** @file set_cursor_pos.S
 *  @brief Set the cursor postion
 *
 *  This is a stub library for set_cursor_pos syscall
 *
 *  @author Ishant Dawer (idawer)
 *
 *  @bug No known bugs
 */


#include <syscall_int.h>
.globl set_cursor_pos

set_cursor_pos:
	push %ebp /*Save previous frame ptr*/
	movl %esp,%ebp
	push %esi
	lea 0x8(%ebp),%esi
	int $SET_CURSOR_POS_INT
	pop %esi
	leave 
	ret

