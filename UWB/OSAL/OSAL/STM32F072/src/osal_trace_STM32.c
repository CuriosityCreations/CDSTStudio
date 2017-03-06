/**
 * @file OSAL_trace_STM32.c
 * @brief STM32 specific trace 
 * @author ore@bespoon.com
 * @date 26/11/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include "osal_trace.h"
#include "ProdSectionDef.h"


#define TRACE_ERR_MASK 		0x01
#define TRACE_WARNING_MASK 	0x02
#define TRACE_INFO_MASK 	0x04
#define TRACE_DEBUG_MASK 	0x08
#define TRACE_VERBOSE_MASK 	0x10
#define TRACE_ANY_MASK 		0x7F

#define VPRINT_FUNCTION(type, fmt, ap) do{\
		printf("["type"]:");\
		vprintf((const char*) fmt,ap);\
		printf(PRINT_ENDOFLINE);\
	}while(0)

#define PRINT_FUNCTION(_fmt, _vmacro) do{\
		va_list ap;\
		va_start(ap, fmt);\
		_vmacro(fmt,ap);\
		va_end(ap);\
	}while(0)
    
#ifdef EN_TRACE_ERR
void ERR(const OSAL_u8* fmt, ...)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_ERR_MASK)
		PRINT_FUNCTION(fmt,VERR);
}

void VERR(const OSAL_u8* fmt, va_list ap)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_ERR_MASK)
		VPRINT_FUNCTION("E",fmt,ap);
}
#endif //EN_TRACE_ERR

#ifdef EN_TRACE_WARNING
void WARNING(const OSAL_u8* fmt, ...)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_WARNING_MASK)
		PRINT_FUNCTION(fmt,VWARNING);
}

void VWARNING(const OSAL_u8* fmt, va_list ap)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_WARNING_MASK)
		VPRINT_FUNCTION("W",fmt,ap);
}
#endif //#ifdef EN_TRACE_WARNING

#ifdef EN_TRACE_INFO
void INFO(const OSAL_u8* fmt, ...)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_INFO_MASK)
		PRINT_FUNCTION(fmt,VINFO);
}

void VINFO(const OSAL_u8* fmt, va_list ap)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_INFO_MASK)
		VPRINT_FUNCTION("I",fmt,ap);
}
#endif //#ifdef EN_TRACE_INFO

#ifdef EN_TRACE_DEBUG
void DBG(const OSAL_u8* fmt, ...)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_DEBUG_MASK)
		PRINT_FUNCTION(fmt,VDBG);
}

void VDBG(const OSAL_u8* fmt, va_list ap)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_DEBUG_MASK)
		VPRINT_FUNCTION("D",fmt,ap);
}
#endif //#ifdef EN_TRACE_DEBUG

#ifdef EN_TRACE_VERBOSE
void VERB(const OSAL_u8* fmt, ...)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_VERBOSE_MASK)
		PRINT_FUNCTION(fmt,VVERB);
}

void VVERB(const OSAL_u8* fmt, va_list ap)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_VERBOSE_MASK)
		VPRINT_FUNCTION("V",fmt,ap);
}

#endif //#ifdef EN_TRACE_VERBOSE

#ifdef EN_TRACE_ANY
void ANY(const OSAL_u8* fmt, ...)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_ANY_MASK)
		PRINT_FUNCTION(fmt,VANY);
}
void VANY(const OSAL_u8* fmt, va_list ap)
{
	if(prodSection.userSection.moduleCfg.tracelevel&TRACE_ANY_MASK)
		VPRINT_FUNCTION("A",fmt,ap);
}
#endif
