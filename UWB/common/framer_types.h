/**
 * @file framer_types.h
 * @brief definitions of structures for UWB RTLS framer
 * @author cca@bespoon.com
 * @date 23/09/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */


#ifndef _FRAMER_TYPES_H_
#define _FRAMER_TYPES_H_
#include <osal_type.h>

/**
 *	@brief frame_content_beacon1_t
 *	@note This structure is sized 7 bytes
 */ 
typedef struct frame_content_beacon1 {
    	OSAL_u32 src_hash;			/**< please coment me */
	OSAL_u16 hyperframe_number;		/**< please coment me */
	OSAL_u8 beacon_id;			/**< please coment me */
} OSAL_PACKED_STRUCTURE frame_content_beacon1_t;

/**
 *	@brief frame_content_beacon2_t
 *	@note This structure is sized 7 bytes
 */
typedef struct frame_content_beacon2 {
	OSAL_u32 src_hash;			/**< please coment me */
	framer_data_slot_t data_slot;		/**< please coment me */
	OSAL_bool_t moving;			/**< please coment me */
	OSAL_u8 superframe_number;		/**< please coment me */
} OSAL_PACKED_STRUCTURE frame_content_beacon2_t;

/**
 *	@brief frame_content_tag_ranging_t
 *	@note This structure is sized 144 bytes
 */
typedef struct frame_content_tag_ranging {
	OSAL_u32 src_hash;			/**< please coment me */
	OSAL_u32 dst_hash;			/**< please coment me */
	OSAL_u32 data_size;			/**< size of data in bits */
	OSAL_u8 data[128];			/**< data ( can be sensor, bat/mov, or user data )*/	
} OSAL_PACKED_STRUCTURE frame_content_tag_ranging_t;

/**
 *	@brief frame_content_rdv_ack_t
 *	@note This structure is sized 9 bytes
 */
typedef struct frame_content_rdv_ack {
	OSAL_u32 src_hash;			/**< please coment me */
	OSAL_u32 dst_hash;			/**< please coment me */
	OSAL_u8 seq_nb;				/**< please coment me */
} OSAL_PACKED_STRUCTURE frame_content_rdv_ack_t;

/**
 *	@brief frame_content_pairing_request_t
 *	@note This structure is sized 17 bytes
 */
typedef struct frame_content_pairing_request {
	OSAL_u32 dst_hash;			/**< please coment me */
	OSAL_u8	src_mac[UWB_MAC_LEN];		/**< please coment me */
	OSAL_u8 seq_nb;				/**< please coment me */
	OSAL_u8 auth_type_request;		/**< please coment me */
	OSAL_u8 caps;				/**< please coment me */
} OSAL_PACKED_STRUCTURE frame_content_pairing_request_t;

/**
 *	@brief frame_content_simple_pairing_reply_t
 *	@note This structure is sized 21 bytes
 */
typedef struct frame_content_simple_pairing_reply {
	OSAL_u32 src_hash;			/**< please coment me */
	OSAL_u32 dst_hash;			/**< please coment me */
	OSAL_u32 new_dst_hash;			/**< please coment me */
	OSAL_u8 seq_nb;				/**< please coment me */
	OSAL_u8 pairing_answer;			/**< please coment me */
	OSAL_u8 gts_type;			/**< please coment me */
	OSAL_u8 slot_nb;			/**< please coment me */
	OSAL_u8 zone;				/**< please coment me */
	OSAL_u8 it_start;			/**< please coment me */
	OSAL_u8 channel;			/**< please coment me */
	OSAL_u32 rngDataType;			/**< please coment me */
	gts_rate_t gts_rate;			/**< please coment me */
} OSAL_PACKED_STRUCTURE frame_content_simple_pairing_reply_t;

/**
 *	@brief frame_content_dissociation_request_t
 *	@note This structure is sized 10 bytes
 */
typedef struct frame_content_dissociation_request {
	OSAL_u32 src_hash;			/**< please coment me */
	OSAL_u32 dst_hash;			/**< please coment me */
	OSAL_u8 seq_nb;				/**< please coment me */
	OSAL_bool_t reboot;			/**< please coment me */
} OSAL_PACKED_STRUCTURE frame_content_dissociation_request_t;

#if 0
/**
 *	@brief frame_content_beacon1_t
 *	@note This structure is sized 19 bytes
 */
typedef struct frame_content_device_info_t {
        OSAL_u32 src_hash;			/**< please coment me */
        OSAL_u32 dst_hash;			/**< please coment me */
        OSAL_u32 caps;				/**< please coment me */
        OSAL_u8 reply_cnt;			/**< please coment me */
        OSAL_u8 transaction_id;			/**< please coment me */
        OSAL_bool_t moving;			/**< please coment me */
        OSAL_bool_t low_bat;			/**< please coment me */
        OSAL_u8 sw_version;			/**< please coment me */
        OSAL_u8 hw_version;			/**< please coment me */
        OSAL_u8 supported_rates;		/**< please coment me */
} OSAL_PACKED_ENUM frame_content_device_info_t;

/**
 *	@brief frame_content_device_device_uwb_mac_t
 *	@note This structure is sized 26 bytes
 */
typedef struct frame_content_device_uwb_mac {
        OSAL_u32 src_hash;			/**< please coment me */
        OSAL_u32 dst_hash;			/**< please coment me */
        OSAL_u8 address[16];			/**< please coment me */
        OSAL_u8 reply_cnt;			/**< please coment me */
        OSAL_u8 transaction_id;			/**< please coment me */
} OSAL_PACKED_ENUM frame_content_device_device_uwb_mac_t;

/**
 *	@brief frame_content_device_device_friendly_name_t
 *	@note This structure is sized 26 bytes
 */
typedef struct frame_content_device_friendly_name {
        OSAL_u32 src_hash;			/**< please coment me */
        OSAL_u32 dst_hash;			/**< please coment me */
        OSAL_u8 friendly_name[16];		/**< please coment me */
        OSAL_u8 reply_cnt;			/**< please coment me */
        OSAL_u8 transaction_id;			/**< please coment me */
} OSAL_PACKED_ENUM frame_content_device_device_friendly_name_t;
#endif //#if 0

#endif
