/**
 * @file osal_comp.c
 * @brief implementation for Pinpointer comparison functions
 * @author pbo@bespoon.com
 * @date 01/03/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include <ctype.h>
#include "../../../include/osal_comp.h"

OSAL_s32          OSAL_islower(OSAL_s32 i){return islower(i);}

OSAL_s32          OSAL_isupper(OSAL_s32 i){return isupper(i);}
