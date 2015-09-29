/** @file thr_internals.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */



#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H


extern void * stack_high_ptr;
extern void * stack_low_ptr;

/*EDIT: REMOVE LATE */
int thread_fork(void *stack);
/* unsigned errno codes */

#define SIZE_NOT_ALIGNED -1

#define SUCCESS 0
#define ERROR -1


#endif /* THR_INTERNALS_H */
