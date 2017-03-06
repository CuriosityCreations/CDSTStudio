/**
 * @file ccal_api.h
 * @brief API for pinpointer Clock Calibration
 * @author cca@bespoon.com
 * @date 18/02/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _CCAL_API_H_
#define _CCAL_API_H_

#include <osal_type.h>

OSAL_s32 ccal_increase_ppb(OSAL_u32 corr_ppb);
OSAL_s32 ccal_decrease_ppb(OSAL_u32 corr_ppb);

#endif

