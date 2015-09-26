 /** @file readline.S
 *  @brief Read the line 
 *
 *  This is a stub library for readline syscall
 *
 *  @author Ishant Dawer (idawer)
 *
 *  @bug No known bugs
 */


#include <syscall_int.h>
.globl readline

readline:
	push %ebp /*Save previous frame ptr*/
	movl %esp,%ebp
	push %esi
	lea 0x8(%ebp),%esi
	int $READLINE_INT
	pop %esi
	leave 
	ret

