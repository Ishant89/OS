/** @file mm.c
 *  
 *  @brief Memory allocator for the thread library
 *   
 *  This file contains functions for managing memory for the thread library
 *  API
 *
 *  @author Ishant & Shelton
 *
 *  @bug 
 */

#define DEBUG 0
#include <mm_new_pages.h>
#include <stdlib.h>
#include <thr_internals.h>
#include "thr_private.h"
#include <thread.h>
#include <syscall.h>
#include <stdlib.h>
#include <simics.h>
#include <autostack.h>
#include <string.h>

int mm_init_new_pages(size_t stack_size)
{
  mem_base = stack_low_ptr;
  thread_stack_size = stack_size;
  mutex_init(&new_pages_lock);

  freeListSize = EXTEND_PAGE_SIZE;

  /* Allocate memory for free list */
  if((freeListHead = malloc(freeListSize)) == NULL)
  {
    SIPRINTF("mm_init : Could not allocate memory for free list\n");
    err_num = MALLOC_ERROR;
    return err_num;
  }

  freeListStart = freeListHead;
  freeListEnd = GET_FREE_LIST_END(freeListStart,freeListSize);

  return 0;
}

void * extend_memory(size_t num_words)
{
  
  size_t extend_size = ROUND_OFF_PAGE_SIZE(num_words);
  mem_base = GET_STACK_BASE_ADDRESS(mem_base,extend_size);

  if(new_pages(mem_base,extend_size) < 0)
  {
    SIPRINTF("New pages error in extend_memory\n");
    err_num = NEW_PAGES_ERROR;
    return NULL;
  }

  return mem_base;

}

void insertFreeBlock(void *bp)
{
 
 PUTW(freeListHead,bp);
 freeListHead = INCREMENT_HEAD(freeListHead); 
 if(freeListHead == freeListEnd)
 {
  lprintf("Incresing free list size");
  freeListSize += EXTEND_PAGE_SIZE;
  void *new_ptr = realloc(freeListStart,freeListSize);
  freeListHead = INCREMENT_HEAD_BY_SIZE(new_ptr,EXTEND_PAGE_SIZE);
  freeListStart = new_ptr;
  freeListEnd = GET_FREE_LIST_END(freeListStart,freeListSize);
 }

}

void * new_pages_malloc()
{
  size_t reqd_size = TCB_SIZE + thread_stack_size + STACK_BUFFER + CRASH_HANDLER_STACK_SIZE;

  mutex_lock(&new_pages_lock);
  SIPRINTF("Entering new_pages_malloc");
  void *bp = search_free_block_list();

  if(bp == NULL)
  {
    bp = extend_memory(reqd_size);
    SIPRINTF("Exiting new_pages_malloc by extening memory bp : %p",bp);
    mutex_unlock(&new_pages_lock);
    return bp;
  }

  else
  {
    SIPRINTF("Exiting new_pages_malloc bp : %p",bp);
    mutex_unlock(&new_pages_lock);
    return bp;
  }
}

void free_pages(void *bp)
{
  mutex_lock(&new_pages_lock);
  SIPRINTF("Entering free pages bp : %p",bp);
  remove_pages(bp);
  insertFreeBlock(bp);
  mutex_unlock(&new_pages_lock);
  return;
}

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
      SIPRINTF("New pages error in extend_memory\n");
      err_num = NEW_PAGES_ERROR;
      return NULL;
    }

    return bp;
  }
}



