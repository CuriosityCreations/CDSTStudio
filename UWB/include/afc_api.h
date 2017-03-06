/**
 * @file afc_api.h
 * @brief AFC state machine main API
 * @author bgi@bespoon.com
 * @date 22/04/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */
 
#ifndef _AFC_API_H_
#define _AFC_API_H_

typedef enum {
	NO_AFC_TYPE = 0,	/**< Don't estimate 26Mhz clock drift */
	CLOCK_TRACK = 1,	/**< Estimate drift using metrics from clock track */
	B1TOB2 = 2,		/**< Estimate drift using TOA of Beacon 1 and Beacon 2 of the same superframe */ 
	B1TOB1 = 4, 		/**< Estimate drift using TOA of Beacon 1 from one superframe to another */
	B2TOB2 = 8,		/**< Estimate drift using TOA of Beacon 2 from one superframe to another */
} OSAL_PACKED_ENUM afc_type;

OSAL_void afc_init(context_t* contexte);

OSAL_void afc_add_beacon_data(context_t * contexte, OSAL_u8 beacon_nb, OSAL_s32 err_beacon, protocol_time_t time, OSAL_u8 beacon_id);
OSAL_void afc_reset_beacon_data(context_t* contexte);
OSAL_void afc_update_sfi(context_t* contexte);

OSAL_bool_t afc_run_fsm(context_t * contexte, smbuff_t* recv_smbuff);

OSAL_void set_afc_status(context_t * contexte, afc_status afc);
OSAL_u8 get_afc_status(context_t * contexte);
OSAL_u8 get_afc_type(context_t * contexte);

#endif 
