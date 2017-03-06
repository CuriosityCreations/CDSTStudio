/**
 * @file RTC.c
 * @brief RTC.c
 * @author  bespoon.com
 * @date 22/07/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/


#include "HAL_config.h"

#include "stm32f0xx_pwr.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_rtc.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_misc.h"
#include "osal_type.h"
#include "osal_trace.h"
#include "osal_time.h"
#include "RTC.h"
#include "RCC.h"
#include "TIMER.h"
#include "types.h"
#include "ProdSectionDef.h"
#include "module_api.h"

#pragma GCC optimize("Os")


static OSAL_u8 SCALEFACTOR=16;
static OSAL_u8 nextSCALEFACTOR=16;

#define RTC_ASYNCHPREDIV_BAZ		128										// RTC_ASYNCHPREDIV x RTC_SYNCHPREDIV  = RTCFreq = 32768
#define RTC_SYNCHPREDIV_BAZ			256										// RTC_ASYNCHPREDIV x RTC_SYNCHPREDIV  = RTCFreq = 32768
#define RTC_SYNCHPREDIV_BAZ_SHIFT	8										// 256 = (1<<8)



#define RTC_ASYNCHPREDIV		(RTC_ASYNCHPREDIV_BAZ/(SCALEFACTOR))		// RTC_ASYNCHPREDIV x RTC_SYNCHPREDIV  = RTCFreq
#define RTC_SYNCHPREDIV			(RTC_SYNCHPREDIV_BAZ*(SCALEFACTOR))			// you cna increase precision by increasing SCALEFACTOR, but this will increase cunsumption

#define RTC_ASYNCHPREDIV_MINUS1	((RTC_ASYNCHPREDIV)-1)
#define RTC_SYNCHPREDIV_MINUS1	((RTC_SYNCHPREDIV)-1)

#define RTC_LIMIT 				((OSAL_u64) ((1000000/(RTC_SYNCHPREDIV))))	// This define an alarm granularity of 1/RTC_SYNCHPREDIV, in usecond ( )

#define RTC_REFERENCE_FREQ		(32768*(RTC_SYNCHPREDIV*RTC_ASYNCHPREDIV/RTC_SYNCHPREDIV_BAZ/RTC_ASYNCHPREDIV_BAZ))	// Can be lsi or lse.
#define DEFAULT_RTC_FREQ 0x800000000000 //32768Hz<<32
#define FREQCORR_ERROR_MAX 0x51EB852 		//2% on 64bits, Q32

OSAL_u64 MeasuredRTCClockFreq=DEFAULT_RTC_FREQ;	//default lsi freq
static OSAL_u64 freqCorrection=0x100000000;		//This correction will be MeasuredRTCClockFreq/RTC_REFERENCE_FREQ
static OSAL_u64 tuneCorrection=0x100000000;		//This correction will be MeasuredRTCClockFreq/RTC_REFERENCE_FREQ
static OSAL_u8 RTC_SYNCHPREDIV_SHIFT=12; //div or mul by RTC_SYNCHPREDIV_BAZ*(SCALEFACTOR) = (<< or >< ) by (RTC_SYNCHPREDIV_BAZ_SHIFT+4)

OSAL_u64 corInt=0x100000000;
OSAL_u64 corFloat=0;
OSAL_u64 invCorInt=0x100000000;
OSAL_u64 invCorFloat=0;

static OSAL_void updateRealCor()
{
	OSAL_u64 realCor=0x100000000; //correction Q32 on 64bits
	OSAL_u64 tuneCorrectionINT=tuneCorrection&0xFFFFFFFF00000000;
	OSAL_u64 tuneCorrectionFLOAT=tuneCorrection&0xFFFFFFFF;
	OSAL_u64 freqCorrectionINT=freqCorrection&0xFFFFFFFF00000000;
	OSAL_u64 freqCorrectionFLOAT=freqCorrection&0xFFFFFFFF;
	//correction are Q32
	//result on Q32
	//we must do (Q32*q32)>>32
	//But multiplication of Q32xq32 =q64, wich will overflow
	//so shift some member, to stay on u64 values
	//using (a+b)*(c+d), splitting int and float part
	realCor = ((tuneCorrectionINT>>32)*freqCorrectionINT);
	realCor+= ((tuneCorrectionINT>>32)*(freqCorrectionFLOAT));
	realCor+= (tuneCorrectionFLOAT*(freqCorrectionINT>>32));
	realCor+= ((tuneCorrectionFLOAT*freqCorrectionFLOAT)>>32);
	if(realCor!=0) //otherwise not possible, don't modify previous value
	{
		OSAL_u64 unityQ63=0x8000000000000000;
		OSAL_u64 invrealCor=0;
		corInt = (realCor&0xFFFFFFFF00000000); 
		corFloat = (realCor&0xFFFFFFFF);
		//divier 1/realCor. realCor on 32bits. shift 1 from63bits, divide by Q32=>Q31, reshift <<1 = Q32 
		invrealCor=((unityQ63/realCor)<<1);//loose precision on 1^-10
		//get invertedCorrection now = 0x100000000/realCor = 0x800000000
		invCorInt=invrealCor&0xFFFFFFFF00000000;
		invCorFloat=invrealCor&0xFFFFFFFF;
	}
}

static OSAL_u64 HAL_convertTVALToUsec64Corr(OSAL_timeval *tim,OSAL_u64 corint,OSAL_u64 corfloat)
{
	OSAL_u64 usec64=(((OSAL_u64) tim->tv_sec)*1000000)+((OSAL_u64) tim->tv_usec);
	OSAL_u64 usec64INT   = ((usec64)&0x7FFFFFFF00000000);
	OSAL_u64 usec64FLOAT = ((usec64)&0x00000000FFFFFFFF);

	usec64=(corint>>32)*usec64INT;
	usec64+=(corint>>32)*usec64FLOAT;
	usec64+=(corfloat*(usec64INT>>32));
	usec64+=((corfloat*usec64FLOAT)>>32);
	return usec64;
}

static OSAL_u32 roundusec_2_SYNCHPREDIV(OSAL_u32 timeusec)
{
	OSAL_u32 reg=0;
	OSAL_u64 invertedusec=(OSAL_u64) (1000000-timeusec);
	OSAL_u64 u1 = 0;
	OSAL_u64 u2 = 0;
	if(invertedusec<=RTC_LIMIT)
		return 0;
	u1 = (invertedusec<<RTC_SYNCHPREDIV_SHIFT)/1000000;
	u2 = ((invertedusec<<RTC_SYNCHPREDIV_SHIFT)+500000)/1000000;
	if(u1 < u2) 
		reg = (u2-1);
	else
		reg = (u1-1);
	return reg;
}

static OSAL_u64 HAL_RTC_GetSysTime_usec64()
{
	OSAL_u64 usec64=0;
	RTC_TimeTypeDef tt;
	uint32_t ss=RTC_GetSubSecond();

	RTC_GetTime(RTC_Format_BIN, &tt);
	//we could <<32 before division to gain in precision. We don't. Indeed, in case RTC_SYNCHPREDIV = 32768, it overflows
	//<< 28 allow to pass this case
	//usec64=((((((OSAL_u64) (RTC_SYNCHPREDIV_MINUS1-ss))*1000000)<<28)/RTC_SYNCHPREDIV)>>28); // = (FF-reg value)*1000000/RTC_SYNCHPREDIV
	//But this is equivalent to, without overflow
	usec64 = (((((OSAL_u64) (RTC_SYNCHPREDIV_MINUS1-ss)))*1000000)>>RTC_SYNCHPREDIV_SHIFT); // = (FF-reg value)*1000000/RTC_SYNCHPREDIV
	usec64 +=  ((((OSAL_u64) (tt.RTC_Seconds+tt.RTC_Minutes*60+tt.RTC_Hours*3600))*1000000));
	//we have up to 68years on 64bits, Q0
	return usec64;
}

void HAL_RTC_DeInit()
{
	RTC_DeInit(); 
}

void HAL_RTC_Init()
{
	RTC_InitTypeDef RTC_InitStructure; 
	//put here factor change:
	SCALEFACTOR=nextSCALEFACTOR;
	RTC_SYNCHPREDIV_SHIFT=0;
	while((1<<RTC_SYNCHPREDIV_SHIFT)!=RTC_SYNCHPREDIV) RTC_SYNCHPREDIV_SHIFT++;

	//put config to 0x7F, RTC_SYNCHPREDIV_MINUS1, mean consider we have a '32768hZ' * (RTC_SYNCHPREDIV/256) on rtc clock
	RTC_InitStructure.RTC_SynchPrediv  = RTC_SYNCHPREDIV_MINUS1;
	RTC_InitStructure.RTC_AsynchPrediv = RTC_ASYNCHPREDIV_MINUS1;
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);
	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();
	RTC_TimeTypeDef RTC_TimeStructure;
	/* Set the time to 00h 00mn 00s AM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x00;
	RTC_TimeStructure.RTC_Minutes = 0x00;
	RTC_TimeStructure.RTC_Seconds = 0x00;  
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
}

#define RT_TOLERANCE_US 500 //used to manage other process wakeup

static OSAL_u64 alarmusec64WithoutCorrection=0;
static OSAL_u64 latestcurTimeWithoutCorrection=0;
static OSAL_u64 alarmRelativeToStartDay=0;
void HAL_RTC_ReLaunchOrStopAlarm()
{
	OSAL_u64 curusec64WithoutCorrection = HAL_RTC_GetSysTime_usec64()+RTC_LIMIT+RT_TOLERANCE_US;

	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	RTC_ClearITPendingBit(RTC_IT_ALRA);
	RTC_ClearFlag(RTC_FLAG_ALRAF);

	//got time is on24h. if current got time<latest one, mean we rool over 1day.
	if(curusec64WithoutCorrection<latestcurTimeWithoutCorrection) alarmRelativeToStartDay++;
	latestcurTimeWithoutCorrection=curusec64WithoutCorrection+alarmRelativeToStartDay*86400000000;
	//alarm is sure to have occured if < latesttime
	if(alarmusec64WithoutCorrection<=latestcurTimeWithoutCorrection)
		sys_set_wake_up_reason(SYS_WAKE_UP_RTC_MASK);
	else
	{
		RTC_AlarmTypeDef RTC_AlarmStructure;
		OSAL_u32 alarmh=0;
		OSAL_u32 alarmm=0;
		OSAL_u32 alarms=0;
		OSAL_u64 alarmus=0;
		RTC_AlarmStructInit(&RTC_AlarmStructure);

		RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31; // dont care
		RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date; // dont care
		RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
		//we must ring at alarm modulo 1day
		alarmus = (alarmusec64WithoutCorrection%86400000000);
		alarms = (OSAL_u32) (alarmus/1000000);
		alarmus = alarmus%1000000;
		alarmm = alarms/60;
		alarms = alarms%60;
		alarmh = alarmm/60;
		alarmm = alarmm%60;

		//otherwise ring at 0:0:0:0
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = alarmh;
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = alarmm;
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = alarms;
		RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
		RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
		//for milliseconds, 
		RTC_AlarmSubSecondConfig(RTC_Alarm_A, roundusec_2_SYNCHPREDIV((OSAL_u32) alarmus), RTC_AlarmSubSecondMask_None);
		RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		RTC_ClearFlag(RTC_FLAG_ALRAF);
		/* Enable AlarmA interrupt */
		RTC_ITConfig(RTC_IT_ALRA, ENABLE);
		/* Enable the alarmA */
		RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	}
}
void HAL_RTC_StartAlarm(OSAL_timeval* tim)
{
	latestcurTimeWithoutCorrection=HAL_RTC_GetSysTime_usec64();
	alarmusec64WithoutCorrection=HAL_convertTVALToUsec64Corr(tim,corInt,corFloat);
	alarmusec64WithoutCorrection+=latestcurTimeWithoutCorrection;
	alarmRelativeToStartDay = 0;
	if(alarmusec64WithoutCorrection<=RTC_LIMIT) alarmusec64WithoutCorrection=RTC_LIMIT+1;
	HAL_RTC_ReLaunchOrStopAlarm();
}

void HAL_RTC_CancelAlarm()
{
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	RTC_ClearITPendingBit(RTC_IT_ALRA);
	RTC_ClearFlag(RTC_FLAG_ALRAF);
}


void HAL_RTC_Autocalibrate()
{
	if((prodSection.manufacturerSection.hardwareCapabilities & CAN_ASSET_TRACKING_CAPS_MASK) == 0)
	{
		#define RTC_AUTOCAL_LOOP 5
		OSAL_u64 FreqMid=0;
		int i;
		OSAL_timeval tim;
		tim.tv_sec=5;
		for(i=0;i<RTC_AUTOCAL_LOOP;i++)
		{
			HAL_RTC_StartAlarm(&tim);
			HAL_TIMER_LSI_Autocalibration();
			HAL_RTC_CancelAlarm();
			HAL_RTC_Init();
			FreqMid+=MeasuredRTCClockFreq;
		}
		MeasuredRTCClockFreq=FreqMid/RTC_AUTOCAL_LOOP;
		freqCorrection=MeasuredRTCClockFreq/RTC_REFERENCE_FREQ;
	}
	tuneCorrection = 0x100000000;
	updateRealCor();
	HAL_RTC_Init();
}

void HAL_RTC_GetTime(OSAL_timeval *tim)
{
	if(tim!=NULL)
	{
		OSAL_u64 usec64=HAL_RTC_GetSysTime_usec64();
		OSAL_u64 sec64 = usec64/1000000;

		usec64=usec64-sec64*1000000;
		tim->tv_sec=(int) sec64;
		tim->tv_usec=(int) usec64;
	}
}


void HAL_RTC_Get_SysTime(OSAL_timeval *tim)
{
	OSAL_u64 usec64=HAL_convertTVALToUsec64Corr(tim,corInt,corFloat);

	tim->tv_sec=(int) (usec64/1000000);
	tim->tv_usec=(int) (usec64%1000000);
}

void HAL_RTC_Get_RealTime(OSAL_timeval *tim)
{
	OSAL_u64 usec64=HAL_convertTVALToUsec64Corr(tim,invCorInt,invCorFloat);

	tim->tv_sec=(int) (usec64/1000000);
	tim->tv_usec=(int) (usec64%1000000);
}

void HAL_RTC_GetFineTune(OSAL_u64* RTCClockCorrection,OSAL_u64* fineTune)
{
	if(RTCClockCorrection!=NULL) *RTCClockCorrection = freqCorrection;
	if(fineTune!=NULL) *fineTune = tuneCorrection;
}

void HAL_RTC_SetFineTune(OSAL_u64* RTCClockCorrection,OSAL_u64* fineTune)
{
	if(RTCClockCorrection!=NULL) freqCorrection = *RTCClockCorrection;
	if(fineTune!=NULL) tuneCorrection = *fineTune;
	updateRealCor();
}

void HAL_RTC_GetPrecisionFactor(unsigned char* factor)
{
	*factor=SCALEFACTOR;
}

void HAL_RTC_SetPrecisionFactor(unsigned char factor)
{
	int cnt=0;
	nextSCALEFACTOR = factor;
	while(nextSCALEFACTOR>1) {cnt++;nextSCALEFACTOR/=2;}
	while(cnt>0) {cnt--;nextSCALEFACTOR*=2;}
	if(nextSCALEFACTOR<1) nextSCALEFACTOR=1;
	if(nextSCALEFACTOR>128) nextSCALEFACTOR=128;
}

