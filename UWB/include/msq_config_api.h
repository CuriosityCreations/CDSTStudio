/**
 * @file msq_config_api.h
 * @brief This API is used to safely configure the MSQ
 * @author gpa@bespoon.com
 * @date 01/02/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef MSQ_CONFIG_API_H
#define MSQ_CONFIG_API_H

/** @brief initialise MSQ 
 *  @param ctx contexte pinpointer_data
 *  @return void
 */
OSAL_void msq_init(OSAL_void* ctx);

/** @brief initialise MSQ memories PMEM and DMEM
 *  @param ctx contexte pinpointer_data
 *  @return void
 */
OSAL_void msq_mem_init(OSAL_void* ctx);

/** @brief load a firmware code in MSQ Program MEMory
 *  @param ctx contexte pinpointer_data
 *  @param fw firmware to load in PMEM
 *  @param fw_size size of firmware to load
 *  @return void
 */
OSAL_void msq_ld_fw(OSAL_void* ctx, OSAL_u8 OSAL_PROD_SECT * fw, OSAL_u16 fw_size);

/** @brief load a power mode sequence in DMEM
 *  @param ctx contexte pinpointer_data
 *  @return void
 */
OSAL_void msq_ld_seq(OSAL_void* ctx);

/** @brief enable MSQ
 *  @param ctx contexte pinpointer_data
 *  @return void
 */
OSAL_void msq_start(OSAL_void* ctx);

/** @brief test if MSQ IRQ have be properly trigged
 *  @param ctx contexte pinpointer_data
 *  @param pm_tab settings tab to trig to msq
 *  @return void
 */
OSAL_void msq_seq_write(OSAL_void* ctx, struct setting OSAL_PROD_SECT * pm_tab);

/** @brief test if MSQ is running (need adapted firmware)
 *  @param ctx contexte pinpointer_data
 *  @return true if MSQ OK, false if not
 */
OSAL_bool_t msq_run_test(OSAL_void* ctx);

/** @brief Get MSQ Status 
 *  @param ctx contexte pinpointer_data
 *  @return msq_status: NOT_BUSY(0), BUSY(1)
 */
msq_status msq_get_state(OSAL_void* ctx);

/** @brief disable MSQ
 *  @param ctx contexte pinpointer_data
 *  @return void
 */
OSAL_void msq_stop(OSAL_void* ctx);

/** @brief Configure MUX for MSQ usage
 *  @param ctx contexte pinpointer_data
 *  @return void
 */
OSAL_void msq_mux_cfg(OSAL_void* ctx, OSAL_bool_t pm_msq, OSAL_bool_t enable_pm);

/** @brief Start DCO
 *  @param ctx contexte pinpointer_data
 *  @return void
 */
OSAL_void msq_DCO_start(OSAL_void* ctx, OSAL_bool_t pm_msq);

/** @brief Stop DCO
 *  @param ctx contexte pinpointer_data
 *  @return void
 */
OSAL_void msq_DCO_stop(OSAL_void* ctx, OSAL_bool_t pm_msq);

#endif 
//MSQ_CONFIG_API_H