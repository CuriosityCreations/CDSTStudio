/**
 * @file config_api.h
 * @brief api definition for pinpointer config
 * @author bgi@bespoon.com
 * @date 30/10/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _CONFIG_API_H_
#define _CONFIG_API_H_

#include <types.h>
#include "../config/prodSectionStruct.h"
#include <conf_common_api.h>
/** @brief initialize a config contexte (but the prod section)
 * @param cfg_ctx the context to initialize
 * @param enable_pm enable or not the Power Management
 * @param pm_msq use MSQ to apply Power Management sequences
 * @param lta_enable enable or disable LiveThresholdAdjustment
 * @return OSAL_OK on success, soemthing else otherwise
 */
OSAL_error_t pp_conf_init(struct pp_conf* cfg_ctx, OSAL_bool_t enable_pm, OSAL_bool_t pm_msq, OSAL_bool_t lta_enable);

/** @brief Get Synchronisation and Ranging thresholds for a given zone an channel
 * @param cfg_ctx        context of the config
 * @param zone           the zone for which user want to get the thresholds
 * @param channel        the channel for which user want to get the thresholds
 * @param rf_path        the rf_path for which user want to get the thresholds
 * @param sync_threshold the output synchro threshold for the given zone and channel
 * @param rng_threshold  the output ranging threshold for the given zone and channel
 */
OSAL_void pp_conf_get_threshold_from_zone(struct pp_conf* cfg_ctx, OSAL_u8 zone, OSAL_u8 channel, OSAL_u8 rf_path,
						 OSAL_u32* sync_threshold, OSAL_u32* rng_threshold);

/** @brief Get production Synchronisation thresholds for a given zone, channel and rf_path
 * @param cfg_ctx        context of the config
 * @param zone           the zone for which user want to get the thresholds
 * @param channel        the channel for which user want to get the thresholds
 * @param rf_path        the rf_path (1 or 2) for which user want to get the thresholds
 * @param sync_threshold the output synchro threshold for the given zone and channel
 */
OSAL_void pp_conf_get_prod_threshold_from_zone(OSAL_u8 zone, OSAL_u8 channel, OSAL_u8 rf_path,
					       OSAL_u32* sync_threshold);

#ifdef CONFIG_RTLS_PROTOCOL
/** @brief Fill tab32k with data in contexte, pt, act, rst_cpt & force_shadow
 * @param tab32k   output to write to BAR_32K_M
 * @param st       input
 * @param ts_delay input
 * @param force_ts_delay input
 * @param act      input
 * @param lclk_cal input
 * @param auto_switch input
 * @param rst_cpt  input
 * @param force_shadow input
 */
OSAL_void prep_config_32k(OSAL_u8 *tab32k,
	sequencer_time_t* st, OSAL_u32 ts_delay, OSAL_bool_t force_ts_delay,
	action_t act, OSAL_bool_t lclk_cal, OSAL_bool_t auto_switch, OSAL_bool_t rst_cpt, OSAL_bool_t force_shadow);
#endif //#ifdef CONFIG_RTLS_PROTOCOL

/** @brief get settings to apply to pinpointer to get switch from one PM state to another
 *  @param cfg_ctx Config context, contains old_pm
 *  @param new_pm Target PM mode
 *  @return setting_tab Array to put SPI sequences to switch from one <code>old_pm<\code> to <code>new_pm<\code>
 */
struct setting OSAL_PROD_SECT * prep_config_pm(struct pp_conf * cfg_ctx, pm_state_t new_pm);

/** @brief get settings to apply to put SPI sequences to reset MUX registers
 *  @return setting to put SPI sequences to reset MUX registers
 */
struct setting get_setting_mux_rst(void);

/** @brief get settings to apply to pinpointer to get switch from one PM state to another (without changing the 
 *         current pm)
 *  @param curr_pm source PM mode
 *  @param dest_pm Target PM mode
 *  @param Settings Array to put SPI sequences to switch from one <code>old_pm<\code> to <code>new_pm<\code>
 *  @return the next PM MODE to reach Target PM MODE
 */
pm_state_t get_config_pm(pm_state_t curr_pm, pm_state_t dest_pm,struct setting OSAL_PROD_SECT ** Setting);

/** @brief change pm
 *  @param new_pm new pm to apply
 *  @param cfg_ctx: used to get current_config_phy
 *  @param hw_priv_data context for spi access
 */
OSAL_void set_new_pm(pm_state_t new_pm, struct pp_conf* cfg_ctx, OSAL_void* hw_priv_data);

/** @brief force another prodsection
 *  @param prodSection: the new prodsection structure pointer
 */
OSAL_bool_t set_prodSection(prodSectionStruct_t *prodSection);

/** @brief generate initial PP config
 *  @param new_conf the config_phy at the end of the sequence
 *  @return pointer to setting tab
 */
struct setting OSAL_PROD_SECT * prep_config_init(config_phy_t * new_conf);

/** @brief generate exit PP config
 *  @return pointer to setting tab
 */
struct setting OSAL_PROD_SECT * prep_config_exit(OSAL_void);

/** @brief get firmware binary code
 *  @param fw_addr pointer on the firmware code
 *  @return size of the firmware
 */
OSAL_u16 get_fw_msq(OSAL_u8 OSAL_PROD_SECT ** fw_addr);

/** @brief set a given config (config_phy * new_config_phy) on pinpointer chip
 *  @param cfg_ctx: used to get current_config_phy
 *  @param new_config_phy
 *  @param new_config_tab: the settings table where differences between current_config_phy and new_config are stored
 *  @param force_rfpath: force internal and external RF switch parameters for runs without PM
 */
OSAL_u8 set_config_phy(struct pp_conf * cfg_ctx,
			 config_phy_t * new_config_phy,
			 struct setting * new_config_tab,
			 OSAL_u8 new_config_tab_size,
			 OSAL_bool_t force_rfpath);

#ifdef CONFIG_RTLS_PROTOCOL

/** @brief set a given config (seq_command_t * new_config) on pinpointer chip
 *  @param cfg_ctx: used to get current_config_phy
 *  @param new_config
 *  @param new_config_tab: the settings table where differences between current_config_phy and new_config are stored
 *  @param force_rfpath: force internal and external RF switch parameters for runs without PM
 *  @param pm2_wakeup_glitch: indicates if we were in PM2. If yes, wakeup may have been shifted of 1/2 TS, so had it in preamble_to
 */
OSAL_u8 set_config_phy_from_zone(struct pp_conf * cfg_ctx,
					seq_command_t * new_config,
					struct setting * new_config_tab,
					OSAL_u8 new_config_tab_size,
					OSAL_bool_t force_rfpath,
					OSAL_bool_t pm2_wakeup_glitch);
#endif //#ifdef CONFIG_RTLS_PROTOCOL

/** @brief get a pointer to the current config_phy
 *  @param cfg_ctx
 *  @param phy
 */
config_phy_t * get_current_config_phy(struct pp_conf * cfg_ctx);

/** @brief return the uwb mac address stored in prodsection structure
 */
OSAL_void get_longAddress(OSAL_u8 longAddress[UWB_MAC_LEN]);

/** @brief return the channelDefault stored in prodsection structure
 */
OSAL_u8 get_channelDefault(OSAL_void);

/** @brief return the zoneDefault stored in prodsection structure
 */
OSAL_u8 get_zoneDefault(OSAL_void);

/** @brief return the framer_config stored in prodsection structure
 */
OSAL_u8 get_framerCfgDefault(OSAL_void);

/** @brief return the dacDefault stored in prodsection structure
 */
OSAL_u32 get_dacDefault(OSAL_void);

#ifdef CONFIG_RTLS_PROTOCOL
/** @brief update current nbTS
 */
OSAL_void set_nbTS(OSAL_u32 new_nbTS);

/** @brief return the current nbTS used. Default nbTs comes from prodsection
 */
OSAL_u32 get_nbTS(OSAL_void);

/** @brief update current PHS
 */
OSAL_void set_PHS(OSAL_u32 new_PHS);

/** @brief return the current PHS used. Default PHS comes from prodsection 
 */
OSAL_u32 get_PHS(OSAL_void);

/** @brief return the Maximum beacon listen rate ( for deep sleep optimization)
 */
OSAL_u32 get_maxBeaconListenRate(OSAL_void);

#endif //#ifdef CONFIG_RTLS_PROTOCOL

/** @brief return the hardware offset stored in prodsection structure
 * value is in picoseconds and is to substract to the Time Of Arrival
 */
OSAL_void pp_conf_get_hw_offset(struct pp_conf* cfg_ctx, OSAL_s32* new_hw_offset, OSAL_u8 channel, OSAL_u8 rf_path, action_t act);

#ifdef CONFIG_RTLS_PROTOCOL
/** @brief Tell if device can do loc rate or not
 */
OSAL_void get_device_agility(struct pp_conf * cfg_ctx, dev_rate_types* lr);

/** @brief Get which sensors are presents on this device. Return value is hadrware dependent
 */
OSAL_u8 get_hwCaps(OSAL_void);

#define DEFAULT_SFI_PHS 6
#define DEFAULT_SFI_PSN 32
#define DEFAULT_SFI_PSS 64
#define DEFAULT_SFI_SLOT_RANGING 15
#define DEFAULT_SFI_RDV_SLOT 31
#define DEFAULT_SFI_CHANNEL 2
#define DEFAULT_NB_RELAY 6

#define DEFAULT_SFI_ZONE_BEACON 2
#define DEFAULT_SFI_ZONE_RANGING 2
#define DEFAULT_SFI_ZONE_RDV 2

/** @brief return default PHS
 */
OSAL_u16 get_sfi_PHS(OSAL_void);

/** @brief return default PSN
 */
OSAL_u16 get_sfi_PSN(OSAL_void);

/** @brief return default PSS
 */
OSAL_u16 get_sfi_PSS(OSAL_void);

/** @brief return default nbRelay
 */
OSAL_u16 get_sfi_nbRelay(OSAL_void);

/** @brief return default first ranging slot
 */
OSAL_u16 get_sfi_firstRangingSlot(OSAL_void);

/** @brief return ranging slot number
 */
OSAL_u16 get_sfi_rangingSlotNb(OSAL_void);

/** @brief return default sfi rdv slot
 */
OSAL_s16 get_sfi_rdvSlot(OSAL_void);

/** @brief return default sfi zone beacon
 */
OSAL_u8 get_sfi_zoneBeacon(OSAL_void);

/** @brief return default sfi zone ranging
 */
OSAL_u8 get_sfi_zoneRanging(OSAL_void);

/** @brief return default sfi zone rdv
 */
OSAL_u8 get_sfi_zoneRdv(OSAL_void);

/** @brief return default sfi channel
 */
OSAL_u8 get_sfi_channel(OSAL_void);

/** @brief return default sfi syncCodeID
 */
OSAL_u8 get_sfi_sync_code_ID(OSAL_void);

/** @brief return default sfi demodCodeID
 */
OSAL_u8 get_sfi_demod_code_ID(OSAL_void);

/** @brief return default UWB caps (bitfields with DEVCAPS_ROOT, DEVCAP_UWB_MASTER, ...)
 */
OSAL_u16 get_prot_UWBCaps(OSAL_void);

/** @brief tell if protocol should automatically start at boot
 */
OSAL_u8 get_prot_autostart(OSAL_void);

/** @brief tell if protocol can do PM2
 */
OSAL_u8 get_prot_pm2(OSAL_void);

#endif //#ifdef CONFIG_RTLS_PROTOCOL

/** @brief get root threshold (channel and RF_PATH dependant)
 *  @param cfg_ctx : used to get current_config_phy
 *  @param channel : root_threshold are channel dependant
 *  @param rf_path : root threshold are rf path dependant (1 or 2)
 *  @return THE root threshold
*/
OSAL_u32 pp_conf_get_root_threshold(struct pp_conf* cfg_ctx, OSAL_u8 channel, OSAL_u8 rf_path);

/** @brief set root threshold to be used on get threshold
 *  @param cfg_ctx : used to get current_config_phy
 *  @param channel : root_threshold are channel dependant
 *  @param rf_path : root threshold are rf path dependant (1 or 2)
 *  @param thld : the new threshold
*/
OSAL_void pp_conf_set_root_threshold(struct pp_conf* cfg_ctx, OSAL_u8 channel, OSAL_u8 rf_path, OSAL_u32 thld);

/** @brief set temperature to be used on get threshold via temp model
 *  @param temp temperature in Celsius degrees
 */
OSAL_void pp_conf_set_temperature(struct pp_conf* cfg_ctx, OSAL_s8 temperature);

/** @brief get temperature to be used on get threshold via temp model
 *  @return temperature in Celsius degrees
 */
OSAL_s8 pp_conf_get_temperature(struct pp_conf* cfg_ctx);

/** @brief set the aggressivity coefficient for threshold policy
 *  @param new_coeff the new muliplicative coefficient for the threshold policy
 *  @note this coefficient is the same for every Code/Repetition pair
 *  @note this coefficient divideved by 1000 when applied to threshold
 */
OSAL_error_t pp_conf_set_threshold_coeff(struct pp_conf* cfg_ctx, OSAL_u16 new_coeff);

/** @brief set hw_offset
 *  @param hw_offset the new hardware_offset to apply to BOTH TX and RX
 *  @note this value is applied in addition to the one in prodsection
 */
OSAL_error_t pp_conf_set_hw_offset(struct pp_conf* cfg_ctx, OSAL_s16 hw_offset);

/** @brief Tell if device has power management capabilities
 */
#define pp_conf_is_pm_enable(cfg_ctx) cfg_ctx->enable_pm

#ifdef CONFIG_RTLS_PROTOCOL
#ifdef CONFIG_RTLS_FULL_LOC
/** @brief Tell if device could manage an annoncement slot or not
 *         In particular, devices will answer pairing request
 */
OSAL_bool_t pp_conf_is_uwb_master_enable(struct pp_conf* cfg_ctx);

#endif // CONFIG_RTLS_FULL_LOC

/** @brief Tell if device is used for synchronization (root device)
 */
OSAL_bool_t pp_conf_is_root_enable(struct pp_conf* cfg_ctx);

/** @brief Tell if device should pair (and get ranging info) or not
 *         In particular, devices run pairing request before ranging
 */
OSAL_bool_t pp_conf_is_uwb_slave_enable(struct pp_conf* cfg_ctx);

/** @brief Tell if device should do scan_rf or not
 */
OSAL_bool_t pp_conf_is_scan_rf_enable(struct pp_conf* cfg_ctx);

/** @brief get asset tracking configuration
 */
asset_tracking_config_e get_assetTracking_config(struct pp_conf* cfg_ctx);

/** @brief get sub locrate for asset tracking
 */
OSAL_u32 get_assetTracking_subLocRate(struct pp_conf* cfg_ctx);

/** @brief get ranging number before sleep for asset tracking
 */
OSAL_u16 get_assetTracking_rangingNb(struct pp_conf* cfg_ctx);
#endif // defined CONFIG_RTLS_PROTOCOL

/** @brief en/dis -ables live threshold adjustment
 */
OSAL_void pp_conf_set_enable_lta(struct pp_conf* cfg_ctx, OSAL_bool_t en);

/** @brief get en/dis -ables live threshold adjustment flag
 */
OSAL_bool_t pp_conf_get_enable_lta(struct pp_conf* cfg_ctx);

/** @brief Helper for framing_config field 
 */
#define GET_FRAMING_STYLE(framer_config) (((framer_config) >> 4) & 0x03)
#define FRAMING_STYLE_LONG_PHR 		0x00
#define FRAMING_STYLE_SHORT_PHR 	0x01
#define FRAMING_STYLE_NO_PHR		0x02

#endif
