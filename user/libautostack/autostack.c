/* If you want to use assembly language instead of C,
 * delete this autostack.c and provide an autostack.S
 * instead.
 */

#include<thr_internals.h>

void * stack_high_ptr;
void * stack_low_ptr;

void
install_autostack(void *stack_high, void *stack_low)
{
	stack_high_ptr = stack_high;
	stack_low_ptr = stack_low;
}
