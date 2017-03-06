/**
 * @file types.h
 * @brief header structure smbuff
 * @author bgi@bespoon.com
 * @date 12/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _GENERIC_TYPE_H_
#define _GENERIC_TYPE_H_

/** 
 *	@brief Internal context
*/
struct pp_seq
{
	struct pp_conf * cfg_ctx;
	OSAL_void*	hw_priv_data;
};

/** 
 *	@brief Main context
*/
typedef struct context{
	struct pp_seq sequencer;
	struct pp_conf * cfg_ctx;
	OSAL_void * hw_priv_data;		/**< Contexte for SPI access */
	lta_ctx_t lta_ctx[LTA_MAX_CHANNEL];	/**< Live Threshold Adjustment context */
	OSAL_void* ctxMemPool;
} context_t;

#endif //_GENERIC_TYPE_H_
