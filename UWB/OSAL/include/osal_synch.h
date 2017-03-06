/**
 * @file osal_synch.h
 * @brief definitions for Pinpointer synchronisation functions
 * @author bgi@bespoon.com
 * @date 4/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _OSAL_SYNCH_H_
#define _OSAL_SYNCH_H_

#include "osal_type.h"
/******* definitions of main PinPointer synchronisation functions  ********/

#ifdef CONFIG_HAVE_MULTITHREAD
typedef volatile OSAL_s32 * OSAL_semaphore;

typedef volatile OSAL_s32 * OSAL_mutex;

typedef volatile OSAL_s32 * OSAL_fast_mutex;

extern OSAL_error_t OSAL_createSemaphore (OSAL_semaphore *sema, OSAL_u32 v);

extern OSAL_void OSAL_catchSemaphore(OSAL_semaphore sema);

extern OSAL_error_t OSAL_releaseSemaphore(OSAL_semaphore sema);

extern OSAL_error_t OSAL_destroySemaphore(OSAL_semaphore sema);

extern OSAL_error_t OSAL_createMutex(OSAL_mutex *sema);

extern OSAL_void  OSAL_catchMutex(OSAL_mutex sema);

extern OSAL_error_t OSAL_releaseMutex(OSAL_mutex sema);

extern OSAL_error_t OSAL_destroyMutex(OSAL_mutex sema);

extern OSAL_error_t OSAL_createFastMutex(OSAL_fast_mutex *sema);

extern OSAL_void  OSAL_catchFastMutex(OSAL_fast_mutex sema);

extern OSAL_error_t OSAL_releaseFastMutex(OSAL_fast_mutex sema);

extern OSAL_error_t OSAL_destroyFastMutex(OSAL_fast_mutex sema);
#else

#define OSAL_createSemaphore(sema, v)
#define OSAL_catchSemaphore(sema)
#define OSAL_releaseSemaphore(sema)
#define OSAL_destroySemaphore(sema)
#define OSAL_createMutex(psema)
#define OSAL_catchMutex(sema)
#define OSAL_releaseMutex(sema)
#define OSAL_destroyMutex(sema)
#define OSAL_createFastMutex(psema)
#define OSAL_catchFastMutex(sema)
#define OSAL_releaseFastMutex(sema)
#define OSAL_destroyFastMutex(sema)
#endif

#endif
