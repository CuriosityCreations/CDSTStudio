/**
 * @file module_api_def_protocol.h
 * @brief api enum definition for module, specific to protocol
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef __MODULE_API_DEF_PROTOCOL_H__
#define __MODULE_API_DEF_PROTOCOL_H__


/** @page module_api Module API
 * 
*/

/**
 * @brief uwb_com_sysstatus_protocol_e
 * @note Interruption type definition (u16 enum).This must be used as parameter in call of 
 * - #sys_set_status_mask
 * - #sys_set_status
 * An this can be sent by #sys_get_status
 * This is specific to protocol
*/
typedef enum {

	#define UWB_STAT_PROTOCOL_MASK(_stat)  (~(_stat))	/**< Macro to mask status with bitfield */
	#define UWB_STAT_IS_PROTOCOL		0x1		/**< Value on reserved1 indicating its protocol IT */
	UWB_STAT_PROTOCOL_NONE                  = 0x0000,	/**< No IT */
	UWB_STAT_PROTOCOL_RSVD0001              = 0x0001,	/**< Reserved */
	UWB_STAT_PROTOCOL_STATUS_CHG       	 	= 0x0002,	/**< Protocol has started (available only if protocol defined)*/
	UWB_STAT_PROTOCOL_ARTLS_AVAILABLE       = 0x0004,	/**< New ARTLS data are available */
	UWB_STAT_PROTOCOL_NO_MORE_ARTLS			= 0x0008,	/**< No more ARTLS data are available */
	UWB_STAT_PROTOCOL_RANGING_AVAILABLE     = 0x0010,	/**< New Ranging data are available */
	UWB_STAT_PROTOCOL_END_OF_SUPERFRAME     = 0x0020,	/**< End of superframe occurs */
	UWB_STAT_PROTOCOL_START_OF_SUPERFRAME   = 0x0040,	/**< Start of a superframe */
	UWB_STAT_PROTOCOL_RSVD0080              = 0x0080,	/**< Reserved */
	UWB_STAT_PROTOCOL_RSVD0100              = 0x0100,	/**< Reserved */
	UWB_STAT_PROTOCOL_RSVD0200              = 0x0200,	/**< Reserved */
	UWB_STAT_PROTOCOL_RSVD0400              = 0x0400,	/**< Reserved */
	UWB_STAT_PROTOCOL_RSVD0800              = 0x0800,	/**< Reserved */
	UWB_STAT_PROTOCOL_RSVD1000              = 0x1000,	/**< Reserved */
	UWB_STAT_PROTOCOL_RSVD2000              = 0x2000,	/**< Reserved */
	UWB_STAT_PROTOCOL_RSVD4000              = 0x4000,	/**< Reserved */
	UWB_STAT_PROTOCOL_DEAD                  = 0x8000,	/**< UM100 is in blocked state. should never happen.Cannot be masked */
	UWB_STAT_PROTOCOL_ALL                   = 0xFFFF,	/**< All IT */

	UWB_STAT_PROTOCOL_MASK_ALL               = UWB_STAT_PROTOCOL_MASK(UWB_STAT_PROTOCOL_ALL)|UWB_STAT_IS_PROTOCOL,	/**< Convenience value indicating we want to know about all Protocol IT */
	UWB_STAT_PROTOCOL_MASK_NONE              = UWB_STAT_PROTOCOL_MASK(UWB_STAT_PROTOCOL_NONE)|UWB_STAT_IS_PROTOCOL	/**< Convenience value indicating we want to ignore all protocol IT */
} OSAL_PACKED_ENUM uwb_com_sysstatus_protocol_e;

/**
 * @brief uwb_com_protocol_cmd_e
 * @note Definition of PROTOCOL API Command, undefinied in case of generic software
*/
typedef enum
{
	PROTOCOL_BASE_CMD     = 0x30,				/**< Base value for protocol commands */
	PROT_INIT             = PROTOCOL_BASE_CMD,		/**< Initialise Bespoon protocol stack */
	PROT_GET_CAPS         = PROTOCOL_BASE_CMD + 0x1,	/**< Get current module settings */
	PROT_SET_CAPS         = PROTOCOL_BASE_CMD + 0x2,	/**< Set module capabilities */
	PROT_GET_SFRAME_SCH   = PROTOCOL_BASE_CMD + 0x3,	/**< Get current superframe schema */
	PROT_SET_SFRAME_SCH   = PROTOCOL_BASE_CMD + 0x4,	/**< Set new superframe schema */
	PROT_GET_STATS        = PROTOCOL_BASE_CMD + 0x5,	/**< Get local device's current statistics */
	PROT_RESET_STATS      = PROTOCOL_BASE_CMD + 0x6,	/**< Reset local device's current statistics */
	PROT_GET_ARTLS_PKT    = PROTOCOL_BASE_CMD + 0x7,	/**< Get Artls data: size if subCommand=0x4, data if subCommand=0x05, checksum ifsubCommand= 0x06 */
	PROT_SET_ARTLS_PKT    = PROTOCOL_BASE_CMD + 0x8,	/**< Set Artls data */
	PROT_GET_RANGING_PKT  = PROTOCOL_BASE_CMD + 0x9,	/**< Get RNG data: size if subCommand=0x4, data if subCommand=0x05, checksum if subCommand=0x06 */
	PROT_RSVDA            = PROTOCOL_BASE_CMD + 0xA,	/**< Reserved  */
	PROT_RSVDB            = PROTOCOL_BASE_CMD + 0xB,	/**< Reserved  */
	PROT_CTRL             = PROTOCOL_BASE_CMD + 0xC,	/**< Start(subCommand=0x1)/Stop(subCommand=0x2) Bespoon protocol stack, get status if subCommand=0x03 */
	PROT_RSVDD            = PROTOCOL_BASE_CMD + 0xD,	/**< Reserved  */
	PROT_RSVDE            = PROTOCOL_BASE_CMD + 0xE,	/**< Reserved  */
	PROT_RSVDF            = PROTOCOL_BASE_CMD + 0xF,	/**< Reserved  */
	PROT_LAST_CMD         = PROTOCOL_BASE_CMD + 0xF,	/**< Last value for radio command */

	PROT_CMD_NB           = PROT_LAST_CMD - PROTOCOL_BASE_CMD+1 	/**< nb of protocol command */

} OSAL_PACKED_ENUM uwb_com_protocol_cmd_e;


/**
 * @brief uwb_com_protocol_status_e
 * @note Definition of PROTOCOL API status, 
*/
typedef enum
{
	PROT_STATUS_NO_CHANGE		=	0x0000, /**< no change on status */
	PROT_STATUS_START_SUCCESS	=	0x0001, /**< protocol successfully started ( stet change from stopped to running) */
	PROT_STATUS_START_FAILED	=	0x0002, /**< protocol failed to start, and is still in stopped state */
	PROT_STATUS_STOP_SUCCESS	=	0x0004, /**< protocol successfully stopped ( stet change from running to stopped) */
	PROT_STATUS_STOP_FAILED		=	0x0008, /**< protocol failed to stop, and is still in started state */
	PROT_STATUS_RESERVED0010	=	0x0010, /**< Reserved */
	PROT_STATUS_RESERVED0020	=	0x0020, /**< Reserved */
	PROT_STATUS_RESERVED0040	=	0x0040, /**< Reserved */
	PROT_STATUS_RESERVED0080	=	0x0080, /**< Reserved */
	PROT_STATUS_RESERVED0100	=	0x0100, /**< Reserved */
	PROT_STATUS_RESERVED0200	=	0x0200, /**< Reserved */
	PROT_STATUS_RESERVED0400	=	0x0400, /**< Reserved */
	PROT_STATUS_RESERVED0800	=	0x0800, /**< Reserved */
	PROT_STATUS_RESERVED1000	=	0x1000, /**< Reserved */
	PROT_STATUS_RESERVED2000	=	0x2000, /**< Reserved */
	PROT_STATUS_RESERVED4000	=	0x4000, /**< Reserved */
	PROT_STATUS_RESERVED8000	=	0x8000  /**< Reserved */
}OSAL_PACKED_ENUM uwb_com_protocol_status_e;

/**
 * @brief uwb_com_protocol_status_e
 * @note Definition of PROTOCOL state
*/
typedef enum
{
	MOD_CFG_PROT_STATE_STOPPED	=	0x0, /**< Protocol stopped */
	MOD_CFG_PROT_STATE_STARTING	=	0x1, /**< Protocol is trying to go from state stop, to state start */
	MOD_CFG_PROT_STATE_RUNNING	=	0x2, /**< Protocol is running */
	MOD_CFG_PROT_STATE_STOPPING	=	0x3, /**< Protocol is trying to go from state start, to state stop */
	MOD_CFG_PROT_STATE_UNKNOWN	=	0x4  /**< Protocol is in strange state.better reboot um100 */
}OSAL_PACKED_ENUM uwb_com_protocol_state_e;


/**
 * @brief module_dflt_sfi_prot_rate_t
 * @note change rate of default sfi
*/
typedef enum {
	PROT_SFI_DEFAULT_RATE		= 0x0,	/**< Get default sfi function use prodsection default rate */
	PROT_SFI_FAST_RATE		= 0x1,	/**< Get default sfi function use fast rate */
	PROT_SFI_SLOW_RATE		= 0x2,	/**< Get default sfi function use slow rate */
	PROT_SFI_WR_RATE		= 0x3	/**< Get default sfi function use wr rate */
} OSAL_PACKED_ENUM module_dflt_sfi_prot_rate_t;

#endif //__MODULE_API_DEF_PROTOCOL_H__

