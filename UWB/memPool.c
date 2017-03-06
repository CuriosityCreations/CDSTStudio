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

#include <stddef.h>
#include <types.h>
#include <memPool.h>

/**
 *  @brief memPool_t 
 *  @note memory Pool structure
 *  @note This version currently work ,under the condition of 
 *        allocating the pool and everythiong in it at once ( no dynamic allocation within the pool)
 */
typedef struct memPool
{
	OSAL_void* mem;     /**< The main pointer of the pool */
	OSAL_void* current; /**< The current pointer of free memory within the pool */
	//OSAL_u32 size;      /**< The size of the pool COMMENTED? to gain 4 bytes, in condition we don't need to check for bd pool memory*/
} memPool_t;


/**
 * @brief alignedSizeof
 * @return aligned size of
 */
#define MEMPOOL_ALIGN_TYPE 4 //nb of byte to align with
OSAL_u32        __alignedSizeof(OSAL_u32 size)
{
    //u8 allign
    OSAL_u32 align=((OSAL_u32) size/MEMPOOL_ALIGN_TYPE)*MEMPOOL_ALIGN_TYPE;
    OSAL_u32 nextAlign=((OSAL_u32) (size+MEMPOOL_ALIGN_TYPE)/MEMPOOL_ALIGN_TYPE)*MEMPOOL_ALIGN_TYPE;
    if(align == size)
        return size;
    return nextAlign;
}

/**
 * @brief init_MemPool
 * @return #OSAL_OK on success, #OSAL_ENOMEM if pool cannot be allocated due to memory constraints, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        init_MemPool(OSAL_void** pool, OSAL_s32 poolSize,OSAL_stackLevel coreID)
{
     //a better version should create here memory keyword ( and some macro to create memory keyword)
     //but this is not the case for this version. lets consider we know what we're doing. So OSAL_ERROR won't never be returned
     //we don't check either if pool has not been allocated earlier
     memPool_t* pl=NULL;
     if(pool == NULL) return OSAL_ERROR;
     pl = OSAL_malloc((poolSize+alignedSizeof(sizeof(memPool_t))),coreID);
     if (pl == NULL)  return OSAL_ENOMEM;
     pl->mem = pl;
     pl->mem += alignedSizeof(sizeof(memPool_t));
     OSAL_memset(pl->mem,0,poolSize);
     pl->current = pl->mem;
     *pool = pl;
     //pl->size = poolSize;
     return OSAL_OK;
}

/**
 * @brief init_MemPool_constrained
 * @return #OSAL_OK on success, #OSAL_ENOMEM if pool cannot be allocated due to memory constraints, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        init_MemPool_constrained(OSAL_void** pool, OSAL_s32 poolSize, OSAL_stackLevel coreID ,OSAL_u32 *physical_addr  )
{
    //not yet supported. Does the same than init_MemPool
    return init_MemPool(pool,poolSize,coreID);
}

/**
 * @brief release_MemPool
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        release_MemPool(OSAL_void** pool,OSAL_stackLevel coreID)
{
    //a better version should check here memory keyword ( and some macro to check memory keyword)
    //but this is not the case for this version. lets consider we know what we're doing. So OSAL_ERROR won't never be returned
    memPool_t* pl=NULL;
    if(pool == NULL) return OSAL_ERROR;    
    pl=*pool;
    if(pl==NULL) return OSAL_OK;
    OSAL_free(pl,coreID),*pool=NULL;
    //pool->size=0;
    return OSAL_OK;
}

/**
 * @brief release_MemPool
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        release_MemPool_constrained(OSAL_void** pool,OSAL_u32 size,OSAL_stackLevel coreID, OSAL_u32 physical_addr)
{
    //not yet supported. Does the same than init_MemPool
    return release_MemPool(pool,coreID);
}

/**
 * @brief allocate_in_MemPool
 * @return #OSAL_OK on success, #OSAL_ENOMEM if out of pool, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        __allocate_in_MemPool(OSAL_void* pool, OSAL_s32 size, OSAL_void** ptr)
{
    //a better version should check for memPool_t struct, to be sure we don't go somewhere we don't
    //but this version take as axiom that we know what we're doing
    memPool_t* pl=(memPool_t*) pool;
    if(ptr == NULL) return OSAL_ERROR;
    if(pl == NULL) return OSAL_ERROR;
    //we could check it if we'e not out of pool:
    // if((pl->current+size)>(pl->mem+pl->size))
    //   return OSAL_ERROR;
    //but we won't, under condition tha tpool is allocated and then all element of pool are allocated, and then element are all free, and then poll is freed
    *ptr = pl->current;
    pl->current+= alignedSizeof(size);
    return OSAL_OK;
}

/**
 * @brief free_from_MemPool
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given
 */
OSAL_error_t        __free_from_MemPool(OSAL_void* pool, OSAL_void** ptr)
{
    memPool_t* pl=(memPool_t*) pool;
    //a better version should check for memPool_t struct, to be sure we don't go somewhere we don't
    //but this version take as axiom that we know what we're doing
    //no reallocation within the pool for the moment
    if(pl == NULL) return OSAL_ERROR;
    *ptr = NULL;
    return OSAL_OK;
}

