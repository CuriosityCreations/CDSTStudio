/**
 * @file I2C.c
 * @brief I2C.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#include "HAL_config.h"

#include "stm32f0xx_dma.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_i2c.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_syscfg.h"
#include "I2C.h"
#pragma GCC optimize("Os")
//-------------------------------------------
// Definition of local constants
//-------------------------------------------
#define I2C_TIMEOUT  (0x1000)

//-------------------------------------------
// Declaration of local functions
//-------------------------------------------
#if 0
static void I2C_DMA_Config(unsigned char Direction, unsigned char* buffer, unsigned char NumData);
#endif

void HAL_I2C_DeInit(void)
{
	#ifdef USE_I2C
	I2C_DeInit(Open_I2C);
	#endif
}

/*******************************************************************************
* Function Name  : I2C_BusInit
* Description    : Configure I2C-Bus
* Input          : None
* Output         : None
* Return         : None
* Attention	     : None
*******************************************************************************/
void HAL_I2C_Init(void)
{
	#ifdef USE_I2C
	I2C_InitTypeDef  I2C_InitStructure;

	//NVIC_InitTypeDef NVIC_InitStructure;
	//=========================================================================================
	// Enable GPIO clock
	//-----------------------------------------------------------------------------------------
	RCC_AHBPeriphClockCmd(Open_I2C_SCL_GPIO_CLK, ENABLE);

	//=========================================================================================
	// Enable I2C clock
	//-----------------------------------------------------------------------------------------
	RCC_APB1PeriphClockCmd(Open_I2C_CLK, ENABLE);

	// enable DMA clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	//=========================================================================================
	// Configure I2C Controller
	//-----------------------------------------------------------------------------------------
	I2C_InitStructure.I2C_Timing = I2C_SCL_SPEED;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Disable;
	I2C_InitStructure.I2C_DigitalFilter = 0;
	I2C_InitStructure.I2C_OwnAddress1 = I2C_OWN_ADDR;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(Open_I2C, &I2C_InitStructure);
	//=========================================================================================
	// Enable I2C
	//-----------------------------------------------------------------------------------------
	I2C_10BitAddressingModeCmd(Open_I2C, DISABLE);

	I2C_Cmd(Open_I2C, ENABLE);

	//=========================================================================================
	// Configure the I2C interrupt priority
	//-----------------------------------------------------------------------------------------
	/*
	NVIC_InitStructure.NVIC_IRQChannel = Open_I2C_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	*/
	#endif //#ifdef USE_I2C
}

//===============================================================================
// I2C_ReadTransfer - initiate a read transfer on the I2C bus
//
// Initiates a read transfer on the I2C bus.
// If a register address is specified (<addressLength> != 0) a write without a stop
// condition will be initiated to the device to write the address, before the
// read is initiated.
// If no address if required (sequential read) <addressLength> is set to 0.
//-------------------------------------------------------------------------------
// u08 dev_addr  I2C device address
// u08 *buffer   pointer to the buffer to store the read data.
//               The buffer must be at least 'cnt' bytes long
// int cnt       number of bytes to read
// u32 ptr       register address, if required by the I2C-device
// u08 ptrlen    length of the register address to be written.
//               Valid values are 0..4
//-------------------------------------------------------------------------------
int HAL_I2C_ReadTransfer(unsigned char dev_addr, unsigned char *buffer, int cnt, unsigned long ptr, unsigned char ptrlen)
{
	int rc = NOK;
	#ifdef USE_I2C
	int i;
	int DMA_Timeout = I2C_TIMEOUT * 10;
	unsigned long DataNum = 0;
	//-----------------------------------------------------------------------------
	// parameter check
	//-----------------------------------------------------------------------------
	if ((buffer == 0) || (ptrlen > 4) || ((cnt | ptrlen) == 0))
	{
		//return I2C_check_dev(dev_addr); // may be used to check if device is responding
	}
	//-----------------------------------------------------------------------------
	// write the register address pointer to the device
	//-----------------------------------------------------------------------------
	  /* Configure DMA Peripheral */
	//I2C_DMA_Config(0, buffer, cnt);

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(Open_I2C, dev_addr, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

	/* Wait until TXIS flag is set */
	DMA_Timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus(Open_I2C, I2C_ISR_TXIS) == RESET)
	{
		if((DMA_Timeout--) == 0) return rc;
	}

	if (ptrlen > 0)
	{
		for (i=1; i<=ptrlen; i++)
		{
			I2C_SendData(Open_I2C,((ptr >>(8*(ptrlen-i))) & 0xff));

			/* Wait until TC flag is set */
			DMA_Timeout = I2C_TIMEOUT;
			while(I2C_GetFlagStatus(Open_I2C, I2C_ISR_TC) == RESET)
			{
				if((DMA_Timeout--) == 0) return rc;
			}
		}
	}
	//-----------------------------------------------------------------------------
	// read data from device
	//-----------------------------------------------------------------------------
	if (cnt > 0)
	{
		/* Configure slave address, nbytes, reload, end mode and start or stop generation */
		I2C_TransferHandling(Open_I2C, dev_addr, cnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

		/* Reset local variable */
		DataNum = 0;

		/* Wait until all data are received */
		while (DataNum != cnt)
		{
		  /* Wait until RXNE flag is set */
		  DMA_Timeout = I2C_TIMEOUT;
		  while(I2C_GetFlagStatus(Open_I2C, I2C_ISR_RXNE) == RESET)
		  {
			  if((DMA_Timeout--) == 0) return rc;
		  }

		  /* Read data from RXDR */
		  buffer[DataNum]= I2C_ReceiveData(Open_I2C);

		  /* Update number of received data */
		  DataNum++;
		}

		/* Enable I2C DMA request */
		//I2C_DMACmd(I2C1, I2C_DMAReq_Rx, ENABLE);

		/* Enable DMA RX Channel */
		//DMA_Cmd(DMA1_Channel7, ENABLE);

		/* Wait until DMA Transfer Complete */
		/*while (!DMA_GetFlagStatus(DMA1_FLAG_TC7))
		{
			if((DMA_Timeout--) == 0) rc = NOK;
		}*/
	}
	//-----------------------------------------------------------------------------
	/* Wait until STOPF flag is set */
	DMA_Timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus(Open_I2C, I2C_ISR_STOPF) == RESET)
	{
		if((DMA_Timeout--) == 0) return rc;
	}

	/* Clear STOPF flag */
	I2C_ClearFlag(Open_I2C, I2C_ICR_STOPCF);

	rc = OK;

	/* Disable DMA RX Channel */
	//DMA_Cmd(DMA1_Channel7, DISABLE);

	/* Disable I2C DMA request */
	//I2C_DMACmd(I2C1, I2C_DMAReq_Rx, DISABLE);

	/* Clear DMA RX Transfer Complete Flag */
	//DMA_ClearFlag(DMA1_FLAG_TC7);
	#endif //#ifdef USE_I2C
	return rc;
}


//===============================================================================
// I2C_WriteTransfer r- initiate a write transfer on the I2C bus
//
// Initiates a write transfer on the I2C bus.
// If a register address is supplied it is inserted between the I2C device address
// and the data.
//-------------------------------------------------------------------------------
// u08 dev_addr  I2C device address
// u08 *buffer   pointer to the buffer to store the read data.
//               The buffer must be at least 'cnt' bytes long
// int cnt       number of bytes to read
// u32 ptr       register address, if required by the I2C-device
// u08 ptrlen    length of the register address to be written.
//               Valid values are 0..4
//-------------------------------------------------------------------------------
int  HAL_I2C_WriteTransfer(unsigned char dev_addr, unsigned char *buffer, int cnt, unsigned long ptr, unsigned char ptrlen)
{
	int rc = NOK;
	#ifdef USE_I2C
	int i;	
	int DMA_Timeout = I2C_TIMEOUT * 10;
	unsigned long DataNum = 0;
	//-----------------------------------------------------------------------------
	// parameter check
	//-----------------------------------------------------------------------------
	if ((buffer == 0) || (ptrlen > 4) || ((cnt | ptrlen) == 0))
	{
		//return I2C_check_dev(dev_addr); // may be used to check if device is responding
	}
	//-----------------------------------------------------------------------------
	/* Configure DMA Peripheral */
	//I2C_DMA_Config(1, buffer, cnt);

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(Open_I2C, dev_addr, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

	/* Wait until TXIS flag is set */
	DMA_Timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus(Open_I2C, I2C_ISR_TXIS) == RESET)
	{
	  if((DMA_Timeout--) == 0) return rc;
	}

	//---------------------------------------------------------------------------
	// write the register address pointer to the device
	//---------------------------------------------------------------------------
	if (ptrlen > 0)
	{
	  for (i=1; i<=ptrlen; i++)
	  {
		I2C_SendData(Open_I2C,((ptr >>(8*(ptrlen-i))) & 0xff));

		/* Wait until TC flag is set */
		DMA_Timeout = I2C_TIMEOUT;
		while(I2C_GetFlagStatus(Open_I2C, I2C_ISR_TCR) == RESET)
		{
			if((DMA_Timeout--) == 0) return rc;
		}
	  }
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(Open_I2C, dev_addr, cnt, I2C_AutoEnd_Mode, I2C_No_StartStop);

	/* Wait until TXIS flag is set */
	DMA_Timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus(Open_I2C, I2C_ISR_TXIS) == RESET)
	{
	  if((DMA_Timeout--) == 0) return rc;
	}

	/* Reset local variable */
	DataNum = 0;

	/* Wait until all data are sent */
	while (DataNum != cnt)
	{
		/* Write data to TXDR */
		I2C_SendData(Open_I2C, buffer[DataNum]);

		/* Wait until STOPF flag is set */
		DMA_Timeout = I2C_TIMEOUT;
		while(I2C_GetFlagStatus(Open_I2C, I2C_ISR_STOPF) == RESET)
		{
			if((DMA_Timeout--) == 0) return rc;
		}

		DataNum++;
	}

	/* Clear STOPF flag */
	I2C_ClearFlag(Open_I2C, I2C_ICR_STOPCF);

	rc = OK;

	/* Enable I2C DMA request */
	//I2C_DMACmd(I2C1, I2C_DMAReq_Tx, ENABLE);

	/* Enable DMA TX Channel */
	//DMA_Cmd(DMA1_Channel6, ENABLE);

	/* Wait until DMA Transfer Complete */
	/*while (!DMA_GetFlagStatus(DMA1_FLAG_TC6))
	{
		if((DMA_Timeout--) == 0) rc = NOK;
	}*/

	//-----------------------------------------------------------------------------
	//I2C_stop(Open_I2C);                // stop the transmission

	/* Disable DMA RX Channel */
	//DMA_Cmd(DMA1_Channel6, DISABLE);

	/* Disable I2C DMA request */
	//I2C_DMACmd(I2C1, I2C_DMAReq_Tx, DISABLE);

	/* Clear DMA RX Transfer Complete Flag */
	//DMA_ClearFlag(DMA1_FLAG_TC6);
	#endif //#ifdef USE_I2C
	return rc;
}





void I2C1_EV_IRQHandler(void)
{
#ifdef USE_I2C
  //OSAL_trace(TRACE_DEBUG,"I2C1_EV_IRQHandler(): Event=0x%08X",event);
  //---------------------------------------------------
  // todo, if I2C1 ISR mode shall be used
  //---------------------------------------------------
#endif //#ifdef USE_I2C
}

void I2C2_EV_IRQHandler(void)
{
#ifdef USE_I2C

  //OSAL_trace(TRACE_DEBUG,"I2C2_EV_IRQHandler(): Event=0x%08X",event);
  //---------------------------------------------------
  // todo, if I2C2 ISR mode shall be used
  //---------------------------------------------------
#endif //#ifdef USE_I2C
}


#ifdef USE_I2C

static void I2C_DMA_Config(unsigned char Direction, unsigned char* buffer, unsigned char NumData)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	// remap the DMA channels
	SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_I2C1, ENABLE);	// Rx in channel 7, Tx in channel 6

	/* Initialize the DMA_PeripheralBaseAddr member */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned long)I2C1->RXDR;
	/* Initialize the DMA_MemoryBaseAddr member */
	DMA_InitStructure.DMA_MemoryBaseAddr = (unsigned long)buffer;
	/* Initialize the DMA_PeripheralInc member */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/* Initialize the DMA_MemoryInc member */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	/* Initialize the DMA_PeripheralDataSize member */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	/* Initialize the DMA_MemoryDataSize member */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	/* Initialize the DMA_Mode member */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	/* Initialize the DMA_Priority member */
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	/* Initialize the DMA_M2M member */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	/* If using DMA for Reception */
	if (Direction == 0)
	{
		/* Initialize the DMA_DIR member */
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

		/* Initialize the DMA_BufferSize member */
		DMA_InitStructure.DMA_BufferSize = NumData;

		DMA_DeInit(DMA1_Channel3);

		DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	}
	/* If using DMA for Transmission */
	else if (Direction == 1)
	{
		/* Initialize the DMA_DIR member */
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

		/* Initialize the DMA_BufferSize member */
		DMA_InitStructure.DMA_BufferSize = NumData;

		DMA_DeInit(DMA1_Channel2);

		DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	}
}

#endif //#ifdef USE_I2C
