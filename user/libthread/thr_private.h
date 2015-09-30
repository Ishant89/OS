/** @file thr_private.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */

#ifndef THR_PRIVATE
#define THR_PRIVATE


typedef struct tcb   tcb_struct;
typedef tcb_struct* tcb;

typedef struct threadList children_list;

/** @brief children list */

struct threadList {
  children_list * next;
  tcb name;
};

/** @brief TCB Struct */

struct tcb {
  void * sp;
  unsigned int tid;
  unsigned int kid;
  struct tcb * next;
  void *(*func)(void*);
  void * arg;
  int status;
  void* exit_status;
  int waiter;
  children_list * children;
};

/** @brief word size */
#define WORD_SIZE 4

#define TCB_SIZE sizeof(tcb_struct)

#define STACK_BUFFER 16

#define TCB_NOT_FOUND NULL

#define THREAD_NOT_CREATED -1

#define setDone(t) ((t) -> status = 1)
#define isDone(t) ((t) -> status & 1)

/** @brief Stack size */
unsigned int stack_size;

/** @brief Thread fork system call */
int thread_fork(void *stack);

/** @brief List of tcbs*/
tcb tcb_head;

void insert_tcb_list(tcb);

tcb get_tcb(int pid);

tcb get_tcb_from_tid(int pid);

tcb get_tcb_from_kid(int kid);

void push_children(children_list **head,tcb child);

//void remove_children(children_list **head,tcb child);

void free_child_thread_list(children_list * head);

void freeThread(tcb thread);

void remove_tcb_from_list(tcb entry);

void print_tcb_list();

#endif /* THR_PRIVATE */


