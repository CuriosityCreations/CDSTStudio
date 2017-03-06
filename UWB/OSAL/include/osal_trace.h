/**
 * @file OSAL_trace.h
 * @brief definitions for Pinpointer log
 * @author bgi@bespoon.com
 * @date 4/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _OSAL_TRACE_H_
#define _OSAL_TRACE_H_

/******* definitions of main PinPointer Log functions where standard C lib available ********/


#include "osal_type.h"
#include "osal_trace_STM32.h"

#define TRACE_ERROR 0
#define TRACE_WARN  1
#define TRACE_INFO  2
#define TRACE_DEBUG 3
#define TRACE_VERBOSE 4
#define TRACE_ANY 5 //Trace always shown as soon as on level is defined


#define OSAL_trace_TRACE_WARN 	WARNING
#define OSAL_trace_TRACE_ERROR 	ERR
#define OSAL_trace_TRACE_INFO 	INFO
#define OSAL_trace_TRACE_DEBUG 	DBG
#define OSAL_trace_TRACE_VERBOSE VERB
#define OSAL_trace_TRACE_ANY 	ANY

#define OSAL_vtrace_TRACE_WARN 	VWARNING
#define OSAL_vtrace_TRACE_ERROR VERR
#define OSAL_vtrace_TRACE_INFO 	VINFO
#define OSAL_vtrace_TRACE_DEBUG VDBG
#define OSAL_vtrace_TRACE_VERBOSE VVERB
#define OSAL_vtrace_TRACE_ANY VANY

#define OSAL_tracer(type) OSAL_trace_##type
#define OSAL_trace(type, fmt, args...) do{OSAL_trace_##type((const OSAL_u8*) fmt, ##args);}while(0)
#define OSAL_vtrace(type, fmt, ap) do{OSAL_vtrace_##type((const OSAL_u8*) fmt, ap);}while(0)

#endif
