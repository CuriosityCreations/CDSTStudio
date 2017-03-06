/**
 * @file SPI.c
 * @brief SPI.c
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

#include "stm32f0xx_spi.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_exti.h"
#include "stm32f0xx_syscfg.h"
#include "SPI.h"
#include "USART.h"
#include "Ext_Com.h"
#include "types.h"

#pragma GCC optimize("Os")

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void HAL_SPI_DeInit (void)
{
	SPI_I2S_DeInit(SPI1);
	SPI_I2S_DeInit(SPI2);
}

void HAL_SPI_Init (void)
{
	HAL_SPI1_Init();
	HAL_SPI1_Init_Slave();
	HAL_SPI2_Init();
	HAL_SPI2_Init_Slave();
}

void HAL_SPI1_Init (void)
{
}



/* COM with external master */
void HAL_SPI1_Init_Slave (void)
{
	SPI_InitTypeDef sSPI;	
	DMA_InitTypeDef sDMA;
	//EXTI structure to init EXT
	//NVIC structure to set up NVIC controller
	//NVIC_InitTypeDef NVIC_InitStructure;


	// init the gpio struct
	// enable the SPI clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	//configure the interrupts
	
	// init the spi struct
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
	SPI_Cmd(SPI1, ENABLE);

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);


	/* Init DMA structure */
	DMA_DeInit(DMA1_Channel3);
	DMA_DeInit(DMA1_Channel2);
	sDMA.DMA_PeripheralBaseAddr = (unsigned long)&(SPI1->DR);
	sDMA.DMA_MemoryBaseAddr = (unsigned long)EXTCOM_BUF_TxFrame;
	sDMA.DMA_BufferSize = EXTCOM_BUF_TxLen;
	sDMA.DMA_DIR = DMA_DIR_PeripheralDST;
	sDMA.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	sDMA.DMA_MemoryInc = DMA_MemoryInc_Enable;
	sDMA.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	sDMA.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	sDMA.DMA_Mode = DMA_Mode_Circular;
	sDMA.DMA_Priority = DMA_Priority_Medium ;
	sDMA.DMA_M2M = DMA_M2M_Disable;
	/* Init DMA Channel (MEM->SPI) */
	DMA_Init(DMA1_Channel3, &sDMA);

	/* Init DMA Channel (SPI->MEM) */
	sDMA.DMA_MemoryBaseAddr = (unsigned long)EXTCOM_BUF_RxFrame;
	sDMA.DMA_BufferSize = EXTCOM_BUF_RxLen;
	sDMA.DMA_DIR = DMA_DIR_PeripheralSRC;
	sDMA.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel2, &sDMA);

	// enable TX and RX DMA
	DMA_Cmd(DMA1_Channel3, ENABLE);
	DMA_Cmd(DMA1_Channel2, ENABLE);
}




/* PP SPI */
void HAL_SPI2_Init (void)
{
	SPI_InitTypeDef sSPI;
	DMA_InitTypeDef sDMA;
	//NVIC_InitTypeDef sNVIC;
	// enable the SPI clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_I2S_DeInit(SPI2);


	// init the spi struct
	sSPI.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	sSPI.SPI_Mode = SPI_Mode_Master;
	sSPI.SPI_DataSize = SPI_DataSize_8b;
	sSPI.SPI_CPOL = SPI_CPOL_Low;
	sSPI.SPI_CPHA = SPI_CPHA_1Edge;
	sSPI.SPI_NSS = SPI_NSS_Soft;
	sSPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	sSPI.SPI_FirstBit = SPI_FirstBit_MSB;
	sSPI.SPI_CRCPolynomial = 0;
	SPI_RxFIFOThresholdConfig(SPI2, SPI_RxFIFOThreshold_QF);
	SPI_Init(SPI2, &sSPI);

	SPI_Cmd(SPI2, ENABLE);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);


	/* Init DMA structure */
	sDMA.DMA_PeripheralBaseAddr = (unsigned long)&(SPI2->DR);
	sDMA.DMA_DIR = DMA_DIR_PeripheralDST;
	sDMA.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	sDMA.DMA_MemoryInc = DMA_MemoryInc_Enable;
	sDMA.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	sDMA.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	sDMA.DMA_Mode = DMA_Mode_Normal;
	sDMA.DMA_Priority = DMA_Priority_High;
	sDMA.DMA_M2M = DMA_M2M_Disable;

	/* Init DMA Channel (MEM->SPI) */
	DMA_Init(DMA1_Channel5, &sDMA);

	/* Init DMA Channel (SPI->MEM) */
	sDMA.DMA_Priority = DMA_Priority_VeryHigh;
	sDMA.DMA_DIR = DMA_DIR_PeripheralSRC;

	DMA_Init(DMA1_Channel4, &sDMA);
}


void HAL_SPI2_Init_Slave (void)
{
}



void HAL_SPI1_write_buffer(unsigned char *data_reg, unsigned short size_reg)
{
}


void HAL_SPI2_write_buffer(unsigned char *data_reg, unsigned short size_reg)
{
	DMA_Channel_TypeDef* DMA_Tx = DMA1_Channel5;


	GPIO_ResetBits(GPIOB, GPIO_Pin_9);	// set CS to 0

	/* tell DMA what to transfer */
	DMA_Tx->CMAR = (unsigned long)data_reg;
	DMA_Tx->CNDTR = size_reg;
	/* enable Tx DMA channel */
	DMA_Cmd(DMA_Tx, ENABLE);

	while(SPI_GetTransmissionFIFOStatus(SPI2)!= SPI_TransmissionFIFOStatus_Empty);
	while (!DMA_GetFlagStatus(DMA1_FLAG_TC5));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	GPIO_SetBits(GPIOB, GPIO_Pin_9);	// set CS to 1

	DMA_Cmd(DMA_Tx, DISABLE);

	while (SPI_GetReceptionFIFOStatus(SPI2)!= SPI_ReceptionFIFOStatus_Empty)
	{
		SPI_ReceiveData8(SPI2);
	}
}



void HAL_SPI1_read_buffer(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_in, unsigned short size_in)
{
	}



void HAL_SPI2_read_buffer(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_in, unsigned short size_in)
{
	DMA_Channel_TypeDef* DMA_Tx = DMA1_Channel5;
	DMA_Channel_TypeDef* DMA_Rx = DMA1_Channel4;


	GPIO_ResetBits(GPIOB, GPIO_Pin_9);	// set CS to 0

	/* tell DMA where to receive */
	DMA_Rx->CMAR = (unsigned long)data_in;
	DMA_Rx->CNDTR = size_in;
	/* enable Rx DMA channel and requests */
	DMA_Cmd(DMA_Rx, ENABLE);

	/* tell DMA what to transfer */
	DMA_Tx->CMAR = (unsigned long)data_reg;
	DMA_Tx->CNDTR = size_reg;
	/* enable Tx DMA channel */
	DMA_Cmd(DMA_Tx, ENABLE);

	while(SPI_GetTransmissionFIFOStatus(SPI2)!= SPI_TransmissionFIFOStatus_Empty);
	while (!DMA_GetFlagStatus(DMA1_FLAG_TC4));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	GPIO_SetBits(GPIOB, GPIO_Pin_9);	// set CS to 1

	DMA_Cmd(DMA_Tx, DISABLE);
	DMA_Cmd(DMA_Rx, DISABLE);
}




void HAL_SPI1_write_burst(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_in, unsigned short size_in)
{
}



void HAL_SPI2_write_burst(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_in, unsigned short size_in)
{
	DMA_Channel_TypeDef* DMA_Tx = DMA1_Channel5;


	GPIO_ResetBits(GPIOB, GPIO_Pin_9);	// set CS to 0

	/* tell DMA what to transfer */
	DMA_Tx->CMAR = (unsigned long)data_reg;
	DMA_Tx->CNDTR = size_reg;
	/* enable Tx DMA channel */
	DMA_Cmd(DMA_Tx, ENABLE);

	while(SPI_GetTransmissionFIFOStatus(SPI2)!= SPI_TransmissionFIFOStatus_Empty);
	while (!DMA_GetFlagStatus(DMA1_FLAG_TC5));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	DMA_Cmd(DMA_Tx, DISABLE);


	/* tell DMA what to transfer */
	DMA_Tx->CMAR = (unsigned long)data_in;
	DMA_Tx->CNDTR = size_in;
	/* enable Tx DMA channel */
	DMA_Cmd(DMA_Tx, ENABLE);

	while(SPI_GetTransmissionFIFOStatus(SPI2)!= SPI_TransmissionFIFOStatus_Empty);
	while (!DMA_GetFlagStatus(DMA1_FLAG_TC5));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	GPIO_SetBits(GPIOB, GPIO_Pin_9);	// set CS to 1

	DMA_Cmd(DMA_Tx, DISABLE);

	while (SPI_GetReceptionFIFOStatus(SPI2)!= SPI_ReceptionFIFOStatus_Empty)
	{
		SPI_ReceiveData8(SPI2);
	}
}





void HAL_SPI1_read_burst(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_out, unsigned short size_out)
{
}



void HAL_SPI2_read_burst(unsigned char *data_reg, unsigned short size_reg, unsigned char *data_out, unsigned short size_out)
{
	DMA_Channel_TypeDef* DMA_Tx = DMA1_Channel5;
	DMA_Channel_TypeDef* DMA_Rx = DMA1_Channel4;

	GPIO_ResetBits(GPIOB, GPIO_Pin_9);	// set CS to 0

	/* tell DMA where to receive */
	DMA_Rx->CMAR = (unsigned long)data_reg;
	DMA_Rx->CNDTR = size_reg;
	/* enable Rx DMA channel and requests */
	DMA_Cmd(DMA_Rx, ENABLE);


	/* tell DMA what to transfer */
	DMA_Tx->CMAR = (unsigned long)data_reg;
	DMA_Tx->CNDTR = size_reg;
	/* enable Tx DMA channel */
	DMA_Cmd(DMA_Tx, ENABLE);

	while(SPI_GetTransmissionFIFOStatus(SPI2)!= SPI_TransmissionFIFOStatus_Empty);
	while (!DMA_GetFlagStatus(DMA1_FLAG_TC4));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	DMA_Cmd(DMA_Tx, DISABLE);
	DMA_Cmd(DMA_Rx, DISABLE);


	/* tell DMA where to receive */
	DMA_Rx->CMAR = (unsigned long)data_out;
	DMA_Rx->CNDTR = size_out;
	/* enable Rx DMA channel and requests */
	DMA_Cmd(DMA_Rx, ENABLE);


	/* tell DMA what to transfer */
	DMA_Tx->CMAR = (unsigned long)data_out;
	DMA_Tx->CNDTR = size_out;
	/* enable Tx DMA channel */
	DMA_Cmd(DMA_Tx, ENABLE);

	while(SPI_GetTransmissionFIFOStatus(SPI2)!= SPI_TransmissionFIFOStatus_Empty);
	while (!DMA_GetFlagStatus(DMA1_FLAG_TC4));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	GPIO_SetBits(GPIOB, GPIO_Pin_9);	// set CS to 1

	DMA_Cmd(DMA_Tx, DISABLE);
	DMA_Cmd(DMA_Rx, DISABLE);
}




void HAL_SPI2_bridge(unsigned char *data_in, unsigned short size_in, unsigned char *data_out, unsigned short size_out)
{
	DMA_Channel_TypeDef* DMA_Tx = DMA1_Channel5;
	DMA_Channel_TypeDef* DMA_Rx = DMA1_Channel4;

	GPIO_ResetBits(GPIOB, GPIO_Pin_9);	// set CS to 0

	/* tell DMA where to receive */
	DMA_Rx->CMAR = (unsigned long)data_out;
	DMA_Rx->CNDTR = size_out;
	/* enable Rx DMA channel and requests */
	DMA_Cmd(DMA_Rx, ENABLE);


	/* tell DMA what to transfer */
	DMA_Tx->CMAR = (unsigned long)data_in;
	DMA_Tx->CNDTR = size_in;
	/* enable Tx DMA channel */
	DMA_Cmd(DMA_Tx, ENABLE);

	while(SPI_GetTransmissionFIFOStatus(SPI2)!= SPI_TransmissionFIFOStatus_Empty);
	while (!DMA_GetFlagStatus(DMA1_FLAG_TC4));
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));

	GPIO_SetBits(GPIOB, GPIO_Pin_9);	// set CS to 1

	DMA_Cmd(DMA_Tx, DISABLE);
	DMA_Cmd(DMA_Rx, DISABLE);
}
