/**
 * @file callback.c
 * @brief helper for callback management
 * @author cca@bespoon.com
 * @date 25/02/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */


#include <osal_type.h>
#include <callback_api.h>

OSAL_error_t callback_init(callback_ctx_t *cb)
{
	if(cb != NULL)
	{
		cb->fct = NULL;
		cb->user_data = NULL;
	}
	return OSAL_OK;
}

OSAL_error_t callback_set(callback_ctx_t *cb, callback_t fct, OSAL_void* user_data)
{
	if(cb == NULL)
		return OSAL_ERROR;
	
	cb->fct = fct;
	cb->user_data = user_data; 
	return OSAL_OK;
}

OSAL_error_t callback_clear(callback_ctx_t *cb)
{
	if(cb == NULL)
		return OSAL_ERROR;
	
	cb->fct = NULL;
	cb->user_data = NULL; 
	return OSAL_OK;
}

OSAL_error_t callback_run(callback_ctx_t* cb, OSAL_void* reason)
{
	if(cb == NULL)
		return OSAL_ERROR;

	if(cb->fct != NULL)
		cb->fct(reason, cb->user_data);
	return OSAL_OK;
}
