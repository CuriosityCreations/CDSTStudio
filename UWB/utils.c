/**
 * @file utils.c
 * @brief Various utils functions
 * @author cca@bespoon.com
 * @date 08/08/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include <osal_type.h>
#include <utils_api.h>

/*@techreport{magicalgorithms,
author={Henry Gordon Dietz},
title={{The Aggregate Magic Algorithms}},
institution={University of Kentucky},
howpublished={Aggregate.Org online technical report},
URL={http://aggregate.org/MAGIC/}
*/

OSAL_u32 ones32(OSAL_u32 x)
{
	// 32-bit recursive reduction using SWAR...
	// but first step is mapping 2-bit values
	// into sum of 2 1-bit values in sneaky way
	x -= ((x >> 1) & 0x55555555);
	x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
	x = (((x >> 4) + x) & 0x0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);
	return(x & 0x0000003f);
}

// Stolen from http://aggregate.org/MAGIC/
OSAL_u32 floor_log2(OSAL_u32 x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
#ifdef	LOG0UNDEFINED
	return(ones32(x) - 1);
#else
	return(ones32(x >> 1));
#endif
}

