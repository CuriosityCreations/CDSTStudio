/**
 * @file time_types.h
 * @brief definitions of various times used in RTLS protocol
 * @author cca@bespoon.com
 * @date 23/09/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _TIME_TYPES_H_
#define _TIME_TYPES_H_
#include <osal_type.h>


typedef struct protocol_time {
	OSAL_u32 ts;
	OSAL_u32 mega_frame;
	OSAL_u16 hyper_frame;
	OSAL_u8  super_frame;
	OSAL_u8	 zone;
} protocol_time_t;

// Pinpointer digital input
typedef struct sequencer_time {
	OSAL_u32 it;
	OSAL_u32 ts;
	OSAL_u32 ntoa;
} sequencer_time_t;

// Pinpointer rx output
typedef struct coarse_rng {
	OSAL_u32 ep;
	OSAL_s32 toa;
	//TODO add fine DLL data
} coarse_rng_t;

typedef struct relative_toa
{
	OSAL_s32 iw;   // in IW (~1ns)
	OSAL_s32 ntoa; // in NTOA (~125ps)
}rtoa_t;


#endif
