/** @file semaphore_private.h
 *  
 *  @brief Semaphore library 
 *
 *  This file implements the semaphore functions for the 
 *  thread library 
 *
 *  1. sem_init 
 *  2. sem_destroy
 *  3. sem_wait
 *  4. sem_signal
 *  
 *  @author Shelton Dsozua(sdsouza)
 *  @bug No known bugs
 */

/** @brief Thread queue structure 
 *  
 *  This contains : 
 *  1. Thread id (list of thread ids in the waiting queue
 *  2. One in the top should always be running 
 *  3. Others should be waiting. 
 */

#include <malloc.h>
#include <syscall.h>
#include <thread.h>
#include <contracts.h>
#include <mutex_type.h>
#include <sem_type.h>

#define SUCCESS 0
#define FAILURE -1
#define KILL_STATUS -2
