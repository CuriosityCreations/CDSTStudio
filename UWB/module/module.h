/**
 * @file module.h
 * @brief common def for module api, includng memory mapping, etc...
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

//include everything to have a single include in apis
#include "stm32f0xx.h"
#include "stm32f0xx_pwr.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_dbgmcu.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_usart.h"
#include <coreId.h>
#include <math.h>
#include "config_api.h"
#include "osal_comp.h"
#include "osal_type.h"
#include "osal_time.h"
#include "osal_trace.h"
#include "osal_math.h"
#include "osal_dac.h"


#include "HAL_config.h"
#include "ADC.h"
#include "dac.h"
#include "I2C.h"
#include "SPI.h"
#include "TIMER.h"
#include "USART.h"
#include "ADC.h"
#include "GPIO.h"
#include "RCC.h"
#include "RTC.h"
#include "ACCELERO.h"
#include "EXTI.h"
#include <pinpointer_regs.h>
#include "pinpointer_hw_api.h"
#include <power_mode_types.h>
#include <msq_config_api.h>
#include "ProdSectionDef.h"
#include "prodSectionTestsDef.h"
#include <time_helper_api.h>
#include <spi_helper.h>
#include <release_note.h>
#include "module_api_internal.h"
#include "uwbmodule_com.h"
#include "Ext_Com.h"


#if defined(PROD)
	#define FIRMWARE_NAME "PROD"
#elif defined(CONFIG_RTLS_PROTOCOL)
	#ifdef CONFIG_MINITAG_V2
		#define FIRMWARE_NAME "MINITAG V2"
	#else	
		#ifdef CONFIG_EVB_REV3
			#define FIRMWARE_NAME "PROTOCOL EVB REV3"
		#else
			#ifdef CONFIG_RTLS_FULL_LOC
				#define FIRMWARE_NAME "PROTOCOL"
			#else
				#define FIRMWARE_NAME "PROTOCOL(limited)"
			#endif
		#endif
	#endif
#else
	#ifdef CONFIG_RTLS_FULL_LOC
		#define FIRMWARE_NAME "GENERIC"
	#else
		#define FIRMWARE_NAME "GENERIC(limited)"
	#endif
	
#endif

#define MAX_SEQ_SIZE 12
#define MAX_TXRX_DATA_SIZE 130 //128 +2bytes for size
#define MAX_RNG_DATA_SIZE sizeof(rng_data_generic_t)
#define MAX_RXFRAME_SIZE (MAX_TXRX_DATA_SIZE+MAX_RNG_DATA_SIZE)
#define MAX_SPI_RW_SIZE 260
#define MAX_SHARED_BUFFER_SIZE_1 OSAL_MAX(MAX_RXFRAME_SIZE,MAX_SEQ_SIZE)

#ifndef CONFIG_RTLS_PROTOCOL
	#define MAX_SHARED_BUFFER_SIZE 		OSAL_MAX(MAX_SHARED_BUFFER_SIZE_1,MAX_SPI_RW_SIZE)
#else
	#define MAX_SHARED_BUFFER_SIZE_2 	OSAL_MAX(MAX_SHARED_BUFFER_SIZE_1,MAX_SPI_RW_SIZE)
	#define MOD_CFG_POS_PROT_MAX_BIGGEST_RNG_BUFFER_BYIT 3
	#define MAX_PROT_RNG_SIZE_FULL 		(sizeof(rng_protocol_header_t)+MOD_CFG_POS_PROT_MAX_BIGGEST_RNG_BUFFER_BYIT*sizeof(rng_protocol_entry_t))
	#define MAX_SHARED_BUFFER_SIZE_3 	OSAL_MAX(MAX_SHARED_BUFFER_SIZE_2,MAX_PROT_RNG_SIZE_FULL)
	#define MAX_SHARED_BUFFER_SIZE 		MAX_SHARED_BUFFER_SIZE_3	
#endif 

#define SHARED_BUFFER_SIZE			MAX_SHARED_BUFFER_SIZE

//Define position of each data needed by module
//this wil allow to concatenate a maximum of informations
//without loosing size due to alignement
#define MOD_CFG_POS_BEGIN					0x0000

#define MOD_CFG_POS_SHARE0					(MOD_CFG_POS_BEGIN)
#define MOD_CFG_POS_SHARE_BUFFER_0_PAD0				(MOD_CFG_POS_SHARE0) //SHARE_BUFFER_0 padding 0, aligned on 32bits
#define MOD_CFG_POS_SHARE_BUFFER_0_PAD1				(MOD_CFG_POS_SHARE0+1) //SHARE_BUFFER_0 padding 1
#define MOD_CFG_POS_SHARE_BUFFER_0_PAD2				(MOD_CFG_POS_SHARE0+2) //SHARE_BUFFER_0 padding 2
#define MOD_CFG_POS_SHARE_BUFFER_0_ACK				(MOD_CFG_POS_SHARE0+3) //SHARE_BUFFER_0 ack position
#define MOD_CFG_POS_SHARE_BUFFER_0_BUF				(MOD_CFG_POS_SHARE_BUFFER_0_ACK)
#define MOD_CFG_POS_SHARE_BUFFER_0_DATA				(MOD_CFG_POS_SHARE_BUFFER_0_BUF+1) //SHARE_BUFFER_0 data position //aligned on 32bits
#define MOD_CFG_POS_SHARE_BUFFER_0_END				(MOD_CFG_POS_SHARE0+SHARED_BUFFER_SIZE-1) //SHARE_BUFFER_0 last data position
#define MOD_CFG_POS_SHARE_BUFFER_0_SIZE_MSB			(MOD_CFG_POS_SHARE_BUFFER_0_END+1) //SHARE_BUFFER_0 Size msb, align on 32 bits
#define MOD_CFG_POS_SHARE_BUFFER_0_SIZE_LSB			(MOD_CFG_POS_SHARE_BUFFER_0_END+2) //SHARE_BUFFER_0 Size lsb
#define MOD_CFG_POS_SHARE_BUFFER_0_CHECKSUM_MSB			(MOD_CFG_POS_SHARE_BUFFER_0_END+3) //SHARE_BUFFER_0 checksum msb
#define MOD_CFG_POS_SHARE_BUFFER_0_CHECKSUM_LSB			(MOD_CFG_POS_SHARE_BUFFER_0_END+4) //SHARE_BUFFER_0 checksum lsb
#define MOD_CFG_POS_SHARE_BUFFER_0_OVERHEAD		 	(MOD_CFG_POS_SHARE_BUFFER_0_DATA-MOD_CFG_POS_SHARE_BUFFER_0_BUF)

#define MOD_CFG_POS_SHARE1					(MOD_CFG_POS_SHARE_BUFFER_0_END+5) //aligned on 32 bits
#define MOD_CFG_POS_SHARE_BUFFER_1_PAD0				(MOD_CFG_POS_SHARE1) //SHARE_BUFFER_1 padding 0, aligned on 32bits
#define MOD_CFG_POS_SHARE_BUFFER_1_PAD1				(MOD_CFG_POS_SHARE1+1) //SHARE_BUFFER_1 padding 1
#define MOD_CFG_POS_SHARE_BUFFER_1_PAD2				(MOD_CFG_POS_SHARE1+2) //SHARE_BUFFER_1 padding 2
#define MOD_CFG_POS_SHARE_BUFFER_1_ACK				(MOD_CFG_POS_SHARE1+3) //SHARE_BUFFER_1 ack position
#define MOD_CFG_POS_SHARE_BUFFER_1_BUF				(MOD_CFG_POS_SHARE_BUFFER_1_ACK)
#define MOD_CFG_POS_SHARE_BUFFER_1_DATA				(MOD_CFG_POS_SHARE_BUFFER_1_BUF+1) //SHARE_BUFFER_1 data position //aligned on 32bits
#define MOD_CFG_POS_SHARE_BUFFER_1_END				(MOD_CFG_POS_SHARE1+SHARED_BUFFER_SIZE-1) //SHARE_BUFFER_1 last data position
#define MOD_CFG_POS_SHARE_BUFFER_1_SIZE_MSB			(MOD_CFG_POS_SHARE_BUFFER_1_END+1) //SHARE_BUFFER_1 Size msb, align on 32 bits
#define MOD_CFG_POS_SHARE_BUFFER_1_SIZE_LSB			(MOD_CFG_POS_SHARE_BUFFER_1_END+2) //SHARE_BUFFER_1 Size lsb
#define MOD_CFG_POS_SHARE_BUFFER_1_CHECKSUM_MSB			(MOD_CFG_POS_SHARE_BUFFER_1_END+3) //SHARE_BUFFER_1 checksum msb
#define MOD_CFG_POS_SHARE_BUFFER_1_CHECKSUM_LSB			(MOD_CFG_POS_SHARE_BUFFER_1_END+4) //SHARE_BUFFER_1 checksum lsb
#define MOD_CFG_POS_SHARE_BUFFER_1_OVERHEAD			(MOD_CFG_POS_SHARE_BUFFER_1_DATA-MOD_CFG_POS_SHARE_BUFFER_1_BUF)

//SYSTEM SPECIFIC
#define MOD_CFG_POS_SYS_BASE					(MOD_CFG_POS_SHARE_BUFFER_1_END+5) //aligned on 32 bits 
#define MOD_CFG_POS_SYS_WAKE_UP_REASON				(MOD_CFG_POS_SYS_BASE) //align to 32 bits
#define MOD_CFG_POS_SYS_MCU_STATE				(MOD_CFG_POS_SYS_BASE+1) //were to find mcu state and if mcu can sleep or not
#define MOD_CFG_POS_SYS_CAPATUNE_VAL				(MOD_CFG_POS_SYS_BASE+2) //were to find capatune val
#define MOD_CFG_POS_SYS_RXFRAME_CFG				(MOD_CFG_POS_SYS_BASE+3) //were to find rxframe config
#define MOD_CFG_POS_SYS_DAC_VAL_MSB				(MOD_CFG_POS_SYS_BASE+4) //were to find dac val(msb) (aligned on 32 bits)
#define MOD_CFG_POS_SYS_DAC_VAL_LSB				(MOD_CFG_POS_SYS_BASE+5) //were to find dac val(msb)
#define MOD_CFG_POS_SYS_STATUS_MSK_MSB				(MOD_CFG_POS_SYS_BASE+8) //were to find status mask msb (aligned on 32 bits)
#define MOD_CFG_POS_SYS_STATUS_MSK_LSB				(MOD_CFG_POS_SYS_BASE+9) //were to find status mask lsb
#define MOD_CFG_POS_SYS_STATUS_MSK_BUSY				(MOD_CFG_POS_SYS_BASE+10) //is status msk currently changing ?
#define MOD_CFG_POS_SYS_BUSY					(MOD_CFG_POS_SYS_BASE+11) //is rxframe cfg msk currently changing
#define MOD_CFG_POS_SYS_STATUS_MSB				(MOD_CFG_POS_SYS_BASE+12) //were to find status 0msb (aligned on 32bits)
#define MOD_CFG_POS_SYS_STATUS_LSB				(MOD_CFG_POS_SYS_BASE+13) //were to find status 0lsb
#define MOD_CFG_POS_SYS_CHECKCACHE				(MOD_CFG_POS_SYS_BASE+14) //last value of check cache asked 
#define MOD_CFG_POS_SYS_LED						(MOD_CFG_POS_SYS_BASE+15) //LED management
#define MOD_CFG_POS_SYS_MODULE_MOVING			(MOD_CFG_POS_SYS_BASE+16) //indicate moving
#define MOD_CFG_POS_SYS_BATLEVEL				(MOD_CFG_POS_SYS_BASE+17) //batterylevel
#define MOD_CFG_POS_SYS_HARDFAULT				(MOD_CFG_POS_SYS_BASE+18) //hardfault detected
#define MOD_CFG_POS_SYS_DIMMER_STATUS			(MOD_CFG_POS_SYS_BASE+19) //dimmer status
#define MOD_CFG_POS_SYS_END						(MOD_CFG_POS_SYS_DIMMER_STATUS) //last value of sys (aligned on 32bits -1)




#define MOD_CFG_BATTEMP_UPDATE_IN_SEC				5
#define MOD_CFG_POS_SYS_INIT_BUSY_MSK				0x01
#define MOD_CFG_POS_SYS_CAPATUNE_BUSY_MSK			0x02
#define MOD_CFG_POS_SYS_DAC_VAL_BUSY_MSK			0x04
#define MOD_CFG_POS_SYS_RXFRAME_CFG_BUSY_MSK			0x08
#define MOD_CFG_POS_SYS_STATUS_MSK_BUSY_MSK			0x10
#define MOD_CFG_POS_SYS_CHECKCACHE_BUSY_MSK			0x20

#define MOD_CFG_POS_SYS_MCU_RUN_FORCED_MASK			0x20
#define MOD_CFG_POS_SYS_MCU_IS_SLEEPING_MASK			0x10
#define MOD_CFG_POS_SYS_MCU_IS_SLEEPING				0x10

#define MOD_CFG_POS_SYS_MCU_SM_RESET_MASK			0xF8
#define MOD_CFG_POS_SYS_MCU_MUST_RUN				0x04
#define MOD_CFG_POS_SYS_MCU_CAN_STOP				0x02
#define MOD_CFG_POS_SYS_MCU_CAN_STANDBY				0x01

//RADIO SPECIFIC
#define MOD_CFG_POS_RADIO_BASE					(MOD_CFG_POS_SYS_END+1)//aligned on 32 bits
#define MOD_CFG_POS_RADIO_PHY_CONFIG_SIZE_MSB			(MOD_CFG_POS_RADIO_BASE)   //current phy config size msb //aligned on 32 bits
#define MOD_CFG_POS_RADIO_PHY_CONFIG_SIZE_LSB			(MOD_CFG_POS_RADIO_BASE+1) //current phy config size lsb
#define MOD_CFG_POS_RADIO_TX_DATA_SIZE_INBITS_MSB		(MOD_CFG_POS_RADIO_BASE+2) //current tx data size msb
#define MOD_CFG_POS_RADIO_TX_DATA_SIZE_INBITS_LSB		(MOD_CFG_POS_RADIO_BASE+3) //current tx data size lsb
#define MOD_CFG_POS_RADIO_RX_CFG				(MOD_CFG_POS_RADIO_BASE+4) //current rx phy cfg //aligned on 32bits
#define MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_MSB		(MOD_CFG_POS_RADIO_BASE+5) //current rx data size msb
#define MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_LSB		(MOD_CFG_POS_RADIO_BASE+6) //current rx data size lsb//aligned on 32bits
#define MOD_CFG_POS_RADIO_TX_ONGOING				(MOD_CFG_POS_RADIO_BASE+7) //tx ongoign
#define MOD_CFG_POS_RADIO_RX_ONGOING				(MOD_CFG_POS_RADIO_BASE+8) //rx ongoing
#define MOD_CFG_POS_RADIO_TX_ABORT				(MOD_CFG_POS_RADIO_BASE+9) //tx ongoign
#define MOD_CFG_POS_RADIO_RX_ABORT				(MOD_CFG_POS_RADIO_BASE+10) //rx ongoing
#define MOD_CFG_POS_RADIO_PM_STATE				(MOD_CFG_POS_RADIO_BASE+11) //current radio pm state
#define MOD_CFG_POS_RADIO_BUSY					(MOD_CFG_POS_RADIO_BASE+12) //is rx cfg currently changed ? //aligned on 32bits

#define MOD_CFG_POS_RADIO_RSVD1					(MOD_CFG_POS_RADIO_BASE+13)
#define MOD_CFG_POS_RADIO_RSVD2					(MOD_CFG_POS_RADIO_BASE+14)
#define MOD_CFG_POS_RADIO_RSVD3					(MOD_CFG_POS_RADIO_BASE+15)
#define MOD_CFG_POS_RADIO_END					(MOD_CFG_POS_RADIO_RSVD3) //last value of radio (aligned on 32bits -1)

#define MOD_CFG_POS_RADIO_INIT_BUSY_MSK				0x01
#define MOD_CFG_POS_RADIO_RX_CFG_BUSY_MSK			0x02
#define MOD_CFG_POS_RADIO_PMSTATE_BUSY_MSK			0x04
#define MOD_CFG_POS_RADIO_SPIWR_BUSY_MSK			0x08
#define MOD_CFG_POS_RADIO_PHYCFG_BUSY_MSK			0x10


#ifdef CONFIG_GENERIC
//SEQUENCER SPECIFIC
#define MOD_CFG_POS_SEQ_BASE					(MOD_CFG_POS_RADIO_END+1) // align on 32bits
#define MOD_CFG_POS_SEQ_CFG0					(MOD_CFG_POS_SEQ_BASE) // were to find cfg0 , align on 32bits
#define MOD_CFG_POS_SEQ_CFG1					(MOD_CFG_POS_SEQ_BASE+1) // were to find cfg1
#define MOD_CFG_POS_SEQ_CFG2					(MOD_CFG_POS_SEQ_BASE+2) // were to find cfg2
#define MOD_CFG_POS_SEQ_STATE					(MOD_CFG_POS_SEQ_BASE+3) // status of sequencer, 0=ongoign, 1 stopped, 2: about to be aborted
#define MOD_CFG_POS_SEQ_BUSY					(MOD_CFG_POS_SEQ_BASE+4) // is seq cfg currently been changed ? //aligne don 32 bits
#define MOD_CFG_POS_SEQ_CFG_BUSY_MSK				0x01
#define MOD_CFG_POS_SEQ_SEQPOSITION_WR				(MOD_CFG_POS_SEQ_BASE+5) // index in circular buffer of sequence (to write in 2nd it of sendExtra sheme)
#define MOD_CFG_POS_SEQ_SEQPOSITION_RD				(MOD_CFG_POS_SEQ_BASE+6) // index in circular buffer of sequence (to read from mainloop)
#define MOD_CFG_POS_SEQ_SEQ_SIZE				12 	//size of each buffer sequence
#define MOD_CFG_POS_SEQ_SEQ0_ACT				(MOD_CFG_POS_SEQ_BASE+8) // first buffer in which sequences (1/4) are stored, aligned on 32bits
#define MOD_CFG_POS_SEQ_SEQ1_ACT				(MOD_CFG_POS_SEQ_SEQ0_ACT+MOD_CFG_POS_SEQ_SEQ_SIZE) // 2nd buffer in which sequences (1/4) are stored, aligned on 32bits, because MOD_CFG_POS_SEQ_SEQ_SIZE is
#define MOD_CFG_POS_SEQ_SEQ2_ACT				(MOD_CFG_POS_SEQ_SEQ1_ACT+MOD_CFG_POS_SEQ_SEQ_SIZE) // 3rd buffer in which sequences (1/4) are stored, aligned on 32bits, because MOD_CFG_POS_SEQ_SEQ_SIZE is
#define MOD_CFG_POS_SEQ_SEQ3_ACT				(MOD_CFG_POS_SEQ_SEQ2_ACT+MOD_CFG_POS_SEQ_SEQ_SIZE) // 4th buffer in which sequences (1/4) are stored, aligned on 32bits, because MOD_CFG_POS_SEQ_SEQ_SIZE is

#define MOD_CFG_POS_SEQ_SEQ3_END				(MOD_CFG_POS_SEQ_SEQ3_ACT+MOD_CFG_POS_SEQ_SEQ_SIZE) // end of cache value for sequence. aligned on 32 bits
#define MOD_CFG_POS_SEQ_RSVD1					MOD_CFG_POS_SEQ_SEQ3_END //rsvd for future use
#define MOD_CFG_POS_SEQ_RSVD2					(MOD_CFG_POS_SEQ_SEQ3_END+1) //rsvd for future use
#define MOD_CFG_POS_SEQ_RSVD3					(MOD_CFG_POS_SEQ_SEQ3_END+2) //rsvd for future use
#define MOD_CFG_POS_SEQ_RSVD4					(MOD_CFG_POS_SEQ_SEQ3_END+3) //rsvd for future use
#define MOD_CFG_POS_SEQ_END						MOD_CFG_POS_SEQ_RSVD4 //alignedon 32bits -1
#define MODULE_CONFIG_SIZE						(MOD_CFG_POS_SEQ_END+1) //aligned on 32 bits

#else  //PROTOCOL SPECIFIC

	#define MOD_CFG_POS_PROT_BASE				(MOD_CFG_POS_RADIO_END+1) // align on 32bits

	#ifndef CONFIG_RTLS_FULL_LOC
		#define MOD_CFG_POS_PROT_NEXT			(MOD_CFG_POS_PROT_BASE) // align on 32bits
	#else
		#define MOD_CFG_PROT_ARTLS_IN			(MOD_CFG_POS_PROT_BASE) //base of artls, aligned on 32 bits
		#define MOD_CFG_PROT_ARTLS_IN_END		(MOD_CFG_PROT_ARTLS_IN+sizeof(artls_down_t)-1) //end of artls_in : @artls_in[25]
		#define MOD_CFG_PROT_ARTLS_IN_SIZE		(MOD_CFG_PROT_ARTLS_IN_END-MOD_CFG_PROT_ARTLS_IN+1) //size of artls_in = artls_down_t
		#define MOD_CFG_PROT_ARTLS_IN_RSVD1		(MOD_CFG_PROT_ARTLS_IN_END+1) //end of artls_in +1 (@artls_in[26])
		#define MOD_CFG_PROT_ARTLS_IN_RSVD2		(MOD_CFG_PROT_ARTLS_IN_END+2) //end of artls_in +2 (@artls_in[27])
		#define MOD_CFG_PROT_ARTLS_IN_AFTER		(MOD_CFG_PROT_ARTLS_IN_END+3)//=> @artls_in[28] == align on 32 bits, because artls_down_t isn't
		
		#define MOD_CFG_PROT_ARTLS_OUT			(MOD_CFG_PROT_ARTLS_IN_AFTER) //base of artls, aligned on 32 bits, because  superframe_info_t is
		#define MOD_CFG_PROT_ARTLS_OUT_END		(MOD_CFG_PROT_ARTLS_OUT+sizeof(artls_up_t)-1) //end of artls_out
		#define MOD_CFG_PROT_ARTLS_OUT_SIZE		(MOD_CFG_PROT_ARTLS_OUT_END-MOD_CFG_PROT_ARTLS_OUT+1) //end of artls_out
		#define MOD_CFG_PROT_ARTLS_OUT_AFTER		(MOD_CFG_PROT_ARTLS_OUT_END+1)//aligned on 32 bits, because artls_up_t is
	
		#define MOD_CFG_PROT_ARTLS_OUT_CHECKSUM_MSB 	(MOD_CFG_PROT_ARTLS_OUT_AFTER) //artls checksum msb align on 32bits, beacuse artls_up_t is
		#define MOD_CFG_PROT_ARTLS_OUT_CHECKSUM_LSB 	(MOD_CFG_PROT_ARTLS_OUT_AFTER+1) //artls checksum lsb

		#define MOD_CFG_POS_PROT_SHADOWBUF_INDEX	(MOD_CFG_PROT_ARTLS_OUT_AFTER+2)//indicate wich buffer to use
		#define MOD_CFG_POS_PROT_ENTRY_NUMBER		(MOD_CFG_PROT_ARTLS_OUT_AFTER+3)//indicate total entry in rng data
		#define MOD_CFG_POS_PROT_LASTENTRY_SIZE_MSB	(MOD_CFG_PROT_ARTLS_OUT_AFTER+4)//last entry size msb, aligned on 32bits
		#define MOD_CFG_POS_PROT_LASTENTRY_SIZE_LSB	(MOD_CFG_PROT_ARTLS_OUT_AFTER+5)//last entry size lsb
		#define MOD_CFG_POS_PROT_SHADOWBUF_RXTOTAL	(MOD_CFG_PROT_ARTLS_OUT_AFTER+6)//toal sfi frame nb
		#define MOD_CFG_POS_PROT_SHADOWBUF_RXCNT		(MOD_CFG_PROT_ARTLS_OUT_AFTER+7)//indicate which sfi frame nb it is
		#define MOD_CFG_POS_PROT_SHADOWBUF_FIRSTSLOT	(MOD_CFG_PROT_ARTLS_OUT_AFTER+8) //current slot managed
		#define MOD_CFG_POS_PROT_SHADOWBUF_LASTSLOT		(MOD_CFG_PROT_ARTLS_OUT_AFTER+9) //min slot in current sfi managed
		#define MOD_CFG_POS_PROT_SHADOWBUF_CURSLOT		(MOD_CFG_PROT_ARTLS_OUT_AFTER+10) //max slot in current sfi managed
		#define MOD_CFG_POS_PROT_SHADOWBUF_RSVD		(MOD_CFG_PROT_ARTLS_OUT_AFTER+11) //rsvd for future use  
		#define MOD_CFG_POS_PROT_NEXT				(MOD_CFG_PROT_ARTLS_OUT_AFTER+12)// align on 32bits

	#endif

	#define MOD_CFG_POS_PROT_CAPS_MSBMSB			(MOD_CFG_POS_PROT_NEXT) //  were to find caps[32....25] aligned o 32bits
	#define MOD_CFG_POS_PROT_CAPS_MSBLSB			(MOD_CFG_POS_PROT_NEXT+1)// were to find caps[24....17]
	#define MOD_CFG_POS_PROT_CAPS_LSBMSB			(MOD_CFG_POS_PROT_NEXT+2)// were to find caps[16....9]
	#define MOD_CFG_POS_PROT_CAPS_LSBLSB			(MOD_CFG_POS_PROT_NEXT+3)// were to find caps[8....1]
	#define MOD_CFG_POS_PROT_BUSY				(MOD_CFG_POS_PROT_NEXT+4)// were to find something busy in protocol// aligned o 32bits
	#define MOD_CFG_POS_PROT_INIT_BUSY_MSK			0x01 //indicate prot init is ongoing, in this case, no further action can be done
	#define MOD_CFG_PROT_SUPERFRAME_BUSY_MSK		0x02
	#define MOD_CFG_PROT_ARTLS_BUSY_MSK			0x04
	#define MOD_CFG_PROT_CAPS_BUSY_MSK			0x08
	#define MOD_CFG_PROT_STATS_BUSY_MSK			0x10

	#define MOD_CFG_POS_PROT_STATUS_CHG_MSB			(MOD_CFG_POS_PROT_NEXT+5)// protocol state = 0:stopped, 1: running
	#define MOD_CFG_POS_PROT_STATUS_CHG_LSB			(MOD_CFG_POS_PROT_NEXT+6)// protocol state = 0:stopped, 1: running
	#define MOD_CFG_POS_PROT_STATE				(MOD_CFG_POS_PROT_NEXT+7)// this is the prostate
	#define MOD_CFG_POS_PROT_DISSOCIATE			(MOD_CFG_POS_PROT_NEXT+8) //MOD_CFG_POS_PROT_DISSOCIATE aligned on 32 bits
	
	#define MOD_CFG_PROT_DISSOCIATE_STOP			0x01 //indicate prot that it should stop on next main loop
	#define MOD_CFG_PROT_DISSOCIATE_REBOOT			0x02 //indicate prot that it should reboot on next main loop

	#define MOD_CFG_POS_PROT_RSVD1				(MOD_CFG_POS_PROT_NEXT+9) //rsvd for future use  
	#define MOD_CFG_POS_PROT_RSVD2				(MOD_CFG_POS_PROT_NEXT+10) //rsvd for future use  
	#define MOD_CFG_POS_PROT_RSVD3				(MOD_CFG_POS_PROT_NEXT+11) //rsvd for future use  
	#define MODULE_CONFIG_SIZE					(MOD_CFG_POS_PROT_NEXT+12) // align on 32 bits
#endif // #ifdef CONFIG_RTLS_PROTOCOL

#define RADIO_FRAMING_CFG_GOLAY_CODEC_MASK			0x01
#define RADIO_FRAMING_CFG_PHR_CRC_MASK				0x02
#define RADIO_FRAMING_CFG_PSDU_CRC_MASK				0x04
#define RADIO_FRAMING_CFG_PSDU_CRC_TYPE_MASK			0x08
#define RADIO_FRAMING_CFG_PSDU_CRC_TYPE_CRC16			0x00
#define RADIO_FRAMING_CFG_PSDU_CRC_TYPE_CRC12			0x01
#define RADIO_FRAMING_CFG_FRAMING_STYLE_MASK			0x30
#define RADIO_FRAMING_CFG_FRAMING_STYLE_LONG_PHR		0x00
#define RADIO_FRAMING_CFG_FRAMING_STYLE_SHORT_PHR		0x10
#define RADIO_FRAMING_CFG_FRAMING_STYLE_NO_PHR			0x20

#define RADIO_STOPPED_ABORTED_BY_USER				0xFFF1
#define RADIO_STOPPED_SFD_TOUT					0xFFF2
#define RADIO_STOPPED_PREAMBLE_TOUT				0xFFF3
#define RADIO_STOPPED_PHR_CRC_ERROR				0xFFF4
#define RADIO_STOPPED_PSDU_CRC_ERROR				0xFFF5
#define RADIO_STOPPED_SIZE_MISMATCH				0xFFF6
#define RADIO_STOPPED_FEC_ERROR					0xFFF7
#define RADIO_FEC_ERROR_LIMITS					3

#define RADIO_VECT0_MSK_TX_DONE					0xFFFE
#define RADIO_VECT0_MSK_SYNCHRO_ESTIMATION_RDY			0xFFFD
#define RADIO_VECT0_MSK_SFD_DETECTED				0xFFFB
#define RADIO_VECT0_MSK_RECEIVED_PHR_VALID			0xFFF7
#define RADIO_VECT0_MSK_RX_DONE					0xFFEF
#define RADIO_VECT0_MSK_SFD_TIME_OUT				0xFFDF
#define RADIO_VECT0_MSK_TX_ACTIVE				0xFFBF
#define RADIO_VECT0_MSK_RX_ACTIVE				0xFF7F
#define RADIO_VECT0_MSK_LCLK_CALIBRATION_FINISHED		0xFEFF
#define RADIO_VECT0_MSK_AFC_CALIBRATION_FINISHED 		0xFDFF
#define RADIO_VECT0_MSK_PREAMB_TIME_OUT				0xFBFF
#define RADIO_VECT0_MSK_PDSU_CRC_OK				0xF7FF
#define RADIO_VECT0_MSK_IR_CONFIRMED				0xEFFF
#define RADIO_VECT0_MSK_ACTION_EVENT				0xDFFF
#define RADIO_VECT0_MSK_PHR_CRC_OK				0xBFFF

#define RADIO_VECT2_TIMER_32K_PERIOD_MASK			0xFEFF
#define RADIO_VECT2_TIMER_32K_ACTION_MASK 			0xFDFF
#define RADIO_VECT2_FR_START_EVENT_MASK				0xFBFF
#define RADIO_VECT2_TS_DELAY_EVENT_MASK				0xF7FF



#define RADIO_VECT0_TX_DONE_MASK				0x01	//on vector W0
#define RADIO_VECT0_RX_DONE_MASK				0x10	//on vector W0
#define RADIO_VECT0_SFD_TOUT_MASK				0x20	//on vector W0
#define RADIO_VECT1_PREAMBLE_TOUT_MASK				0x04	//on vector W1
#define RADIO_VECT1_PDSU_CRC_OK_MASK				0x08	//on vector W1
#define RADIO_VECT1_ACTION_EVENT_MASK				0x20	//on vector W1
#define RADIO_VECT1_PHR_CRC_OK_MASK				0x40	//on vector W1



#define RADIO_CTRLCFG_SFD_TIME_OUT_OFF_MSK			0xFE
#define RADIO_CTRLCFG_SFD_TIME_OUT_ON				0x01
#define RADIO_CTRLCFG_SFD_TIME_OUT_BEHAVIOUR			0x06	//hardcoded TO STOP for now, use mask and co to increase granularity
#define RADIO_CTRLCFG_PREAMB_TIME_OUT_OFF_MSK			0xF7
#define RADIO_CTRLCFG_PREAMB_TIME_OUT_ON			0x08
#define RADIO_CTRLCFG_PREAMB_TIME_OUT_BEHAVIOUR			0x10	//hardcoded TO STOP for now, use mask and co to increase granularity


#define SEQ_TIMER_32K_ACTION_MASK				0x02	//on vector w3
#define SEQ_TS_DELAY_EVENT_MASK					0x08	//on vector w3


#define module_set_busyOFF(_busyU8, _busyMask) module_config[_busyU8] &=~(_busyMask)
#define module_set_busyON(_busyU8, _busyMask)  module_config[_busyU8] |=(_busyMask)

#define module_set_busy(_bstate, _busyU8, _busyMask) do{\
		if(_bstate) module_set_busyON(_busyU8,_busyMask);\
		else module_set_busyOFF(_busyU8,_busyMask);\
	}while(0)

#define module_is_busy(_busyU8, _busyMask) (module_config[_busyU8]&(_busyMask))

extern OSAL_u8 module_config[MODULE_CONFIG_SIZE];
