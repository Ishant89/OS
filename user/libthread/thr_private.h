/** @file thr_private.h
 *
 *  @brief thread related data structures declarations
 *  
 *  This file contains:
 *  1. TCB structures
 *  2. ERROR return codes   
 */

#include <cond.h>
#include <mutex.h>

#ifndef THR_PRIVATE
#define THR_PRIVATE

/* Child function handler */
typedef void *(*func)(void*) ;
/* TCB struct */
typedef struct tcb   tcb_struct;
typedef tcb_struct* tcb;


/** @brief TCB Struct */

struct tcb {
  void *sp; /*Child stack */
  void * crash_handler_sp;/*Exception stack*/
  unsigned int tid;/* User thread id*/
  unsigned int kid;/* Kernel thread id*/
  unsigned int creator_tid;/*Parent id*/
  struct tcb * next;/*next tcb list*/
  void *(*func)(void*);/*child handler*/
  int status;
  void * arg;/*arg*/
  void* exit_status;/*exit status*/
  int waiter;/*waiter id*/
  int  private_lock;/*lock b/w child & waiter*/
};

/** @brief return success as PASS */
#define PASS 0

/** @brief return success as FAIL*/
#define FAIL -1

/** @brief return value  as 0*/
#define RETVAL_0 0 

/** @brief return value  as 1*/
#define RETVAL_1  1

/** @brief word size */
#define WORD_SIZE 4

/** @brief TCB size */
#define TCB_SIZE sizeof(tcb_struct)

/** @brief Size of the stack buffer */
#define STACK_BUFFER 256

/** @brief Size of the exception stack */
#define CRASH_HANDLER_STACK_SIZE 512


#define setDone(t) ((t) -> status = 1)
#define isDone(t) ((t) -> status & 1)


/** @brief Stack size */
unsigned int stack_size;

/** @brief lock for tcb */
mutex_t tcb_lock;

/** @brief lock for allocator(malloc,free)*/
mutex_t alloc_lock;

/** @brief Thread fork system call */
int thread_fork(void *stack);

/** @brief List of tcbs*/
tcb tcb_head;

void insert_tcb_list(tcb);

tcb get_tcb_from_tid(int pid);

tcb get_tcb_from_kid(int kid);

int check_if_pid_exists_tcb(int tid);

void free_child_data_structures(tcb child);

void remove_tcb_from_list(tcb entry);

void vanish_thread_exit(int*lock_addr);

int compAndXchg(void *,int,int);

#endif /* THR_PRIVATE */


