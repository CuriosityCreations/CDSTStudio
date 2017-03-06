/**
 * @file module_api.c
 * @brief api for module
 * @author ore@bespoon.com
 * @date 26/08/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */
#include "module.h"
#include <live_threshold_adjustment_api.h>
#include <memPool.h>
#include <afc_filter_api.h>
#include <utils_api.h>
#include <config_api.h>

OSAL_u8 radio_is_init_busy()
{
	return module_is_busy(MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_INIT_BUSY_MSK);
}

OSAL_void radio_set_init_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_INIT_BUSY_MSK);

}
OSAL_u8 radio_msq_init(OSAL_bool_t previous_msq)
{
	OSAL_u16 fw_size;
	OSAL_u8 OSAL_PROD_SECT * fw = NULL;
	
	ctx.cfg_ctx->pm_msq = OSAL_false; //enable old way to switch PM	
	msq_DCO_start(&ctx, OSAL_false);
	ctx.cfg_ctx->pm_msq = previous_msq; //restore previous way to switch PM
	fw_size = get_fw_msq(&fw);

	if((fw != NULL) && (fw_size != 0))
	{
		// DISABLE MSQ
		msq_stop(&ctx);
		// DMEM and PMEM init
		msq_mem_init(&ctx);
		// LOAD FIRMWARE AND VECTOR
		msq_ld_fw(&ctx, fw, fw_size);
		// LOAD SEQUENCES
		msq_ld_seq(&ctx);
		// INIT MSQ
		msq_init(&ctx);
		// ENABLE MSQ
		msq_start(&ctx);
		// CONFIG MUX
		msq_mux_cfg(&ctx, ctx.cfg_ctx->pm_msq, ctx.cfg_ctx->enable_pm);
		//TEST RUN MSQ
		if(!msq_run_test(&ctx))
		{
			// Save the MSQ as quick as we can for asset tracking
			pp_hw_reg_write(&ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W41, 0xE8); // give host access to BAR_CLK_SRC_EN_W1
			msq_start(&ctx);
		}
		// STOP DCO
		msq_DCO_stop(&ctx, OSAL_false);
	}
	else
	{
		OSAL_trace(TRACE_ERROR, "!MSQ %s",(fw == NULL)?"POINTER":"SIZE");
		return 1;
	}
	return 0;
	
}

static OSAL_void radio_ext32K_calibrate()
{
	//TODO: do this for all module, but with different values to have a 10ms max calibration
	//and set captune accordingly, to have a similar 32K for all module
	//code almost there, theoric hclk accu to be tuned, and timin/lasts value to be tuned
	OSAL_u64 hclk_accu_32k_theoric;
	OSAL_u16 confTIM=0x001B;
	OSAL_u16 confLAST=0x0100;
	OSAL_u16 bckTIM,bckLAST;
	OSAL_u32 hclk_accu=0;
	OSAL_u8 intr_vect[4]={0,0,0,0}; // store INTR_VECT0, INTR_VECT2
	OSAL_u8 seqBuf[12]={0x1,0x4,0x0,0x4,0x0,0x4,0x0,0x0,0x0,0x4,0x0,0x0};//sequence 

	if((prodSection.manufacturerSection.hardwareCapabilities & CAN_ASSET_TRACKING_CAPS_MASK)!=0)
	{
		//put 27 (0x001B) and 256(0x0100). This will let seq do a calibration of around 250ms
		hclk_accu_32k_theoric=830029537;// Theoric value of HCLK_ACCU if 32K is exact for these value of  CCAL_LCLK_CAL_LASTS and CCAL_LCLK_CAL_TIMING
		confTIM=0x001B;
		confLAST=0x0100;
	}
	#ifdef TODO_WORK_AND_REFINE_ME
	else
	{
		//put 2 (0x0002) and 256(0x0100). This will let seq do a calibration of around 7.8ms
		hclk_accu_32k_theoric= 31924212;// Theoric value(to be refined) of HCLK_ACCU if 32K is exact for these value of  CCAL_LCLK_CAL_LASTS and CCAL_LCLK_CAL_TIMING
		confTIM=0x0002;
		confLAST=0x0100;
	}
	#endif //#ifdef TODO_WORK_AND_REFINE_ME


	#ifndef TODO_WORK_AND_REFINE_ME
	if((prodSection.manufacturerSection.hardwareCapabilities & CAN_ASSET_TRACKING_CAPS_MASK)!=0) {
	#endif //#ifndef TODO_WORK_AND_REFINE_ME
	//read current LCLK_Cal_timing and LCLK_Cal_Lasts
	pp_hw_reg16_read(0,	BAR_CFG_SYSTEM,		BAR_CCAL_LCLK_CAL_TIMING_W0,&bckTIM);
	pp_hw_reg16_read(0,	BAR_CFG_SYSTEM,		BAR_CCAL_LCLK_CAL_LASTS_W0,	&bckLAST);
	//time to wait is around = LCLK_Cal_timing x 256 - LCLK_Cal_last @ 32K
	//put 27 (0x001B) and 256(0x0100). This will let seq do a calibration of around 250ms
	hclk_accu_32k_theoric=830029537;// Theoric value of HCLK_ACCU if 32K is exact for these value of  CCAL_LCLK_CAL_LASTS and CCAL_LCLK_CAL_TIMING
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_CCAL_LCLK_CAL_TIMING_W0,confTIM);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_CCAL_LCLK_CAL_LASTS_W0,	confLAST);
	//calibrate the external 32k
	//configure mask
	radio_set_pmstate(UWB_SC_PM_STDBY);
	pp_hw_reg_write(0,BAR_CFG_SYSTEM,BAR_CCAL_SETTINGS, 0x1);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_INTR_VECT0_MSK_W0,		RADIO_VECT0_MSK_LCLK_CALIBRATION_FINISHED);
	pp_hw_reg_write(0,	BAR_CFG_32K,  		BAR_32K_TIMER_CONFIG,		0x00); // stop sequencer
	//Timer sequencer init
	pp_hw_reg16_write(0,BAR_CFG_32K, 		BAR_32K_TIMER_PERIOD_W0,	128);
	pp_hw_reg_write(0,	BAR_CFG_32K, 		BAR_32K_FRAMERATE_LENGTH,	1);
	pp_hw_burst_write(0,BAR_CFG_32K,		BAR_32K_M,  				seqBuf, 12);
	//Clear IRQ register
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_INTR_VECT0_CLR_W0,		0xFFFF);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_INTR_VECT0_CLR_W0,		0x0000);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_INTR_VECT2_CLR_W0,		0xFFFF);
	OSAL_udelay(100);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_INTR_VECT2_CLR_W0,		0x0000);
	//prepare FIFO TX and RX
	radio_set_pmstate(UWB_SC_PM_STDBY);
	pp_hw_reg_write(0,	BAR_CFG_32K,		BAR_32K_TIMER_CONFIG,  		0x1);
	pp_hw_burst_read(   0,  BAR_CTRL_SYSTEM,BAR_INTR_VECT0_W0,			intr_vect, 4);
	while((intr_vect[1]&0x01)==0x00) pp_hw_burst_read(0,BAR_CTRL_SYSTEM,BAR_INTR_VECT0_W0,intr_vect, 4);
	pp_hw_reg32_read(0, BAR_CTRL_SYSTEM,	BAR_CCAL_N_HCLK_ACCU_W0,	&hclk_accu);
	radio_set_pmstate(UWB_SC_PM_2);
	pp_hw_reg_write(0,	BAR_CFG_32K,		BAR_32K_TIMER_CONFIG,		0x0);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_INTR_VECT0_CLR_W0,		0xFFFF);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_INTR_VECT0_CLR_W0,		0x0000);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_INTR_VECT2_CLR_W0,		0xFFFF);
	OSAL_udelay(100);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_INTR_VECT2_CLR_W0,		0x0000);
	//and put back defautl cal prms
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_CCAL_LCLK_CAL_TIMING_W0,bckTIM);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM,		BAR_CCAL_LCLK_CAL_LASTS_W0,	bckLAST);
	if(hclk_accu>0)
	{
		#ifdef TODO_WORK_AND_REFINE_ME
		if((prodSection.manufacturerSection.hardwareCapabilities & CAN_ASSET_TRACKING_CAPS_MASK)==0)
		{
			OSAL_u64 hclk_accu_64=(OSAL_u64) hclk_accu;
			OSAL_u8 capatun;
			OSAL_s64 drift, capatunDrift, capatunStep = 1150; // HCK_ACCU += 1150 when capatun += 1 (approximation)
			drift = hclk_accu_32k_theoric - hclk_accu_64;
			capatunDrift = (drift * 1000 / capatunStep); // * 1000 to avoid floating numbers
			// Round to nearest int
			if (capatunDrift > 0)
				capatunDrift = (capatunDrift + 500) / (OSAL_s32)1000;
			else
				capatunDrift = (capatunDrift - 500) / (OSAL_s32)1000;
			pp_hw_reg_read(0, BAR_CFG_ANALOG, BAR_MISC_TUNE_OSCQ32K_CAPATUN, &capatun);
			if (capatunDrift != 0)
			{
				capatun = capatun + (OSAL_u8)((capatunDrift & 0xFF));
				pp_hw_reg_write(0, BAR_CFG_ANALOG, BAR_MISC_TUNE_OSCQ32K_CAPATUN, capatun);
			}
		}
		else
		#endif //#ifdef TODO_WORK_AND_REFINE_ME
		{
			//set the rtc fine tune
			OSAL_u64 ftune=0;
			OSAL_u64 hclk_accu_64=(OSAL_u64) hclk_accu;
			//this value must change if BAR_CCAL_LCLK_CAL_LASTS/BAR_CCAL_LCLK_CAL_TIMING change
			//if hclk_accu<hclk_accu_32k_theoric, mean 32K is faster than 32768 KHz, time must be increased. fine tune > 1
			//if hclk_accu>hclk_accu_32k_theoric, mean 32K is slower than 32768 KHz, time must be decreased. fine tune < 1
			//anyway rtc finetune should be hclk_accu_32k_theoric/hclk_accu
			//sys_rtc_set_fineCorrection(arg), with arg a 64bits Q32.
			//hclk_accu is on Q0 so << 32 => Q32
			ftune = (hclk_accu_32k_theoric<<32);
			ftune /=hclk_accu_64;
			sys_rtc_set_fineCorrection(ftune);
		}
	}
	#ifndef TODO_WORK_AND_REFINE_ME
	}
	#endif //#ifndef TODO_WORK_AND_REFINE_ME
}

typedef void (*f_tracer)(const char*,...);

OSAL_void radio_showCurPhyAndDac(int forceShow)
{
	OSAL_u32 dv=0;
	config_phy_t * current_config_phy = get_current_config_phy(ctx.cfg_ctx);
	f_tracer tracefunc = (f_tracer) OSAL_tracer(TRACE_INFO);
	const char* end="";
	OSAL_dac_get_value(&dv);

	if(forceShow)
	{
		tracefunc=(f_tracer) printline_fct;
		end=PRINT_ENDOFLINE;
	}
	tracefunc("Phy:%s",end);
	tracefunc("Ch:    %d%s", current_config_phy->channel,end);
	tracefunc("Sync:  %d/%d,ID %d%s", current_config_phy->sync_code,current_config_phy->sync_rep, current_config_phy->sync_code_ID,end);
	tracefunc("Demod  %d/%d,ID %d%s", current_config_phy->demod_code,current_config_phy->demo_rep, current_config_phy->demod_code_ID,end);
	tracefunc("CTRK:  %d%s", current_config_phy->clock_track,end);
	tracefunc("FRMR:  %x%s", current_config_phy->framer_config,end);
	tracefunc("RFPath:%x%s", current_config_phy->rf_path,end);
	tracefunc("THld:  %d/%d%s",current_config_phy->sync_threshold,current_config_phy->rng_threshold,end);
	tracefunc("Tx/RXG:%d/%d%s", current_config_phy->tx_gain,current_config_phy->rx_gain,end);
	tracefunc("PM:    %d%s", current_config_phy->power_mode,end);
	tracefunc("P tout:%d%s", current_config_phy->preamble_timeout,end);
	tracefunc("DAC:   %d%s", dv,end);
	tracefunc("MSQ/PM:%d/%d%s",ctx.cfg_ctx->enable_pm,ctx.cfg_ctx->pm_msq,end);
}

OSAL_u8 radio_init()
{
	OSAL_bool_t previous_msq = ctx.cfg_ctx->pm_msq;
	OSAL_u8 status = OSAL_ERROR;
	OSAL_u16 backup_dac_val = 0;
	OSAL_u8 backup_capatune_val = 0;
#ifdef CONFIG_GENERIC
	OSAL_u8 channel;
	OSAL_u8 root_code, root_rep;
	OSAL_u32 prod_root_thld;
#endif //CONFIG_GENERIC

	struct setting OSAL_PROD_SECT * init_tab = NULL;
	radio_set_init_busy(OSAL_true);

	sys_reset_status();
	//backup dac, capatune value, and restore them after radio init
	sys_get_dac_val(&backup_dac_val);
	sys_get_capatune_val(&backup_capatune_val);

	init_tab = prep_config_init(&ctx.cfg_ctx->phy);
	if ( init_tab != NULL )
		pp_hw_prod_seq_write(ctx.cfg_ctx, init_tab);
	else
	{
		OSAL_trace(TRACE_ERROR, "!!INIT_TAB");
		goto beach;
	}
	ctx.cfg_ctx->pm_msq = OSAL_false; //enable old way to switch PM	
	radio_set_pmstate_busy(OSAL_true);
	radio_set_pmstate(UWB_SC_PM_2);
	radio_set_pmstate_busy(OSAL_false);
	if(radio_msq_init(previous_msq)!=0)
		goto beach;
	ctx.cfg_ctx->pm_msq=previous_msq;//backup

	#ifdef CONFIG_GENERIC
	// LTA init, management within GENERIC software only
	release_MemPool(&ctx.ctxMemPool, 0);
	init_MemPool(&ctx.ctxMemPool, filter_get_alloc_Size(FILTER_KALMAN, 0)*LTA_MAX_CHANNEL, 0);
	for(channel=1; channel<=LTA_MAX_CHANNEL; channel++)
	{
		lta_init(&ctx.lta_ctx[channel-1], ctx.ctxMemPool);
		pp_conf_get_sync_code_repetition_from_zone(get_zoneDefault(), &root_code, &root_rep);
		pp_conf_get_prod_threshold_from_zone(get_zoneDefault(), channel, DEFAULT_RFPATH, &prod_root_thld);
		lta_set_params(&ctx.lta_ctx[channel-1], channel, DEFAULT_RFPATH, root_code, root_rep, prod_root_thld);
		lta_init_filter_from_temperature(&ctx.lta_ctx[channel-1], ctx.cfg_ctx->temperature);
	}
	#endif

	#ifdef CONFIG_RTLS_PROTOCOL
	if( 	! (get_assetTracking_config(ctx.cfg_ctx) & ASSETTRACKING_ENABLE) ||
		( (get_assetTracking_config(ctx.cfg_ctx) & ASSETTRACKING_ENABLE) && ctx.assetTracking.status == 0 ) )
	{
		radio_ext32K_calibrate();
	}
	#endif

	status = OSAL_OK;
beach:
	//restore dac, capatune value,
	sys_set_dac_val(backup_dac_val);
	sys_set_capatune_val(backup_capatune_val);
	radio_set_init_busy(OSAL_false);
	radio_showCurPhyAndDac(0);
	return status;
}


OSAL_void radio_manage_hardFault()
{
}
#define ALIGN_EP  0
#define ALIGN_PRP 1
OSAL_u8 radio_is_spiwr_busy()
{
	return module_is_busy(MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_SPIWR_BUSY_MSK);
}

OSAL_void radio_set_spiwr_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_SPIWR_BUSY_MSK);
}

OSAL_void radio_spi_write_get_buf(OSAL_u8** data,OSAL_u16 burstLen)
{
	//will be unsetted once write done
	sys_set_shared_data_size(0,burstLen);
	sys_get_shared_data(0,data,NULL);
}

OSAL_void radio_spi_write(OSAL_u8* data,OSAL_u16 burstLen)
{
	//spi read always on buffer1
	OSAL_u8* rd = NULL;
	//use tx_ongoign to indicate that spi is ongoing, x and spi won't never be used together
	sys_set_shared_data_size(1,burstLen);
	sys_get_shared_data(1,&rd,NULL);
	HAL_SPI2_bridge(data, burstLen, rd, burstLen);
}

OSAL_void radio_spi_read(OSAL_u8** data,OSAL_u16 burstLen,OSAL_u16* dataLenWithAck)
{
	sys_set_shared_data_size(1,burstLen);
	sys_get_shared_buf(1,data,dataLenWithAck);
}

OSAL_u8 radio_is_pmstate_busy()
{
	return module_is_busy(MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_PMSTATE_BUSY_MSK);
}

OSAL_void radio_set_pmstate_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_PMSTATE_BUSY_MSK);
}

OSAL_void radio_get_pmstate(uwb_com_subcmd_e* state)
{
	*state = module_config[MOD_CFG_POS_RADIO_PM_STATE];
}

//pw enum conversion. for the moment, enum uwb_com_subcmd_e is mapped on the same value thn pm_state_t, but could change
#define PMSTATE_2_UWB_PMSTATE(_p) _p
#define UWB_PMSTATE_2_PMSTATE(_up) _up

OSAL_void radio_set_pmstate(uwb_com_subcmd_e state)
{
	if((state>=UWB_SC_PM_3)&&(state<=UWB_SC_PM_TXRX_IDLE))
	{
		set_new_pm(	PMSTATE_2_UWB_PMSTATE(state),
				ctx.sequencer.cfg_ctx,
				ctx.sequencer.hw_priv_data);
		module_config[MOD_CFG_POS_RADIO_PM_STATE] = state;				
	}
}

OSAL_u8 radio_is_phy_cfg_busy()
{
	return module_is_busy(MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_PHYCFG_BUSY_MSK);
}

OSAL_void radio_set_phy_cfg_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_PHYCFG_BUSY_MSK);
}

OSAL_void radio_get_phy_cfg_data(OSAL_u8** buf)
{
	//no chance phycfg is used at the same time than ranging, so use the same buf
	sys_get_shared_buf(0,buf,NULL);
}

OSAL_void radio_get_phy_cfg(OSAL_u8** phy_cfg,OSAL_u16 phy_cfg_size)
{
	if((phy_cfg)&&(*phy_cfg))
	{
		config_phy_t* ans_config_phy=(config_phy_t*) *phy_cfg;
		radio_get_phy_cfg_data(phy_cfg);
		OSAL_memcpy(*phy_cfg,get_current_config_phy(ctx.cfg_ctx),OSAL_MIN(phy_cfg_size,sizeof(config_phy_t)));
		ans_config_phy->manual_threshold = (pp_conf_get_enable_lta(ctx.cfg_ctx)==OSAL_true)?0:1;
	}
}

OSAL_void radio_set_phy_cfg(OSAL_u8* phy_cfg,OSAL_u16 phy_cfg_size)
{
	config_phy_t new_config_phy;
	OSAL_memcpy(&new_config_phy,phy_cfg,OSAL_MIN(phy_cfg_size,sizeof(config_phy_t)));
	struct setting* config_tab = NULL;
	if(!phy_cfg) return;
	config_tab = OSAL_malloc(sizeof(struct setting)*MAX_PHY_CONF_SIZE_CONFIG_TAB,COREID_CONFIG);
	//Check for manual_threshold
	if(config_tab != NULL)
	{
		pp_conf_set_enable_lta(ctx.cfg_ctx,(new_config_phy.manual_threshold)?OSAL_false:OSAL_true);
		radio_set_pmstate_busy(OSAL_true);
		radio_set_pmstate(UWB_PMSTATE_2_PMSTATE(new_config_phy.power_mode));
		radio_set_pmstate_busy(OSAL_false);
		set_config_phy(ctx.cfg_ctx, &new_config_phy, config_tab, MAX_PHY_CONF_SIZE_CONFIG_TAB, OSAL_false);
		pp_hw_seq_write(&ctx, config_tab);
		OSAL_free(config_tab,COREID_CONFIG);
	}
	else
	{
		OSAL_trace(TRACE_WARN,"Mem");
	}
}

OSAL_void radio_get_rx_cfg_data(OSAL_u8** rx_cfg, OSAL_u16* rxcfgSize)
{
	//put to 1, we will set it asap
	*rx_cfg = &module_config[MOD_CFG_POS_RADIO_RX_CFG];
	*rxcfgSize = 3;
}

OSAL_u8 radio_is_rx_cfg_busy()
{
	return module_is_busy(MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_RX_CFG_BUSY_MSK);
}

OSAL_void radio_set_rx_cfg_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_RX_CFG_BUSY_MSK);
}

OSAL_void radio_get_rx_cfg(OSAL_u8* rx_cfg, OSAL_u16* rx_dataSizeInBits)
{
	*rx_cfg = module_config[MOD_CFG_POS_RADIO_RX_CFG];
	MSBLSB_TO_U16VAL(	module_config[MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_MSB],
				module_config[MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_LSB],
				*rx_dataSizeInBits);
}

OSAL_void radio_set_rx_cfg(OSAL_u8 rx_cfg, OSAL_u16 rx_dataSizeInBits)
{
	module_config[MOD_CFG_POS_RADIO_RX_CFG] = rx_cfg;
	U16VAL_TO_MSBLSB(   	rx_dataSizeInBits,
				module_config[MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_MSB],
				module_config[MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_LSB]);
	//will be redone on rx_start, because tx can reset it

	/* *********************** */
	//systematically set size in bits in BAR_FRAME_SIZE_W0
	pp_hw_reg16_write(  0,  BAR_CFG_DIGITAL,    BAR_FRAME_SIZE_W0,      rx_dataSizeInBits);
	pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);
}

#ifdef CONFIG_GENERIC
static OSAL_void radio_clear_IT()
{
	OSAL_u8 clear_all[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	OSAL_u8 clear_none[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	pp_hw_burst_write(  0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT1_CLR_W0,  clear_all, sizeof(clear_all)/sizeof(char));
	OSAL_udelay(100);
	pp_hw_burst_write(  0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT1_CLR_W0,  clear_none, sizeof(clear_none)/sizeof(char));
}

OSAL_void radio_clear_rng_timer()
{
	OSAL_u8 f_clear = 0x00;
	pp_hw_reg_read(     0,  BAR_CFG_SYSTEM,     BAR_FORCE_CLR,          &f_clear);
	f_clear |=0x80;
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_FORCE_CLR,          f_clear);
	OSAL_udelay(100);
	f_clear &=0x7F;
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_FORCE_CLR,          f_clear);
}

OSAL_void radio_lta_IR_management(OSAL_u8* prp, OSAL_u32* nrj)
{
	OSAL_u32 root_thld=0;
	config_phy_t* curPhy = get_current_config_phy(ctx.cfg_ctx);

	lta_compute_root_threshold_from_IR(&ctx.lta_ctx[curPhy->channel-1], curPhy->sync_code, curPhy->sync_rep, nrj, prp);
	root_thld = lta_get_live_root_threshold(&ctx.lta_ctx[curPhy->channel-1]);
	pp_conf_set_root_threshold(ctx.cfg_ctx, curPhy->channel, curPhy->rf_path, root_thld);
	radio_set_phy_cfg((OSAL_u8*) curPhy,sizeof(config_phy_t));
}

OSAL_void radio_lta_temp_management()
{
	OSAL_u32 root_thld=0;
	OSAL_s32 temp;
	config_phy_t* curPhy = get_current_config_phy(ctx.cfg_ctx);

	sys_get_temperature_level(NULL,&temp);
	pp_conf_set_temperature(ctx.cfg_ctx, (OSAL_s8)(temp));

	lta_compute_root_threshold_from_temperature(&ctx.lta_ctx[curPhy->channel-1], ctx.cfg_ctx->temperature);
	root_thld = lta_get_live_root_threshold(&ctx.lta_ctx[curPhy->channel-1]);
	pp_conf_set_root_threshold(ctx.cfg_ctx, curPhy->channel, curPhy->rf_path, root_thld);
	radio_set_phy_cfg((OSAL_u8*) curPhy,sizeof(config_phy_t));
}

OSAL_void radio_set_tx_data_buf_size(OSAL_u16 sizeInBytes)
{
	sys_set_shared_data_size(1,sizeInBytes+2);
}

OSAL_void radio_get_tx_data_buf(OSAL_u8** data,OSAL_u16* sizeInBytes)
{
	OSAL_u16 sizePlus2=0;
	OSAL_u8* dataPlus2=0;
	sys_get_shared_data(1,&dataPlus2,&sizePlus2);
	*data = dataPlus2+2;
	*sizeInBytes = sizePlus2-2;
}

OSAL_void radio_write_tx_data(OSAL_u8* data,OSAL_u16 sizeInBits)
{
	OSAL_u16 sizeInBytes = ROUND_BITS_TO_BYTES(sizeInBits);
	OSAL_u8 framing_cfg = 0;
	OSAL_u16 sizePlus2=0;
	OSAL_u8* dataPlus2=0;
	OSAL_trace(TRACE_DEBUG,"write TX");
	U16VAL_TO_MSBLSB(	sizeInBits,
				module_config[MOD_CFG_POS_RADIO_TX_DATA_SIZE_INBITS_MSB],
				module_config[MOD_CFG_POS_RADIO_TX_DATA_SIZE_INBITS_LSB]);
	pp_hw_reg_read(     0,  BAR_CFG_DIGITAL,    BAR_FRAMING_CFG,        &framing_cfg);
	sys_get_shared_data(1,&dataPlus2,&sizePlus2);
	if(data != NULL)
	{
		OSAL_u8* dataTX = dataPlus2+2;
		OSAL_memcpy(dataTX,data,sizeInBytes);
	}
		
	switch(framing_cfg&RADIO_FRAMING_CFG_FRAMING_STYLE_MASK)
	{
		//TODO : add here protection against size overflow.
		case RADIO_FRAMING_CFG_FRAMING_STYLE_LONG_PHR://size is in bits
			dataPlus2[0]=(OSAL_u8) (sizeInBytes&0xFF);
			dataPlus2[1]=(OSAL_u8) (sizeInBytes>>8);
			//systematically set size in bits in BAR_FRAME_SIZE_W0
			pp_hw_reg16_write(   0,  BAR_CFG_DIGITAL,    BAR_FRAME_SIZE_W0,       sizeInBits+16);
			pp_hw_reg_write(0,BAR_FIFO_TXRX, 		BAR_FIFO_TX_CLEAR,		0x01);
			pp_hw_burst_write(0,BAR_FIFO_TXRX,BAR_FIFO_TX_DATA,
						dataPlus2,sizeInBytes+2);
			break;
		case RADIO_FRAMING_CFG_FRAMING_STYLE_SHORT_PHR://size must be in bytes
			dataPlus2++;
			*dataPlus2=(OSAL_u8) sizeInBytes;
			pp_hw_reg16_write(   0,  BAR_CFG_DIGITAL,    BAR_FRAME_SIZE_W0,       sizeInBits+8);
			pp_hw_reg_write(0,BAR_FIFO_TXRX, 		BAR_FIFO_TX_CLEAR,		0x01);
			pp_hw_burst_write(0,BAR_FIFO_TXRX,BAR_FIFO_TX_DATA,
						dataPlus2,sizeInBytes+1);

			break;
		default://case RADIO_FRAMING_CFG_FRAMING_STYLE_NO_PHR
			dataPlus2+=2;
			pp_hw_reg16_write(   0,  BAR_CFG_DIGITAL,    BAR_FRAME_SIZE_W0,       sizeInBits);
			pp_hw_reg_write(0,BAR_FIFO_TXRX,  BAR_FIFO_TX_CLEAR,		0x01);
			pp_hw_burst_write(0,BAR_FIFO_TXRX,BAR_FIFO_TX_DATA,dataPlus2,sizeInBytes);
			//already set in BAR_FRAME_SIZE_W0
			break;
	}
}

OSAL_void radio_get_rx_data_size(OSAL_u16* dataSizeWithoutAck)
{
	sys_get_shared_data(0,NULL,dataSizeWithoutAck);
}

OSAL_void radio_get_rx_data_checksum(OSAL_u16* checksum)
{
	sys_get_shared_data_checksum(0,checksum);
}

OSAL_void radio_get_rx_buf(OSAL_u8** buf,OSAL_u16* dataSizeWithAck)
{
	sys_get_shared_buf(0,buf,dataSizeWithAck);
}

OSAL_void radio_tx_start()
{
	OSAL_u16 sizeInBits =0;
	OSAL_u16 IRQ_Conf =0xFFFF;
	module_config[MOD_CFG_POS_RADIO_TX_ONGOING] = 1;
	module_config[MOD_CFG_POS_RADIO_TX_ABORT] = 0;
	/* start from clean state */
	//in case of no phr, if we've done some RX between set_tx_data and this, rewrite size
	MSBLSB_TO_U16VAL(	module_config[MOD_CFG_POS_RADIO_TX_DATA_SIZE_INBITS_MSB],
				module_config[MOD_CFG_POS_RADIO_TX_DATA_SIZE_INBITS_LSB],
				sizeInBits);
	//systematically set size in bits in BAR_FRAME_SIZE_W0
	pp_hw_reg16_write(   0,  BAR_CFG_DIGITAL,    BAR_FRAME_SIZE_W0,       sizeInBits);
	/* start from clean state */
	radio_clear_IT();
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT1_MSK_W0,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT1_MSK_W1,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT2_MSK_W0,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT2_MSK_W1,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT3_MSK_W0,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT3_MSK_W1,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT0_MSK_W0,  0xEE);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT0_MSK_W1,  0xFF);
	// Enable TX
	IRQ_Conf &=RADIO_VECT0_MSK_TX_DONE;
	IRQ_Conf &=RADIO_VECT0_MSK_TX_ACTIVE;
	pp_hw_reg16_write(    0,  BAR_CFG_SYSTEM,   BAR_INTR_VECT0_MSK_W0,  IRQ_Conf);

	pp_hw_reg_write(    0,  BAR_CFG_DIGITAL,    BAR_PHY_CTRL_CMD,       0x01);
}

OSAL_void radio_rx_start()
{
	OSAL_u16 rx_dataSize=0;
	OSAL_u8 crtl_cfg = 0;
	OSAL_u8 rx_cfg = 0;
	OSAL_u16 IRQ_Conf =0xFFFF;
	OSAL_u8 mux_reg_ctrl_cfg = 0x00;
	OSAL_u8 framing_cfg=0;

	//reseted on abort
	module_config[MOD_CFG_POS_RADIO_RX_ONGOING] = 1;
	module_config[MOD_CFG_POS_RADIO_RX_ABORT] = 0;
	radio_set_rx_cfg_busy(OSAL_true);
	//redo it, as it could have been reseted trough a tx process
	radio_get_rx_cfg(&rx_cfg,&rx_dataSize);
	radio_set_rx_cfg(rx_cfg,rx_dataSize);
	radio_set_rx_cfg_busy(OSAL_false);

	/* *********************** */
	pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);
	pp_hw_reg_read(     0,  BAR_CFG_DIGITAL,    BAR_FRAMING_CFG,        &framing_cfg);
	/* start from clean state */
	/* set the registers for time out */
	IRQ_Conf &=RADIO_VECT0_MSK_RX_DONE;
	IRQ_Conf &=RADIO_VECT0_MSK_RX_ACTIVE;
	crtl_cfg &= RADIO_CTRLCFG_SFD_TIME_OUT_OFF_MSK;
	crtl_cfg &= RADIO_CTRLCFG_PREAMB_TIME_OUT_OFF_MSK;
	crtl_cfg |= RADIO_CTRLCFG_SFD_TIME_OUT_BEHAVIOUR;
	crtl_cfg |= RADIO_CTRLCFG_PREAMB_TIME_OUT_BEHAVIOUR;
	if (rx_cfg&RADIO_RX_CFG_PREAMBLE_TOUT_ON_MASK)
	{
		IRQ_Conf &= RADIO_VECT0_MSK_PREAMB_TIME_OUT;
		crtl_cfg |= RADIO_CTRLCFG_PREAMB_TIME_OUT_ON;
	}
	if (rx_cfg&RADIO_RX_CFG_SFD_TOUT_ON_MASK)
	{
		IRQ_Conf &= RADIO_VECT0_MSK_SFD_TIME_OUT;
		crtl_cfg |= RADIO_CTRLCFG_SFD_TIME_OUT_ON;
	}

	if(framing_cfg&RADIO_FRAMING_CFG_PSDU_CRC_MASK)
		IRQ_Conf &= RADIO_VECT0_MSK_PDSU_CRC_OK;	
	if(framing_cfg&RADIO_FRAMING_CFG_PHR_CRC_MASK)
		IRQ_Conf &= RADIO_VECT0_MSK_PHR_CRC_OK;	


	pp_hw_reg16_write(    0,  BAR_CFG_SYSTEM,   BAR_INTR_VECT0_MSK_W0,  IRQ_Conf);
	pp_hw_reg_read(     0,  BAR_CFG_MUX_REG,    BAR_MUX_REG_W33,        &mux_reg_ctrl_cfg);
	pp_hw_reg_write(    0,  BAR_CFG_MUX_REG,    BAR_MUX_REG_W33,        mux_reg_ctrl_cfg & ~0x10);
	pp_hw_reg_write(    0,  BAR_CFG_DIGITAL,    BAR_CTRL_CFG,           crtl_cfg);
	pp_hw_reg_write(    0,  BAR_CFG_MUX_REG,    BAR_MUX_REG_W33,        mux_reg_ctrl_cfg | 0x10);
	radio_clear_IT();
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT1_MSK_W0,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT1_MSK_W1,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT2_MSK_W0,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT2_MSK_W1,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT3_MSK_W0,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT3_MSK_W1,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT0_MSK_W0,  0xEE);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT0_MSK_W1,  0xFF);
	// Enable RX
	pp_hw_reg_write(    0,  BAR_CFG_DIGITAL,    BAR_PHY_CTRL_CMD,       0x02);
}

//0:continue, non zero stopped
OSAL_u16 radio_is_tx_finished(uwb_com_sysstatus_e* endStatus,OSAL_u8 IRQ0_Vector_W0)
{
	if( module_config[MOD_CFG_POS_RADIO_TX_ABORT] == 1)
	{
		//reset on abort
		*endStatus = UWB_STAT_NONE;
		OSAL_trace(TRACE_INFO,"Aborted");
		return RADIO_STOPPED_ABORTED_BY_USER;
	}
	if(IRQ0_Vector_W0 &RADIO_VECT0_TX_DONE_MASK)
	{
		*endStatus=UWB_STAT_TX_DONE;
		OSAL_trace(TRACE_DEBUG,"TX DONE");
		return 1;
	}
	return 0;
}


static OSAL_s32 radio_compute_dll_fine(OSAL_u32 conf0_nrj_m4dll, OSAL_u32 conf0_nrj_p0dll, OSAL_u32 conf0_nrj_p4dll)
{
	OSAL_s64 ax8192 = 0;
	OSAL_s64 bx8192 = 0;
	OSAL_s64 TOAdllx8192 = 0;

	// NRJ can be null if pulse have not been correctly refreshed
	if(conf0_nrj_m4dll != 0 && conf0_nrj_p0dll != 0 && conf0_nrj_p4dll != 0)
	{
		ax8192 = OSAL_div_s64_s64(((OSAL_s64)conf0_nrj_p4dll + (OSAL_s64)conf0_nrj_m4dll -2*(OSAL_s64)conf0_nrj_p0dll) * (OSAL_s64)8192, 128 * (OSAL_s64)conf0_nrj_p0dll);
		bx8192 = OSAL_div_s64_s64(((OSAL_s64)conf0_nrj_m4dll - 64 * ax8192 - (OSAL_s64)conf0_nrj_p0dll) * (OSAL_s64)8192, -8 * (OSAL_s64)conf0_nrj_p0dll);
	}

	if(ax8192 != 0)
		TOAdllx8192 = OSAL_div_s64_s64(-bx8192, 2 * ax8192) - 4; // Search max + Center parabola to 0

	return (OSAL_s32)TOAdllx8192;
}

// FIXME: We should understand why drift given by clock track is underestimated of about 30%
static OSAL_bool_t radio_clock_track_get_drift(rng_data_generic_t* rng)
{
	OSAL_s32 dynamic_dll_drift;
	OSAL_u32 timer_rx_done = 0, timer_clktrk_start = 0;
	//OSAL_s8 AlignEp = 0, AlignPrp = 0;
	OSAL_u32 DriftPPB32;

	if(rng == NULL) return OSAL_false;

	// TODO: should I take clock track align into account?
	// CEA example code suggests not using them
	/*pp_hw_reg_read(contexte->sequencer.hw_priv_data, BAR_CTRL_DIGITAL, BAR_RX_ACC_ALIGN_PRP,(OSAL_u8*)&AlignPrp);
	pp_hw_reg_read(contexte->sequencer.hw_priv_data, BAR_CTRL_DIGITAL, BAR_RX_ACC_ALIGN_EP, (OSAL_u8*)&AlignEp);*/
	pp_hw_reg32_read(0, BAR_CTRL_DIGITAL, BAR_DYNA_DLL_DRIFT_W0 ,(OSAL_u32*)&dynamic_dll_drift);

	pp_hw_reg32_read(0, BAR_CTRL_DIGITAL, BAR_TIMER_RX_DONE_W0, &timer_rx_done);
	pp_hw_reg32_read(0, BAR_CTRL_DIGITAL, BAR_TIMER_CLKTRK_START_W0, &timer_clktrk_start);

	// Exact formula is:
	// driftPPB32 = 1000*(dynamic_dll_drift * 125 *1000)/((timer_rx_done-timer_clktrk_start)*16)

	//FIXME: using 64 bits computation may help 
	if(OSAL_ABS(dynamic_dll_drift) > (max_u32/(to_u32(125)*to_u32(1000)*to_u32(1000)))) // prevent u32 overflow
	{
		OSAL_trace(TRACE_WARN, 
			"Huge dynamic ddl drift (%d), truncate clock track",
			dynamic_dll_drift);
		DriftPPB32 = max_u32;
	}
	else
	{
		DriftPPB32 = OSAL_ABS(dynamic_dll_drift) * (to_u32(125)*to_u32(1000)*to_u32(1000));
	}

	if(timer_rx_done > timer_clktrk_start)
		DriftPPB32 /= EP_TO_NANO(timer_rx_done-timer_clktrk_start);
	else
	{
		OSAL_trace(TRACE_WARN, "Weird Clock Track behavior");
		DriftPPB32 /= 0xFFFFFFF - timer_clktrk_start + timer_rx_done;
	}


	if(DriftPPB32 > max_u16)
	{
		OSAL_trace(TRACE_WARN,
			"Drift (%d) > what we can correct (%d), truncated to max",
			DriftPPB32, max_u16);
		DriftPPB32 = max_u16;
	}

	//OSAL_trace(TRACE_DEBUG, "Measured Absolute Drift PPB %u\n", DriftPPB32);
	
	rng->clock_drift = dynamic_dll_drift > 0 ? DriftPPB32 : - DriftPPB32;
	return OSAL_true;
}


#ifdef HYBRID_TOA
// Average offset between FAT and FML
static const OSAL_u32 FAT_to_FML_correction_tab[]={
	/* CH     offset (ps) */    
	/* 1*/	  3037,
	/* 2*/	  2811, 
	/* 3*/	  2934,
	/* 4*/	  2047
};
#endif // HYBRID_TOA

#define ALIGN_EP  0
#define ALIGN_PRP 1
static OSAL_void radio_rx_process_toa(rng_data_generic_t* rng)
{
	OSAL_u32 EP;
	OSAL_u32 nrj_sum_fml = rng->toa_le_corr;
	OSAL_s32 sfd_delay, rtoa_ep = 0; //all in EP
	OSAL_s32 IW = 0, MISSINGIW=0, IWMULT = 0;
	OSAL_s32 toa_le_raw_pico=0;
	OSAL_s8  ALIGN_TAB[2]; // Align EP , Align PRP
	OSAL_u16 LePosReg = 0, Fg1PosReg = 0; // in iw, only aliases
	OSAL_s32 ntoa_correction = 0;	//in NTOA
	OSAL_s32 hw_offset = 0;
#ifdef CONFIG_DLL_FINE
	OSAL_u8 Dll[12];
	OSAL_u32 conf0_nrj_m4dll, conf0_nrj_p0dll, conf0_nrj_p4dll;
#endif //defined CONFIG_DLL_FINE
#ifdef HYBRID_TOA
	OSAL_u16 idxFAT = 0;
	OSAL_u32 correction_FAT = 0;
	OSAL_s32 IW_FAT = 0, NTOA_FAT = 0, IW_FML = 0, NTOA_FML = 0; 
	OSAL_u32 FML_NRJ_SUM_saturation_value = 0;
#endif // HYBRID_TOA

	/***  TOA Processing ***/

	/*Clock track alignement*/
	pp_hw_burst_read(0, 
		BAR_CTRL_DIGITAL, BAR_RX_ACC_ALIGN_EP,
		(OSAL_u8*)(ALIGN_TAB),sizeof(ALIGN_TAB));

	/* EP COUNTER */
	pp_hw_reg32_read(0, BAR_CTRL_DIGITAL, BAR_RNG_COARSE_RX_W0,&EP);

	// Ooups.. too late, next action already started
	if(EP <= 512)
	{
		//everything to 0
		rng->toa_le_raw = 0;
		rng->toa_le_corr = 0;
		rng->iwfg1 = 0;
		rng->iwfg2 = 0;
		rng->iwfg3 = 0;
		rng->iwfg4 = 0;
		rng->iwle = 0;
		rng->toa_fg1=0;
		rng->toa_fg2=0;
		rng->toa_fg3=0;
		rng->toa_fg4=0;
		return;
	}

	//FIXME:  Fg1PosReg and LePosReg will be wrong if we did not get the estimate
	Fg1PosReg = rng->prp[rng->iwfg1]*64+rng->iwfg1;
	LePosReg  = rng->prp[rng->iwle]*64+rng->iwle;

	sfd_delay = PRP_TO_EP((pp_conf_get_preamble_length(rng->sync_code, rng->sync_rep)+SFD_SIZE_BIT) * to_s32(rng->sync_code) * to_s32(rng->sync_rep)); // in EP

	//Computing rtoa_ep from above collected values
	rtoa_ep = (EP - sfd_delay);				// Position of SFD in EP vs what we expect
	rtoa_ep -= to_s32(512);					// CEA offest for non-negative ranging
	rtoa_ep += ALIGN_TAB[ALIGN_EP] + PRP_TO_EP(ALIGN_TAB[ALIGN_PRP]);	// Clock track compensation
	rtoa_ep -= to_s32(56);// this offset was checked for C127R1, C12R2, C127R4 and C127R8

	// checked with C127R8/R1 may be checked if code size change
	rtoa_ep += 	PRP_TO_EP(to_s32(rng->sync_code) * to_s32(rng->sync_rep)) -
			PRP_TO_EP(to_s32(rng->demod_code) * to_s32(rng->demo_rep));
						
	// Substract hw offset. We do approximation that 125ps = 1ntoa as offset values are small
	// pp_conf_get_hw_offset(hw_offset, rng->channel, rng->rf_path, RX);
	hw_offset = PICO_TO_NTOA(hw_offset);

	// RX can't be started at a 125ps granularity, only at EP one
	// thus compensate with cmd.time.ntoa
	ntoa_correction =  hw_offset;

	IW = EP_TO_IW(rtoa_ep);

	// Last iw. Position of Leading Edge vs MaxPower
	// Note that LePosReg - (Fg1PosReg & 0xFFF0) should always be less than 16, but because of a bug in case
	// of "negative" LePosReg, it might happen there is an error of 1PRP (e.g. 64), thus we add a "modulo 16" here
	IW += (to_s32(LePosReg) - to_s32(Fg1PosReg & 0xFFF0));
	IW += NTOA_TO_IW(ntoa_correction);

#ifdef HYBRID_TOA
#define FAT_CORRECTION_COEFF 240
	// Compute saturation value (log2). At this nrj FAT TOA begin to increase
	FML_NRJ_SUM_saturation_value = (floor_log2(rng->sync_code+1) + 5) * 2 + 1 + floor_log2(rng->sync_rep);

	idxFAT = rng->prp[rng->iwfat]*64+rng->iwfat;

	// Define IW and NTOA for both FAT and FML
	IW_FAT = IW - (LePosReg - idxFAT);
	NTOA_FAT = 0;

	IW_FML = IW;
	NTOA_FML = 0;

	// FAT Correction with FML NRJ SUM
	if(nrj_sum_fml < FML_NRJ_SUM_saturation_value)
		correction_FAT = (FML_NRJ_SUM_saturation_value - nrj_sum_fml) * FAT_CORRECTION_COEFF;

	IW_FAT 		-= correction_FAT / 1000;
	NTOA_FAT 	-= (correction_FAT % 1000) / 125;
	IW_FML 		-= FAT_to_FML_correction_tab[rng->channel-1] / 1000; // We need to substract this offset to merge FML and FAT
	NTOA_FML 	-= (FAT_to_FML_correction_tab[rng->channel-1] % 1000) / 125;

	// FML Correction with NRJ interpolation
	if(OSAL_ABS(rng->prp[rng->iwle]-rng->prp[rng->iwle-1]) <= 1 &&
		OSAL_ABS(rng->prp[rng->iwle+1]-rng->prp[rng->iwle]) <= 1)
	{
		NTOA_FML += radio_compute_dll_fine(rng->nrj[rng->iwle-1], rng->nrj[rng->iwle], rng->nrj[rng->iwle+1]);
	}

	// MERGE RANGING
	// FML = MPW (good pulse) && FML not saturated (really good pulse)
	if(rng->iwle == rng->iwfg1 && nrj_sum_fml < FML_NRJ_SUM_saturation_value)
	{
		rng->toa_le_corr= NTOA_TO_PICO(NTOA_FML);

		IWMULT = IW_FML/IW_TO_NANO_PPCM;
		MISSINGIW = IW_FML%IW_TO_NANO_PPCM;
	}
	else
	{
		rng->toa_le_corr= NTOA_TO_PICO(NTOA_FAT);

		IWMULT = IW_FAT/IW_TO_NANO_PPCM;
		MISSINGIW = IW_FAT%IW_TO_NANO_PPCM;
	}
#else
	rng->toa_le_corr= 0;

	IWMULT = IW/IW_TO_NANO_PPCM;
	MISSINGIW = IW%IW_TO_NANO_PPCM;

#endif // HYBRID_TOA

	//missingiw can pass iw_to_pico macro
	toa_le_raw_pico = IW_TO_PICO(MISSINGIW);
	
	rng->toa_le_raw = toa_le_raw_pico/1000;
	toa_le_raw_pico -= 1000*rng->toa_le_raw;
	
	rng->toa_le_raw +=IWMULT*IW_TO_NANO_PPCM_NS;
	
	//add missing pico part to le_corr
	rng->toa_le_corr+=toa_le_raw_pico;
	//now we will change corr to let its value be in [0...999], sending nanosecond part into toa_le_raw
    rng->toa_le_raw = rng->toa_le_raw + (rng->toa_le_corr/1000);
    toa_le_raw_pico = rng->toa_le_corr - (rng->toa_le_corr/1000)*1000;
    if(toa_le_raw_pico<0)
    {
        rng->toa_le_raw--;
        toa_le_raw_pico += 1000;
    }
    rng->toa_le_corr=toa_le_raw_pico;

	//FIXME: this does not work if we did not get the estimate
	rng->toa_fg1 = EP_TO_IW(rtoa_ep);
	rng->toa_fg1 += rng->prp[rng->iwfg1] * 64 + rng->iwfg1 - (Fg1PosReg & 0xFFF0);
	rng->toa_fg1 += NTOA_TO_IW(ntoa_correction);
	rng->toa_fg1 = IW_TO_NANO(rng->toa_fg1);
	
	rng->toa_fg2 = EP_TO_IW(rtoa_ep);
	rng->toa_fg2 += rng->prp[rng->iwfg2] * 64 + rng->iwfg2 - (Fg1PosReg & 0xFFF0);
	rng->toa_fg2 += NTOA_TO_IW(ntoa_correction);
	rng->toa_fg2 = IW_TO_NANO(rng->toa_fg2);
	
	rng->toa_fg3 = EP_TO_IW(rtoa_ep);
	rng->toa_fg3 += rng->prp[rng->iwfg3] * 64 + rng->iwfg3 - (Fg1PosReg & 0xFFF0);
	rng->toa_fg3 += NTOA_TO_IW(ntoa_correction);
	rng->toa_fg3 = IW_TO_NANO(rng->toa_fg3);
	
	rng->toa_fg4 = EP_TO_IW(rtoa_ep);
	rng->toa_fg4 += rng->prp[rng->iwfg4] * 64 + rng->iwfg4 - (Fg1PosReg & 0xFFF0);
	rng->toa_fg4 += NTOA_TO_IW(ntoa_correction);
	rng->toa_fg4 = IW_TO_NANO(rng->toa_fg4);

	// Clock track drift
	radio_clock_track_get_drift(rng);
}

static const OSAL_u32 radio_SnrDb_scaled_ratio_tab[]={
	/*index ratio	index	ratio */    
	/*1*/	1,	/*2*/	2,
	/*3*/	3,	/*4*/	4,
	/*5*/	5,	/*6*/	6,
	/*7*/	7,	/*8*/	8,
	/*9*/	9,	/*10*/	10,
	/*11*/	11,	/*12*/	12,
	/*13*/	13,	/*14*/	14,
	/*15*/	15,	/*16*/	17,
	/*17*/	18,	/*18*/	19,
	/*19*/	21,	/*20*/	23,
	/*21*/	25,	/*22*/	27,
	/*23*/	29,	/*24*/	32,
	/*25*/	34,	/*26*/	37,
	/*27*/	40,	/*28*/	44,
	/*29*/	48,	/*30*/	52,
	/*31*/	56,	/*32*/	61,
	/*33*/	66,	/*34*/	72,
	/*35*/	78,	/*36*/	85,
	/*37*/	92,	/*38*/	100,
	/*39*/	109,	/*40*/	118,
	/*41*/	128,	/*42*/	139,
	/*43*/	151,	/*44*/	164,
	/*45*/	178,	/*46*/	194,
	/*47*/	210,	/*48*/	229,
	/*49*/	248,	/*50*/	270,
	/*51*/	293,	/*52*/	318,
	/*53*/	346,	/*54*/	375,
	/*55*/	408,	/*56*/	443,
	/*57*/	481,	/*58*/	523,
	/*59*/	568,	/*60*/	617,
	/*61*/	670,	/*62*/	728,
	/*63*/	791,	/*64*/	860,
	/*65*/	934,	/*66*/	1014,
	/*67*/	1102,	/*68*/	1197,
	/*69*/	1301,	/*70*/	1413,
	/*71*/	1535,	/*72*/	1668,
	/*73*/	1812,	/*74*/	1968,
	/*75*/	2138,	/*76*/	2323,
	/*77*/	2524,	/*78*/	2742,
	/*79*/	2979,	/*80*/	3236,
	/*81*/	3516,	/*82*/	3820
};

static const OSAL_u8 radio_SnrDb_scaled_prcent_tab[]={
	/*index prcent	index	prcent */
	/*1*/	0,	/*2*/	8,
	/*3*/	13,	/*4*/	17,
	/*5*/	19,	/*6*/	22,
	/*7*/	23,	/*8*/	25,
	/*9*/	27,	/*10*/	28,
	/*11*/	29,	/*12*/	30,
	/*13*/	31,	/*14*/	32,
	/*15*/	33,	/*16*/	34,
	/*17*/	35,	/*18*/	36,
	/*19*/	37,	/*20*/	38,
	/*21*/	39,	/*22*/	40,
	/*23*/	41,	/*24*/	42,
	/*25*/	43,	/*26*/	44,
	/*27*/	45,	/*28*/	46,
	/*29*/	47,	/*30*/	48,
	/*31*/	49,	/*32*/	50,
	/*33*/	51,	/*34*/	52,
	/*35*/	53,	/*36*/	54,
	/*37*/	55,	/*38*/	56,
	/*39*/	57,	/*40*/	58,
	/*41*/	59,	/*42*/	60,
	/*43*/	61,	/*44*/	62,
	/*45*/	63,	/*46*/	64,
	/*47*/	65,	/*48*/	66,
	/*49*/	67,	/*50*/	68,
	/*51*/	69,	/*52*/	70,
	/*53*/	71,	/*54*/	72,
	/*55*/	73,	/*56*/	74,
	/*57*/	75,	/*58*/	76,
	/*59*/	77,	/*60*/	78,
	/*61*/	79,	/*62*/	80,
	/*63*/	81,	/*64*/	82,
	/*65*/	83,	/*66*/	84,
	/*67*/	85,	/*68*/	86,
	/*69*/	87,	/*70*/	88,
	/*71*/	89,	/*72*/	90,
	/*73*/	91,	/*74*/	92,
	/*75*/	93,	/*76*/	94,
	/*77*/	95,	/*78*/	96,
	/*79*/	97,	/*80*/	98,
	/*81*/	99,	/*82*/	100
};

static OSAL_u8 radio_get_SnrDbScaled(OSAL_u32 nrj, OSAL_u32 threshold)
{
	OSAL_u32 lq;
	OSAL_s16 i;
	OSAL_s16 j;
	#define LOOKUP_SUBDIV 4
	OSAL_s16 min[LOOKUP_SUBDIV]={	1,
					OSAL_ARRAY_SIZE(radio_SnrDb_scaled_ratio_tab)/4,
					OSAL_ARRAY_SIZE(radio_SnrDb_scaled_ratio_tab)/2,
					3*OSAL_ARRAY_SIZE(radio_SnrDb_scaled_ratio_tab)/4
				};
	OSAL_s16 max[LOOKUP_SUBDIV]={	OSAL_ARRAY_SIZE(radio_SnrDb_scaled_ratio_tab)/4,
					OSAL_ARRAY_SIZE(radio_SnrDb_scaled_ratio_tab)/2,
					3*OSAL_ARRAY_SIZE(radio_SnrDb_scaled_ratio_tab)/4,
					OSAL_ARRAY_SIZE(radio_SnrDb_scaled_ratio_tab)-1
				};
	if(threshold>0)
	{
		lq=nrj/threshold;
		//ok now, we must put it on a U8, that mean claculate 
		//SnrDb = 10log10(lq)
                //Convert to percent
                //scale RSSI to 255 = 36db (empiric measurment conducted 0db att)
		//scale Accuracy 255 = 3 * filterMeasStd
		//#define MAX_SNR_DB 36
                //SnrDb = (SnrDb * 100.0f)/MAX_SNR_DB;
                //saturate
                //if(SnrDb>100.0f) SnrDb = 100.0f;
                //if(SnrDb < 0.0) SnrDb = 0.0;
		//lets use a table of u16 with ratio, and a tbale of u8 with scaled between[0..36db] of prcent, resp SnrDb_scaled_ratio_tab and 
		//SnrDb_scaled_prcent_tab, with same size
                //if we keep round of percent, a simple excel can show you that there are only 82 values
                //to recreate excel, if MAX_SNR_DB limits change:
                //	A		B		C		D		E	
                //1	0		-		-		-		-
                //2	=A1+1		=10*LOG(A2)	=B2*100/limit	=ROUND(C2;0)	=D2
                //3	=A2+1		=10*LOG(A3)	=B3*100/limit	=ROUND(C3;0)	=IF(D3<>D2;D3;)
                //4	...		...		...		...		...
                //...
                //N	=A(n-1)+1	=10*LOG(An)	=Bn*100/limit	=ROUND(Cn;0)	=IF(Dn<>D(n-1);Dn;0) stop when E(N) = 100
                //and filter E1, removing 0 and empty, copy filtere A column into SnrDb_scaled_ratio_tab  
                //and filtered E column into SnrDb_scaled_prcent_tab
                //result with MAX_SNR_DB = 36 is
                //	index 		lq		10log10 scaled on[0..36]
                /*	1		1		0	*/
		/*	2		2		8	*/
		/*	3		3		13	*/
		/*	4		4		17	*/
		/*	5		5		19	*/
		/*	6		6		22	*/
		/*	7		7		23	*/
		/*	8		8		25	*/
		/*	9		9		27	*/
		/*	10		10		28	*/
		/*	11		11		29	*/
		/*	12		12		30	*/
		/*	13		13		31	*/
		/*	14		14		32	*/
		/*	15		15		33	*/
		/*	16		17		34	*/
		/*	17		18		35	*/
		/*	18		19		36	*/
		/*	19		21		37	*/
		/*	20		23		38	*/
		/*	21		25		39	*/
		/*	22		27		40	*/
		/*	23		29		41	*/
		/*	24		32		42	*/
		/*	25		34		43	*/
		/*	26		37		44	*/
		/*	27		40		45	*/
		/*	28		44		46	*/
		/*	29		48		47	*/
		/*	30		52		48	*/
		/*	31		56		49	*/
		/*	32		61		50	*/
		/*	33		66		51	*/
		/*	34		72		52	*/
		/*	35		78		53	*/
		/*	36		85		54	*/
		/*	37		92		55	*/
		/*	38		100		56	*/
		/*	39		109		57	*/
		/*	40		118		58	*/
		/*	41		128		59	*/
		/*	42		139		60	*/
		/*	43		151		61	*/
		/*	44		164		62	*/
		/*	45		178		63	*/
		/*	46		194		64	*/
		/*	47		210		65	*/
		/*	48		229		66	*/
		/*	49		248		67	*/
		/*	50		270		68	*/
		/*	51		293		69	*/
		/*	52		318		70	*/
		/*	53		346		71	*/
		/*	54		375		72	*/
		/*	55		408		73	*/
		/*	56		443		74	*/
		/*	57		481		75	*/
		/*	58		523		76	*/
		/*	59		568		77	*/
		/*	60		617		78	*/
		/*	61		670		79	*/
		/*	62		728		80	*/
		/*	63		791		81	*/
		/*	64		860		82	*/
		/*	65		934		83	*/
		/*	66		1014		84	*/
		/*	67		1102		85	*/
		/*	68		1197		86	*/
		/*	69		1301		87	*/
		/*	70		1413		88	*/
		/*	71		1535		89	*/
		/*	72		1668		90	*/
		/*	73		1812		91	*/
		/*	74		1968		92	*/
		/*	75		2138		93	*/
		/*	76		2323		94	*/
		/*	77		2524		95	*/
		/*	78		2742		96	*/
		/*	79		2979		97	*/
		/*	80		3236		98	*/
		/*	81		3516		99	*/
		/*	82		3820		100	*/
		/*all ration wich value is between [k;k+1] will have the same percentage than SnrDb_scaled_prcent_tab[k]*/
		//subdivide the loop in 4 part, to optimize average time of look process
		//well toussapoursa
		
		for(j=0;j<LOOKUP_SUBDIV;j++)
		{
			if(lq<=radio_SnrDb_scaled_ratio_tab[max[j]])
			{
				for(i=min[j];i<max[j];i++)
				{
					if(lq<radio_SnrDb_scaled_ratio_tab[i]) return radio_SnrDb_scaled_prcent_tab[i-1];
				}
				return radio_SnrDb_scaled_prcent_tab[max[j]];
			}
		}		
	}
	return radio_SnrDb_scaled_prcent_tab[max[LOOKUP_SUBDIV-1]];

}

//TODO: maybe I can split that in SEQ_REQ_ESTIMATE_NRJ and SEQ_REQ_ESTIMATE_IQ
static OSAL_void radio_get_rng_data_generic(rng_data_generic_t* rng)
{
	OSAL_u8 prpi=0;
	OSAL_u32 nrj_sum_fml=0;
	OSAL_u16 ti_offset=0;
	OSAL_s64 acc_med=0;
	OSAL_s64 acc_rms=0;
	OSAL_s64 acc_nrj=0;
	OSAL_u32 i;
	OSAL_u64 nrj_sum = 0;
	OSAL_u8 nrj_temp[MAX_CHANNEL_ESM*4];
	OSAL_u64 prp_valid;
	OSAL_u16 fgr_pos_tab[5]; //FG1, FG2, FG3, FG4, LE
#ifdef HYBRID_TOA
	OSAL_u32 idxMPW = 0, idxFAT = 0;
#endif // HYBRID_TOA

	pp_hw_burst_read(0,
			BAR_MEM_ESTIM_PRP, 0x00, 
			&(rng->prp[0]), MAX_CHANNEL_ESM);

	pp_hw_burst_read(0,
			BAR_MEM_NRJ, 0x00,
			&nrj_temp[0], MAX_CHANNEL_ESM*4);

	pp_hw_burst_read(0,
			BAR_CTRL_DIGITAL, BAR_IR_WR_FLAG_W0,
			(OSAL_u8*)&prp_valid, MAX_CHANNEL_ESM/8);

	pp_hw_burst_read(0,
			BAR_CTRL_DIGITAL, BAR_FNGR1_POS_W0,
			(OSAL_u8*)&fgr_pos_tab, sizeof(fgr_pos_tab));

	for(i = 0; i < MAX_CHANNEL_ESM; i++)
	{
		if((prp_valid & 0x1) == 0x0) // NRJ and PRP values were not updated, erase them
			rng->prp[i] = rng->nrj[i] = 0;
		else
		{
			rng->nrj[i] = spi_to_u32(&nrj_temp[4*i]);
			nrj_sum += rng->nrj[i];
			rng->prp[i] = rng->prp[i] & 0x7F;
		}

		prp_valid >>= 1;
	}

	rng->iwle  = (OSAL_u8)(fgr_pos_tab[4]&0x003f);
	rng->iwfg1 = (OSAL_u8)(fgr_pos_tab[0]&0x003f);	
	rng->rxqid_fg1 = rng->nrj[rng->iwfg1];
			
	// Fill RXQID data
	rng->iwfg2 = (OSAL_u8)(fgr_pos_tab[1]&0x003f);
	rng->iwfg3 = (OSAL_u8)(fgr_pos_tab[2]&0x003f);
	rng->iwfg4 = (OSAL_u8)(fgr_pos_tab[3]&0x003f);

	rng->rxqid_fg2 = rng->nrj[rng->iwfg2];
	rng->rxqid_fg3 = rng->nrj[rng->iwfg3];
	rng->rxqid_fg4 = rng->nrj[rng->iwfg4];

	//get link quality
	rng->link_quality = radio_get_SnrDbScaled(rng->rxqid_fg1,rng->sync_threshold);


#ifdef HYBRID_TOA
#define IW_SBW_FROM_MPW 160 // Search back 2.5 PRP maximum from max power to find FAT
#define ADJACENT_IW_NB 2  // Number of IW on right / left to take into FML NRJ sum

	// With HYBRID_TOA PP is configured to search FML, we have to search FAT manually
	idxMPW = rng->iwfg1 + 64 * rng->prp[rng->iwfg1];
	idxFAT = rng->iwfg1 + 64 * rng->prp[rng->iwfg1];
	rng->iwfat = rng->iwfg1;
	
	for(i = 0; i < MAX_CHANNEL_ESM; i++)
	{
		int curIdx = i + MAX_CHANNEL_ESM * rng->prp[i];

		if(	( curIdx < idxFAT ) &&
			( idxMPW - curIdx < IW_SBW_FROM_MPW ) &&
			( rng->nrj[i] > rng->rng_threshold ) )
		{
			OSAL_u32 idxPRE = (i == 0) ? (63 + 64 * rng->prp[63]) 	: i-1 + 64 * rng->prp[i-1];
			OSAL_u32 idxNXT = (i == 63) ? 64 * rng->prp[0] 	: i+1 + 64 * rng->prp[i+1];

			if((OSAL_s32)curIdx - (OSAL_s32)idxPRE == 1 || (OSAL_s32)idxNXT - (OSAL_s32)curIdx == 1)
			{
				idxFAT = curIdx;
				rng->iwfat = i;
			}
		}
	}

	// Calculate log2(nrj_sum_fml)
	nrj_sum_fml = rng->nrj[rng->iwle];
	
	for(i = 1; i <= ADJACENT_IW_NB; i++)
	{
		if(OSAL_ABS(rng->prp[rng->iwle-i] - rng->prp[rng->iwle]) <= 1)
			nrj_sum_fml += rng->nrj[rng->iwle-i];

		if(OSAL_ABS(rng->prp[rng->iwle+i] - rng->prp[rng->iwle]) <= 1)
			nrj_sum_fml += rng->nrj[rng->iwle+i];
	}
	

	nrj_sum_fml = floor_log2(nrj_sum_fml); //will be erased later
	//use toa_le_corr to store nrj_sum_fml
	rng->toa_le_corr = nrj_sum_fml;
#endif //HYBRID_TOA

	rng->nrj_sum = 0;
	rng->med = 0;
	rng->rms = 0;
	ti_offset = fgr_pos_tab[0];
	for(i=0;i<sizeof(fgr_pos_tab)/sizeof(fgr_pos_tab[0]);i++)
	{
		OSAL_u16 diff = ti_offset - (fgr_pos_tab[i]&0x1FFF);
		if(diff<64) ti_offset=fgr_pos_tab[i];
	}				
	for(i =0;i<EST_SIZE;i++)
	{
		prpi = rng->prp[0];
		if(prpi & 0x80)
		{
			OSAL_u16 ti= ((((prpi << 6) + i) - ti_offset) &0x1fff);
			OSAL_s64 ti_nrj= to_s64(to_s64(ti) * rng->nrj[i]);
			OSAL_s64 ti_square_nrj = ti * (OSAL_s64)ti_nrj;
			// compute MED
			acc_med += ti_nrj;            
			// compute RMSDS
			acc_rms += ti_square_nrj;
			acc_nrj += rng->nrj[i];
		}
	}
	rng->nrj_sum = acc_nrj;
	rng->med = acc_med;
	rng->rms = acc_rms;
}
		
//0:continue, non zero stop
OSAL_u16 radio_is_rx_finished(uwb_com_sysstatus_e* endStatus,OSAL_u8 IRQ0_Vector_W0, OSAL_u8 IRQ0_Vector_W1)
{
	OSAL_u8* buf=NULL;
	rng_data_generic_t* full = NULL;
	OSAL_u8 rx_framecfg =0;
	OSAL_u8 framing_cfg = 0;
		
	if( module_config[MOD_CFG_POS_RADIO_RX_ABORT] == 1)
	{
		//reset on abort
		*endStatus = UWB_STAT_RX_DONE;
		OSAL_trace(TRACE_INFO,"Abort");
		//use data to store status
		sys_set_shared_data_size(0,RADIO_STOPPED_ABORTED_BY_USER);
		//clear rx fifo
		pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);
		return RADIO_STOPPED_ABORTED_BY_USER;
	}
	if(IRQ0_Vector_W0&RADIO_VECT0_SFD_TOUT_MASK)    // SFd time out
	{
		config_phy_t* curPhy = get_current_config_phy(ctx.cfg_ctx);
		sys_get_rxframe_cfg(&rx_framecfg);		
		//get  output		
		pp_hw_reg_read(     0,  BAR_CFG_DIGITAL,    BAR_FRAMING_CFG,        &framing_cfg);
		sys_get_shared_data(0,&buf,NULL);
		
		full = (rng_data_generic_t*) &buf[0];
		//fill config phy part
		OSAL_memcpy((OSAL_u8*) &(full->startPhyConf),(OSAL_u8*) curPhy,sizeof(config_phy_t));
		radio_get_rng_data_generic(full);
		radio_lta_IR_management(&full->prp[0], &full->nrj[0]);;

		*endStatus = UWB_STAT_SFD_TIME_OUT;
		OSAL_trace(TRACE_DEBUG,"SFD Tout");
		//use data to store status
		sys_set_shared_data_size(0,RADIO_STOPPED_SFD_TOUT);
		//clear rx fifo
		pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);
		return RADIO_STOPPED_SFD_TOUT;
	}
	if(IRQ0_Vector_W1&RADIO_VECT1_PREAMBLE_TOUT_MASK)    // Preamble time out
	{
		*endStatus = UWB_STAT_PREAMBLE_TIMEOUT;
		OSAL_trace(TRACE_DEBUG,"PR Tout");
		//use data to store status
		sys_set_shared_data_size(0,RADIO_STOPPED_PREAMBLE_TOUT);
		//clear rx fifo
		pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);

		radio_lta_temp_management();

		return RADIO_STOPPED_PREAMBLE_TOUT;
	}
	if(IRQ0_Vector_W0&RADIO_VECT0_RX_DONE_MASK)
	{
		
		OSAL_u16 rx_buffSize = 0;
		OSAL_u16 fec_nb_err_bits=0;
		OSAL_u16 RX_ExpectedSizeInBits=0;
		OSAL_u16 RX_ExpectedSizeInBytes=0;
		OSAL_u8 RX_sizeInBytes=0;
		config_phy_t* curPhy = get_current_config_phy(ctx.cfg_ctx);
		
		sys_get_rxframe_cfg(&rx_framecfg);		
		//get  output		
		pp_hw_reg_read(     0,  BAR_CFG_DIGITAL,    BAR_FRAMING_CFG,        &framing_cfg);
		sys_get_shared_data(0,&buf,NULL);
		
		full = (rng_data_generic_t*) &buf[0];
		//fill config phy part
		OSAL_memcpy((OSAL_u8*) &(full->startPhyConf),(OSAL_u8*) curPhy,sizeof(config_phy_t));
		radio_get_rng_data_generic(full);
		radio_lta_IR_management(&full->prp[0], &full->nrj[0]);

		*endStatus = UWB_STAT_RX_DONE;
		//is PSDU_CRC Set in framing cfg ?
		if(framing_cfg&RADIO_FRAMING_CFG_PSDU_CRC_MASK)
		{
			if(!(IRQ0_Vector_W1&RADIO_VECT1_PDSU_CRC_OK_MASK))    // PDSU_CRC Not ok
			{
				OSAL_trace(TRACE_WARN,"PSDU CRC ERR");
				//use data to store status
				sys_set_shared_data_size(0,RADIO_STOPPED_PSDU_CRC_ERROR);
				//clear rx fifo
				pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);
				return RADIO_STOPPED_PSDU_CRC_ERROR;
			}
		}

		if(framing_cfg&RADIO_FRAMING_CFG_PHR_CRC_MASK)
		{
			if(!(IRQ0_Vector_W1&RADIO_VECT1_PHR_CRC_OK_MASK))    // PHR_CRC Not ok
			{
				OSAL_trace(TRACE_WARN,"PHR CRC ERR");
				//use data to store status
				sys_set_shared_data_size(0,RADIO_STOPPED_PHR_CRC_ERROR);
				//clear rx fifo
				pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);
				return RADIO_STOPPED_PHR_CRC_ERROR;
			}
		}
		pp_hw_reg16_read(   0,  BAR_CTRL_DIGITAL,   BAR_FEC_NB_ERROR_BITS_W0,&fec_nb_err_bits);
		if(fec_nb_err_bits>RADIO_FEC_ERROR_LIMITS)
		{
			OSAL_trace(TRACE_WARN,"FEC ERR");
			//use data to store status
			sys_set_shared_data_size(0,RADIO_STOPPED_FEC_ERROR);
			//clear rx fifo
			pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);
			return RADIO_STOPPED_FEC_ERROR;
		}
		if (rx_framecfg&SYS_RXDATA_ON_MASK)
		{
			pp_hw_reg_read(     0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_UW ,        &RX_sizeInBytes);
			MSBLSB_TO_U16VAL(	module_config[MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_MSB],
						module_config[MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_LSB],
						RX_ExpectedSizeInBits);
			RX_ExpectedSizeInBytes = ROUND_BITS_TO_BYTES(RX_ExpectedSizeInBits);
			//size differs regarding mode:
			switch(framing_cfg&RADIO_FRAMING_CFG_FRAMING_STYLE_MASK)
			{
				case RADIO_FRAMING_CFG_FRAMING_STYLE_LONG_PHR:RX_sizeInBytes-=2;break;
				case RADIO_FRAMING_CFG_FRAMING_STYLE_SHORT_PHR:RX_sizeInBytes-=1;break;
				default:break;
			}
			if(RX_sizeInBytes != RX_ExpectedSizeInBytes)
			{
				OSAL_trace(TRACE_WARN,"SIZE ERR(%d VS %d)",RX_sizeInBytes,RX_ExpectedSizeInBytes);
				//use data to store status
				sys_set_shared_data_size(0,RADIO_STOPPED_SIZE_MISMATCH);
				//clear rx fifo
				pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);
				return RADIO_STOPPED_SIZE_MISMATCH;
			}
		}
		
		//Config.RX_Toa = ON;
		if (rx_framecfg&SYS_TOA_ON_MASK)
		{
			
			radio_rx_process_toa(full);
			radio_clock_track_get_drift(full);

			if (rx_framecfg&SYS_FULL_RNG_ON_MASK )
			{
				//in full loc all bytes interess us
				rx_buffSize += sizeof(rng_data_generic_t);
			}
			else								
			{
				//in reduced loc, only the 32 first byte,s matching reduced_rng_data_generic_t interess us
				//rng_data_generic_t and reduced_rng_data_generic_t are almost the same expet for clock_drift and lqi position:
				reduced_rng_data_generic_t* red = (reduced_rng_data_generic_t*) &buf[0];
				red->clock_drift = full->clock_drift;
				red->link_quality = full->link_quality;
				rx_buffSize += sizeof(reduced_rng_data_generic_t);
			}			
		}
		
		if (rx_framecfg&SYS_RXDATA_ON_MASK)
		{
			
			U16VAL_TO_MSBLSB(RX_ExpectedSizeInBytes,buf[rx_buffSize],buf[rx_buffSize+1]);
			rx_buffSize+=2;
			//now get each bytes from output buffer
			switch(framing_cfg&RADIO_FRAMING_CFG_FRAMING_STYLE_MASK)
			{
				case RADIO_FRAMING_CFG_FRAMING_STYLE_LONG_PHR:
					//don't care the two first bytes
					pp_hw_reg_read(     0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_DATA , &buf[rx_buffSize]);
					pp_hw_reg_read(     0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_DATA , &buf[rx_buffSize]);
					break;
				case RADIO_FRAMING_CFG_FRAMING_STYLE_SHORT_PHR:
					//don't care the first byte
					pp_hw_reg_read(     0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_DATA , &buf[rx_buffSize]);
					break;
				default://case RADIO_FRAMING_CFG_FRAMING_STYLE_NO_PHR0=>2
					break;
			}
			pp_hw_burst_read(   0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_DATA , &buf[rx_buffSize]   , RX_ExpectedSizeInBytes);
			rx_buffSize += RX_ExpectedSizeInBytes;
		}
		sys_set_shared_data_size(0,rx_buffSize);
		sys_set_shared_data_checksum(0,rx_buffSize);
		//clear rx fifo
		OSAL_trace(TRACE_DEBUG,"RX DONE");
		pp_hw_reg_write(    0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_CLEAR,      0x01);
		return rx_buffSize;
	}
	return 0;//continue
}

OSAL_void radio_tx_end()
{
	/* finish in clean state */
	pp_hw_reg_write(    0,  BAR_CFG_DIGITAL,    BAR_PHY_CTRL_CMD,       0x00);    // Disable Tx/RX transmission
	radio_clear_IT();
	module_config[MOD_CFG_POS_RADIO_TX_ONGOING] = 0;
	module_config[MOD_CFG_POS_RADIO_TX_ABORT] = 0;
}

OSAL_void radio_rx_end()
{
	
	/* finish in clean state */
	pp_hw_reg_write(    0,  BAR_CFG_DIGITAL,    BAR_PHY_CTRL_CMD,       0x00);    // Disable Tx/RX transmission
	radio_clear_IT();

	module_config[MOD_CFG_POS_RADIO_RX_ONGOING] = 0;
	module_config[MOD_CFG_POS_RADIO_RX_ABORT] = 0;
}

//PROBLEM: how to do RX and TX simulataneously ????
OSAL_void radio_phy_ctrl_prepare(OSAL_u8 ctrl,OSAL_u16* rxOrTx)
{
	switch(ctrl)
	{
		case UWB_SC_START:
			if(*rxOrTx == UWB_SC_STARTSTOP_TX)//its TX
			{
				if(module_config[MOD_CFG_POS_RADIO_TX_ONGOING]==1)
					//already ongoing, do nothing on radio_phyctrl
					*rxOrTx=0;
			}
			else if(*rxOrTx == UWB_SC_STARTSTOP_RX)//its RX
			{
				if(module_config[MOD_CFG_POS_RADIO_RX_ONGOING]==1)
					//already ongoing, do nothing on radio_phyctrl
					*rxOrTx=0;
			}
			break;
		case UWB_SC_STOP://done on prepare
			if(*rxOrTx == UWB_SC_STARTSTOP_TX)//its TX
			{
				if(
					(module_config[MOD_CFG_POS_RADIO_TX_ONGOING]==0)||
					(module_config[MOD_CFG_POS_RADIO_TX_ABORT]==1)//otherwise abort is already ongoing
				)
					*rxOrTx=0;//dont do anything after this
			}
			else if(*rxOrTx == UWB_SC_STARTSTOP_RX)//its RX
			{
				if(
					(module_config[MOD_CFG_POS_RADIO_RX_ONGOING]==0)||
					(module_config[MOD_CFG_POS_RADIO_RX_ABORT]==1)//otherwise abort is already ongoing
				)
					*rxOrTx=0;//dont do anything after this
			}
			break;
		default:break;
	}
}

OSAL_void radio_phy_ctrl(OSAL_u8 ctrl, OSAL_u8 rxOrTx)
{
	switch(ctrl)
	{
		case UWB_SC_START:
			if(rxOrTx == UWB_SC_STARTSTOP_TX)//its TX
				radio_tx_start();
			else if(rxOrTx == UWB_SC_STARTSTOP_RX)//its RX
				radio_rx_start();
			break;
		case UWB_SC_STOP://will be stopped in radio_process, radio_tx, or radio_rx
			if(rxOrTx == UWB_SC_STARTSTOP_TX)//its TX
				module_config[MOD_CFG_POS_RADIO_TX_ABORT] = 1;
			else if(rxOrTx == UWB_SC_STARTSTOP_RX)//its RX
				module_config[MOD_CFG_POS_RADIO_RX_ABORT] = 1;
			break;
		default:break;
	}
}

OSAL_void radio_process()
{
	uwb_com_sysstatus_e endStatus = UWB_STAT_NONE;
	OSAL_u8 IRQ0_Vector_W0=0x00;
	OSAL_u8 IRQ0_Vector_W1=0x00;

	if(module_config[MOD_CFG_POS_RADIO_RX_ONGOING]==1)
	{
		pp_hw_reg_read(     0,  BAR_CTRL_SYSTEM,    BAR_INTR_VECT0_W0,      &IRQ0_Vector_W0);    // Read the IRQ0 Vector, common to tx/rx
		pp_hw_reg_read(     0,  BAR_CTRL_SYSTEM,    BAR_INTR_VECT0_W1,      &IRQ0_Vector_W1);    // Read the IRQ1 Vector
	}
	else if(module_config[MOD_CFG_POS_RADIO_TX_ONGOING]==1)
	{
		pp_hw_reg_read(     0,  BAR_CTRL_SYSTEM,    BAR_INTR_VECT0_W0,      &IRQ0_Vector_W0);    // Read the IRQ0 Vector, common to tx/rx
	}
	if(module_config[MOD_CFG_POS_RADIO_TX_ONGOING]==1)
	{
		if(radio_is_tx_finished(&endStatus,IRQ0_Vector_W0) != 0)
		{
			radio_tx_end();
			if(endStatus != UWB_STAT_NONE)
				sys_set_status(endStatus);
		}
	}
	if(module_config[MOD_CFG_POS_RADIO_RX_ONGOING]==1)
	{
		if(radio_is_rx_finished(&endStatus,IRQ0_Vector_W0,IRQ0_Vector_W1) != 0)
		{
			radio_rx_end();
			if(endStatus != UWB_STAT_NONE)
				sys_set_status(endStatus);
		}
	}
}
#endif //#ifdef CONFIG_GENERIC

OSAL_void radio_checkcache(OSAL_u8 checkId)
{
	module_config[MOD_CFG_POS_SYS_CHECKCACHE]=1;//rsvd id, or software things return 0, it necessarily ok
	switch(checkId)
	{
		default:break;
		case RADIO_GET_RX_CFG:
		case RADIO_SET_RX_CFG:{
			OSAL_u8 rx_cfg=0;
			OSAL_u16 rx_dataSizeInBitsCACHE=0;
			OSAL_u16 rx_dataSizeInBits=0;
			//check rx_dataSizeInBits
			radio_get_rx_cfg(&rx_cfg,&rx_dataSizeInBitsCACHE);
			pp_hw_reg16_read(  0,  BAR_CFG_DIGITAL,    BAR_FRAME_SIZE_W0,      &rx_dataSizeInBits);
			if(rx_dataSizeInBitsCACHE != rx_dataSizeInBits) module_config[MOD_CFG_POS_SYS_CHECKCACHE]=0;
			break;}
	}
}
