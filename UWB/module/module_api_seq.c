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

#ifdef CONFIG_GENERIC
OSAL_void seq_get_cfg_buffer(OSAL_u8** cfgBuf, OSAL_u16* cfgBufSize)
{
	*cfgBuf = &module_config[MOD_CFG_POS_SEQ_CFG0];
	*cfgBufSize = 3;
}

OSAL_void seq_get_sequence_data(OSAL_u8** seqBuf, OSAL_u16* seqBufSize)
{

	//use circular buffer of sequence
	OSAL_u8* bPos = &module_config[MOD_CFG_POS_SEQ_SEQ0_ACT];
	bPos +=(MOD_CFG_POS_SEQ_SEQ_SIZE*module_config[MOD_CFG_POS_SEQ_SEQPOSITION_WR]);
	*seqBuf =  bPos;
	module_config[MOD_CFG_POS_SEQ_SEQPOSITION_WR]=module_config[MOD_CFG_POS_SEQ_SEQPOSITION_WR]+1;
	module_config[MOD_CFG_POS_SEQ_SEQPOSITION_WR]&=0x3;//4buffer in fifo
	*seqBufSize =MOD_CFG_POS_SEQ_SEQ_SIZE;
}

OSAL_u8 seq_init()
{
	seq_ctrl(UWB_SC_STOP);
	sys_reset_status();
	return OSAL_OK;
}

OSAL_void seq_manage_hardFault()
{
}

OSAL_u8 seq_is_cfg_busy()
{
	return module_is_busy(MOD_CFG_POS_SEQ_BUSY,MOD_CFG_POS_SEQ_CFG_BUSY_MSK);
}

OSAL_void seq_set_cfg_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_SEQ_BUSY,MOD_CFG_POS_SEQ_CFG_BUSY_MSK);
}

OSAL_void seq_get_cfg(OSAL_u8* cfg0, OSAL_u8* cfg1, OSAL_u8* cfg2)
{
	*cfg0 = module_config[MOD_CFG_POS_SEQ_CFG0];
	*cfg1 = module_config[MOD_CFG_POS_SEQ_CFG1];
	*cfg2 = module_config[MOD_CFG_POS_SEQ_CFG2];
}

OSAL_void seq_set_cfg()
{
	pp_hw_reg_write(    0,  BAR_CFG_32K,    BAR_32K_TIMER_PERIOD_W0,    module_config[MOD_CFG_POS_SEQ_CFG0]);
	pp_hw_reg_write(    0,  BAR_CFG_32K,    BAR_32K_TIMER_PERIOD_W1,    module_config[MOD_CFG_POS_SEQ_CFG1]);
	pp_hw_reg_write(    0,  BAR_CFG_32K,    BAR_32K_FRAMERATE_LENGTH,   module_config[MOD_CFG_POS_SEQ_CFG2]);
}

OSAL_void seq_queue_single_act()
{
	OSAL_u8* seqBuf = &module_config[MOD_CFG_POS_SEQ_SEQ0_ACT];
	seqBuf +=(MOD_CFG_POS_SEQ_SEQ_SIZE*module_config[MOD_CFG_POS_SEQ_SEQPOSITION_RD]);
	module_config[MOD_CFG_POS_SEQ_SEQPOSITION_RD]=module_config[MOD_CFG_POS_SEQ_SEQPOSITION_RD]+1;
	module_config[MOD_CFG_POS_SEQ_SEQPOSITION_RD]&=0x3;//4buffer in fifo
	//TODO: TX FINE SHIFTIBG
	//if ((cmd_ctx->time.ntoa & 0x7F) < 0x40)
	//	pp_hw_reg_write(0, BAR_CFG_SYSTEM, BAR_DELAY_TUNE_TX_CMD, 0x04);
	//else
	//	pp_hw_reg_write(0, BAR_CFG_SYSTEM, BAR_DELAY_TUNE_TX_CMD, 0x06);
	//Temporary, force lsb bits to 0 in ntoa, no fine nto to 125ps
	seqBuf[5] &= 0x80;
	// MOD_CFG_POS_SEQ_SEQ_SIZE == 12, for last bytes to zero, whatever user asked, its reserved
	seqBuf[10] = 0x0;
	seqBuf[11] = 0x0;
	//OSAL_trace(TRACE_DEBUG,"Enq seq for %x",seqBuf[9]);
	pp_hw_burst_write(  0,  BAR_CFG_32K,    BAR_32K_M,  seqBuf, MOD_CFG_POS_SEQ_SEQ_SIZE);

}

OSAL_void seq_start()
{
	OSAL_u16 rx_dataSize=0;
	OSAL_u8 crtl_cfg = 0;
	OSAL_u8 rx_cfg = 0;
	OSAL_u16 IRQ_Conf =0xFFFF;
	OSAL_u16 SEQ_Conf =0xFFFF;
	OSAL_u8 mux_reg_ctrl_cfg = 0x00;
	OSAL_u8 framing_cfg=0;

	module_config[MOD_CFG_POS_SEQ_STATE] = 1;
	/* *********************** */
	//everything could be possible
	radio_get_rx_cfg(&rx_cfg,&rx_dataSize);
	IRQ_Conf &=RADIO_VECT0_MSK_RX_DONE;
	//IRQ_Conf &=RADIO_VECT0_MSK_RX_ACTIVE;
	IRQ_Conf &=RADIO_VECT0_MSK_TX_DONE;
	//IRQ_Conf &=RADIO_VECT0_MSK_TX_ACTIVE;
	IRQ_Conf &=RADIO_VECT0_MSK_ACTION_EVENT;

	crtl_cfg &= RADIO_CTRLCFG_SFD_TIME_OUT_OFF_MSK;
	crtl_cfg &= RADIO_CTRLCFG_PREAMB_TIME_OUT_OFF_MSK;
	crtl_cfg |= RADIO_CTRLCFG_SFD_TIME_OUT_BEHAVIOUR;
	crtl_cfg |= RADIO_CTRLCFG_PREAMB_TIME_OUT_BEHAVIOUR;
	radio_get_rx_cfg(&rx_cfg,&rx_dataSize);
	//read framing cfg
	pp_hw_reg_read(     0,  BAR_CFG_DIGITAL,    BAR_FRAMING_CFG,        &framing_cfg);
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

	pp_hw_reg_write(0,BNK_PP_REG |BAR_CFG_32K,  		BAR_32K_TIMER_CONFIG,  		0x00); // stop sequencer

	pp_hw_reg_read(       0,  BAR_CFG_MUX_REG,  BAR_MUX_REG_W33,    &mux_reg_ctrl_cfg);
	pp_hw_reg_write(      0,  BAR_CFG_MUX_REG,  BAR_MUX_REG_W33,    mux_reg_ctrl_cfg & ~0x10);
	pp_hw_reg_write(      0,  BAR_CFG_DIGITAL,  BAR_CTRL_CFG,       crtl_cfg);
	pp_hw_reg_write(      0,  BAR_CFG_MUX_REG,  BAR_MUX_REG_W33,    mux_reg_ctrl_cfg | 0x10);
	
	/* configure mask*/
	IRQ_Conf &= RADIO_VECT0_MSK_ACTION_EVENT;
	pp_hw_reg16_write(    0,  BAR_CFG_SYSTEM,   BAR_INTR_VECT0_MSK_W0, IRQ_Conf);

	//SEQ_Conf &= RADIO_VECT2_TIMER_32K_ACTION_MASK;
	SEQ_Conf &= RADIO_VECT2_TS_DELAY_EVENT_MASK;
	pp_hw_reg16_write(    0,  BAR_CFG_SYSTEM,   BAR_INTR_VECT2_MSK_W0, SEQ_Conf);
	

	pp_hw_reg_write(      0,  BAR_FIFO_TXRX,    BAR_FIFO_RX_CLEAR,  0x01);
	/*Clear IRQ register*/
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM, BAR_INTR_VECT0_CLR_W0, 0xFFFF);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM, BAR_INTR_VECT0_CLR_W0, 0x0000);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM, BAR_INTR_VECT2_CLR_W0, 0xFFFF);
	OSAL_udelay(100);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM, BAR_INTR_VECT2_CLR_W0, 0x0000);
	//TODO
	//pp_hw_reg_write(0,BNK_PP_REG |BAR_CFG_32K,  		BAR_32K_TIMER_CONFIG,  		0x33); // start sequencer
	pp_hw_reg_write(0,BNK_PP_REG |BAR_CFG_32K,  		BAR_32K_TIMER_CONFIG,  		0x3); // start sequencer
}

//return 0 if sequencer finish
OSAL_u8 seq_run()
{
	OSAL_u8 intr_vect[4]={0,0,0,0}; // store INTR_VECT0, INTR_VECT2
	OSAL_u16 doWeClear=0;
	uwb_com_sysstatus_e status = UWB_STAT_NONE;

	//abort by user
	if(module_config[MOD_CFG_POS_SEQ_STATE]==2)
	{
		sys_set_shared_data_size(0,RADIO_STOPPED_ABORTED_BY_USER);
		OSAL_trace(TRACE_INFO,"Abort");
		sys_set_status(UWB_STAT_RX_DONE);
		return 0;
	}
	if(module_config[MOD_CFG_POS_SEQ_STATE]==0)//should not happen, but mean we're stopped
		return 0;
	pp_hw_burst_read(   0,  BAR_CTRL_SYSTEM,BAR_INTR_VECT0_W0,      intr_vect, 4);
	//manage sequencer event
	//clear status, it ahs been treate
	if(intr_vect[3]&SEQ_TS_DELAY_EVENT_MASK)
	{
		OSAL_trace(TRACE_DEBUG,"[IT]:TS_DELAY");
		sys_set_status(UWB_STAT_SEQ_TS_DELAY);
	}
	doWeClear=intr_vect[2]+intr_vect[3];
	if(doWeClear != 0)
	{
		// We need to add a delay here to avoid doubled start_action with module
		// This can be due to the difference of clock between INTR_VECT and INTR_VECT2_CLR
		// (32kHz when clearing, 62.4MHz for the status register)		
		OSAL_udelay(100);
		pp_hw_reg16_write(0, BAR_CFG_SYSTEM, BAR_INTR_VECT2_CLR_W0, (OSAL_u16) (intr_vect[3]<<8)|intr_vect[2]);
		OSAL_udelay(100);
		pp_hw_reg16_write(0, BAR_CFG_SYSTEM, BAR_INTR_VECT2_CLR_W0, 0x0000); 
	} 
	///manage event start act
	if(intr_vect[1]&RADIO_VECT1_ACTION_EVENT_MASK)
	{
		OSAL_trace(TRACE_DEBUG,"[IT]:ACTION_START");
		sys_set_status(UWB_STAT_SEQ_ACTION_START);
	}
	//manage tx event
	if(radio_is_tx_finished(&status,intr_vect[0]) != 0)
	{
		if(status != UWB_STAT_NONE)
		{
			OSAL_trace(TRACE_DEBUG,"[IT]:%x",status);
			sys_set_status(status);
		}
	}

	//manage rx event
	if(radio_is_rx_finished(&status,intr_vect[0],intr_vect[1]) != 0)
	{
		if(status != UWB_STAT_NONE)
		{
			OSAL_trace(TRACE_DEBUG,"[IT]:%x",status);
			sys_set_status(status);
		}
	}
	else
	{
		if(RADIO_VECT1_PHR_CRC_OK_MASK&intr_vect[1])
		{
			//dont clean this IT now, it will be cleaned after radio_is_rx_finished return an IT
			intr_vect[1] &=~RADIO_VECT1_PHR_CRC_OK_MASK;
		}
		if(RADIO_VECT1_PDSU_CRC_OK_MASK&intr_vect[1])
		{
			//dont clean this IT now, it will be cleaned after radio_is_rx_finished return an IT
			intr_vect[1] &=~RADIO_VECT1_PDSU_CRC_OK_MASK;
		}
	}
	doWeClear=intr_vect[0]+intr_vect[1];
	if(doWeClear != 0)
	{
		OSAL_udelay(10);
		/*clear VECTOR 0 IRQ register */
		pp_hw_reg16_write(0, BAR_CFG_SYSTEM, BAR_INTR_VECT0_CLR_W0, (OSAL_u16) (intr_vect[1]<<8)|intr_vect[0]);
		pp_hw_reg16_write(0, BAR_CFG_SYSTEM, BAR_INTR_VECT0_CLR_W0, 0x0000);
	}
	return 1;
}


OSAL_void seq_stop()
{
	pp_hw_reg_write(    0,  BAR_CFG_32K,    BAR_32K_TIMER_CONFIG,   0x00);
	module_config[MOD_CFG_POS_SEQ_STATE] = 0;
	pp_hw_reg_write(    0, BAR_CFG_DIGITAL, BAR_PHY_CTRL_CMD,       0x00);// Disable Tx/RX transmission
	/*Clear IRQ register*/
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM, BAR_INTR_VECT0_CLR_W0, 0xFFFF);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM, BAR_INTR_VECT0_CLR_W0, 0x0000);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM, BAR_INTR_VECT2_CLR_W0, 0xFFFF);
	OSAL_udelay(100);
	pp_hw_reg16_write(0,BAR_CFG_SYSTEM, BAR_INTR_VECT2_CLR_W0, 0x0000);
	pp_hw_reg_write(0,BAR_FIFO_TXRX, 	BAR_FIFO_TX_CLEAR,	0x01);
	pp_hw_reg_write(0,BAR_FIFO_TXRX, 	BAR_FIFO_RX_CLEAR,	0x01);
}

OSAL_void seq_ctrl_prepare(OSAL_u8* ctrl)
{
	switch(*ctrl)
	{
		case UWB_SC_START:
			if(module_config[MOD_CFG_POS_SEQ_STATE]==1)
				//already ongoing, do nothing on radio_phyctrl
				*ctrl=UWB_SC_NONE;
			break;
		case UWB_SC_STOP://done on prepare
			if(module_config[MOD_CFG_POS_SEQ_STATE]!=1)
				*ctrl=UWB_SC_NONE;//dont do anything after this
			break;
	}
}

OSAL_void seq_ctrl(OSAL_u8 ctrl)
{
	switch(ctrl)
	{
		case UWB_SC_START:
			seq_start();
			OSAL_trace(TRACE_DEBUG,"UWB_STAT_SEQ_START");
			sys_set_status(UWB_STAT_SEQ_START);
			break;
		case UWB_SC_STOP://will be stopped in radio_process, radio_tx, or radio_rx
			if(module_config[MOD_CFG_POS_SEQ_STATE] != 0)
				module_config[MOD_CFG_POS_SEQ_STATE] = 2;
			break;
	}
}

OSAL_void seq_process()
{
	//sequencer ongoing
	if(module_config[MOD_CFG_POS_SEQ_STATE]>=1)
	{
		if(seq_run() == 0)
			seq_stop();
	}
}

OSAL_void seq_checkcache(OSAL_u8 checkId)
{
	module_config[MOD_CFG_POS_SYS_CHECKCACHE]=1;//rsvd id, or software things return 0, it necessarily ok
	switch(checkId)
	{
		default:break;
		case SEQ_GET_CFG:
		case SEQ_SET_CFG:{
			OSAL_u16 i;
			OSAL_u8 cfgCACHE[3];
			OSAL_u8 cfg[3];
			seq_get_cfg(&cfgCACHE[0],&cfgCACHE[1],&cfgCACHE[2]);
			pp_hw_reg_read(    0,  BAR_CFG_32K,    BAR_32K_TIMER_PERIOD_W0,    &cfg[0]);
			pp_hw_reg_read(    0,  BAR_CFG_32K,    BAR_32K_TIMER_PERIOD_W1,    &cfg[1]);
			pp_hw_reg_read(    0,  BAR_CFG_32K,    BAR_32K_FRAMERATE_LENGTH,   &cfg[2]);
			for(i=0;i<2;i++)
			{
				if(cfg[i] != cfgCACHE[i])
				{
					module_config[MOD_CFG_POS_SYS_CHECKCACHE]=0;
					break;
				}
	
			}
			break;}
		case SEQ_QUEUE_SINGLE_ACT:{
			OSAL_u8* seqBufCache = &module_config[MOD_CFG_POS_SEQ_SEQ0_ACT];
			OSAL_u8 seqi;
			OSAL_u16 i;
			seqBufCache +=(MOD_CFG_POS_SEQ_SEQ_SIZE*module_config[MOD_CFG_POS_SEQ_SEQPOSITION_WR]);
			for(i=0;i<12;i++)
			{
				pp_hw_reg_read(  0,  BAR_CFG_32K,    BAR_32K_M+i, &seqi);
				if(seqBufCache[i] != seqi)
				{
					module_config[MOD_CFG_POS_SYS_CHECKCACHE]=0;
					break;
				}	
			}
			break;}
	}
}

#endif //#ifdef CONFIG_GENERIC


