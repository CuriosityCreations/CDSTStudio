/**
 * @file callback_types.h
 * @brief fully define a callback
 * @author cca@bespoon.com
 * @date 25/02/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _CALLBACK_TYPES_H_
#define _CALLBACK_TYPES_H_

#include <osal_type.h>

typedef OSAL_void (*callback_t)(OSAL_void* reason, OSAL_void* user_data);

typedef struct callback_ctx {
	callback_t fct;
	OSAL_void* user_data;
} callback_ctx_t;
#endif
