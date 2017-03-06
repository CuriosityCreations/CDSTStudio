/**
 * @file osal_stdlib.h
 * @brief definitions for Pinpointer stdlib functions
 * @author bgi@bespoon.com
 * @date 4/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _OSAL_STDLIB_H_
#define _OSAL_STDLIB_H_


//#define OSAL_MEM_TRACE //no trace at all if undefined.

#include "osal_type.h"
/******* definitions of main PinPointer stdlib functions  ********/

/** OSAL_malloc allocates [size] bytes and returns a posinter to the allocated memory.The memory is not cleared*/
OSAL_void*        __OSAL_malloc( OSAL_u32 size, OSAL_stackLevel coreID , OSAL_u8 *func, OSAL_u16 line);

/** OSAL_malloc_constrained allocates [size] bytes and returns a pointer to the allocated continuous memory.The memory is not cleared*/
OSAL_void*        __OSAL_malloc_constrained( OSAL_u32 size, OSAL_stackLevel coreID ,OSAL_u32 *physical_addr  , OSAL_u8 *func, OSAL_u16 line);

/** OSAL_free free memory described by [ptr] */
OSAL_void         __OSAL_free( OSAL_void * ptr, OSAL_stackLevel coreID  , OSAL_u8 *func, OSAL_u16 line);

/** OSAL_free_constrained free memory described by [ptr] */
OSAL_void         __OSAL_free_constrained( OSAL_void * ptr, OSAL_stackLevel coreID, OSAL_u32 size, OSAL_u32 physical_addr  , OSAL_u8 *func, OSAL_u16 line);

/** OSAL_realloc changes the size of the memory block pointed to by [ptr] to [size] bytes.*/
OSAL_void*        __OSAL_realloc( OSAL_void * ptr, OSAL_u32 size  , OSAL_u8 *func, OSAL_u16 line);

/** OSAL_calloc allocates memory for an array of [nmemb] elements of [elsize] bytes each and returns a pointer to the allocated memory.The memory is cleared*/
OSAL_void*        __OSAL_calloc( OSAL_u32 nmemb, OSAL_u32 elsize  , OSAL_u8 *func, OSAL_u16 line);

#ifdef OSAL_MEM_TRACE

/** set nb of information to show in case of MEM trace activated */
OSAL_void 		 OSAL_setAllocTraceLevel(OSAL_u32 level);
#define OSAL_malloc(_size,_coreID)  __OSAL_malloc(_size,_coreID,(OSAL_u8 *) __FUNCTION__,__LINE__)
#define OSAL_malloc_constrained(_size,_coreID,_paddr)  __OSAL_malloc_constrained(_size,_coreID,_paddr,(OSAL_u8 *) __FUNCTION__,__LINE__)
#define OSAL_free(_ptr,_coreID)  __OSAL_free(_ptr,_coreID,(OSAL_u8 *) __FUNCTION__,__LINE__)
#define OSAL_free_constrained(_ptr,_coreID,_size,_paddr)  __OSAL_free_constrained(_ptr,_coreID,_size,_paddr,(OSAL_u8 *) __FUNCTION__,__LINE__)
#define OSAL_realloc(_ptr,_size)  __OSAL_realloc(_ptr,_size,(OSAL_u8 *) __FUNCTION__,__LINE__)
#define OSAL_calloc(_nmemb,_elsize)  __OSAL_calloc(_nmemb,_elsize,(OSAL_u8 *) __FUNCTION__,__LINE__)
#else // define OSAL_MEM_TRACE
#define OSAL_setAllocTraceLevel(...) do{}while(0)
#define OSAL_malloc(_size,_coreID)  __OSAL_malloc(_size,_coreID,NULL,0)
#define OSAL_malloc_constrained(_size,_coreID,_paddr)  __OSAL_malloc_constrained(_size,_coreID,_paddr,NULL,0)
#define OSAL_free(_ptr,_coreID)  __OSAL_free(_ptr,_coreID,NULL,0)
#define OSAL_free_constrained(_ptr,_coreID,_size,_paddr)  __OSAL_free_constrained(_ptr,_coreID,_size,_paddr,NULL,0)
#define OSAL_realloc(_ptr,_size)  __OSAL_realloc(_ptr,_size,NULL,0)
#define OSAL_calloc(_nmemb,_elsize)  __OSAL_calloc(_nmemb,_elsize,NULL,0)
#endif //OSAL_MEM_TRACE

/** OSAL_getenv searches  the  environment  list  to find the environment variable name*/
OSAL_s8*        OSAL_getenv(const OSAL_s8 *name);

/** OSAL_setenv function adds the variable name to the environment with the value [value], if name does not already exist.  If name does exist in the environment, then its value is changed to value if overwrite is non-zero*/
OSAL_s32         OSAL_setenv(const OSAL_s8 *name, const OSAL_s8 *value, OSAL_s32 overwrite);

/** OSAL_putenv function  adds  or  changes  the  value of environment variables name*/
OSAL_s32          OSAL_putenv(OSAL_s8 *name);

/** OSAL_unsetenv function  deletes the variable name from the environment.*/
OSAL_s32         OSAL_unsetenv(const OSAL_s8 *name);

/** OSAL_memmove copies the values of num bytes from the location pointed by source to the memory block pointed by destination*/
OSAL_void * OSAL_memmove ( OSAL_void * destination, const OSAL_void * source, OSAL_u32 num );

/** OSAL_memcopy copies the values of num bytes from the location pointed by source directly to the memory block pointed by destination*/
OSAL_void * OSAL_memcpy ( OSAL_void * destination, const OSAL_void * source, OSAL_u32 num );

/**  OSAL_memcmp  returns an integer less than, equal to, or greater than zero if ptr1 is found, respectively, to be less than, to match, or be greater than ptr2.*/
OSAL_s32        OSAL_memcmp( const OSAL_void * ptr1, const OSAL_void * ptr2, OSAL_u32 num );

/**  OSAL_memset fills the first n bytes of the memory area pointed to by ptr with the constant byte value.*/
OSAL_void*       OSAL_memset( OSAL_void * ptr, OSAL_s32 value, OSAL_u32 num );

/** OSAL_random() get a random number between min and max values (included)*/
OSAL_u32	OSAL_random(OSAL_u32 min, OSAL_u32 max);
#endif
