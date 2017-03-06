/**
 * @file scan_rf_types.h
 * @brief Types for Scan RF
 * @author cca@bespoon.com
 * @date 07/10/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _SCAN_RF_TYPES_H_
#define _SCAN_RF_TYPES_H_

//states of scan_rf state machine
typedef enum {
	SCAN_START = 0,		// Scan RF must be started
	SCAN_RUNNING,	 	// currently scanning to find a beacon1 or a beacon2
	SCAN_CATCH,	 	// got a beacon1 or a beacon2 but not both
	SCAN_DONE,		// nothing to do regarding the scan
} OSAL_PACKED_ENUM scan_rf_status;

typedef struct scan_rf_state_machine
{
	OSAL_u32 next_scan_ts;
	OSAL_u32 rx_cnt;
	scan_rf_status status;  /** < manage the state scan_rf state machine */
	OSAL_u8 zone;
	OSAL_u8 channel;
	OSAL_bool_t b1_ok;
	OSAL_bool_t b2_ok;
	OSAL_u8 rsvd[3];
} scan_rf_state_machine_t;
#endif // !defined  _SCAN_RF_TYPES_H_
