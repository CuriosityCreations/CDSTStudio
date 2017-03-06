/**
 * @file pinpointer_config.c
 * @brief prepare both PHY and Sequencer registers config
 * @author bgi@bespoon.com
 * @date 30/10/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include <types.h>
#include <pinpointer_regs.h>
#include <config_api.h>
#include <time_helper_api.h>
#include <osal_trace.h>
#include "pinpointer_hw_api.h"
#include "msq_config_api.h"
#include "pm_config.h"
#include "init_config.h"
#include "prodSection_read.h"
#include "prodSectionStruct.h"

#define ZONE_COEFF_MULTIPLIER 1000
#define DEFAULT_THLD_AGGRESIVITY_COEF 1250 // multiplicative coeff. Approximatively 1 db above the noise

//#define BYPASS_PRODFW_MSQ
#ifdef BYPASS_PRODFW_MSQ
 	#include "msq_fw.h"
#endif


#define PP_CFG_DEV_TRACE(fmt, args...) do{}while(0)
//#define PP_CFG_DEV_TRACE(fmt, args...) do{OSAL_trace(TRACE_VERBOSE, fmt,##args);}while(0)

#define generate_add_setting_function(suffix, val_type, val_member)		\
static OSAL_u8 add_setting_ ## suffix (struct setting * tab, \
					treg_t type_reg, \
					OSAL_u8 bank, OSAL_u8 reg, val_type val, \
					OSAL_u8 i, OSAL_u8 new_config_tab_size) \
{ \
	if (type_reg != T_STOP && i >= new_config_tab_size - 1) \
		return new_config_tab_size; \
	tab[i].Type = type_reg; \
	tab[i].Bank = bank; \
	tab[i].Reg = reg; \
	tab[i].Val.val_member = val; \
	return(i + 1); \
}

generate_add_setting_function(u32, OSAL_u32, t32)
// generate_add_setting_function(u16, OSAL_u16, t16)
generate_add_setting_function(u8, OSAL_u8, t8)
#undef generate_add_setting_function

// If you need add_setting_tburst, uncomment the following code
/*static OSAL_u8 add_setting_tburst (struct setting * tab,
					treg_t type_reg,
					OSAL_u8 bank, OSAL_u8 reg, const OSAL_u8* ptr, OSAL_u8 len,
					OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	if (type_reg != T_STOP && i > new_config_tab_size - 2)
		return new_config_tab_size;

	tab[i].Type = type_reg;
	tab[i].Bank = bank;
	tab[i].Reg = reg;
	tab[i].Val.tburst.ptr = (OSAL_u8*) ptr;
	tab[i].Val.tburst.len = len;
	return(i + 1);
}*/

static OSAL_u8 add_setting_tprod_burst (struct setting * tab, 
					treg_t type_reg,
					OSAL_u8 bank, OSAL_u8 reg, OSAL_u8 OSAL_PROD_SECT* ptr, OSAL_u8 len,
					OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	if (type_reg != T_STOP && i > new_config_tab_size - 2)
		return new_config_tab_size;

	tab[i].Type = type_reg;
	tab[i].Bank = bank;
	tab[i].Reg = reg;
	tab[i].Val.tprodburst.ptr = ptr;
	tab[i].Val.tprodburst.len = len;
	return (i + 1);
}


OSAL_void pp_conf_set_enable_lta(struct pp_conf* cfg_ctx, OSAL_bool_t en)
{
	if (cfg_ctx != NULL)
		cfg_ctx->enable_live_thld = en;
}

OSAL_bool_t pp_conf_get_enable_lta(struct pp_conf* cfg_ctx)
{	
	return cfg_ctx->enable_live_thld;
}

OSAL_error_t pp_conf_init(struct pp_conf* cfg_ctx, OSAL_bool_t enable_pm, OSAL_bool_t pm_msq, OSAL_bool_t lta_enable)
{
	OSAL_u8 channel = 0, def_zone = 0, rf_path = 0;
	OSAL_u32 thld = 0;

	if (cfg_ctx == NULL)
		return OSAL_ERROR;

#ifdef CONFIG_RTLS_PROTOCOL
	cfg_ctx->caps_enabled = DEVCAPS_UWB_SLAVE; // Start as tag by default
#endif //#ifdef CONFIG_RTLS_PROTOCOL
	cfg_ctx->enable_pm = enable_pm;
	cfg_ctx->pm_msq = pm_msq;
	cfg_ctx->listen_all_beacons = OSAL_false;

	// Init threshold aggresivity coef
	// Some prod sections have not yet a valid coef (e.g. 0), so use the default one in this case
	cfg_ctx->threshold_coeff = prodSection.userSection.radioCfg.threshold_coeff >= 100 ? prodSection.userSection.radioCfg.threshold_coeff : DEFAULT_THLD_AGGRESIVITY_COEF;

	def_zone = get_zoneDefault();

	//Initialise Root threshold table (for both RF path and channel)
	for(rf_path = 0; rf_path < RFPATH_NB; rf_path++)
	{
		//TODO only BG0 for the moment
		for(channel = 1; channel <= RT_MAX_CHANNEL; channel++)
		{
			pp_conf_get_prod_threshold_from_zone(def_zone, channel, rf_path, &thld);
			pp_conf_set_root_threshold(cfg_ctx, channel, rf_path, thld);
		}
	}

	//default state of live threshold adjustment is user defined
	pp_conf_set_enable_lta(cfg_ctx, lta_enable);

	//default temperature is agressive for board without temp sensor
	// This will make threshold decrease when no treatment from IR available
	// keep it <55° to avoid 1v2pll toggle mechanism
	pp_conf_set_temperature(cfg_ctx, 54);

	//default state for the booster is OFF
	cfg_ctx->boost_1v2pll = OSAL_false;
	cfg_ctx->user_hw_offset = 0;

	return OSAL_OK;
}

/* Check temperature and set a higher 1V2Pll voltage for temperature above 55° */
/* When temperature decrease, go back to production value */
static OSAL_u8 prep_conf_1v2pll_boost(struct pp_conf* cfg_ctx,	OSAL_u8 channel,
				       struct setting * new_config_tab, 
				       OSAL_u8 i, OSAL_u8 new_config_tab_size)
{

	OSAL_u8 j;

	if((cfg_ctx->boost_1v2pll == OSAL_false) && (cfg_ctx->temperature > 55))
	{
		cfg_ctx->boost_1v2pll = true;
		i = add_setting_u8(new_config_tab, T_REG8,
				BAR_CFG_ANALOG, BAR_FS_TUNE_VDD1V2_PLL, 0x37,
				i, new_config_tab_size); 
	}
	else if ( (cfg_ctx->boost_1v2pll == OSAL_true) && (cfg_ctx->temperature < 55)) //1° hysteresis
	{
		for(j=0 ; j < CH_DEP_TAB_SIZE ; j++)
		{
			if ((prodSection.manufacturerSection.channelDependentReg[j].bank == BAR_CFG_ANALOG) &&
			   (prodSection.manufacturerSection.channelDependentReg[j].address == BAR_FS_TUNE_VDD1V2_PLL))
			{
				i = add_setting_u8(new_config_tab, T_REG8,
					BAR_CFG_ANALOG, BAR_FS_TUNE_VDD1V2_PLL, 
					prodSection.manufacturerSection.channelDependentReg[j].val[channel],
					i, new_config_tab_size); 
				cfg_ctx->boost_1v2pll = false;
				break;
			}
		}
	}

	return i;
}

static OSAL_u8 prep_conf_timeout(OSAL_u8 synch_code_lgth, OSAL_u8 synch_repetition_nb, OSAL_u32 preamble_timeout,
				struct setting * config, OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	/*Configuration of Timeouts according to repetition number and code length*/
	OSAL_u32 preamble_time_out = 0;
	OSAL_u32 sfd_time_out = 0;

	// Following variables are used for SFD Timeout computation
	OSAL_u32 first_scan_MS = 16;
	OSAL_u32 second_scan_MS = 16;
	OSAL_u32 SFD_MS = 16;
	OSAL_u32 SFD_timeout_MS;

	/***** Preamble Timeout ****/
	/*If preamble timeout bit 31 is equal to 1 that means we want auto setting of preamble timeout, else it's forced to 
	 the value in argument*/
	if(preamble_timeout & (1UL << 31))
	{
		preamble_timeout &= ~(1UL << 31);
		preamble_time_out = preamble_timeout + to_u32(4) * pp_conf_get_preamble_length(synch_code_lgth, synch_repetition_nb) *
						to_u32(synch_code_lgth) * to_u32(synch_repetition_nb);
	}
	else if (preamble_timeout != 0)
		preamble_time_out = preamble_timeout;

	i = add_setting_u32(config, T_REG32,
			BAR_CFG_DIGITAL, BAR_PREAMB_TIME_OUT_W0, preamble_time_out,
			i, new_config_tab_size);

	/***** SFD Timeout ****/
	if (synch_code_lgth < 7 || synch_code_lgth > 127 || synch_repetition_nb > 64)
	{
		OSAL_trace(TRACE_ERROR, "%d/%d/ BAD SYNCH CODE LENGTH OR REP", synch_code_lgth, synch_repetition_nb);
		return 0;
	}
	else
	{
		SFD_timeout_MS = pp_conf_get_preamble_length(synch_code_lgth, synch_repetition_nb) - first_scan_MS - second_scan_MS + SFD_MS + 1;
		sfd_time_out = SFD_timeout_MS * to_u32(4) * to_u32(synch_code_lgth) * to_u32(synch_repetition_nb);

		i = add_setting_u32(config, T_REG32, 
			BAR_CFG_DIGITAL, BAR_SFD_TIME_OUT_W0, sfd_time_out,
			i, new_config_tab_size);
	}

	return i;
}

static OSAL_u32 pp_log2(OSAL_u32 input_natural)
{
	OSAL_u32 input_natural_msb = 0;
	OSAL_u8 i;

	for(i = 0; i < 31; i++)
	{
		if ( ((input_natural >> i) & 0x00000001) == 1 )
		{
			input_natural_msb = i;
			break;
		}
	}

	return input_natural_msb;
}

static OSAL_u8 prep_conf_differential_detection(OSAL_u8 synch_code_lgth,
						OSAL_u8 synch_repetition_nb,
						OSAL_u8 demod_code_lgth, 
						struct setting * config, 
						OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	/*Hermitian product (i.e differential detection)*/
	OSAL_u8 dbb_conf_diff_det_sat_shft = 0x00;
	OSAL_u8 diff_det_code_synch_lgth = 0;
	OSAL_u8 diff_det_code_demod_lgth = 0;
	OSAL_u8 diff_det_code_synch_rep = 0;
	OSAL_s32 conf_diff_det_sat_shft = 0;

	//find the upper power of 2 of codes length
	if (diff_det_code_synch_lgth != 1)
		diff_det_code_synch_lgth = synch_code_lgth + 1;
	else
		diff_det_code_synch_lgth = synch_code_lgth;

	if (diff_det_code_demod_lgth != 1)
		diff_det_code_demod_lgth = demod_code_lgth + 1;
	else
		diff_det_code_demod_lgth = demod_code_lgth;

	//Check if repetition of synchro is a power of 2
	if (((synch_repetition_nb & 0x00000001) == 1) && (synch_repetition_nb != 1))
		diff_det_code_synch_rep = synch_repetition_nb + 1;
	else
		diff_det_code_synch_rep = synch_repetition_nb;

	conf_diff_det_sat_shft = 	pp_log2(diff_det_code_demod_lgth*diff_det_code_demod_lgth)-
					pp_log2(diff_det_code_synch_lgth*diff_det_code_synch_lgth)-
					pp_log2(diff_det_code_synch_rep);

	dbb_conf_diff_det_sat_shft = (OSAL_u8)(conf_diff_det_sat_shft & 0x000000FF);

	i = add_setting_u8(config, T_REG8,
				BAR_CFG_DIGITAL, BAR_DIFF_DET_SAT_SHFT, dbb_conf_diff_det_sat_shft,
				i, new_config_tab_size);

	return i;
}

static OSAL_u8 prep_conf_sync_code(OSAL_u8 sync_code, OSAL_u8 sync_code_ID, struct setting * config, OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	if( sync_code_ID >= sizeof(prodSection.burstSection.spreading_codes) / sizeof(struct codesWord) )
	{
		OSAL_trace(TRACE_ERROR, "BAD SPREADCODE");
		return i;
	}

	switch(sync_code)
	{
	case 7:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_SYNC_CODE_W0, 
				prodSection.burstSection.spreading_codes[sync_code_ID].code7, 1,
				i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8,
					BAR_CFG_DIGITAL, BAR_SYNC_CODE_LGTH, 7,
					i, new_config_tab_size);
		break;
	case 15:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_SYNC_CODE_W0,
					prodSection.burstSection.spreading_codes[sync_code_ID].code15, 2,
					i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8,
					BAR_CFG_DIGITAL, BAR_SYNC_CODE_LGTH, 15,
					i, new_config_tab_size);
		break;
	case 31:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_SYNC_CODE_W0,
					prodSection.burstSection.spreading_codes[sync_code_ID].code31, 4,
					i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8, BAR_CFG_DIGITAL, BAR_SYNC_CODE_LGTH, 31,
					i, new_config_tab_size);
		break;
	case 63:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_SYNC_CODE_W0,
					prodSection.burstSection.spreading_codes[sync_code_ID].code63, 8,
					i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8,
					BAR_CFG_DIGITAL, BAR_SYNC_CODE_LGTH, 63,
					i, new_config_tab_size);
		break;
	case 127:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_SYNC_CODE_W0,
					prodSection.burstSection.spreading_codes[sync_code_ID].code127, 16,
					i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8,
					BAR_CFG_DIGITAL, BAR_SYNC_CODE_LGTH, 127,
					i, new_config_tab_size);
		break;
	default:
		OSAL_trace(TRACE_ERROR, "UNKNOWN SYNC CODE  %d", sync_code);
		break;
	}

	return i;
}

static OSAL_u8 prep_conf_demod_code(OSAL_u8 demod_code,	OSAL_u8 demod_code_ID, struct setting * config, OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	if( demod_code_ID >= sizeof(prodSection.burstSection.spreading_codes) / (sizeof(struct codesWord)) )
	{
		OSAL_trace(TRACE_ERROR, "BAD SPREADCODE");
		return i;
	}

	switch(demod_code)
	{
	case 7:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_W0,
					prodSection.burstSection.spreading_codes[demod_code_ID].code7, 1,
					i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_LGTH, 7,
					i, new_config_tab_size);
		break;
	case 15:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_W0,
					prodSection.burstSection.spreading_codes[demod_code_ID].code15, 2,
					i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_LGTH, 15, i, new_config_tab_size);
		break;
	case 31:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_W0,
					prodSection.burstSection.spreading_codes[demod_code_ID].code31, 4,
					i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_LGTH, 31,
					i, new_config_tab_size);
		break;
	case 63:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_W0,
					prodSection.burstSection.spreading_codes[demod_code_ID].code63, 8,
					i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_LGTH, 63,
					i, new_config_tab_size);
		break;
	case 127:
			i = add_setting_tprod_burst(config, T_PROD_BURST,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_W0,
					prodSection.burstSection.spreading_codes[demod_code_ID].code127, 16,
					i, new_config_tab_size);

			i = add_setting_u8(config, T_REG8,
					BAR_CFG_DIGITAL, BAR_DEMO_CODE_LGTH, 127,
					i, new_config_tab_size);
		break;
	default:
		OSAL_trace(TRACE_ERROR, "UNKNOWN DEMOD CODE %d", demod_code);
		break;
	}

	return i;
}

#ifdef CONFIG_RTLS_DYNAMIC_CONFIG

static OSAL_u8 prep_conf_channel(OSAL_u8 channel, struct setting * config, OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	OSAL_u8 j = 0;

	// Check if correct BG is enable + Get channel index
	if (channel == 1 || channel == 2 || channel == 3 || channel == 4)
	{
#ifdef CONFIG_RF_BG1
		OSAL_trace(TRACE_ERROR, "BG0 DISABLED, DEACTIVATE CONFIG_RF_BG1 TO SWITCH TO %d", channel);
		return i;
#else
		channel -= 1;
#endif
	}
	else if (channel == 8 || channel == 9 || channel == 10 || channel == 11)
	{
#ifndef CONFIG_RF_BG1
		OSAL_trace(TRACE_ERROR, "BG1 DISABLED, ACTIVATE CONFIG_RF_BG1 TO SWITCH TO %d", channel);
		return i;
#else
		channel -= 4;
#endif
	}
	else
	{
		OSAL_trace(TRACE_ERROR, "BAD CHANNEL %d", channel);
		return i;
	}

	while (	prodSection.manufacturerSection.channelDependentReg[j].bank != 0x00 || 
		prodSection.manufacturerSection.channelDependentReg[j].address != 0x00 ||
		prodSection.manufacturerSection.channelDependentReg[j].val[channel] != 0x00)
	{
		i = add_setting_u8(config, T_REG8, 
			prodSection.manufacturerSection.channelDependentReg[j].bank,
			prodSection.manufacturerSection.channelDependentReg[j].address,
			prodSection.manufacturerSection.channelDependentReg[j].val[channel],
			i, new_config_tab_size);
		j++;
	}

	return i;
}

static OSAL_u8 prep_conf_tx_gain(OSAL_u8 tx_gain, struct setting * config, OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	OSAL_trace(TRACE_DEBUG, "TX GAIN NYI");
	/*TODO */
	return i;
}

static OSAL_u8 prep_conf_rx_gain(OSAL_u8 rx_gain, struct setting * config, OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	OSAL_trace(TRACE_DEBUG, "RX GAIN NYI");
	/*TODO */
	return i;
}
#endif  // defined CONFIG_RTLS_DYNAMIC_CONFIG

static OSAL_u8 prep_conf_clktracking(OSAL_u8 clktracking, OSAL_u8 channel,
				struct setting * config,
				OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	OSAL_u8 dll_drift 	= 0;
	OSAL_u8 derot_afe 	= 0;
	OSAL_u8 flo1_flo2 	= 0;
	OSAL_u8 clktrack_en 	= (clktracking & 0x01) >> 0; // clktracking contains cktrack (1st bit) and precomp (2nd bit)
	OSAL_u8 precomp_en 	= (clktracking & 0x02) >> 1;

	if (clktrack_en == 1)
	{
#ifdef CONFIG_RF_BG1
		dll_drift = (	(0 << 0) |	// setpoint_en, 	0: disable static setpoint and dynamic feedback
				(0 << 1) |	// setpoint_mod,	0: setpoint then dynamic feedback(once preamble_detected), 1: setpoint + dynamic feedback
				(1 << 2) |	// dll_shft_en,		DLL compensation shift application enable
				(0 << 3) |	// feedback_en,		1: feedback enable 
				(1 << 4) );	// feedback_dir,	0: feedback pos direction (if feedback_en = 0, set feedback_dir = 0)
#else
		dll_drift = (	(0 << 0) |	// setpoint_en
				(0 << 1) |	// setpoint_mod
				(1 << 2) |	// dll_shft_en
				(1 << 3) |	// feedback_en
				(0 << 4) );	// feedback_dir
#endif	
	}


	if (precomp_en == 1)
	{
		dll_drift |= (	(1 << 0) |	// setpoint_en
				(1 << 1) |	// setpoint_mod
				(1 << 2) |	// dll_shft_en
				(0 << 3) |	// feedback_en
				(0 << 4) );	// feedback_dir

#ifdef CONFIG_RF_BG1
		switch(channel)
		{
			case 8:
				derot_afe = 0x0B;
				flo1_flo2 = 0x12;
				break;
			case 9:
				derot_afe = 0x01;
				flo1_flo2 = 0x06;
				break;
			case 10:
				derot_afe = 0x03;
				flo1_flo2 = 0x12;
				break;
			case 11:
				derot_afe = 0x01;
				flo1_flo2 = 0x06;
				break;
			default:
				break;
		}
#else
		switch(channel)
		{
			case 1:
				derot_afe = 0x07;
				flo1_flo2 = 0x11;
				break;
			case 2:
				derot_afe = 0x05;
				flo1_flo2 = 0x05;
				break;
			case 3:
				derot_afe = 0x0F;
				flo1_flo2 = 0x11;
				break;
			case 4:
				derot_afe = 0x05;
				flo1_flo2 = 0x05;
				break;
			default:
				break;
		}
#endif
	}

	i = add_setting_u8(config, T_REG8, BAR_CFG_DIGITAL,	BAR_DLL_DRIFT,	dll_drift, 	i, new_config_tab_size);
	i = add_setting_u8(config, T_REG8, BAR_CFG_DIGITAL, 	BAR_DEROT_AFE, 	derot_afe,	i, new_config_tab_size);
	i = add_setting_u8(config, T_REG8, BAR_CFG_SYSTEM, 	BAR_FLO1_FLO2, 	flo1_flo2,	i, new_config_tab_size);

	return i;
}

#ifdef CONFIG_RTLS_DYNAMIC_CONFIG
static OSAL_u8 prep_conf_preamble_length(OSAL_u8 sync_code, OSAL_u8 sync_rep,
				struct setting * config, 
				OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	i = add_setting_u8(config, T_REG8, BAR_CFG_DIGITAL, BAR_PREAMBLE_LGTH, pp_conf_get_preamble_length(sync_code, sync_rep), i, new_config_tab_size);

	return i;
}
#endif // defined CONFIG_RTLS_DYNAMIC_CONFIG


static OSAL_u8 prep_conf_sync_threshold(struct pp_conf* cfg_ctx, OSAL_u8 channel, OSAL_u8 sync_code, OSAL_u8 sync_rep, 
					OSAL_u32 *sync_threshold,  struct setting * new_config_tab, 
					OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	OSAL_u32 new_sync_thld = 0;
	OSAL_u32 sync_thld = 0;
	OSAL_u32 rng_thld = 0;
	OSAL_u8 sync_code_ref = 0;
	OSAL_u8 sync_rep_ref = 0;

	if (*sync_threshold != 0 && cfg_ctx->enable_live_thld == OSAL_false)
		new_sync_thld = *sync_threshold;
	else
	{
		//Need to have a reference (measure) threshold to adapt the 
		//model to the device.
		//Currently, we take the threshold at default zone (code, rep)
		//and desired channel. But, if more than one measure is available,
		//we could choose another one based on a distance metric from desired 
		//(code, rep) to available measures (code, rep) for ex.
		pp_conf_get_threshold_from_zone(
			cfg_ctx,
			prodSection.userSection.defaultProtocolCfg.zone,
			channel,
			DEFAULT_RFPATH,
			&sync_thld, &rng_thld);

		//Get the default zone code and rep.
		pp_conf_get_sync_code_repetition_from_zone(
			prodSection.userSection.defaultProtocolCfg.zone,
			&sync_code_ref, &sync_rep_ref);

		//compute the threshold based on model
		new_sync_thld = threshold_from_pg_model(
			sync_code, sync_rep , sync_thld, 
			sync_code_ref, sync_rep_ref);

		//set new threshold to config
		*sync_threshold = new_sync_thld;

		PP_CFG_DEV_TRACE( "sync thld set to %d", new_sync_thld);
	}

	i = add_setting_u32(new_config_tab, T_REG32,
			    BAR_CFG_DIGITAL, BAR_NRJCOR_THLD_W0, new_sync_thld,
			    i, new_config_tab_size);
	return i;
}

static OSAL_u8 prep_conf_rng_threshold(struct pp_conf* cfg_ctx,
				       OSAL_u8 channel, OSAL_u8 rng_code, OSAL_u8 rng_rep, 
				       OSAL_u32 *rng_threshold,  struct setting * new_config_tab, 
				       OSAL_u8 i, OSAL_u8 new_config_tab_size)
{
	OSAL_u32 new_rng_thld = 0;
	OSAL_u32 sync_thld = 0;
	OSAL_u32 rng_thld = 0;
	OSAL_u8 rng_code_ref = 0;
	OSAL_u8 rng_rep_ref = 0;

	if (*rng_threshold != 0 && cfg_ctx->enable_live_thld == OSAL_false)
		new_rng_thld = *rng_threshold;
	else
	{
		//Need to have a reference (measure) threshold to adapt the 
		//model to the device.
		//Currently, we take the threshold at default zone (code, rep)
		//and desired channel. But, if more than one measure is available,
		//we could choose another one based on a distance metric from desired 
		//(code, rep) to available measures (code, rep) for ex.
		pp_conf_get_threshold_from_zone(
			cfg_ctx,
			prodSection.userSection.defaultProtocolCfg.zone,
			channel,
			DEFAULT_RFPATH,
			&sync_thld, &rng_thld);

		//Get the default zone code and rep
		pp_conf_get_sync_code_repetition_from_zone(
			prodSection.userSection.defaultProtocolCfg.zone,
			&rng_code_ref, &rng_rep_ref);

		//compute the threshold based on model
		new_rng_thld = threshold_from_pg_model(
			rng_code, rng_rep , rng_thld, 
			rng_code_ref, rng_rep_ref);

		//set new threshold to config
		*rng_threshold = new_rng_thld;
	}

	i = add_setting_u32(new_config_tab, T_REG32,
			    BAR_CFG_DIGITAL, BAR_LEAD_EDGE_THLD_W0, new_rng_thld,
			    i, new_config_tab_size);
	return i;
}

OSAL_u32 pp_conf_get_live_threshold_from_zone(struct pp_conf* cfg_ctx, OSAL_u8 channel, OSAL_u8 rf_path, OSAL_u8 zone)
{	
	OSAL_u32 thld = 1;
	OSAL_u8 code, rep, code_ref, rep_ref;

	thld = pp_conf_get_root_threshold(cfg_ctx, channel, rf_path);	

	//thld represent what the measure should be
	//in default zone. must translate back to current zone

	pp_conf_get_sync_code_repetition_from_zone(zone, &code, &rep);

	pp_conf_get_sync_code_repetition_from_zone(
		prodSection.userSection.defaultProtocolCfg.zone,
		&code_ref, &rep_ref);

	//compute the threshold based on model from default zone
	return threshold_from_pg_model(code, rep, thld, code_ref, rep_ref);
}

OSAL_void pp_conf_get_threshold_from_zone(struct pp_conf* cfg_ctx,
						OSAL_u8 zone, OSAL_u8 channel, OSAL_u8 rf_path,
						OSAL_u32* sync_threshold, OSAL_u32* rng_threshold)
{
	OSAL_u32 threshold;
	OSAL_u32 coeff = ZONE_COEFF_MULTIPLIER;

	/* TODO restore BG1 compliance as with RFPath2 mannufacturer section will change*/
	if (zone < 1 || zone > 5 || channel < 1 || channel > 4)
	{
		OSAL_trace(TRACE_ERROR, "BAD PARMS Z%d C%d ", zone, channel);
		return;
	}

	if (cfg_ctx == NULL || cfg_ctx->enable_live_thld == OSAL_false)
		pp_conf_get_prod_threshold_from_zone(zone, channel, rf_path, &threshold);
	else
		threshold = pp_conf_get_live_threshold_from_zone(cfg_ctx, channel, rf_path, zone);

	if(cfg_ctx)
	{
		coeff = cfg_ctx->threshold_coeff;
		threshold = (threshold * coeff) / ZONE_COEFF_MULTIPLIER;
	}

	if (sync_threshold)
		*sync_threshold = threshold;

	if (rng_threshold)
		*rng_threshold = threshold;
}

OSAL_void pp_conf_get_prod_threshold_from_zone(OSAL_u8 zone, OSAL_u8 channel, OSAL_u8 rf_path,
						OSAL_u32* sync_threshold)
{

	/* TODO restore BG1 compliance as with RFPath2 mannufacturer section will change*/
	if (zone < 1 || zone > 5 || channel < 1 || channel > 4)
	{
		OSAL_trace(TRACE_ERROR, "BAD PARMS Z%d C%d ", zone, channel);
		return;
	}

	// TODO add RF path1 calibration in prodSection, today duplicate Path2
	*sync_threshold = prodSection.manufacturerSection.thresholds[zone-1][channel-1]; // zone are 1-based
}

OSAL_void pp_conf_get_hw_offset(struct pp_conf* cfg_ctx, OSAL_s32* new_hw_offset, OSAL_u8 channel, OSAL_u8 rf_path, action_t act)
{
	OSAL_u8 j = 0, found_values = 0;
	OSAL_u8 pulse_shaping_min = 0xFF, tx_sp_pulse_startclk = 0x00;
	OSAL_s32 pulse_shaping_offset = 0;

	if (!new_hw_offset || !cfg_ctx)
		return;

	*new_hw_offset = prodSection.userSection.radioCfg.hardware_offsets[rf_path];

	// Apply user offset (from server) on RX and TX
	*new_hw_offset += cfg_ctx->user_hw_offset;

	// Measured delays
	switch(channel)
	{
		case 1:
			*new_hw_offset += 875;
		break;
		default:
		case 2:
			*new_hw_offset += 750;
		break;
		case 3:
			*new_hw_offset += 1125;
		break;
		case 4:
			*new_hw_offset += 1000;
		break;
	}


	// Compute delay introduced by pulse shaping
	while (	prodSection.manufacturerSection.channelDependentReg[j].bank != 0x00 || 
			prodSection.manufacturerSection.channelDependentReg[j].address != 0x00 ||
			prodSection.manufacturerSection.channelDependentReg[j].val[channel-1] != 0x00)
	{
		// We need to know TX_SP_PULSE_STARTCLK value as it told us if PP use TX_TUNE_PULSE_SWITCH_START (!=0) or not (=0)
		if(	prodSection.manufacturerSection.channelDependentReg[j].bank == BAR_CFG_ANALOG &&
					prodSection.manufacturerSection.channelDependentReg[j].address == BAR_TX_SP_PULSE_STARTCLK)
		{
			tx_sp_pulse_startclk = prodSection.manufacturerSection.channelDependentReg[j].val[channel-1];
			break;
		}
		j++;
	}

	j = 0;

	while (	prodSection.manufacturerSection.channelDependentReg[j].bank != 0x00 ||
			prodSection.manufacturerSection.channelDependentReg[j].address != 0x00 ||
			prodSection.manufacturerSection.channelDependentReg[j].val[channel-1] != 0x00)
	{
		if(	prodSection.manufacturerSection.channelDependentReg[j].bank == BAR_CFG_ANALOG &&
			prodSection.manufacturerSection.channelDependentReg[j].address == BAR_TX_SP_PULSE_START0 &&
			prodSection.manufacturerSection.channelDependentReg[j].val[channel-1] < pulse_shaping_min)
		{
			found_values++;
			pulse_shaping_min = prodSection.manufacturerSection.channelDependentReg[j].val[channel-1];
		}
		else if(prodSection.manufacturerSection.channelDependentReg[j].bank == BAR_CFG_ANALOG &&
			prodSection.manufacturerSection.channelDependentReg[j].address == BAR_TX_SP_PULSE_START1 &&
			prodSection.manufacturerSection.channelDependentReg[j].val[channel-1] < pulse_shaping_min)
		{
			found_values++;
			pulse_shaping_min = prodSection.manufacturerSection.channelDependentReg[j].val[channel-1];
		}
		else if(prodSection.manufacturerSection.channelDependentReg[j].bank == BAR_CFG_ANALOG &&
			prodSection.manufacturerSection.channelDependentReg[j].address == BAR_TX_TUNE_PULSE_SWITCH_START0 &&
			tx_sp_pulse_startclk != 0x00 &&
			prodSection.manufacturerSection.channelDependentReg[j].val[channel-1] < pulse_shaping_min)
		{
			found_values++;
			pulse_shaping_min = prodSection.manufacturerSection.channelDependentReg[j].val[channel-1];
		}
		else if(prodSection.manufacturerSection.channelDependentReg[j].bank == BAR_CFG_ANALOG &&
			prodSection.manufacturerSection.channelDependentReg[j].address == BAR_TX_TUNE_PULSE_SWITCH_START1 &&
			tx_sp_pulse_startclk != 0x00 &&
			prodSection.manufacturerSection.channelDependentReg[j].val[channel-1] < pulse_shaping_min)
		{
			found_values++;
			pulse_shaping_min = prodSection.manufacturerSection.channelDependentReg[j].val[channel-1];
		}

		if(tx_sp_pulse_startclk != 0x00 && found_values >= 4)
			break;

		if(tx_sp_pulse_startclk == 0x00 && found_values >= 2)
			break;

		j++;
	}

	// Delay = bits 6->b3 * 1ns + b2->b0 * 152ps. Bit 7 unused
	pulse_shaping_offset = -((pulse_shaping_min & 0x78) >> 3) * 1000 + (pulse_shaping_min & 0x07) * 152;


	if(act == TX)
		*new_hw_offset += pulse_shaping_offset;
}

static OSAL_void get_default_config_phy(config_phy_t * phy)
{
	phy->channel = prodSection.userSection.defaultProtocolCfg.channel;
	pp_conf_get_sync_code_repetition_from_zone(
				prodSection.userSection.defaultProtocolCfg.zone,
				&phy->sync_code, &phy->sync_rep);

	pp_conf_get_demod_code_repetition_from_zone(
				prodSection.userSection.defaultProtocolCfg.zone,
				&phy->demod_code, &phy->demo_rep);

	phy->clock_track = prodSection.userSection.radioCfg.clock_track;
	phy->framer_config = prodSection.userSection.radioCfg.framer_config;
	phy->rf_path = prodSection.userSection.radioCfg.rx_path;
	pp_conf_get_threshold_from_zone(
				NULL,
				prodSection.userSection.defaultProtocolCfg.zone,
				prodSection.userSection.defaultProtocolCfg.channel,
				DEFAULT_RFPATH,
				&phy->sync_threshold, &phy->rng_threshold);

	phy->tx_gain = prodSection.userSection.radioCfg.tx_gain;
	phy->rx_gain = prodSection.userSection.radioCfg.rx_gain;
	phy->power_mode = PM_3;
	phy->act = WAIT;
	phy->preamble_timeout = DEFAULT_PREAMBLE_TIMEOUT;
	phy->sync_code_ID = prodSection.userSection.radioCfg.sync_code_ID;
	phy->demod_code_ID = prodSection.userSection.radioCfg.demod_code_ID;
}

OSAL_u8 set_config_phy(struct pp_conf * cfg_ctx,
			 config_phy_t * new_config_phy,
			 struct setting * new_config_tab, 
			 OSAL_u8 new_config_tab_size,
			 OSAL_bool_t force_rfpath)
{
	OSAL_u8 i = 0;
	config_phy_t* current_config_phy = &cfg_ctx->phy;
	pm_state_t current_pm_mode = (pm_state_t)current_config_phy->power_mode;

	if (current_config_phy == NULL)
		return -1;

	if (new_config_phy == NULL)
		get_default_config_phy(new_config_phy);

	/* Check and add 1v2pll boost setting*/
	i = prep_conf_1v2pll_boost(cfg_ctx,
				new_config_phy->channel, new_config_tab,
				 i, new_config_tab_size);	

	/*clearly sub-optimal but the right case is RF path managed by pm*/
	if(force_rfpath == OSAL_true)
	{
		if(new_config_phy->act == RX)
		{	
			i = add_setting_u8(new_config_tab, T_REG8,
					BAR_CFG_ANALOG, BAR_RXFE_SP_TXRXDEP, 0x04,
					i, new_config_tab_size); //Antenna dependent: 0x01: Rx1, 0x04: Rx2

			i = add_setting_u8(new_config_tab, T_REG8,
					BAR_CFG_SYSTEM, BAR_DIO_TA_VAL, 0x88,
					i, new_config_tab_size);
		}
		else if (new_config_phy->act == TX)
		{
			i = add_setting_u8(new_config_tab, T_REG8, 
					BAR_CFG_ANALOG,BAR_RXFE_SP_TXRXDEP, 0x02,
					i, new_config_tab_size);

			i = add_setting_u8(new_config_tab, T_REG8,
					BAR_CFG_SYSTEM, BAR_DIO_TA_VAL, 0x82,
					i, new_config_tab_size);
		}
	}

	/* Store difference between current_config_phy & new_config_phy */

	// sync_threshold
	if(current_config_phy->sync_threshold != new_config_phy->sync_threshold || new_config_phy->sync_threshold == 0 || cfg_ctx->enable_live_thld)
	{
		i = prep_conf_sync_threshold(cfg_ctx,
					new_config_phy->channel, new_config_phy->sync_code, 
					new_config_phy->sync_rep, &new_config_phy->sync_threshold,
					new_config_tab, i, new_config_tab_size);
	}

	// rng_threshold
	if(current_config_phy->rng_threshold != new_config_phy->rng_threshold || new_config_phy->rng_threshold == 0  || cfg_ctx->enable_live_thld)
	{
		i = prep_conf_rng_threshold(cfg_ctx,
					new_config_phy->channel, new_config_phy->sync_code,
					new_config_phy->sync_rep, &new_config_phy->rng_threshold,
					new_config_tab, i, new_config_tab_size);
	}

	// sync_rep
	if(current_config_phy->sync_rep != new_config_phy->sync_rep)
	{
		i = add_setting_u8(new_config_tab, T_REG8,
					BAR_CFG_DIGITAL, BAR_SYNC_SYMB_REP, new_config_phy->sync_rep,
					i, new_config_tab_size);
	}

	// demod_rep
	if(current_config_phy->demo_rep != new_config_phy->demo_rep)
	{
		i = add_setting_u8(new_config_tab, T_REG8,
					BAR_CFG_DIGITAL, BAR_DEMO_SYMB_REP, new_config_phy->demo_rep,
					i, new_config_tab_size);
	}

	// Preamble & SFD timeouts
	if(current_config_phy->sync_code != new_config_phy->sync_code || 
	   current_config_phy->sync_rep != new_config_phy->sync_rep ||
	   current_config_phy->preamble_timeout != new_config_phy->preamble_timeout)
	{
		i = prep_conf_timeout(	new_config_phy->sync_code, new_config_phy->sync_rep,
					new_config_phy->preamble_timeout, 
					new_config_tab,
					i, new_config_tab_size);
	}

	// differential_detection
	if(current_config_phy->sync_code != new_config_phy->sync_code || 
	   current_config_phy->sync_rep != new_config_phy->sync_rep || 
	   current_config_phy->demod_code != new_config_phy->demod_code)
	{
		i = prep_conf_differential_detection(new_config_phy->sync_code, new_config_phy->sync_rep,
					new_config_phy->demod_code,
					new_config_tab,
					i, new_config_tab_size);
	}

	// clktracking
	if(current_config_phy->clock_track != new_config_phy->clock_track || 
	   current_config_phy->channel != new_config_phy->channel)
	{
		i = prep_conf_clktracking(new_config_phy->clock_track, new_config_phy->channel, 
					new_config_tab,
					i, new_config_tab_size);
	}


#ifdef CONFIG_RTLS_DYNAMIC_CONFIG

	// Preamble length
	if ( pp_conf_get_preamble_length(current_config_phy->sync_code, current_config_phy->sync_rep) != pp_conf_get_preamble_length(new_config_phy->sync_code, new_config_phy->sync_rep) )
	{
		i = prep_conf_preamble_length(	new_config_phy->sync_code, new_config_phy->sync_rep,
						new_config_tab,
						i, new_config_tab_size);
	}
#endif //defined CONFIG_RTLS_DYNAMIC_CONFIG

	// sync_code 
	if(current_config_phy->sync_code != new_config_phy->sync_code || current_config_phy->sync_code_ID != new_config_phy->sync_code_ID)
	{
		i = prep_conf_sync_code(new_config_phy->sync_code, new_config_phy->sync_code_ID,
					new_config_tab,
					i, new_config_tab_size);
	}

	// demod_code
	if(current_config_phy->demod_code != new_config_phy->demod_code || current_config_phy->demod_code_ID != new_config_phy->demod_code_ID)
	{
		i = prep_conf_demod_code(new_config_phy->demod_code, new_config_phy->demod_code_ID,
					new_config_tab,
					i, new_config_tab_size);
	}

#ifdef CONFIG_RTLS_DYNAMIC_CONFIG

	// channel
	if (current_config_phy->channel != new_config_phy->channel)
		i = prep_conf_channel(new_config_phy->channel,
				new_config_tab,
				i, new_config_tab_size);

	//tx gain
	if(current_config_phy->tx_gain != new_config_phy->tx_gain)
	{
		i = prep_conf_tx_gain(new_config_phy->tx_gain,new_config_tab,i, new_config_tab_size);
	}

	//rx gain
	if(current_config_phy->rx_gain != new_config_phy->rx_gain)
	{
		i = prep_conf_rx_gain(new_config_phy->rx_gain,new_config_tab,i, new_config_tab_size);
	}
#endif // defined CONFIG_RTLS_DYNAMIC_CONFIG

	// framer_config
	if(current_config_phy->framer_config != new_config_phy->framer_config)
	{
		i = add_setting_u8(new_config_tab, T_REG8,
					BAR_CFG_DIGITAL, BAR_FRAMING_CFG, new_config_phy->framer_config,
					i, new_config_tab_size);
	}



	// Setting tab is too short: cancel all modifications
	if (i > new_config_tab_size - 1)
	{
		OSAL_trace(TRACE_WARN, "CFG error");
		add_setting_u8(new_config_tab, T_STOP, 0, 0, 0, 0, new_config_tab_size);

		return -1;
	}
	else
	{
		OSAL_memcpy(current_config_phy, new_config_phy, sizeof(config_phy_t));
		current_config_phy->power_mode = current_pm_mode;
		add_setting_u8(new_config_tab, T_STOP, 0, 0, 0, i, new_config_tab_size);

		return 0;
	}
}

#ifdef CONFIG_RTLS_PROTOCOL
OSAL_u8 set_config_phy_from_zone(struct pp_conf * cfg_ctx,
					seq_command_t * new_config,
					struct setting * new_config_tab,
					OSAL_u8 new_config_tab_size,
					OSAL_bool_t force_rfpath,
					OSAL_bool_t pm2_wakeup_glitch)
{
	config_phy_t new_config_phy;
	config_phy_t* current_config_phy = &cfg_ctx->phy;

	if (current_config_phy == NULL)
		return -1;

	// FIXME: in following lines, we don't set all fields of the config_phy,
	// thus some fields may be uninitialized and thus differ from current_config_pÄ¥y
	// we fix that by doing a memcpy, but it's wasting resources
	OSAL_memcpy(&new_config_phy, current_config_phy, sizeof(config_phy_t));

	/* First step: convert new_config (seq_command_t) in new_config_phy (config_phy) */
	// new_config is NULL in case of "WAIT",
	// FIXME: handling this very special case here may not be a good idea
	if (new_config == NULL)
		get_default_config_phy(&new_config_phy);

	else
	{
		pp_conf_get_sync_code_repetition_from_zone(
			new_config->time.zone, &new_config_phy.sync_code, &new_config_phy.sync_rep);

		pp_conf_get_demod_code_repetition_from_zone(
			new_config->time.zone, &new_config_phy.demod_code, &new_config_phy.demo_rep);

		pp_conf_get_threshold_from_zone(
			cfg_ctx,
			new_config->time.zone, 
			new_config->channel,
			DEFAULT_RFPATH,
			&new_config_phy.sync_threshold, &new_config_phy.rng_threshold);

		new_config_phy.channel = new_config->channel;

		new_config_phy.framer_config = new_config->framer_config;

		new_config_phy.act = new_config->act;

		new_config_phy.rf_path = new_config->rf_path;
		new_config_phy.preamble_timeout = 0;

		if(new_config->act == RX && new_config->preamble_timeout == 0 && new_config->ts_offset != 0)
		{
			new_config_phy.preamble_timeout = TS_TO_EP(new_config->ts_offset);
			if(pm2_wakeup_glitch)
				new_config_phy.preamble_timeout += TS_TO_EP(3);
			new_config_phy.preamble_timeout |= (1UL<<31);
		}
		else if (new_config->act == RX)
			new_config_phy.preamble_timeout = new_config->preamble_timeout;
	}


	/* Second step: create a setting tab from the difference between current_config_phy & new_config_phy */
	return set_config_phy(cfg_ctx, &new_config_phy, new_config_tab, new_config_tab_size, force_rfpath);
}
#endif //#ifdef CONFIG_RTLS_PROTOCOL

struct setting OSAL_PROD_SECT * prep_config_init(config_phy_t * new_conf)
{
	OSAL_get_prodSection(&prodSection);
	if (new_conf == NULL)
	{
		OSAL_trace(TRACE_ERROR, "prep_config_init(): NO config_phy SPECIFIED");
		return NULL;
	}

	if (	pp_conf_crc16((OSAL_u8 *)&prodSection.userSection, sizeof(struct userSectionStruct)-2) != prodSection.userSection.crc ||
		pp_conf_crc16((OSAL_u8 *)&prodSection.burstSection, sizeof(struct burstSectionStruct)-2) != prodSection.burstSection.crc ||
		pp_conf_crc16((OSAL_u8 *)&prodSection.manufacturerSection, sizeof(struct manufacturerSectionStruct)-2) != prodSection.manufacturerSection.crc)
	{
		OSAL_trace(TRACE_ERROR, "CRC error in PS, update PS");
		return NULL;
	}

	if(prodSection.userSection.generalCfg.prodSection_version_number > PRODSECTION_VERSION_NUMBER)
	{
		OSAL_trace(TRACE_ERROR, "PSv%X > FWv%X: update FW",
			prodSection.userSection.generalCfg.prodSection_version_number,
			PRODSECTION_VERSION_NUMBER);
		return NULL;
	}
	else if(prodSection.userSection.generalCfg.prodSection_version_number < PRODSECTION_VERSION_NUMBER)
	{
		OSAL_trace(TRACE_ERROR, "FWv%X > PSv%X, update PS",
			PRODSECTION_VERSION_NUMBER,
			prodSection.userSection.generalCfg.prodSection_version_number);
		return NULL;
	}

#ifdef BYPASS_PRODFW_MSQ
	OSAL_memset(prodSection.burstSection.msq_pmem,0xFF,MSQ_PMEM_SIZE);
	OSAL_memcpy(prodSection.burstSection.msq_pmem,PMEM_firmware, sizeof(PMEM_firmware)/sizeof(PMEM_firmware[0]));
#endif

	get_default_config_phy(new_conf);

	return init_setting_tab;
}

struct setting OSAL_PROD_SECT * prep_config_exit()
{
	OSAL_get_prodSection(&prodSection);

	return exit_setting_tab;
}

OSAL_u16 get_fw_msq(OSAL_u8 OSAL_PROD_SECT ** fw_addr)
{
	*fw_addr = prodSection.burstSection.msq_pmem;

	return MSQ_PMEM_SIZE;
}

#ifdef CONFIG_RTLS_FULL_LOC
OSAL_bool_t set_prodSection(prodSectionStruct_t *prodSectionTmp)
{
	OSAL_memcpy( &prodSection, prodSectionTmp, sizeof(prodSectionStruct_t));
	return OSAL_true;
}
#endif


#ifdef CONFIG_RTLS_PROTOCOL
OSAL_void prep_config_32k(OSAL_u8 *tab32k, 							// output
		sequencer_time_t* st, OSAL_u32 ts_delay, OSAL_bool_t force_ts_delay, 	// input (time)
		action_t act, OSAL_bool_t lclk_cal, OSAL_bool_t auto_switch, OSAL_bool_t rst_cpt, OSAL_bool_t force_shadow) // input (actions)
{
	OSAL_u32 tmp_it, tmp_ts, tmp_ntoa, tmp_ts_delay;

	if(ts_delay>=get_nbTS())
	{
		OSAL_trace(TRACE_ERROR, "CANT SET TS-DELAY >= %d", get_nbTS());
		ts_delay = 0;
	}

	// Hardware timer/sequencer can't fire TS-DELAY if TIMER_32K_DELAY > TIMER_32K_TS
	// Thus, put it at previous IT if upper layer allows us to do so
	// Because timer/sequencer can't use the latest 3 TS, we put it at nbTS()-4
	if(st->ts < ts_delay && force_ts_delay)
	{
		st->it = (get_PHS() + st->it - 1) % get_PHS();
		st->ts = get_nbTS() - 4;
		st->ntoa = 0;
	}

	/*PPV3 starts counting M from 1 */
	tmp_it = st->it + 1;
	tmp_ts = st->ts;
	tmp_ntoa = st->ntoa;
	tmp_ts_delay = ts_delay;
	/**/

	tab32k[0] = tmp_it;			// BAR_32K_M
	tab32k[1] = tmp_ts & 0xff;		// BAR_32K_TS (lsb)
	tab32k[2] = (tmp_ts >> 8) & 0xff;	// BAR_32K_TS (msb)
	tab32k[3] = tmp_ts_delay;		// BAR_32K_DELAY (lsb)
	tab32k[4] = (tmp_ts_delay >> 8) & 0xff;	// BAR_32K_DELAY (msb)
	tab32k[5] = tmp_ntoa & 0xff;		// BAR_32K_NTOA (lsb)
	tab32k[6] = (tmp_ntoa >> 8) & 0xff;	// BAR_32K_NTOA
	tab32k[7] = (tmp_ntoa >> 16) & 0xff;	// BAR_32K_NTOA
	tab32k[8] = (tmp_ntoa >> 24) & 0xff;	// BAR_32K_NTOA (msb)
	tab32k[9] = 0x00;			// BAR_32K_ACTION
	tab32k[10] = 0x00;			// BAR_32K_SHADOW_MSB
	tab32k[11] = 0x00;			// BAR_32K_SHADOW_LSB
	tab32k[12] = 0x00;			// BAR_32K_SHADOW_POP

	switch(act)
	{
	case TX:
		PP_CFG_DEV_TRACE( "[act]tx");
		tab32k[9] = 0x02;
		break;
	case RX:
		PP_CFG_DEV_TRACE( "[act]rx");
		tab32k[9] = 0x01;
		break;
	case WAIT:
		PP_CFG_DEV_TRACE( "[act]wait");
		break;
	case NO_ACTION:
		PP_CFG_DEV_TRACE( "[act]no action");
		break;
	default:
		break;
	}

	if (lclk_cal == OSAL_true)
		tab32k[9] |= 0x04; // start LCLK calibration

	if (rst_cpt == OSAL_true)
		tab32k[9] |= 0x40; // enable clear ranging timer

	if (auto_switch == OSAL_true)
		tab32k[9] |= 0x80; // Timer auto-switch

	if (force_shadow == OSAL_true)
		tab32k[12] = 0x01;
}
#endif //#ifdef CONFIG_RTLS_PROTOCOL

struct setting OSAL_PROD_SECT * prep_config_pm(struct pp_conf * cfg_ctx, pm_state_t new_pm)
{
	struct pm_switch OSAL_PROD_SECT * edge = NULL;

	if (new_pm >= POWER_MODE_NB)
	{
		OSAL_trace(TRACE_ERROR, "BAD PM %d", new_pm);
		return NULL;
	}

	edge = PowerModeSwitch[cfg_ctx->phy.power_mode][new_pm];

	if(edge == NULL)
	{
		OSAL_trace(TRACE_ERROR,
			"NO PM SEQ TO SWITCH FROM %d to %d",
				cfg_ctx->phy.power_mode, new_pm);

		return NULL;
	}

	//HOST HANDLE PM
	if(cfg_ctx->pm_msq == OSAL_false)
	{
		cfg_ctx->phy.power_mode = edge->Dest;
		return edge->Setting;
	}
	else//MSQ HANDLE PM
	{
		if(edge->Setting_MSQ[0].Type == T_STOP)
		{
			OSAL_trace(TRACE_ERROR,
					"(PM MSQ)NO PM SEQ TO SWITCH FROM %d to %d",
					cfg_ctx->phy.power_mode, new_pm);

			return NULL;
		}
		else
		{
			cfg_ctx->phy.power_mode = edge->Dest;
			return edge->Setting_MSQ;
		}
	}
}

struct setting get_setting_mux_rst(void)
{
	struct setting setting_mux = {
				T_PROD_BURST,
				BAR_CFG_MUX_REG,
				BAR_MUX_REG_W00,
                0,
				{.tprodburst = { prodSection.burstSection.mux_reg, MUX_REG_SIZE }}
	};

	return setting_mux;
}

pm_state_t get_config_pm(pm_state_t curr_pm, pm_state_t dest_pm,struct setting OSAL_PROD_SECT ** Setting)
{
	struct pm_switch OSAL_PROD_SECT * edge = PowerModeSwitch[curr_pm][dest_pm];

	if(edge == NULL)
	{
		OSAL_trace(TRACE_ERROR, "GET:NO PM SEQ TO SWITCH FROM %d to %d", curr_pm, dest_pm);
		return POWER_MODE_NB;
	}
	
	*Setting = edge->Setting;
	return edge->Dest;
}


// There are 2 functions set_new_pm(), here and in pinpointer_fs.c
// They should be aligned EXCEPTED the delay between PM2 and PM0 (see below)
OSAL_void set_new_pm(pm_state_t new_pm, struct pp_conf* cfg_ctx, OSAL_void* hw_priv_data)
{
	struct setting OSAL_PROD_SECT * pm_tab = NULL;
	OSAL_u8 backupPmMsq = 0;
	OSAL_bool_t previous_msq = cfg_ctx->pm_msq;

	if(cfg_ctx->phy.power_mode == new_pm) return;

	if((new_pm > PM_2)&&(cfg_ctx->phy.power_mode == PM_3))
	{
		set_new_pm(PM_2, cfg_ctx, hw_priv_data);
		set_new_pm(new_pm, cfg_ctx, hw_priv_data);
		return;
	}
	else if((new_pm == PM_3)&&(cfg_ctx->phy.power_mode > PM_2))
	{
		set_new_pm(PM_2, cfg_ctx, hw_priv_data);
		set_new_pm(PM_3, cfg_ctx, hw_priv_data);
		return;
	}

	while (cfg_ctx->phy.power_mode != new_pm)
	{
		if(
			((cfg_ctx->phy.power_mode == PM_2)&&
			(new_pm == PM_3))||
			((cfg_ctx->phy.power_mode == PM_3)&&
			(new_pm == PM_2))
		)
		{
			backupPmMsq = 1;
			previous_msq = cfg_ctx->pm_msq;
			cfg_ctx->pm_msq = OSAL_false;
		}
		else backupPmMsq=0;

		// Enable DCO in PM0 before MSQ begin his hob
		if (cfg_ctx->phy.power_mode == PM_2 && new_pm > PM_2)
			msq_DCO_start(hw_priv_data, cfg_ctx->pm_msq);

		// If pm_msq=0 2ms delay after PM0->STDBY will be skipped so we have to do it there
		if( !cfg_ctx->pm_msq && cfg_ctx->phy.power_mode == PM_0 && new_pm > PM_0 )
			OSAL_msleep(2);

		pm_tab = prep_config_pm(cfg_ctx, new_pm);

		if ( pm_tab != NULL )
		{
			if (cfg_ctx->pm_msq == OSAL_true)
				msq_seq_write(hw_priv_data, pm_tab);
			else
				pp_hw_prod_seq_write(hw_priv_data, pm_tab);
		}
		else
		{
			OSAL_trace(TRACE_ERROR, "ERROR SIWTCHING PM");
			return;
		}

		//wait MSQ not BUSY to allow action next action
		if (cfg_ctx->pm_msq == OSAL_true)
			while(msq_get_state(hw_priv_data) == BUSY);

		// Disable DCO in PM2 once msq has finished his job
		if (cfg_ctx->phy.power_mode == PM_2)
			msq_DCO_stop(hw_priv_data, OSAL_false);
		if(backupPmMsq)
		{
			cfg_ctx->pm_msq = previous_msq;
			backupPmMsq = 0;
		}

	}
}

config_phy_t* get_current_config_phy(struct pp_conf * cfg_ctx)
{
	return &cfg_ctx->phy;
}

#define SIZE_LONG_ADDRESS_IN_PROD_SECTION 16
OSAL_void get_longAddress(OSAL_u8 longAddress[UWB_MAC_LEN])
{
	OSAL_u8 i;
	for (i = 0; i < UWB_MAC_LEN && i < SIZE_LONG_ADDRESS_IN_PROD_SECTION; i++)
		longAddress[i] = prodSection.manufacturerSection.longAddress[i+SIZE_LONG_ADDRESS_IN_PROD_SECTION-UWB_MAC_LEN];
}

OSAL_u8 get_channelDefault(OSAL_void)
{
	return prodSection.userSection.defaultProtocolCfg.channel;
}

OSAL_u8 get_zoneDefault(OSAL_void)
{
	return prodSection.userSection.defaultProtocolCfg.zone;
}

OSAL_u8 get_framerCfgDefault(OSAL_void)
{
	return prodSection.userSection.radioCfg.framer_config;
}

OSAL_u32 get_dacDefault(OSAL_void)
{
	return prodSection.userSection.radioCfg.dacDefault;
}

OSAL_u32 pp_conf_get_root_threshold(struct pp_conf* cfg_ctx, OSAL_u8 channel, OSAL_u8 rf_path)
{
	if( rf_path >= 0 && rf_path < RFPATH_NB && channel > 0 && channel <= RT_MAX_CHANNEL )
		return cfg_ctx->root_thld[rf_path][channel-1];
	else
	{
		OSAL_trace(TRACE_ERROR, "GetRt bad parm %d %d",rf_path,channel);
		return 0;
	}
}

OSAL_void pp_conf_set_root_threshold(struct pp_conf* cfg_ctx, OSAL_u8 channel, OSAL_u8 rf_path, OSAL_u32 thld)
{
	if( rf_path >= 0 && rf_path < RFPATH_NB && channel > 0 && channel <= RT_MAX_CHANNEL )
		cfg_ctx->root_thld[rf_path][channel-1] = thld;
	else
		OSAL_trace(TRACE_ERROR, "SetRt bad parm");
}

OSAL_error_t pp_conf_set_threshold_coeff(struct pp_conf* cfg_ctx, OSAL_u16 new_thld_coeff)
{
	if(!cfg_ctx)
		return OSAL_ERROR;

	if(new_thld_coeff < 100)
	{
		OSAL_trace(TRACE_ERROR, "Threshold coefficient (%d) too low (<100)", new_thld_coeff);
		return OSAL_ERROR;
	}

	cfg_ctx->threshold_coeff = new_thld_coeff;
	return OSAL_OK;
}

OSAL_error_t pp_conf_set_hw_offset(struct pp_conf* cfg_ctx, OSAL_s16 hw_offset)
{
	if(!cfg_ctx)
		return OSAL_ERROR;

	cfg_ctx->user_hw_offset = hw_offset;

	return OSAL_OK;
}

OSAL_s8 pp_conf_get_temperature(struct pp_conf* cfg_ctx)
{
	return cfg_ctx->temperature;
}

OSAL_void pp_conf_set_temperature(struct pp_conf* cfg_ctx, OSAL_s8 temperature)
{
	cfg_ctx->temperature = temperature;
}

#ifdef CONFIG_RTLS_PROTOCOL

/* Temporary hack to allow modifying nbTS /PHS */
static OSAL_u32 protocol_superframe_size = 0;
static OSAL_u32 protocol_hyperframe_size = 0;

OSAL_void set_nbTS(OSAL_u32 new_nbTS)
{
	protocol_superframe_size = new_nbTS;
}

OSAL_u32 get_nbTS(OSAL_void)
{
	if (protocol_superframe_size == 0)
		protocol_superframe_size = to_u32(prodSection.userSection.defaultProtocolCfg.default_sfi.psn * 
							prodSection.userSection.defaultProtocolCfg.default_sfi.pss);
	return protocol_superframe_size;
}

OSAL_void set_PHS(OSAL_u32 new_PHS)
{
	protocol_hyperframe_size = new_PHS;
}

OSAL_u32 get_PHS(OSAL_void)
{
	if (protocol_hyperframe_size == 0)
		protocol_hyperframe_size = to_u32(prodSection.userSection.defaultProtocolCfg.default_sfi.phs);
	return protocol_hyperframe_size;
}

OSAL_u32 get_maxBeaconListenRate(OSAL_void)
{
	/* MODULE ONLY : return true maxListenBeaconRate set with module tool */
	return prodSection.userSection.defaultProtocolCfg.maxBeaconListenRate;
}

OSAL_u8 get_hwCaps(OSAL_void)
{
	return prodSection.manufacturerSection.hardwareCapabilities;
}

OSAL_void get_device_agility(struct pp_conf * cfg_ctx, dev_rate_types* lr)
{
	if (!lr)
		return;

	// devices capabilities can be enabled or not for a session
	if (cfg_ctx->caps_enabled & DEVCAPS_AGILE)
		*lr = prodSection.manufacturerSection.hardwareCapabilities & 0x01 ? LOCRATE_AGILE : LOCRATE_STATIC;
	else
		*lr = LOCRATE_STATIC;
}

OSAL_u16 get_sfi_PHS()
{
	if (prodSection.userSection.defaultProtocolCfg.default_sfi.phs != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.phs;
	else
		return DEFAULT_SFI_PHS;
}

OSAL_u16 get_sfi_PSN()
{
	if (prodSection.userSection.defaultProtocolCfg.default_sfi.psn != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.psn;
	else
		return DEFAULT_SFI_PSN;
}

OSAL_u16 get_sfi_PSS()
{
	if (prodSection.userSection.defaultProtocolCfg.default_sfi.pss != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.pss;
	else
		return DEFAULT_SFI_PSS;
}

OSAL_u16 get_sfi_nbRelay()
{
	if (prodSection.userSection.defaultProtocolCfg.default_sfi.nbRelay != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.nbRelay;
	else
		return DEFAULT_NB_RELAY;
}

OSAL_u16 get_sfi_firstRangingSlot()
{
	if(prodSection.userSection.defaultProtocolCfg.default_sfi.firstRangingSlot != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.firstRangingSlot;
	else
		return DEFAULT_SFI_SLOT_RANGING;
}

OSAL_u16 get_sfi_rangingSlotNb()
{
	OSAL_u16 rangingSlotNb = 0;
	if(prodSection.userSection.defaultProtocolCfg.default_sfi.firstRangingSlot != 0 && prodSection.userSection.defaultProtocolCfg.default_sfi.psn != 0)
	{
		rangingSlotNb = prodSection.userSection.defaultProtocolCfg.default_sfi.psn - 
				prodSection.userSection.defaultProtocolCfg.default_sfi.firstRangingSlot;
		if(prodSection.userSection.defaultProtocolCfg.default_sfi.rachSlot >= 0)
			rangingSlotNb -= 1;

		return rangingSlotNb;
	}
	else
		return DEFAULT_SFI_PSN - DEFAULT_SFI_SLOT_RANGING - 1;
}

OSAL_s16 get_sfi_rdvSlot()
{
	if (prodSection.userSection.defaultProtocolCfg.default_sfi.rachSlot != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.rachSlot;
	else
		return DEFAULT_SFI_RDV_SLOT;
}

OSAL_u8 get_sfi_zoneBeacon()
{
	if (prodSection.userSection.defaultProtocolCfg.default_sfi.zoneBeacon != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.zoneBeacon;
	else
		return DEFAULT_SFI_ZONE_BEACON;
}

OSAL_u8 get_sfi_zoneRanging()
{
	if (prodSection.userSection.defaultProtocolCfg.default_sfi.zoneRanging != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.zoneRanging;
	else
		return DEFAULT_SFI_ZONE_RANGING;
}

OSAL_u8 get_sfi_zoneRdv()
{
	if (prodSection.userSection.defaultProtocolCfg.default_sfi.zoneRach != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.zoneRach;
	else
		return DEFAULT_SFI_ZONE_RDV;
}

OSAL_u8 get_sfi_channel()
{
	if (prodSection.userSection.defaultProtocolCfg.default_sfi.channel != 0)
		return prodSection.userSection.defaultProtocolCfg.default_sfi.channel;
	else
		return DEFAULT_SFI_CHANNEL;
}

OSAL_u8 get_sfi_sync_code_ID()
{
	return prodSection.userSection.defaultProtocolCfg.default_sfi.sync_code_ID;
}

OSAL_u8 get_sfi_demod_code_ID()
{
	return prodSection.userSection.defaultProtocolCfg.default_sfi.demod_code_ID;
}

OSAL_u16 get_prot_UWBCaps()
{
	return prodSection.userSection.defaultProtocolCfg.UWBCapabilities;
}

OSAL_u8 get_prot_autostart()
{
	return prodSection.userSection.defaultProtocolCfg.autostart;
}

OSAL_u8 get_prot_pm2()
{
	return prodSection.userSection.defaultProtocolCfg.pm2;
}

OSAL_bool_t pp_conf_is_scan_rf_enable(struct pp_conf* cfg_ctx)
{
	if (!cfg_ctx)
		return OSAL_false;
	return cfg_ctx->caps_enabled & DEVCAPS_ROOT ? OSAL_false : OSAL_true; // scan_rf is enabled if device is *NOT* root
}

OSAL_bool_t pp_conf_is_uwb_slave_enable(struct pp_conf* cfg_ctx)
{
	if (!cfg_ctx)
		return OSAL_false;
	return cfg_ctx->caps_enabled & DEVCAPS_UWB_SLAVE ? OSAL_true : OSAL_false;
}

OSAL_bool_t pp_conf_is_root_enable(struct pp_conf* cfg_ctx)
{
	if (!cfg_ctx)
		return OSAL_false;
	return cfg_ctx->caps_enabled & DEVCAPS_ROOT ? OSAL_true : OSAL_false;
}

asset_tracking_config_e get_assetTracking_config(struct pp_conf* cfg_ctx)
{
	if (!cfg_ctx)
		return OSAL_false;

	return prodSection.userSection.defaultProtocolCfg.assetTrackingCfg.assetTrackingType;
}

OSAL_u32 get_assetTracking_subLocRate(struct pp_conf* cfg_ctx)
{
	if (!cfg_ctx)
		return OSAL_false;

	return prodSection.userSection.defaultProtocolCfg.assetTrackingCfg.subLocRate;
}

OSAL_u16 get_assetTracking_rangingNb(struct pp_conf* cfg_ctx)
{
	if (!cfg_ctx)
		return OSAL_false;

	return prodSection.userSection.defaultProtocolCfg.assetTrackingCfg.rangingNb;
}

#ifdef CONFIG_RTLS_DYNAMIC_CONFIG

OSAL_bool_t pp_conf_is_uwb_master_enable(struct pp_conf* cfg_ctx)
{
	if (!cfg_ctx)
		return OSAL_false;
	return cfg_ctx->caps_enabled & DEVCAPS_UWB_MASTER ? OSAL_true : OSAL_false;
}

#endif //#ifdef CONFIG_RTLS_DYNAMIC_CONFIG
#endif //#ifdef CONFIG_RTLS_PROTOCOL

