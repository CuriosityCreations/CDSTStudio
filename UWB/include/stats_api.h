/**
 * @file stats_api.h
 * @brief  API functions to access protocol's statistics 
 * @author cca@bespoon.com
 * @date 11/12/2014
 */
/*
 * Copyright (C) Bespoon 2014. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _STATS_API_H_
#define _STATS_API_H_
#include <types.h>
#ifdef CONFIG_HAVE_STATS
OSAL_error_t pinpointer_get_stats(context_t* contexte, protocol_stats_t* pout);
OSAL_error_t pinpointer_reset_stats(context_t* contexte);

#define GET_STATS(stats,item,val) 	do { val = ((item) < STATS_NB) ?(stats)[(item)]:0;}while(0)
#define INCREASE_STATS(stats,item)	do { if((item) < STATS_NB && (stats)[(item)] < max_u32) (stats)[(item)]++; } while(0)
#define DECREASE_STATS(stats,item)	do { if((item) < STATS_NB && (stats)[(item)] > 0) (stats)[(item)]--; } while(0)
#define SET_STATS(stats,item,value)	do { if((item) < STATS_NB) (stats)[(item)] = to_u32(value); } while(0)
#define RESET_STATS(stats,item) 	do { if((item) < STATS_NB) (stats)[(item)] = 0; } while(0)

#else
#define GET_STATS(stats,item,val) 	do {} while(0)
#define INCREASE_STATS(stats,item)	do {} while(0)
#define DECREASE_STATS(stats,item)	do {} while(0)
#define SET_STATS(stats,item, value)	do {} while(0)
#define RESET_STATS(stats,item)		do {} while(0)

#endif // defined CONFIG_HAVE_STATS

#endif // defined _STATS_API_H_
