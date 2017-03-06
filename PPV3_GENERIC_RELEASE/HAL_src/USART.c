/**
 * @file USART.c
 * @brief USART.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Includes ------------------------------------------------------------------*/
#include "osal_trace.h"
#include "HAL_config.h"
#include "USART.h"

#include "stm32f0xx_usart.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_rcc.h"
#include <osal_time.h>
#include <osal_comp.h>
#include <osal_stdlib.h>
#include "ProdSectionDef.h"

#pragma GCC optimize("Os")
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

unsigned char USE_SHELL_USART_NB=0;//it can be 0xAB, or 0xB, with A=uart nb ( 2 or 1) where to receive, B=uart nb ( 2 or 1) where to send, A=B

void HAL_USART1_Init ()
{
	USART_InitTypeDef sUSART;
	USART_DeInit (USART1);

// enable the clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	if(prodSection.userSection.moduleCfg.shellUartBaudrate!=0)
		sUSART.USART_BaudRate = prodSection.userSection.moduleCfg.shellUartBaudrate;
	else
	/* Configuration : 57600 bauds, 8 N 1 as default*/
	sUSART.USART_BaudRate = 57600;

	sUSART.USART_WordLength = USART_WordLength_8b;
	sUSART.USART_StopBits = USART_StopBits_1;
	sUSART.USART_Parity = USART_Parity_No;
	sUSART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	sUSART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1,&sUSART);
	if (USART1_MODE == USART_SYNC_MODE_ENABLED)	// use as SPI Master
	{
		USART_ClockInitTypeDef sUSART_Sync;

		sUSART_Sync.USART_CPHA = USART_CPHA_1Edge;
		sUSART_Sync.USART_CPOL = USART_CPOL_Low;
		sUSART_Sync.USART_Clock = USART_Clock_Enable;
		sUSART_Sync.USART_LastBit = USART_LastBit_Disable;
		USART_ClockInit(USART1, &sUSART_Sync);
	}
	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
}

/* USART for the trace (printf) */
void HAL_USART2_Init ()
{
	USART_InitTypeDef sUSART;
	USART_DeInit (USART2);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	/* Configuration : 115200 bauds, 8 N 1 */
	//sUSART.USART_BaudRate = 115200;

	if(prodSection.userSection.moduleCfg.shellUartBaudrate!=0)
		sUSART.USART_BaudRate = prodSection.userSection.moduleCfg.shellUartBaudrate;
	else
		sUSART.USART_BaudRate = 57600;

	sUSART.USART_WordLength = USART_WordLength_8b;
	sUSART.USART_StopBits = USART_StopBits_1;
	sUSART.USART_Parity = USART_Parity_No;
	sUSART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	sUSART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2,&sUSART);


	if (USART2_MODE == USART_SYNC_MODE_ENABLED)	// use as SPI Master
	{
		USART_ClockInitTypeDef sUSART_Sync;

		sUSART_Sync.USART_CPHA = USART_CPHA_1Edge;
		sUSART_Sync.USART_CPOL = USART_CPOL_Low;
		sUSART_Sync.USART_Clock = USART_Clock_Enable;
		sUSART_Sync.USART_LastBit = USART_LastBit_Disable;
		USART_ClockInit(USART2, &sUSART_Sync);
	}

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);
}

/* return -1 if nothing happen, 0 if char on uart, with char val filled*/
int HAL_USART_GetChar(char* val)
{
	int ret = -1;

	if(USE_SHELL_USART_NB&0x1)
	{
		if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
		{
			*val = USART_ReceiveData(USART1);
			ret = 0;
		}

		if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
		{
			ret = -1;
		}

		if(USART1->ISR != 0)
		{
			USART_ClearFlag(USART1,USART1->ISR);
		}
	}
	else if(USE_SHELL_USART_NB&0x2)
	{
		if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
		{
			*val = USART_ReceiveData(USART2);
			ret = 0;
		}

		if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
		{
			ret = -1;
		}

		if(USART2->ISR != 0)
		{
			USART_ClearFlag(USART2,USART2->ISR);
		}
	}

	return ret;
}




/* USART Sync for the accelerometer */
void HAL_USART3_Init ()
{
}

void HAL_USART3_Sync_write_buffer(unsigned char *data_out, unsigned short size_out)
{
}

void HAL_USART3_Sync_read_buffer(unsigned char *data_tx, unsigned short size_tx, unsigned char *data_rx, unsigned short size_rx)
{
}

void HAL_USART4_Init ()
{
}

void HAL_USART_DeInit()
{
	USART_DeInit (USART1);
	USART_DeInit (USART2);
	USART_DeInit (USART3);
	USART_DeInit (USART4);
}

void HAL_USART_Init ()
{
	if(USE_SHELL_USART_NB&0x1)
		HAL_USART1_Init ();
	else if(USE_SHELL_USART_NB&0x2)
		HAL_USART2_Init ();
	HAL_USART3_Init ();
	HAL_USART4_Init ();
}

