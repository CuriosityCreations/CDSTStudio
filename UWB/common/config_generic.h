/**
 * @file config_generic.h
 * @brief define constant and switches for the stack
 * @author pbo@bespoon.com
 * @date 08/01/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _CONFIG_GENERIC_H_
#define _CONFIG_GENERIC_H_

#ifdef CONFIG_RTLS_PROTOCOL
  #include "config_protocol.h"
#endif

/** @brief MAJMIN_TO_VERSION macro to embed 2 u8 representing version maj/min version into a single u8 (0.0->15.15) */
#define MAJMIN_TO_VERSION(_maj,_min) (((_maj & 0x0F)<<4) + (_min & 0x0F ))

//FIXME: this hw version is obsolete, everything should be taken from prodsection
//current is ppv3bis =>3.1
#define HW_VERSION_MAJ				3 //[0-15]
#define HW_VERSION_MIN				1 //[0-15]

/******************** radio ****************************/
#define SFD_SIZE_BIT				to_u32(16)	// in bits
#define DEFAULT_ZONE				4		// C127R8/C127R1+FEC
#define DEFAULT_RFPATH				2
#define RFPATH_NB					4
/*************************** PM ***************************/
#define PM_DURATION				to_u32(64)	// Typical time to go from a PM to another in TS, ~ 1.9ms
#define STDBY_TO_TXIDLE_DURATION		PM_DURATION	// To be refine
#define STDBY_TO_RXIDLE_DURATION		PM_DURATION	// To be refine
#define IDLE_TO_STDBY_DURATION			PM_DURATION	// Considere the same duration to go from TX_IDLE to STDBY and RX_IDLE to STDBY, to be refine
#define PM2_TO_STDBY_DURATION			to_u32(192)	// In TS, ~ 5.8ms. Currently higher than correct value because of TS-DELAY bug
#define STDBY_TO_PM2_DURATION			to_u32(32)	// In TS, ~ 1ms

#define EXTERNAL_PA_WAKEUP_DURATION		to_u32(1150) 	// in TS, Dduration for external PA (if any) to wake up, ~ 35ms
/********************* config **************************/
#define MAX_PHY_CONF_SIZE_CONFIG_TAB		100		//this is used to define size to allocate (100*sizeofsetting)=1200bytes
#define MAX_PHY_CONF_SIZE			15		// Should work at 10 (highest phy conf size currently, but still subject to change)
#define MAX_PHY_CONF_SIZE_SFI_TAB		7		// Used in on_sfi_changed, only to change sync_code_id and possibly thresholds (4 for code_id, 2 for thresholds, 1 for stop)
#define OFFSET_RX				1		// Negative offset, in TS
#define DEFAULT_PREAMBLE_TIMEOUT		to_u32(NANO_TO_EP(5000000)) //5ms
#define HYBRID_TOA 					1	// Enable Hybrid TOA (FML / FAT)

/********************* MSQ *****************************/
//#define NO_FORCESHADOW			1	
//#define CONFIG_HW_METRIC 1 //firmware 02 needed at least
#define STAT_TABLE_SIZE				24
#define CLK_DIVA_HIGH				1 // Clock division of MSQ: 26MHz / 2^x
#define CLK_DIVA_LOW				1


/****************** root threshold (default values if not set by protocol) **********************/
#define RT_MAX_CHANNEL				4

#ifdef LTA_MAX_CHANNEL
	#undef LTA_MAX_CHANNEL
#endif
#define LTA_MAX_CHANNEL				4

/******************** IRQ Masks ************************/
#define ENABLED_IRQ_VECT0_PM2			~(RD_ACTION_EVENT | RD_LCLK_CALIBRATION_FINISHED)
#define ENABLED_IRQ_VECT0			~(RD_ACTION_EVENT | RD_LCLK_CALIBRATION_FINISHED | RD_TX_DONE | RD_PREAMB_TIME_OUT | RD_SFD_TIME_OUT | RD_RX_DONE | RD_SYNCHRO_ESTIMATION_RDY | RD_SFD_DETECTED | RD_PDSU_CRC_OK)
#define ENABLED_IRQ_VECT2_PM2			~(RD_TS_DELAY_EVENT)
#define ENABLED_IRQ_VECT2			~(0x0000)


/**************** Asset Tracking **********************/
#ifdef CONFIG_ASSET_TRACKING
	#undef STDBY_TO_RXIDLE_DURATION
	#define STDBY_TO_RXIDLE_DURATION		128
	#undef OFFSET_RX
	#define OFFSET_RX				50			// Negative offset, in TS
#endif
#define AT_DEFAULT_STARTUP_DURATION			1500			// Time to init MCU and PP, in TS
#define SEQUENCER_SHIFT_TARGET 				(OFFSET_RX + 10)	// Sequencer_shift target value
#define SEQUENCER_SHIFT_HYSTERESIS 			10			// Adjust sleep if sequencer_shift deviates too much from SEQUENCER_SHIFT_TARGET

#endif //_CONFIG_GENERIC_H_
