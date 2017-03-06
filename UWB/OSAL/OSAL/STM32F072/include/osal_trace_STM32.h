/**
 * @file OSAL_trace_STM32.h
 * @brief definitions for Pinpointer log
 * @author bgi@bespoon.com
 * @date 8/10/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */
#include <stdio.h>
#include <stdarg.h>

#ifndef _osal_STM32_TRACE_H_
#define _osal_STM32_TRACE_H_

#define PRINT_ENDOFLINE "\r\n\r"

#define EN_TRACE_ERR 1
#define EN_TRACE_ANY 1
#define EN_TRACE_WARNING 1
#define EN_TRACE_INFO 1
#define EN_TRACE_DEBUG 1
#define EN_TRACE_VERBOSE 1


void PrintChar(char c);
//fast convenient macro
#define print(fmt,args...)  printf(fmt,##args)
#define printline_fct  printf
#define printline(fmt,args...)  printf(fmt ""PRINT_ENDOFLINE,##args)

#ifdef EN_TRACE_ERR
void ERR(const OSAL_u8* fmt, ...);
void VERR(const OSAL_u8* fmt, va_list ap);
#else
#define ERR(...) do{}while(0)
#define VERR(...) do{}while(0)
#endif

#ifdef EN_TRACE_WARNING
void WARNING(const OSAL_u8* fmt, ...);
void VWARNING(const OSAL_u8* fmt, va_list ap);
#else
#define WARNING(...) do{}while(0)
#define VWARNING(...) do{}while(0)
#endif

#ifdef EN_TRACE_INFO
void INFO(const OSAL_u8* fmt, ...);
void VINFO(const OSAL_u8* fmt, va_list ap);
#else
#define INFO(...) do{}while(0)
#define VINFO(...) do{}while(0)
#endif

#ifdef EN_TRACE_DEBUG
void DBG(const OSAL_u8* fmt, ...);
void VDBG(const OSAL_u8* fmt, va_list ap);
#else
#define DBG(...) do{}while(0)
#define VDBG(...) do{}while(0)
#endif

#ifdef EN_TRACE_VERBOSE
void VERB(const OSAL_u8* fmt, ...);
void VVERB(const OSAL_u8* fmt, va_list ap);
#else
#define VERB(...) do{}while(0)
#define VVERB(...) do{}while(0)
#endif

#ifdef EN_TRACE_ANY
void ANY(const OSAL_u8* fmt, ...);
void VANY(const OSAL_u8* fmt, va_list ap);
#else
#define ANY(...) do{}while(0)
#define VANY(...) do{}while(0)
#endif

#endif
