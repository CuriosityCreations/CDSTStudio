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
#define SYS_UART_DIMMER 10
OSAL_u8 module_config[MODULE_CONFIG_SIZE]={0};
static OSAL_u32 module_shortAddr=0;
context_t ctx; // Include all Context Variable for software
struct pp_conf contexte_config;

OSAL_void sys_get_shared_buf(OSAL_u8 sharedBufNb,OSAL_u8** buf, OSAL_u16* totalsizeWithAck)
{
	if(buf)
	{
		if(0 == sharedBufNb)
			*buf = &module_config[MOD_CFG_POS_SHARE_BUFFER_0_BUF];
		else
			*buf = &module_config[MOD_CFG_POS_SHARE_BUFFER_1_BUF];
	}
	if(totalsizeWithAck)
	{
		if(0 == sharedBufNb)
		{
			 MSBLSB_TO_U16VAL( 	module_config[MOD_CFG_POS_SHARE_BUFFER_0_SIZE_MSB],
						module_config[MOD_CFG_POS_SHARE_BUFFER_0_SIZE_LSB],
						*totalsizeWithAck);
			*totalsizeWithAck += MOD_CFG_POS_SHARE_BUFFER_0_OVERHEAD;
		}
		else
		{
			MSBLSB_TO_U16VAL(  	module_config[MOD_CFG_POS_SHARE_BUFFER_1_SIZE_MSB],
						module_config[MOD_CFG_POS_SHARE_BUFFER_1_SIZE_LSB],
						*totalsizeWithAck);
			*totalsizeWithAck += MOD_CFG_POS_SHARE_BUFFER_1_OVERHEAD;
		}
	}
}

OSAL_void sys_get_shared_data(OSAL_u8 sharedBufNb,OSAL_u8** data, OSAL_u16* sizeWithoutAck)
{
	if(data)
	{
		if(0 == sharedBufNb)
			*data = &module_config[MOD_CFG_POS_SHARE_BUFFER_0_DATA];
		else
			*data = &module_config[MOD_CFG_POS_SHARE_BUFFER_1_DATA];
	}
	if(sizeWithoutAck)
	{
		if(0 == sharedBufNb)
			 MSBLSB_TO_U16VAL(	module_config[MOD_CFG_POS_SHARE_BUFFER_0_SIZE_MSB],
						module_config[MOD_CFG_POS_SHARE_BUFFER_0_SIZE_LSB],
						*sizeWithoutAck);
		else
			MSBLSB_TO_U16VAL(	module_config[MOD_CFG_POS_SHARE_BUFFER_1_SIZE_MSB],
						module_config[MOD_CFG_POS_SHARE_BUFFER_1_SIZE_LSB],
						*sizeWithoutAck);
	}
}

OSAL_void sys_set_shared_data_size(OSAL_u8 sharedBufNb,OSAL_u16 size)
{
	if(0 == sharedBufNb)
		U16VAL_TO_MSBLSB(	size,
					module_config[MOD_CFG_POS_SHARE_BUFFER_0_SIZE_MSB],
					module_config[MOD_CFG_POS_SHARE_BUFFER_0_SIZE_LSB]);
	else
		U16VAL_TO_MSBLSB(	size,
					module_config[MOD_CFG_POS_SHARE_BUFFER_1_SIZE_MSB],
					module_config[MOD_CFG_POS_SHARE_BUFFER_1_SIZE_LSB]);
}

OSAL_void sys_set_shared_data_checksum(OSAL_u8 sharedBufNb,OSAL_u16 chckSize)
{
	OSAL_u8* dat;
	OSAL_u16 size;
	OSAL_u16 checksum;
	sys_get_shared_data(sharedBufNb,&dat,&size);
	GET_SIMPLE_U16CHECKSUM(dat,OSAL_MIN(chckSize,size),checksum);
	if(0 == sharedBufNb)
		U16VAL_TO_MSBLSB(	checksum,
					module_config[MOD_CFG_POS_SHARE_BUFFER_0_CHECKSUM_MSB],
					module_config[MOD_CFG_POS_SHARE_BUFFER_0_CHECKSUM_LSB]);
	else
		U16VAL_TO_MSBLSB(	checksum,
					module_config[MOD_CFG_POS_SHARE_BUFFER_1_CHECKSUM_MSB],
					module_config[MOD_CFG_POS_SHARE_BUFFER_1_CHECKSUM_LSB]);
}

OSAL_void sys_get_shared_data_checksum(OSAL_u8 sharedBufNb,OSAL_u16* chcksum)
{
	if( 0 == sharedBufNb)
		MSBLSB_TO_U16VAL(	module_config[MOD_CFG_POS_SHARE_BUFFER_0_CHECKSUM_MSB],
					module_config[MOD_CFG_POS_SHARE_BUFFER_0_CHECKSUM_LSB],
					*chcksum);
	else
		MSBLSB_TO_U16VAL( 	module_config[MOD_CFG_POS_SHARE_BUFFER_1_CHECKSUM_MSB],
					module_config[MOD_CFG_POS_SHARE_BUFFER_1_CHECKSUM_LSB],
					*chcksum);
}

OSAL_timeval lastbattery_and_temperature_update={0,0};
static OSAL_void sys_deinit_firmware()
{
	//EXTI, NVIC
	HAL_EXTI_DeInit();
	//ACCELERO
	LIS2DM_DeInit();
	//GPIO
	HAL_GPIO_DeInit();
	// SPI
	HAL_SPI_DeInit();
	// I2C
	HAL_I2C_DeInit();
	// DAC
	HAL_DAC_DeInit();
	// USART
	HAL_USART_DeInit();
	//RTC
	HAL_RTC_DeInit();
	//RCC
	HAL_RCC_Init(DISABLE);
}

static OSAL_void sys_init_firmware()
{
	OSAL_u8 usartnb=(prodSection.userSection.moduleCfg.shellConfig&SHELL_UARTNB_MASK)>>1;
	OSAL_u8 usartshell=prodSection.userSection.moduleCfg.shellConfig&SHELL_UARTSHELL_MASK;

#ifdef HAS_RED_LED
	sys_led_management(SYS_LED_RED,OSAL_true); // light on red led as soon as possible. This is used for production / init
	sys_led_management(SYS_LED_GREEN,OSAL_false); // cannot have red/green together
#endif

	switch(usartnb)
	{
		case 1: if(usartshell) USE_SHELL_USART_NB=0x11;
			else USE_SHELL_USART_NB=0x01;
			break;
		case 2: if(usartshell) USE_SHELL_USART_NB=0x22;
			else USE_SHELL_USART_NB=0x02;
			break;
		default:
			#if defined(CONFIG_MINITAG_V2)
			if(usartshell) USE_SHELL_USART_NB=0x11;
			else USE_SHELL_USART_NB=0x01;
			#else
			if(usartshell) USE_SHELL_USART_NB=0x22;
			else USE_SHELL_USART_NB=0x02;
			#endif
		break;
	}
	/*  due to sw reset, and to be sure we start at clean state, deinit everything, before doing init routines */
	sys_deinit_firmware();

	/*  NOW INIT THEM ALL */
	// init low clock
	HAL_RCC_LowClockInit();
	// init RCC
	HAL_RCC_Init(ENABLE);
	// init RTC
	HAL_RTC_Init();
	// timer init
	HAL_TIMER_Init();
	//wait for hw clocking fine
	OSAL_udelay(1000);
	// init gpio
	HAL_GPIO_Init();
	// USART Init
	HAL_USART_Init();
	// init DAC
	HAL_DAC_Init();
	// I2C
	HAL_I2C_Init();
	// init SPI
	HAL_SPI_Init();
	// Accelero
	LIS2DM_init();
	//wait for gpios  line, etc... to be at their 'running' state,
	OSAL_udelay(1000);
	// EXTI
	HAL_EXTI_Init();
	//let calibrate RTC clock if needed
	HAL_RTC_Autocalibrate();
	//Battery and temperature
	HAL_ADC_Read_Values();
	OSAL_gettimeofday(&lastbattery_and_temperature_update,NULL);
}

static OSAL_timeval sys_dimmer_before_sleep={0,0};
static OSAL_void sys_reset_dimmer()
{
	sys_dimmer_before_sleep.tv_sec=0;
	sys_dimmer_before_sleep.tv_usec=0;
	module_config[MOD_CFG_POS_SYS_DIMMER_STATUS]=0;
}

static OSAL_void sys_get_deflt_value()
{
	unsigned long prodSectionval_12bits = 0;
	//dac get prodsection
	//captu lire reg pp_hwçrea(bank analog capatune
	//valeur default virtuel at 0
	//in this function, put only non zero value
	OSAL_memset(module_config,0,MODULE_CONFIG_SIZE);
	//rest of code is redondnat with memeste, but its for us to knwo whart we re doing
	//dont touch share buffer, its not necessary
	//SYSTEM DEFAULT VALUE
	sys_reset_wake_up_reason(SYS_WAKE_UP_ALL_MASK);
	//authorize stopped state
	//module_config[MOD_CFG_POS_SYS_MCU_STATE] = 0;
	sys_set_mcu_state(UWB_SC_MCU_STOPPED);
	//uncomment this to let MCU always run
	//sys_set_mcu_state(UWB_SC_MCU_RUN);

	//get default dac val from prodsection (in UV-> in Dac 12 bits value)
	prodSectionval_12bits = prodSection.userSection.radioCfg.dacDefault;
	prodSectionval_12bits /=10;//250000
	prodSectionval_12bits *=0xFFF;
	prodSectionval_12bits /=VDDA;
	U16VAL_TO_MSBLSB(	((OSAL_u16) prodSectionval_12bits),
				module_config[MOD_CFG_POS_SYS_DAC_VAL_MSB],
				module_config[MOD_CFG_POS_SYS_DAC_VAL_LSB]);
	//will be seted on radio init
	//get default capatune val
	pp_hw_reg_read(     0,  BAR_CFG_ANALOG, BAR_MISC_TUNE_OSCQ32K_CAPATUN,&module_config[MOD_CFG_POS_SYS_CAPATUNE_VAL]);
	//module_config[MOD_CFG_POS_SYS_RXFRAME_CFG]=0;
	sys_reset_status();
	//default status amsk is no IT
	sys_set_status_mask(UWB_STAT_MASK_NONE);
	//module_config[MOD_CFG_POS_SYS_STATUS_MSK_BUSY]=0;
	//module_config[MOD_CFG_POS_SYS_BUSY]=0;
	module_config[MOD_CFG_POS_SYS_CHECKCACHE]=1;//everything is ok at this step
	sys_led_management(SYS_LED_GREEN,OSAL_false);
	sys_led_management(SYS_LED_RED,OSAL_true);

	//module_config[MOD_CFG_POS_SYS_MODULE_MOVING]=0;
	//time to achieve before going ot sleep
	sys_reset_dimmer();
	PWR_PreventFromDeepSleep(DISABLE);

	//RADIO SPECIFIC
	module_set_busyOFF(MOD_CFG_POS_RADIO_BUSY,MOD_CFG_POS_RADIO_INIT_BUSY_MSK);
	module_config[MOD_CFG_POS_RADIO_PM_STATE]=UWB_SC_PM_2;//will be changed by radio init
	//module_config[MOD_CFG_POS_RADIO_TX_DATA_SIZE_INBITS_MSB]=0;
	//module_config[MOD_CFG_POS_RADIO_TX_DATA_SIZE_INBITS_LSB]=0;
	//module_config[MOD_CFG_POS_RADIO_RX_CFG]=0;
	//module_config[MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_MSB]=0;
	//module_config[MOD_CFG_POS_RADIO_RX_DATA_SIZE_INBITS_LSB]=0;
	//module_config[MOD_CFG_POS_RADIO_TX_ONGOING]=0;
	//module_config[MOD_CFG_POS_RADIO_RX_ONGOING]=0;
	//module_config[MOD_CFG_POS_RADIO_TX_ABORT]=0;
	//module_config[MOD_CFG_POS_RADIO_RX_ABORT]=0;
	//module_config[MOD_CFG_POS_RADIO_BUSY]=0;

	//SEQUENCER SPECIFIC
	//module_config[MOD_CFG_POS_SEQ_CFG0]=0;
	//module_config[MOD_CFG_POS_SEQ_CFG1]=0;
	//module_config[MOD_CFG_POS_SEQ_CFG2]=0;
	//module_config[MOD_CFG_POS_SEQ_STATE]=0;
	//module_config[MOD_CFG_POS_SEQ_BUSY]=0;

	//module_config[MOD_CFG_POS_SEQ_SEQPOSITION_WR] = 0;
	//module_config[MOD_CFG_POS_SEQ_SEQPOSITION_RD] = 0;

	#ifdef CONFIG_RTLS_PROTOCOL
		//module_config[MOD_CFG_POS_PROT_BUSY]=0;
		#ifdef CONFIG_RTLS_FULL_LOC
			//module_config[MOD_CFG_PROT_ARTLS_OUT_CHECKSUM_MSB]=0;
			//module_config[MOD_CFG_PROT_ARTLS_OUT_CHECKSUM_LSB]=0;
			//module_config[MOD_CFG_POS_PROT_SHADOWBUF_INDEX]=0;
			//module_config[MOD_CFG_POS_PROT_ENTRY_NUMBER] = 0;
			//module_config[MOD_CFG_POS_PROT_LASTENTRY_SIZE_MSB]=0;
			//module_config[MOD_CFG_POS_PROT_LASTENTRY_SIZE_LSB]=0;
			//module_config[MOD_CFG_POS_PROT_SHADOWBUF_RXCNT]=0;
			//module_config[MOD_CFG_POS_PROT_SHADOWBUF_RXTOTAL]=0;
			//module_config[MOD_CFG_POS_PROT_SHADOWBUF_FIRSTSLOT]=0;
			//module_config[MOD_CFG_POS_PROT_SHADOWBUF_LASTSLOT]=0;
			module_config[MOD_CFG_POS_PROT_SHADOWBUF_CURSLOT]=(OSAL_u8) -1;
		#endif
		//module_config[MOD_CFG_POS_PROT_CAPS_MSBMSB]=0;
		//module_config[MOD_CFG_POS_PROT_CAPS_MSBLSB]=0;
		//module_config[MOD_CFG_POS_PROT_CAPS_LSBMSB]=0;
		module_config[MOD_CFG_POS_PROT_CAPS_LSBLSB]=DEVCAPS_UWB_SLAVE; // TAG by default
		U16VAL_TO_MSBLSB(PROT_STATUS_NO_CHANGE,
				 module_config[MOD_CFG_POS_PROT_STATUS_CHG_MSB],
				 module_config[MOD_CFG_POS_PROT_STATUS_CHG_LSB]);
		module_config[MOD_CFG_POS_PROT_STATE]=MOD_CFG_PROT_STATE_STOPPED;
		//module_config[MOD_CFG_POS_PROT_DISSOCIATE]=0;
		prot_get_shortAddr(&module_shortAddr);
	#else
		MSBLSB_TO_U32VAL(	prodSection.manufacturerSection.shortAddress[0],
							prodSection.manufacturerSection.shortAddress[1],
							prodSection.manufacturerSection.shortAddress[2],
							prodSection.manufacturerSection.shortAddress[3],
							module_shortAddr );
	#endif // #ifdef CONFIG_RTLS_PROTOCOL
}

OSAL_u8 sys_get_mcuid(OSAL_u8* unique_id)
{
	OSAL_u8 checkid[16]={'B','E','S','P',};
	OSAL_u32 *uid = (OSAL_u32*) SYS_UUID_VALUE;	
	OSAL_u8* getid=unique_id;
	OSAL_u8 i;
	if(unique_id==NULL)
	{
		checkid[0]='B';
		checkid[1]='E';
		checkid[2]='S';
		checkid[3]='P';
		getid=&checkid[4];
	}
	getid[8]  = (OSAL_u8)((uid[0]    )&0xFF);
	getid[9]  = (OSAL_u8)((uid[0]>>8 )&0xFF);
	getid[10] = (OSAL_u8)((uid[0]>>16)&0xFF);
	getid[11] = (OSAL_u8)((uid[0]>>24)&0xFF);
	getid[4]  = (OSAL_u8)((uid[1]    )&0xFF);
	getid[5]  = (OSAL_u8)((uid[1]>>8 )&0xFF);
	getid[6]  = (OSAL_u8)((uid[1]>>16)&0xFF);
	getid[7]  = (OSAL_u8)((uid[1]>>24)&0xFF);
	getid[0]  = (OSAL_u8)((uid[2]    )&0xFF);
	getid[1]  = (OSAL_u8)((uid[2]>>8 )&0xFF);
	getid[2]  = (OSAL_u8)((uid[2]>>16)&0xFF);
	getid[3]  = (OSAL_u8)((uid[2]>>24)&0xFF);
	
	if(unique_id==NULL)
	{
		for(i=0;i<16;i++)
		{
			if ((prodSection.manufacturerSection.longAddress[i] != checkid[i]))
			{
				OSAL_trace(TRACE_ANY,"MCUID SHOULD BE:");
				OSAL_trace(TRACE_ANY,"%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x",
							checkid[0],checkid[1],checkid[2],checkid[3],
							checkid[4],checkid[5],checkid[6],checkid[7],
							checkid[8],checkid[9],checkid[10],checkid[11],
							checkid[12],checkid[13],checkid[14],checkid[15]);
				return 1;
			}
		}
	}
	return 0;
}

//not in cache, to avoid to let them be reseted
OSAL_bool_t default_MSQ_ENABLE=OSAL_true;
OSAL_bool_t default_PM_ENABLE=OSAL_true;
OSAL_bool_t default_LTA_ENABLE=OSAL_true;

OSAL_void sys_enable_msq(OSAL_bool_t dfltMSQ_EN)
{
	default_MSQ_ENABLE = dfltMSQ_EN;
}

OSAL_void sys_enable_pm(OSAL_bool_t dfltPM_EN)
{
	default_PM_ENABLE = dfltPM_EN;
}

OSAL_void sys_enable_lta(OSAL_bool_t dfltLTA_EN)
{
	default_LTA_ENABLE = dfltLTA_EN;
}

OSAL_u8 sys_is_init_busy()
{
	return module_is_busy(MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_INIT_BUSY_MSK);
}

OSAL_void sys_set_init_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_INIT_BUSY_MSK);
}


static OSAL_u8 _sys_init(OSAL_u8** errMsg, OSAL_bool_t silent)
{
	OSAL_u8 ret = OSAL_ERROR;
	OSAL_u8* sn=NULL;
	sys_set_init_busy(OSAL_true);
	
	#ifdef PROD
		prod_api_init();
	#endif

	sys_init_firmware();
	sn=&prodSection.manufacturerSection.serial[0];
	if(silent == OSAL_false)
	{
		OSAL_trace(TRACE_ANY,FIRMWARE_NAME " v%d.%d",SW_VERSION_MAJ,SW_VERSION_MIN);
		OSAL_trace(TRACE_ANY,"UM100 S/N:%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
					sn[0],sn[1],sn[2],sn[3], sn[4],sn[5],sn[6],sn[7],
					sn[8],sn[9],sn[10],sn[11], sn[12],sn[13],sn[14],sn[15]);
	}

#ifndef PROD
	if(sys_get_mcuid(NULL))
	{
		if(errMsg) *errMsg= (OSAL_u8*)"MCU ID";
		goto out_of_init;
	}
	do{
		OSAL_u32 i;
		OSAL_u32 rslt;
		//avoid to loose stack
		//check testResults
		for(i=TEST_ID_NB;i<TEST_END;i++)
		{
			get_TestRslt(prodSection.manufacturerSection.testsResults,i,rslt);
			if(rslt ==0)
			{
				if(errMsg) *errMsg= (OSAL_u8*)"!!CAL";
				goto out_of_init;
			}
		}
	}while(0);
	do{
		// Prodsection stuff
		if(prodSection.userSection.generalCfg.prodSection_version_number < PRODSECTION_VERSION_NUMBER)
		{
			if(errMsg) *errMsg= (OSAL_u8*)"UPDATE UM100 WITH STM LOADER.PS OLDER THAN FIRMWARE";
			goto out_of_init;
		}
		if(prodSection.userSection.generalCfg.prodSection_version_number > PRODSECTION_VERSION_NUMBER)
		{
			if(errMsg) *errMsg= (OSAL_u8*)"UPDATE UM100 WITH STM LOADER.FIRMWARE OLDER THAN PS";
			goto out_of_init;
		}
		if (	pp_conf_crc16((OSAL_u8 *)&prodSection.userSection, sizeof(struct userSectionStruct)-2) != prodSection.userSection.crc ||
			pp_conf_crc16((OSAL_u8 *)&prodSection.burstSection, sizeof(struct burstSectionStruct)-2) != prodSection.burstSection.crc ||
			pp_conf_crc16((OSAL_u8 *)&prodSection.manufacturerSection, sizeof(struct manufacturerSectionStruct)-2) != prodSection.manufacturerSection.crc)
		{
			if(errMsg) *errMsg= (OSAL_u8*)"PS CRC";
			goto out_of_init;
		}
	}while(0);
#endif
	ctx.cfg_ctx = &contexte_config;
	ctx.sequencer.cfg_ctx = &contexte_config;
	pp_conf_init(ctx.cfg_ctx, default_PM_ENABLE,default_MSQ_ENABLE,default_LTA_ENABLE);
	#ifdef UWB_COM_CAN_ANSWER
	uwb_clean_queue(NULL);
	#endif //#ifdef UWB_COM_CAN_ANSWER
	sys_reset_status();
	sys_get_deflt_value();
	//CHECK_COMMON_SIZE_ENUM;
	//CHECK_COMMON_SIZE_STRUCT;
	if(radio_init() != OSAL_OK)
	{
		//OSAL_trace(TRACE_ERROR, "**RADIO INIT FAILED**");
		if(errMsg) *errMsg= (OSAL_u8*)"RADIO INIT";
		goto out_of_init;
	}
	#ifdef CONFIG_GENERIC
	if(seq_init() != OSAL_OK)
	{
		//OSAL_trace(TRACE_ERROR, "**SEQ INIT FAILED**");
		if(errMsg) *errMsg= (OSAL_u8*)"SEQ INIT";
		goto out_of_init;
	}
	#endif //#ifdef CONFIG_GENERIC
	#ifdef CONFIG_RTLS_PROTOCOL
	if(prot_init() != OSAL_OK)
	{
		//OSAL_trace(TRACE_ERROR, "**PROT INIT FAILED**");
		if(errMsg) *errMsg= (OSAL_u8*)"PROT INIT";
		goto out_of_init;
	}
	#endif //#ifdef CONFIG_RTLS_PROTOCOL

	#ifdef CONFIG_ACTIVATE_UART_SHELL
		#ifndef PROD
		if(prodSection.userSection.moduleCfg.shellConfig&SHELL_UARTSHELL_MASK)
		#endif
			sys_authorizesleep_after_X_sec(SYS_UART_DIMMER,SYS_WAKE_UP_UART_MASK);
	#endif
	#ifdef CONFIG_RTLS_PROTOCOL
	if(get_prot_autostart())
	{
		prot_ctrl(UWB_SC_START);
	}
	#endif
	sys_set_init_busy(OSAL_false);
	ret=OSAL_OK;
out_of_init:
	return ret;
}

static OSAL_u8 _sys_reinit(OSAL_bool_t silent)
{
	//don't generate any IT on reset
	sys_set_init_busy(OSAL_true);
	sys_set_status_mask(UWB_STAT_MASK_NONE);
	//can only be called after sys_init has been called
	//be sure everything is freed on seq or protocol side before doing anything else
	#ifdef CONFIG_RTLS_PROTOCOL
	prot_init();
	#endif //#ifdef CONFIG_RTLS_PROTOCOL
	#ifdef CONFIG_GENERIC
	seq_init();
	#endif //#ifdef CONFIG_GENERIC
	return _sys_init(NULL,silent);
}

OSAL_u8 sys_reinit()
{
	return _sys_reinit(OSAL_false);
}

OSAL_u8 sys_init(OSAL_u8** errMsg)
{
	return _sys_init(errMsg,OSAL_false);
}

static OSAL_u32 traccnt = 100000;//((unsigned long)hardfault_args[7]) ;
static void sys_hardFault_save(unsigned long *hardfault_args)
{
	uwb_com_command_t cmd;
	if(traccnt==100000)
	{
		volatile OSAL_u32 stacked_r0 = ((OSAL_u32)hardfault_args[0]) ;
		volatile OSAL_u32 stacked_r1 = ((OSAL_u32)hardfault_args[1]) ;
		volatile OSAL_u32 stacked_r2 = ((OSAL_u32)hardfault_args[2]) ;
		volatile OSAL_u32 stacked_r3 = ((OSAL_u32)hardfault_args[3]) ;
		volatile OSAL_u32 stacked_r12 = ((OSAL_u32)hardfault_args[4]) ;
		volatile OSAL_u32 stacked_lr = ((OSAL_u32)hardfault_args[5]) ;
		volatile OSAL_u32 stacked_pc = ((OSAL_u32)hardfault_args[6]) ;
		volatile OSAL_u32 stacked_psr = ((OSAL_u32)hardfault_args[7]) ;
		traccnt=100;
		printline("[Hard fault]");
		printline("R0: 0x%.8x R1: 0x%.8x", stacked_r0,stacked_r1);
		printline("R2: 0x%.8x R3: 0x%.8x", stacked_r2,stacked_r3);
		printline("R12:0x%.8x", stacked_r12);
		printline("LR[R14]: 0x%x  call return address", stacked_lr);
		printline("PC[R15]: 0x%x  counter", stacked_pc);
		printline("PSR: 0x%.8x", stacked_psr);
		//printline("BFAR:0x%.8x", (OSAL_u32)(*((volatile OSAL_u32 *)(0xE000ED38))));
		//printline("CFSR:0x%.8x", (OSAL_u32)(*((volatile OSAL_u32 *)(0xE000ED28))));
		//printline("HFSR:0x%.8x", (OSAL_u32)(*((volatile OSAL_u32 *)(0xE000ED2C))));
		//printline("DFSR:0x%.8x", (OSAL_u32)(*((volatile OSAL_u32 *)(0xE000ED30))));
		//printline("AFSR:0x%.8x", (OSAL_u32) (*((volatile OSAL_u32 *)(0xE000ED3C))));
		//printline("SCB_SHCSR:0x%.8x", (OSAL_u32) SCB->SHCSR);
		traccnt=0;
	}
	uwb_dequeue_cmd(&cmd);
	traccnt++;
}

OSAL_void sys_manage_hardFault(unsigned long *hardfault_args)
{
	//hardfault, so some variable should be put to unknown state
	radio_manage_hardFault();
	#ifdef CONFIG_GENERIC
	seq_manage_hardFault();
	#endif
	#ifdef CONFIG_RTLS_PROTOCOL
	prot_manage_hardFault();
	#endif
	sys_set_status(UWB_STAT_DEAD);
	sys_check_status();
	module_config[MOD_CFG_POS_SYS_HARDFAULT]=0xFF;
	sys_hardFault_save(hardfault_args);
}

static OSAL_void sys_authorizesleep_after_tval(OSAL_timeval* addedTval)
{
	OSAL_timeval add,cur,newdimmer,diff;
	add.tv_sec=addedTval->tv_sec;
	add.tv_usec=addedTval->tv_usec;

	OSAL_get_systemtime(&add);
	OSAL_gettimeofday(&cur,NULL);
	//correct the time
	OSAL_timeradd(&cur,&add,&newdimmer);
	if(newdimmer.tv_sec>=86400)//above 24hour, back to 0
		newdimmer.tv_sec-=86400;
	if(module_config[MOD_CFG_POS_SYS_DIMMER_STATUS])
	{
		if(OSAL_timercmp(&newdimmer,&sys_dimmer_before_sleep,'>'))
		{
			OSAL_timersub(&newdimmer,&sys_dimmer_before_sleep,&diff);
			//newdimmer is bigger than sys_dimmer_before_sleep
			//since we cannot dim more than 1 hour (lets say 2h with correction, just to be (very) large), if difference betwen both is larger than 2h (7200sec)
			//this mean in reality that sys_dimmer_before_sleep is currently above newdimmer from one day.  so keep it, its already bigger than what needed in this function call
			//otherwise, newdimmer is the new dimmer to take into account
			if(diff.tv_sec<7200)
			{
				
				sys_dimmer_before_sleep.tv_sec = newdimmer.tv_sec;
				sys_dimmer_before_sleep.tv_usec = newdimmer.tv_usec;
				return;
			}
		}
		else
		{
			OSAL_timersub(&sys_dimmer_before_sleep,&newdimmer,&diff);
			//sys_dimmer_before_sleep is bigger than newdimmer
			//since we cannot dim more than 1 hour (lets say 2h with correction, just to be (very) large), if difference betwen both is larger than 2h(7200sec)
			//this mean in reality that newdimmer is currently above sys_dimmer_before_sleep from one day. so newdimmer is the new dimmer to take into account
			//otherwise, sys_dimmer_before_sleep must be kept, its already bigger than what needed in this function call
			if(diff.tv_sec>7200)
			{
				
				sys_dimmer_before_sleep.tv_sec = newdimmer.tv_sec;
				sys_dimmer_before_sleep.tv_usec = newdimmer.tv_usec;
				return;
			}
		}
	}
	else
	{
		// a dimmer has been requested
		sys_dimmer_before_sleep.tv_sec = newdimmer.tv_sec;
		sys_dimmer_before_sleep.tv_usec = newdimmer.tv_usec;
		module_config[MOD_CFG_POS_SYS_DIMMER_STATUS]=1;
	}
}

OSAL_void sys_authorizesleep_after_X_sec(OSAL_u32 timeInSec, OSAL_u8 wake_up_reason )
{
	OSAL_timeval add;
	add.tv_sec = timeInSec;
	if(add.tv_sec>3599) add.tv_sec=3599;//saturate
	add.tv_usec = 0;
	sys_authorizesleep_after_tval(&add);
}

OSAL_void sys_authorizesleep_after_X_msec(OSAL_u32 timeInMSec, OSAL_u8 wake_up_reason )
{
	OSAL_timeval add;
	add.tv_sec = timeInMSec/1000;
	if(add.tv_sec>3599) {add.tv_sec=3599;add.tv_usec =0;}//saturate
	else
		add.tv_usec = (timeInMSec - add.tv_sec*1000)*1000;
	sys_authorizesleep_after_tval(&add);
}

OSAL_void sys_authorizesleep_after_X_usec(OSAL_u32 timeInUSec, OSAL_u8 wake_up_reason )
{
	OSAL_timeval add;
	add.tv_sec = timeInUSec/1000000;
	if(add.tv_sec>3599) {add.tv_sec=3599;add.tv_usec =0;}//saturate
	else
		add.tv_usec = timeInUSec-add.tv_sec*1000000;
	sys_authorizesleep_after_tval(&add);
}


OSAL_void sys_get_wake_up_reason(OSAL_u8* reason)
{
	*reason =module_config[MOD_CFG_POS_SYS_WAKE_UP_REASON];
}

OSAL_void sys_set_wake_up_reason(OSAL_u8 mask)
{
	PWR_PreventFromDeepSleep(ENABLE);
	module_config[MOD_CFG_POS_SYS_WAKE_UP_REASON] |= mask;
	module_config[MOD_CFG_POS_SYS_MCU_STATE] &= ~(MOD_CFG_POS_SYS_MCU_IS_SLEEPING);
}

OSAL_void sys_reset_wake_up_reason(OSAL_u8 mask)
{
	module_config[MOD_CFG_POS_SYS_WAKE_UP_REASON] &= (~mask);
}

OSAL_void sys_get_info(OSAL_u8 infoType, OSAL_u8 infoPrm, OSAL_u8** infoBuf, OSAL_u16* infoBufLen)
{
	OSAL_u8* data= &module_config[MOD_CFG_POS_SHARE_BUFFER_0_DATA];
	OSAL_u8 zone,channel,vreg,channelp1;
	OSAL_u32 i=0;
	switch(infoType)
	{
		case UWB_SC_INFO_VERSION:
			*infoBufLen = 10;
			//sw version
			data[0] = MAJMIN_TO_VERSION(SW_VERSION_MAJ,SW_VERSION_MIN);
			//hw version, obsolete
			data[1] = MAJMIN_TO_VERSION(HW_VERSION_MAJ,HW_VERSION_MIN);
			//chipversion
			U32VAL_TO_MSBLSB(prodSection.manufacturerSection.chipVersion,data[2],data[3],data[4],data[5]);
			//emultagversion
			U32VAL_TO_MSBLSB(prodSection.manufacturerSection.emultagVersion,data[6],data[7],data[8],data[9]);
			break;
		case UWB_SC_INFO_CAPS:
			*infoBufLen = 1;
			#ifdef CONFIG_RTLS_PROTOCOL
				data[0]   = 1;
			#else
				data[0]   = 0;
			#endif
			break;
		case UWB_SC_INFO_LONGADDR:
		case UWB_SC_INFO_MACADDR:
			*infoBufLen = 16;
			OSAL_memcpy(&data[0],&prodSection.manufacturerSection.longAddress[0],16);
			break;
		case UWB_SC_INFO_SHORTADDR:
			*infoBufLen = 4;
			U32VAL_TO_MSBLSB(module_shortAddr,data[0],data[1],data[2],data[3]);
			break;
		case UWB_SC_INFO_SN:
			*infoBufLen = SERIAL_NB_DIGIT_MAX;
			OSAL_memcpy(&data[0],&prodSection.manufacturerSection.serial,SERIAL_NB_DIGIT_MAX);
			break;
		case UWB_SC_INFO_THRESH:
			*infoBufLen = 4;
			zone = ((infoPrm&0xF0)>>4);
			channel = (infoPrm&0xF);
			U32VAL_TO_MSBLSB(	prodSection.manufacturerSection.thresholds[zone<5?zone:0][channel<8?channel:0],
						data[0],data[1],data[2],data[3]);
			break;
		case UWB_SC_INFO_TRESULT:
			*infoBufLen = 4;
			U32VAL_TO_MSBLSB(prodSection.manufacturerSection.testsResults,data[0],data[1],data[2],data[3]);
			break;
		case UWB_SC_INFO_VOLTAGE:
			*infoBufLen = 1;
			vreg = (infoPrm==1)?BAR_FS_TUNE_VDD1V2_PLLNUM:((infoPrm==2)?BAR_FS_TUNE_VDD1V2_PLL:BAR_TX_TUNE_VDD1V2_TX);
			while (prodSection.manufacturerSection.channelDependentReg[i].bank != 0x00 ||
				prodSection.manufacturerSection.channelDependentReg[i].address != 0x00 ||
				prodSection.manufacturerSection.channelDependentReg[i].val[0] != 0x00
			)
			{
				if (prodSection.manufacturerSection.channelDependentReg[i].bank == BAR_CFG_ANALOG &&
					prodSection.manufacturerSection.channelDependentReg[i].address == vreg)
				{
					data[0] = prodSection.manufacturerSection.channelDependentReg[i].val[0];
					break;
				}
				i++;
			}
			break;
		case UWB_SC_INFO_SPVTUNE:
			*infoBufLen = 1;
			channelp1 = infoPrm;
			if(channelp1 < 1 || ((channelp1 > 4)&&(channelp1 < 8)) || (channelp1>11))
				channel = 2;
			while (prodSection.manufacturerSection.channelDependentReg[i].bank != 0x00 ||
				prodSection.manufacturerSection.channelDependentReg[i].address != 0x00 ||
				prodSection.manufacturerSection.channelDependentReg[i].val[0] != 0x00
			)
			{
				if (prodSection.manufacturerSection.channelDependentReg[i].bank == BAR_CFG_ANALOG &&
					prodSection.manufacturerSection.channelDependentReg[i].address == BAR_TX_SP_VTUNE)
				{
					data[0] = prodSection.manufacturerSection.channelDependentReg[i].val[channelp1-1];
					break;
				}
				i++;
			}
			break;
		#ifdef PROD 
		case UWB_SC_INFO_PROD_RESULT:
			sys_prod_result_get_buf(infoBuf, infoBufLen);
			return;
		#endif
		default:
			*infoBuf = NULL;
			return;
			break;
	}
	sys_set_shared_data_size(0,*infoBufLen);
	sys_get_shared_buf(0,infoBuf,infoBufLen);
}


OSAL_u8 sys_is_mcu_stopped()
{
	return (module_config[MOD_CFG_POS_SYS_MCU_STATE]&MOD_CFG_POS_SYS_MCU_IS_SLEEPING)?1:0;
}

#define SYS_TRACE_SLEEP_
#ifdef SYS_TRACE_SLEEP
	#define sys_sleep_trace(fmt,args...) do{OSAL_trace(TRACE_INFO,fmt,##args);}while(0)
#else
	#define sys_sleep_trace(fmt,args...) do{}while(0)
#endif

OSAL_void sys_stop_mcu()
{
	OSAL_u8 wake_up_reason=0;
	#if !defined(PROD) || (defined(PROD_WITHAPI)) //in PROD mode, we never sleep, nor standby
	if(module_config[MOD_CFG_POS_SYS_DIMMER_STATUS])
	{
		OSAL_timeval cur,diff;
		//we can sleep only if:
		//the current time is above the sys_dimmer_before_sleep
		OSAL_gettimeofday(&cur,NULL);
		if(OSAL_timercmp(&cur,&sys_dimmer_before_sleep,'>'))
		{
			OSAL_timersub(&cur,&sys_dimmer_before_sleep,&diff);
			//current is bigger than sys_dimmer_before_sleep
			//since we cannot dim more than 1 hour (lets say 2h with correction, just to be (very) large), if difference betwen both is larger than 2h(7200sec)
			//this mean in reality that sys_dimmer_before_sleep is currently above newdimmer from one day.  so dont sleep. we must still wait
			//otherwise, dimmer elapsed, and we can sleep
			if(diff.tv_sec>=7200) goto beach;
		}
		else
		{
			OSAL_timersub(&sys_dimmer_before_sleep,&cur,&diff);
			//sys_dimmer_before_sleep is bigger than current
			//since we cannot dim more than 1 hour (lets say 2h with correction, just to be (very) large), if difference betwen both is larger than 2h(7200sec)
			//this mean in reality that current is abode sys_dimmer_before_sleep from one day. so sleep can be done
			//otherwise, dont sleep. we must still wait
			if(diff.tv_sec<7200) goto beach;
		}
		sys_reset_dimmer();
	}
	//module configured to run
	if(module_config[MOD_CFG_POS_SYS_MCU_STATE]&MOD_CFG_POS_SYS_MCU_MUST_RUN)
	{
		sys_sleep_trace("MUST RUN");
		goto beach;
	}
	//force to run by hw
	if((module_config[MOD_CFG_POS_SYS_MCU_STATE]&MOD_CFG_POS_SYS_MCU_RUN_FORCED_MASK) != 0)
	{
		sys_sleep_trace("FORCE RUN");
		goto beach;
	}
	if(PWR_IsPreventedFromDeepSleep() == ENABLE)
	{
		sys_sleep_trace("IT AWAITING");
		goto beach;
	}
	//otherwise, we can sleep, or standby
	module_config[MOD_CFG_POS_SYS_MCU_STATE] |= (MOD_CFG_POS_SYS_MCU_IS_SLEEPING);
	//reset timer for next step
	if(module_config[MOD_CFG_POS_SYS_MCU_STATE]&MOD_CFG_POS_SYS_MCU_CAN_STOP)
	{
		sys_sleep_trace("SLEEP IN");
		//HAL_MCU_Sleep_Mode(0);
		HAL_MCU_Stop_Mode(0);
		HAL_Out_of_LowPower_Mode();
		//reset time counter
		sys_sleep_trace("SLEEP OUT(%x)",module_config[MOD_CFG_POS_SYS_WAKE_UP_REASON]);
	}
	else
	{
		sys_sleep_trace("STANDBY IN");
		HAL_MCU_Standby_Mode();
		HAL_Out_of_LowPower_Mode();
		sys_sleep_trace("STANDBY OUT(%x)",module_config[MOD_CFG_POS_SYS_WAKE_UP_REASON]);
	}
	module_config[MOD_CFG_POS_SYS_MCU_STATE] &= ~(MOD_CFG_POS_SYS_MCU_IS_SLEEPING);
beach:
	PWR_PreventFromDeepSleep(DISABLE);
	#endif // #if !defined(PROD) || (defined(PROD_WITHAPI)) //in PROD mode, we never sleep, nor standby
	sys_get_wake_up_reason(&wake_up_reason);
	// Check wake up source
	if(wake_up_reason&SYS_WAKE_UP_MASTER_MASK)
	{
		sys_reset_wake_up_reason(SYS_WAKE_UP_MASTER_MASK);
	}
	if (wake_up_reason&SYS_WAKE_UP_PP_MASK)
	{
		sys_reset_wake_up_reason(SYS_WAKE_UP_PP_MASK);
	}
	if(wake_up_reason&SYS_WAKE_UP_SPI_MASK)
	{
		sys_reset_wake_up_reason(SYS_WAKE_UP_SPI_MASK);
		if(EXTCOM_BUF_PCKTTRACE > 1)
			sys_authorizesleep_after_X_sec(EXTCOM_BUF_PCKTTRACE,SYS_WAKE_UP_SPI_MASK);
		else if(EXTCOM_BUF_PCKTTRACE == 1)
			sys_authorizesleep_after_X_msec(300,SYS_WAKE_UP_SPI_MASK);
		else //use the worst case of wait time form the other side, to be sure nothing lost
			sys_authorizesleep_after_X_msec(UWB_C_DFLT_BUSY_DELAY5_MS,SYS_WAKE_UP_SPI_MASK);
	}
	if(wake_up_reason&SYS_WAKE_UP_UART_MASK)
	{
		sys_reset_wake_up_reason(SYS_WAKE_UP_UART_MASK);
		#ifdef CONFIG_ACTIVATE_UART_SHELL
			#ifndef PROD
			if(prodSection.userSection.moduleCfg.shellConfig&SHELL_UARTSHELL_MASK)
			#endif
				sys_authorizesleep_after_X_sec(SYS_UART_DIMMER,SYS_WAKE_UP_UART_MASK);
		#endif
	}
	if (wake_up_reason&SYS_WAKE_UP_ACC_LOW_MASK)
	{
		sys_reset_wake_up_reason(SYS_WAKE_UP_ACC_LOW_MASK);
		module_config[MOD_CFG_POS_SYS_MODULE_MOVING]=1;
	}
	if (wake_up_reason&SYS_WAKE_UP_ACC_HIGH_MASK)
	{
		sys_reset_wake_up_reason(SYS_WAKE_UP_ACC_HIGH_MASK);
		module_config[MOD_CFG_POS_SYS_MODULE_MOVING]=0;
	}
	if (wake_up_reason&SYS_WAKE_UP_RTC_MASK)
		sys_reset_wake_up_reason(SYS_WAKE_UP_RTC_MASK);
}

OSAL_void sys_get_mcu_state(uwb_com_subcmd_e* state)
{
	*state = module_config[MOD_CFG_POS_SYS_MCU_STATE];
}

OSAL_void sys_force_mcu_run(OSAL_u8 forceRun)
{
	if(forceRun) module_config[MOD_CFG_POS_SYS_MCU_STATE] |= MOD_CFG_POS_SYS_MCU_RUN_FORCED_MASK;
	else module_config[MOD_CFG_POS_SYS_MCU_STATE] &= ~MOD_CFG_POS_SYS_MCU_RUN_FORCED_MASK;
}

OSAL_void sys_set_mcu_state(uwb_com_subcmd_e state)
{
	if(state<UWB_SC_MCU_RESET)
	{
		module_config[MOD_CFG_POS_SYS_MCU_STATE] &= MOD_CFG_POS_SYS_MCU_SM_RESET_MASK;
		if(state==UWB_SC_MCU_STANDBY)
			module_config[MOD_CFG_POS_SYS_MCU_STATE] |= MOD_CFG_POS_SYS_MCU_CAN_STANDBY;
		else if(state==UWB_SC_MCU_STOPPED)
			module_config[MOD_CFG_POS_SYS_MCU_STATE] |= MOD_CFG_POS_SYS_MCU_CAN_STOP;
		else
			module_config[MOD_CFG_POS_SYS_MCU_STATE] |= MOD_CFG_POS_SYS_MCU_MUST_RUN;
	}
	else
	{
		OSAL_bool_t msq=OSAL_false,pm=OSAL_false,lta=OSAL_false;
		switch(state)
		{
			case UWB_SC_MCU_RESET:/**< Sub command reset mcu with msq ON, PM ON, lta ON (default)*/
				msq=OSAL_true;
				pm=OSAL_true;
				lta=OSAL_true;
				break;
			case UWB_SC_MCU_RESET_011:/**< Sub command reset mcu with msq OFF, PM ON, lta ON  */
				msq=OSAL_false;
				pm=OSAL_true;
				lta=OSAL_true;
				break;
			case UWB_SC_MCU_RESET_101:/**< Sub command reset mcu with msq ON, PM OFF, lta ON */
				msq=OSAL_true;
				pm=OSAL_false;
				lta=OSAL_true;
				break;
			case UWB_SC_MCU_RESET_001:/**< Sub command reset mcu with msq OFF, PM OFF, lta ON */
				msq=OSAL_false;
				pm=OSAL_false;
				lta=OSAL_true;
				break;
			case UWB_SC_MCU_RESET_110:/**< Sub command reset mcu with msq ON, PM ON, lta OFF */
				msq=OSAL_true;
				pm=OSAL_true;
				lta=OSAL_false;
				break;
			case UWB_SC_MCU_RESET_010:/**< Sub command reset mcu with msq OFF, PM ON, lta OFF */
				msq=OSAL_false;
				pm=OSAL_true;
				lta=OSAL_false;
				break;
			case UWB_SC_MCU_RESET_100:/**< Sub command reset mcu with msq ON, PM OFF, lta OFF */
				msq=OSAL_true;
				pm=OSAL_false;
				lta=OSAL_false;
				break;
			default:
				break;
		}
		sys_enable_msq(msq);
		sys_enable_pm(pm);
		sys_enable_lta(lta);
		sys_reinit();
	}
}

static OSAL_void sys_rtc_deepSleep(OSAL_u32 seconds, OSAL_u32 useconds)
{
	OSAL_timeval tim;
	OSAL_u8 wakeUpReason=0;
	tim.tv_sec=seconds;
	tim.tv_usec=useconds;
	HAL_RTC_StartAlarm(&tim);
	//HAL_MCU_Sleep_Mode(0);
	while((wakeUpReason&SYS_WAKE_UP_RTC_MASK)==0)
	{
		HAL_MCU_Stop_Mode(0);
		//HAL_MCU_Standby_Mode();
		HAL_Out_of_LowPower_Mode();
		sys_get_wake_up_reason(&wakeUpReason);
	}
	sys_reset_wake_up_reason(SYS_WAKE_UP_RTC_MASK);
}

OSAL_void sys_rtc_wake_up_after(OSAL_u32 seconds, OSAL_u32 useconds)
{
	OSAL_u8 cfg=module_config[MOD_CFG_POS_SYS_MCU_STATE];
	//we can sleep only if:
	//the time before sleep is authorizedf has elapsed
	sys_reset_dimmer();
	PWR_PreventFromDeepSleep(DISABLE);
	//SYSTEM DEFAULT VALUE
	sys_reset_wake_up_reason(SYS_WAKE_UP_ALL_MASK);
	//authorize stopped state
	module_config[MOD_CFG_POS_SYS_MCU_STATE] = 0;
	sys_set_mcu_state(UWB_SC_MCU_STOPPED);
	sys_rtc_deepSleep(seconds,useconds);
	module_config[MOD_CFG_POS_SYS_MCU_STATE]=cfg;
}

OSAL_void sys_rtc_set_fineCorrection(OSAL_u64 fineTune)
{
	OSAL_u64 c=fineTune;
	HAL_RTC_SetFineTune(NULL,&c);
}

OSAL_void sys_rtc_get_corrections(OSAL_u64* freqCorrection, OSAL_u64* tuneCorrection)
{
	HAL_RTC_GetFineTune(freqCorrection,tuneCorrection);
}

OSAL_void sys_rtc_set_precisionFactor(OSAL_u8 factor)
{
	HAL_RTC_SetPrecisionFactor(factor);
	sys_reinit();
}

OSAL_void sys_rtc_get_precisionFactor(OSAL_u8* factor)
{
	HAL_RTC_GetPrecisionFactor(factor);
}

OSAL_u8 sys_is_dac_val_busy()
{
	return module_is_busy(MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_DAC_VAL_BUSY_MSK);
}

OSAL_void sys_set_dac_val_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_DAC_VAL_BUSY_MSK);
}

OSAL_void sys_get_dac_val(OSAL_u16* dacVal)
{
	MSBLSB_TO_U16VAL( 	module_config[MOD_CFG_POS_SYS_DAC_VAL_MSB],
				module_config[MOD_CFG_POS_SYS_DAC_VAL_LSB],
				*dacVal);
}

OSAL_void sys_set_dac_val(OSAL_u16 dacVal)
{
	U16VAL_TO_MSBLSB(	dacVal,
				module_config[MOD_CFG_POS_SYS_DAC_VAL_MSB],
				module_config[MOD_CFG_POS_SYS_DAC_VAL_LSB]);
	//TODO on real HW dac_set_val
	dac_set_value(dacVal);
}

OSAL_u8 sys_is_capatune_busy()
{
	return module_is_busy(MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_CAPATUNE_BUSY_MSK);
}

OSAL_void sys_set_capatune_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_CAPATUNE_BUSY_MSK);
}

OSAL_void sys_get_capatune_val(OSAL_u8* capatune)
{
	*capatune = module_config[MOD_CFG_POS_SYS_CAPATUNE_VAL];
}

OSAL_void sys_set_capatune_val(OSAL_u8 capatune)
{
	//capatune is on 6bits
	if(capatune>0x3F) module_config[MOD_CFG_POS_SYS_CAPATUNE_VAL] = 0x3F;
	else module_config[MOD_CFG_POS_SYS_CAPATUNE_VAL] = capatune&0x3F;
	pp_hw_reg_write(    0,  BAR_CFG_ANALOG, BAR_MISC_TUNE_OSCQ32K_CAPATUN,module_config[MOD_CFG_POS_SYS_CAPATUNE_VAL]);
}

OSAL_u8 sys_is_rxframe_cfg_busy()
{
	return module_is_busy(MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_RXFRAME_CFG_BUSY_MSK);
}

OSAL_void sys_set_rxframe_cfg_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_RXFRAME_CFG_BUSY_MSK);
}

OSAL_void sys_get_rxframe_cfg(OSAL_u8* cfg)
{
	*cfg = module_config[MOD_CFG_POS_SYS_RXFRAME_CFG];
}

OSAL_void sys_set_rxframe_cfg(OSAL_u8 cfg)
{
	module_config[MOD_CFG_POS_SYS_RXFRAME_CFG] = cfg;
}

OSAL_u8 sys_is_statusmsk_busy()
{
	return module_is_busy(MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_STATUS_MSK_BUSY_MSK);
}

OSAL_void sys_set_statusmsk_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_STATUS_MSK_BUSY_MSK);
}

OSAL_void sys_get_status_mask(OSAL_u16* mask)
{
	MSBLSB_TO_U16VAL(	module_config[MOD_CFG_POS_SYS_STATUS_MSK_MSB],
				module_config[MOD_CFG_POS_SYS_STATUS_MSK_LSB],
				*mask);
}

OSAL_void sys_set_status_mask(OSAL_u16 mask)
{
	OSAL_u16 masked=mask;
	masked &=(~UWB_STAT_DEAD);
	U16VAL_TO_MSBLSB(	masked,
				module_config[MOD_CFG_POS_SYS_STATUS_MSK_MSB],
				module_config[MOD_CFG_POS_SYS_STATUS_MSK_LSB]);
}

OSAL_void sys_check_status()
{
	//check sys status msk
	OSAL_u16 status;
	sys_get_status(&status);
	if(status)
	{
		#ifdef UWB_COM_CAN_SEND
		if(GPIO_ReadOutputDataBit(GPIO_RECEIVE_M1_IT, GPIO_PIN_RECEIVE_M1_IT))
			GPIO_ResetBits(GPIO_RECEIVE_M1_IT, GPIO_PIN_RECEIVE_M1_IT);
		#endif
		#ifndef HAS_GREEN_LED
		if(GPIO_ReadOutputDataBit(GPIO_MODULEIT, GPIO_PIN_MODULEIT))
			GPIO_ResetBits(GPIO_MODULEIT, GPIO_PIN_MODULEIT);
		#endif //CONFIG_MINITAG_V2
	}
}

OSAL_void sys_reset_status()
{
	module_config[MOD_CFG_POS_SYS_STATUS_MSB]=0;
	module_config[MOD_CFG_POS_SYS_STATUS_LSB]=0;
	#ifdef UWB_COM_CAN_SEND
	if(!GPIO_ReadOutputDataBit(GPIO_RECEIVE_M1_IT, GPIO_PIN_RECEIVE_M1_IT))
		GPIO_SetBits(GPIO_RECEIVE_M1_IT, GPIO_PIN_RECEIVE_M1_IT);
	#endif
	#ifndef CONFIG_MINITAG_V2
	if(!GPIO_ReadOutputDataBit(GPIO_MODULEIT, GPIO_PIN_MODULEIT))
		GPIO_SetBits(GPIO_MODULEIT, GPIO_PIN_MODULEIT);
	#endif //CONFIG_MINITAG_V2
}

OSAL_void sys_get_status(OSAL_u16* status)
{
	//this is done under it, so its higher priority
	MSBLSB_TO_U16VAL(	module_config[MOD_CFG_POS_SYS_STATUS_MSB],
				module_config[MOD_CFG_POS_SYS_STATUS_LSB],
				*status);
}

OSAL_void sys_set_status(OSAL_u16 status)
{
	OSAL_u16 maskedStatus;
	OSAL_u16 mask;
	sys_get_status_mask(&mask);
	maskedStatus = (status&(~mask));
	#ifdef CONFIG_RTLS_PROTOCOL
		maskedStatus&=~UWB_STAT_IS_PROTOCOL;
	#endif
	if(maskedStatus == UWB_STAT_NONE) return;
	#ifdef CONFIG_RTLS_PROTOCOL
	if(mask&UWB_STAT_IS_PROTOCOL)
		maskedStatus |=	UWB_STAT_IS_PROTOCOL;
	#endif
	U16VAL_ADDED_TO_MSBLSB( maskedStatus,
				module_config[MOD_CFG_POS_SYS_STATUS_MSB],
				module_config[MOD_CFG_POS_SYS_STATUS_LSB]);
}

OSAL_u8 sys_is_checkcache_busy()
{
	return module_is_busy(MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_CHECKCACHE_BUSY_MSK);
}

OSAL_void sys_set_checkcache_busy(OSAL_u8 busyState)
{
	module_set_busy(busyState,MOD_CFG_POS_SYS_BUSY,MOD_CFG_POS_SYS_CHECKCACHE_BUSY_MSK);
}

OSAL_void sys_checkcache(OSAL_u8 checkId)
{
	//default

	module_config[MOD_CFG_POS_SYS_CHECKCACHE]=1;//rsvd id, or software things return 0, it necessarily ok
	if(checkId<=SYS_LAST_CMD)
	{
		switch(checkId)
		{
			default:break;
			case SYS_GET_DAC_VAL:
			case SYS_SET_DAC_VAL:{
				OSAL_u16 dv=0,cacheDV=0;
				sys_get_dac_val(&cacheDV);
				dac_get_value(&dv);
				if(cacheDV != dv) module_config[MOD_CFG_POS_SYS_CHECKCACHE]=0;
				break;}
			case SYS_GET_CAPATUNE_VAL:
			case SYS_SET_CAPATUNE_VAL:{
				OSAL_u8 cp=0,cacheCP=0;
				sys_get_capatune_val(&cacheCP);
				pp_hw_reg_read(    0,  BAR_CFG_ANALOG, BAR_MISC_TUNE_OSCQ32K_CAPATUN,&cp);
				if(cacheCP != cp) module_config[MOD_CFG_POS_SYS_CHECKCACHE]=0;
				break;}
		}
	}
	else if((checkId>=RADIO_BASE_CMD)&&(checkId<=RADIO_LAST_CMD)) radio_checkcache(checkId);
	#ifdef CONFIG_GENERIC
	else if((checkId>=SEQUENCER_BASE_CMD)&&(checkId<=SEQ_LAST_CMD)) seq_checkcache(checkId);
	#endif //#ifdef CONFIG_GENERIC
	#ifdef CONFIG_RTLS_PROTOCOL
	else if((checkId>=PROTOCOL_BASE_CMD)&&(checkId<=PROT_LAST_CMD)) prot_checkcache(checkId);
	#endif
}

OSAL_void sys_check_sysid_cache(OSAL_u8 checkId)
{
	//default

}

OSAL_void sys_get_checkcache(OSAL_u8* ok)
{
	*ok = module_config[MOD_CFG_POS_SYS_CHECKCACHE];
}


#ifdef CONFIG_RTLS_UNITTESTS
#define BURST_TEST_LEN 16
#define SEQ_TEST_LEN
static OSAL_u8 burst_data[BURST_TEST_LEN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
struct setting tab[SEQ_TEST_LEN] =  {
	{T_REG8,	BAR_CFG_ANALOG,	BAR_FS_ENABLE,			0,{.t8 = 0xAA}},
	{T_REG16,	BAR_CFG_ANALOG,	BAR_RXFE_TUNE_VTUNE_LNA,	0,{.t16 = 0xBBCC}},
	{T_REG32,	BAR_CFG_32K,	BAR_32K_TIMER_PERIOD_W0,	0,{.t32 = 0xEEFFAABB}},
	{T_BURST,	BAR_CFG_DIGITAL,BAR_SYNC_CODE_W0,		0,{.tburst = 0}},
	{T_DELAY,	0,		0,				0,{.tdelay = 100}},
	{T_STOP ,	0,		0,				0,{.t8 = 0}} };

static OSAL_void sys_tls_unittest(OSAL_void)
{
	OSAL_u8 data_[16];
	OSAL_u16 dr_data;
	OSAL_u32 r32_data;
	OSAL_u8* p_burst;
	OSAL_u16 i;
	OSAL_msleep(1000);

	//single Reg Write/read test
	pp_hw_reg_write(    0,  BAR_CFG_DIGITAL,BAR_SYNC_SYMB_REP,      0xaa);
	pp_hw_reg_read(     0,  BAR_CFG_DIGITAL,BAR_SYNC_SYMB_REP,      &data_[0]);

	pp_hw_reg16_write(  0,  BAR_CFG_32K,    BAR_32K_TIMER_PERIOD_W0,0x3061);
	pp_hw_reg16_read(   0,  BAR_CFG_32K,    BAR_32K_TIMER_PERIOD_W0,&dr_data);

	pp_hw_reg32_write(  0,  BAR_CFG_32K,    BAR_32K_TIMER_PERIOD_W0,0x30610221);
	pp_hw_reg32_read(   0,  BAR_CFG_32K,    BAR_32K_TIMER_PERIOD_W0,&r32_data);

	pp_hw_burst_write(  0,  BAR_CFG_DIGITAL,BAR_SYNC_CODE_W0,       &burst_data[0],BURST_TEST_LEN);
	pp_hw_burst_read(   0,  BAR_CFG_DIGITAL,BAR_SYNC_CODE_W0,       &data_[0],BURST_TEST_LEN);

	// Seq test
	pp_hw_seq_write(0,tab);
	for(i=0; i< SEQ_TEST_LEN; i++)
		tab[i].Val.t32 = 0;
	for(i=0; i< BURST_TEST_LEN; i++)
		data_[i] = 0;
	tab[3].Val.t32 = (OSAL_u32)data_;		// indicate the burst adress read destination

	pp_hw_seq_read(0, tab);
}
#endif //#ifdef CONFIG_RTLS_UNITTESTS

OSAL_void sys_process()
{
	//Highest priority : do Bespoon protocol process
	if(module_config[MOD_CFG_POS_SYS_HARDFAULT]) return;
	#ifdef CONFIG_RTLS_PROTOCOL
	prot_process();
	#endif //#ifdef CONFIG_RTLS_PROTOCOL
	#ifdef CONFIG_GENERIC
	seq_process();
	radio_process();
	#endif //#ifdef CONFIG_GENERIC

	sys_update_battery_and_temperature_info();

	#ifdef CONFIG_RTLS_UNITTESTS
		sys_tls_unittest()
	#endif
	sys_check_status();
}

//#define BAT_ON_2BIT
//#define BAT_ON_3BIT
#if defined(BAT_ON_2BIT) //=> 4 level
	#define ALARM_LEVEL 0x3
#elif defined(BAT_ON_3BIT) //On 3bit=> 8 level
	#define ALARM_LEVEL 0x7
#else
	#define ALARM_LEVEL 0x1
#endif

OSAL_void sys_led_management(OSAL_u8 led_Mask, OSAL_bool_t newState)
{
	GPIO_TypeDef* GPIOx=NULL;
	uint16_t GPIO_Pin=0;
	if(SYS_LED_STATUS==led_Mask)
	{
		if (module_config[MOD_CFG_POS_SYS_BATLEVEL]==ALARM_LEVEL)
		{
			sys_led_management(SYS_LED_GREEN,OSAL_false);
			sys_led_management(SYS_LED_RED,newState);
		}
		else
		{
			sys_led_management(SYS_LED_GREEN,newState);
			sys_led_management(SYS_LED_RED,OSAL_false);
		}
	}
	else
	{
		OSAL_bool_t oldState = ((module_config[MOD_CFG_POS_SYS_LED]&led_Mask)!=0)?OSAL_true:OSAL_false;
		if(oldState!=newState)
		{
			if(newState) module_config[MOD_CFG_POS_SYS_LED]|=led_Mask;
			else module_config[MOD_CFG_POS_SYS_LED]&=(~led_Mask);
			switch(led_Mask)
			{
				#ifdef HAS_RED_LED
				case SYS_LED_RED:GPIOx=GPIO_RED_LED;GPIO_Pin=GPIO_PIN_RED_LED;break;
				#endif //HAS_RED_LED
				#ifdef HAS_GREEN_LED
				case SYS_LED_GREEN:GPIOx=GPIO_GREEN_LED;GPIO_Pin=GPIO_PIN_GREEN_LED;break;
				#endif //HAS_GREEN_LED
				default:break;
			}
		}
		if(GPIOx!=NULL)
		{
			if(newState) GPIO_ResetBits(GPIOx, GPIO_Pin);
			else GPIO_SetBits(GPIOx, GPIO_Pin);
		}
	}
}

OSAL_void sys_is_module_moving(OSAL_bool_t* movingstate)
{
	if(movingstate)
		*movingstate=module_config[MOD_CFG_POS_SYS_MODULE_MOVING];
}


OSAL_void sys_update_battery_and_temperature_info()
{
	OSAL_timeval cur,res;
	OSAL_gettimeofday(&cur,NULL);
	OSAL_timersub_realtime(&cur,&lastbattery_and_temperature_update,&res);
	if((res.tv_sec>=MOD_CFG_BATTEMP_UPDATE_IN_SEC)||(res.tv_sec<0)||res.tv_usec<0)
	{
		HAL_ADC_Read_Values();
		//we can sleep only if:
		//the time before sleep is authorizedf has elapsed
		// Hysteresis
		#if defined(BAT_ON_2BIT) //=> 4 level
		alarm_level = 0x3;
		if (Vbat_Module < VBAT_LOW_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x3;
		else if (Vbat_Module < VBAT_LEVEL3_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x2;
		else if (Vbat_Module < VBAT_HIGH_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x1;
		else// if (Vbat_Module > VBAT_HIGH_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x0;
		#elif defined(BAT_ON_3BIT) //On 3bit=> 8 level
		if (Vbat_Module < VBAT_LOW_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x7;
		else if (Vbat_Module < VBAT_LEVEL1_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x6;
		else if (Vbat_Module < VBAT_LEVEL2_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x5;
		else if (Vbat_Module < VBAT_LEVEL3_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x4;
		else if (Vbat_Module < VBAT_LEVEL4_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x3;
		else if (Vbat_Module < VBAT_LEVEL5_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x2;
		else if (Vbat_Module < VBAT_HIGH_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x1;
		else module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x0;		
		#else // 1 bit:lowbat or not
		// Hysteresis
		if (Vbat_Module < VBAT_LOW_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x1;
		else if (Vbat_Module > VBAT_HIGH_THRES)
			module_config[MOD_CFG_POS_SYS_BATLEVEL] = 0x0;
		#endif
		OSAL_gettimeofday(&lastbattery_and_temperature_update,NULL);
	}
}

OSAL_void sys_get_battery_data(OSAL_u8** data,OSAL_u16* dataSizeinBits)
{
	//Default implementation: sensor 0 = battery level
	if(data) *data=&module_config[MOD_CFG_POS_SYS_BATLEVEL];
	if(dataSizeinBits)
	#if defined(BAT_ON_2BIT) //=> 4 level
		 *dataSizeinBits=2;
	#elif defined(BAT_ON_3BIT) //=> 8 level
		*dataSizeinBits=3;
	#else //On 1 bit, low bat or not
		*dataSizeinBits=1;
	#endif
}

OSAL_void sys_get_battery_level(OSAL_u8* level, OSAL_u8* levelLOW, OSAL_u8* levelHIGH,OSAL_u32* levelmV)
{
	if(level) *level=module_config[MOD_CFG_POS_SYS_BATLEVEL];

	#if defined(BAT_ON_2BIT) //=> 4 level
		if(levelLOW) *levelLOW=0x4;
	#elif defined(BAT_ON_3BIT) //=> 8 level
		if(levelLOW) *levelLOW=0x7;
	#else //On 1 bit, low bat or not
		if(levelLOW) *levelLOW=0x1;
	#endif
	if(levelHIGH) *levelHIGH=0x0;
	if(levelmV) *levelmV=Vbat_Module/100;
}

OSAL_void sys_get_temperature_level(OSAL_s32* temp,OSAL_s32* tempRounded)
{
	if(temp) *temp=Intern_Temperature;
	if(tempRounded)
	{
		*tempRounded=(Intern_Temperature/1000+((Intern_Temperature%1000)+500)/1000);
	}
}

/** TEST PART */
#pragma GCC optimize("Os")
static void ClearIRQReg()
{
    /*Clear IRQ register*/
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_SYSTEM, 		BAR_INTR_VECT0_CLR_W0,  	0xFFFF);
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_SYSTEM, 		BAR_INTR_VECT2_CLR_W0,  	0xFFFF);
	OSAL_udelay(100);
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_SYSTEM, 		BAR_INTR_VECT0_CLR_W0,  	0x0000);
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_SYSTEM, 		BAR_INTR_VECT2_CLR_W0,  	0x0000);
}

OSAL_s32 sys_noise_or_treshold_test(OSAL_u8 channel, OSAL_u8 zone, OSAL_u8 path,OSAL_s32 *min, OSAL_s32* max,OSAL_u8 thTest, OSAL_u32* processTimeUS)
{
	OSAL_timeval tv1, tv2, diff;
	OSAL_u32 lpmax=1;
	OSAL_u8 i;
	OSAL_u8 mux_w10, mux_w33, mux_w43;
	OSAL_u8  timeout = 100; // 1 sec timeout
	OSAL_u16  ctrl_irq0_vec_read;
	OSAL_u32 max_pwr, noise=0, noisemin=0xFFFFFFFF, noisemax=0;
	OSAL_u8 conf_interrupt_mask[]	= {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	config_phy_t  *curr_config_phy;
	config_phy_t new_config_phy;
	if(processTimeUS) OSAL_gettimeofday(&tv1, NULL);
	//disable LTA
	curr_config_phy = get_current_config_phy(ctx.cfg_ctx);
	if(!curr_config_phy) return 0;
	OSAL_memcpy(&new_config_phy,curr_config_phy,sizeof(config_phy_t));

	//initialize new config phy
	new_config_phy.power_mode =  UWB_SC_PM_RX_IDLE;
	new_config_phy.channel =  channel;
	//necessary to force threshold an deactivate lta
	new_config_phy.manual_threshold = 1;
	pp_conf_get_sync_code_repetition_from_zone(zone,&new_config_phy.sync_code, &new_config_phy.sync_rep);
	pp_conf_get_demod_code_repetition_from_zone(zone,&new_config_phy.demod_code, &new_config_phy.demo_rep);
	new_config_phy.sync_threshold = 1;
	new_config_phy.rng_threshold = 1;
	//apply this config phy
	radio_set_phy_cfg((OSAL_u8*) &new_config_phy,sizeof(config_phy_t));
	pp_hw_reg_read(0, 	BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W10, &mux_w10);
	pp_hw_reg_write(0, 	BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W10, (mux_w10&0xF0)); // give host access to BAR_RXFE_SP_TXRXDEP
	pp_hw_reg_read(0, 	BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W33, &mux_w33);
	pp_hw_reg_write(0, 	BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W33, (mux_w33&0xEF)); // give host access to CTRL_CFG
	pp_hw_reg_read(0, 	BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W43, &mux_w43);
	pp_hw_reg_write(0, 	BNK_PP_REG | BAR_CFG_MUX_REG, BAR_MUX_REG_W43, (mux_w43&0xFB)); // give host access to BAR_DIO_TA_VAL
	if(path == 1)
	{
		pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_ANALOG, 	BAR_RXFE_SP_TXRXDEP,	0x02);
		pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_SYSTEM, 	BAR_DIO_TA_VAL,	0x82);
		if(thTest==0) 		pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_ANALOG, 	BAR_RXFE_TUNE_IPOL_GMCBUFFER, 0x66);//default
	}
	else if(path == 2)
	{
		pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_ANALOG, 	BAR_RXFE_SP_TXRXDEP,	0x04);
		pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_SYSTEM, 	BAR_DIO_TA_VAL,	0x88);
		if(thTest==0)		pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_ANALOG, 	BAR_RXFE_TUNE_IPOL_GMCBUFFER, 0x10);
	}
	if(thTest) lpmax=10;
	for(i=1; i<=lpmax+1; i++)
	{
		// start from clean state
		pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_DIGITAL, 	BAR_PHY_CTRL_CMD,	0x00);
		pp_hw_reg_write(0,	BNK_PP_REG | BAR_FIFO_TXRX, 	BAR_FIFO_RX_CLEAR,	0x01);
		ClearIRQReg();
		pp_hw_burst_write(0,	BNK_PP_REG | BAR_CFG_SYSTEM, 	BAR_INTR_VECT0_MSK_W0,	conf_interrupt_mask,		OSAL_ARRAY_SIZE(conf_interrupt_mask));
		pp_hw_reg32_write(0,	BNK_PP_REG | BAR_CFG_DIGITAL, 	BAR_SFD_TIME_OUT_W0,	(5000000/16)); // 5ms (Lsb 16ns)        
		if(i>lpmax) break;
		if(thTest)
		{
			pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_DIGITAL, 	BAR_CTRL_CFG,		0x03);
			pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_DIGITAL, 	BAR_PHY_CTRL_CMD,	0x02);
			timeout = 100;
			do {
				msleep(10);
				pp_hw_reg16_read(0, BNK_PP_REG | BAR_CTRL_SYSTEM, 	BAR_INTR_VECT0_W0,	(OSAL_u16*)&ctrl_irq0_vec_read);
				timeout--;
			} while (	((ctrl_irq0_vec_read & 0x0020) != 0x0020) && //SFD_TIME_OUT
						((ctrl_irq0_vec_read & 0x0010) != 0x0010) && //RX_DONE
						(timeout != 0)
					);
			pp_hw_reg32_read(0, BNK_PP_REG | BAR_CTRL_DIGITAL, BAR_COMP_NRJ_INT1_W0,		&max_pwr);
			//for a single measure, noise = max_pwr
			noise += max_pwr;
			noisemin=(noisemin>max_pwr)?max_pwr:noisemin;
			noisemax=(noisemax<max_pwr)?max_pwr:noisemax;
			if(timeout == 0)
			{
				noise = 0;
				OSAL_trace(TRACE_ERROR,"TOUT!");
				break;
			}
		}
		else
		{
			pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_DIGITAL, 	BAR_CTRL_CFG,		0x03);
			pp_hw_reg_write(0,	BNK_PP_REG | BAR_CFG_DIGITAL, 	BAR_PHY_CTRL_CMD,	0x02);
			timeout = 100;
			do {
				msleep(10);
				pp_hw_reg16_read(0, BNK_PP_REG | BAR_CTRL_SYSTEM, 	BAR_INTR_VECT0_W0,	(OSAL_u16*)&ctrl_irq0_vec_read);
				timeout--;
			} while (	((ctrl_irq0_vec_read & 0x0020) != 0x0020) && //SFD_TIME_OUT
						((ctrl_irq0_vec_read & 0x0010) != 0x0010) && //RX_DONE
						(timeout != 0)
					);
			pp_hw_reg32_read(0, BNK_PP_REG | BAR_CTRL_DIGITAL, BAR_COMP_NRJ_INT1_W0,		&max_pwr);
			//for a single measure, noise = max_pwr
			noise = max_pwr;
			if(timeout == 0)
			{
				noise = 0;
				OSAL_trace(TRACE_ERROR,"TOUT!");
				break;
			}
		}
	}
	//for a single measure, noise /= 1
	noise /= lpmax;
	if(processTimeUS)
	{
		OSAL_gettimeofday(&tv2, NULL);
		OSAL_timersub_realtime(&tv2,&tv1,&diff);
		*processTimeUS = diff.tv_sec*1000000 +  diff.tv_usec;
	}
	_sys_reinit(OSAL_true);
	if(min) *min=(OSAL_s32) noisemin;
	if(max) *max=(OSAL_s32) noisemax;
	return (OSAL_s32) noise;
}

OSAL_void sys_test_mcuid(st_uwb_test_mcuid* result)
{
	#ifdef PROD
		prod_test_mcuid(result);
	#endif
}

OSAL_void sys_test_soldering(st_uwb_test_soldering *result)
{
	OSAL_u8 fifo_tx_data[] = {0x05, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04};
	OSAL_u8 fifo_rx_data[7] = {0};
	config_phy_t refphy;
	OSAL_u8 PhyStatus = 0x00;
	OSAL_u8 SeqStatus = 0x00;
	OSAL_u8 tmp=0,i=0,j=0;
	//prepare FIFO TX and RX
	OSAL_u8 testComplete=0;

	OSAL_memset(result,0,sizeof(st_uwb_test_soldering));
	result->status=TEST_KO;
	pp_hw_reg16_write(0, BAR_CFG_SYSTEM, BAR_INTR_VECT0_MSK_W0,  	0x0000); //enable interrupt V0

	//set phy cfg for this test
	OSAL_memset(&refphy,0,sizeof(config_phy_t));
	refphy.sync_code_ID=0;
	refphy.demod_code_ID=0;
	refphy.channel=2;
	refphy.sync_code=127;
	refphy.demod_code=127;
	refphy.sync_rep=8;
	refphy.demo_rep=8;
	refphy.clock_track=1;
	refphy.framer_config=0;
	refphy.rf_path=DEFAULT_RFPATH;
	refphy.sync_threshold=650000;
	refphy.rng_threshold=650000;
	refphy.tx_gain=3;
	refphy.rx_gain=3;
	refphy.power_mode=PM_2;
	refphy.act=0;
	refphy.preamble_timeout=312500;
	radio_set_phy_cfg((OSAL_u8*) &refphy,sizeof(config_phy_t));
	pp_hw_reg_read(	0, BAR_CTRL_SYSTEM, BAR_MAJ_REV,  &tmp); //check Chipset version
	if(tmp != 0x03) //ppv3
	{
		OSAL_trace(TRACE_ANY,"SPI NOK");
		result->Tspi=TEST_KO;
	}
	else
	{
		result->Tspi=TEST_OK;
	}
	testComplete+=result->Tspi;
	//SEQ TEST
	result->TSeq=TEST_KO;
	radio_set_pmstate(UWB_SC_PM_TXRX_IDLE);
	/*Clear IRQ register*/
	ClearIRQReg();
	/* configure mask*/
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_SYSTEM, 		BAR_INTR_VECT0_MSK_W0,		0xFFEE); //TX done / RX done
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_SYSTEM, 		BAR_INTR_VECT1_MSK_W0,		0xFFFF);
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_SYSTEM, 		BAR_INTR_VECT2_MSK_W0,		0xF5FF); //ts-delay/Action done
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_SYSTEM, 		BAR_INTR_VECT3_MSK_W0,		0xFFFF);
	pp_hw_reg_write(0,BNK_PP_REG |BAR_CFG_32K,  		BAR_32K_TIMER_CONFIG,		0x00); // stop sequencer
	pp_hw_reg_write(0,BNK_PP_REG | BAR_CFG_DIGITAL,		BAR_CTRL_CFG,				0x00); // disable preamble & SFD timeout
	//Timer sequencer init
	pp_hw_reg16_write(0,BNK_PP_REG| BAR_CFG_32K, 		BAR_32K_TIMER_PERIOD_W0,	8192);
	pp_hw_reg_write(0,BNK_PP_REG|BAR_CFG_32K, 			BAR_32K_FRAMERATE_LENGTH,	3);
	pp_hw_reg_write(0,BNK_PP_REG|BAR_CFG_32K, 			BAR_32K_M,					0x01);
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_32K, 		BAR_32K_TS_W0,				32);
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_32K, 		BAR_32K_DELAY_W0,			1);
	pp_hw_reg32_write(0,BNK_PP_REG|BAR_CFG_32K, 		BAR_32K_NTOA_W0,			16000);
	pp_hw_reg_write(0,BNK_PP_REG|BAR_CFG_32K,			BAR_32K_ACTION,				3);
	pp_hw_reg16_write(0,BNK_PP_REG|BAR_CFG_32K,			BAR_32K_SHADOW_W0,			0);
	//prepare FIFO TX and RX
	pp_hw_reg_write(0,BNK_PP_REG|BAR_FIFO_TXRX, 		BAR_FIFO_TX_CLEAR,			0x01);
	pp_hw_reg_write(0,BNK_PP_REG|BAR_FIFO_TXRX,   		BAR_FIFO_RX_CLEAR,			0x01);
	pp_hw_burst_write(0,BNK_PP_REG|BAR_FIFO_TXRX, 		BAR_FIFO_TX_DATA,		(OSAL_u8 *)fifo_tx_data, 	OSAL_ARRAY_SIZE(fifo_tx_data));
	pp_hw_reg_write(0,BNK_PP_REG|BAR_CFG_32K,		BAR_32K_TIMER_CONFIG,  		0x03); // start sequencer and 32Khzfrom62.4Mhz
	msleep(1000);//to be sure sequencer is started
	tmp=20;//timout=20*100ms=2sec
	while(((PhyStatus&0x10)==0x00)&&(tmp>0))
	{
		tmp--;
		pp_hw_reg_read(0,BNK_PP_REG|BAR_CTRL_SYSTEM, BAR_INTR_VECT0_W0,  	(OSAL_u8 *)&PhyStatus);
		pp_hw_reg_read(0,BNK_PP_REG|BAR_CTRL_SYSTEM, BAR_INTR_VECT2_W1,  	(OSAL_u8 *)&SeqStatus);
		msleep(100);
	}
	if(tmp < 1) goto seqTestStopped;
	
	if(SeqStatus == 0x00) //no TS-DELAY and no start action !
	{
		result->T32K=TEST_KO;
		OSAL_trace(TRACE_ANY,"32K KO");
		goto seqTestStopped;
	}
	result->T32K=TEST_OK;
	if(SeqStatus == 0X08) //no start action !
	{
		result->T26M=TEST_KO;
		OSAL_trace(TRACE_ANY,"26M KO");
		goto seqTestStopped;
	}
	result->T26M=TEST_OK;
	if((PhyStatus & 0x01) == 0x00)
	{
		result->TTX=TEST_KO;
		OSAL_trace(TRACE_ANY,"TX KO");
		goto seqTestStopped;
	}
	result->TTX=TEST_OK;
	if((PhyStatus & 0x10) == 0x00)
	{
		result->TRX=TEST_KO;
		OSAL_trace(TRACE_ANY,"RX KO");
		goto seqTestStopped;
	}
	result->TRX=TEST_OK;
	pp_hw_burst_read(0,BNK_PP_REG|BAR_FIFO_TXRX, BAR_FIFO_RX_DATA, (OSAL_u8 *)fifo_rx_data, OSAL_ARRAY_SIZE(fifo_rx_data));
	if(fifo_rx_data[0] != 0x05)
	{
		result->TRXPHR=TEST_KO;
		OSAL_trace(TRACE_ANY,"PHR KO");
		goto seqTestStopped;
	}
	result->TRXPHR=TEST_OK;
	for(i=2;i<7;i++)
	{
		if(fifo_rx_data[i] != (i-2))
		{
			for(j=0;j<7;j++)
			{
				if(((fifo_rx_data[i]>>j)&0x01) != (((i-2)>>j)&0x01))
				{
					result->TRXPSDU=TEST_KO;
					OSAL_trace(TRACE_ANY,"PSDU KO");
					goto seqTestStopped;
				}
			}
		}
	}
	result->TRXPSDU=TEST_OK;
	result->TSeq=TEST_OK;
	testComplete+=result->TSeq;
seqTestStopped:
	if(result->TSeq!=TEST_OK)
		OSAL_trace(TRACE_ANY,"SEQ KO");
	/*Clear IRQ register*/
	ClearIRQReg();
	pp_hw_reg_write(0,BNK_PP_REG|BAR_CFG_32K,		BAR_32K_TIMER_CONFIG,	0x00); // stop sequencer
	pp_hw_reg_write(0,BNK_PP_REG|BAR_FIFO_TXRX, 	BAR_FIFO_TX_CLEAR,		0x01);
	pp_hw_reg_write(0,BNK_PP_REG|BAR_FIFO_TXRX, 	BAR_FIFO_RX_CLEAR,		0x01);
	radio_set_pmstate(UWB_SC_PM_2);
	tmp=0;
	sys_get_wake_up_reason(&tmp);
	if (tmp&SYS_WAKE_UP_PP_MASK)
	{
		sys_reset_wake_up_reason(SYS_WAKE_UP_PP_MASK);
		result->TIT=TEST_OK;
		testComplete+=result->TIT;
	}
	else
	{
		result->TIT=TEST_KO;
		OSAL_trace(TRACE_ANY,"IT KO");
	}
	if(testComplete==(3*TEST_OK))
	{
		result->status=TEST_OK;
		OSAL_trace(TRACE_ANY,"SOLDERING OK");
	}
	else
		OSAL_trace(TRACE_ANY,"SOLDERING NOK");
	sys_reinit();
}

OSAL_void sys_test_io(uwb_test_status_e* result)
{
	#ifdef PROD
		prod_test_io(result);
	#endif
}

OSAL_void sys_test_voltage(st_uwb_test_voltage* result)
{
	#ifdef PROD
		prod_test_voltage(result);
	#endif
}

OSAL_void sys_test_txtune(st_uwb_test_txtune* result)
{
	#ifdef PROD
		prod_test_txtune(result);
	#endif
}

OSAL_void sys_test_lna(st_uwb_test_lna *result)
{
	#ifdef PROD
		prod_test_lna(result);
	#endif
}

OSAL_void sys_test_rxtune(st_uwb_test_rxtune* result)
{
	#ifdef PROD
		prod_test_rxtune(result);
	#endif
}

OSAL_void sys_test_txidle(uwb_test_status_e* result)
{
	#ifdef PROD
		prod_test_txidle(result);
	#endif
}

OSAL_void sys_test_rxidle(uwb_test_status_e* result)
{
	#ifdef PROD
		prod_test_rxidle(result);
	#endif
}

OSAL_void sys_test_pm2(uwb_test_status_e* result)
{
	#ifdef PROD
		prod_test_pm2(result);
	#endif
}

OSAL_void sys_test_txcont(uwb_test_status_e* result)
{
	#ifdef PROD
		prod_test_txcont(result);
	#endif
}

OSAL_void sys_test_sensitivity(uwb_test_status_e* result)
{
	#ifdef PROD
		prod_test_sensitivity(result);
	#endif
}

OSAL_void sys_test_ranging(uwb_test_status_e* result)
{
	#ifdef PROD
		prod_test_ranging(result);
	#endif
}

OSAL_void sys_test_rfdesign(st_uwb_test_rfdesign* result,int measnb)
{
	#define MEAS_NUMBER_DFLT	100
	#define MEAS_ZONE		5 //C127r64
	#define MEAS_RFPATH		2
	OSAL_u32 MEAS_NUMBER = (measnb>0)?measnb:MEAS_NUMBER_DFLT;
	OSAL_float average,max_noise,min_noise;
	OSAL_u32 cur_noise;
	OSAL_u32 prod_noise;
	OSAL_u16 i;
	OSAL_u8 channel;
	OSAL_u8 chindex;
	OSAL_u8 rsltCount=0;//incremented on eahc channel test success
	if(MEAS_NUMBER>MEAS_NUMBER_DFLT) MEAS_NUMBER=MEAS_NUMBER_DFLT;
	OSAL_memset(result,0,sizeof(st_uwb_test_rfdesign));

	//for(channel=1;channel<12;channel++)//decomment me to test all bg 
	for(channel=1;channel<5;channel++)//comment me to test all bg 
	{
		if(channel<5) chindex=channel-1;
		else if (channel<8) continue;
		else chindex=channel-4;
		min_noise = 0xFFFFFFFF;
		max_noise = 0;
		average=0;
		/* get prod noise */
		pp_conf_get_prod_threshold_from_zone(MEAS_ZONE, channel, MEAS_RFPATH, &prod_noise);
		for(i=0;i<MEAS_NUMBER;i++)
		{
			OSAL_trace(TRACE_ANY,"CH %d(%d/%d)",channel,i+1,MEAS_NUMBER);
			cur_noise = sys_noise_or_treshold_test(channel,MEAS_ZONE,MEAS_RFPATH,NULL,NULL,0,NULL);
			if(cur_noise>max_noise) max_noise = (OSAL_float) cur_noise;
			if(cur_noise<min_noise) min_noise = (OSAL_float) cur_noise;
			average+=((OSAL_float) cur_noise);
		}
		average /=MEAS_NUMBER;
		result->average_noise[chindex] = (OSAL_u32) ((10*log10f(average))*1000);
		result->max_noise[chindex] = (OSAL_u32) ((10*log10f(max_noise))*1000);
		result->min_noise[chindex] = (OSAL_u32) ((10*log10f(min_noise))*1000);
		/* Ratio check */
		result->noise_ratio[chindex] = result->max_noise[chindex] - result->min_noise[chindex];
		result->prod_noise[chindex]=0;
		if(prod_noise>0)
			result->prod_noise[chindex] = (OSAL_u32) ((10*log10f((OSAL_float)prod_noise))*1000);
		result->noise_ratio_ref[chindex] = result->average_noise[chindex] - result->prod_noise[chindex] ;
		if(result->noise_ratio[chindex]>MAX_NOISE_RATIO)
			result->chstatus[chindex]=TEST_KO;
		else if(result->noise_ratio[chindex]<0)//if min>max c chopolokanmeme
			result->chstatus[chindex]=TEST_KO;
		else
		{
			result->chstatus[chindex]=TEST_OK;
			rsltCount++;
		}
	}
	//if(rsltCount == 8)//if all bg are tested
	if(rsltCount == 4 ) result->status=TEST_OK;
	else  result->status=TEST_KO;
	OSAL_trace(TRACE_ANY,"RFDESIGN %s",(result->status==TEST_OK)?"OK":"KO");
	sys_reinit();
}

OSAL_void sys_test_production(st_uwb_test_results** result)
{
	#ifdef PROD
		prod_test_all(result);
	#endif
}


