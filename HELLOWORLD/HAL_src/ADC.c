/**
 * @file ADC.c
 * @brief ADC.c
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

#include "stm32f0xx_adc.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_exti.h"
#include "ADC.h"

#include "osal_trace.h"
#include "osal_time.h"

#pragma GCC optimize("Os")

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_ADDRESS     ((uint32_t)0x40012440)

#define TS_CAL_30		*((unsigned short*) 0x1FFFF7B8)
#define TS_CAL_110		*((unsigned short*) 0x1FFFF7C2)
#define VDD_APPLI		((uint32_t) (250))
#define VDD_CALIB		((uint32_t) (330))
#define VREFINT_CAL		*((unsigned short*) 0x1FFFF7BA)
#define VREFINT_CAL_T	(VDD_CALIB*1000*VREFINT_CAL)
#define AVG_SLOPE		(uint32_t) ((110-30) * 1000 / (TS_CAL_110 - TS_CAL_30))


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
unsigned short ADCConvertedValue[4]={0}; //Array that is used to store the calculated DMA values for ADC1
unsigned char ADC_MeasureDone = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/* Public variables ---------------------------------------------------------*/
unsigned long Vptest=0;
unsigned long Vbat_Module=0;
unsigned int VDDA_real=0;
long Intern_Temperature=0;

void HAL_ADC_DeInit (void)
{
	ADC_DeInit(ADC1);
}

void HAL_ADC_Init(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	NVIC_InitTypeDef 	  NVIC_InitStructure;
	/* ADC1 DeInit */
	ADC_DeInit(ADC1);
	
	/* Enable ADC clock ****************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv2);

	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);

	/* Configure the ADC1 in discontinous mode with a resolutuion equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // Manual trigger
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; // Unused
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure);

	// enable temp sensor, vref and vbat
	ADC_TempSensorCmd(ENABLE);
	ADC_VrefintCmd(ENABLE);
	ADC_VbatCmd(DISABLE); // VBAT intern, useless because regulated

	/* sequence of conversion */
	
	ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_239_5Cycles); // VBAT extern (!= VBAT intern)
	ADC_ChannelConfig(ADC1, ADC_Channel_6, ADC_SampleTime_239_5Cycles); // Used in prod for voltage measures
	ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor, ADC_SampleTime_239_5Cycles); // Temperature sensor
	ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint, ADC_SampleTime_239_5Cycles); // Internal voltage reference

	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);

	/* Enable DMA request after last transfer (OneShot-ADC mode) */
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

	/* DMA1 Stream1 channel1 configuration **************************************/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	
	DMA_Cmd(DMA1_Channel1, DISABLE);

	//==Configure the DMA Interrupt
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	ADC_MeasureDone = 0;
}


void DMA1_Channel1_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_TC1))
	{
		/* clear int pending bit */
		DMA_ClearITPendingBit(DMA1_IT_GL1);
		// This indicates that ADC AND DMA have finished their job
		ADC_MeasureDone = 1;
	}
}
static void HAL_ADC_Start()
{
	ADC_GetCalibrationFactor(ADC1);
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void HAL_ADC_Read_Values(void)
{
	volatile OSAL_u32 timeout_cnt = 0;
	OSAL_u32 timeout = 1000;
	ADC_MeasureDone = 0;
	HAL_ADC_Init();
	HAL_ADC_Start();
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));

	ADC_StartOfConversion(ADC1);

	// Wait DMA has finished transfer
	do
	{
		timeout_cnt++;
	} while ((timeout_cnt < timeout) && (ADC_MeasureDone == 0));
	if(ADC_MeasureDone != 0)//timeout
	{
		VDDA_real = 330000 * ((float)VREFINT_CAL/(float)(ADCConvertedValue[3]&0x0FFF));
		// calculation of temperature
		Intern_Temperature = AVG_SLOPE * ((uint32_t)ADCConvertedValue[2] * VDD_APPLI / VDD_CALIB - (uint32_t)TS_CAL_30) + (uint32_t)22000;	// resolution 0.001 deg
		// calculation of Vptest
		Vptest = ((ADCConvertedValue[1]&0x0FFF) * VDDA_real) / 4095;	// resolution 10uV
		// calculation of Vbat module: to be implemented, both sw and hw
		//Vbat_Module = ((ADCConvertedValue[0]&0x0FFF) * VDDA_real) / 4095;	// resolution 10uV
		Vbat_Module = VBAT_HIGH_THRES;
	}
	ADC_StopOfConversion(ADC1);
	DMA_Cmd(DMA1_Channel1, DISABLE);
	ADC_DMACmd(ADC1, DISABLE);
	ADC_Cmd(ADC1, DISABLE);
	HAL_ADC_DeInit();
	ADC_MeasureDone = 0;
}

