/**
 * @file osal_time.h
 * @brief OSAL definitions for timers and delay functions
 * @author pbo@bespoon.com
 * @date 8/01/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */
#include "TIMER.h"
#include "RTC.h"

#ifndef _OSAL_TIME_H_
#define _OSAL_TIME_H_

/******* definitions of main PinPointer timers/delay functions  ********/

/** OSAL_udelay create a tempo of usec_delay micro-seconds, cpu not sleeping or accurate timer*/
#define	 OSAL_udelay( usec_delay ) udelay(usec_delay);

/** OSAL_msleep create a sleep time of msec_delay milli-seconds, cpu may sleep, not maximum accuracy */
#define	 OSAL_msleep( msec_sleep ) msleep(msec_sleep)

//this always return system time, aka not corrected, between 0 and 24H
//if you want to do any operation on real time value, you should use OSAL_get_realtime after
OSAL_s32 OSAL_gettimeofday(OSAL_timeval *tv, OSAL_timezone *tz);

//CMP is '<','>' or '=' for the moment
OSAL_u32 OSAL_timercmp(OSAL_timeval *tvaPTR, OSAL_timeval *tvbPTR, OSAL_u8 CMP);

//do a-b into diff
//you should check if a > b through OSAL_timercmp
//if you're sure b>a, and b<a , it can de due to <0-24> limit of gettimeofday
//add 24h to b then
OSAL_void OSAL_timersub(OSAL_timeval *a, OSAL_timeval *b, OSAL_timeval *diff);
//use this if a and b have been got through gettimeof day. they are system time, and you want ot have a real time difference
OSAL_void OSAL_timersub_realtime(OSAL_timeval *a, OSAL_timeval *b, OSAL_timeval *diff);
//do a+b into sum
//if one timeval a or b is real time, younshould use OSAL_get_systemtime on it first
OSAL_void OSAL_timeradd(OSAL_timeval *a, OSAL_timeval *b, OSAL_timeval *sum);
//apply correction on a system time, to have real time
OSAL_void OSAL_get_realtime(OSAL_timeval *tim);
//apply correction on a real time, to have system time
OSAL_void OSAL_get_systemtime(OSAL_timeval *tim);
//chgeck if time is non zero
OSAL_s32 OSAL_timerisset(OSAL_timeval *tvaPTR);

#endif
