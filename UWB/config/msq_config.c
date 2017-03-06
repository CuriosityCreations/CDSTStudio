/**
 * @file msq_config.c
 * @brief this part is usefull to configure ,load firmware, start and make a load of
 *	configuration for the integrated msp430 MSQ	
 * @author gpa@bespoon.com
 * @date 01/02/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include <types.h>
#include <osal_comp.h>
#include <osal_stdlib.h>
#include <pinpointer_regs.h>
#include <pinpointer_hw_api.h>
#include <config_api.h>
#include <msq_config_api.h>
#include "msq_config.h"

OSAL_u8 host_irq_cnt=0;

OSAL_void msq_init(OSAL_void* ctx)
{
	OSAL_u8 cfg_msq_irq_xbar[] = {0x10, 0x32, 0x54};
#ifdef CONFIG_HW_METRIC
	OSAL_u8 cfg_msq_irq_msk[] = {0x4D, 0xFB};
#else
	OSAL_u8 cfg_msq_irq_msk[] = {0x4F, 0xFB};
#endif // CONFIG_HW_METRIC

	//clock selection CLK_DIVA_1X
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_SYSTEM, BAR_CLK_MSQ_SRC_SEL, 0x09);

	//Enable MSQ access to PMEM/DMEM (BAR_CFG_MSQ/BAR_MSQ_MEM_SEL)
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_MSQ_MEM_SEL, 0x01);

	//Configure MSQ interrupt crossbar (vect0 <=> irq0) (BAR_CFG_MSQ/BAR_TRIG_SELECTION_W0...)
	pp_hw_burst_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_TRIG_SELECTION_W0, &cfg_msq_irq_xbar[0], OSAL_ARRAY_SIZE(cfg_msq_irq_xbar));

#ifdef CONFIG_HW_METRIC
	//Enable MSQ interrupts (synchro_estim_rdy, rx_active, rx_eof, preamb_timeout, sfd_timeout) (BAR_CFG_MSQ/BAR_TRIG_VECT0_MSK_W0)
	pp_hw_burst_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_TRIG_VECT0_MSK_W0, &cfg_msq_irq_msk[0], OSAL_ARRAY_SIZE(cfg_msq_irq_msk));
#else
	//Enable MSQ interrupts (rx_active,rx_eof, preamble/sfd timeouts) (BAR_CFG_MSQ/BAR_TRIG_VECT0_MSK_W0)
	pp_hw_burst_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_TRIG_VECT0_MSK_W0, &cfg_msq_irq_msk[0], OSAL_ARRAY_SIZE(cfg_msq_irq_msk));
#endif // CONFIG_HW_METRIC

	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_TRIG_VECT4_MSK_W0, 0x00);

	// Set MSQ low frequency
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_TRIG_TABL, BAR_TRIG_TABL_W18, CLK_DIVA_LOW);

	// Set MSQ high frequency
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_TRIG_TABL, BAR_TRIG_TABL_W19, CLK_DIVA_HIGH);
}

OSAL_void msq_mem_init(OSAL_void* ctx)
{
	OSAL_u8 cfg_msq_dmem[] = {0x00, 0x00};
	OSAL_u8 cfg_msq_pmem[] = {0x00, 0x04};

	//Configure DMEM address offset (BAR_CFG_MSQ/BAR_MSQ_DMEM_OFFSET_W0...)
	pp_hw_burst_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_MSQ_DMEM_OFFSET_W0, &cfg_msq_dmem[0], OSAL_ARRAY_SIZE(cfg_msq_dmem));

	//Configure PMEM address offset (BAR_CFG_MSQ/BAR_MSQ_PMEM_OFFSET_W0...)
	pp_hw_burst_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_MSQ_PMEM_OFFSET_W0, &cfg_msq_pmem[0], OSAL_ARRAY_SIZE(cfg_msq_pmem));
}

OSAL_void msq_ld_fw(OSAL_void* ctx, OSAL_u8 OSAL_PROD_SECT * fw, OSAL_u16 fw_size)
{
	// give SPI acces to HOST to write PMEM and DMEM
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_MSQ_MEM_SEL, 0x00);
	
	//Download MSQ firmware to PMEM (BNK_MSQ_PMEM)
	pp_hw_prod_burst_write(ctx, BNK_MSQ_PMEM | 0x00, 0x00, fw, fw_size); 
}

OSAL_void msq_ld_seq(OSAL_void* ctx)
{
	msq_context_t msq_ctx;

	OSAL_u32 i, seq_en_msk=0;
	OSAL_u8 cfg_trg_cfg[] = {0x00, 0x00, //VECT0: none
                                 0x00, 0x00, //VECT1: none
                                 0x00, 0x00, //VECT2: none
                                 0x00, 0x00, //VECT3: none
                                 0x01, 0x00};//VECT4: HOST_REQUEST enable

	OSAL_u8 cfg_trg_seq_addr[] = {0x00, 0x80, // VECT0
                                      0x00, 0x80, // VECT1
                                      0x00, 0x80, // VECT2
                                      0x00, 0x80, // VECT3
                                      0x00, 0x80};// VECT4

        OSAL_u8 cfg_seq_tab_addr[] = {0x00, 0x00};
        OSAL_u8 cfg_trig_seq_tab_vect4_0[] = {0x00, 0x00, 0x00, 0x00}; //enable seq0-seq14

        OSAL_u8 cfg_sq_en[] = {	0x00, 0x00, //EN_TRIG (VECT0)
				0x00, 0x00, //EN_TRIG (VECT1)
				0x00, 0x00, //EN_TRIG (VECT2)
				0x00, 0x00, //EN_TRIG (VECT3)
				0x01, 0x00, //EN_TRIG (VECT4)
				0x00, 0x00, 0x00, 0x00, // EN_SEQ
				0x00, 0x00, 0x00, 0x00};// BP_SHADOW (all sequences)

	OSAL_u8 sq_cp_spi_msq[] = { 	SEQ_MEMCPY, 
					0x00, (((MSQ_BNK>>8)&0xFF)|BAR_CFG_ANALOG),
					0x00, (((SPI_BNK>>8)&0xFF)|BAR_CFG_ANALOG),  
					128, //can't do a memcpy with size of 256 (max 255)
					SEQ_MEMCPY, 
					0x00|127, (((MSQ_BNK>>8)&0xFF)|BAR_CFG_ANALOG),
					0x00|127, (((SPI_BNK>>8)&0xFF)|BAR_CFG_ANALOG),  
					128,
					SEQ_MEMCPY, 
					0x00, (((MSQ_BNK>>8)&0xFF)|BAR_CFG_DIGITAL), 
					0x00, (((SPI_BNK>>8)&0xFF)|BAR_CFG_DIGITAL), 
					128, 
					SEQ_MEMCPY,  
					0x00, (((MSQ_BNK>>8)&0xFF)|BAR_CFG_SYSTEM),
					0x00, (((SPI_BNK>>8)&0xFF)|BAR_CFG_SYSTEM),
					128, 
					SEQ_END };

	msq_ctx.DMEM_Addr_p = (SQ_ADDRTAB_ADDR + 2); // first availlable addr in DMEM
	OSAL_memset((OSAL_u8*)&(msq_ctx.Seq_Addr_tab), 0x00, OSAL_ARRAY_SIZE(msq_ctx.Seq_Addr_tab)*sizeof(msq_ctx.Seq_Addr_tab[0]));

	// give SPI acces to HOST to write PMEM and DMEM
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_MSQ_MEM_SEL, 0x00);
	
	//clear DMEM
	for(i=0;i<DMEM_SEQ_SPACE;i++)
		pp_hw_reg_write(ctx, BNK_MSQ_DMEM | ((i>>8)&0xFF), i&0xFF, 0x00);

	// Create the enable sequence mask according to the sequences numbers (for BP_SHADOW and Enable sequences array)
	for(i=0;i<MSQ_SEQUENCES_NB;i++)
		seq_en_msk |= (1<<i);
	OSAL_memcpy((OSAL_u8*)cfg_trig_seq_tab_vect4_0,(OSAL_u8*)&seq_en_msk, sizeof(OSAL_u32));
	OSAL_memcpy((OSAL_u8*)&cfg_sq_en[SQ_SHW_BYPASS],(OSAL_u8*)&seq_en_msk, sizeof(OSAL_u32));

	//fill addr of trig/vect association and the value for vector 4
	cfg_trg_seq_addr[VECT4_W0] = (msq_ctx.DMEM_Addr_p & 0xFF);
	cfg_trg_seq_addr[VECT4_W1] = ((DMEM>>8)&0xFF) | ((msq_ctx.DMEM_Addr_p>>8) & 0xFF);
	pp_hw_burst_write(ctx, BNK_MSQ_DMEM | ((OSAL_u8)(msq_ctx.DMEM_Addr_p>>8) & 0x1F), (OSAL_u8)msq_ctx.DMEM_Addr_p & 0xFF, cfg_trig_seq_tab_vect4_0, OSAL_ARRAY_SIZE(cfg_trig_seq_tab_vect4_0));
	
	msq_ctx.DMEM_Addr_p += (NB_ACTIVATED_TRIGGER/*cfg_trg_cfg*/ *4/*32bit*/);
	
	//sequences table start address
	cfg_seq_tab_addr[0] = (msq_ctx.DMEM_Addr_p & 0xFF);
	cfg_seq_tab_addr[1] = ((DMEM>>8)&0xFF) | ((msq_ctx.DMEM_Addr_p>>8) & 0xFF);
	msq_ctx.Seq_Addr_tab_start = DMEM | ((msq_ctx.DMEM_Addr_p>>8) & 0xFF) | (msq_ctx.DMEM_Addr_p & 0xFF);

	//leave space for sequence address table before write sequence
	msq_ctx.DMEM_Addr_p += (MSQ_SEQUENCES_NB * 2); // addr 16bits

	// trigger events configured in memory
	pp_hw_burst_write(ctx, BNK_MSQ_DMEM | ((OSAL_u8)(TRG_CFG_0>>8) & 0x1F), (OSAL_u8)TRG_CFG_0 & 0xFF, cfg_trg_cfg, OSAL_ARRAY_SIZE(cfg_trg_cfg));
	// location of trig/seq association in dmem
	pp_hw_burst_write(ctx, BNK_MSQ_DMEM | ((OSAL_u8)(TRG_SQ_ADDR_0>>8) & 0x1F), (OSAL_u8)TRG_SQ_ADDR_0 & 0xFF, cfg_trg_seq_addr, OSAL_ARRAY_SIZE(cfg_trg_seq_addr));
	// location of sequence address table
	pp_hw_burst_write(ctx, BNK_MSQ_DMEM | ((OSAL_u8)(SQ_ADDRTAB_ADDR>>8) & 0x1F), (OSAL_u8)SQ_ADDRTAB_ADDR & 0xFF, cfg_seq_tab_addr, OSAL_ARRAY_SIZE(cfg_seq_tab_addr));

	//write sequences and fill sequence address table associated
	msq_write_pm_seq(ctx, &msq_ctx, PM_2, 		PM_0, 		SEQ_PM2_PM0);
	msq_write_pm_seq(ctx, &msq_ctx, PM_0, 		PM_STDBY, 	SEQ_PM0_STDBY);
	msq_write_pm_seq(ctx, &msq_ctx, PM_STDBY, 	PM_RX_IDLE, 	SEQ_STDBY_RXIDLE);
	msq_write_pm_seq(ctx, &msq_ctx, PM_STDBY, 	PM_TX_IDLE, 	SEQ_STDBY_TXIDLE);
	msq_write_pm_seq(ctx, &msq_ctx, PM_RX_IDLE, 	PM_STDBY, 	SEQ_RXIDLE_STDBY);
	msq_write_pm_seq(ctx, &msq_ctx, PM_TX_IDLE, 	PM_STDBY, 	SEQ_TXIDLE_STDBY);
	msq_write_pm_seq(ctx, &msq_ctx, PM_STDBY, 	PM_0, 		SEQ_STDBY_PM0);
	msq_write_pm_seq(ctx, &msq_ctx, PM_0, 		PM_2, 		SEQ_PM0_PM2);

	msq_write_pm_seq(ctx, &msq_ctx, PM_STDBY, 	PM_TXRX_IDLE, 	SEQ_STDBY_TXRXIDLE);
	msq_write_pm_seq(ctx, &msq_ctx, PM_TXRX_IDLE, 	PM_STDBY, 	SEQ_TXRXIDLE_STDBY);

	//SEQ COPY SPI->MSQ
	msq_write_raw_seq(ctx, &msq_ctx, sq_cp_spi_msq, OSAL_ARRAY_SIZE(sq_cp_spi_msq), SEQ_SPI_MSQ_BNK_CPY);

	pp_hw_burst_write(ctx, BNK_PP_REG | BAR_TRIG_TABL, BAR_TRIG_TABL_W00, &cfg_sq_en[0],OSAL_ARRAY_SIZE(cfg_sq_en));
}


#define MSQ_BUSY_TIME_OUT to_u16(100)

/*Wait for MSQ to be ready, return true in case of timeout*/
OSAL_bool_t wait_for_msq_ready_timeout(OSAL_void* ctx, OSAL_u16 timeout_msec)
{
	OSAL_u32 timeout_usec = timeout_msec * 1000;
	OSAL_udelay(20); // This avoid random loop in wait_for_msq_ready_timeout

	/* Timeout is, for sure a multiple of 100 usec*/
	while (	(msq_get_state(ctx) == BUSY) && (timeout_usec != 0))
	{
		OSAL_udelay(100);
		timeout_usec -= 100;
	}

	if(timeout_usec == 0)
	{
		OSAL_trace(TRACE_ERROR, "MSQ T/OUT BUSY");
		return OSAL_true;
	}
	else
		return OSAL_false;
}

OSAL_void msq_start(OSAL_void* ctx)
{
	OSAL_u8 host_irq_cnt_tmp = 0;

	// enable msq & disable reset
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_RUN_MSQ, 0x07);

	//read msq counter
	pp_hw_reg_read(ctx, BNK_MSQ_REG | BAR_STAT_TABL, BAR_STAT_TABL_W27, &host_irq_cnt_tmp);

	//enable sequence
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_HOST_TRIG_W0, SEQ_SPI_MSQ_BNK_CPY);

	//check if msq missed sequence
	pp_hw_reg_read(ctx, BNK_MSQ_REG | BAR_STAT_TABL, BAR_STAT_TABL_W27, &host_irq_cnt);

	// Test if msq missed sequence, but we should not go there
	if(host_irq_cnt_tmp == host_irq_cnt)
	{
		while(host_irq_cnt_tmp == host_irq_cnt)
		{
			pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_HOST_TRIG_W0, SEQ_SPI_MSQ_BNK_CPY);//enable sequence
			pp_hw_reg_read(ctx, BNK_MSQ_REG | BAR_STAT_TABL, BAR_STAT_TABL_W27, &host_irq_cnt);

			if(wait_for_msq_ready_timeout(ctx, MSQ_BUSY_TIME_OUT) == OSAL_true)
				break;
		}
	}

	wait_for_msq_ready_timeout(ctx, MSQ_BUSY_TIME_OUT);
}

//WORKAROUND TO PREVENT LOST IRQ 
//WARNING: TO USE EACH TIME WE TRIG SEQUENCE
OSAL_void msq_seq_write(OSAL_void* ctx, struct setting OSAL_PROD_SECT * pm_tab)
{
	OSAL_u8 msq_irq_cnt;

	if(wait_for_msq_ready_timeout(ctx, MSQ_BUSY_TIME_OUT) == OSAL_true)
		return;

	// Write pm
	pp_hw_prod_seq_write(ctx, pm_tab);
	host_irq_cnt++;

	// Test if msq missed a pm, but we should not go there
	pp_hw_reg_read(ctx, BNK_MSQ_REG | BAR_STAT_TABL, BAR_STAT_TABL_W27, &msq_irq_cnt);

	if((msq_irq_cnt < host_irq_cnt) || (msq_irq_cnt==255 && host_irq_cnt==0))
	{
		while(msq_irq_cnt != host_irq_cnt)
		{
			pp_hw_prod_seq_write(ctx, pm_tab);
			pp_hw_reg_read(ctx, BNK_MSQ_REG | BAR_STAT_TABL, BAR_STAT_TABL_W27, &msq_irq_cnt);

			if(wait_for_msq_ready_timeout(ctx, MSQ_BUSY_TIME_OUT) == OSAL_true)
				break;
		}
	}
	else if(msq_irq_cnt > host_irq_cnt)
	{
		//just in case should not happen
		host_irq_cnt = msq_irq_cnt;
	}
}

//ONLY AVAILLAIBLE SINCE FIRMWARE 04 
OSAL_bool_t msq_run_test(OSAL_void* ctx)
{
	OSAL_u8 run_test[2];

	pp_hw_reg_read(ctx, BNK_MSQ_REG | BAR_STAT_TABL, MSQ_RUN_TEST, &run_test[0]);
	OSAL_msleep(1);
	pp_hw_reg_read(ctx, BNK_MSQ_REG | BAR_STAT_TABL, MSQ_RUN_TEST, &run_test[1]);
	if(run_test[0] == run_test[1])
	{
		OSAL_trace(TRACE_ERROR, "MSQ START FAIL");
		return OSAL_false;
	}
	else
	{
		OSAL_trace(TRACE_INFO, "MSQ STARTED");
		return OSAL_true;
	}
}

msq_status msq_get_state(OSAL_void* ctx)
{
	OSAL_u16 status;
	pp_hw_reg16_read(ctx, BNK_MSQ_REG | BAR_STAT_TABL, MSQ_STATUS_BUSY, &status);
	return (msq_status)status;
}

OSAL_void msq_stop(OSAL_void* ctx)
{
	// disable msq
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MSQ, BAR_RUN_MSQ, 0x00);
}

OSAL_void msq_mux_cfg(OSAL_void* ctx, OSAL_bool_t pm_msq, OSAL_bool_t enable_pm)
{
	//See in pm_config.h to know which bits are anabled
	//KEEP ME ALIGNED PLEASE !!!!
	//Decimal organisation to respect pinpointer documentation
	OSAL_u8 mux_reg_val[] = {
	/*00*/	0xC0, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0x00, 0x00, 0xF8, 0x0F, 
	/*10*/	0xFF, 0x03, 0xFC, 0xF1, 0xFF, 0xF3, 0x87, 0x7F, 0x8F, 0xF9,  
	/*20*/	0x07, 0x44, 0xF0, 0xFF, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00,
	/*30*/	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 
	/*40*/	0x00, 0xE9, 0x10, 0x44, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
	/*50*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	/*60*/	0x00, 0x00, 0x00, 0x00,
	};

	if(enable_pm == OSAL_false)
	{
		//Provide HOST control over BAR_RXFE_SP_TXRXDEP
		mux_reg_val[10] &= 0xF0;

		//Provide HOST control over BAR_DIO_TA_VAL
		mux_reg_val[43] &= 0xFB;
	}

	if(pm_msq == OSAL_true)
	{
		// provide MSQ control over register needed for PM management
		// TODO: separate each burst to win space on IAR
		pp_hw_burst_write(ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W00, mux_reg_val, OSAL_ARRAY_SIZE(mux_reg_val));

		//note for futur integration:
		//when pm_handle by msq disable usage of DIO_TA_VAL by protocol
	}

#ifdef CONFIG_HW_METRIC
	//Provide MSQ control over energy memories read register (BAR_CFG_MUX_REG/BAR_MUX_REG_W37)
	pp_hw_reg_read(ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W37, &tmp);
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W37, tmp | 0x01);
#endif  // CONFIG_HW_METRIC
}

OSAL_s8 msq_write_eos(OSAL_void* ctx, msq_context_t* msq_ctx)
{
	OSAL_s8 status;
	status = pp_hw_reg_write(ctx, BNK_MSQ_DMEM | (OSAL_u8)((msq_ctx->DMEM_Addr_p >> 8)&0xFF), (OSAL_u8)(msq_ctx->DMEM_Addr_p & 0xFF), (OSAL_u8)SEQ_END);
	(msq_ctx->DMEM_Addr_p)++;

	return status;
}

OSAL_s8 msq_write_raw_seq(OSAL_void* ctx, msq_context_t* msq_ctx, OSAL_u8* raw_seq, OSAL_u16 size, msq_pm_seq seq_nb)
{
	if(msq_ctx->Seq_Addr_tab[seq_nb] == 0x0000)
	{
		msq_ctx->Seq_Addr_tab[seq_nb] = msq_ctx->Seq_Addr_tab_start + (2*seq_nb);

		pp_hw_reg16_write(ctx, 	BNK_MSQ_DMEM | ((OSAL_u8)(msq_ctx->Seq_Addr_tab[seq_nb]>>8) & 0x1F), //BANK
					(OSAL_u8)(msq_ctx->Seq_Addr_tab[seq_nb] & 0xFF), //REG
					msq_ctx->DMEM_Addr_p|DMEM); //VALUE

		msq_ctx->Seq_Addr_tab[seq_nb] = msq_ctx->DMEM_Addr_p|DMEM;
	}
	else
	{
		OSAL_trace(TRACE_ERROR, "TRY TO OVERWRITE SEQ ADDR %d ALREADY USED",seq_nb);
		return -1;
	}

	if(((msq_ctx->DMEM_Addr_p) + size) < DMEM_SEQ_SPACE)
	{
		pp_hw_burst_write(ctx, BNK_MSQ_DMEM | ((OSAL_u8)(msq_ctx->DMEM_Addr_p>>8) & 0x1F), (OSAL_u8)(msq_ctx->DMEM_Addr_p & 0xFF), raw_seq, size);
		msq_ctx->DMEM_Addr_p += size;
	}
	else
	{
		OSAL_trace(TRACE_ERROR,"TRY TO WRITE ILLEGAL DMEM ADDR");
		return -1;
	}

	return 0;
}

OSAL_s8 msq_write_pm_seq(OSAL_void* ctx, msq_context_t* msq_ctx, pm_state_t src_pm, pm_state_t dest_pm, msq_pm_seq seq_nb)
{
	struct setting OSAL_PROD_SECT * pm_tab = NULL;
	
	if(msq_ctx->Seq_Addr_tab[seq_nb] == 0x0000)
	{
		msq_ctx->Seq_Addr_tab[seq_nb] = msq_ctx->Seq_Addr_tab_start + (2*seq_nb);

		pp_hw_reg16_write(ctx, 	BNK_MSQ_DMEM | ((OSAL_u8)(msq_ctx->Seq_Addr_tab[seq_nb]>>8) & 0x1F), //BANK
					(OSAL_u8)(msq_ctx->Seq_Addr_tab[seq_nb] & 0xFF), //REG
					msq_ctx->DMEM_Addr_p|DMEM); //VALUE

		msq_ctx->Seq_Addr_tab[seq_nb] = msq_ctx->DMEM_Addr_p|DMEM;
	}
	else
	{
		OSAL_trace(TRACE_ERROR, "TRY TO OVERWRITE SEQ ADDR %d ALREADY USED",seq_nb);
		return -1;
	}

	while (src_pm != dest_pm)
	{
		src_pm = get_config_pm(src_pm, dest_pm, &pm_tab);

		if ( pm_tab != NULL )
		{
			msq_write_pm(ctx, msq_ctx, pm_tab);
		}
		else
		{
			OSAL_trace(TRACE_ERROR, "ERROR GETTING DMEM SEQ");
			return -1;
		}
	}
	msq_write_eos(ctx, msq_ctx);

	return 0;
}

OSAL_s8 msq_write_pm(OSAL_void* ctx, msq_context_t* msq_ctx, struct setting OSAL_PROD_SECT * tab)
{
	struct setting OSAL_PROD_SECT* command;
	OSAL_u8 status = 0,i;

	seq_regset_fmt_t reg_cmd;
	seq_delay_fmt_t	delay_cmd;

	for(i = 0; tab[i].Type != T_STOP; i++)
	{
		command = &tab[i];
		switch(command->Type)
		{
			case T_REG8 :
				reg_cmd.op_code = SEQ_REGSET;	
				reg_cmd.dest_LSB = command->Reg;
				reg_cmd.dest_MSB = (((MSQ_BNK>>8)&0xFF) | command->Bank);
				reg_cmd.value = command->Val.t8;

				if(((msq_ctx->DMEM_Addr_p) + sizeof(reg_cmd)) < DMEM_SEQ_SPACE)
				{
					status += pp_hw_burst_write(ctx, BNK_MSQ_DMEM | (OSAL_u8)((msq_ctx->DMEM_Addr_p>>8)&0xFF), (OSAL_u8)(msq_ctx->DMEM_Addr_p&0xFF), (OSAL_u8*)&reg_cmd, sizeof(reg_cmd));
					msq_ctx->DMEM_Addr_p += sizeof(reg_cmd);
				}
				else
				{
					OSAL_trace(TRACE_ERROR,"TRY TO WRITE ILLEGAL DMEM ADDR");
					return -1;
				}
				break;

			case T_REG16 :
				reg_cmd.op_code = SEQ_REGSET;	
				reg_cmd.dest_LSB = command->Reg;
				reg_cmd.dest_MSB = (((MSQ_BNK>>8)&0xFF) | command->Bank);
				reg_cmd.value = command->Val.t16 & 0xFF;

				if(((msq_ctx->DMEM_Addr_p) + sizeof(reg_cmd)) < DMEM_SEQ_SPACE)
				{
					status += pp_hw_burst_write(ctx, BNK_MSQ_DMEM | (OSAL_u8)((msq_ctx->DMEM_Addr_p>>8)&0xFF), (OSAL_u8)(msq_ctx->DMEM_Addr_p&0xFF), (OSAL_u8*)&reg_cmd, sizeof(reg_cmd));
					msq_ctx->DMEM_Addr_p += sizeof(reg_cmd);
				}
				else
				{
					OSAL_trace(TRACE_ERROR,"TRY TO WRITE ILLEGAL DMEM ADDR %d", (msq_ctx->DMEM_Addr_p) + sizeof(reg_cmd));
					return -1;
				}

				reg_cmd.value = (command->Val.t16 >> 8) & 0xFF;

				if(((msq_ctx->DMEM_Addr_p) + sizeof(reg_cmd)) < DMEM_SEQ_SPACE)
				{
					status += pp_hw_burst_write(ctx, BNK_MSQ_DMEM | (OSAL_u8)((msq_ctx->DMEM_Addr_p>>8)&0xFF), (OSAL_u8)(msq_ctx->DMEM_Addr_p&0xFF), (OSAL_u8*)&reg_cmd, sizeof(reg_cmd));
					msq_ctx->DMEM_Addr_p += sizeof(reg_cmd);
				}
				else
				{
					OSAL_trace(TRACE_ERROR,"TRY TO WRITE ILLEGAL DMEM ADDR");
					return -1;
				}
				break;

			case T_DELAY :
				delay_cmd.op_code = SEQ_DELAY;	
				delay_cmd.value_LSB = (OSAL_u8)(MSP_DELAY(command->Val.tdelay)&0xFF);
				delay_cmd.value_MSB = (OSAL_u8)((MSP_DELAY(command->Val.tdelay)>>8)&0xFF);
				if(((msq_ctx->DMEM_Addr_p) + sizeof(delay_cmd)) < DMEM_SEQ_SPACE)
					status += pp_hw_burst_write(ctx, BNK_MSQ_DMEM | (OSAL_u8)((msq_ctx->DMEM_Addr_p>>8)&0xFF), (OSAL_u8)(msq_ctx->DMEM_Addr_p&0xFF), (OSAL_u8*)&delay_cmd, sizeof(delay_cmd));
				else
				{
					OSAL_trace(TRACE_ERROR,"TRY TO WRITE ILLEGAL DMEM ADDR");
					return -1;
				}
				msq_ctx->DMEM_Addr_p += sizeof(delay_cmd);
				break;

			case T_STOP :
				break;

			case T_DAC_EN :
				break;
				
			default :
				OSAL_trace(TRACE_ERROR,"DMEM_SEQ_WRITE: UNKNOWN SEQ TYPE %d",command->Type);
				break;
		}
	}
	return status;
}

OSAL_void msq_DCO_start(OSAL_void* ctx, OSAL_bool_t pm_msq)
{
	OSAL_u8 tmp;

	pp_hw_reg_read(ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W41, &tmp);
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W41, tmp & 0xFE); // give host access to BAR_CLK_SRC_EN_W1

	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_ANALOG, BAR_MISC_SELECT_W1, 0x02); // enable 1v2RTC boost
	OSAL_udelay(100);
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_SYSTEM, BAR_CLK_SRC_EN_W1, 0x03); // enable DCO source clock DIVA_1X

	if(pm_msq)
		pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W41, tmp | 0x01); // give msq access to BAR_CLK_SRC_EN

	OSAL_udelay(100);
}

OSAL_void msq_DCO_stop(OSAL_void* ctx, OSAL_bool_t pm_msq)
{
	OSAL_u8 tmp;

	pp_hw_reg_read(ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W41, &tmp);
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W41, tmp & 0xFE); // give host access to BAR_CLK_SRC_EN_W1

	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_SYSTEM, BAR_CLK_SRC_EN_W1, 0x00); // disable DCO source clock DIVA_1X
	pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_ANALOG, BAR_MISC_SELECT_W1, 0x03); // disable 1v2RTC boost

	if(pm_msq)
		pp_hw_reg_write(ctx, BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W41, tmp | 0x01); // give msq access to BAR_CLK_SRC_EN
}
