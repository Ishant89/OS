/** @file mm_new_pages.h
 *  
 *  @brief Memory allocator for the thread library
 *   
 *  This file contains functions for managing memory for the thread library
 *  API
 *
 *  @author Ishant & Shelton
 *
 *  @bug No known bugs 
 */

#include <mutex.h>

typedef unsigned int size_t ;


#define WORD_SIZE 4

#define EXTEND_PAGE_SIZE 4096

#define GET_FREE_LIST_END(start,size) ((char *)start + size) 

#define ROUND_OFF_PAGE_SIZE(val) ((val/EXTEND_PAGE_SIZE) * EXTEND_PAGE_SIZE) + EXTEND_PAGE_SIZE

#define GET_EXTENDED_STACK_SIZE(bp,val) ((char *)bp) 

#define GET_STACK_BASE_ADDRESS(bp,val) ((char *)bp - val)

#define PUTW(p,val) ((*(size_t *)(p))) = ((size_t)val)

#define INCREMENT_HEAD(bp) ((char *)bp + WORD_SIZE)

#define DECREMENT_HEAD(bp) ((char *)bp - WORD_SIZE)

#define INCREMENT_HEAD_BY_SIZE(bp,size) ((char *)bp + WORD_SIZE)

#define GETW(bp) (*((size_t *)bp))

/* Globar pointer which points to last occupied memory location */
void * mem_base;

/* Stack size specified in thr_init */
size_t thread_stack_size;

/* Free list head */
void * freeListHead;

/* Free list start */
void * freeListStart;

/* Free list end */
void * freeListEnd;

/* Error number */
int err_num;

/* lock for allocator */
mutex_t new_pages_lock;

/* Size of free list */
size_t freeListSize;

int mm_init_new_pages(size_t stack_size);

void * extend_memory(size_t num_words);

void insertFreeBlock(void *bp);

void * new_pages_malloc();

void free_pages(void *bp);

void * search_free_block_list();




