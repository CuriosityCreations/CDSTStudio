/**
 * @file osal_time.c
 * @brief OSAL implementation for timers and delay functions
 * @author pbo@bespoon.com
 * @date 8/01/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include <osal_type.h>
#include <osal_time.h>

OSAL_s32 OSAL_gettimeofday(OSAL_timeval *tv, OSAL_timezone *tz)
{
	HAL_RTC_GetTime(tv);
	return 0;
}


OSAL_u32 OSAL_timercmp(OSAL_timeval *tvaPTR, OSAL_timeval *tvbPTR, OSAL_u8 CMP)
{
	//CMP is '<','>' or '=' for the moment
	switch(CMP)
	{
		case '>':
			if(tvaPTR->tv_sec>tvbPTR->tv_sec) return 1;
			if(tvaPTR->tv_sec<tvbPTR->tv_sec) return 0;
			if(tvaPTR->tv_usec>tvbPTR->tv_usec) return 1;
			return 0;
		case '<':
			if(tvbPTR->tv_sec>tvaPTR->tv_sec) return 1;
			if(tvbPTR->tv_sec<tvaPTR->tv_sec) return 0;
			if(tvbPTR->tv_usec>tvaPTR->tv_usec) return 1;
			return 0;
		default:
			if(
				(tvbPTR->tv_sec==tvaPTR->tv_sec)&&
				(tvbPTR->tv_usec>tvaPTR->tv_usec)
			) return 1;
			return 0;
	}
}

OSAL_void OSAL_timersub(OSAL_timeval *a, OSAL_timeval *b, OSAL_timeval *diff)
{
	//tvaPTR SHOULD be > tvbPTR, and diff of time won't be > 1day
	//if its not the case, tvaPTR will be considered as tvaPTR+24h
	(diff)->tv_sec = (a)->tv_sec - (b)->tv_sec;
	(diff)->tv_usec = (a)->tv_usec - (b)->tv_usec;
	if ((diff)->tv_usec < 0) {
		--(diff)->tv_sec;
		(diff)->tv_usec += 1000000;
	}
}

OSAL_void OSAL_timersub_realtime(OSAL_timeval *a, OSAL_timeval *b, OSAL_timeval *diff)
{
	OSAL_timeval newa;
	newa.tv_sec=a->tv_sec;
	newa.tv_usec=a->tv_usec;
	if(OSAL_timercmp(&newa,b,'<')) newa.tv_sec+=86400;//here a must be > b. if not, we passed the day
	OSAL_timersub(&newa,b,diff);
	OSAL_get_realtime(diff);
}

OSAL_void OSAL_timeradd(OSAL_timeval *a, OSAL_timeval *b, OSAL_timeval *sum)
{
	(sum)->tv_usec=(a)->tv_usec+(b)->tv_usec;
	(sum)->tv_sec=(a)->tv_sec+(b)->tv_sec;
	if ((sum)->tv_usec >= 1000000) 
	{
		++(sum)->tv_sec;
		(sum)->tv_usec -= 1000000;
	}
}

//apply correction on a system time, to have real time
OSAL_void OSAL_get_realtime(OSAL_timeval *tim)
{
	HAL_RTC_Get_RealTime(tim);
}
//apply correction on a real time, to have system time
OSAL_void OSAL_get_systemtime(OSAL_timeval *tim)
{
	HAL_RTC_Get_SysTime(tim);
}

OSAL_s32 OSAL_timerisset(OSAL_timeval *tvaPTR) 
{
	return (tvaPTR->tv_sec !=0)?1:(tvaPTR->tv_usec !=0)?1:0;
}
