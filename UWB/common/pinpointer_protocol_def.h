/**
 * @file pinpointer_protocol_def.h
 * @brief enum and structure definition, used by UWB stack specific to bespoon protocol
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015.
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */


#ifndef PINPOINTER_PROTOCOL_DEF_H
#define PINPOINTER_PROTOCOL_DEF_H

//#################################
// DEFINE SPECIFIC TO BSP PROTOCOL
//#################################

/**
* @brief MAX_FRAME_PER_SUPERFRAME number max of frame per a superframe
*/
#define MAX_FRAME_PER_SUPERFRAME	32

/**
* @brief NB_METRIC_DEBUG nb of debug metrics
*/
#define NB_METRIC_DEBUG			3


/**
 * @brief MAX_LISTENER_NUMBER : number or potential listener for rng data. 
 * @note a listener is an instance in server, bridge, viewer, whatever that will be interested in a certain part of data in rng data
 * @note rng data are stored in listener order, mean rng data 
 */
#define MAX_LISTENER_NUMBER		8

//#################################
// MACRO SPECIFIC TO BSP PROTOCOL
//#################################

/**
 * @brief RGN_INFO_SEQ_STATUS : Extract info from status field of	the rng_data structure
 */
#define RGN_INFO_SEQ_STATUS(error) ((error) & 0xFFFF)		/**< last 16bits are reserved for sequencer status */

/**
 * @brief RGN_INFO_DEFRAME_STATUS : Extract info from status field of	the rng_data structure
 */
#define RGN_INFO_DEFRAME_STATUS(error) (((error) >> 16) & 0x01) /**< bit 16 is deframing status: 1 ok, 1 ko */

/**
 * @brief RGN_INFO_SIGN_STATUS : Extract info from status field of	the rng_data structure
 */
#define RGN_INFO_SIGN_STATUS(error) (((error) >> 18) & 0x01) 	/**< bit 18 is positive status: 1 ranging positive, 
									 0 ranging negative */

//#################################
// ENUMS SPECIFIC TO BSP PROTOCOL
//#################################

/**
 *	@brief afc_config_e : contains afc settings
 */
typedef enum
{
	AFC_CLOCKTRACK_FLAG	= (1<<0),	/**< Do AFC with clock track values */
	AFC_BEACONS_FLAG	= (1<<1)	/**< Do AFC with measures between beacon 1 & beacon 2 */
} OSAL_PACKED_ENUM afc_config_e;

/**
 *	@brief devcaps_bitfield_e : Device Capabilities
*/
typedef enum{
	DEVCAPS_UWB_SLAVE			= 0x00000001,	/**< Device should find a coordinator and ask him for the configuration to use via the RDV slot*/
	DEVCAPS_UWB_MASTER			= 0x00000002,	/**< Device should answer request on RDV slot */
	DEVCAPS_ROOT				= 0x00000004,	/**< Device is synchronisation root device (e.g. do not use other beacon to sync its clock, instead other devices sync on it) */ 
	DEVCAPS_TYPE_MASK			= 0x00000007,	/**< MAsk on type of device */ 
	DEVCAPS_NO_B_FRAME 			= 0x00000008,	/**< Device won't send beacon unified frame metrics to user space. By default, all frame are sent(if FULL LOC activated)*/
	DEVCAPS_NO_B1_FRAME 		= 0x00000010,	/**< Device won't send beacon1 frame metrics  to user space. By default, all frame are sent(if FULL LOC activated)*/
	DEVCAPS_NO_B2_FRAME 		= 0x00000020,	/**< Device won't send beacon2 frame metrics  to user space. By default, all frame are sent(if FULL LOC activated)*/
	DEVCAPS_NO_RNG_FRAME		= 0x00000040,	/**< Device won't send ranging frame metrics to user space. By default, all frame are sent(if FULL LOC activated)*/
	DEVCAPS_NO_RSVD_FRAME		= 0x00000080,	/**< Device won't send frame metrics to user space. By default, all frame are sent(if FULL LOC activated)*/
	DEVCAPS_NO_RDV_FRAME		= 0x00000100,	/**< Device won't send rdv frame metrics  to user space. By default, all frame are sent*/
	DEVCAPS_NO_FRAME			= 0x000001F8,	/**< Device won't send any frame metrics  to user space. By default, all frame are sent*/
	DEVCAPS_FILTER_NULL			= 0x00001000,	/**< Device wont notify for ranging wiche src and dst are =0*/
	DEVCAPS_AGILE				= 0x00010000,	/**< Device agile mode */
	DEVCAPS_FAKEADDR			= 0x00020000	/**< Instead of using its own hash in ranging frame, tag use the slot number as a hash. This is used for stress tests */
} OSAL_PACKED_ENUM devcaps_bitfield_e;

/**
 * @brief type_trame_t Trame typeof frame enumeration
*/
typedef enum {
	TT_UNUSED 		= 0,		/**< Unused type. This allow to detect for unalignement with server */
	TT_BEACON  		= 1,		/**< Unified Beacon (send by coord) */
	TT_BEACON1 		= 2,		/**< Beacon 1 (send by coord) */
	TT_BEACON2 		= 3,		/**< Beacon 2 (send by coord) */
	TT_TAG_RANGING	= 4,		/**< Ranging data (send by tag) */
	TT_RSVD1		= 5,		/**< Reserved for future use */
	TT_RDV 			= 6,		/**< Rendez Vous */
	TT_UNKNOWN		= 0xFF,  	/**< unknown frame */
} OSAL_PACKED_ENUM type_trame_t;

/**
 * @brief type_rdv_t subtype for RDV slot enumeration
*/
typedef enum {
	RDV_ACK = 0,						/**< RDV type is ACK*/
	RDV_PAIRING_REQUEST = 2,			/**< RDV type is pairing request*/
	RDV_SIMPLE_PAIRING_REPLY = 3,		/**< RDV type is pairing reply*/
	RDV_DISSOCIATION_REQUEST = 4,		/**< RDV type is dissociation request*/
	RDV_DISSOCIATION_NOTIFICATION = 5,	/**< RDV type is dissociation notification*/
	RDV_PAIRING_NACK,					/**< RDV type is ACK*/
	RDV_SLOT_REQUEST,					/**< RDV type is request for a SLOT */
	RDV_SLOT_GRANTED,					/**< RDV type is SLOT has been granted*/
	RDV_PING,							/**< RDV type is PING */
	RDV_INFO_REQUEST,					/**< RDV type is request for INFO */
	RDV_DEVICE_INFO_REPLY,				/**< RDV type is request for INFO reply */
	RDV_FRIENDLY_NAME_REPLY,			/**< RDV type is request for Name reply */
	RDV_UNKNOWN = 255					/**< Special internal value when scheduling a RX of RDV frame but of unknown size */
} OSAL_PACKED_ENUM type_rdv_t;

/**
 * @brief framer_data_slot_t 
*/
typedef enum {
	DS_BROADCAST_TRANSMITING = 0,	/**< Master has something to say */
	DS_BROADCAST_LISTENING = 1,		/**< Slave is listenning */
	DS_DEDICATED_TRANSMITTING = 2,	/**< Master is in transaction */
	DS_DEDICATED_LISTENING = 3,		/**< Slave is in transaction */
} OSAL_PACKED_ENUM framer_data_slot_t;

/*
 * //brief type_rng_t
*/
typedef enum {
	RNGT_BASIC = 0,			// please coment me 
	RNGT_THRESHOLD = 1,		// please coment me 
	RNGT_UNKNOWN = 255		// please coment me 
} OSAL_PACKED_ENUM type_rng_t;

/*
 * //brief framer_auth_type_request_t 
*/
typedef enum {
	ATR_SHORT_OPEN = 0,		// please coment me
	ATR_EXT_OPEN   = 1,		// please coment me
	ATR_SECURE     = 2,		// please coment me
} OSAL_PACKED_ENUM framer_auth_type_request_t;

/*
 * //brief framer_loc_type_request_t 
*/
typedef enum {
	LTR_SLOWEST = 1,		// please coment me 
	LTR_STANDARD = 2,		// please coment me 
	LTR_FASTEST = 3,		// please coment me 
} OSAL_PACKED_ENUM framer_loc_type_request_t;

/**
 * @brief gts_rate_t 
*/
typedef enum {
	GR_RATE_AUTO	= 0,		/**< Device will range with the rate it wants */
	GR_RATE_1	= 1,			/**< Device will range every superframe */
	GR_RATE_2	= 2,			/**< Device will range each 2 superframe */
	GR_RATE_4	= 3,			/**< Device will range each 4 superframe */
	GR_RATE_6	= 4,			/**< Device will range each 6 superframe */
	GR_RATE_40	= 5,			/**< Device will range each 40 superframe */
	GR_RATE_120	= 6,			/**< Device will range each 120 superframe */
	GR_RESERVED	= 7				/**< Reserved for future use */
} OSAL_PACKED_ENUM gts_rate_t;


/**
 * @brief rng_config_e Type of ranging data
 * Basically, ranging will always have at leaset 4 bits, 3 for bat level, 1 for moving info
 * 
 * bit    | 31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|  => Bit managed internally to retrieve other information from local device(not sent trough uwb)
 * Section| 31|30|          29-22        |21|20|19|18|17|16|  => these bits will indicate local what he has to had in rng strcuture, and by the way, the final size of rng data

 * bit    | 15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|  => Bit from 11 to 0 are sent trough UWB to configure (16bits).
 * Section| 15|14|13|12|11|     10-6     | 5| 4| 3| 2| 1| 0|  => these bits will indicate src device what he has to send trough rng packet
 * 
 *
 * Section 'bit00'  : Sensor 0 / Battery in our implementation on 1 bit, activated or not.Unused in case of beacon/rdv
 * Section 'bit01'  : Sensor 1 / Moving in our implementation on 1 bit, activated or not.Unused in case of beacon/rdv
 * Section 'bit02'  : Sensor 2  on 1 bit, activated or not.Unused in case of beacon/rdv
 * Section 'bit03'  : Sensor 3  on 1 bit, activated or not.Unused in case of beacon/rdv
 * Section 'bit04'  : Sensor 4  on 1 bit, activated or not.Unused in case of beacon/rdv
 * Section 'bit05'  : Sensor 5  on 1 bit, activated or not.Unused in case of beacon/rdv
 * Section 'bit0610': Bespoon Stats activated, on 2 bit.Unused in case of beacon/rdv
 * Section 'bit11'  : External Data, on 1 bit, activated or not.Unused in case of beacon/rdv
 * Section 'bit12'  : RNG/BEACON/RDV:Activate toa_le retrieving, info in loc_toa_t structure.
 * Section 'bit13'  : RNG/BEACON/RDV:Activate nrj summary information retrieving, info in loc_metrics_t structure.
 * Section 'bit14'  : RNG/BEACON/RDV:Activate finger summary, info in loc_nrj_t structure.
 * Section 'bit15'  : RNG/BEACON/RDV:Activate phy config basic summary, info in loc_phy_t structure.
 * Section 'bit16'  : RNG/BEACON/RDV:Activate iw, prp and nrj for pulse reconstruction, info in loc_estimate_t structure.
 * Section 'bit17'  : Reserved 
 * Section 'bit1821': Frame Type : see type_trame_t
 * Section 'bit2229': Index of loc info in bytes. FOr beacon, it will be always 0
 * Section 'bit30'  : Byte order config, 0 is big Endian, 1 is little endian.This concern only data form section bit16 to section bit 28
 * Section 'bit31'  : Filled in real time. If 1, it indicate overload of data, aka, too much bit have been activated, and final data ( limited to 448 bytes) cannot be completelly filled with all info
 * 
 * Data in rng data buffer are sorted by 'Section order'
 * Each listener will treat its own data. Once its oden, and IF above listener do not need a section anymore, 
 * It will reconcanate rng data, and decrease total size.
 * Example:Assume  All section are used,
 * Buffer contain data corresponding to Section bit0,bit1,bit2,bit3,bit4,bit5,bit6
 *   listener0 want section bit0,    ,bit2,    ,bit4,bit5
 *   listener1 want section bit1,bit2,bit3,bit4,bit6
 *   listener2 want section bit0,bit1,bit3
 *   listener3 want section bit1,bit2,bit3,bit4
 *   listener4 want section bit0,bit1,bit3
 *   listener5 want section bit1
 * 
 *  - listener 0 can remove data corresponding to Section bit5, and reset bit5 in rng config to 0
 *    => Buffer contain data corresponding to Section bit0,bit1,bit2,bit3,bit4,bit6
 *  - listener 1 can remove data corresponding to Section bit6, and reset bit6 in rng config to 0
 *    => Buffer contain data corresponding to Section bit0,bit1,bit2,bit3,bit4
 *  - listener 2 can't remove anything
 *    => Buffer contain data corresponding to Section bit0,bit1,bit2,bit3,bit4
 *  - listener 3 can remove data corresponding to Section bit2 and bit4, and reset bit2 and bit4 in rng config to 0
 *    => Buffer contain data corresponding to Section bit0,bit1,bit3
 *  - listener 4 can remove data corresponding to Section bit0 and bit3, and reset bit0 and bit3 in rng config to 0
 *    => Buffer contain data corresponding to Section bit1
 *  - listener 5 treat last data
 */
typedef enum {
	//################### RANGING SPECIFIC ######################################
	//Sensor 0
	RNG_CONF_SENSOR_0_MASK	= 0x00000001,	/**< Mask for Sensor 0, Section bit00*/
	RNG_CONF_SENSOR_0		= 0x00000001,	/**< Sensor 0, activated or not*/
	//Sensor 1
	RNG_CONF_SENSOR_1_MASK	= 0x00000002,	/**< Mask for Sensor 1, section bit01*/
	RNG_CONF_SENSOR_1		= 0x00000002,	/**< Sensor 1, activated or not*/
	//Sensor 2
	RNG_CONF_SENSOR_2_MASK	= 0x00000004,	/**< Mask for Sensor 2, section bit02*/
	RNG_CONF_SENSOR_2		= 0x00000004,	/**< Sensor 2, activated or not*/
	//Sensor 3
	RNG_CONF_SENSOR_3_MASK	= 0x00000008,	/**< Mask for Sensor 3, section bit03*/
	RNG_CONF_SENSOR_3		= 0x00000008,	/**< Sensor 3, activated or not*/
	//Sensor 4
	RNG_CONF_SENSOR_4_MASK	= 0x00000010,	/**< Mask for Sensor 4, section bit04*/
	RNG_CONF_SENSOR_4		= 0x00000010,	/**< Sensor 4, activated or not*/
	//Sensor 5
	RNG_CONF_SENSOR_5_MASK	= 0x00000020,	/**< Mask for Sensor 5, section bit05*/
	RNG_CONF_SENSOR_5		= 0x00000020,	/**< Sensor 5, activated or not*/
	#define RNG_CONF_SENSOR_NUMBER 6 /**< total number of sensor*/

	//Stats
	RNG_CONF_STATS_MASK		= 0x000007C0,	/**< Mask for BeSpoon stats,Section bit0610*/
			#define RNG_CONF_STATS_INDEX(_conf) 		(((unsigned) (_conf)&RNG_CONF_STATS_MASK)>>6)
			#define RNG_CONF_STATS_INDEX2CONF(_index)	(((unsigned) ((_index+1)&0x3F)<<6))
			#define RNG_CONF_STATS_INDEX_ROTATE			RNG_CONF_STATS_MASK

	//External data
	RNG_CONF_EXTDATA_MASK	= 0x00000800,	/**< Mask for External data, Section bit11, activated or not*/
	RNG_CONF_EXTDATA		= 0x00000800,	/**< External Data, activated or not*/

	RNG_CONF_UWB_DATA_MASK	= 0x00000FFF,	/**< Mask indicating something is configured on uwb data part*/

	//################### BEACON SPECIFIC ######################################
	BEA_CONF_RSVD_BIT0_MASK	= 0x00000001,	/**< Mask for Reserved Section bit00*/
	BEA_CONF_RSVD_BIT0		= 0x00000001,	/**< Reserved bit00, Section bit00*/

	BEA_CONF_RSVD_BIT1_MASK	= 0x00000002,	/**< Mask for Reserved Section bit01*/
	BEA_CONF_RSVD_BIT1		= 0x00000002,	/**< Reserved bit01, Section bit01*/

	BEA_CONF_RSVD_BIT2_MASK	= 0x00000004,	/**< Mask for Reserved Section bit02*/
	BEA_CONF_RSVD_BIT2		= 0x00000004,	/**< Reserved bit02, Section bit02*/

	BEA_CONF_RSVD_BIT3_MASK	= 0x00000008,	/**< Mask for Reserved Section bit03*/
	BEA_CONF_RSVD_BIT3		= 0x00000008,	/**< Reserved bit03, Section bit03*/

	BEA_CONF_RSVD_BIT4_MASK	= 0x00000010,	/**< Mask for Reserved Section bit04*/
	BEA_CONF_RSVD_BIT4		= 0x00000010,	/**< Reserved bit04, Section bit04*/

	BEA_CONF_RSVD_BIT5_MASK	= 0x00000020,	/**< Mask for Reserved Section bit05*/
	BEA_CONF_RSVD_BIT5		= 0x00000020,	/**< Reserved bit05, Section bit05*/

	BEA_CONF_RSVD_BIT6_MASK	= 0x00000040,	/**< Mask for Reserved Section bit06*/
	BEA_CONF_RSVD_BIT6		= 0x00000040,	/**< Reserved bit06, Section bit06*/

	BEA_CONF_RSVD_BIT7_MASK	= 0x00000080,	/**< Mask for Reserved Section bit07*/
	BEA_CONF_RSVD_BIT7		= 0x00000080,	/**< Reserved bit07, Section bit07*/

	BEA_CONF_RSVD_BIT8_MASK	= 0x00000100,	/**< Mask for Reserved Section bit08*/
	BEA_CONF_RSVD_BIT8		= 0x00000100,	/**< Reserved bit08, Section bit08*/

	BEA_CONF_RSVD_BIT9_MASK	= 0x00000200,	/**< Mask for Reserved Section bit09*/
	BEA_CONF_RSVD_BIT9		= 0x00000200,	/**< Reserved bit09, Section bit09*/

	BEA_CONF_RSVD_BIT10_MASK= 0x00000400,	/**< Mask for Reserved Section bit10*/
	BEA_CONF_RSVD_BIT10		= 0x00000400,	/**< Reserved bit10, Section bit10*/

	BEA_CONF_RSVD_BIT11_MASK= 0x00000800,	/**< Mask for Reserved Section bit11*/
	BEA_CONF_RSVD_BIT11		= 0x00000800,	/**< Reserved bit11, Section bit11*/

			#define BEA_CONF_RSVD_RESET(_conf)  do{\
					_conf &= (~0xFFF);\
				}while(0)

	//################### RDV SPECIFIC ######################################
	RDV_CONF_RSVD_BIT0_MASK	= 0x00000001,	/**< Mask for Reserved Section bit00*/
	RDV_CONF_RSVD_BIT0		= 0x00000001,	/**< Reserved bit00, Section bit00*/

	RDV_CONF_RSVD_BIT1_MASK	= 0x00000002,	/**< Mask for Reserved Section bit01*/
	RDV_CONF_RSVD_BIT1		= 0x00000002,	/**< Reserved bit01, Section bit01*/

	RDV_CONF_RSVD_BIT2_MASK	= 0x00000004,	/**< Mask for Reserved Section bit02*/
	RDV_CONF_RSVD_BIT2		= 0x00000004,	/**< Reserved bit02, Section bit02*/

	RDV_CONF_RSVD_BIT3_MASK	= 0x00000008,	/**< Mask for Reserved Section bit03*/
	RDV_CONF_RSVD_BIT3		= 0x00000008,	/**< Reserved bit03, Section bit03*/

	RDV_CONF_RSVD_BIT4_MASK	= 0x00000010,	/**< Mask for Reserved Section bit04*/
	RDV_CONF_RSVD_BIT4		= 0x00000010,	/**< Reserved bit04, Section bit04*/

	RDV_CONF_RSVD_BIT5_MASK	= 0x00000020,	/**< Mask for Reserved Section bit05*/
	RDV_CONF_RSVD_BIT5		= 0x00000020,	/**< Reserved bit05, Section bit05*/

	RDV_CONF_RSVD_BIT6_MASK	= 0x00000040,	/**< Mask for Reserved Section bit06*/
	RDV_CONF_RSVD_BIT6		= 0x00000040,	/**< Reserved bit06, Section bit06*/

	RDV_CONF_RSVD_BIT7_MASK	= 0x00000080,	/**< Mask for Reserved Section bit07*/
	RDV_CONF_RSVD_BIT7		= 0x00000080,	/**< Reserved bit07, Section bit07*/

	RDV_CONF_RSVD_BIT8_MASK	= 0x00000100,	/**< Mask for Reserved Section bit08*/
	RDV_CONF_RSVD_BIT8		= 0x00000100,	/**< Reserved bit08, Section bit08*/

	RDV_CONF_RSVD_BIT9_MASK	= 0x00000200,	/**< Mask for Reserved Section bit09*/
	RDV_CONF_RSVD_BIT9		= 0x00000200,	/**< Reserved bit09, Section bit09*/

	RDV_CONF_RSVD_BIT10_MASK= 0x00000400,	/**< Mask for Reserved Section bit10*/
	RDV_CONF_RSVD_BIT10		= 0x00000400,	/**< Reserved bit10, Section bit10*/

	RDV_CONF_RSVD_BIT11_MASK= 0x00000800,	/**< Mask for Reserved Section bit11*/
	RDV_CONF_RSVD_BIT11		= 0x00000800,	/**< Reserved bit11, Section bit11*/

			#define RDV_CONF_RSVD_RESET(_conf)  do{\
					_conf &= (~0xFFF);\
				}while(0)

	//################### COMMON  ######################################
	//Activate toa_le retrieving, info in loc_toa_t structure
	RNG_CONF_LOC_TOA_MASK	= 0x00001000,	/**< Mask for Activate toa_le retrieving, Section bit12*/
	RNG_CONF_LOC_TOA		= 0x00001000,	/**< Is toa_le retrieving activated, info in loc_toa_t structure */
	
	//Activate metrics summary information retrieving, info in loc_metrics_t structure
	RNG_CONF_LOC_METR_MASK	= 0x00002000,	/**< Mask for Activate nrj summary information retrieving, Section bit13*/
	RNG_CONF_LOC_METR		= 0x00002000,	/**< Is nrj summary information retrievingg activated, info in loc_metrics_t structure */
	
	//Activate finger summary, info in loc_nrj_t structure
	RNG_CONF_LOC_NRJ_MASK	= 0x00004000,	/**< Mask for Activate finger summary, Section bit14*/
	RNG_CONF_LOC_NRJ		= 0x00004000,	/**< Is finger summaryg activated, info in loc_nrj_t structure*/
	
	//Activate phy config basic summary, info in loc_phy_t structure
	RNG_CONF_LOC_PHY_MASK	= 0x00008000,	/**< Mask for Activate phy config basic summary, Section bit15*/
	RNG_CONF_LOC_PHY		= 0x00008000,	/**< Is phy config basic summaryg activated, info in loc_phy_t structure*/
	
	//Activate iw, prp and nrj for pulse reconstruction, info in loc_estimate_t structure
	RNG_CONF_LOC_IW_MASK	= 0x00010000,	/**< Mask for Activate iw, prp and nrj for pulse reconstruction, Section bit16*/
	RNG_CONF_LOC_IW			= 0x00010000,	/**< Is iw, prp and nrj for pulse reconstructiong activated, info in loc_estimate_t structure */

	//################### CONFIG ######################################
	//Rsvd
	RNG_CONF_RSVD_BIT17_MASK= 0x00020000,	/**< Mask for Reserved Section bit17*/
	RNG_CONF_RSVD_BIT17		= 0x00020000,	/**< Reserved bit17, Section bit17*/
	
	//indicate frame type
	RNG_CONF_FTYPE_RNGMASK = 0x003C0000,	/**< Mask for TT type conf, on 32bits: see type_trame_t*/
			#define RNG_CONF_GET_TTTYPE(_conf)		(((unsigned) (_conf)&RNG_CONF_FTYPE_RNGMASK)>>18)
			#define RNG_CONF_FTYPE_IS_RNG(_conf)	(RNG_CONF_GET_TTTYPE(_conf)==TT_TAG_RANGING)
			#define RNG_CONF_FTYPE_IS_B1(_conf)	(RNG_CONF_GET_TTTYPE(_conf)==TT_BEACON1)
			#define RNG_CONF_FTYPE_IS_B2(_conf)	(RNG_CONF_GET_TTTYPE(_conf)==TT_BEACON2)
			#define RNG_CONF_FTYPE_IS_RDV(_conf)	(RNG_CONF_GET_TTTYPE(_conf)==TT_RDV)
			#define RNG_CONF_SET_FTYPE_X(_conf,_x) do{\
					_conf &= (~RNG_CONF_FTYPE_RNGMASK);\
					_conf |= ((_x&0xF)<<18);\
				}while(0)
			#define RNG_CONF_SET_FTYPE_RNG(_conf) RNG_CONF_SET_FTYPE_X(_conf,TT_TAG_RANGING)
			#define RNG_CONF_SET_FTYPE_B1(_conf) RNG_CONF_SET_FTYPE_X(_conf,TT_BEACON1)
			#define RNG_CONF_SET_FTYPE_B2(_conf) RNG_CONF_SET_FTYPE_X(_conf,TT_BEACON2)
			#define RNG_CONF_SET_FTYPE_RDV(_conf) RNG_CONF_SET_FTYPE_X(_conf,TT_RDV)

	//Index of loc info in output data
	RNG_CONF_LOC_INDEX_MASK	= 0x3FC00000,	/**< Mask for Section bit29-22,Index of loc info in output data*/
			#define RNG_CONF_LOC_INDEX(_conf)	(((unsigned) (_conf)&RNG_CONF_LOC_INDEX_MASK)>>22)
			#define RNG_CONF_SET_LOC_INDEX(_conf,_sizeInBytes) do{\
					_conf &= (~RNG_CONF_LOC_INDEX_MASK);\
					_conf |= (((_sizeInBytes)&0xFF)<<22);\
				}while(0)
			#define RNG_CONF_EMPTYDATA_MASK 0xC0000000

	//Byte order
	RNG_CONF_BYTE_ORDER_MASK= 0x40000000,	/**< Mask for Byte order config Section bit30*/
	RNG_CONF_BYTE_ORDER_BE	= 0x00000000,	/**< Byte order is Big Endian*/
	RNG_CONF_BYTE_ORDER_LE	= 0x40000000,	/**< Byte order is Little Endian*/	
			#define RNG_CONF_BYTE_ORDER_IS_LE(_conf) (((unsigned) (_conf)&RNG_CONF_BYTE_ORDER_MASK)>>30)

	//DATA OVERLOAD
	RNG_CONF_OVERLOAD_MASK	= 0x80000000,	/**< Mask Section bit31, aka INDICATE IF DATA OVERLOAD */
			#define RNG_CONF_RNGOVERLOAD(_conf) (((unsigned) (_conf)&RNG_CONF_OVERLOAD_MASK)>>31)
} OSAL_PACKED_ENUM rng_config_e;


/**
 * @brief frame_info_type : Type of slot that can be scheduled in a superframe schema
 */
typedef enum frame_info_type {
	FIT_SCAN	= 0,	/**< Schedule a SCAN */
	FIT_BEACON1 	= 1,	/**< Schedule a BEACON 1 */
	FIT_BEACON2 	= 2,	/**< Schedule a BEACON 2 */
	FIT_RANGING 	= 3,	/**< Schedule a RANGING */
	FIT_RACH	= 4	/**< Schedule a RACH */
} OSAL_PACKED_ENUM frame_info_type_t;

/**
 * @brief dev_status_t device state enumeration
*/
typedef enum {
	DS_DETACHED = 0,	/**< Device detached */
	DS_ATTACH_REQUEST,	/**< Device ask for attaching */
	DS_ATTACHED,		/**< Device attached */
} OSAL_PACKED_ENUM dev_status_t;

/**
 * @brief artls_state_t artls Attachement State enumeration
*/
typedef enum {
	AS_DETACHED=0,		/**< as detached */
	AS_RACH_REQUEST,	/**< rach request */
	AS_RACH_WAIT,		/**< wait for rach */
	AS_RACH_DECLINED,	/**< rach detached */
	AS_RACH_ACCEPTED,	/**< rach acceptedd */
} OSAL_PACKED_ENUM artls_state_t;

/**
 * artls_lostatus_t artls Local Status enumeration
*/
typedef enum {
	LOSTATUS_STOPED = 0,	/**< stopped */
	LOSTATUS_SCAN_RF,	/**< scanning */
	LOSTATUS_CALIBRATING,	/**< calibrating */
	LOSTATUS_RANGING,	/**< ranging */
	LOSTATUS_ON_ERROR,	/**< error */
	LOSTATUS_NB,		/**< nb of possible state */
} OSAL_PACKED_ENUM artls_lostatus_t;

#ifdef CONFIG_HAVE_STATS

/**
 *	@brief protocol_stats_type_t Statistics
*/
typedef enum {
	STATS_RX=0,		/**< Number of scheduled RX*/
	STATS_TX,		/**< Number of scheduled TX*/
	STATS_SFD_TO,		/**< Number of SFT timeout*/
	STATS_NOISE_LEVEL,	/**< Noise reference level*/
	STATS_CORRUPTED_FRAME,	/**< Number of FEC error (or bad Rx size)*/
	STATS_CANCELED_SEQ_FRAME,	/**< Number of canceled frames*/
	STATS_DEFRAME_ERROR,	/**< Number of deframe error*/
	STATS_SEQUENCER_ERROR,  /**< Number of error in sequencer*/
	STATS_SEQUENCER_TSA,	/**< time slot adjustment (in ps)*/
	STATS_SCAN,		/**< Number of time the device do scan_rf*/
	STATS_AFC_ERROR,	/**< Number of error in AFC*/
	STATS_AFC_FILTER,	/**< AFC filter output (in ppb)*/
	STATS_RDV_RX,		/**< Number of received frame on RDV slot*/
	STATS_RDV_MISSING_RX,	/**< Number of missing RDV frame*/
	STATS_RDV_TX,		/**< Number of RDV frame sent*/
	STATS_LOCRATE_INFO,	/**< Loc rate info: dynamic flag, SF start and actual loc rate (tag only)*/
	STATS_BEACON_SCORE, 	/**< ref base current score*/
	STATS_RSVD,		/**< reserved for future use or debug */
	STATS_ROTATE,		/**< Rotate upon all stat above*/
	STATS_NB
} OSAL_PACKED_ENUM protocol_stats_type_t;

#endif //#ifdef CONFIG_HAVE_STATS

/**
 * @brief artls_up_cmd_e - tell which field is set in artls_up_t payload
 */
typedef enum {
	ARTLS_LOSTATUS_UP = 0,		/**< Field local_status is filled*/
	ARTLS_LO_REF_BASE_UP,		/**< Field local_ref_base is filled */
	ARTLS_PAIRING_REQUEST_UP,	/**< Field pairing_req is filled */
	ARTLS_INFO_UP,			/**< Field device_info is filled */
	ARTLS_DISSOCIATION_REQUEST_UP,  /**< Field dreq is filled */
	ARTLS_CMD_STATUS_UP,		/**< Field cmd_status is filled */
}OSAL_PACKED_ENUM artls_up_cmd_e;

/**
 * @brief artls_down_cmd_e - tell which field is set in artls_down_t payload
 */
typedef enum {
	ARTLS_SIMPLE_PAIRING_REPLY_DOWN = 0,	/**< Send a Simple Pairing Reply frame to a remote tag */
	ARTLS_DISSOCIATION_REQUEST_DOWN = 1,	/**< Send a Dissociation Request frame to a remote tag */
	ARTLS_SET_DEV_ID_DOWN = 10,		/**< Set the device id (a.k.a short address a.k.a hash) */
	ARTLS_SET_PREFERED_REF_BASE_DOWN = 11,	/**< Force the device to bind to a specific ref base */
	ARTLS_SET_THRESHOLD_COEFF_DOWN = 12,	/**< Apply a multiplicative coefficient to all synchronisation threshold */
	ARTLS_SET_HW_OFFSET_DOWN = 13,		/**< Apply a specific hardware offset */
}OSAL_PACKED_ENUM artls_down_cmd_e;

/**
 * @brief artls_command_return_code_t - return code of a artls_down_t command
 */
typedef enum {
	ACC_SUCCESS = 0,	/**< Command finished sucessfully */
	ACC_NO_ACK	= 1,	/**< Remote peer do not ack the command */
	ACC_INTERNAL_ERROR = 2, /**< Command not finished due to internal error */
	// TODO: add timeout, canceled, ...
} OSAL_PACKED_ENUM artls_cmd_return_code_t;

/**
 * @brief artls_tracking_t - define how user want to track an artls_down_t command
 */
typedef enum {
	AT_NO_TRACKING = 0,	/**< Fire and forget, do not notify anything about the end of the command */
	AT_NOTIFY_END		/**< Notify at the end of the command */
} OSAL_PACKED_ENUM artls_tracking_t;

/**
 * @brief asset_tracking_config_e - define how user want to configure asset tracking
 */
typedef enum {
	ASSETTRACKING_ENABLE 		= 0x0001,		/**< Enable / disable sleep between ranging */
	ASSETTRACKING_RACH		= 0x0002,		/**< Enable / disable rach on wake up. If disabled rach data will be restored from first attachment */
	ASSETTRACKING_RANDOM_SLOT	= 0x0004,		/**< Enable / disable random ranging slot. Overwrite ASSETTRACKING_RACH if enabled */
	ASSETTRACKING_AFC		= 0x0008		/**< Enable / disable AFC on each wake up */
} OSAL_PACKED_ENUM asset_tracking_config_e;


//#################################
// STRUCT SPECIFIC TO BSP PROTOCOL
//#################################

/**
 *	@brief toa : contains TOAs for all fingers
 *	@note This structure is aligned and sized 24 bytes (8 bytes w/o CONFIG_RTLS_FULL_LOC)
 */ 
typedef struct toa
{
	OSAL_u32 le_raw;	/**< Time of arrival( nano) */
	OSAL_u32 le_fine;	/**< Time of arrival( nano) */
#ifdef CONFIG_RTLS_FULL_LOC
	OSAL_u32 fg1;		/**< Finger 1 toa */
	OSAL_u32 fg2;		/**< Finger 2 toa */
	OSAL_u32 fg3;		/**< Finger 3 toa */
	OSAL_u32 fg4;		/**< Finger 4 toa */
#endif //defined CONFIG_RTLS_FULL_LOC
} OSAL_PACKED_STRUCTURE toa_t;

/**
 *	@brief rxqid : contains RX qualities for all fingers
 *	@note This structure is aligned and sized 24 bytes/4 bytes without CONFIG_RTLS_FULL_LOC
 */
typedef struct rxqid
{
#ifdef CONFIG_RTLS_FULL_LOC
	OSAL_u32 le_raw;	/**< Time of arrival( nano)*/
	OSAL_u32 le_fine;	/**< Time of arrival( pico) */
#endif
	OSAL_u32 fg1;		/**< RSSID Finger 1 toa	*/
#ifdef CONFIG_RTLS_FULL_LOC
	OSAL_u32 fg2;		/**< RSSID Finger 2 toa	*/
	OSAL_u32 fg3;		/**< RSSID Finger 3 toa	*/
	OSAL_u32 fg4;		/**< RSSID Finger 4 toa	*/
#endif
} OSAL_PACKED_STRUCTURE rxq_id_t;


#ifdef CONFIG_HAVE_STATS
/**
 *	@brief protocol_stats_t : contains stat info
 */
typedef struct protocol_stats {
	OSAL_u8 rsvd[4];		/**< Reserved fields. In case of spi transmission, it will be used for ack */
	OSAL_u32 megaframe;	/**< Current mega frame */
	OSAL_u16 hyperframe;	/**< Current hyper frame */
	OSAL_u8	superframe;	/**< Current super frame */
	OSAL_u8 rsvd2;
	OSAL_u32 entries[STATS_NB];	/**< entries for stats */
} protocol_stats_t;
#endif //#ifdef CONFIG_HAVE_STATS

/**
 *	@brief rng_info : contains information about the ranging
 *	@note This structure is aligned and sized 8 bytes
 */
typedef struct rng_time
{
	OSAL_u32 mega_frame;	/**< Counter incremented each 32 hyperframe*/
	OSAL_u16 hyper_frame;	/**< Counter incremented each 6 superframe */
	OSAL_u8	super_frame;	/**< Superframe counter */
	OSAL_u8	slot;		/**< Number of slot */
} OSAL_PACKED_STRUCTURE rng_time_t; 

/**
 *	@brief loc_toa_t : contains toa le
 *	@note This structure is aligned and sized 4 bytes
 */ 
typedef struct loc_toa
{
	OSAL_u32 toa_le;	/**< Time of arrival( nano) */
} OSAL_PACKED_STRUCTURE loc_toa_t;

/**
 *	@brief loc_metrics_t : contains nrj summary for channel
 *	@note This structure is aligned and sized 20 bytes.
 */ 
typedef struct loc_metrics
{
	OSAL_u64 nrj_sum;		/**< Sum of values of estimate.nrj */
	OSAL_u32 med;			/**< Mean Excess Delay of energy */
	OSAL_u32 rms;			/**< RMS Delta Spread of energy*/	
	OSAL_u8	nb_spl;			/**< Number of valid sample to compute the nrj sum */			
	OSAL_u8	rsvd3[3];		/**< 32 bits alignement	*/
} OSAL_PACKED_STRUCTURE loc_metrics_t;

/**
 *	@brief loc_nrj_t : contains finger information
 *	@note This structure is aligned and sized 32 bytes.
 */ 
typedef struct loc_nrj
{
	OSAL_u32 toa_fg1;		/**< Finger 1 toa */
	OSAL_u32 toa_fg2;		/**< Finger 2 toa */
	OSAL_u32 toa_fg3;		/**< Finger 3 toa */
	OSAL_u32 toa_fg4;		/**< Finger 4 toa */
	OSAL_u32 rxqid_fg1;		/**< RSSID Finger 1 toa	*/
	OSAL_u32 rxqid_fg2;		/**< RSSID Finger 2 toa	*/
	OSAL_u32 rxqid_fg3;		/**< RSSID Finger 3 toa	*/
	OSAL_u32 rxqid_fg4;		/**< RSSID Finger 4 toa	*/
} OSAL_PACKED_STRUCTURE loc_nrj_t;

/**
 *	@brief loc_phy_t : contains most important value for phy config retruned trough ranging
 *	@note This structure is aligned and sized 12 bytes
 */ 
typedef struct loc_phy
{
	OSAL_u32 sync_threshold;	/**< Thresholds used for preamble detection.0 mean automatic */
	OSAL_u32 rng_threshold;		/**< Thresholds used for leading edge detection.0 mean automatic */
	OSAL_u8	channel;		/**< Channel definition for this ranging:Low band : channels 1, 2/4 and 3, High band: channels 8, 9/11 */
	OSAL_u8	zone;			/**< Zone definition for this ranging */
	OSAL_u8	rsvd[2];		/**< 32bits alignement */
} OSAL_PACKED_STRUCTURE loc_phy_t;

/**
 *	@brief loc_estimate_t : groups all info needed to reconstruct pulse
 *	@note This structure is aligned and is 328 bytes.
 */
typedef struct loc_estimate
{
	OSAL_u8	iwle;			/**< IW Time of arrival */
	OSAL_u8	iwfg1;			/**< IW Finger 1 toa	*/
	OSAL_u8	iwfg2;			/**< IW Finger 2 toa	*/
	OSAL_u8	iwfg3;			/**< IW Finger 3 toa	*/
	OSAL_u8	iwfg4;			/**< IW Finger 4 toa	*/
	OSAL_u8 iwfat;			/**< IW First Above Thresold */
	OSAL_u8	rsvd2[2];		/**< 32 bits alignement	*/
	OSAL_u8	prp[EST_SIZE];		/**< Channel estimation memory, PRP index 64*/			
	OSAL_u32 nrj[EST_SIZE];		/**< Channel estimation memory, cumulated energy values256*/
} OSAL_PACKED_STRUCTURE loc_estimate_t;

/**
 *	@brief rng_protocol_header_t header of nb of packet to be sent 
 *	@note This structure is aligned and is 12 bytes.
 *      @note all field in this structure are filled in big endian
 */
typedef struct rng_protocol_header
{
	OSAL_u32 receiver_hash;		/**< Short ID of ME THAT IM HERE */
	OSAL_u32 ref_base_hash;		/**< Short ID of base on which I am sync */
	OSAL_u32 mega_frame;		/**< Counter incremented each 32 hyperframe ?*/
	OSAL_u16 hyper_frame;		/**< Counter incremented each 6 superframe */
	OSAL_u8	super_frame;		/**< Superframe counter */
	OSAL_u8 nb_entry;		/**< Nb of entry in burst */	
} OSAL_PACKED_STRUCTURE rng_protocol_header_t;

/**
 *	@brief beacon_protocol_entry_basic_t : entry for each slot of type unified beacon
 *	@note This structure MUST BE ALIGNED ON 32 bits, which is the case since its 12 bytes.
 * 	@note must be always placed at [frame]_protocol_entry_t->data[0]
 */
typedef struct beacon_protocol_entry_basic
{
	OSAL_u32 src_hash;				/**< Short ID of device emitting the TX.Endianess indicated in rngDataType */
	OSAL_u16 hyperframe_number;		/**< beacon hf number*/
	OSAL_u8 beacon_id;				/**< beacon id */
	OSAL_u8 data_slot;				/**< framer data slot  */
	OSAL_bool_t moving;				/**< moving or not*/
	OSAL_u8 superframe_number;		/**< beacon sf number */
	OSAL_u8 rsvd[2];				/**< reserved for alignement */
} OSAL_PACKED_STRUCTURE beacon_protocol_entry_basic_t;

/**
 *	@brief beacon1_protocol_entry_basic_t : entry for each slot of type BEACON1
 *	@note This structure MUST BE ALIGNED ON 32 bits, which is the case since its 8 bytes.
 *	@note must be always placed at [frame]_protocol_entry_t->data[0]
 */
typedef struct beacon1_protocol_entry_basic
{
	OSAL_u32 src_hash;				/**< Short ID of device emitting the TX.Endianess indicated in rngDataType */
	OSAL_u16 hyperframe_number;		/**< beacon hf number*/
	OSAL_u8 beacon_id;				/**< beacon id */
	OSAL_u8 rsvd;					/**< reserved for alignement */
} OSAL_PACKED_STRUCTURE beacon1_protocol_entry_basic_t;

/**
 *	@brief beacon2_protocol_entry_basic_t : entry for each slot of type BEACON2
 *	@note This structure MUST BE ALIGNED ON 32 bits, which is the case since its 8 bytes.
 *	@note must be always placed at [frame]_protocol_entry_t->data[0]
 */
typedef struct beacon2_protocol_entry_basic
{
	OSAL_u32 src_hash;			/**< Short ID of device emitting the TX.Endianess indicated in rngDataType */
	OSAL_u8 data_slot;			/**< framer data slot  */
	OSAL_bool_t moving;			/**< moving or not*/
	OSAL_u8 superframe_number;	/**< beacon sf number */
	OSAL_u8 rsvd;				/**< reserved for alignement */
} OSAL_PACKED_STRUCTURE beacon2_protocol_entry_basic_t;

/**
 *	@brief rng_protocol_entry_basic : entry for each slot of type RANGING
 *	@note This structure MUST BE ALIGNED ON 32 bits, which is the case since its 8 bytes.
 *	@note must be always placed at [frame]_protocol_entry_t->data[0]
 */
typedef struct rng_protocol_entry_basic
{
	OSAL_u32 src_hash;		/**< Short ID of device emitting the TX.Endianess indicated in rngDataType */
	OSAL_u32 dst_hash;		/**< Short ID of device destination.Endianess indicated in rngDataType */
} OSAL_PACKED_STRUCTURE rng_protocol_entry_basic_t;

/**
 *	@brief rdv_ack_protocol_entry_basic_t : entry for each slot of type RDV ACK 
 *	@note This structure MUST BE ALIGNED ON 32 bits, which is the case since its 12 bytes.
 *	@note must be always placed at [frame]_protocol_entry_t->data[0]
 */
typedef struct rdv_ack_protocol_entry_basic
{
	OSAL_u32 src_hash;		/**< Short ID of device emitting the TX.Endianess indicated in rngDataType */
	OSAL_u32 dst_hash;		/**< Short ID of device destination.Endianess indicated in rngDataType */
	OSAL_u8 seq_nb;			/**< rdv seq nb */
	OSAL_u8 rsvd[3];		/**< reserved for alignement */
} OSAL_PACKED_STRUCTURE rdv_ack_protocol_entry_basic_t;

/**
 *	@brief rdv_pr_protocol_entry_basic : entry for each slot of type RDV PAIRING REQUEST
 *	@note This structure MUST BE ALIGNED ON 32 bits, which is the case since its 16 bytes.
 *	@note must be always placed at [frame]_protocol_entry_t->data[0]
 */
typedef struct rdv_pr_protocol_entry_basic
{
	OSAL_u32 dst_hash;			/**< Short ID of device destination.Endianess indicated in rngDataType */
	OSAL_u8	src_mac[UWB_MAC_LEN];		/**< UWB Mac address of the device that request pairing */
	OSAL_u8 seq_nb;				/**< rdv seq nb */
	OSAL_u8 auth_type_request;	/**< reserved for future use */
	OSAL_u8 caps;				/**< Bitfield describing capabilities of device requesting pairing */
	OSAL_u8 rsvd;				/**< reserved for alignement */
} OSAL_PACKED_STRUCTURE rdv_pr_protocol_entry_basic_t;

/**
 *	@brief rdv_spr_protocol_entry_basic_t : entry for each slot of type RDV SIMPLE PAIRING REPLY
 *	@note This structure MUST BE ALIGNED ON 32 bits, which is the case since its 20 bytes.
 *	@note must be always placed at [frame]_protocol_entry_t->data[0]
 */
typedef struct rdv_spr_protocol_entry_basic
{
	OSAL_u32 src_hash;		/**< Short ID of device emitting the TX.Endianess indicated in rngDataType */
	OSAL_u32 dst_hash;		/**< Short ID of device destination.Endianess indicated in rngDataType */
	OSAL_u32 new_dst_hash;	/**< New short id to be used*/
	OSAL_u8 seq_nb;			/**< rdv seq nb */
	OSAL_u8 pairing_answer;	/**< Was a slot allocated successfully */
	OSAL_u8 gts_type;		/**< Direction of frame, from UWB slave to UWB master or vice-versa */
	OSAL_u8 slot_nb;		/**< Allocated slot in the superframe */
	OSAL_u8 zone;			/**< UWB zone that will be used on specified slot (1 to 5) */
	OSAL_u8 it_start;		/**< At which superframe number the device should start ranging */
	OSAL_u8 channel;		/**< channel, 1,2,3,4 or	8,9,10,11 */
	gts_rate_t gts_rate;	/**< At which rate the device should range */
} OSAL_PACKED_STRUCTURE rdv_spr_protocol_entry_basic_t;

/**
 *	@brief rdv_dreq_protocol_entry_basic : entry for each slot of type RDV DISSOCIATION REQUEST
 *	@note This structure MUST BE ALIGNED ON 32 bits, which is the case since its 16 bytes.
 *	@note must be always placed at [frame]_protocol_entry_t->data[0]
 */
typedef struct rdv_dreq_protocol_entry_basic
{
	OSAL_u32 src_hash;		/**< Short ID of device emitting the TX.Endianess indicated in rngDataType */
	OSAL_u32 dst_hash;		/**< Short ID of device destination.Endianess indicated in rngDataType */
	OSAL_u8 seq_nb;			/**< rdv seq nb */
	OSAL_bool_t reboot;		/**< Should the requested device reboot after detaching */
	OSAL_u8 rsvd[2];		/**< reserved for alignement */
} OSAL_PACKED_STRUCTURE rdv_dreq_protocol_entry_basic_t;


/**
 *	@brief rdv_protocol_entry_basic : entry for each slot of type RDV ACK 
 *	@note This structure MUST BE ALIGNED ON 32 bits, which is the case since its 24 bytes.
 *	@note must be always placed at [frame]_protocol_entry_t->data[0]
 */
typedef struct rdv_protocol_entry_basic
{
	type_rdv_t type;		/**< rdv Type */
	OSAL_u8 rsvd[3];		/**< reserved for alignement */
	union{ //max size is the one of rdv_spr_protocol_entry_basic_t = 20 bytes
		rdv_ack_protocol_entry_basic_t ack;		/**< data in case RDV is an ACK */
		rdv_pr_protocol_entry_basic_t pr; 		/**< data in case RDV is a PAIRING REQUEST */
		rdv_spr_protocol_entry_basic_t spr; 	/**< data in case RDV is a SIMPLE PAIRING REPLY */
		rdv_dreq_protocol_entry_basic_t dreq;	/**< data in case RDV is a DISSOCIATION REQUEST */
	}sub;
} OSAL_PACKED_STRUCTURE rdv_protocol_entry_basic_t;


/**
 *	@brief rng_protocol_entry_t : entry for each slot.
 *	@note This structure is aligned and is max 488 bytes. the data part is 472
 */
typedef struct rng_protocol_entry
{
	#define RNG_PROTOCOL_ENTRY_COMMON_SIZE 8/**< the common size is sizeof(rng_config_e)+sizeof(u16)+2xsizeof(u8) = 8*/
	#define RNG_PROTOCOL_ENTRY_BASE_SIZE 24/**< the max size for an header is the one in rdv , aka 24 bytes*/
	#define RNG_PROTOCOL_ENTRY_MAX_UWBDATA_SIZE 128 /**< maximum size for UWB data  */
	#define RNG_PROTOCOL_ENTRY_MAX_INFO_SIZE 344 /**< maximum size for channel info, until we consider overload = (sizeof(loc_toa_t)+sizeof(loc_phy_t)+sizeof(loc_estimate)) */

	//define here the max size we can authorize, until overload. 
	//to opimize memory, we can asssume that worst case is RNG_PROTOCOL_ENTRY_MAX_INFO_SIZE(loc_toa, phy, estimate +  14 u32 of datas)
	//scalable until a certain point then
	#define PROTOCOL_ENTRY_MAX_DATA_SIZE (RNG_PROTOCOL_ENTRY_BASE_SIZE+RNG_PROTOCOL_ENTRY_MAX_INFO_SIZE)
	#define PROTOCOL_ENTRY_MAX_SIZE (RNG_PROTOCOL_ENTRY_COMMON_SIZE+PROTOCOL_ENTRY_MAX_DATA_SIZE)

	rng_config_e rngDataType;	/**< Type of ranging data, used to interpret next data, on 4bytes.This filed is always in BIG ENDIAN*/
	OSAL_u16 status;		/**< status */
	OSAL_u8 slotNb;			/**< slot nb for this RDV */
	OSAL_u8 link_quality;	/**< link quality */
	OSAL_u8	data[PROTOCOL_ENTRY_MAX_DATA_SIZE]; /**< 	Ranging Data up to 128bytes, LOC info up to RNG_PROTOCOL_ENTRY_MAX_INFO_SIZE
												This data always begin with basic entry which can be rng_protocol_entry_basic,beacon1_protocol_entry_basic,beacon2_protocol_entry_basic,rdv_protocol_entry_basic
												There is always a padidng between Ranigng Data, and LOC data,
												in order to let LOC data begin at multiple of 32bits
												Exemple :  if ranging data is 1023 bits, loc data will begin at data[128]
												* If ranging data is 1015 bits, loc data will begin at data[128]
												* If ranging data is 546 bits, loc data will being at data[72]
												* More Generally, loc[index], with index = (DATA sizeinbits%32==0)?DATA sizeinbits/8:((DATA sizeinbits/32)+1)*4
												*/
	
} OSAL_PACKED_STRUCTURE rng_protocol_entry_t;

/**
 *  @brief average_rng_measure_t : - groups metrics for average TOA, average min and max nrj
 *  @note This structure is aligned and is 20 bytes
 */
typedef struct average_rng_measure
{
	OSAL_u32 rangingNB;	/**<total number of ranging */
	OSAL_s32 averageTOA;	/**<average toa got */
	OSAL_s32 averageNRJ;	/**<average fg1 NRJ */
	OSAL_s32 minNRJ;	/**<min of fg1 NRJ got */
	OSAL_s32 maxNRJ;	/**<max of fg1 NRJ got */
} OSAL_PACKED_STRUCTURE average_rng_measure_t;

/**
 *	@brief frame_info : - Describe a single slot in a superframe schema
 *	@note This structure is aligned and sized 16 bytes
 */
typedef struct frame_info {
	OSAL_u8	slot_nb;		/**< Slot Number, 0 to PSN. LSB is one PSS*/
	frame_info_type_t type;		/**< Type of slot */
	action_t act;			/**< Type of action */
	OSAL_u8	beacon_id;		/**< BeaconId (only used if type is BEACON1 or BEACON2 )*/
	OSAL_u8 zone;			/**< From 1 to 5 (inclusive) */
	OSAL_u8 channel;		/**< ETSI Channel: 1, 2, 3, 4	or 8, 9, 10, 11 */
	OSAL_u8 rf_path;		/**< Select different rf path (eg with multiple antennas) */
	OSAL_u8 rsvd1[1];		/**< 32 bits alignement */
	rng_config_e rngDataType;	/**< Type of ranging data, used to interpret next data, on 4bytes*/
	OSAL_u16 uwbPayloadInBits;	/**< size of uwb payload data (in bits) */
	OSAL_u8 rsvd2[2];		/**< 32 bits alignement */
} OSAL_PACKED_STRUCTURE frame_info_t;

/**
 *	@brief superframe_info : to store information concerning the superframe
 *	@note This structure is aligned and sized 528 bytes
 */
typedef struct superframe_info
{
	OSAL_u32 phs;			/**< Protocol Hyperframe Size : number of superframe per Hyperframe */
	OSAL_u32 psn;			/**< Protocol Slot Number: Numbers of slot in a superframe */
	OSAL_u32 pss;			/**< Protocol Slot Size: Size of a slot, in TS */
	OSAL_u16 nb_frames;		/**< Number of slots described in this frames_desc */	
	OSAL_u8	beacon_distance;	/**< Delay between a BEACON1 and a BEACON2, in PSS */
	OSAL_u8  sync_code_ID;		/**< Spreading code ID for synchro */
	OSAL_u8  demod_code_ID;		/**< Spreading code ID for demod */
	OSAL_u8  rsvd[3];		/**< Reserved:32 bits alignement */      
	frame_info_t frames_desc[MAX_FRAME_PER_SUPERFRAME]; /**<slot description */
} OSAL_PACKED_STRUCTURE superframe_info_t;

#ifdef CONFIG_RTLS_FULL_LOC

/**
 *	@brief dev_info : contains information of an UWB device
 *	@note This structure is aligned and sized 32 bytes
 */
typedef struct dev_info
{
	OSAL_u8		address[UWB_MAC_LEN];	/**< UWB Mac Address of the device, aka long id */	
	OSAL_u32 	hash_address;		/**< Hash of mac address, aka short id*/	
	dev_status_t	status;			/**< Device status */
	OSAL_u8		movement;		/**< Tell if the device is moving */
	OSAL_u8		level_bat;		/**< Tell the battery level*/
	OSAL_u8		loc_rate;		/**< LSR rate type	*/
	OSAL_u8		hwVersion;		/**< Device hardware version */
	OSAL_u8		swVersion;		/**< Device software version */
	OSAL_u8		sensor;			/**< Sensor value (unused for the moment)*/	
	OSAL_u8		rsvd;			/**< Reserved:32 bits alignement */
	OSAL_u32	secureKey;		/**< Device secure key (unused for the moment)*/
	OSAL_u32	MacCtrl1;		/**< Slot position	*/
	OSAL_u32	MacCtrl2;		/**< N.C. */
} OSAL_PACKED_STRUCTURE artls_dev_info_update_t;

#endif // defined CONFIG_RTLS_FULL_LOC

/**
 *	@brief artls_pairing_request : contains pairing request
 *	Pairing request are sent from a UWB Slave device to a UWB Master
 *	@note This structure is aligned and sized 16 bytes
 */
typedef struct artls_pairing_request {
	OSAL_u8 tag_mac[UWB_MAC_LEN];	/**< UWB Mac address of the device that request pairing */
	OSAL_u32 tag_hash;		/**< Short ID (aka hash) of the device requesting pairing */
	OSAL_u8 tag_profile;	/**< Bitfield describing capabilities of device requesting pairing */
	OSAL_u8 seq_id;			/**< Sequence number */
	OSAL_u8 rsvd[2];		/**< Reserved:32 bits alignement */
} OSAL_PACKED_STRUCTURE artls_pairing_request_t;

/**
 *	@brief artls_pairing_request : contains pairing reply
 *	Pairing reply are sent from a UWB Master device to a UWB Slave
 *	@note This structure is aligned and sized 20 bytes
 */
typedef struct artls_simple_pairing_reply {
	OSAL_u32 old_dst_hash;		/**< Previous short ID (aka hash) of UWB slave */
	OSAL_u32 new_dst_hash;		/**< Short ID (aka hash) that UWB slave now use */
	OSAL_u8 seq_id;			/**< Sequence number */
	OSAL_u8 pairing_answer;		/**< Was a slot allocated successfully */
	OSAL_u8 gts_type;		/**< Direction of frame, from UWB slave to UWB master or vice-versa */
	OSAL_u8 slot_nb;		/**< Allocated slot in the superframe */
	OSAL_u8 zone;			/**< UWB zone that will be used on specified slot (1 to 5) */
	OSAL_u8 it_start;		/**< At which superframe number the device should start ranging */
	OSAL_u8 channel;		/**< channel, 1,2,3,4 or	8,9,10,11 */
	OSAL_u8 gts_rate;		/**< At which rate the device should range */
	rng_config_e rngDataType;	/**< data configuration of the allocated slot */
} OSAL_PACKED_STRUCTURE artls_simple_pairing_reply_t;

/**
 *	@brief artls_dissociation_request: contains dissociation request
 *	Dissocatiation request are sent from a UWB Master device to a UWB Slave
 *	@note This structure is aligned and sized 8 bytes
 */
typedef struct artls_dissociation_request {
	OSAL_u32 dst_hash;	/**< Short ID (aka hash) of device that should dissociate */
	OSAL_bool_t reboot;	/**< Should the requested device reboot after detaching */
	OSAL_u8 seq_id;		/**< Sequence number */
	OSAL_u8 rsvd[2];	/**< Reserved:32 bits alignement */      
} OSAL_PACKED_STRUCTURE artls_dissociation_request_t;


#ifdef CONFIG_RTLS_FULL_LOC

/**
 *	@brief artls_pairing_request : contains pairing reply
 *	@note This structure is unaligned and sized 5 bytes 
 */
typedef struct artls_cmd_status {
	OSAL_u32 user_data;				/**< Internal defined data that will be sent back when command will be finished */
	artls_cmd_return_code_t status;	/**< status of sent ARTLS command */
} OSAL_PACKED_STRUCTURE artls_cmd_status_t;

/**
 *	@brief artls_up : contains artls notification - send from driver to userspace
*	@note This structure is UNALIGNED and sized 37 bytes.
 */
typedef struct artls_up
{
	OSAL_u8			reserved[4];	 	/**< Reserved data. Usefull for ack in case of Ext_com communication */
	artls_up_cmd_e		notif_type;	 	/**< signal which fields are updated */
	union {
		OSAL_u32		local_ref_base;	/**< Reference base of the local pinpointer */
		artls_lostatus_t	local_status;	/**< Status of the local pinpointer */
		artls_pairing_request_t pairing_req;	/**< Pairing request from a remote peer */
		artls_dev_info_update_t	device_info;	/**< Got Information of a remote peer */
		artls_dissociation_request_t dreq;	/**< Got a dissoctiation request from an UWB Master */
		artls_cmd_status_t	cmd_status;	/**< A command artls_down_t has finished */
	} payload;
} OSAL_PACKED_STRUCTURE artls_up_t;

/**
 *	@brief artls_down : contains artls notification - send from userspace to driver
*	@note This structure is aligned and sized 36 bytes.
 */
typedef struct artls_down
{
	artls_down_cmd_e cmd_type;			/**< type of artls (could be command defined data that will be sent back when command will be finished*/
	OSAL_u8 cmd_tracking;				/**< Define how the command status should be tracked*/
	OSAL_u8 rsvd[2];				/**< Reserved:32 bits alignement */
	OSAL_u32 user_data;				/**< User defined data that will be sent back when command will be finished*/
	OSAL_u32 sf_start;				/**< Superframe number at which the artls command must be run */
	OSAL_u32 sf_period;				/**< If command failed, repeat the command every sf_period */
	union {
		artls_simple_pairing_reply_t spr;	/**< used if cmd_type == ARTLS_SIMPLE_PAIRING_REPLY_DOWN*/
		artls_dissociation_request_t dreq;	/**< used if cmd_type == ARTLS_DISSOCIATION_REQUEST_DOWN */
		OSAL_u32 dev_id;			/**< used if cmd_type == ARTLS_SET_DEV_ID_DOWN*/
		OSAL_u32 prefered_ref_base;		/**< used if cmd_type == ARTLS_SET_PREFERED_REF_BASE_DOWN*/
		OSAL_u16 threshold_coeff;		/**< used if cmd_type == ARTLS_SET_THRESHOLD_COEFF */
		OSAL_s16 hw_offset;			/**< used if cmd_type == ARTLS_SET_HW_OFFSET */
	} payload;
} OSAL_PACKED_STRUCTURE artls_down_t;

#endif // defined CONFIG_RTLS_FULL_LOC


#ifdef CONFIG_HAVE_STATS
	#define CHECK_ENUM_STATS_SIZE do{CHECKMYSIZE(protocol_stats_type_t);}while(0)
	#define CHECK_STRUCT_STATS_SIZE do{CHECKMYSIZE(protocol_stats_t);}while(0)
#else
	#define CHECK_ENUM_STATS_SIZE 	do{}while(0)
	#define CHECK_STRUCT_STATS_SIZE do{}while(0)
#endif


#ifdef CONFIG_RTLS_FULL_LOC
	#define CHECK_STRUCT_FULLLOC_SIZE do{\
		CHECKMYSIZE(loc_estimate_t);\
		CHECKMYSIZE(loc_nrj_t);\
		CHECKMYSIZE(loc_metrics_t);\
		CHECKMYSIZE(artls_dev_info_update_t);\
		CHECKMYSIZE(artls_pairing_request_t);\
		CHECKMYSIZE(artls_simple_pairing_reply_t);\
		CHECKMYSIZE(artls_cmd_status_t);\
		CHECKMYSIZE(artls_up_t);\
		CHECKMYSIZE(artls_down_t);\
		}while(0)
		
#else

	#define CHECK_STRUCT_FULLLOC_SIZE do{}while(0)

#endif

#define CHECK_PROTOCOL_SIZE_ENUM do{ \
		CHECKMYSIZE(afc_config_e);\
		CHECKMYSIZE(devcaps_bitfield_e);\
		CHECKMYSIZE(type_trame_t);\
		CHECKMYSIZE(rng_config_e);\
		CHECKMYSIZE(frame_info_type_t);\
		CHECKMYSIZE(dev_status_t);\
		CHECKMYSIZE(artls_state_t);\
		CHECKMYSIZE(artls_lostatus_t);\
		CHECKMYSIZE(artls_up_cmd_e);\
		CHECKMYSIZE(artls_down_cmd_e);\
		CHECKMYSIZE(artls_cmd_return_code_t);\
		CHECKMYSIZE(artls_tracking_t);\
		CHECK_ENUM_STATS_SIZE;\
	}while(0)

#define CHECK_PROTOCOL_SIZE_STRUCT do{\
		CHECKMYSIZE(loc_toa_t);\
		CHECKMYSIZE(loc_phy_t);\
		CHECKMYSIZE(rng_protocol_header_t);\
		CHECKMYSIZE(rng_protocol_entry_t);\
		CHECKMYSIZE(rng_protocol_entry_basic_t);\
		CHECKMYSIZE(beacon_protocol_entry_basic_t);\
		CHECKMYSIZE(beacon1_protocol_entry_basic_t);\
		CHECKMYSIZE(beacon2_protocol_entry_basic_t);\
		CHECKMYSIZE(rng_protocol_entry_basic_t);\
		CHECKMYSIZE(rdv_ack_protocol_entry_basic_t);\
		CHECKMYSIZE(rdv_pr_protocol_entry_basic_t);\
		CHECKMYSIZE(rdv_spr_protocol_entry_basic_t);\
		CHECKMYSIZE(rdv_dreq_protocol_entry_basic_t);\
		CHECKMYSIZE(rdv_protocol_entry_basic_t);\
		CHECKMYSIZE(frame_info_t);\
		CHECKMYSIZE(superframe_info_t);\
		CHECKMYSIZE(toa_t);\
		CHECKMYSIZE(rxq_id_t);\
		CHECKMYSIZE(loc_estimate_t);\
		CHECKMYSIZE(rng_time_t);\
		CHECK_STRUCT_FULLLOC_SIZE;\
		CHECK_STRUCT_STATS_SIZE;\
	} while(0)


#endif //PINPOINTER_PROTOCOL_DEF_H

