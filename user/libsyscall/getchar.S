/** @file getchar.S
 *  @brief Stub file for getchar
 *
 *  This is a stub library for getchar syscall
 *
 *  @author Ishant Dawer (idawer)
 *
 *  @bug No known bugs
 */


#include <syscall_int.h>
.globl getchar 

getchar:
	push %ebp /*Save previous frame ptr*/
	mov %esp,%ebp
	int $GETCHAR_INT
	leave 
	ret

