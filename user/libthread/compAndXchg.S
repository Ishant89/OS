/** @file Compare and exchange lock 
 *  
 *  @brief This implements Compare and exchange lock 
 *
 *  @author Ishant & Shelton 
 *
 *  @bug No known bugs
 */

.globl compAndXchg

/** @brief Compare and exchange instruction
 *  
 *  This function implements compare and exchanged instruction 
 *
 *  @param destination location
 *  @param compare_value value to be compared
 *  @param source_value source value
 *  @return lock (0 if available else 1)
 */

compAndXchg:
	movl 0xc(%esp),%ecx
	movl 0x8(%esp),%eax
	mov  0x4(%esp),%edx
	lock cmpxchgl %ecx,(%edx)
	ret

