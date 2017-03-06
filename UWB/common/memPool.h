/**
 * @file memPool.c
 * @brief functions use to allocate a pool of memory and to work on it
 * @author ore@bespoon.com
 * @date 18/07/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#include <stddef.h>
#include "types.h"


/**
 * @brief alignedSizeof
 * @note return a size of aligned with 8/16/32/64 configurable
 * @param size size parameter (usually sizeof(truc) 
 * @return aligned size of
 */
OSAL_u32        __alignedSizeof(OSAL_u32 size);
#define alignedSizeof(_size) __alignedSizeof((OSAL_u32) _size)

/**
 * @brief init_MemPool
 * @note initialize a memory pool
 * @param pool[in/out] a pointer on mempool to create
 * @param poolSize[in] the sizae of the pool to create
 * @param coreID[in] indicate who is allocating
 * @return #OSAL_OK on success, #OSAL_ENOMEM if pool cannot be allocated due to memory constraints, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        init_MemPool(OSAL_void** pool, OSAL_s32 poolSize,OSAL_stackLevel coreID);


/**
 * @brief init_MemPool_constrained
 * @note initialize a memory pool (constrained, for future use)
 * @param pool[in/out] a pointer on mempool to create
 * @param poolSize[in] the size of the pool to create
 * @param coreID[in] indicate who is allocating
 * @param physical_addr[in] physical address to use
 * @return #OSAL_OK on success, #OSAL_ENOMEM if pool cannot be allocated due to memory constraints, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        init_MemPool_constrained(OSAL_void** pool, OSAL_s32 poolSize, OSAL_stackLevel coreID ,OSAL_u32 *physical_addr  );

/**
 * @brief release_MemPool
 * @note release a memory pool
 * @param pool[in/out] a pointer on mempool to destroy
 * @param coreID[in] indicate who is allocating
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        release_MemPool(OSAL_void** pool,OSAL_stackLevel coreID);

/**
 * @brief release_MemPool
 * @note release a memory pool (constrained, for future use)
 * @param pool[in/out] a pointer on mempool to destroy
 * @param coreID[in] indicate who is allocating
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        release_MemPool_constrained(OSAL_void** pool,OSAL_u32 size,OSAL_stackLevel coreID, OSAL_u32 physical_addr );

/**
 * @brief __allocate_in_MemPool
 * @note allocate a pointer within a memory pool
 * @param pool[in] a pointer on mempool
 * @param size[in] the size of memory to take in the pool
 * @param ptr[out] the pointer to be 'pool' allocated
 * @return #OSAL_OK on success, #OSAL_ENOMEM if out of pool, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        __allocate_in_MemPool(OSAL_void* pool, OSAL_s32 size, OSAL_void** ptr);
#define allocate_in_MemPool(_pool,_size,_ptr) __allocate_in_MemPool(_pool,_size,(OSAL_void**) _ptr)

/**
 * @brief __free_from_MemPool
 * @note release a pointer from memory pool
 * @note as this version is a simple pool manager, there is not possibility to do free/realloc
 * @note as a consequence, once the free is done for an element, we expect to free the whole pool
 * @note reallocation can be done only by reallocating a pool after having freed it entierely
 * @param pool[in] a pointer on mempool
 * @param ptr[out] the pointer to be 'pool' freed
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        __free_from_MemPool(OSAL_void* pool, OSAL_void** ptr);
#define free_from_MemPool(_pool,_ptr) __free_from_MemPool(_pool,(OSAL_void**) _ptr)

#endif //#ifndef _MEMPOOL_H_

