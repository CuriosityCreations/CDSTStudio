/**
 * @file types.h
 * @brief header structures for UWB protocol
 * @author bgi@bespoon.com
 * @date 12/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _PROTOCOL_TYPE_H_
#define _PROTOCOL_TYPE_H_
#include "./list.h"
#include "./memPool.h"
#include "./config_generic.h" 

#include <framer_types.h>
#include <callback_types.h>
#include <scan_rf_types.h>
#include <afc_filter_api.h>


typedef struct full_id {
	OSAL_u8 id[UWB_MAC_LEN];
} full_id_t;

typedef OSAL_u32 hash_id_t; 

//existing devices profiles 
typedef enum {
	PROFILE_TAG			= 0x01,
	PROFILE_BASE		= 0x02,
	PROFILE_COORDINATOR		= 0x04,
	PROFILE_ARTLS_BASE		= 0x10,
	PROFILE_ARTLS_SERVER	= 0x20,
} OSAL_PACKED_ENUM device_profiles_types;

//existing devices profiles 
typedef enum {
	LOCRATE_STATIC	= 0x01,
	LOCRATE_AGILE	= 0x02,
 } OSAL_PACKED_ENUM dev_rate_types;

//states of Automatic Frequency Compensation (AFC) state machine
typedef enum {
	AFC_DISABLED = 0,   // No AFC is running at all
	INIT_AFC_START,		// initial AFC with clock track 
	INIT_AFC_RUNNING,	// currently scanning to find a beacon2 and a few measure after
	LONG_RUN_AFC_START,	// Initialize for AFC on the long run start the ranging
	LONG_RUN_AFC_RUNNING, // Do AFC on the long run
} OSAL_PACKED_ENUM afc_status;

//states of ranging TX
typedef enum {
	NOT_ACTIVATED = 0,	// protocol not yet started
	SCAN,		 	// listen to catch a beacon from a base (only rx)
	RANGING_DISABLE,	// listen the beacon and don't make ranging (only rx)
	RANGING_ENABLE, 	// listen the beacon and make ranging (rx+tx)
} OSAL_PACKED_ENUM ranging_status;

typedef void (*fct_payload_getData) (OSAL_u8**,OSAL_u16*);

typedef struct next_event{
	type_trame_t last_tt;
	OSAL_u8 afc_status;  /** < manage the state of afc state machine */
	OSAL_u8 ranging_status; /** < manage the ranging status  */
	OSAL_bool_t clock_cal;/** < reajustement des temps logiciel pour etre en phase avec le coordinateur */
	OSAL_u32 mega_frame;
	OSAL_u16 hyper_frame;
	OSAL_u8 super_frame;/**< superframe on l'on se trouve (it) */
	OSAL_u8 position;/**< position dans la periode qui va etre traité */
	OSAL_u8 periode;/**< periode CFP ou CDP */
	OSAL_bool_t now_beacon; /**< Should listen to beacon on current superframe. This is used on tag in loc rate*/
	OSAL_bool_t now_rng;    /**< Should do TX ranging on current superframe. This is used on tag in loc rate */
	OSAL_u8 rsvd;
}next_event_t;



/**
 * @brief base_t defines a remote uwb_device that emits beacons
 */
typedef struct _base {
#ifndef CONFIG_FAST_BEACON_SWITCH
	filter_t* rxqid_filter;
#endif
	hash_id_t addr;		//< Short Address of this base (aka hash)
	OSAL_u8 score;
	OSAL_u8 beacon_ref; 	//< What is the beacon_id of this base
	OSAL_u8 zone;		//< On which zone the base is transmitting
	OSAL_u8 channel;	//< On which channel the base is transmitting
	OSAL_bool_t moving;	//< Is this base moving?
	struct {
		#define TSA_B1_OK 1
		#define TSA_B2_OK 2
		#define TSA_ALL_BEACON_OK (TSA_B1_OK | TSA_B2_OK)
		OSAL_u8 beacon_ok;	//< Was beacon 1 and 2 corectly receives
		OSAL_u8 lqi_b1;		//< LQI of Beacon 1 in %
		OSAL_u8 lqi_b2;		//< LQI of Beacon 2 in %
#ifdef CONFIG_FAST_BEACON_SWITCH
		OSAL_u8 rsvd;		//< 32 bits aligment
		OSAL_s32 err_b1;	//< in NTOA, used for TSA
		OSAL_s32 err_b2;	//< in NTOA, used for TSA
#else
		OSAL_bool_t listen_beacon;	//< Should I listen this base?
#endif
	} beacons;			//< Data for time slot adjustment
	OSAL_u8 beacon_rate_ratio;	//unused
	framer_data_slot_t data_slot;
	OSAL_u8 rsvd2[2];
}base_t;

#ifdef CONFIG_RTLS_FULL_LOC
typedef struct artls_linked
{
	list_t  	linked_list;
	union {
		artls_up_t 	up;
		artls_down_t	down;
	} data;
} artls_linked_t;

#endif // defined CONFIG_RTLS_FULL_LOC

typedef enum
{
	RST_POLICY_EACH_SUPERFRAME = 0,  // reset at the begining of each superframe: default value
	RST_POLICY_EACH_RX		 // reset at each RX
} OSAL_PACKED_ENUM reset_policy_t;

// Sequencer internal context
struct pp_seq
{
	OSAL_void* seqMemPool;			/**< a memory pool for sequencer.All memory allocation will be done within this pool, allowing to remove overhead of each single allocation*/
	list_t * buff_seq;			/**< queue of seq_command_ctx that are currently scheduled in the
							hardware pinpointer sequencer */
	list_t * pool_seq;			/**< pool (as a queue) of seq_command_ctx that are available to 
							schedule "NO_ACTION" seq_command_ctx in the sequencer */
	list_t * buff_phy;			/**< queue of seq_command_ctx received by the upper layer */
	list_t * buff_result_phy;		/**< queue of seq_command_ctx to be send back to the upper layer */
	struct {
		struct seq_command* pool;
		OSAL_u8* bitmap;		/**< Bitmap saying which sequencer_cmd_t in the array 
							<code>cmd_pool</code> is used */
		OSAL_u32 size;			//< Size of the <code>cmd_pool</code>
		OSAL_u32* frame_phy_addresses; 	//< physical_addr for each sequencer_cmd_t::frame pointer in the pool
	} cmd_pool;				/**< Pool of sequencer_cmd_t available for upper layer (see 
						<code>pp_seq_get_command_from_pool()</code> ) */
	struct {
		struct setting* pool;
		OSAL_u8 bitmap;
		OSAL_u8 rsvd[3];
	} conf_pool;

	struct pp_conf *cfg_ctx;
	OSAL_u32	reset_data_sframe_cnt;		//< number of super frame scheduled since last reset 
	OSAL_bool_t	reset_data_force_reset;		//< Do we need to enqueue a reset *before* the next frame
	reset_policy_t	reset_data_policy;		/**< when do we need to reset the sequencer. Temp hack (to handle
						 scan_rf case) */
	OSAL_bool_t	tsa_enable;		// < Are we doing time-slot readjustment (true on tag and base, false on coordinator)
	OSAL_bool_t	tx_block_cleared;	//< Is pinpointer (VHDL) TX block cleared. True after PM2->STDBY, false after first TX
	OSAL_bool_t	pm2_wakeup_glitch;	//< True if pinpointer was in PM2 and before TSA. Indicates that PP clocks may have shifted of 2 ts
	sequencer_time_t delta;		/** < time difference between software and hardware sequencer
					 this is mainly to ensure that RX are received with a delta_toa of 0 */
	sequencer_time_t next_delta;
	protocol_time_t	 last_good_beacon; // last time we receive a "good" (e.g. not rejected) beacon 2
	OSAL_void*	hw_priv_data;
	sequencer_time_t reset_time;
	OSAL_bool_t force_pm2_after_clear; // Used by vect0 actions (start_action, rxdone...) to go down in PM2 ater clearing intr_vect0
	OSAL_s16 base_ts;
	OSAL_s16 sf_shift; 			// If we fall in a forbidden case (end of sf) and are an UWB master we shift all sf of sf_shift
	OSAL_bool_t seq_need_restart; 		// Indicates if sequencer is stuck in a forbidden zone
};

typedef struct rdv_frame {
	list_t linked_list;	/**< List in which the frame is */
	hash_id_t dst_hash;	/**< Destinatair of the frame */
	type_rdv_t type;	/**< Type of rendez-vous frame */
	OSAL_u8 seq_nb;		/**< Sequence number */
	OSAL_u8 retry_cnt;	/**< How many time should I try to resent the frame */
	OSAL_bool_t notify_artls;	/**< should we call artls_notif_*() when the frame is sent */
	OSAL_u32 artls_priv_data;	/**< data set by artls_down_t command if any */
	OSAL_u32 it_start;
	OSAL_u32 beacon_rate;
	union {
		artls_pairing_request_t pr;
		artls_simple_pairing_reply_t spr;
		artls_dissociation_request_t dreq;
	} data;
} rdv_frame_t;

/**
 * @brief Pairing step of the pairing_state_machine
 * @note Used only on UWB_SLAVE (aka tag) side
 */
typedef enum pairing_status {
	PS_RDV_SLOT_UNKNOWN,
	PS_RDV_SLOT_FOUND,
	PS_SEND_PAIRING_REQUEST,
	PS_WAIT_FOR_ACK,
	PS_WAIT_FOR_SLOT,
	PS_WAIT_FOR_START,
	PS_ATTACHED,
} OSAL_PACKED_ENUM pairing_status_t;

/** 
 *	@brief State machine for tag attachment (used on UWB_SLAVE (aka tag) side)
* 	@note aligned structure and sized 8 bytes
*/
typedef struct pairing_state_machine
{
	hash_id_t base_addr;
	pairing_status_t status;
	OSAL_u8 timeout_attach_request;		// A time to wait, in superframe. Exact meaning depends on status. if 
						// status is RDV_SLOT_UNKNOWN, it's the number of SF to wait before 
						// doing an attach request (backoff behavior)
						// if status is RDV_WAIT_FOR_SLOT, it's the number of SF to wait becore
						// considering infra will not reply and redo a pairing request
	OSAL_u8 failed_req_cnt;			// How many time did we fail in the attach request (to compute backoff)
	OSAL_u8 rsvd;
} pairing_state_machine_t;

/** 
 *	@brief loc_rate_state_machine_t
* 	@note aligned structure and sized 4 bytes
*/
typedef struct loc_rate_state_machine
{
	OSAL_u8		ratio; //< Current loc rate 
	OSAL_u8		it_start;
	OSAL_bool_t	dynamic; 		// Tell if we need to do dynamic loc rate or static
	OSAL_u8		stationary_counter; //< How many time did we stay unmoving (in superframe)
}loc_rate_state_machine_t;

/** 
 *	@brief Data for Automatic Frequency Control computation
* 	@note aligned structure and sized 20 bytes
*/
typedef struct afc_beacon_data {
	protocol_time_t		time;
	OSAL_s32		err_beacon;	// delay of Beacon vs expected TOA (in NTOA)
	OSAL_bool_t		beacon_ok;	// was Beacon correctly received?
	OSAL_u8 		beacon_id;
	OSAL_u8 rsvd[2];
} afc_beacon_data_t;

/** 
 *	@brief frame_blocker_t 
 * 	@note aligned structure and sized 8 bytes
*/
typedef struct frame_blocker
{
	list_t linked_list;
	struct smbuff* blocker;		/**< please comment me */
	frame_info_type_t type;		/**< please comment me */
	action_t act;			/**< please comment me */
	OSAL_u8 rsvd[2];		/**<alignement on 32 bits */
} frame_blocker_t;


/**
 * @brief What are the limits and default values for AFC behavior
 */
struct default_afc_values {
#ifdef CONFIG_RTLS_FULL_LOC
	const char* name;			//< name of the configuration (for debug purpose)
#endif
	OSAL_u32 init_afc_filter_size;		//< Number of samples to do the average on INIT_AFC
	OSAL_u8 rerun_init_afc_threshold;	//< Above  this drift value, re-run INIT AFC
	OSAL_u8 max_init_afc_loop;		//< How many time could we loop for the INIT_AFC
	OSAL_bool_t enable_residual_drift_correction; //< Enable residual dirift correction
	OSAL_u8 dac_hysteresis;			//< hysteresis for DAC value (unused for now)
	filter_init_t long_run_afc_params;	// Parameters for kalman filter for LONG_RUN_AFC
};


/** 
 *	@brief context_t context for UWB protocol
*/
typedef struct context{
	OSAL_void* ctxMemPool;		/**< a memory pool for protocol.All memory allocation will be done within this pool, allowing to remove overhead of each single allocation*/

	list_t * pool_smbuff; 		/**< pool de smbuff a utiliser */
	list_t * buff_smbuff;		/**< queue of smbuff that must be framed (e.g. call framer() on it)*/
#ifdef CONFIG_RTLS_FULL_LOC
#ifdef CONFIG_HAVE_MULTITHREAD
	OSAL_fast_mutex mut_artls_in; 	/**< mutex to protect artls buffer in */
	OSAL_fast_mutex mut_artls_out; 	/**< mutex to protect artls buffer out */
	OSAL_fast_mutex mut_artls_pool; /**< mutex to protect artls pool */
#endif
	list_t * buff_artls_in;		/**< list's head for incoming artls stream */
	list_t * buff_artls_out; 	/**< list's head for outcoming artls stream */
	list_t * pool_artls; 		/**< list's pool for both kind of artls stream */
#endif

	// Blocked frames & FORCE_SHADOW
	list_t* pool_frame_blocker;	/**< Pool of frame_blocker_t for blocked_frames list */
	list_t* blocked_frames;		/**< Frames blocked (e.g. waiting for another frame (already scheduled) to be finished */

	// Device configuration
	hash_id_t	addr;		/**< Device short address (aka hash) */

	loc_rate_state_machine_t loc_rate; /**<Loc Rate management */

	next_event_t *  evenement;		/**<  definie le prochain type d'action */
#ifndef CONFIG_FAST_BEACON_SWITCH
	// Time Slot Adjustment
	struct {
		OSAL_u32	ts_b1;		/**< TS (protocol P.o.V) of beacon 1, 0 if B1 not received */
		OSAL_u32	ts_b2;		/**< TS (protocol P.o.V) of beacon 2, 0 if B2 not received */
		OSAL_s32	err_b1;		/**< delay of Beacon 1 vs expected TOA (in NTOA) */
		OSAL_s32	err_b2; 	/**< delay of Beacon 1 vs expected TOA (in NTOA) */
	} tsa;
#endif //!defined CONFIG_FAST_BEACON_SWITCH

	// AFC
	struct {
		OSAL_PROD_SECT struct default_afc_values* defaults;
		afc_beacon_data_t b1_last;		/**< FIXME not very clean, best solution would be to send each beacon_data directly in filter */
		afc_beacon_data_t b2_last;		/**< please comment me */
		afc_beacon_data_t b1_old;		/**< please comment me */
		afc_beacon_data_t b2_old;		/**< please comment me */

		OSAL_s32 	mem_err_beacon;		/**< correction offset used for B1TOB1 AFC (in NTOA) */
		OSAL_u8		long_run_afc_type;	/**< change afc type (B1toB1/B1toB2) depending on time between beacons */
		OSAL_u8		rsvd[3];		/**< alignement on 32 bits */
	} afc;

	filter_t* afc_filter;				/**< Automatic Frequency Adjustment */
	lta_ctx_t lta_ctx[LTA_MAX_CHANNEL];		/**< Live Threshold Adjustment context */

	// Device management
	base_t*  bases[MAX_BASES];		/**< please comment me */
	base_t * ref_base;			/**< define base on which we do Time Slot Adjustment, fixing protocol clock , ... (usually the coordinator), used for base and tag only */

	hash_id_t prefered_ref_base;  		/**< If set, connect to this base only */


	// Multithread management
#ifdef CONFIG_HAVE_MULTITHREAD
	volatile OSAL_s32 * mut;			/**< mutex pour stopper le protocol FIXME :create OSAL_VOLATILE*/
	volatile OSAL_s32 * mutGlobalConfig;		/**< mutex when changing global config (see config field) */
#endif //defined CONFIG_HAVE_MULTITHREAD
	volatile OSAL_s32 * mut_list;			/**< mutex pour proteger 'accés au list FIXME :create OSAL_VOLATILE*/

	// Superframe management
	superframe_info_t *sfi; /**< current frame description, point to sf_swap[0] or sf_swap[1] */
	OSAL_u8 padding[4]; 		/**< some padding for spi ack , when retrieving sfi*/
	#if defined(CONFIG_CAN_SWAP_FRAME)
		superframe_info_t sf_swap[2];		/**< please comment me */
		OSAL_bool_t dirty_superframe_schema;	/**<swap indicator */
		OSAL_u8 rsvd[2];
	#else
		superframe_info_t sf_swap[1];		/**< please comment me */
	#endif //CONFIG_CAN_SWAP_FRAME

#ifdef CONFIG_RTLS_FULL_LOC

#ifdef CONFIG_HAVE_MULTITHREAD
	volatile OSAL_s32 *mutSetSuperframeSchema;	/**< Mutex to protect changes on superframe schema */
#endif //defined CONFIG_HAVE_MULTITHREAD
#endif //defined CONFIG_RTLS_FULL_LOC

	// Callback management
	callback_ctx_t cb_afc_status; 		/**< AFC status changes */
	callback_ctx_t cb_ranging_status;	/**< Ranging status changes */
	callback_ctx_t cb_pairing_status;	/**< Attachment status changes */
	callback_ctx_t cb_next_superframe; 	/**< End of superframe */
	callback_ctx_t cb_ref_beacon_done; 	/**< Ref Beacon received or emitted */
	callback_ctx_t cb_tag_ranging_done;	/**< Tag ranging */
	callback_ctx_t cb_tag_led_on;		/**< Tag LED ON */
	callback_ctx_t cb_tag_led_off;		/**< Tag LED OFF */

#ifdef CONFIG_RTLS_FULL_LOC
	callback_ctx_t cb_frame;			/**< Callback run when got a trame */
	callback_ctx_t cb_artls;		/**< Callback run when got ARTLS data */
	callback_ctx_t cb_sfi_changed;		/**< Callback run when SFI Changed */
#else
	callback_ctx_t cb_dissociation;		/**< Callback run when dissociation request come */
#endif //defined CONFIG_RTLS_FULL_LOC

	struct pp_seq sequencer;		/**< Context of the sequencer layer (e.g. 'mac' module) */
	struct pp_conf * cfg_ctx;		/**< Context of the configuration module (e.g. 'config' module) */
	OSAL_void * hw_priv_data;		/**< Contexte for SPI access */


	// Slaves FSM (RDV and time syncho)
	pairing_state_machine_t pairing_fsm;	/**< State of the pairing on UWB_SLAVE devices (a.k.a tag) */
	scan_rf_state_machine_t scan_rf_fsm;	/**< State of the UWB synchronisation (a.k.a SCAN_RF) on time slave devices (e.g non-root devices)  */
	
	// RDV (Master & Slave)
	list_t* pool_rdv;			/**< Pool of rdv_frame_t that can be used to send RDV frames */
	list_t* tx_rdv;				/**< List of rdv_frame_t that must be send to other devices */
	list_t* unack_rdv;			/**< List of rdv_frame_t that has not been acknowledged by peer device */
	list_t* toack_rdv;			/**< List of rdv_frame_t that we receive and for which we must send an ack */
	
	// Data to send in ranging frames
	fct_payload_getData sensors_getter_cb[RNG_CONF_SENSOR_NUMBER]; /**< functions to retreive data from external sensors and send it in ranging frame */
	fct_payload_getData extdata_getter_cb;	/**< function to retreive external data and send it in ranging frame */
#ifdef CONFIG_HAVE_STATS
	OSAL_u32 current_stats_idx;		/**< Index of statistics to send in the next ranging frame, when
						  ranging payload is configured as rotation of stats */
#endif // CONFIG_HAVE_STATS

	// device info of this tag in tag mode
	rng_config_e    tag_info_rng_cfg;	/**< Configuration of Ranging frame payload (obtained during pairing)*/
	OSAL_u8		tag_info_position_loc;	/**< Ranging slot (obtained during pairing) - valid only on UWB_SLAVE */

	OSAL_u8         fin;			/**< defini l'arret du protocole */

	OSAL_u8 afc_init_loop_cnt;		/**< How many loop of afc_status = AFC_INIT_* we successively run*/
	OSAL_u16 nb_smbuff_in_seq;		/**< How many smbuff are scheduled in sequencer */
	OSAL_u8 afc_wrong_drift_nb;		/**< Number of wrong afc values in LONG_RUN_AFC*/
	OSAL_u8 ext_sensor;		        /**< Info about external sensor (lowbat, moving, ...)  - deprecated*/

	protocol_time_t sequencer_delta;	/**< time difference between protocol and (software) sequencer
						e.g. difference between smbuff_t::time and smbuff_t::sequencer_cmd::time */
#ifdef CONFIG_HAVE_STATS
	OSAL_u32 stats[STATS_NB];		/**< Collect various statistics about protocol */
#endif // defined CONFIG_HAVE_STATS

	// Asset Tracking
	struct {
		OSAL_u8  status;			/**< Asset tracking status */
		OSAL_u16 ranging_done_nb;		/**< Number of ranging done between each sleep */
		OSAL_u32 sequencer_shift;		/**< Sequencer shift to apply at each wake up, in TS */
		OSAL_s32 sleep_adjustment;		/**< Variable part of computed sleep time, in TS */
		OSAL_s32 first_startup_duration;	/**< Time to start MCU + sequencer, in TS. Set during first asset tracking step. */
		OSAL_s32 current_startup_duration;	/**< Time to start MCU + sequencer, in TS */
	} assetTracking;

} context_t;

// Sequencer request fields bitmask
#define SEQ_REQ_RX_FRAME		(1<<0) 	// get received frame
#define SEQ_REQ_TOA			(1<<1)	// fill toa_delay
#define SEQ_REQ_SET_RX_SIZE		(1<<2)	// fill the size field with the received size
#define SEQ_REQ_ESTIMATE		(1<<3)	// fill channel_estimates and channel_metrics
#ifdef CONFIG_RTLS_FULL_LOC
#define SEQ_REQ_RXQID			((1<<4) | SEQ_REQ_ESTIMATE)	// fill rxqid_data & iw_data, implies SEQ_REQ_ESTIMATE
#define SEQ_REQ_COARSE_RNG		(1<<5)	// fill sfd_time
#define SEQ_REQ_SCHEDULED_TIME		(1<<6)	// debug: fill seq_time
#define SEQ_REQ_CONFIG_PHY		(1<<7) // fill phy_config  
#endif // defined CONFIG_RTLS_FULL_LOC
#define SEQ_REQ_SAVE_TOA1		(1<<8)	// set struct pp_seq::ep_b1, struct pp_seq::toa_b1, struct pp_seq::ts_b1, ...
#define SEQ_REQ_SAVE_TOA2		(1<<9)	// set struct pp_seq::ep_b2, struct pp_seq::toa_b2, struct pp_seq::ts_b2, ...
#define SEQ_REQ_CLKTRK_DRIFT		(1<<10)	// set clktrk_drift  
#define SEQ_REQ_TOA_FINGERS		(1<<11)	// fill toa_fingers, actually don't work in tag only mode
#define SEQ_REQ_ADD_TSA			(1<<12)	// Add Time Stot Adjustment slots before this command
#define SEQ_REQ_ENABLE_PM2		(1<<13)	// Authorize sequencer to go in PM2
#define SEQ_REQ_CHECK_RX_SIZE		(1<<14) // Check that the received size match the size field (before setting it in case SEQ_REQ_SET_RX_SIZE is set)
#define SEQ_REQ_SF_END			(1UL<<15) // Indicate superframe end to sequencer

typedef enum {
	FRAME_STATUS_TX_OK = 0,
	FRAME_STATUS_RX_OK,
	FRAME_STATUS_CORRUPTED,	//CRC KO or frame size not as expected
	FRAME_STATUS_RX_TIMEOUT,
	FRAME_STATUS_SFD_TIMEOUT,
	FRAME_STATUS_CANCELED,
	FRAME_STATUS_UNKNOWN = 0xFF	// internal to sequencer
} OSAL_PACKED_ENUM frame_status;

/**
 *	@brief seq_command_t : Input/output structure for protocol/sequencer dialog
 *	@note This structure is aligned and is 468 bytes.
*/
typedef struct seq_command {
	void*			user_data;		/**< User-defined data (left untouched) */

// Input
	protocol_time_t 	time;			/**< when to schedule (input) */

	action_t		act;			/**< what to do: TX or RX (input) */
	OSAL_u8 		channel;		/**< UWB channel on which the action must be perform (input) */
	OSAL_u8			rf_path;		/**< RF path to use (e.g. with multiple antennas) (input) */
	OSAL_u8			framer_config;		/**< Pinpointer Framing config: (FEC, CRC, PHR) (input) */

	//FIXME: this must be computed from time.zone, not given by the protocol
	OSAL_u32		preamble_timeout; 	/**< preamble timout to use in case of RX (input) */

	OSAL_u16		request;		/**< which fields of the structure to fill at the end of action (input) */
	
// Input/output
	OSAL_u8			rsvd0[1];		/**< 32 bits alignement */
	OSAL_u8			size;			/**< the size of the frame, in bits (input/output) */

	OSAL_u8*		frame;			/**< a pointer to the frame (intput/output) */
	rtoa_t			toa_delay;		/**< For RX, it's delay (of leading edge) between measured Time Of Arrival and theoric one (output), for TX it's an additional fine delay to add to \ref seq_command::time time (input)*/


// output
	frame_status		status;			/**< TX_OK, RX_OK, TX_CRC_KO, RX_TIMEOUT, SFD_TIMEOUT, CANCELED,... always set (output) */
	OSAL_u8			link_quality;		/**< got link quality (output)*/

	OSAL_s16		clktrk_drift;   	/**< Drift estimation computed with clock track metrics (output) */

	loc_nrj_t		nrj;			/**< contain toa and rxqid for all finger (output)  */

	loc_estimate_t 		estimate;		/**< Pulse as it was seen by pinpointer (energy values) (ouput)*/
	OSAL_u32 		nrj_sum_fml;		/**< Sum of energy of First Maximul Local finger (output) */
	OSAL_u32 		ts_offset;		/**< Offset (in TS) added by sequencer to schedule the frame. Already integretated in toa_delay computation (output) */

#ifdef CONFIG_RTLS_FULL_LOC
	coarse_rng_t		sfd_time;		/**< when sfd was recv/send  (output) */
	loc_metrics_t		channel_metrics;	/**< contains nrj summary for channel (output)*/
	loc_phy_t		phy_config;		/**< Phy configuration at the time of the command is run (output)*/
	sequencer_time_t	seq_time;		/**< when scheduled in sequencer (ouput) (debug)*/
#endif // defined CONFIG_RTLS_FULL_LOC
} seq_command_t;

/**
 *  @brief smbuff : Define a frame in the UWB protocol
 *  @note This structure is aligned and sized 184 bytes.
 */
typedef struct smbuff {
	seq_command_t *sequencer_cmd;	/**< Data to send to sequencer */
	rdv_frame_t* rdv;		/**< Rendez Vous data associated with this smbuff */
	list_t linked_list; 		/**< structure for the double linked list*/
	protocol_time_t time;		/**< time/zone, frame counter info */
	OSAL_u32 peer_hash;		/**< Hash (id) of peer pinpointer */
	type_trame_t typeframe; 	/**< contains the type of frame,1 byte*/
	rng_config_e rngDataType;	/**< Current data configuration.*/
	type_rdv_t   typerdv;   	/**< sub type of the frame in case of RDV frame (typeframe == TT_RDV) */
	OSAL_u8 beacon_id;		/**< Expected beacon ID, used only for RX beacon */
	OSAL_error_t deframe_status;	/**< Does the frame we received was correctly deframed*/
	OSAL_u8 slot_nb;
#ifdef CONFIG_RTLS_FULL_LOC
	OSAL_u32 drift_value;		/**< drift value*/
	OSAL_u8 rsvd[3];		/**< alignement on 32 bits */
#endif // defined CONFIG_RTLS_FULL_LOC
	union {
		frame_content_beacon1_t b1;			/**< please comment me*/
		frame_content_beacon2_t b2;			/**< please comment me*/
		frame_content_tag_ranging_t rng;		/**< please comment me*/
		frame_content_rdv_ack_t rdv_ack;		/**< please comment me*/
		frame_content_pairing_request_t rdv_pr;		/**< please comment me*/
		frame_content_simple_pairing_reply_t rdv_spr;	/**< please comment me*/
		frame_content_dissociation_request_t rdv_dreq;	/**< please comment me*/
	}  frame_content;  					/**< This is an union of dirrent farame type. The worst case in memory sense is rng, with 144bytes */
} /*OSAL_PACKED_STRUCTURE*/ smbuff_t ;

/**
 * @brief Define which block to reset when starting protocol
 * @note Used for resuming protocol, which means reseting only some functions (AFC / TSA / RACH...)
 */
 typedef enum {
 	PROTOCOLRESET_SEQUENCER 	= 0x0001,		/**< sequencer */
 	PROTOCOLRESET_TSA 		= 0x0002,		/**< time slot adjustment */
 	PROTOCOLRESET_AFC		= 0x0004,		/**< afc */
 	PROTOCOLRESET_RACH		= 0x0008,		/**< rach */
 	PROTOCOLRESET_EVENT		= 0x0010,		/**< next event */
 	PROTOCOLRESET_STATS		= 0x0020,		/**< stats */
 	PROTOCOLRESET_LTA		= 0x0040,		/**< live threshold adjustment */
 	PROTOCOLRESET_TAGINFO		= 0x0080,		/**< tag info */
 	PROTOCOLRESET_LOCRATE		= 0x0100,		/**< locrate */
 	PROTOCOLRESET_FRAMEBLOCKERS	= 0x0200,		/**< frame blockers */
 	PROTOCOLRESET_ASSETTRACKING	= 0x0400,		/**< asset tracking */
 	PROTOCOLRESET_DAC		= 0x0800,		/**< DAC */
 	PROTOCOLRESET_POSITION		= 0x1000,		/**< position in SFI */
 	PROTOCOLRESET_ALL		= 0xFFFF		/**< reset everything */
} OSAL_PACKED_ENUM protocol_reset_e;

#endif
