/**
 * @file module_api_def.h
 * @brief api enum definition for module
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */
 
#ifdef CONFIG_RTLS_PROTOCOL
#include "module_api_def_protocol.h"
#endif


#ifndef __MODULE_API_DEF_H__
#define __MODULE_API_DEF_H__


/** @page module_api Module API
 * 
*/
/**
* @brief SYS_UUID_VALUE unique UUID of the mcu
*/
#define SYS_UUID_VALUE				0x1FFFF7AC

/**
 * @brief UWB_P_NOPARAMETER : no parameter value
*/
#define UWB_P_NOPARAMETER 0xFFFFFFFF


#ifdef CONFIG_GENERIC
/**
 * @brief uwb_com_subcmd_e
 * @note Sub commands definition.This must be used as parameter in call of 
 * - #sys_set_mcu_state
 * - #sys_get_info
 * - #radio_set_pmstate
 * - #radio_phy_ctrl
 * - #radio_get_rx_data_size
 * - #seq_ctrl
*/
#else
/**
 * @brief uwb_com_subcmd_e
 * @note Sub commands definition.This must be used as parameter in call of 
 * - #sys_set_mcu_state
 * - #sys_get_info
 * - #radio_set_pmstate
 * - #prot_ctrl
*/
#endif



typedef enum {
	UWB_SC_NONE         = 0xFF,	/**< no sub command */
	UWB_SC_START        = 0x01,	/**< Sub command 'START' */
	UWB_SC_STOP         = 0x02,	/**< Sub command 'STOP' */
	UWB_SC_STATUS       = 0x03,	/**< Sub command get status */
	UWB_SC_STATE        = 0x04,	/**< Sub command get state */
	UWB_SC_SIZE         = 0x04,	/**< Sub command get size */
	UWB_SC_DATA         = 0x05,	/**< Sub command get data */
	UWB_SC_CHECKSUM     = 0x06,	/**< Sub command get checksum (u8 checksum)*/
	UWB_SC_POP          = 0x07,     /**< Sub command clear packet */
	//specific to RADIO_PHY_CTRL Start
	UWB_SC_STARTSTOP_TX = 0x01,	/**< Sub command 'START/STOP' will be done for TX */
	UWB_SC_STARTSTOP_RX = 0x02,	/**< Sub command 'START/STOP' will be done for TX' */
	//specific to sys mcu state
	UWB_SC_MCU_STANDBY  = 0x00,	/**< Sub command put MCU In standby */
	UWB_SC_MCU_STOPPED  = 0x01,	/**< Sub command Authorize mcu to sleep */
	UWB_SC_MCU_RUN      = 0x02,	/**< Sub command Force mcu to stay in run state*/
	UWB_SC_MCU_RESET    = 0x03,	/**< Sub command reset mcu with msq ON, PM ON, LTA ON*/
	UWB_SC_MCU_RESET_110= 0x04,	/**< Sub command reset mcu with msq ON, PM ON, LTA OFF*/
	UWB_SC_MCU_RESET_101= 0x05,	/**< Sub command reset mcu with msq ON, PM OFF, LTA ON*/
	UWB_SC_MCU_RESET_100= 0x06,	/**< Sub command reset mcu with msq ON, PM OFF, LTA OFF*/
	UWB_SC_MCU_RESET_011= 0x07, /**< Sub command reset mcu with msq OFF, PM ON, LTA ON*/
	UWB_SC_MCU_RESET_010= 0x08, /**< Sub command reset mcu with msq OFF, PM ON, LTA OFF*/
	UWB_SC_MCU_RESET_001= 0x09,	/**< Sub command reset mcu with msq OFF, PM OFF, LTA ON*/
	UWB_SC_MCU_RESET_000= 0x0a,	/**< Sub command reset mcu with msq OFF, PM OFF, LTA OFF*/
	//specific to pm
	UWB_SC_PM_3         = 0x00,	/**< Sub command PM_3 */
	UWB_SC_PM_2         = 0x01,	/**< Sub command PM_2 */
	UWB_SC_PM_0         = 0x02,	/**< Sub command PM_0 */
	UWB_SC_PM_STDBY     = 0x03,	/**< Sub command PM_STDBY */
	UWB_SC_PM_RX_IDLE   = 0x04,	/**< Sub command PM_RX_IDLE */
	UWB_SC_PM_TX_IDLE   = 0x05,	/**< Sub command PM_TX_IDLE */
	UWB_SC_PM_TXRX_IDLE = 0x06,	/**< Sub command PM_TXRX_IDLE */
	//specific to sys get info
	UWB_SC_INFO_VERSION = 0x00,	/**< Sub command info HW and SW version */
	UWB_SC_INFO_CAPS    = 0x01,	/**< Sub command info capabilities ( genereic/protocol)*/
	UWB_SC_INFO_MACADDR = 0x02,	/**< Sub command info mac address*/
	UWB_SC_INFO_SN      = 0x03,	/**< Sub command info serial number*/
	UWB_SC_INFO_THRESH  = 0x04,	/**< Sub command info threshold*/
	UWB_SC_INFO_TRESULT = 0x05,	/**< Sub command info prod tests result */
	UWB_SC_INFO_LONGADDR= 0x06,	/**< Sub command info long address*/
	UWB_SC_INFO_SHORTADDR=0x07,	/**< Sub command info short address */
	UWB_SC_INFO_VOLTAGE = 0x08,	/**< Sub command info voltage ( prm can be 1,2,3)*/
	UWB_SC_INFO_SPVTUNE = 0x09,	/**< Sub command info spvtune (prm is channel)*/
	UWB_SC_INFO_PROD_RESULT = 0x0a,	/**< Sub command info prod result.Available with some firmware*/
	//specific to sys get hw value
	UWB_SC_ASKCHECKCACHE= 0x00,	/**< Sub command ask to check if cache is conform to real hw. prm can be 0 is real dac val, 1: real captune */
	UWB_SC_CHECKCACHE   = 0x01,	/**< Sub command read if last check if cache is conform to real hw is ok (1), 0 if mismatch */
	//nb of possible command (worst case)
	UWB_SC_MIN_NB       = 0x00, 	/**< Min value of subcommand */
	UWB_SC_MAX_NB       = 0x0a, 	/**< Max value of subcommand (to be updated if more command are added)*/

} OSAL_PACKED_ENUM uwb_com_subcmd_e;

/**
 * @brief uwb_com_sysstatus_e
 * @note Interruption type definition (u16 enum).This must be used as parameter in call of 
 * - #sys_set_status_mask
 * - #sys_set_status
 * An this can be sent by #sys_get_status
*/
typedef enum {
	#define UWB_STAT_MASK(_stat)  (~(_stat))	/**< Macro to mask status with bitfield */
	UWB_STAT_NONE                   = 0x0000,	/**< No IT */
	UWB_STAT_RSVD0001               = 0x0001,	/**< Reserved */
	UWB_STAT_RSVD0002               = 0x0002,	/**< Reserved */
	UWB_STAT_RSVD0004               = 0x0004,	/**< Reserved */
	UWB_STAT_RSVD0008               = 0x0008,	/**< Reserved */
	UWB_STAT_TX_DONE                = 0x0010,	/**< TX Done */
	UWB_STAT_RX_DONE                = 0x0020,	/**< Rx Done*/
	UWB_STAT_PREAMBLE_TIMEOUT       = 0x0040,	/**< Could not detect preamble*/
	UWB_STAT_SFD_TIME_OUT           = 0x0080,	/**< SFD detection time-out*/
	UWB_STAT_SEQ_START              = 0x0100,	/**< Sequencer has started */
	UWB_STAT_SEQ_ACTION_START       = 0x0200,	/**< A sequencer action has started*/
	UWB_STAT_SEQ_TS_DELAY           = 0x0400,	/**< A sequencer TS-DELAY event has occured */
	UWB_STAT_RSVD0800               = 0x0800,	/**< Reserved */
	UWB_STAT_RSVD1000               = 0x1000,	/**< Reserved */
	UWB_STAT_RSVD2000               = 0x2000,	/**< Reserved */
	UWB_STAT_RSVD4000               = 0x4000,	/**< Reserved */
	UWB_STAT_DEAD                   = 0x8000,	/**< UM100 is in blocked state. should never happen.Cannot be masked */
	UWB_STAT_ALL                    = 0xFFFF,	/**< All IT */

	UWB_STAT_MASK_ALL               = UWB_STAT_MASK(UWB_STAT_ALL)&0xFFFE,	/**< Convenience value indicating we want to know about all IT */
	UWB_STAT_MASK_NONE              = UWB_STAT_MASK(UWB_STAT_NONE)&0xFFFE	/**< Convenience value indicating we want to ignore all IT */
} OSAL_PACKED_ENUM uwb_com_sysstatus_e;

/**
 * @brief uwb_com_system_cmd_e
 * @note Definition of SYS API Command
*/
typedef enum
{
	SYS_BASE_CMD          = 0x0,			/**< Base value for system command */
	SYS_RSVD0             = SYS_BASE_CMD,		/**< Reserved  */
	SYS_GET_INFO          = SYS_BASE_CMD + 0x1,	/**< Get info on module(hw/sw version, custo, mac addr, sn, threshold values....) */
	SYS_RSVD2             = SYS_BASE_CMD + 0x2,	/**< Reserved  */
	SYS_SET_MCU_STATE     = SYS_BASE_CMD + 0x3,	/**< Authorize mcu to enter in sleep mode. Reset/or standby it */
	SYS_GET_DAC_VAL       = SYS_BASE_CMD + 0x4,	/**< Get DAC value */
	SYS_SET_DAC_VAL       = SYS_BASE_CMD + 0x5,	/**< Set new DAC - value  */
	SYS_GET_CAPATUNE_VAL  = SYS_BASE_CMD + 0x6,	/**< Get current Capa-tune val for 32Khz oscillator  */
	SYS_SET_CAPATUNE_VAL  = SYS_BASE_CMD + 0x7,	/**< Set new Capa-tune to tune 32Khz oscillator frequency  */
	SYS_GET_RXFRAME_CFG   = SYS_BASE_CMD + 0x8,	/**< Get the RX frame format sent to host MCU  */
	SYS_SET_RXFRAME_CFG   = SYS_BASE_CMD + 0x9,	/**< Set the RX frame format sent to host MCU  */
	SYS_GET_STATUS_MSK    = SYS_BASE_CMD + 0xA,	/**< Get event mask value */
	SYS_SET_STATUS_MSK    = SYS_BASE_CMD + 0XB,	/**< Set new event mask */
	SYS_GET_STATUS        = SYS_BASE_CMD + 0xC,	/**< Get current module status */
	SYS_RSVDD             = SYS_BASE_CMD + 0xD,	/**< Reserved  */
	SYS_RSVDE             = SYS_BASE_CMD + 0xE,	/**< Reserved  */
	SYS_PROD_LIMITS       = SYS_BASE_CMD + 0xD,	/**< Reserved for production, valid only with some specific firmware */
	SYS_PROD_CTRL         = SYS_BASE_CMD + 0xE,	/**< Reserved for production, valid only with some specific firmware */
	SYS_CHECKCACHE        = SYS_BASE_CMD + 0xF,	/**< check if  a value in cache is ok against real value on HW. use any SYS_.., RADIO_.., SEQ_... command as first prm*/
	SYS_LAST_CMD          = SYS_BASE_CMD + 0xF,	/**< Last value for system command */

	SYS_CMD_NB            = SYS_LAST_CMD - SYS_BASE_CMD+1	/**< nb of sys command */

} OSAL_PACKED_ENUM uwb_com_system_cmd_e;

/**
 * @brief uwb_com_radio_cmd_e
 * @note Definition of RADIO API Command
*/
typedef enum
{
	RADIO_BASE_CMD        = 0x10,			/**< Base value for system command */
	RADIO_INIT            = RADIO_BASE_CMD,		/**< Reset and initialize UWB radio */
	RADIO_SPI_READ        = RADIO_BASE_CMD + 0x1,	/**< Read single or burst of radio register */
	RADIO_SPI_WRITE       = RADIO_BASE_CMD + 0x2,	/**< write single or burst of radio register */
	RADIO_GET_PM_STATE    = RADIO_BASE_CMD + 0x3,	/**< Get current radio state */
	RADIO_SET_PM_STATE    = RADIO_BASE_CMD + 0x4,	/**< Set new radio state */
	RADIO_GET_PHY_CFG     = RADIO_BASE_CMD + 0x5,	/**< Get current radio settings */
	RADIO_SET_PHY_CFG     = RADIO_BASE_CMD + 0x6,	/**< Set new radio settings */
	RADIO_GET_RX_CFG      = RADIO_BASE_CMD + 0x7,	/**< Get current module output format for RX frames */
	RADIO_SET_RX_CFG      = RADIO_BASE_CMD + 0x8,	/**< Set new module output format */
	RADIO_CLR_RNG_TIMER   = RADIO_BASE_CMD + 0x9,	/**< Reset the module rng timer */
	RADIO_WRITE_TX_DATA   = RADIO_BASE_CMD + 0xA,	/**< Write data to TX FIFO */
	RADIO_READ_RX_DATA    = RADIO_BASE_CMD + 0xB,	/**< Read ouput RX data */
	RADIO_PHY_CTRL        = RADIO_BASE_CMD + 0xC,	/**< Start a TX and/or a RX if subCommand=0x1, Abort if subCommand=0x2, get size of subCommand=0x3, set size if subCommand=0x4 */
	RADIO_RSVDD           = RADIO_BASE_CMD + 0xD,	/**< Reserved  */
	RADIO_RSVDE           = RADIO_BASE_CMD + 0xE,	/**< Reserved  */
	RADIO_RSVDF           = RADIO_BASE_CMD + 0xF,	/**< Reserved  */
	RADIO_LAST_CMD        = RADIO_BASE_CMD + 0xF,	/**< Last value for radio command */

	RADIO_CMD_NB          = RADIO_LAST_CMD - RADIO_BASE_CMD+1	/**< nb of radio command */

} OSAL_PACKED_ENUM uwb_com_radio_cmd_e;

/**
 * @brief uwb_com_seq_cmd_e
 * @note Definition of SEQUENCER API Command
*/
typedef enum
{
	SEQUENCER_BASE_CMD    = 0x20,				/**< Base value for system commands */
	SEQ_INIT              = SEQUENCER_BASE_CMD,		/**< Initialize sequencer */
	SEQ_GET_CFG           = SEQUENCER_BASE_CMD + 0x1,	/**< Get global sequencer configuration */
	SEQ_SET_CFG           = SEQUENCER_BASE_CMD + 0x2,	/**< Set global sequencer configuration */
	SEQ_QUEUE_SINGLE_ACT  = SEQUENCER_BASE_CMD + 0x3,	/**< Enqueue single action to be performed by sequencer */
	SEQ_RSVD4             = SEQUENCER_BASE_CMD + 0x4,	/**< Reserved  */
	SEQ_RSVD5             = SEQUENCER_BASE_CMD + 0x5,	/**< Reserved  */
	SEQ_RSVD6             = SEQUENCER_BASE_CMD + 0x6,	/**< Reserved  */
	SEQ_RSVD7             = SEQUENCER_BASE_CMD + 0x7,	/**< Reserved  */
	SEQ_RSVD8             = SEQUENCER_BASE_CMD + 0x8,	/**< Reserved  */
	SEQ_RSVD9             = SEQUENCER_BASE_CMD + 0x9,	/**< Reserved  */
	SEQ_RSVDA             = SEQUENCER_BASE_CMD + 0xA,	/**< Reserved  */
	SEQ_RSVDB             = SEQUENCER_BASE_CMD + 0xB,	/**< Reserved  */
	SEQ_CTRL              = SEQUENCER_BASE_CMD + 0xC,	/**< Control sequencer: subCommand=0x1 Start/ subCommand=0x2 Stop */
	SEQ_RSVDD             = SEQUENCER_BASE_CMD + 0xD,	/**< Reserved  */
	SEQ_RSVDE             = SEQUENCER_BASE_CMD + 0xE,	/**< Reserved  */
	SEQ_RSVDF             = SEQUENCER_BASE_CMD + 0xF,	/**< Reserved  */
	SEQ_LAST_CMD          = SEQUENCER_BASE_CMD + 0xF,	/**< Last value for radio command */

	SEQ_CMD_NB            = SEQ_LAST_CMD - SEQUENCER_BASE_CMD+1	/**< nb of sequencer command */

} OSAL_PACKED_ENUM uwb_com_seq_cmd_e;

/**
 * @brief uwb_com_wake_up_reason_e
 * @note Definition of bitfield that will wake up mcu
*/
typedef enum
{
	SYS_WAKE_UP_PP_MASK				= 0x1, /** SYS_WAKE_UP_PP_MASK mask for pinpointer wake up condition */
	SYS_WAKE_UP_ACC_LOW_MASK		= 0x2,/** SYS_WAKE_UP_ACC_LOW_MASK mask for accelero/uart3 wake up condition*/
	SYS_WAKE_UP_ACC_HIGH_MASK		= 0x4,/** SYS_WAKE_UP_ACC_HIGH_MASK mask for accelero/uart3 wake up condition*/
	SYS_WAKE_UP_SPI_MASK			= 0x8,/** SYS_WAKE_UP_SPI_MASK mask for SPI wake up condition*/
	SYS_WAKE_UP_UART_MASK			= 0x10,/** SYS_WAKE_UP_UART_MASK mask for uart2 wake up condition*/
	SYS_WAKE_UP_MASTER_MASK			= 0x20,/** SYS_WAKE_UP_MASTER_MASK mask for wake up condition through hardware ext line*/
	SYS_WAKE_UP_RTC_MASK			= 0x40,/** SYS_WAKE_UP_RTC_MASK mask for wake up condition through RTC*/
	SYS_WAKE_UP_ALL_MASK			= 0xFF/** SYS_WAKE_UP_ALL_MASK mask for all wake up condition*/
} OSAL_PACKED_ENUM uwb_com_wake_up_reason_e;


/**
 * @brief uwb_com_led_mask_e
 * @note Definition of bitfield for led
*/
typedef enum
{
	SYS_LED_STATUS			= 0xFF, /** A status led.Could be green/led depending on battery */
	SYS_LED_RED				= 0x1, /** Red led */
	SYS_LED_GREEN			= 0x2, /** Green led */
} OSAL_PACKED_ENUM uwb_com_led_mask_e;


/**
 * @brief uwb_test_status_e
 * @note Definition of bitfield for led
*/
typedef enum
{
	TEST_NOTDONE	= 0x0, /** TEST NOT DONE */
	TEST_OK			= 0x1, /** TEST NOT OK */
	TEST_KO			= 0x2 /** TEST NOT KO */
} OSAL_PACKED_ENUM uwb_test_status_e;

/**
 * @brief st_uwb_test_limits
 * @note limit of test definition
 */
typedef struct {
	OSAL_u32 vmin[3];/**< minimal limit for V1,2,3, in mV. Test failed if value is below */
	OSAL_u32 vmax[3];/**< maximal limit for V1,2,3, in mV. Test failed if value is above */
	OSAL_u32 spvmin[8];/**< minimal limit in mV for spvtune of each uwb channel,below wich test failed (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u32 spvmax[8];/**< maximal limit in mV for spvtune of each uwb channel,above wich test failed (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u8  spvoffset[8];/**< offset to spvtune register, for fine tune) */
	OSAL_u32 lnamin;	/**< minimal lna ratio below which test failed*/
	OSAL_u32 lnamax;	/**< maximal lna ratio above which test failed*/
	OSAL_s32 thldmin[8][5];/**< minimal limit for threshold (in millidb) of each uwb channel/each zone,below wich test failed (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_s32 thldmax[8][5];/**< maximal limit for threshold (in millidb)of each uwb channel/each zone,above wich test failed (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_s32 thlddelta[8][5];/**< maximal delta (in millidb) between min and max threshold of each uwb channel/each zone,above wich test failed (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u32 pm2min;/**< minimum cunsumption(in uA) value in pm2, below wich test failed*/
	OSAL_u32 pm2max;/**< maximal cunsumption(in uA) value in pm2, above test failed*/
	OSAL_u32 txmin;/**< minimum cunsumption(in uA) value in tx idle, below wich test failed*/
	OSAL_u32 txmax;/**< maximal cunsumption(in uA) value in tx idle, above test failed*/
	OSAL_u32 rxmin;/**< minimum cunsumption(in uA) value in rx idle, below wich test failed*/
	OSAL_u32 rxmax;/**< maximal cunsumption(in uA) value in rx idle, above test failed*/
}OSAL_PACKED_STRUCTURE st_uwb_test_limits;

/**
 * @brief st_uwb_test_mcuid
 * @note mcu id test result
 */
typedef struct {
	uwb_test_status_e status;	/**< indicate mcu d test succeed*/
	OSAL_u8 id[16];				/**< mcu id values */
	OSAL_u8 serial[16];			/**< serial values */
	OSAL_u8 date[12];			/**< show date of this firmware build*/
	OSAL_u8 time[6];			/**< show time of this firmware build*/
	OSAL_u8 vmajor;				/**< show version major of this firmware build*/
	OSAL_u8 vminor;				/**< show version minor of this firmware build*/
}OSAL_PACKED_STRUCTURE st_uwb_test_mcuid;

/**
 * @brief st_uwb_test_soldering
 * @note soldering test result 
 */
typedef struct {
	uwb_test_status_e status;	/**< indicate whole soldering test succeed */
	uwb_test_status_e TSeq;		/**< indicate if sequencer of pinpointer succeed */
	uwb_test_status_e Tspi;		/**< indicate if spi comm wih pinponter succeed*/
	uwb_test_status_e T32K;		/**< indicate if 32K on pp succeed*/
	uwb_test_status_e T26M;		/**< indicate if 26M on pp succeed*/
	uwb_test_status_e TTX;		/**< indicate if TX sequence on pp succeed*/
	uwb_test_status_e TRX;		/**< indicate if RX sequence on pp succeed*/
	uwb_test_status_e TRXPHR;	/**< indicate if PHR settings on pp succeed*/
	uwb_test_status_e TRXPSDU;	/**< indicate if PHR settings on pp succeed*/
	uwb_test_status_e TIT;		/**< indicate if IT from pp succeed*/
}OSAL_PACKED_STRUCTURE st_uwb_test_soldering;

/**
 * @brief st_uwb_test_voltage
 * @note meas voltage test result 
 */
typedef struct {
	uwb_test_status_e status;	/**< indicate voltage succeed */
	OSAL_u32 measure;			/**< value inmv of the voltage*/
	OSAL_u8 reg;				/**< reg value of this voltage*/
	OSAL_u8 index;				/**< voltage index (1,2 or 3)*/
}OSAL_PACKED_STRUCTURE st_uwb_test_voltage;

/**
 * @brief st_uwb_test_txtune
 * @note txtune test result 
 */
typedef struct {
	uwb_test_status_e status;		/**< indicate globally if tx tune tests succeed */
	uwb_test_status_e chstatus[8];	/**< indicate tx tune tests succeed of each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u32 measure[8];			/**< measure of V in mV for each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u8 reg[8];					/**< tune tx register value for each uwb channel,above wich test failed (index 0:3=channel 1->4, index4:8=channel 8->11) */
}OSAL_PACKED_STRUCTURE st_uwb_test_txtune;

/**
 * @brief st_uwb_test_lna
 * @note lna test result 
 */
typedef struct {
	uwb_test_status_e status;		/**< indicate lna succeed */
	OSAL_s32 result_2_4;			/**< Ratio of test threshold on channel 2, zone 4, wiht path1 agains path2*/
	OSAL_s32 result_3_2;			/**< Ratio of test threshold on channel 3, zone 2, wiht path1 agains path2*/
}OSAL_PACKED_STRUCTURE st_uwb_test_lna;


/**
 * @brief st_uwb_test_rxtune
 * @note rxtune test result 
 */
typedef struct {
	uwb_test_status_e status;	/**< indicate globally if rx tune tests succeed */
	uwb_test_status_e chstatus[8];	/**< indicate rx tune tests succeed of each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u32 thld[8][5];			/**< Threshold value of each uwb channel/each zone,below wich test failed (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_s32 thldmin[8][5];			/**< Threshold value of each uwb channel/each zone,below wich test failed (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_s32 thldmax[8][5];			/**< Threshold value of each uwb channel/each zone,below wich test failed (index 0:3=channel 1->4, index4:8=channel 8->11) */
}OSAL_PACKED_STRUCTURE st_uwb_test_rxtune;

/**
 * @brief st_uwb_test_rfdesign test result 
 * @note rfdesign test result 
 */
typedef struct {
	uwb_test_status_e status;	/**< indicate globally if rfdesign tests succeed */
	uwb_test_status_e chstatus[8]; /**< indicate if rfdesign ok , not done or failed, for each each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u32 min_noise[8];		/**< Min noise (in millidb) for each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u32 max_noise[8];		/**< Max noise (in millidb) for each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u32 average_noise[8];	/**< Average noise (in millidb) for each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_u32 prod_noise[8];		/**< Noise got on prod (in millidb) for each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
	#define MAX_NOISE_RATIO		3190//in millidb. min/max ratio <1580=3.19db 
	OSAL_u32 noise_ratio[8];	/**< ratio bewxteen max/min noise in millidb for each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
	OSAL_s32 noise_ratio_ref[8];/**< ratio bewxteen average and prod noise one in millidb for each uwb channel (index 0:3=channel 1->4, index4:8=channel 8->11) */
}OSAL_PACKED_STRUCTURE st_uwb_test_rfdesign;


/**
 * @brief st_uwb_test_results
 * @note test results
 */
typedef struct {
	OSAL_u8				rsvd[4];	/**< reserved */
	st_uwb_test_mcuid	mcu;		/**< result for mcu test */
	st_uwb_test_soldering soldering;/**< result for soldering test */
	uwb_test_status_e	io;			/**< result for io test */
	st_uwb_test_voltage	v1;			/**< result for v1 test */
	st_uwb_test_voltage	v2;			/**< result for v2 test */
	st_uwb_test_voltage	v3;			/**< result for v3 test */
	st_uwb_test_txtune	txtune;		/**< result for spvtune test */
	st_uwb_test_lna 	lna;		/**< result for lna test */
	st_uwb_test_rxtune	rxtune;		/**< result for rxtune test */
	uwb_test_status_e	txidle;		/**< result for txidle test */
	uwb_test_status_e	rxidle;		/**< result for rxidle test */
	uwb_test_status_e	pm2;		/**< result for pm2 test */
	uwb_test_status_e	txcont;		/**< result for tx continous test */
	uwb_test_status_e	sensitivity;/**< result for sensitivity test */
	uwb_test_status_e	ranging;	/**< result for ranging test */
	uwb_test_status_e	prodstatus;	/**< global status for test */
	OSAL_u32			testRes;	/**< test result bitfield */
}OSAL_PACKED_STRUCTURE st_uwb_test_results;

/**
 * @brief st_uwb_slotRngDataType
 * @note used to define various rngdatatype ( beacon1,2,rng,rdv). used in some tests
 */
typedef struct {
	OSAL_s32			b1type;		/**< ignored if -1. Value of beacon1 Slot datatype (see rng_config_e of pinpointer_protocol_def.h) */
	OSAL_s32			b2type;		/**< ignored if -1. Value of beacon2 Slot datatype (see rng_config_e of pinpointer_protocol_def.h) */
	OSAL_s32			rngtype;	/**< ignored if -1. Value of RANGING Slot datatype (see rng_config_e of pinpointer_protocol_def.h) */
	OSAL_s32			rdvtype;	/**< ignored if -1. Value of RANGING Slot datatype (see rng_config_e of pinpointer_protocol_def.h) */
}OSAL_PACKED_STRUCTURE st_uwb_slotRngDataType;

/**
* @brief SYS_RXDATA_ON_MASK set the module Output form for RXrames (RX Data on)
*/
#define SYS_RXDATA_ON_MASK			0x8
/**
* @brief SYS_FULL_RNG_ON_MASK set the module Output form for RXrames (full rng on)
*/
#define SYS_FULL_RNG_ON_MASK			0x2
/**
* @brief SYS_TOA_ON_MASK set the module Output form for RXrames (toa activated)
*/
#define SYS_TOA_ON_MASK				0x1

/**
* @brief RADIO_SPI_DATA_ACK_SIZE Acknowledge size in case of spi comm
*/
#define RADIO_SPI_DATA_ACK_SIZE			1
/**
* @brief RADIO_RX_CFG_PREAMBLE_TOUT_ON_MASK Preamble timeout activation mask
*/
#define RADIO_RX_CFG_PREAMBLE_TOUT_ON_MASK	0x8
/**
* @brief RADIO_RX_CFG_SFD_TOUT_ON_MASK SFD timeout activation mask
*/
#define RADIO_RX_CFG_SFD_TOUT_ON_MASK		0x1

/**
* @brief U16VAL_TO_MSBLSB convert a single U16/S16 into two U8, MSB, and LSB
*/
#define U16VAL_TO_MSBLSB(_u16Val,_msb,_lsb) do{\
	_msb = (unsigned char) ((_u16Val >> 8) & 0x00FF);\
	_lsb = (unsigned char) (_u16Val & 0x00FF);\
}while(0)

/**
* @brief MSBLSB_TO_U16VAL convert two U8, MSB, and LSB into a single U16/S16 
*/
#define MSBLSB_TO_U16VAL(_msb,_lsb,_u16Val) do{\
	_u16Val = (OSAL_u16) _msb;\
	_u16Val <<= 8;\
	_u16Val |= (OSAL_u16) _lsb;\
}while(0)

/**
* @brief U16VAL_ADDED_TO_MSBLSB add to two U8, MSB, and LSB, a single U16/S16, and add it 
*/
#define U16VAL_ADDED_TO_MSBLSB(_u16Val,_msb,_lsb) do{\
	_msb |= (unsigned char) ((_u16Val >> 8) & 0x00FF);\
	_lsb |= (unsigned char) (_u16Val & 0x00FF);\
}while(0)

/**
* @brief U32VAL_TO_MSBLSB convert a single U32/S32 into 4 U8, MSBMSB, MSBLSB,LSBMSB, and LSBLSB
*/
#define U32VAL_TO_MSBLSB(_u32Val,_msbmsb,_msblsb,_lsbmsb,_lsblsb) do{\
	_msbmsb = (unsigned char) ((_u32Val >> 24) & 0x000000FF);\
	_msblsb = (unsigned char) ((_u32Val >> 16) & 0x000000FF);\
	_lsbmsb = (unsigned char) ((_u32Val >> 8) & 0x000000FF);\
	_lsblsb = (unsigned char) (_u32Val & 0x000000FF);\
}while(0)

/**
* @brief U32VAL_TO_MSBLSB convert 4 U8, MSBMSB, MSBLSB,LSBMSB, and LSBLSB into a single U32/S32 
*/
#define MSBLSB_TO_U32VAL(_msbmsb,_msblsb,_lsbmsb,_lsblsb,_u32Val) do{\
	_u32Val = (OSAL_u32) _msbmsb;\
	_u32Val <<= 8;\
	_u32Val |= (OSAL_u32) _msblsb;\
	_u32Val <<= 8;\
	_u32Val |= (OSAL_u32) _lsbmsb;\
	_u32Val <<= 8;\
	_u32Val |= (OSAL_u32) _lsblsb;\
}while(0)

/**
* @brief U32VAL_TO_MSBLSB add to t4 U8, MSBMSB, MSBLSB,LSBMSB, and LSBLSB a single U32/S32
*/
#define U32VAL_ADDED_TO_MSBLSB(_u32Val,_msbmsb,_msblsb,_lsbmsb,_lsblsb) do{\
	_msbmsb |= (unsigned char) ((_u32Val >> 24) & 0x000000FF);\
	_msblsb |= (unsigned char) ((_u32Val >> 16) & 0x000000FF);\
	_lsbmsb |= (unsigned char) ((_u32Val >> 8) & 0x000000FF);\
	_lsblsb |= (unsigned char) (_u32Val & 0x000000FF);\
}while(0)
#endif //__MODULE_API_DEF_H__

