/**
 * @file  prodSectionStruct.h
 * @brief  main map for prod section
 * @author jla@bespoon.com
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#ifndef _PRODSECTIONSTRUCT_H_
#define _PRODSECTIONSTRUCT_H_

#include <osal_type.h>

#define PRODSECTION_VERSION_NUMBER 	0x00000107

#define CONF_ANALOG_SIZE 		256
#define CONF_DIGIT_SIZE 		128
#define CONF_32K_SIZE 			16
#define CONF_SYSTEM_SIZE 		128
#define MUX_REG_SIZE 			64
#define TRIG_TABLE_SIZE 		32
#define MSQ_PMEM_SIZE 			2048
#define SPREADING_CODES_SIZE		124

#define PS_CHANNEL_NB			8
#define PS_ZONE_NB			5
#define PS_RFPATH_NB			4

#define CH_DEP_TAB_SIZE 		40

#pragma pack(1)

struct codesWord
{
    OSAL_u8 code127[16];
    OSAL_u8 code63[8];
    OSAL_u8 code31[4];
    OSAL_u8 code15[2];
    OSAL_u8 code7[1];
} OSAL_PACKED_STRUCTURE;


// UserSection: This part is used to set and get PP configuration.
// It can be modified by the user before generating prodSection
struct userSectionStruct
{
    struct
    {
        OSAL_u32 prodSection_version_number;
        OSAL_u8 rsvd;
        OSAL_u8 coordShortAddress[4];
        OSAL_u8 padding[7];
    } OSAL_PACKED_STRUCTURE generalCfg;			// 9 byte => 16

    struct
    {
        OSAL_u8 channel;
        OSAL_u8 zone;
        OSAL_u8 rsvd[62];
        OSAL_u8 locRate;
        OSAL_u8 beaconRate;
        OSAL_u8 maxBeaconListenRate;
        struct
        {
            OSAL_u16 phs;
            OSAL_u16 psn;
            OSAL_u16 pss;
            OSAL_u16 nbRelay;
            OSAL_u16 firstRangingSlot;
            OSAL_s16 rachSlot;
            OSAL_u8 zoneBeacon;
            OSAL_u8 zoneRanging;
            OSAL_u8 zoneRach;
            OSAL_u8 channel;
            OSAL_u8 sync_code_ID;
            OSAL_u8 demod_code_ID;
        } OSAL_PACKED_STRUCTURE default_sfi;
        OSAL_u16 UWBCapabilities;
        OSAL_u8 autostart;
        OSAL_u32 rngDataType;
        struct
        {
            OSAL_u32 subLocRate;
            OSAL_u16 rangingNb;
            OSAL_u16 assetTrackingType;              // Bitfield (asset_tracking_config_e)
            OSAL_u8 padding[8];
        } OSAL_PACKED_STRUCTURE assetTrackingCfg;   // 8 bytes => 16
        OSAL_u8 pm2;
        OSAL_u8 padding[19];
    } OSAL_PACKED_STRUCTURE defaultProtocolCfg;		// 109 bytes => 128

    struct
    {
        OSAL_u32 dacDefault;
        OSAL_s16 hardware_offsets[PS_RFPATH_NB];	// 0 & 1: Internal 0, 2 & 3: Internal 1
        OSAL_u8 rsvd[12];
        OSAL_u8 clock_track;
        OSAL_u8 framer_config;
        OSAL_u8 rsvd_config;
        OSAL_u8 tx_gain;
        OSAL_u8 rx_gain;
        OSAL_u8 rx_path;
        OSAL_u8 power_mode;
        OSAL_u8 sync_code_ID;
        OSAL_u8 demod_code_ID;
        OSAL_u16 threshold_coeff;
        OSAL_u8 padding[29];
    } OSAL_PACKED_STRUCTURE radioCfg;			// 31 bytes => 64

    // Module specific
    struct
    {
        OSAL_u32 shellUartBaudrate;			//uart 2 baudrate (0 mean default baudrate)
        #define SHELL_UARTNB_MASK    0xE
        #define SHELL_UARTSHELL_MASK 0x1
        OSAL_u8 shellConfig;				//uart shell config.bit7...4:NC, bit3..1: uart nb ( can be 1,2, 0 is default), bit0 shell activation
        #define TRACE_LEVEL_ERROR_ON 0x1
        #define TRACE_LEVEL_WARN_ON 0x2
        #define TRACE_LEVEL_INFO_ON 0x4
        #define TRACE_LEVEL_DEBUG_ON 0x8
        #define TRACE_LEVEL_VERBOSE_ON 0x10
        OSAL_u8 tracelevel;
        OSAL_u8 padding[10];
    } OSAL_PACKED_STRUCTURE moduleCfg;			// 6 bytes => 16

    OSAL_u8 padding[30];					// -2 for CRC
    OSAL_u16 crc;

} OSAL_PACKED_STRUCTURE;					// 256 bytes

// BurstSection: This part is obtained with the memorymap.xml and patched with the precedent configuration and the
// associated patch files
struct burstSectionStruct
{
    OSAL_u8 cfg_analog[CONF_ANALOG_SIZE];			// 256 bytes
    OSAL_u8 cfg_digital[CONF_DIGIT_SIZE];			// 128 bytes
    OSAL_u8 cfg_32k[CONF_32K_SIZE];				// 16 bytes
    OSAL_u8 cfg_system[CONF_SYSTEM_SIZE];			// 128 bytes
    OSAL_u8 mux_reg[MUX_REG_SIZE];				// 64 bytes
    OSAL_u8 trig_tabl[TRIG_TABLE_SIZE];			// 32 bytes
    OSAL_u8 msq_pmem[MSQ_PMEM_SIZE];			// 2048 bytes
    struct codesWord spreading_codes[4];			// 124 bytes
    OSAL_u8 padding[18];            			// -2 for CRC
    OSAL_u16 crc;
} OSAL_PACKED_STRUCTURE;					// 2674 bytes => 2816

// ManufacturerSection: This part is locked and should not be modified once flashed. It contains all the device
// specific parameters (addresses, tunnings...)
struct manufacturerSectionStruct
{
    OSAL_u8 ethernetMacAddress[6];
    OSAL_u8 longAddress[16];
    OSAL_u8 shortAddress[4];
#define BYTESINFO_TO_VERSION(_maj,_min,_details1,_details2) (((_maj & 0xFF)<<24)|((_min&0xFF)<<16)|((_details1&0xFF)<<8)|((_details2&0xFF)))
    OSAL_u32 chipVersion;
    OSAL_u32 emultagVersion;//should be called hwversion
    #define AGILE_LOC_MASK 0x01
    #define TEMP_CAPS_MASK 0x10
    #define CAN_ASSET_TRACKING_CAPS_MASK  0x40
    #define CAN_BSP_PROTOCOL_CAPS_MASK 0x80
    OSAL_u8 hardwareCapabilities;
    OSAL_u32 testsResults;
    OSAL_u32 thresholds[PS_ZONE_NB][PS_CHANNEL_NB];

    struct
    {
        OSAL_u8 bank;
        OSAL_u8 address;
        OSAL_u8 val[PS_CHANNEL_NB];			// id = channel-1
    } OSAL_PACKED_STRUCTURE channelDependentReg[CH_DEP_TAB_SIZE];

    #define SERIAL_NB_DIGIT_MIN 10
    #define SERIAL_NB_DIGIT_MAX 16
    OSAL_u8 serial[SERIAL_NB_DIGIT_MAX];			//serial number
    OSAL_u8 padding[406];					// -2 for CRC
    OSAL_u16 crc;
} OSAL_PACKED_STRUCTURE;					// 617 bytes => 1023


typedef struct prodSectionStruct
{
    struct userSectionStruct userSection;
    struct burstSectionStruct burstSection;
    struct manufacturerSectionStruct manufacturerSection;
} OSAL_PACKED_STRUCTURE prodSectionStruct_t;			// Total: 4095 bytes


#endif // PRODSECTIONSTRUCT_H
