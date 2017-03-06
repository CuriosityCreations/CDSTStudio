/**
 * @file ShellCommand_system.c
 * @brief ShellCommand_system.c
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
#include "stm32f0xx_rcc.h"


#include "shell.h"
#include "ShellCommand_system.h"
#include "ShellCommand_radio.h"
#include "ShellCommand_misc.h"
#ifdef PROD
#include "ShellCommand_prod.h"
#endif

#define TEST_ROM__
#ifdef TEST_ROM
const OSAL_u8 takeLotOfRom[20000]={0};
#endif

const char* testResultToString(int rslt)
{
	return ((rslt==TEST_NOTDONE)?"?":((rslt==TEST_OK)?"OK":"KO"));
}

void test_reinit()
{
	shellcommand_misc_reinit();
#ifdef PROD
	shellcommand_prod_reinit();
#endif
	sys_reinit();
}

void shellcommand_system_Process()
{
}

signed char help_fct(int argc, char** argv)
{
    if(argc>=2)
        shell_help(OSAL_true);
    else
        shell_help(OSAL_false);
    return 0;
}

signed char reset_fct(int argc, char** argv)
{
    test_reinit();
    send_host(argv[0],0, 1, (signed long)0);
    return 0;
}

signed char mcu_id_fct(int argc, char** argv)
{
	int i;
	unsigned char unique_id[12];
	sys_get_mcuid(unique_id);
	print("ID<");
	for(i=0;i<12;i++) print("%x ",unique_id[i]);
	printline(">");
	send_host(argv[0], 0,12,(signed long)unique_id[0],
				(signed long)unique_id[1],
				(signed long)unique_id[2],
				(signed long)unique_id[3],
				(signed long)unique_id[4],
				(signed long)unique_id[5],
				(signed long)unique_id[6],
				(signed long)unique_id[7],
				(signed long)unique_id[8],
				(signed long)unique_id[9],
				(signed long)unique_id[10],
				(signed long)unique_id[11]);
	return 0;
}

signed char version_fct(int argc, char** argv)
{
	printline("SW V%d.%d",SW_VERSION_MAJ,SW_VERSION_MIN);	
	printline("HW V%d.%d",HW_VERSION_MAJ,HW_VERSION_MIN);	
	if(prodSection.manufacturerSection.chipVersion!=0)//not defined otherwise
		printline("Chip Version:%x",prodSection.manufacturerSection.chipVersion);
	if(prodSection.manufacturerSection.emultagVersion!=0)//not defined otherwise
		printline("UM100 Version:%x",prodSection.manufacturerSection.emultagVersion);
	printline("Build %s,%s",__TIME__,__DATE__);
	send_host(argv[0], 0, 15, 
		(signed long) SW_VERSION_MAJ,
		(signed long) SW_VERSION_MIN,
		(signed long) __DATE__[0],
		(signed long) __DATE__[1],
		(signed long) __DATE__[2],			
		(signed long) __DATE__[4],
		(signed long) __DATE__[5],			
		(signed long) __DATE__[7],
		(signed long) __DATE__[8],
		(signed long) __DATE__[9],
		(signed long) __DATE__[10],			
		(signed long) __TIME__[0],
		(signed long) __TIME__[1],
		(signed long) __TIME__[3],
		(signed long) __TIME__[4]
		);
	return 0;
}

signed char serial_fct(int argc, char** argv)
{
	int i;
	int slen=SERIAL_NB_DIGIT_MAX;
	print("S/N<");
	for (i=0;i<slen;i++)
		print("%x",prodSection.manufacturerSection.serial[i]);
	printline(">");
	#ifdef PROD
	do{
		unsigned char* s=&prodSection.manufacturerSection.serial[0];
		send_host(argv[0],0,slen,s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[8],s[9],s[10],s[11],s[12],s[13],s[14],s[15]);break;
	}while(0);
	#endif //#ifdef PROD	
	return 0;
}

signed char macAddress_fct(int argc, char** argv)
{
  	OSAL_u8 i;	
  	print("Mac<%X",((short)prodSection.manufacturerSection.longAddress[0])<<8|prodSection.manufacturerSection.longAddress[1]);
	for(i = 2; i < 16; i += 2)
		print(":%X",((short)prodSection.manufacturerSection.longAddress[i])<<8|prodSection.manufacturerSection.longAddress[i+1]);
	printline(">");
	send_host(argv[0], 0, 16, 	(signed long)(prodSection.manufacturerSection.longAddress[0]),
					(signed long)(prodSection.manufacturerSection.longAddress[1]),
					(signed long)(prodSection.manufacturerSection.longAddress[2]),
					(signed long)(prodSection.manufacturerSection.longAddress[3]),
					(signed long)(prodSection.manufacturerSection.longAddress[4]),
					(signed long)(prodSection.manufacturerSection.longAddress[5]),
					(signed long)(prodSection.manufacturerSection.longAddress[6]),
					(signed long)(prodSection.manufacturerSection.longAddress[7]),
					(signed long)(prodSection.manufacturerSection.longAddress[8]),
					(signed long)(prodSection.manufacturerSection.longAddress[9]),
					(signed long)(prodSection.manufacturerSection.longAddress[10]),
					(signed long)(prodSection.manufacturerSection.longAddress[11]),
					(signed long)(prodSection.manufacturerSection.longAddress[12]),
					(signed long)(prodSection.manufacturerSection.longAddress[13]),
					(signed long)(prodSection.manufacturerSection.longAddress[14]),
					(signed long)(prodSection.manufacturerSection.longAddress[15]));
	return 0;
}

signed char dac_fct(int argc, char** argv)
{
  	signed char ret=0;
  	unsigned long dac_val;
	if(argc<2) return -1;
	dac_val = (unsigned long)atoi(argv[1]);
	OSAL_dac_set_value(dac_val);
	printline("New:%d uV",(int) dac_val);
	return ret;
}

signed char dac_en_fct(int argc, char** argv)
{
  	signed char ret=0;
	unsigned char dac_en;
	if(argc<2) return -1;
	dac_en = (unsigned char)atoi(argv[1]);
	dac_enable(dac_en);
	printline("%s",dac_en ? "ON" : "OFF");
	return ret;
}

signed char soldering_fct(int argc, char** argv)
{
	st_uwb_test_soldering result;
	sys_test_soldering(&result);
	#ifdef PROD
	send_host(argv[0], 0, 10,
			(signed long)result.status,
			(signed long)result.Tspi,
			(signed long)result.T32K,
			(signed long)result.T26M,
			(signed long)result.TTX,
			(signed long)result.TRX,
			(signed long)result.TRXPHR,
			(signed long)result.TRXPSDU,
			(signed long)result.TSeq,
			(signed long)result.TIT
	);
	#else
	printline("SPI %s",testResultToString(result.Tspi));
	printline("32K %s",testResultToString(result.T32K));
	printline("26M %s",testResultToString(result.T26M));
	printline("TX %s",testResultToString(result.TTX));
	printline("RX %s",testResultToString(result.TRX));
	printline("PHR %s",testResultToString(result.TRXPHR));
	printline("PSDU %s",testResultToString(result.TRXPSDU));
	printline("SEQ %s",testResultToString(result.TSeq));
	printline("IT %s",testResultToString(result.TIT));
	printline("SOLDERING %s",testResultToString(result.status));
	#endif
	test_reinit();
	return 0;
}

signed char io_conf_fct(int argc, char** argv)
{
	HAL_GPIO_Show();
	send_host(argv[0], 0, 1, 0);
	return 0;
}

signed char temperature_fct(int argc, char** argv)
{
	OSAL_s32 tmp=0,round;
	sys_get_temperature_level(&tmp,&round);
	printline("%d.%.3d Celsius (%d C)",tmp/1000,tmp%1000,round);
	return 0;
}

signed char battery_fct(int argc, char** argv)
{
	OSAL_u8 lvl,low,high;
	OSAL_u32 mv;
	sys_get_battery_level(&lvl,&low,&high,&mv);
	printline("%dmv (LVL:%d %s)",mv,lvl,(lvl==low)?"LOW":((lvl==high)?"OK":""));
	return 0;
}

signed char spiTrace_fct(int argc, char** argv) 
{
	if (argc < 2) return -1;
	EXTCOM_BUF_PCKTTRACE = atoi(argv[1]);
	return 0;
}


signed char rfdesign_fct(int argc, char** argv)
{
	st_uwb_test_rfdesign result;
	OSAL_u8 i, channel;
	OSAL_u32 measnb=0;
	if(argc>1) measnb=atoi(argv[1]);
	sys_test_rfdesign(&result,measnb);
	for(i=0;i<8;i++)
	{
		int mi=(result.min_noise[i]/1000);
		int mf=result.min_noise[i]-1000*mi;
		int Mi=(result.max_noise[i]/1000);
		int Mf=result.max_noise[i]-1000*Mi;
		int mMi=(result.noise_ratio[i]/1000);
		int mMf=result.noise_ratio[i]-1000*mMi;
		int ai=(result.average_noise[i]/1000);
		int af=result.average_noise[i]-1000*ai;
		int pi=(result.prod_noise[i]/1000);
		int pf=result.prod_noise[i]-1000*pi;
		int api=(result.noise_ratio_ref[i]/1000);
		int apf=result.noise_ratio_ref[i]-1000*api;
		if(i<4) channel = i+1;
		else channel = i+4;
		if(result.chstatus[i]==TEST_NOTDONE)
			printline(	"CH%d:NOT DONE",channel);
		else
		{
			printline(	"CH%d %s:",
						channel,
						(result.chstatus[i]==TEST_KO)?"ERR!!!":"OK");
			printline(	"  ratio:%d.%.3ddb",mMi,mMf);
			printline(	"  min:%d.%.3ddb\tmax:%d.%.3ddb\taverage:%d.%.3ddb",
						mi,mf,Mi,Mf,ai,af);
			printline(	"  prod:%d.%.3ddb\t VsRef:%d.%.3ddb)",
						pi,pf,api,apf );
		}
	}
	test_reinit();
	return 0;
}

signed char lse_en_fct(int argc, char** argv)
{
	unsigned char lse_en;
	if(argc<2) return -1;
	lse_en = (unsigned char)atoi(argv[1]);
	
	if(lse_en)
	{
		RCC_LSEConfig(RCC_LSE_ON);
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	}
	else
	{
		RCC_LSEConfig(RCC_LSE_OFF);
	}

	printline("%s", lse_en ? "ON" : "OFF");

	return 0;
}

signed char lse_drv_cfg_fct(int argc, char** argv)
{
	unsigned char lse_drv_cfg;
	if(argc<2) return -1;
	lse_drv_cfg = (unsigned char)atoi(argv[1]);

	switch(lse_drv_cfg)
	{
		case 0:
			RCC_LSEDriveConfig(RCC_LSEDrive_Low);
			printline("Low");
			break;
		case 1:
			RCC_LSEDriveConfig(RCC_LSEDrive_MediumLow);
			printline("Mid low");
			break;
		case 2:
			printline("Mid high");
			RCC_LSEDriveConfig(RCC_LSEDrive_MediumHigh);
			break;
		case 3:
			printline("High");
			RCC_LSEDriveConfig(RCC_LSEDrive_High);
			break;
		default:
			return -2;
	}

	return 0;
}

#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL

signed char rtcpres_fct(int argc, char** argv)
{
	OSAL_u8 precision=0;
	if(argc>1)
	{
		precision=(OSAL_u8) atoi(argv[1]);
		sys_rtc_set_precisionFactor(precision);
	}
	sys_rtc_get_precisionFactor(&precision);
	printline("%d",precision);
	return 0;
}

signed char rtcTime_fct(int argc, char** argv)
{
	OSAL_timeval tim;
	OSAL_gettimeofday(&tim,NULL);
	printline("%ds %du",tim.tv_sec,tim.tv_usec);
	return 0;
}

signed char rtc_fct(int argc, char** argv)
{
	OSAL_u32 sec=0,usec=0;
	OSAL_timeval itim,otim,diff;

	if(argc<2) return -1;
	if(argc>1) sec=atoi(argv[1]);
	if(argc>2) usec=atoi(argv[2]);
	printline("RTC:%ds %dms",sec,usec);
	OSAL_gettimeofday(&itim,NULL);
	sys_rtc_wake_up_after(sec,usec);
	OSAL_gettimeofday(&otim,NULL);
	OSAL_timersub_realtime(&otim,&itim,&diff);
	printline("Elapsed (%ds %du)",diff.tv_sec,diff.tv_usec);
	return 0;
}

signed char rtctune_fct(int argc, char** argv)
{
	OSAL_u64 tune=0;
	OSAL_u64 freq=0;
	OSAL_u64 INT=0;
	OSAL_u64 FLOAT=0;
	if(argc>=2)
	{
		INT=(OSAL_u64 ) atoi(argv[1]);
		if(argc>2) FLOAT=(OSAL_u64 ) atoi(argv[2]);
		if((INT>4000)||(FLOAT>999999) )
			return -2;
		tune = (INT<<32) + (FLOAT<<32)/1000000;
		sys_rtc_set_fineCorrection(tune);
	}
	sys_rtc_get_corrections(&freq,&tune);
	INT=(tune>>32);
	FLOAT=((tune&0xFFFFFFFF)*1000000000)>>32;
	printline("Fine:%d.%.9d",(int) INT,(int) FLOAT);
	INT=(freq>>32);
	FLOAT=((freq&0xFFFFFFFF)*1000000000)>>32;
	printline("Clock corr: %d.%.9d",(int) INT,(int) FLOAT);
	return 0;
}


signed char rtcclock_fct(int argc, char** argv)
{
	OSAL_timeval after,before,diff;
	OSAL_u32 i;
	OSAL_u32 n,T,delayT=0;

	if(argc<3) return -1;
	n=atoi(argv[1]);
	T=atoi(argv[2]);
	OSAL_gettimeofday(&before,NULL);
	printline("%ds %.6du ",before.tv_sec,before.tv_usec);
	OSAL_gettimeofday(&after, NULL);
	OSAL_timersub_realtime(&after,&before,&diff);
	delayT=T-diff.tv_usec+diff.tv_sec/1000000;//to avoid considering printline time
	udelay(delayT);
	if(n==0) i=1;
	else i=n-1;
	do
	{
		OSAL_gettimeofday(&before,NULL);
		OSAL_timersub_realtime(&before,&after,&diff);
		printline("%ds %.6du (Elapsed %ds %.6du)",before.tv_sec,before.tv_usec,diff.tv_sec,diff.tv_usec);
		OSAL_gettimeofday(&after,NULL);
		OSAL_timersub_realtime(&after,&before,&diff);
		delayT=T-diff.tv_usec+diff.tv_sec/1000000;//to avoid considering printline time
		if(n>0) i--;
		udelay(delayT);
	}while(i>0);
	return 0;
}

signed char periph_lp_fct(int argc, char** argv)
{
	unsigned char lp_en;
	if(argc<2) return -1;
	lp_en = (unsigned char)atoi(argv[1]);

	if(lp_en)
	{
		HAL_EXTI_LowPower();
		HAL_GPIO_LowPower();
	}
	else
	{
		HAL_EXTI_Init();
		HAL_GPIO_Init();
		HAL_GPIO_OutOfLowPower();
	}

	return 0;
}

signed char io_ctrl_fct(int argc, char** argv)
{
   	unsigned char gpionb=0;
   	unsigned char state=0;
   	char group=0;
   	GPIO_TypeDef* GPIOx;
   	uint16_t GPIO_Pin;

	if(argc<4) return -1;
	group = argv[1][0];
	gpionb = atoi(argv[2]);
	state = atoi(argv[3]);
	switch(group)
	{
		case 'a':case 'A':GPIOx = GPIOA;break;
		case 'b':case 'B':GPIOx = GPIOB;break;
		case 'c':case 'C':GPIOx = GPIOC;break;
		default:
		return -2;
	}
	switch(gpionb)
	{
		case 0:	GPIO_Pin = GPIO_Pin_0;break;
		case 1: GPIO_Pin = GPIO_Pin_1;break;
		case 2: GPIO_Pin = GPIO_Pin_2;break;
		case 3: GPIO_Pin = GPIO_Pin_3;break;
		case 4: GPIO_Pin = GPIO_Pin_4;break;
		case 5: GPIO_Pin = GPIO_Pin_5;break;
		case 6: GPIO_Pin = GPIO_Pin_6;break;
		case 7: GPIO_Pin = GPIO_Pin_7;break;
		case 8: GPIO_Pin = GPIO_Pin_8;break;
		case 9: GPIO_Pin = GPIO_Pin_9;break;
		case 10: GPIO_Pin = GPIO_Pin_10;break;
		case 11: GPIO_Pin = GPIO_Pin_11;break;
		case 12: GPIO_Pin = GPIO_Pin_12;break;
		case 13: GPIO_Pin = GPIO_Pin_13;break;
		case 14: GPIO_Pin = GPIO_Pin_14;break;
		case 15: GPIO_Pin = GPIO_Pin_15 ;break;
		default:
		return -2;
	}
	
	if(state) GPIO_SetBits(GPIOx, GPIO_Pin);
	else GPIO_ResetBits(GPIOx, GPIO_Pin);
	send_host(argv[0], 0, 1, 0);
	return 0;
}

signed char io_read_fct(int argc, char** argv)
{
	unsigned char gpionb=0;
	char type=0;
	char group=0;
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;

	if(argc<4)  return -1;
	group = argv[1][0];
	gpionb = atoi(argv[2]);
	type = argv[3][0];
	switch(group)
	{
		case 'a':case 'A':GPIOx = GPIOA;break;
		case 'b':case 'B':GPIOx = GPIOB;break;
		case 'c':case 'C':GPIOx = GPIOC;break;
		default:
		return -1;
	}
	switch(gpionb)
	{
		case 0:	GPIO_Pin = GPIO_Pin_0;break;
		case 1: GPIO_Pin = GPIO_Pin_1;break;
		case 2: GPIO_Pin = GPIO_Pin_2;break;
		case 3: GPIO_Pin = GPIO_Pin_3;break;
		case 4: GPIO_Pin = GPIO_Pin_4;break;
		case 5: GPIO_Pin = GPIO_Pin_5;break;
		case 6: GPIO_Pin = GPIO_Pin_6;break;
		case 7: GPIO_Pin = GPIO_Pin_7;break;
		case 8: GPIO_Pin = GPIO_Pin_8;break;
		case 9: GPIO_Pin = GPIO_Pin_9;break;
		case 10: GPIO_Pin = GPIO_Pin_10;break;
		case 11: GPIO_Pin = GPIO_Pin_11;break;
		case 12: GPIO_Pin = GPIO_Pin_12;break;
		case 13: GPIO_Pin = GPIO_Pin_13;break;
		case 14: GPIO_Pin = GPIO_Pin_14;break;
		case 15: GPIO_Pin = GPIO_Pin_15 ;break;
		default:
		return -1;
	}
	if((type=='i')||(type== 'I') ){
		printline("IN:%c%d=%d,GPIO%c=%x",group,gpionb,GPIO_ReadInputDataBit(GPIOx,GPIO_Pin),group,GPIO_ReadInputData(GPIOx));
	}
	else {
		printline("OUT:%c%d=%d,GPIO%c=%x",group,gpionb,GPIO_ReadOutputDataBit(GPIOx,GPIO_Pin),group,GPIO_ReadOutputData(GPIOx));
	}
	send_host(argv[0], 0, 1, 0);
	return 0;
}

signed char msq_en_fct(int argc, char** argv)
{
	if(argc<3) return -1;
	if(atoi(argv[1]) == 1)
		sys_enable_msq(OSAL_true);
	else
		sys_enable_msq(OSAL_false);
	if(atoi(argv[2]) == 1)
		sys_enable_pm(OSAL_true);
	else
		sys_enable_pm(OSAL_false);
	test_reinit();
	return 0;
}

signed char lta_fct(int argc, char** argv)
{
	if(argc<2) return -2;
	if(atoi(argv[1]) == 1)
		sys_enable_lta(OSAL_true);
	else
		sys_enable_lta(OSAL_false);
	test_reinit();
	return 0;
}

signed char w_fct(int argc, char** argv)
{
	unsigned char pp_bank,pp_reg,pp_val;
	char *endptr;	
	if(argc<4) return -1;
	pp_bank = (unsigned char)strtoul(argv[1], &endptr, 16); //Hexadecimal
	pp_reg = (unsigned char)strtoul(argv[2], &endptr, 16); //Hexadecimal
	pp_val = (unsigned char)strtoul(argv[3], &endptr, 16); //Hexadecimal		  
	pp_hw_reg_write(0, pp_bank, pp_reg, pp_val);
	send_host(argv[0], 0, 3, (signed long)pp_bank, (signed long)pp_reg, (signed long)pp_val);
	return 0;
}

signed char r_fct(int argc, char** argv)
{
	unsigned char pp_bank,pp_reg,pp_val;
	char *endptr;	
	if(argc<3) return -1;
	pp_bank = (unsigned char)strtoul(argv[1], &endptr, 16); //Hexadecimal
	pp_reg = (unsigned char)strtoul(argv[2], &endptr, 16);
	pp_hw_reg_read(0, pp_bank, pp_reg, &pp_val);
	printline("BNK 0x%x REG 0x%x Val 0x%x",pp_bank,pp_reg,pp_val);
	send_host(argv[0], 0, 3, (signed long)pp_bank, (signed long)pp_reg, (signed long)pp_val);
	return 0;
}

signed char thld_fct(int argc, char** argv)
{
  	signed char ret=0;
	unsigned long Treshold = 0;
	unsigned char treshold_tab[4];
	if(argc<2) return -2;
	Treshold = atoi(argv[1]);
	printline("Thld:%d",Treshold);
	treshold_tab[0] = (unsigned char)(Treshold & 0x0000FF);
	treshold_tab[1] = (unsigned char)((Treshold >> 8 ) & 0x0000FF);
	treshold_tab[2] = (unsigned char)((Treshold >> 16) & 0x0000FF);
	treshold_tab[3] = (unsigned char)((Treshold >> 24) & 0x0000FF);

	pp_hw_burst_write(0,BAR_CFG_DIGITAL, BAR_NRJCOR_THLD_W0, treshold_tab , 4);  // treshold
	pp_hw_burst_write(0,BAR_CFG_DIGITAL, BAR_LEAD_EDGE_THLD_W0, treshold_tab , 4); // rng treshold	

	return ret;
}

signed char read_thld_fct(int argc, char** argv)
{
	int zone = 0;
	if(argc<2) return -1;
	zone = atoi(argv[1]);
	if(zone < 1 || zone > 5) zone = DEFAULT_ZONE;
	send_host(	argv[0],
			0,
			8,
			prodSection.manufacturerSection.thresholds[zone-1][0],
			prodSection.manufacturerSection.thresholds[zone-1][1],
			prodSection.manufacturerSection.thresholds[zone-1][2],
			prodSection.manufacturerSection.thresholds[zone-1][3],
			prodSection.manufacturerSection.thresholds[zone-1][4],
			prodSection.manufacturerSection.thresholds[zone-1][5],
			prodSection.manufacturerSection.thresholds[zone-1][6],
			prodSection.manufacturerSection.thresholds[zone-1][7]
		);
	return 0;
}

signed char thld_test_fct(int argc, char** argv) {
	unsigned char channel=0, zone=0, path=0;
	long thld=0;
	long minthld=0,maxthld=0;
	int c,cmin=1,cmax=4,z,zmin=1,zmax=5,p,pmin=1,pmax=2;
	if(argc<1) return -2;
	if(argc>1) channel = atoi(argv[1]);
	if(argc>2) zone = atoi(argv[2]);
	if(argc>3) path = atoi(argv[3]);
	if(channel!=0)
	{
		cmin=channel;
		cmax=channel;
	}
	if(zone!=0)
	{
		zmin=zone;
		zmax=zone;
	}
	if(path!=0)
	{
		pmin=path;
		pmax=path;
	}
	for(c=cmin;c<=cmax;c++)
	{
		for(z=zmin;z<=zmax;z++)
		{
			for(p=pmin;p<=pmax;p++)
			{
				thld = sys_noise_or_treshold_test(c,z,p,&minthld,&maxthld,1,NULL);
				printline("C%dZ%dP%d: %d[min:%d,max:%d]",c,z,p,thld,minthld,maxthld);
			}
		}
	}
	return 0;
}

signed char noise_test_fct(int argc, char** argv) {
	//arguments provided
	signed char ok = 0;
	int ch;
	int zone;
	int frequency;
	int length;
	unsigned char rf_path;
	OSAL_u32 thld=0;
	OSAL_u32 period_us = 0, measure_time_us = 0;
	int i = 0;

	if (argc <6) return -1;
	ch = atoi(argv[1]);
	zone = atoi(argv[2]);
	frequency = atoi(argv[3]);
	length = atoi(argv[4]);
	rf_path = (unsigned char) atoi(argv[5]);

	// start : values input verification
	if (ch < 1 || ch > 4) ok = -1;
	else if (zone < 1 || zone > 5) ok = -1;
	else if (frequency < 1) ok = -1;
	else if (length < 0) ok = -1;
	else if (rf_path < 1 || rf_path > 2)ok = -1;

	if (ok == -1) return -2;
	// end : values input verification

	period_us = (1000000 / frequency);

	// (length == 0) => infinite
	i = (length == 0 ? -1 : 0);

	printline("C%dZ%dP%d:", ch, zone, rf_path);
	printline("noise;temp(mC)");
	while (i < frequency * length) 
	{
		//remember time used for printing info
		//reading info
		HAL_ADC_Read_Values();
		//measuring noise && temperature calculation
		thld = sys_noise_or_treshold_test(ch,zone,rf_path,NULL,NULL,0,&measure_time_us);
		//if time used is less than the period we want to wait
		if(period_us>measure_time_us) udelay(period_us - measure_time_us);
		printline("%d;%d", thld, Intern_Temperature);
		if (length != 0) i++;
	}
	return 0;
}

signed char alloclevel_fct(int argc, char** argv) 
{
	if (argc < 2) return -1;
	OSAL_setAllocTraceLevel(atoi(argv[1]));    
	return 0;
}

signed char allocate_fct(int argc, char** argv) 
{
	// sure this will loose memory. just use it <i>before</i> doing anything else,
	// in order to check for limit value of heap size
	// after wich we have too low stack 
	if (argc < 2) return -1;
	switch (atoi(argv[1])) 
	{
		case 0:return -2;
		default:
				printline("%d bytes",atoi(argv[1]));
				OSAL_malloc(atoi(argv[1]), 50);
				break;
	}
	return 0;
}

#endif //#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL

#ifdef UWB_SPI_TEST_API
#ifdef UWB_COM_CAN_ANSWER
signed char api_fct(int argc, char** argv)
{
	signed char ok=-1;
	int i;
	int foundExtra=-1;
	int foundExtraEnd=-1;
	char* p;
	uwb_com_command_t cmd;
	OSAL_u8 commandId = 0xFF;
	OSAL_u8 subCommand = UWB_SC_NONE;
	OSAL_u32 parameter = UWB_P_NOPARAMETER;
	OSAL_u8* addData =NULL;
	OSAL_s32 additionalDataLen = 0;    
	OSAL_u8*  outData=NULL;
	OSAL_s32  outDataLen=0;
	OSAL_u32 errLine=0;
	OSAL_u8 showoutput=1;

	if(argc==1) {errLine = __LINE__;goto beach;}
	for(i=1;i<argc;i++)
	{
		//cmd:XX sub:XX prm:XXXX answer:<y/n> Elen:LL Edata:<aa[1] ... aa[N]>
		if(strncmp(argv[i],"c=",2)==0)
			commandId = (OSAL_u8) strtol((argv[i]+2),&p,16);
		else if(strncmp(argv[i],"s=",2)==0)
			subCommand = (OSAL_u8) strtol((argv[i]+2),&p,16);
		else if(strncmp(argv[i],"p=",2)==0)
			parameter = (OSAL_u32) strtol((argv[i]+2),&p,16);
		else if(strncmp(argv[i],"ol=",3)==0)
		{
			sys_get_shared_data(1,&outData,NULL);
			outDataLen = atoi((argv[i]+3));
			sys_set_shared_data_size(1,outDataLen);
		}
		else if(strncmp(argv[i],"a:y",3)==0)
		  showoutput=1;
		else if(strncmp(argv[i],"a:n",3)==0)
		  showoutput=0;
		else if(strncmp(argv[i],"E:<",3)==0)
		{
			foundExtra = i;
			if(argv[i][5]=='>')
				foundExtraEnd = i;
		}
		else if(argv[i][2]=='>')
		{
			foundExtraEnd = i;
		}
	}
	if(commandId == 0xFF)
	{
		errLine = __LINE__;
		goto beach;
	}
	if((foundExtra>=1)&&(foundExtraEnd>=foundExtra))
	{
		sys_get_shared_data(0,&addData,NULL);
		sys_set_shared_data_size(0,foundExtraEnd-foundExtra+1);
		addData[0] = strtol((argv[i]+3),&p,16);//first arg is E:<XX
		for(i=foundExtra+1;i<=foundExtraEnd;i++)
		{
			addData[i-foundExtra] = strtol(argv[i],&p,16);
		}
		additionalDataLen = foundExtraEnd-foundExtra+1;
	}
	if(uwb_com_create_command(&cmd,commandId,subCommand,parameter,addData,additionalDataLen,outData,outDataLen)!=OSAL_OK)
	{
		errLine = __LINE__;
		goto beach;
	}    
	uwb_com_answer_gotcommand(NULL,&cmd);
	//if there is data, or additionnal data, cmd.add, or cmd.data and len have been updated by uwb_com_answer_gotcommand
	if(cmd.outData!=NULL) printline("Ack=%d",cmd.outData[0]);//show 16 byts / line
	if(cmd.cmdAnswer != UWB_C_ANSWER_ACK) {errLine = __LINE__;goto beach;}
	//manage outData, show on screen some
	if((cmd.outData!=NULL)&&(showoutput==1))
	{
		printline("OUT:[");
		for(i=0;i<outDataLen;i++)
		{
			print("0x%x, ",cmd.outData[i]);
			if((i)&&(!(i%16))) printline("");
		}
		printline("]");
	}
	//enqueue cmd, it will be done on next main loop
	uwb_enqueue_cmd(&cmd);
	//rest of procedure will be done in dequeue
	ok = 0;

beach:
	if(ok) printline("Error@line %d",(int) errLine);
	send_host(argv[0], 0, 1, (signed long)ok);
	return ok;
}
#endif //#ifdef UWB_COM_CAN_ANSWER

#ifdef UWB_COM_CAN_SEND
signed char apiExt_fct(int argc, char** argv)
{
	signed char ok=-1;
	#ifdef UWB_COM_CAN_SEND
	int i;
	int foundExtra=-1;
	int foundExtraEnd=-1;
	char* p;
	OSAL_u8 commandId = 0xFF;
	OSAL_u8 subCommand = UWB_SC_NONE;
	OSAL_u32 parameter = UWB_P_NOPARAMETER;
	OSAL_u8* addData =NULL;
	OSAL_s32 additionalDataLen = 0;    
	OSAL_u8*  outData=NULL;
	OSAL_s32  outDataLen=0;
	OSAL_u32 errLine=0;
	OSAL_u8 showoutput=1;

	if(argc==1) {errLine = __LINE__;goto beach;}
	for(i=1;i<argc;i++)
	{
		//cmd:XX sub:XX prm:XXXX answer:<y/n> Elen:LL Edata:<aa[1] ... aa[N]>
		if(strncmp(argv[i],"c=",2)==0)
			commandId = (OSAL_u8) strtol((argv[i]+2),&p,16);
		else if(strncmp(argv[i],"s=",2)==0)
			subCommand = (OSAL_u8) strtol((argv[i]+2),&p,16);
		else if(strncmp(argv[i],"p=",2)==0)
			parameter = (OSAL_u32) strtol((argv[i]+2),&p,16);
		else if(strncmp(argv[i],"ol=",3)==0)
		{
			sys_get_shared_data(1,&outData,NULL);
			outDataLen = atoi((argv[i]+3));
			sys_set_shared_data_size(1,outDataLen);
		}
		else if(strncmp(argv[i],"a:y",3)==0)
		  showoutput=1;
		else if(strncmp(argv[i],"a:n",3)==0)
		  showoutput=0;
		else if(strncmp(argv[i],"E:<",3)==0)
		{
			foundExtra = i;
			if(argv[i][5]=='>')
				foundExtraEnd = i;
		}
		else if(argv[i][2]=='>')
			foundExtraEnd = i;
	}
	if(commandId == 0xFF)
	{
		errLine = __LINE__;
		goto beach;
	}

		if((foundExtra>=1)&&(foundExtraEnd>=foundExtra))
		{
		sys_get_shared_data(0,&addData,NULL);
		sys_set_shared_data_size(0,foundExtraEnd-foundExtra+1);
		addData[0] = strtol((argv[i]+3),&p,16);//first arg is E:<XX
		for(i=foundExtra+1;i<=foundExtraEnd;i++)
		{
			addData[i-foundExtra] = strtol(argv[i],&p,16);
		}
		additionalDataLen = foundExtraEnd-foundExtra+1;
	}
	if(uwb_com_create_and_send_command(	NULL,
					commandId,
					subCommand,
					parameter,
					addData,
					additionalDataLen,
					outData,
					outDataLen)!=OSAL_OK)
	{
		errLine = __LINE__;
		goto beach;
	}
	//manage outData, show on screen the result
	if((outData!=NULL)&&(showoutput == 1))
	{
		printline("OUT:[");
		for(i=0;i<outDataLen;i++)
		{
			print("0x%x, ",outData[i]);
			if((i)&&(!(i%16))) printline("");
		}
		printline("]");
	}
	ok = 0;
beach:
	if(ok) printline("Error@line %d",(int) errLine);
#endif //#ifdef UWB_COM_CAN_SEND
	send_host(argv[0], 0, 1, (signed long)ok);
	return 0;
}
#endif //#ifdef UWB_COM_CAN_SEND
#endif //#ifdef UWB_SPI_TEST_API

signed char testBitfiled_fct(int argc, char** argv)
{
	send_host(argv[0],0,1,prodSection.manufacturerSection.testsResults);
	return 0;
}

#endif //#ifdef CONFIG_ACTIVATE_UART_SHELL


