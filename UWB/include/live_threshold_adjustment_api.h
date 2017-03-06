/**
 * @file live_threshold_adjustment_api.h
 * @brief api for the use of automatic live threshold adjustment
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _LIVE_THRESHOLD_ADJUSTMENT_API_H_
#define _LIVE_THRESHOLD_ADJUSTMENT_API_H_

#include <types.h>

/** @brief Initialize LiveThresholdAdjustment library
 *  @param lta_ctx : pointer to allocated but not initialized lta context
 *  @param mem_pool : pointer to memory pool (filter allocation done by library) 
*/
OSAL_void lta_init(lta_ctx_t *lta_ctx, void *mem_pool);

/** @brief Initialize LiveThresholdAdjustment filter
 *  @param lta_ctx  : pointer to initialized lta_context
 *  @param temp     : current system temperature in Celsius degrees
*/
OSAL_void lta_init_filter_from_temperature(lta_ctx_t *lta_ctx, OSAL_s8 temp);


/** @brief set mandatory parameters
 *  @param lta_ctx        : pointer to initialised lta_context
 *  @param channel        : current system UWB channel
 *  @param rf_path        : current system UWB rf path
 *  @param root_code      : code of root noise
 *  @param root_rep       : repetition of root noise
 *  @param prod_root_thld : producion thld for root_code and rep_code
*/
OSAL_void lta_set_params(lta_ctx_t *lta_ctx, OSAL_u8 channel, OSAL_u8 rf_path, OSAL_u8 root_code, OSAL_u8 root_rep, OSAL_u32 prod_root_thld);

/** @brief get the channel
 *  @param lta_ctx : pointer to initialized lta_context
 *  @return the channel
 */
OSAL_u8 lta_get_channel(lta_ctx_t *lta_ctx);

/** @brief get the rf_path
 *  @param lta_ctx : pointer to initialised lta_context
 *  @return the rf_path
 */
OSAL_u8 lta_get_rf_path(lta_ctx_t *lta_ctx);

/** @brief reset LiveThresholdAdjustment state
 *  @param lta_ctx : pointer to initialized lta_context
*/
OSAL_void lta_reset_live_threshold_adjustment(lta_ctx_t *lta_ctx);

/** @brief return the current output state of LiveThreshold filter (THE threshold)
 *  @param lta_ctx : pointer to initialized lta_context
 *  @return        : THE threshold for the default zone
*/
OSAL_u32 lta_get_live_root_threshold(lta_ctx_t *lta_ctx);

/** @brief compute a new threshold from an Impulse response (IR)
 *  @param lta_ctx : pointer to initialized lta_context
 *  @param code : the code (7,15,31,63,127) for which we want threshold to be computed
 *  @param rep : the repetition (from 1 to 64) for which we want threshold to be computed
 *  @param nrj[] : table of NRJ (64 * 32 bits values)
 *  @param prp[] : table of prp (64 * 8 bits values)
 *  @note        : The raw threshold computed is pushed in internal filter
*/
OSAL_void lta_compute_root_threshold_from_IR(lta_ctx_t *lta_ctx, OSAL_u8 code, OSAL_u8 rep,
					 OSAL_u32 nrj[MAX_CHANNEL_ESM], OSAL_u8 prp[MAX_CHANNEL_ESM]);

/** @brief compute a new threshold from temperature value
 *  @param lta_ctx : pointer to initialized lta_context
 *  @param temp    : current system temperature in Celsius Degrees
 *  @note        : The raw threshold computed is pushed in internal filter
*/
OSAL_void lta_compute_root_threshold_from_temperature(lta_ctx_t *lta_ctx, OSAL_s8 temp);

#endif
