/**
 * @file conf_common_api.h
 * @brief Common functions to configure pinpointer
 * @author cca@bespoon.com
 * @date 10/10/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _CONF_COMMON_API_
#define _CONF_COMMON_API_

#include <osal_type.h>

/** @brief Read preamble length = bit size
 */
OSAL_u8 pp_conf_get_preamble_length(OSAL_u8 synch_code_lgth, OSAL_u8 synch_repetition_nb);

/** @brief Compute a CRC on (a part of) the prod section
 */
OSAL_u16 pp_conf_crc16(OSAL_u8 OSAL_PROD_SECT * buf, OSAL_u16 len);

/** @brief helper to get sync spread code length and number of repeated symbols for a given zone
 * @param zone the zone to get code and repetition from
 * @param spread_code output parameter to store spread code length
 * @param repetition output parameter to store repetition number
 */
OSAL_void pp_conf_get_sync_code_repetition_from_zone(OSAL_u8 zone, OSAL_u8* spread_code, OSAL_u8* repetition);
/** @brief helper to get demod spread code length and number of repeated symbols for a given zone
 * @param zone the zone to get code and repetition from
 * @param spread_code output parameter to store spread code length
 * @param repetition output parameter to store repetition number
 */
OSAL_void pp_conf_get_demod_code_repetition_from_zone(OSAL_u8 zone, OSAL_u8* spread_code, OSAL_u8* repetition);

/** @brief get threshold from a different sync code & rep from a reference threshold
 * @param sync_code the sync_code we want threshold from
 * @param sync_rep the sync_rep we want threshold from
 * @param sync_thld_ref reference threshold
 * @param sync_code_ref reference sync_code
 * @param sync_rep_ref reference sync_rep
 */
 OSAL_u32 threshold_from_pg_model(OSAL_u8 sync_code, OSAL_u8 sync_rep, OSAL_u32 sync_thld_ref, OSAL_u8 sync_code_ref, OSAL_u8 sync_rep_ref);
 
#endif
