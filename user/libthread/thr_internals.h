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

/* printf wrapper function */
#ifndef DEBUG
#define SIPRINTF(...) lprintf(__VA_ARGS__)
#else
#define SIPRINTF(...) ((void) 0)
#endif

/* printf wrapper function */
#ifndef DEBUG_CRITICAL
#define ISPRINTF(...) lprintf(__VA_ARGS__)
#else
#define ISPRINTF(...) ((void) 0)
#endif

#endif /* THR_INTERNALS_H */
