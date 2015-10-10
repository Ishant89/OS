/** @file thr_internals.h
 *
 *  @brief Thread library related declarations
 *
 *  @author Ishant(idawer) & Shelton(sdsouza)
 *       
 *  @bug No known bugs 
 */



#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H
#include<mutex.h>

/** @brief Stack high pointer*/
extern void * stack_high_ptr;
/** @brief Stack low pointer*/
extern void * stack_low_ptr;

mutex_t alloc_lock;

int thread_fork(void *stack);

/* helper function for panic */
char *get_fault_reason(int exception_number);

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
