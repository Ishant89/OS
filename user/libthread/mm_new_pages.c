/** @file mm.c
 *  
 *  @brief Memory allocator for the thread library
 *   
 *  This file contains
 *  allocator functions for managing memory for the thread library
 *  API
 *  It has following functions:
 *
 *  @author Ishant & Shelton
 *
 *  @bug No knwown bugs
 */

#include <mm_new_pages.h>
#include <stdlib.h>
#include <thr_internals.h>
#include "thr_private.h"
#include <thread.h>
#include <syscall.h>
#include <stdlib.h>
#include <autostack.h>
#include <string.h>

/** @brief Init new pages allocator
 *  
 *  This function does following:
 *  1. Define the free list of pages start and end
 *  
 *  @param stack_size Size of the stack 
 *  @return int PASS/FAIL
 */

int mm_init_new_pages(size_t stack_size)
{
  mem_base = stack_low_ptr;
  thread_stack_size = stack_size;
  mutex_init(&new_pages_lock);

  freeListSize = EXTEND_PAGE_SIZE;

  /* Allocate memory for free list */
  if((freeListHead = malloc(freeListSize)) == NULL)
  {
    panic("mm_init : Could not allocate memory for free list\n");
    return FAIL;
  }

  freeListStart = freeListHead;
  freeListEnd = GET_FREE_LIST_END(freeListStart,freeListSize);

  return PASS;
}

/** @brief Allocate more memory
 *  
 *  This allocates the memory used for 
 *  child stack by using system calls 
 *  new_pages
 *
 *  This function does :
 *  1. Get the size
 *  2. Align it on a page boundary
 *  3. Use new_pages to get memory
 *  
 *  @param num_words size of the memory
 *  @return void *  memory_ptr
 */
void * extend_memory(size_t num_words)
{
  
  size_t extend_size = ROUND_OFF_PAGE_SIZE(num_words);
  mem_base = GET_STACK_BASE_ADDRESS(mem_base,extend_size);

  if(new_pages(mem_base,extend_size) < 0)
  {
    panic("New pages error in extend_memory\n");
    return NULL;
  }
  return mem_base;
}

/** @brief Add free page pointer to the free list
 *
 *  This is used to maintain a list of the free page
 *  locations which can be used for child stack 
 *  allocation 
 *
 *  @param bp free page pointer 
 *  @return void
 */
void insertFreeBlock(void *bp)
{
 
 PUTW(freeListHead,bp);
 freeListHead = INCREMENT_HEAD(freeListHead); 
 if(freeListHead == freeListEnd)
 {
  freeListSize += EXTEND_PAGE_SIZE;
  void *new_ptr = realloc(freeListStart,freeListSize);
  freeListHead = INCREMENT_HEAD_BY_SIZE(new_ptr,EXTEND_PAGE_SIZE);
  freeListStart = new_ptr;
  freeListEnd = GET_FREE_LIST_END(freeListStart,freeListSize);
 }

}

/** @brief Allocate new pages 
 *  
 *  This allocate new pages which is used to serve
 *  3 requirements
 *  1. TCB memor requirement
 *  2. child stack memor requirement
 *  3. exception stack  memor requirement
 *  
 *  It firsts looks for free list pointers (means pages 
 *  which are recently freed) otherwise new allocation in
 *  the memory will be done
 *  @return page location
 */
void * new_pages_malloc()
{
  size_t reqd_size = TCB_SIZE + thread_stack_size + STACK_BUFFER + CRASH_HANDLER_STACK_SIZE;

  mutex_lock(&new_pages_lock);
  void *bp = search_free_block_list();

  if(bp == NULL)
  {
    bp = extend_memory(reqd_size);
    mutex_unlock(&new_pages_lock);
    return bp;
  }

  else
  {
    mutex_unlock(&new_pages_lock);
    return bp;
  }
}

/** @brief free the pages
 *  
 *  This reclaims allocated pages and inserts the pointer
 *  to the free list. 
 *  
 *  @param bp page to be freed
 */

void free_pages(void *bp)
{
  mutex_lock(&new_pages_lock);
  remove_pages(bp);
  insertFreeBlock(bp);
  mutex_unlock(&new_pages_lock);
  return;
}

/** @brief search for a free list pointer 
 *  
 *  This does:
 *  1. If there is a free pointer in the list,it
 *  reallocates page at that location
 *  2. Else memoery will be extended
 *  
 *  @param bp page to be freed
 */

void * search_free_block_list()
{
  if(freeListHead == freeListStart)
    return NULL;

  else
  {
    freeListHead = DECREMENT_HEAD(freeListHead);
    void * bp = (void *)GETW(freeListHead);
    size_t reqd_size = TCB_SIZE + thread_stack_size + STACK_BUFFER + CRASH_HANDLER_STACK_SIZE;
    reqd_size = ROUND_OFF_PAGE_SIZE(reqd_size);

    if(new_pages(bp,reqd_size) < 0)
    {
      panic("New pages error in extend_memory\n");
      return NULL;
    }
    return bp;
  }
}



