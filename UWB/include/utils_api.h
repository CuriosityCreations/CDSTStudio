/**
 * @file utils_api.h
 * @brief Various utils functions
 * @author cca@bespoon.com
 * @date 08/08/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _UTILS_API_H_
#define _UTILS_API_H_
#include <osal_type.h>
/**
 * @brief count number of '1' bits in a u32
 * @param x a u32 in which we want to compute number of '1'
 * @return number of '1' in x
 */
OSAL_u32 ones32(OSAL_u32 x);

/**
 * @brief get logarithm in base 2 of a u32
 * @return log2 of x
 */
OSAL_u32 floor_log2(OSAL_u32 x);

#endif
