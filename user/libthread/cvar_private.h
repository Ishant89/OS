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

#define COND_OBJ_SIZE sizeof(cond_t)
#define LOCK_AVAILABLE 0
#define PASS 0
#define FAIL -1

#define GET_COND_ID(cv) ((unsigned int)(cv))


/** @brief compare and exchange instruction */
int compAndXchg(void *,int,int);
