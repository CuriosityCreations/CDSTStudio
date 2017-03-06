/**
 * @file types.h
 * @brief header structure 
 * @author bgi@bespoon.com
 * @date 12/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _TYPE_H_
#define _TYPE_H_
#include <osal_type.h>
#include <osal_stdlib.h>
#include <osal_trace.h>
#include <osal_synch.h>
#include <osal_time.h>
#include "config_generic.h" 
#include <pinpointer_common_def.h>
#include <pp_hw_types.h>
#include <time_types.h>
#include <power_mode_types.h>

/** 
 *	@brief Status of the Pinpointer Micro Sequencer (MSQ)
*/
typedef enum {
	NOT_BUSY	= 0,	/**< MSQ Isn't busy */
	BUSY,				/**< MSQ Is busy */
	MSQ_STATUS_NB		/**< MSQ possible status nb */
} OSAL_PACKED_ENUM msq_status;

/** 
 * 	@brief pp_conf start config
 * 	@note this structure is aligned and sized 56 (generic) or 72 (protocol) 
*/
struct pp_conf {
	config_phy_t phy;			/**< config phy */
	OSAL_u32   caps_enabled;		/**< enabled device capabilities for UWB protocol */
	OSAL_bool_t enable_pm;			/**< Does Host control over BAR_RXFE_SP_TXRXDEP and BAR_DIO_TA_VAL or not (not applicable for auto test) */
	OSAL_bool_t pm_msq;			/**< Does MSQ is used to trigger Power Mode changement (applicable also for auto test) */
	OSAL_bool_t enable_live_thld;		/**< is automatic adjustment of synchronisation threshold enabled */

	OSAL_bool_t listen_all_beacons;		/**< Force a time-slave (e.g. non-root) device to always listen to all beacons. False by default (Debug/prototyping only)*/
	OSAL_u16 threshold_coeff;		/**< multiplicative coefficient to threshold  ( a k a agressivness) */

	OSAL_s8 temperature;			/**< current temperature in Celsius Degrees */
	OSAL_bool_t boost_1v2pll;		/**< Is the voltage booster for high temperature enabled */
	OSAL_u32 root_thld[RFPATH_NB][RT_MAX_CHANNEL];	/**< root threshold Tab, used to compute all other thresholds using processing gain model ( 2 RF path , 4 channels) */
	OSAL_s16 user_hw_offset; 		/** < Hardware offset added by user, in picoseconds */
};

//definition is in afc_filter.c
typedef struct filter filter_t;

/**
 *	@brief Live Threshold Adjustment context
 *	@note aligned structure and sized 20 bytes
*/
typedef struct live_threshold_adjustment_ctx
{
	filter_t * thld_filter;		/**< please comment me */
	OSAL_u32 gain_off;		/**< please comment me */
	OSAL_bool_t temp_calib_en;	/**< please comment me */
	OSAL_bool_t thld_filt_1st_iter;	/**< please comment me */
	OSAL_s8 temp;			/**< please comment me */
	OSAL_u8 channel;		/**< please comment me */
	OSAL_u8 rf_path;		/**< prod_root_thld is rf_path dependant and we need to keep track of it */
	OSAL_u8 root_code;		/**< please comment me */
	OSAL_u8 root_rep;		/**< please comment me */
	OSAL_u8 rsvd;			/**< alignement on 32 bits */
	OSAL_u32 prod_root_thld;	/**< please comment me */
} lta_ctx_t;

#ifdef CONFIG_RTLS_PROTOCOL
#include "protocol_types.h"
#else
#include "generic_types.h"
#endif

#endif //_TYPE_H_
