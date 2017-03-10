/**
 * @file ShellCommand_misc.c
 * @brief ShellCommand_misc.c
 * @author  bespoon.com
 * @date 22/2/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#ifdef CONFIG_ACTIVATE_UART_SHELL
#pragma GCC optimize("Os")
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <coreId.h>
#include "pinpointer_regs.h"
#include "pinpointer_hw_api.h"
#include "osal_type.h"
#include "osal_time.h"
#include "osal_trace.h"
#include "osal_comp.h"
#include "osal_math.h"
#include <config_api.h>
#include <msq_config_api.h>
#include <power_mode_types.h>
#include "ProdSectionDef.h"
#include "release_note.h"
#include "USART.h"
#include "dac.h"
#include "RCC.h"
#include "SPI.h"
#include "TIMER.h"
#include "ADC.h"
#include "GPIO.h"
#include "Ext_Com.h"
#include "stm32f0xx_gpio.h"
#include "module.h"
#include "time_helper_api.h"
#include "live_threshold_adjustment_api.h"

#include "shell.h"
#include "ShellCommand_system.h"
#include "ShellCommand_misc.h"

#ifdef CONFIG_RTLS_PROTOCOL
#include "protocol_api.h"
#endif

#ifdef CONFIG_RTLS_PROTOCOL
static OSAL_u16 hardwareCapsMask = 0xFFFF;
static OSAL_u16 UWBCaps = 0x0000;
//value =1 is ranging or samc test ongoing
static char smactest_running=0;
st_uwb_slotRngDataType tpcfg={
	.b1type=-1,
	.b2type=-1,
	.rngtype=-1,
	.rdvtype=-1
};
#endif //#ifdef CONFIG_RTLS_PROTOCOL

#ifdef CONFIG_GENERIC
static OSAL_u16 twFrate=1024;
static OSAL_u16 twIncrement=256;

static OSAL_u16 twCur=0;
static OSAL_u8 txRxIT=0;
static OSAL_u8 twState=0;
#endif //#ifdef CONFIG_GENERIC
static OSAL_bool_t showMOVE=OSAL_false;
static OSAL_bool_t prevMOVE=OSAL_false;


void shellcommand_misc_reinit()
{
	#ifdef CONFIG_RTLS_PROTOCOL
	hardwareCapsMask = 0xFFFF;
	UWBCaps = 0x0000;
	smactest_running=0;
	#endif //#ifdef CONFIG_RTLS_PROTOCOL
	#ifdef CONFIG_GENERIC
	twFrate=1024;
	twIncrement=256;
	twCur=0;
	txRxIT=0;
	twState=0;
	#endif //#ifdef CONFIG_GENERIC
	showMOVE=OSAL_false;
}


void shellcommand_misc_Process()
{
    #ifdef CONFIG_GENERIC
	twoWayRngProcess();
	#endif //#ifdef CONFIG_GENERIC
	if(showMOVE==OSAL_true)
	{
		OSAL_bool_t move=OSAL_false;
		sys_is_module_moving(&move);
		if(move!=prevMOVE)
		{
			prevMOVE=move;
			printline("\r\nUM100 is %smoving",(prevMOVE==OSAL_true)?"":"not ");
		}
	}
}

#define TESTMODE_TX 1
#define TESTMODE_RX 2
#define TESTMODE_RFLOOP 3
static void resetVect()
{
	OSAL_u8 clear_all[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	OSAL_u8 clear_none[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	pp_hw_burst_write(0, BAR_CFG_SYSTEM, 	BAR_INTR_VECT1_CLR_W0,	clear_all, sizeof(clear_all)/sizeof(char));
	OSAL_udelay(100);
	pp_hw_burst_write(0, BAR_CFG_SYSTEM, 	BAR_INTR_VECT1_CLR_W0,	clear_none, sizeof(clear_none)/sizeof(char));
}

static int getVect0(OSAL_timeval* before,unsigned char* IRQ0_Vector, int* Timeout)
{
	OSAL_timeval after,res;
	pp_hw_reg_read(	0, BAR_CTRL_SYSTEM, BAR_INTR_VECT0_W0,IRQ0_Vector);	// Read the IRQ0 Vector
	OSAL_gettimeofday(&after,NULL);
	OSAL_timersub_realtime(&after,before,&res);
	if (res.tv_sec > 5){
		(*Timeout)++;
		printline("Tout");
		return -1;
	}
	return 0;
}

static void rxtxrf_test_process(int frame_size,
                                int frame_nb,
                                int interdelay,
                                int TESTMODE)
{
	OSAL_s32 fr_nb=frame_nb;
	OSAL_s32 fr_size=frame_size;
	OSAL_u16 sizeInBits =0;
	
	OSAL_u16 IRQ_Conf =0xFFFF;
	config_phy_t refphy;
	config_phy_t* curphy = get_current_config_phy(ctx.cfg_ctx);
	OSAL_s32 i,j;
	//init to bad value, in case test failed
	signed long PER=10000;
	signed long SfdTrate=10000;
	signed long NbMissDetect=10000;
	signed long DriftAverage=10000;
	OSAL_u32 root_thld = 0, nrj[EST_SIZE];
	unsigned char Fifo_Rx_Data[MAX_TEST_FRAME_SIZE + 2]; // LONG PHR + MAX_TEST_FRAME_SIZE
	unsigned char mux_reg_ctrl_cfg = 0x00;
	unsigned char prp[EST_SIZE], nrj_tmp[EST_SIZE*4];
	unsigned long prp_valid[EST_SIZE/32];
	int NbReceiveFrames=0;//number of frames decoded (RX_DONE)
	int NbFalseFrame=0;//a frame received with at least 1 false bit
	int NbGoodFrame=0;//count of good frames (no error)
	int Timeout=0;//the frame is a SFD timeout
	long DriftAccumulator=0;//in ppb (!signed)
	OSAL_u32 tmp = 418750;

	if(fr_size> (MAX_TEST_FRAME_SIZE))
		fr_size = MAX_TEST_FRAME_SIZE;
	sizeInBits = fr_size*8;
	print("LOOP ");
	if(fr_nb<=0) printline("ad lib");
	else print("%d",fr_size,fr_nb);
	printline(" x %d bytes ",fr_size);
	//set phy cfg
	OSAL_memcpy(&refphy,curphy,sizeof(config_phy_t));
	switch(TESTMODE)
	{
		case TESTMODE_TX:refphy.power_mode=PM_TX_IDLE;break;
		case TESTMODE_RX:refphy.power_mode=PM_RX_IDLE;break;
		case TESTMODE_RFLOOP:refphy.power_mode=PM_TXRX_IDLE;break;
	}
	radio_showCurPhyAndDac(1);
	/* start local from clean state */
	radio_set_phy_cfg((OSAL_u8*) &refphy,sizeof(config_phy_t));
	
	switch(TESTMODE)
	{
		case TESTMODE_TX: pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT1_MSK_W0,  0xFF);break;
		default:pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT1_MSK_W0,  0xFF);break;
	}
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT1_MSK_W1,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT2_MSK_W0,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT2_MSK_W1,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT3_MSK_W0,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT3_MSK_W1,  0xFF);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT0_MSK_W0,  0xEE);
	pp_hw_reg_write(    0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT0_MSK_W1,  0xFF);
	switch(TESTMODE)
	{
		case TESTMODE_TX: 
			// Enable TX
			IRQ_Conf &=0xFFFE;
			IRQ_Conf &=0xFFBF;
			pp_hw_reg16_write(  0,  BAR_CFG_SYSTEM,     BAR_INTR_VECT0_MSK_W0,  IRQ_Conf);
			break;
		case TESTMODE_RX:break;
		case TESTMODE_RFLOOP:
			// Enable TX+RX
			IRQ_Conf &=0xFFFE;
			IRQ_Conf &=0xFFBF;
			IRQ_Conf &=0xFFBF;
			IRQ_Conf &=0xFFEF;
			IRQ_Conf &=0xFF7F;
			pp_hw_reg16_write(    0,  BAR_CFG_SYSTEM,   BAR_INTR_VECT0_MSK_W0,  IRQ_Conf);
			break;
	}
	/* start from clean state */
	pp_hw_reg32_read(		0, BAR_CFG_DIGITAL, BAR_SFD_TIME_OUT_W0,&tmp);	
	pp_hw_reg_read(			0, BAR_CFG_MUX_REG, BAR_MUX_REG_W33,  	&mux_reg_ctrl_cfg);
	pp_hw_reg_write(		0, BAR_CFG_MUX_REG, BAR_MUX_REG_W33, 	mux_reg_ctrl_cfg & ~0x10);
	pp_hw_reg_write(    	0, BAR_FIFO_TXRX,   BAR_FIFO_RX_CLEAR,  0x01);
	pp_hw_reg_write(		0, BAR_CFG_DIGITAL, BAR_CTRL_CFG, 		0x05);
	
	/* start local from clean state */		
	i=0;
	while (i++ < fr_nb || (fr_nb<=0))
	{
		unsigned char IRQ0_Vector=0x00;
		OSAL_u8 RX_sizeInBytes=0;
		OSAL_timeval before;
		unsigned char bad_frame = 0;
		//clear fifos
		pp_hw_reg_write(    0, BAR_CFG_DIGITAL, BAR_PHY_CTRL_CMD,       0x00);	// Disable Tx/RX transmission
		resetVect();
		pp_hw_reg_write(	0, BAR_FIFO_TXRX,   BAR_FIFO_RX_CLEAR,  	0x01);
		pp_hw_reg_write(	0, BAR_FIFO_TXRX,   BAR_FIFO_TX_CLEAR,   	0x01);
		print("FR%d ", i);
		OSAL_gettimeofday(&before,NULL);
		switch(TESTMODE)
		{
			case TESTMODE_TX:
				//systematically set size in bits in BAR_FRAME_SIZE_W0
				pp_hw_reg16_write(   0,  BAR_CFG_DIGITAL,    BAR_FRAME_SIZE_W0,       sizeInBits);
				pp_hw_reg16_write(	0, BAR_FIFO_TXRX,   BAR_FIFO_TX_DATA, (OSAL_u16) frame_size);	// Push Long PHR in FIFO Tx 
				for(j=0;j<fr_size;j++)
					pp_hw_reg_write(	0, BAR_FIFO_TXRX,   BAR_FIFO_TX_DATA,   	j);	// Push PSDU in FIFO Tx
				pp_hw_reg_write(	0, BAR_CFG_DIGITAL, BAR_PHY_CTRL_CMD,   	0x01);	// Enable TX+RX	
				do{
					if(getVect0(&before,&IRQ0_Vector,&Timeout)<0) goto next;
				}while((IRQ0_Vector & RD_TX_DONE) != RD_TX_DONE);
				printline("sent");
				goto next;
				break;
			case TESTMODE_RX:				
				pp_hw_reg_write(	0, BAR_CFG_DIGITAL, BAR_PHY_CTRL_CMD,   	0x02);	// Enable RX			
				do{
					if(getVect0(&before,&IRQ0_Vector,&Timeout)<0) goto next;
				}while((IRQ0_Vector & RD_RX_DONE) != RD_RX_DONE);

				// LTA management
				if((IRQ0_Vector & RD_SYNCHRO_ESTIMATION_RDY) == RD_SYNCHRO_ESTIMATION_RDY)
				{
					// Read nrj & prp
					pp_hw_burst_read(0, BAR_MEM_ESTIM_PRP, 	0x00, 				&(prp[0]), 				MAX_CHANNEL_ESM);
					pp_hw_burst_read(0,	BAR_MEM_NRJ, 		0x00, 				&nrj_tmp[0], 			MAX_CHANNEL_ESM*4);
					pp_hw_burst_read(0, BAR_CTRL_DIGITAL, 	BAR_IR_WR_FLAG_W0, 	(OSAL_u8*)&prp_valid, 	MAX_CHANNEL_ESM/8);

					for(j = 0; j < MAX_CHANNEL_ESM; j++)
					{
						if(((prp_valid[j/32]>>(j%32))&0x01) == 0) // NRJ and PRP values were not updated, erase them
							prp[j] = nrj[j] = 0;
						else
						{
							nrj[j] = spi_to_u32(&nrj_tmp[4*j]);
							prp[j] = prp[j] & 0x7F;
						}
					}
					//compute new thld from IR and put it in filter
					lta_compute_root_threshold_from_IR(&ctx.lta_ctx[0], refphy.sync_code, refphy.sync_rep, nrj, prp);
				}
				break;
			case TESTMODE_RFLOOP:
				//systematically set size in bits in BAR_FRAME_SIZE_W0
				pp_hw_reg16_write(   0,  BAR_CFG_DIGITAL,    BAR_FRAME_SIZE_W0,       sizeInBits);
				pp_hw_reg16_write(	0, BAR_FIFO_TXRX,   BAR_FIFO_TX_DATA, (OSAL_u16) fr_size);	// Push Long PHR in FIFO Tx 
				for(j=0;j<fr_size;j++)
					pp_hw_reg_write(	0, BAR_FIFO_TXRX,   BAR_FIFO_TX_DATA,   	j);	// Push PSDU in FIFO Tx
				pp_hw_reg_write(	0, BAR_CFG_DIGITAL, BAR_PHY_CTRL_CMD,   	0x03);	// Enable TX+RX	
				do{
					if(getVect0(&before,&IRQ0_Vector,&Timeout)<0) goto next;
				}while((IRQ0_Vector & RD_TX_DONE) != RD_TX_DONE);
				printline("sent");		
				OSAL_gettimeofday(&before,NULL);		
				do{
					if(getVect0(&before,&IRQ0_Vector,&Timeout)<0) goto next;
				}while((IRQ0_Vector & RD_RX_DONE) != RD_RX_DONE);
				break;
		}
		NbReceiveFrames++;
		//PPradio_Red_LED = 0;
		pp_hw_reg_read(     0,  BAR_FIFO_TXRX,      BAR_FIFO_RX_UW ,        &RX_sizeInBytes);
		pp_hw_burst_read(	0, BAR_FIFO_TXRX,  BAR_FIFO_RX_DATA ,	Fifo_Rx_Data, (fr_size+2));
		printline("Sized(%d)",(int) RX_sizeInBytes);
		printline("PHR:TX=0x%x/RX=0x%x",fr_size,Fifo_Rx_Data[0]);
		printline("Rx:");
		bad_frame = 0;
		for(j=0;j<fr_size;j++)
		{
			if((j+1)%16) print("%x, ",Fifo_Rx_Data[j+2]);
			else printline("%x,",Fifo_Rx_Data[j+2]);
			if(Fifo_Rx_Data[j+2] != j) bad_frame = 1;
		}
		printline("");
		if(bad_frame) NbFalseFrame++;
		else
		{
			OSAL_s32 DriftPPB32=0;
			OSAL_s32 dynamic_dll_drift=0;
			OSAL_u32 timer_rx_done = 0, timer_clktrk_start = 0;
			NbGoodFrame++;
			pp_hw_reg32_read(0, BAR_CTRL_DIGITAL, BAR_DYNA_DLL_DRIFT_W0 ,(OSAL_u32*)&dynamic_dll_drift);
			pp_hw_reg32_read(0, BAR_CTRL_DIGITAL, BAR_TIMER_RX_DONE_W0,         &timer_rx_done);
			pp_hw_reg32_read(0, BAR_CTRL_DIGITAL, BAR_TIMER_CLKTRK_START_W0,    &timer_clktrk_start);
			// Exact formula is:
			// driftPPB32 = 1000*(dynamic_dll_drift * 125 *1000)/((timer_rx_done-timer_clktrk_start)*16)
			//FIXME: using 64 bits computation may help 
			if(OSAL_ABS(dynamic_dll_drift) > (max_u32/(to_u32(125)*to_u32(1000)*to_u32(1000)/to_u32(16)))) // prevent u32 overflow
				DriftPPB32 = max_u32;
			else
				DriftPPB32 = OSAL_ABS(dynamic_dll_drift) * (to_u32(125)*to_u32(1000)*to_u32(1000)/to_u32(16));
			if(timer_rx_done > timer_clktrk_start)
				DriftPPB32 /= timer_rx_done-timer_clktrk_start;
			else
				DriftPPB32 /= 0xFFFFFFF - timer_clktrk_start + timer_rx_done;
			DriftAccumulator +=DriftPPB32;			
		}
		next:
		/* finish in clean state */
		
		if(TESTMODE==TESTMODE_RX)
		{
			HAL_ADC_Read_Values();
			pp_conf_set_temperature(ctx.cfg_ctx, (OSAL_s8)(Intern_Temperature/1000));
			lta_compute_root_threshold_from_temperature(&ctx.lta_ctx[0], ctx.cfg_ctx->temperature);
			root_thld = lta_get_live_root_threshold(&ctx.lta_ctx[0]);
			pp_conf_set_root_threshold(ctx.cfg_ctx, refphy.channel, refphy.rf_path, root_thld);
			radio_set_phy_cfg((OSAL_u8*) &refphy,sizeof(config_phy_t));
		}
		if(TESTMODE==TESTMODE_TX) OSAL_msleep(interdelay);
	}
	pp_hw_reg_write(    0, BAR_CFG_DIGITAL, BAR_PHY_CTRL_CMD,       0x00);	// Disable Tx/RX transmission
	resetVect();
	if(TESTMODE!=TESTMODE_TX)
	{
		//compute now
		if(i>1)
		{
			PER = (signed long) ((NbFalseFrame * 1000) / (i-1));
			SfdTrate = (signed long) ((Timeout  * 1000) / (i-1));
			DriftAverage = (signed long) (DriftAccumulator / NbGoodFrame);
			NbMissDetect = (signed long) (i-1 -NbReceiveFrames);
		}
		printline("PER=%d",PER);
		printline("SfdT=%d",SfdTrate);
		printline("Drift=%d",DriftAverage);
		printline("Miss=%d",NbMissDetect);
	}
	printline("WARNING:");
	printline("Unless you don't do other RX/TX test");
	printline("you should reset the module");
}

signed char tx_fct(int argc, char** argv)
{
	if(argc<4) return -2;
	rxtxrf_test_process(atoi(argv[1]),
			atoi(argv[2]),
			atoi(argv[3]),
			TESTMODE_TX
			);
	return 0;
}

static char rxorrf_fct(int argc, char** argv,int mode)
{
	if(argc<3) return -2;
	rxtxrf_test_process(atoi(argv[1]),
			atoi(argv[2]),
			0,
			mode
			);
	return 0;
}

signed char rx_fct(int argc, char** argv)
{
	return rxorrf_fct(argc,argv,TESTMODE_RX);
}

#ifdef CONFIG_RTLS_PROTOCOL
#ifdef TEST_SHOWSFI
static void showSFI(superframe_info_t* default_sfi)
{
	OSAL_u32 i;
	printline("SFI:");
	printline(" phs=%d",default_sfi->phs);
	printline(" psn=%d",default_sfi->psn);
	printline(" pss=%d",default_sfi->pss);
	printline(" nb=%d",default_sfi->nb_frames);
	printline(" bd=%d",default_sfi->beacon_distance);
	printline(" SCI=%d",default_sfi->sync_code_ID);
	printline(" DCI=%d",default_sfi->demod_code_ID);
	for(i=0;i<MAX_FRAME_PER_SUPERFRAME;i++)
	{
		if(default_sfi->frames_desc[i].type!=0)
		{
			printline(	"SLOT%d(%d):%d,%d,%d,%d,%d,x%.8x,%d",
						default_sfi->frames_desc[i].slot_nb,
						default_sfi->frames_desc[i].type,
						default_sfi->frames_desc[i].act,
						default_sfi->frames_desc[i].beacon_id,
						default_sfi->frames_desc[i].zone,
						default_sfi->frames_desc[i].channel,
						default_sfi->frames_desc[i].rf_path,
						default_sfi->frames_desc[i].rngDataType,
						default_sfi->frames_desc[i].uwbPayloadInBits);
		}
	}
}
#else
#define showSFI(...) do{}while(0)
#endif //#ifdef TEST_SHOWSFI

signed char smac_test_fct(int argc, char** argv)
{
	signed char ret=-1;
	int errLine = __LINE__;
	superframe_info_t* default_sfi=NULL;
	OSAL_u32 choice=0xFF;
	OSAL_u8 rate=0;
	OSAL_u8 nb=0xFF;
	OSAL_u32 caps=0x00000000;

	if(argc>=2)
	{
		choice = atoi(argv[1]);
		if(strlen(argv[1])==1)
			rate = PROT_SFI_DEFAULT_RATE;
		else if(strlen(argv[1])==2)
		{
			rate=(int) (argv[1][1]-'0');
			choice=(int) (argv[1][0]-'0');
		}
		else if(strlen(argv[1])<=4)
		{
			nb=atoi(&argv[1][2]);
			rate=(int) (argv[1][1]-'0');
			choice=(int) (argv[1][0]-'0');
		}
	}
	if(smactest_running)
	{
		average_rng_measure_t av;
		protocol_check_metrics(	(OSAL_void*) &av);
		protocol_free_metrics();
		if(av.rangingNB>0)
		{
			printline("%d RNG:",av.rangingNB);
			printline(" Toa:%d",av.averageTOA);
			printline("-NRJ:%d(%d<.<%d)",av.averageNRJ,av.minNRJ,av.maxNRJ);
		}
		smactest_running = 0;
		test_reinit();
		ret = 0;
		printline("SMAC STOPPED");
	}
	if(choice > 4)
	{
		ret = 0;
		goto beach;
	}
	if(protocol_init_prod_metrics()!=0)
	{
		ret = 1;
		goto beach;
	}
	smactest_running = 1;
	//use our own superframe info
	//launch otherwise
	//1 init protocol
	prot_init();
	prot_get_superframe_shema_buffer((OSAL_u8**)&default_sfi);

	switch(choice)
	{
		case 0://BASE
			print("BASE");
			#ifdef CONFIG_RTLS_FULL_LOC
			prot_get_default_base_sfi(default_sfi,rate,&tpcfg);
			#else //CONFIG_RTLS_FULL_LOC
			print(" unsupported(!CONFIG_RTLS_FULL_LOC)");
			ret = 1;
			goto beach;
			#endif
			break;
		case 1://TAG
			print("TAG");
			caps = DEVCAPS_UWB_SLAVE;
			prot_get_default_tag_sfi(default_sfi,rate,&tpcfg);
			break;
		case 2://COORD
			print("COORD");
			#ifdef CONFIG_RTLS_FULL_LOC
			caps = DEVCAPS_ROOT|DEVCAPS_UWB_MASTER|DEVCAPS_FILTER_NULL;
			prot_get_default_coord_sfi(default_sfi,rate,&tpcfg);
			#else //CONFIG_RTLS_FULL_LOC
			print(" unsupported(!CONFIG_RTLS_FULL_LOC)");
			ret = 1;
			goto beach;
			#endif
			break;
		case 3://FAKE TAG
			print("TAG W/O RACH");
			caps = DEVCAPS_NO_FRAME;
			prot_get_tag_norach_sfi(default_sfi,rate,&tpcfg);
			break;
		case 4://DOS
			print("DOS");
			caps = DEVCAPS_NO_FRAME|DEVCAPS_FAKEADDR;
			prot_get_tag_sfi_multi_slot_norach(default_sfi,rate,&tpcfg,nb);
			break;
	}
	switch(rate)
	{
		default:printline("(dflt)");break;
		case 1:printline("(fast)");break;
		case 2:printline("(slow)");break;
		case 3:printline("(wr)");break;
	}
	printline("STARTED");
	showSFI(default_sfi);
	
	prot_set_caps(caps);
	prot_set_superframe_shema((OSAL_u8*) default_sfi);
	prot_ctrl(UWB_SC_START);
	ret = 0;
	beach:
	if(ret)
	{
		printline("Fail[%d]",errLine);
		protocol_free_metrics();
		smactest_running = 0;
	}
	else
		send_host(argv[0], 0, 1, 0);
	return ret;
}

signed char toa_show_fct(int argc, char** argv)
{
	char* p;
	if(argc<2) return -2;
	prot_showToaTrace((unsigned char) (strtol(argv[1],&p,0)));
	return 0;
}

signed char rngType_fct(int argc, char** argv)
{
	OSAL_s32 CHOICE=0;
	char *endptr;
	int val=0;
	if(argc<2)
	{
		tpcfg.b1type=-1;
		tpcfg.b2type=-1;
		tpcfg.rngtype=-1;
		tpcfg.rdvtype=-1;
		return 0;
	}
	val=(unsigned int)strtoul(argv[1], &endptr, 16); //Hexadecimal necessarily
	if(argc>=3) CHOICE=atoi(argv[2]);
	//CHOICE : "(*) 1: B1, 2:B2, 3: RNG, 4:RDV\n"
	switch(CHOICE)
	{
		case 1:tpcfg.b1type=val;break;
		case 2:tpcfg.b2type=val;break;
		case 3:tpcfg.rngtype=val;break;
		case 4:tpcfg.rdvtype=val;break;
		default:
			tpcfg.b1type=val;
			tpcfg.b2type=val;
			tpcfg.rngtype=val;
			tpcfg.rdvtype=val;
			break;
	}
	tpcfg.b1type&=0xFFFFF000;
	tpcfg.b2type&=0xFFFFF000;
	tpcfg.rdvtype&=0xFFFFF000;
	return 0;
}
#endif //CONFIG_RTLS_PROTOCOL

#ifdef CONFIG_GENERIC
static int get_twOffset()
{
	OSAL_s64 offset_64=to_s64(twFrate);
	OSAL_s32 offset=0;
	offset_64 *= 1000000000;//in this case offset is 2 * framerate
	offset_64 *= 2;//in this case offset is 2 * framerate
	offset_64 /= 32768;//in this case offset is 2 * framerate
	offset = to_s32(offset_64);
	return offset;
}

static void twoWayRngDoProcess(OSAL_u16 status)
{
	if(txRxIT!=0)
	{
		OSAL_u8* seqBuf;
		OSAL_u16 seqBufsize=0;
		OSAL_u16 doShowRng=0;
		//get current IT, and upgrade seq sequence etc....
		switch(twState&0xF)
		{
			//wait until seq started
			case 0:
				if((status&0x0100)==0) return;
				//printline("SEQ Start. enqueue next RX!");
				twState++;
				seq_get_sequence_data(&seqBuf,&seqBufsize);
				//enqueue RX
				if(txRxIT<4)
					seqBuf[0]=(txRxIT+1);
				else
					seqBuf[0]=1;			
				U16VAL_TO_MSBLSB(twCur,seqBuf[2],seqBuf[1]);
				seqBuf[3]=seqBuf[4]=seqBuf[5]=seqBuf[6]=seqBuf[7]=seqBuf[8]=seqBuf[10]=seqBuf[11]=0;
				seqBuf[9]=0x41;
				seq_queue_single_act();
				printline("START TO SCAN...");
			break;
			case 1://wait for seq action start and enqueue TX
				if((status&0x0200)==0) return;
				//printline("Seq action start. enqueu TX!");
				twState++;
				seq_get_sequence_data(&seqBuf,&seqBufsize);
				seqBuf[0]=txRxIT;
				U16VAL_TO_MSBLSB(twCur,seqBuf[2],seqBuf[1]);
				seqBuf[3]=seqBuf[4]=seqBuf[5]=seqBuf[6]=seqBuf[7]=seqBuf[8]=seqBuf[10]=seqBuf[11]=0;
				seqBuf[9]=0x42;
				seq_queue_single_act();		 
			break;
			case 2://wait TX DONE, and set pm to RX_IDLE
				if((status&0x0010)==0) return;
				//printline("TX Done. Put to RX IDLE!");
				twState++;
				radio_set_pmstate(UWB_SC_PM_RX_IDLE);
			break;
			case 3://wait for seq action start and enqueue RX
				if((status&0x0200)==0) return;
				//printline("Seq action start. enqueue RX!");
				twState++;
				seq_get_sequence_data(&seqBuf,&seqBufsize);
				if(txRxIT<4)
					seqBuf[0]=(txRxIT+1);
				else
					seqBuf[0]=1;
				U16VAL_TO_MSBLSB(twCur,seqBuf[2],seqBuf[1]);
				seqBuf[3]=seqBuf[4]=seqBuf[5]=seqBuf[6]=seqBuf[7]=seqBuf[8]=seqBuf[10]=seqBuf[11]=0;
				seqBuf[9]=0x41;
				seq_queue_single_act();	
			break;
			case 4:{
				OSAL_u8 twTXData[12]={0};
				//wait RX Done or premable tout, and enqueue RX, write TXDATA, set pm to TX ILDE and show eventually rng data
				if((status&0x00E0)==0) return;
				twState&=0xF0;
				twState|=0x1;
				if((status&0x0020)!= 0) //RX DONE
				{
					//get data
					OSAL_u16 rxbufSize=0;
					OSAL_u8* rxbuf;
					OSAL_u8* rxdata;
					rng_data_generic_t* rng;
					radio_get_rx_buf(&rxbuf,&rxbufSize);
					//printline("RX Done siz=%d",rxbufSize);
					if(rxbufSize == 1+sizeof(rng_data_generic_t)+2+12)
					{
						OSAL_u32 otherTOAraw=0;
						OSAL_s32 otherTOACorr=0;
						OSAL_s32 distancens=0;
						OSAL_s32 distanceps=0;
						OSAL_s32 distanceCm=0;
						
						rng = (rng_data_generic_t*) &rxbuf[1];
						rxdata = &rxbuf[1+sizeof(rng_data_generic_t)+2];
						MSBLSB_TO_U32VAL(rxdata[3],rxdata[2],rxdata[1],rxdata[0],otherTOAraw);
						MSBLSB_TO_U32VAL(rxdata[7],rxdata[6],rxdata[5],rxdata[4],otherTOACorr);
						U32VAL_TO_MSBLSB(rng->toa_le_raw,twTXData[3],twTXData[2],twTXData[1],twTXData[0]);
						U32VAL_TO_MSBLSB(rng->toa_le_corr,twTXData[7],twTXData[6],twTXData[5],twTXData[4]);
						if(otherTOAraw !=0)
						{					
							doShowRng=1;
							twState|=0x80;
							distancens = ((OSAL_s32) (rng->toa_le_raw) + otherTOAraw - get_twOffset())/2;
							distanceps = ((OSAL_s32) (rng->toa_le_corr) + otherTOACorr)/2;
							distanceCm = ((distancens*1000+distanceps)*30)/1000;
							if(twIncrement==0)
								printline(	"D=%d,%d ns(%d cm), drift=%d ppb, lqi=%d/100 [Tr=%d,%d Te=%d,%d]",
											distancens,
											distanceps,
											distanceCm,
											rng->clock_drift,
											rng->link_quality,
											rng->toa_le_raw,
											rng->toa_le_corr,
											otherTOAraw,
											otherTOACorr);
							else
								printline(	"D=%d,%d ns(%d cm), drift=%d ppb, lqi=%d/100 [Te=%d,%d Tr=%d,%d]",
											distancens,
											distanceps,
											distanceCm,
											rng->clock_drift,
											rng->link_quality,
											rng->toa_le_raw,
											rng->toa_le_corr,
											otherTOAraw,
											otherTOACorr);
  
							if(rng->clock_drift !=0)
							{
								OSAL_u16 dac=0;
								sys_get_dac_val(&dac);
								//artificially decrease dac from 
								if(rng->clock_drift>0) dac -=1;
								else dac +=1;
								sys_set_dac_val(dac);
							}
						}
					}
				}
				//else
				//   printline("Pream tout. Write TX, and put to TX_IDLE");
				if(doShowRng==0)
				{
					//increment tx
					twCur +=twIncrement;
					if(twCur>(twFrate-64))
					{
					       twCur=64;
					       txRxIT++;
					       if(txRxIT>4) txRxIT=1;
					}
					printline("SCANNING....");
					twState&=0xF;
				}
				else if((twState&0x80)==0)
				{
					twState|=0x80;
					printline("SYNCHRONIZED WITH DISTANT !!!!!!!");
				}
				//set TX Data
				//WRITE_TX_DATA"	
				radio_write_tx_data(twTXData,96);
				radio_set_pmstate(UWB_SC_PM_TX_IDLE);
			}	       
			break;
		}
	}
}

void twoWayRngProcess()
{
	if(txRxIT!=0)
	{
		OSAL_u16 status=0;
		sys_get_status(&status);
		//printline("status = %x",status);
		//do it twice, in case mulitple IT in the same status
		twoWayRngDoProcess(status);
		twoWayRngDoProcess(status);
		sys_reset_status();
	}
}

signed char twoWayRng(int argc, char** argv)
{
	OSAL_u8* cfg;
	OSAL_u8* seqBuf;
	OSAL_u16 cfgsize=0,seqBufsize=0;    
	OSAL_s32 pTout=0;
	OSAL_s32 frInUs=0;
	if(argc<6) 
	{
		if(txRxIT == 0)
		{
			printline("no 2 way ranging ongoing....");
		}
		else
		{
			test_reinit();
			printline("2 way ranging stopped.");
		}
	}
	else if(txRxIT!=0)
	{
		printline("2way rng already running, stop it first");
		return -1; 
	}
	else
	{
		config_phy_t refphy;
		OSAL_u8 twTXData[12]={0};
		test_reinit();
		//sys_set_mcu_state(UWB_SC_MCU_RUN);
		//programm some sequence
		//SSET_STATUS_MSK
		sys_set_status_mask(0xFC0F);
		//SSET_RXFRAME_CFG
		sys_set_rxframe_cfg(0xB);
		//RSET_RX_CFG
		radio_set_rx_cfg(0x9,0x60);
		//set phy cfg
		refphy.sync_code_ID=0;
		refphy.demod_code_ID=0;
		refphy.channel=2;
		refphy.sync_code=atoi(argv[1]);
		refphy.demod_code=atoi(argv[1]);
		refphy.sync_rep=atoi(argv[2]);
		refphy.demo_rep=atoi(argv[2]);
		refphy.clock_track=1;
		refphy.framer_config=0;
		refphy.rf_path=DEFAULT_RFPATH;
		refphy.sync_threshold=atoi(argv[3]);
		refphy.rng_threshold=atoi(argv[3]);
		refphy.tx_gain=3;
		refphy.rx_gain=3;
		radio_get_pmstate((uwb_com_subcmd_e*) &refphy.power_mode);
		refphy.act=0;
		txRxIT=1;
		frInUs=atoi(argv[5]);
		
		if(frInUs>250)
		{
			printline("Cant be > 250ms");
			twFrate = 8192;
			frInUs=250000;
		}
		else if(frInUs<4)
		{
			printline("Cant be > 4ms");
			twFrate = 128;
			frInUs=4000;
		}
		else
		{
			frInUs *= 1000;
			twFrate = frInUs*4096/125000;
			twFrate /= 64;
			twFrate *= 64;
			if(twFrate<128) twFrate=128;
			frInUs = twFrate*125000/4096;
		}
		twCur = 64;
		if(argc==5)
			twIncrement=0;
		else
		{
			if(atoi(argv[6])>(frInUs/1000))
			{
				printline("increment Cant be > %dms.Wont scan",(frInUs/1000));
				twIncrement=0;
			}
			else
			{				
				twIncrement=atoi(argv[6])*1000*4096/125000;	
				twIncrement /= 64;
				twIncrement *= 64;
				
			}
		}
		if(atoi(argv[4])>2147)
		{
			printline("Cant be > 2147ms");
			pTout=2147000;//in us		
		}
		else
		{
			pTout=atoi(argv[4])*1000;//in us		
			if(pTout>(5*frInUs/2))
			{
				printline("Tout cant be > 2.5*%d",frInUs/1000);
				pTout=5*frInUs/2;
			}
		}
		refphy.preamble_timeout=NANO_TO_EP(pTout)*1000;//in EP
		printline("Tout = %dEP",refphy.preamble_timeout);
		radio_set_phy_cfg((OSAL_u8*) &refphy,sizeof(config_phy_t));
		
		printline("Tw=%d, inc=%d",twFrate,twIncrement);
		//RSET_PM_STATE
		radio_set_pmstate(UWB_SC_PM_TX_IDLE);
		//SQINIT
		seq_init();
		//SQSET_CFG
		seq_get_cfg_buffer(&cfg,&cfgsize);
		U16VAL_TO_MSBLSB(twFrate,cfg[1],cfg[0]);
		cfg[2]=4;//always 4IT
		seq_set_cfg();
		seq_get_sequence_data(&seqBuf,&seqBufsize);
		//enqueue TX
		seqBuf[0]=txRxIT;
		U16VAL_TO_MSBLSB(twCur,seqBuf[2],seqBuf[1]);
		seqBuf[3]=seqBuf[4]=seqBuf[5]=seqBuf[6]=seqBuf[7]=seqBuf[8]=seqBuf[10]=seqBuf[11]=0;
		seqBuf[9]=0x42;
		seq_queue_single_act();
		//WRITE_TX_DATA"
		printline("%s started with DISTOffset=%d ns",(twIncrement==0)?"Receiver":"Emitter",get_twOffset());	
		radio_write_tx_data(twTXData,96);
		//SQCTRL
		seq_ctrl( UWB_SC_START);		
    }
    send_host(argv[0],0, 1, (signed long)0);
    return 0;
}
#endif //#ifdef CONFIG_GENERIC

#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL

#ifdef CONFIG_RTLS_PROTOCOL
signed char bttm_fct(int argc, char** argv)
{
	if(argc<2) return -2;
	ctx.prefered_ref_base = atoi(argv[1]);
	send_host(argv[0], 0, 1, 0);
	return 0;
}

signed char prot_tune_fct(int argc, char** argv)
{
	if(argc<3) return -2;
	if(smactest_running !=0)
	{
		printline("SMAC running");
		return -1;
	}
	hardwareCapsMask = atoi(argv[1]);
	UWBCaps = atoi(argv[2]);
	//will change default superframe/tag frame
	return 0;
}
#endif //#ifdef CONFIG_RTLS_PROTOCOL

signed char rfloop_fct(int argc, char** argv)
{
	return rxorrf_fct(argc,argv,TESTMODE_RFLOOP);
}

signed char checkMove_fct(int argc, char** argv)
{
	if(argc<2) return -2;
	showMOVE=atoi(argv[1])?OSAL_true:OSAL_false;
	return 0;
}

#endif //#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
#endif //#ifdef CONFIG_ACTIVATE_UART_SHELL

