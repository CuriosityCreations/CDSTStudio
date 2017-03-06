/**
 * @file EXTI.c
 * @brief EXTI.c
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

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_rtc.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_exti.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_spi.h"
#include "osal_type.h"
#include "osal_trace.h"
#include "osal_time.h"
#include "EXTI.h"
#include "SPI.h"
#include "RTC.h"
#include "USART.h"
#include "Ext_Com.h"
#include "module_api.h"

#pragma GCC optimize("Os")

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Global variables */

void HAL_EXTI_DeInit( void)
{
	SYSCFG_DeInit();
	EXTI_DeInit();
}

void HAL_EXTI_Init (void)
{
	
	//EXTI structure to init EXT
	EXTI_InitTypeDef EXTI_InitStructure;
	//NVIC structure to set up NVIC controller
	NVIC_InitTypeDef NVIC_InitStructure;

	/*----- init the wake up IRQ ------*/
	//Connect EXTI Line to Button Pin
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	EXTI_StructInit(&EXTI_InitStructure);
	//Configure Button EXTI line
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	//select interrupt mode
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//generate interrupt on rising edge
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	//enable EXTI line
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//send values to registers
	EXTI_Init(&EXTI_InitStructure);

	/*----- init the PP IRQ ------*/
	//Connect EXTI Line to Button Pin
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	EXTI_StructInit(&EXTI_InitStructure);
	//Configure Button EXTI line
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	//select interrupt mode
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//generate interrupt on rising edge
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	//enable EXTI line
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//send values to registers
	EXTI_Init(&EXTI_InitStructure);

	/*----- SPI1 NSS pin ------*/
	//Connect EXTI Line to NSS pin
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
	// Configure NSS EXTI line
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);


	//Enable and set Transfer Complete for DMA in tx to the highest priority
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_5_6_7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//*****************
	// UART 1
	//*****************
	if(USE_SHELL_USART_NB&0x10)
	{
		/* Connect Button EXTI Line to uart rx line */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource7);

		/* Configure User Button and IDD_WakeUP EXTI line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line7 ;  // PB7 for uart rx pin
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
	}

	//*****************
	// UART 2
	//*****************
	else if(USE_SHELL_USART_NB&0x20)
	{
		/* Connect Button EXTI Line to uart rx line */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource3);

		/* Configure User Button and IDD_WakeUP EXTI line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line3 ;  // PA3 for uart rx pin
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
	}


	//configure NVIC
	//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
	//set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	//enable IRQ channel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//update NVIC registers
	NVIC_Init(&NVIC_InitStructure);


	//configure NVIC
	//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
	//set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	//enable IRQ channel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//update NVIC registers
	NVIC_Init(&NVIC_InitStructure);

	//configure NVIC
	//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	//set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	//enable IRQ channel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//update NVIC registers
	NVIC_Init(&NVIC_InitStructure);

	/* EXTI configuration *******************************************************/
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the RTC Wakeup Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void HAL_EXTI_LowPower (void)
{
	// EXTI structure initialization
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_StructInit(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;

	// PP IRQ (PC13)
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_Init(&EXTI_InitStructure);
}

void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
		HAL_RTC_ReLaunchOrStopAlarm();
	}
}



void EXTI0_1_IRQHandler(void)
{
	//we need to clear line pending bit manually
	sys_set_wake_up_reason(SYS_WAKE_UP_MASTER_MASK);
	EXTI_ClearITPendingBit(EXTI_Line0);
	if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
	{
		sys_force_mcu_run(1);//dont authorize to sleep
	}	
	else
	{
		sys_force_mcu_run(0);//go back to previous state
	}
}

void EXTI2_3_IRQHandler(void)
{
	if(USE_SHELL_USART_NB&0x20)
	{
		//Check if EXTI_Line3 is asserted: USART RX
		if(EXTI_GetITStatus(EXTI_Line3) != RESET)
		{
			sys_set_wake_up_reason(SYS_WAKE_UP_UART_MASK);
			//we need to clear line pending bit manually
			EXTI_ClearITPendingBit(EXTI_Line3);
		}
	}
}

void EXTI4_15_IRQHandler(void)
{
	//Check if EXTI_Line13 is asserted: PinPointer
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		sys_set_wake_up_reason(SYS_WAKE_UP_PP_MASK);
		//we need to clear line pending bit manually
		EXTI_ClearITPendingBit(EXTI_Line13);
	}

	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		sys_set_wake_up_reason(SYS_WAKE_UP_SPI_MASK);
		//we need ASAP to clear line pending bit manually
		EXTI_ClearITPendingBit(EXTI_Line15);
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15))
		{
			DMA_Channel_TypeDef* DMA_Tx = DMA1_Channel3;
			DMA_Channel_TypeDef* DMA_Rx = DMA1_Channel2;
			SPI_InitTypeDef sSPI;	

			DMA_Cmd(DMA1_Channel3, DISABLE);
			DMA_Cmd(DMA1_Channel2, DISABLE);
			DMA_ClearFlag(DMA1_FLAG_TC3);
			DMA_ClearFlag(DMA1_FLAG_TC2);
			SPI_Cmd(SPI1, DISABLE);
			SPI_I2S_DeInit(SPI1);
			sSPI.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
			sSPI.SPI_Mode = SPI_Mode_Slave;
			sSPI.SPI_DataSize = SPI_DataSize_8b;
			sSPI.SPI_CPOL = SPI_CPOL_Low;
			sSPI.SPI_CPHA = SPI_CPHA_1Edge;
			sSPI.SPI_NSS = SPI_NSS_Hard;
			sSPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
			sSPI.SPI_FirstBit = SPI_FirstBit_MSB;
			sSPI.SPI_CRCPolynomial = 0;
			SPI_Init(SPI1, &sSPI);
			SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
			SPI_SSOutputCmd(SPI1,DISABLE);
			
			// decode the command 
			uwb_com_ext();
			DMA_Tx->CMAR = (unsigned long)EXTCOM_BUF_TxFrame;
			DMA_Tx->CNDTR = EXTCOM_BUF_TxLen;
			DMA_Rx->CMAR = (unsigned long)EXTCOM_BUF_RxFrame;
			DMA_Rx->CNDTR = EXTCOM_BUF_RxLen;
			DMA_Cmd(DMA1_Channel3, ENABLE);
			DMA_Cmd(DMA1_Channel2, ENABLE);
			SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
			SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
			SPI_Cmd(SPI1, ENABLE);
		}
	}

	if(USE_SHELL_USART_NB&0x10)
	{
		//Check if EXTI_Line7 is asserted: UART1
		if(EXTI_GetITStatus(EXTI_Line7) != RESET)
		{
			sys_set_wake_up_reason(SYS_WAKE_UP_UART_MASK);
			//we need to clear line pending bit manually
			EXTI_ClearITPendingBit(EXTI_Line7);
		}
	}
}
