/**
 * @file callback.h
 * @brief helper for callback management
 * @author cca@bespoon.com
 * @date 25/02/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _CALLBACK_API_H_
#define _CALLBACK_API_H_

#include <osal_type.h>
#include <callback_types.h>

OSAL_error_t callback_init(callback_ctx_t *cb);

OSAL_error_t callback_set(callback_ctx_t *cb, callback_t fct, OSAL_void* user_data);

OSAL_error_t callback_clear(callback_ctx_t *cb);

OSAL_error_t callback_run(callback_ctx_t* cb, OSAL_void* reason);

#endif
