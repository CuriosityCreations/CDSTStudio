/**
 * @file DAC.c
 * @brief DAC.c
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

#include "stm32f0xx_dac.h"
#include "stm32f0xx_rcc.h"
#include "dac.h"
#include "ADC.h"

#pragma GCC optimize("Os")

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DAC_VMAX_MICRO_V     1749999UL
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static unsigned long dac_current_valUV=0;
static unsigned short dac_current_val=0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void HAL_DAC_DeInit (void)
{
	DAC_DeInit();
}

void HAL_DAC_Init (void)
{
	DAC_InitTypeDef sDAC;

	// init the dac config
	// enable clock for DAC peripheral
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	DAC_StructInit(&sDAC);

	sDAC.DAC_Trigger = DAC_Trigger_None;
	sDAC.DAC_WaveGeneration = DAC_WaveGeneration_None;
	sDAC.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	sDAC.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_2, &sDAC);

	//DAC_Cmd(DAC_Channel_2, ENABLE);
}

void dac_get_value(unsigned short *cur_value_on12bits)
{
	*cur_value_on12bits = dac_current_val;
}

void dac_get_valueUV(unsigned long *cur_value_inuV)
{
	*cur_value_inuV = dac_current_valUV;
}

void dac_set_valueUV(unsigned long new_value_inuV)
{
	// Convert value
	unsigned long val12b = new_value_inuV;
	if (val12b > DAC_VMAX_MICRO_V)
		val12b = DAC_VMAX_MICRO_V;
	dac_current_valUV = val12b;
	/*now use 10uv as lsb, avoid u32 overflow*/
	val12b /= 10;
	val12b *= 0xFFF;
	val12b /= VDDA;
	dac_set_value(val12b);
}

void dac_set_value(unsigned short new_value_on12bits)
{
    dac_current_val = new_value_on12bits&0xFFF;
    DAC_SetChannel2Data(DAC_Align_12b_R, dac_current_val);
}

void dac_enable(unsigned char enable)
{
	DAC_Cmd(DAC_Channel_2, enable);
}










