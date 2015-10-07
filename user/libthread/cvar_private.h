/** @file cvar_private.c
 *  
 *  @brief Conditional variables header files 
 *   
 *  This file implements conditional variables and defines 
 *  following functions : 
 *  1. cond_init()
 *  2. cond_destroy()
 *  3. cond_wait()
 *  4. cond_signal()
 *  5. cond_broadcast()
 *
 *  @author Ishant Dawer(idawer) & Shelton D'Souza (sdsouza)
 *
 *  @bug No known bugs 
 */

#include<malloc.h>
#include<mutex_type.h>
#include<mutex.h>
#include<cond_type.h>
#include<syscall.h>

#include<thr_internals.h>
#include<simics.h>

#define COND_OBJ_SIZE sizeof(cond_var_object)
#define LOCK_AVAILABLE 0
#define PASS 0
#define FAIL -1

#define GET_COND_ID(cv) (unsigned int)(&(cv -> cond_id))

/** @brief Struct for waiting thread queue */
typedef struct wait_thread_queue
{
   	int thread_id;
	struct wait_thread_queue * next_wait_thread;
} wait_thread_queue;

/** @brief Stuct for conditional variable */

typedef struct cond_var_object 
{
	unsigned int cond_id;/*ID of the cond var */
	int cond_lock;/*Lock for the cond objects */
	wait_thread_queue * head_queue;/*Head of the wait queue */
	mutex_t * mutex_object; /* Associated mutex object */
	struct cond_var_object * next_cond_object;/* Next cond object */
} cond_var_object;

/** @brief Head of the cond_var_objects */
cond_var_object * head_cond_object = NULL;

/** @brief compare and exchange instruction */
int compAndXchg(void *,int,int);
