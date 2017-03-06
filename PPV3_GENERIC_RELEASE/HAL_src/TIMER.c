/**
 * @file TIMER.c
 * @brief TIMER.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Includes ------------------------------------------------------------------*/
#include "HAL_config.h"

#include "stm32f0xx_tim.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_pwr.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_spi.h"
#include "TIMER.h"

#include "osal_trace.h"

#include "SPI.h"
#include "stm32f0xx_gpio.h"
#pragma GCC optimize("Os")

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_MSEC_FOR_WAIT_COUNTER   0x200000 //=(0xFFFFFFFF/1000)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define DELAY_PRESCALER 48				//divide 48000000 by register give new frequency => 1mhZ => 1us precision

void HAL_TIMER2_Init ()
{
	// Be very careful if you change timer configuration
	// Some configurations are buggy (eg. random 4us delays instead of 10us in counterMode Up)
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	TIM_DeInit(TIM2);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = DELAY_PRESCALER;	// 1us
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseStructure.TIM_Period = 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
}

/* The tick interrupt handler.  This is always the same other than the part that
clears the interrupt, which is specific to the clock being used to generate the
tick. */
void TIM2_IRQHandler( void )
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		TIM_ClearITPendingBit( TIM2, TIM_IT_Update );
}

#define DEFAULT_TIMER3_RATE 1000 //ms
#define TIMER3_PRESCALER 1000
void HAL_TIMER3_Init (unsigned int rate_ms)
{
	unsigned int period;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	if(rate_ms !=0 )  period = rate_ms * 48;
	else period = 48*DEFAULT_TIMER3_RATE;
	
	TIM_Cmd (TIM3, DISABLE);     /* TIM3 disable */

	TIM_DeInit(TIM3);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/* TIM3 Configuration */
	TIM_TimeBaseStructInit (&TIM_TimeBaseStructure);
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period        = period; 
	TIM_TimeBaseStructure.TIM_Prescaler     = TIMER3_PRESCALER;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;      // 48MHz/4 timer clock
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	/* TIM3 TRGO selection */
	TIM_SelectOutputTrigger (TIM3, TIM_TRGOSource_Update);
	TIM_Cmd (TIM3, ENABLE);     /* TIM3 enable counter */	
}

unsigned int CaptureNumber=0;
unsigned int PeriodValue=0;
unsigned int IC1ReadValue1 = 0;
unsigned int IC1ReadValue2 =0;
extern OSAL_u64 MeasuredRTCClockFreq;

void TIM14_IRQHandler( void )
{
	if (TIM_GetITStatus(TIM14, TIM_IT_CC1) != RESET)
	{
		/* Clear TIM14 Capture Compare 1 interrupt pending bit */
		TIM_ClearITPendingBit(TIM14, TIM_IT_CC1);
		if(CaptureNumber == 0)
		{
		  /* Get the Input Capture value */
		  IC1ReadValue1 = TIM_GetCapture1(TIM14);
		  CaptureNumber = 1;
		}
		else if(CaptureNumber == 1)
		{
			RCC_ClocksTypeDef  RCC_ClockFreq;

			/* Get the Input Capture value */
			IC1ReadValue2 = TIM_GetCapture1(TIM14); 
			TIM_ITConfig(TIM14, TIM_IT_CC1, DISABLE);
			/* Capture computation */
			if (IC1ReadValue2 > IC1ReadValue1)
				PeriodValue = (IC1ReadValue2 - IC1ReadValue1);
			else
				PeriodValue = ((0xFFFF - IC1ReadValue1) + IC1ReadValue2);
			/* capture of two values is done */
			CaptureNumber = 2;
			/* Get SYSCLK, HCLK and PCLKx frequency */
			RCC_GetClocksFreq(&RCC_ClockFreq);
			//get timer 14 work back
			/* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
			MeasuredRTCClockFreq = (((((OSAL_u64) RCC_ClockFreq.PCLK_Frequency)<<32) / ((OSAL_u64) PeriodValue)) * 8);
		}
	}
}

void HAL_TIMER_LSI_Autocalibration()
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;

	TIM_Cmd (TIM14, DISABLE);     /* TIM14 disable */
	TIM_DeInit(TIM14);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	/* Configure TIM14 prescaler */
	TIM_PrescalerConfig(TIM14, 0, TIM_PSCReloadMode_Immediate);
	/* Connect internally the TIM14_CH1 to the RTC clock output */
	TIM_RemapConfig(TIM14, TIM14_RTC_CLK);
	/* TIM14 configuration: Input Capture mode ---------------------
	 The reference clock(LSE or external) is connected to TIM14 CH1
	 The Rising edge is used as active edge,
	 The TIM14 CCR1 is used to compute the frequency value 
	------------------------------------------------------------ */
	TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM14, &TIM_ICInitStructure);
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	CaptureNumber=0;
	PeriodValue=0;
	/* Enable TIM14 counter */
	TIM_Cmd(TIM14, ENABLE);
	/* Reset the flags */
	TIM14->SR = 0;
	/* Enable the CC1 Interrupt Request */  
	TIM_ITConfig(TIM14, TIM_IT_CC1, ENABLE);
	/* Wait until the TIM14 get 2 LSI edges (refer to TIM14_IRQHandler()  */
	while(CaptureNumber != 2) udelay(200);
	/* Deinitialize the TIM14 peripheral registers to their default reset values */
	TIM_DeInit(TIM14);
	TIM_Cmd (TIM14, DISABLE);     /* TIM14 disable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, DISABLE);
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

//-----------------------------------------------------------------------------
// High Level Service Routines
//-----------------------------------------------------------------------------

static void start_waitcounter_timeBase(waitTimeIn_timebase)
{
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_SetCounter(TIM2, waitTimeIn_timebase);
	TIM_Cmd(TIM2, ENABLE);
}

static unsigned char check_waitcounter_running()
{
	if(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update))
	{
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
		TIM_Cmd(TIM2, DISABLE);

		return 0;
	}
	return 1;
}

void msleep(unsigned short Time)
{
	if (Time)
	{
		if(Time>MAX_MSEC_FOR_WAIT_COUNTER)
			start_waitcounter_timeBase(MAX_MSEC_FOR_WAIT_COUNTER * 1000);
		else
			start_waitcounter_timeBase(Time * 1000);

		while(check_waitcounter_running());
	}
}

void udelay(unsigned long Delay)
{
	if (Delay)
	{
		start_waitcounter_timeBase(Delay);
		while(check_waitcounter_running());
	}
}

void reset_time(void)
{
	TIM_SetCounter(TIM2, 0);
}

void HAL_TIMER_DeInit()
{
	TIM_Cmd (TIM2, DISABLE);     /* TIM2 disable */
	TIM_DeInit(TIM2);
	TIM_Cmd (TIM3, DISABLE);     /* TIM3 disable */
	TIM_DeInit(TIM3);
	TIM_Cmd (TIM14, DISABLE);     /* TIM14 disable */
	TIM_DeInit(TIM14);
}

void HAL_TIMER_Init()
{
	HAL_TIMER2_Init ();
	HAL_TIMER3_Init (0);
	//TODO add other here
}


