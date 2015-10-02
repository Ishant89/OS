/** @file Compare and exchange lock 
 *  
 *  @brief This implements Compare and exchange lock 
 *
 *  @author Ishant & Shelton 
 *
 *  @bug No known bugs
 */

.globl compAndXchg

compAndXchg:
	movl 0xc(%esp),%ecx
	movl 0x8(%esp),%eax
	mov  0x4(%esp),%edx
	lock cmpxchgl %ecx,(%edx)
	ret

