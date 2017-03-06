/**
 * @file config_protocol.h
 * @brief define constant and switches for the stack
 * @author ore@bespoon.com
 * @date 26/02/2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _CONFIG_PROTOCOL_H_
#define _CONFIG_PROTOCOL_H_

/******************* protocol ***************************/
#define MAX_HYPER_FRAME 			to_s32(255) 	// 2^8 - 1
#define MAX_DYNAMIC_BEACON			to_s32(16) /* in IW*/
#define MAX_DYNAMIC_AFC				to_s32(160)

#define SCAN_MAX_HYPERFRAME_AFC			10			// if scan last less that this number of hyperframe, keep current clock calibration (e.g. don't change AFC status) 10 hyperframes == 20 sec 

#define DEFAULT_RDV_RETRY_CNT			7
#define PAIRING_REQUEST_TIMEOUT			120	// in superframe ==30 seconds

/********************** smbuf *********************************/
#define SMBUFF_POOL_SIZE			6		// This is largely oversized, however, in fast rate 
								// with bsp_loc_bridge, the legacy value of 7 
								// is (sometime) not enought as most frames are waiting
								// in result_loc queue (e.g. deque ranging)
#define SMBUFF_POOL_SIZE_TAG_ONLY		4		// in Tag Only configuration, we only need a reduced number of smbuff
#define SMBUFF_FRAMEBUFF_SIZE			15		// Max size (in bytes) is pairing request frames size (PHR excluded)
#define ARTLS_MSG_POOL_SIZE			8 		// number of artls_t messages. Arbitrary value
#define RDV_MSG_POOL_SIZE			8 		// number of rdv_frame_t messages. Arbitrary value

#define MAX_BASES				8
#define BLOCKED_FRAMES_CNT			10		// Unoptimized, 2 or less should be enought

/******************** radio ****************************/
#define PROTOCOL_CLKTRACK			1
#define PROTOCOL_PRECOMP			0	
#define PROTOCOL_DEFAULT_FRAMER_CFG		0x2D		//NO PHR + CRC PSDU (12bits) +  FEC (PSDU)
#define PROTOCOL_RDV_FRAMER_CFG			0x1F		//short PHR + CRC PHR (4bits) + CRC PSDU (12bits) + FEC (PHR+PSDU)
// #define CONFIG_RF_BG1			1			// Enable access to channels 8-9-10-11

/****************** root threshold (only one allowed in protocol) **********************/
#define LTA_MAX_CHANNEL				1

/********************* sequencer **************************/
#define SEQUENCER_TIMEGUARD			to_u32(64)	// time requied between two sequencer actions (because CPU is too slow) ~ 2.8ms
#define TIMEGUARD_SEQ_COMMAND_WRITE		to_u32(100)	// Time between writing 2 commands on sequencer registers. in us. This look to be needed on demonstrator only, not on module (to be verified)

/********************* config **************************/
#define DEFAULT_DELTA_NTOA			0
#define MAX_LOST_B2				9

/********************* TAG *****************************/
#define DEFAULT_ADDR 				0x0FFF		// 2^12 - 1
#define RANGING_DATA_LEN			2 // number of bits to send in ranging payload, today, only 'move' and 'lowbat' 
#define TSA_CONFIDENCE_NTOA_DIV			16		// NTOA step for tsaConfidence sensor (e.g. 16 = 2ns step)

/********************* AFC *****************************/
#define INITIAL_AFC_FILTER_THRESHOLD		4

#ifdef CONFIG_16BITS_DAC
#define LONG_RUN_AFC_FILTER_THRESHOLD		KALMAN_SPEED_FAST
#else
#define LONG_RUN_AFC_FILTER_THRESHOLD		KALMAN_SPEED_MEDIUM
#endif

#define INIT_AFC_TYPE				B1TOB1 | B2TOB2
#define MAX_DRIFT_AFC_RUNNING			80 /* in ppb */
#define MAX_DRIFT_INIT_AFC			2000 /* in ppb */
#define MAX_INIT_AFC_LOOP			3
#define MAX_WRONG_DRIFT_VALUES			10
#define RERUN_INIT_AFC_THRESHOLD		64 /* in ppb */


/********************* DEBUG **************************/
#ifdef CONFIG_RTLS_FULL_LOC
#define NB_METRIC 				3
#endif // defined CONFIG_RTLS_FULL_LOC

//#define FORCED_RANGING_SLOT_NUMBER	2 // This option enable X forced ranging TX on a tag

#endif
