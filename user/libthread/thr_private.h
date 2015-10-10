/** @file thr_private.h
 *
 *  @brief This file is used to define things
 *         internal to the thread library.
 */

#include <cond.h>
#include <mutex.h>

#ifndef THR_PRIVATE
#define THR_PRIVATE


typedef void *(*childfunc)(void*) ;

typedef struct tcb   tcb_struct;
typedef tcb_struct* tcb;


/** @brief TCB Struct */

struct tcb {
  void *sp;
  void * crash_handler_sp;
  unsigned int tid;
  unsigned int kid;
  unsigned int creator_tid;
  struct tcb * next;
  void *(*func)(void*);
  void * arg;
  int status;
  void* exit_status;
  int waiter;
  mutex_t  private_lock;
  cond_t  exit_cond;
};


#define PASS 0

#define FAIL -1

/** @brief word size */
#define WORD_SIZE 4

#define TCB_SIZE sizeof(tcb_struct)

#define STACK_BUFFER 256

#define CRASH_HANDLER_STACK_SIZE 512

#define TCB_NOT_FOUND NULL

#define THREAD_NOT_CREATED -1

#define setDone(t) ((t) -> status = 1)
#define isDone(t) ((t) -> status & 1)

/** @brief Stack size */
unsigned int stack_size;

mutex_t tcb_lock;

mutex_t alloc_lock;

/** @brief Thread fork system call */
int thread_fork(void *stack);

/** @brief List of tcbs*/
tcb tcb_head;

void insert_tcb_list(tcb);

tcb get_tcb(int pid);

tcb get_tcb_from_tid(int pid);

tcb get_tcb_from_kid(int kid);

int check_if_pid_exists_tcb(int tid);

void free_child_data_structures(tcb child);

void remove_tcb_from_list(tcb entry);

void print_tcb_list();

void vanish_thread_exit(mutex_t *lock_addr);

int compAndXchg(void *,int,int);

#endif /* THR_PRIVATE */


