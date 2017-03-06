/**
 * @file osal_stdlib.c
 * @brief osal_stdlib.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/**
 * AT file osal_stdlib.c
 * AT brief implementation for Pinpointer stdlib functions
 * AT author pboAT bespoon.com
 * AT date 01/03/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include <stdlib.h>
#include <string.h>
#include "osal_trace.h"
#include "osal_stdlib.h"
#include "osal_time.h"
#include "stm32f0xx_adc.h"
#include "stm32f0xx_crc.h"

//#define OSAL_MEM_TRACE_LEVEL 0 //indicate total size allocated
//#define OSAL_MEM_TRACE_LEVEL 1 //LEVEL 0 + core id, size allocated, were its allocated
//#define OSAL_MEM_TRACE_LEVEL 2 //LEVEL 1 + function/line instead of coreId were OSAL_stdlib function has been called
//#define OSAL_MEM_TRACE_LEVEL 3 //LEVEL 2 + but real size and real memory (including alloc overhead on vm) instead of size and mem

//#define OSAL_MEM_TRACE_LEVEL 4 //LEVEL 2 + LEVEL1
#define OSAL_MEM_TRACE_LEVEL 5 // trace LEVEL 34+ LEFT size in RAM + lost size due to overhead


#ifdef OSAL_MEM_TRACE
#ifndef OSAL_MEM_TRACE_LEVEL
	#define OSAL_MEM_TRACE_LEVEL 0 
#endif
#define STM32F072_RAM_END 0x20004000 
static OSAL_u32 RealAllocSize = 0;
static OSAL_u32 AllocSize = 0;
static OSAL_s32 LostSize = 0;
static OSAL_u32 DefltLevel=OSAL_MEM_TRACE_LEVEL;

OSAL_void OSAL_setAllocTraceLevel(OSAL_u32 level)
{
	DefltLevel = level;
}

static OSAL_void traceMalloc(OSAL_u8* allocDesc, OSAL_void *ptr, OSAL_u32 size, OSAL_stackLevel coreID , OSAL_u8 *func, OSAL_u16 line)
{
	OSAL_s32 realsize=size+8;
	if((realsize/8)*8 < realsize)
	realsize=((realsize/8)+1)*8;
	AllocSize += size;
	RealAllocSize += realsize;
	LostSize += (realsize-size);

	
	if(DefltLevel == 0)//indicate total size allocated
		OSAL_trace(TRACE_ANY,"<MEM>%s TOT:x%x/%d",allocDesc,AllocSize,AllocSize);
	else if (DefltLevel == 1)//LEVEL 0 + core id, size allocated, were its allocated
	{
		OSAL_trace(TRACE_ANY,"<MEM>[%d]:%s %d AT x%x",coreID,allocDesc,size,ptr);
		OSAL_trace(TRACE_ANY," TOT:x%x/%d",AllocSize,AllocSize);
	}
	else if (DefltLevel == 2)//LEVEL 1 + function/line were OSAL_stdlib function has been called
	{
		OSAL_trace(TRACE_ANY,"<MEM>[%s][%d]:%s %d AT x%x",func,line,allocDesc,size,ptr);
		OSAL_trace(TRACE_ANY,"TOT:x%x/%d",AllocSize,AllocSize);
	}
	else if (DefltLevel == 3)//LEVEL 2 + but real size and real memory (including alloc overhead on vm) instead of size and mem
	{
		OSAL_trace(TRACE_ANY,"<MEM>[%s][%d]:%s %d (ovhead) AT x%x",func,line,allocDesc,realsize,ptr);
		OSAL_trace(TRACE_ANY,"TOT(ohead):x%x/%d",RealAllocSize,RealAllocSize);
	}
	else if (DefltLevel == 4) //LEVEL 2 + LEVEL1
	{
		OSAL_trace(TRACE_ANY,"<MEM>[%s][%d]:%s %d AT x%x",func,line,allocDesc,size,ptr);
		OSAL_trace(TRACE_ANY,"  With ohead:%d",realsize);
		OSAL_trace(TRACE_ANY,"  TOT:x%x/%d",AllocSize,AllocSize);
		OSAL_trace(TRACE_ANY,"  TOT(ohead):x%x/%d",RealAllocSize,RealAllocSize);
	}
	else // indicate in DefltLevel the end vector or RAM, in this case, trace LEVEL 3 + LEFT size in RAM + lost size due to overhead
	{
		OSAL_trace(TRACE_ANY,"<MEM>[%s][%d] : %s %d AT x%x",func,line,allocDesc,size,ptr);
		OSAL_trace(TRACE_ANY," With ohead:%d",realsize);
		OSAL_trace(TRACE_ANY," TOT:x%x/%d",AllocSize,AllocSize);
		OSAL_trace(TRACE_ANY," TOT(ohead):x%x/%d",RealAllocSize,RealAllocSize);
		OSAL_trace(TRACE_ANY," LEFT before %d/after %d",STM32F072_RAM_END-(OSAL_s32) ptr,STM32F072_RAM_END-(OSAL_s32) ptr-realsize);
		OSAL_trace(TRACE_ANY," LOST %d",LostSize);
	}
}

static OSAL_void traceFree(OSAL_u8* freeDesc, OSAL_void * ptr, OSAL_stackLevel coreID  , OSAL_u8 *func, OSAL_u16 line)
{
	//unfortunately cannot count realsize after a free...
	if(DefltLevel == 0)//indicate total size allocated
		OSAL_trace(TRACE_ANY,"%s",freeDesc);
	else if (DefltLevel == 1)
		OSAL_trace(TRACE_ANY,"[%d]:%s AT x%x",coreID,freeDesc,ptr);
	else
		OSAL_trace(TRACE_ANY,"[%s][%d][%d]:%s AT x%x",func,line,coreID,freeDesc,ptr);
}

#else

#define traceMalloc(...) do{}while(0)
#define traceFree(...) do{}while(0)

#endif //ifdef OSAL_MEM_TRACE

OSAL_void * __OSAL_malloc( OSAL_u32 size, OSAL_stackLevel coreID , OSAL_u8 *func, OSAL_u16 line)
{
	OSAL_void * ptr;
	ptr = malloc(size);	
	traceMalloc((OSAL_u8*) "malloc",ptr,size,coreID,func,line);
	return ptr;
}

OSAL_void*  __OSAL_malloc_constrained( OSAL_u32 size, OSAL_stackLevel coreID ,OSAL_u32 *physical_addr  , OSAL_u8 *func, OSAL_u16 line)
{
	OSAL_void *ptr;	
	ptr = malloc(size);
	traceMalloc((OSAL_u8*) "malloc(cst)",ptr,size,coreID,func,line);
	return ptr;	
}

OSAL_void  __OSAL_free( OSAL_void * ptr, OSAL_stackLevel coreID  , OSAL_u8 *func, OSAL_u16 line)
{ 
	traceFree((OSAL_u8*) "free",ptr,coreID,func,line);
	free(ptr);	
}

OSAL_void  __OSAL_free_constrained( OSAL_void * ptr, OSAL_stackLevel coreID, OSAL_u32 size, OSAL_u32 physical_addr  , OSAL_u8 *func, OSAL_u16 line)
{
	traceFree((OSAL_u8*) "free(cst)",ptr,coreID,func,line);
	free(ptr);
}

#if 0
OSAL_void *         __OSAL_realloc( OSAL_void * ptr, OSAL_u32 size  , OSAL_u8 *func, OSAL_u16 line) { /* TODO*/ return NULL; }

OSAL_void *        __OSAL_calloc( OSAL_u32 nmemb, OSAL_u32 elsize  , OSAL_u8 *func, OSAL_u16 line) { /* TODO*/ return NULL; }

OSAL_s8 *        OSAL_getenv(const OSAL_s8 *name){ /* TODO*/ return NULL;}

OSAL_s32          OSAL_setenv(const OSAL_s8 *name, const OSAL_s8 *value, OSAL_s32 overwrite) {/* TODO*/return 0;}

OSAL_s32          OSAL_putenv(OSAL_s8 *name) {/* TODO */return 0;}

OSAL_s32          OSAL_unsetenv(const OSAL_s8 *name) {/* TODO*/return 0;}

OSAL_void *        OSAL_memmove( OSAL_void * destination, const OSAL_void * source, OSAL_u32 num ) { return memmove(destination, source, num); }


#endif // #if 0

OSAL_void *      OSAL_memcpy( OSAL_void * destination, const OSAL_void * source, OSAL_u32 num ) { return memcpy(destination,source,num); }

OSAL_void*       OSAL_memset( OSAL_void * ptr, OSAL_s32 value, OSAL_u32 num ){return memset(ptr, value, num);}

OSAL_s32        OSAL_memcmp( const OSAL_void * ptr1, const OSAL_void * ptr2, OSAL_u32 num ){ return memcmp(ptr1, ptr2, num);}


static OSAL_u64 randconst = 0x67de456f;
static OSAL_u64 lfsr = 0xA66CFE81;

OSAL_u32	OSAL_random(OSAL_u32 min, OSAL_u32 max)
{
	OSAL_timeval tv;
	OSAL_s64 m,M,r;

	if(min==max) return min;
	if(min>max)
	{
		m=(OSAL_s64)max;
		M=(OSAL_s64)min;
	}
	else
	{
		m=(OSAL_s64)min;
		M=(OSAL_s64)max;
	}
	OSAL_gettimeofday(&tv,NULL);
	lfsr = (((lfsr * ((OSAL_s64) tv.tv_usec)))>>10);
	randconst = ((randconst*(((OSAL_u64)tv.tv_usec)+1))>>10);
	r=((lfsr * randconst)>>32);
	r %= (M+1-m);
	r += m;
	return (OSAL_u32) r;
}
