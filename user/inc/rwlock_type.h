/** @file rwlock_type.h
 *  @brief This file defines the type for reader/writer locks.
 */

#ifndef _RWLOCK_TYPE_H
#define _RWLOCK_TYPE_H

/** @brief User exposed struct */
typedef struct rwlock {
  /* Id of the rwlock object from user*/
  unsigned int rwlock_id;
} rwlock_t;

#endif /* _RWLOCK_TYPE_H */
