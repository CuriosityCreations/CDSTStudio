/**
 * @file pinpointer_hw.c
 * @brief pinpointer_hw.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

//-----------------------------------------------------------------------------
// pinpointer_hw.c
//-----------------------------------------------------------------------------
//
// Program Description:
//
// This section set the analog configuration on PINPOINTER
//
// Target: STM32F072
// Tool chain: CooCox CoIDE 1.7.6 / GCC ARM 4.8
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <pinpointer_hw_api.h>
#include <types.h>
#include <stdio.h>
#include "stm32f0xx_gpio.h"
#include "osal_trace.h"
#include "HAL_config.h"
#include "SPI.h"
#include "dac.h"
#include "GPIO.h"

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------
#define BAR_READ 			0x00
#define BAR_WRITE 			0x80

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// API Functions
//-----------------------------------------------------------------------------

OSAL_void pp_hw_dbg(OSAL_u8 val)
{
  //DBG_IO = val;
}

/************************** Chipset Reset lines ********************************************/

OSAL_void pp_hw_rst(OSAL_u8 val)
{
	if (!val)
	{
		GPIO_ResetBits(GPIO_RESET_PP, GPIO_PIN_RESET_PP);	// set Reset to 0
	}
	else
	{
		GPIO_SetBits(GPIO_RESET_PP, GPIO_PIN_RESET_PP);	// set Reset to 1
	}
}

OSAL_void pp_hw_power(OSAL_bool_t val)
{

}

// No Power Amplifier for now
OSAL_void pp_hw_pa_enable(OSAL_bool_t val)
{

}

// No Power Amplifier for now
OSAL_bool_t pp_hw_is_pa_enable(OSAL_void)
{
	return OSAL_false;
}

/************************ Single register access ********************************************/

OSAL_s8 pp_hw_reg_read(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8* buf_data)
{
	OSAL_u8  TxBuff[3];
	OSAL_u8  RxBuff[3];

	TxBuff[0] = BAR_READ | bank;
	TxBuff[1] = reg;
	TxBuff[2] = 0x00;

	HAL_SPI2_read_buffer(TxBuff, 3, RxBuff, 3);

	buf_data[0] = RxBuff[2];

	return 0;
}

OSAL_s8 pp_hw_reg_write(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8 buf_data)
{
	OSAL_u8  TxBuff[3];

	TxBuff[0] = BAR_WRITE | bank;
	TxBuff[1] = reg;
	TxBuff[2] = buf_data;
	
	HAL_SPI2_write_buffer(TxBuff, 3);

	return 0;
}

/************************ double register access ********************************************/

OSAL_s8 pp_hw_reg16_read(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u16* buf_data)
{
	OSAL_u8  TxBuff[4];
	OSAL_u8  RxBuff[4];

	TxBuff[0] = BAR_READ | bank;
	TxBuff[1] = reg;
	TxBuff[2] = 0x00;
	TxBuff[3] = 0x00;

	HAL_SPI2_read_buffer(TxBuff, 4, RxBuff, 4);

	buf_data[0] = (RxBuff[2] | RxBuff[3]<<8);

	return 0;
}

OSAL_s8 pp_hw_reg16_write(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u16 buf_data)
{
	OSAL_u8  TxBuff[4];

	TxBuff[0] = BAR_WRITE | bank;
	TxBuff[1] = reg;
	TxBuff[2] = (buf_data & 0xFF);
	TxBuff[3] = (buf_data>>8);
	
	HAL_SPI2_write_buffer(TxBuff, 4);

	return 0;
}

/************************ 32bit register access ********************************************/

OSAL_s8 pp_hw_reg32_read(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u32* buf_data)
{
	OSAL_u8  TxBuff[6];
	OSAL_u8  RxBuff[6];

	TxBuff[0] = BAR_READ | bank;
	TxBuff[1] = reg;
	TxBuff[2] = 0x00;
	TxBuff[3] = 0x00;
	TxBuff[4] = 0x00;
	TxBuff[5] = 0x00;

	HAL_SPI2_read_buffer(TxBuff, 6, RxBuff, 6);

	buf_data[0] = ((OSAL_u32)RxBuff[2] | ((OSAL_u32)RxBuff[3]<<8) | ((OSAL_u32)RxBuff[4]<<16) | ((OSAL_u32)RxBuff[5]<<24));

	return 0;
}

OSAL_s8 pp_hw_reg32_write(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u32 buf_data)
{
	OSAL_u8  TxBuff[6];

	TxBuff[0] = BAR_WRITE | bank;
	TxBuff[1] = reg;
	TxBuff[2] = (buf_data & 0xFF);
	TxBuff[3] = ((buf_data>>8)& 0xFF);
	TxBuff[4] = ((buf_data>>16)& 0xFF);
	TxBuff[5] =	(buf_data>>24);
	
	HAL_SPI2_write_buffer(TxBuff, 6);

	return 0;
} 

/************************* Burst registers access **********************************************/

OSAL_s8 pp_hw_burst_read(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8* buf_data, OSAL_u16 len)
{
	OSAL_u8  TxBuff[2];

	TxBuff[0] = BAR_READ | bank;
	TxBuff[1] = reg;	

	HAL_SPI2_read_burst(TxBuff, 2, buf_data, len);

	return 0;
}

OSAL_s8 pp_hw_burst_write(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8 OSAL_READ_ONLY* buf_data, OSAL_u16 len)
{
	OSAL_u8  TxBuff[2];

	TxBuff[0] = BAR_WRITE | bank;
	TxBuff[1] = reg;
	
	HAL_SPI2_write_burst(TxBuff, 2, (OSAL_u8*)buf_data, len);

	return 0;
}

OSAL_s8 pp_hw_prod_burst_write(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8 OSAL_PROD_SECT* buf_data, OSAL_u16 len)
{
	OSAL_u8  TxBuff[2];

	TxBuff[0] = BAR_WRITE | bank;
	TxBuff[1] = reg;

	HAL_SPI2_write_burst(TxBuff, 2, (OSAL_u8*)buf_data, len);

	return 0;
}


#ifdef CONFIG_RTLS_UNITTESTS

/************************* Sequence registers access **********************************************/

OSAL_s8 pp_hw_seq_read(OSAL_void *ctx, struct setting tab[])
{
	int i=0;
	int tmp = 0;
	int status = 0;

	while(tab[i].Type != T_STOP)
	{
		switch(tab[i].Type)
		{
			case T_REG8 :
			case T_MASK :
				tmp = pp_hw_reg_read(ctx, tab[i].Bank, tab[i].Reg, &tab[i].Val.t8);
				break;
			case T_REG16 :
				tmp = pp_hw_reg16_read(ctx, tab[i].Bank, tab[i].Reg, &tab[i].Val.t16);
				break;
			case T_REG32 :
				tmp = pp_hw_reg32_read(ctx, tab[i].Bank, tab[i].Reg, &tab[i].Val.t32);
				break;
			case T_BURST :
				tmp = pp_hw_burst_read(ctx, tab[i].Bank, tab[i].Reg, tab[i].Val.tburst.ptr, tab[i].Val.tburst.len);
				break;
			case T_DELAY :
				udelay(tab[i].Val.tdelay);
				break;
 			default :
				OSAL_trace(TRACE_ERROR,"SEQ_READ: BAD SEQ TYPE %d",tab[i].Type);
				break;
		}
		i++;
		status = status + tmp;
	}
	return status;
}
#endif

OSAL_s8 pp_hw_seq_write(OSAL_void *ctx, OSAL_READ_ONLY struct setting tab[])
{
	int i=0;
	int status = 0;
	
	for(i = 0; tab[i].Type != T_STOP; i++)
		status += pp_hw_command_write(ctx, tab[i]);
	
	return status;
}

OSAL_s8 pp_hw_prod_seq_write(OSAL_void *ctx, struct setting OSAL_PROD_SECT * tab)
{
	int i = 0;
	int status = 0;

	for(i = 0; tab[i].Type != T_STOP; i++)
		status += pp_hw_command_write(ctx, tab[i]);

	return status;
}

OSAL_s8 pp_hw_command_write(OSAL_void *ctx, struct setting command)
{
	OSAL_u8 status = 0, j = 0;
	OSAL_u8 dummy = 0;
	
	switch(command.Type)
	{
		case T_REG8 :
			status = pp_hw_reg_write(ctx, command.Bank, command.Reg, command.Val.t8);
			break;
			
		case T_REG16 :
			status = pp_hw_reg16_write(ctx, command.Bank, command.Reg, command.Val.t16);
			break;
			
		case T_REG32 :
			status = pp_hw_reg32_write(ctx, command.Bank, command.Reg, command.Val.t32);
			break;
			
		case T_BURST :
			status = pp_hw_burst_write(ctx, command.Bank, command.Reg, command.Val.tburst.ptr, command.Val.tburst.len);
			break;
			
		case T_PROD_REG8 :
			status = pp_hw_reg_write(ctx, command.Bank, command.Reg, *(command.Val.tprod8));
			break;

		case T_PROD_REG16 :
			status = pp_hw_reg16_write(ctx, command.Bank, command.Reg, *(command.Val.tprod16));
			break;
		
		case T_PROD_BURST :
			status = pp_hw_prod_burst_write(ctx, command.Bank, command.Reg, command.Val.tprodburst.ptr, command.Val.tprodburst.len);
			break;
			
		case T_DELAY :
			udelay(command.Val.tdelay);
			break;
			
		case T_DAC_EN :
			if (command.Val.tdac != 0) // Change DAC value only if command.Val.tdac != 0
				dac_set_valueUV(*command.Val.tdac);
			
			dac_enable(command.Bank); // DAC status is stored in command.Bank, no relation with bank
			break;
			
		case T_MASK :
			status = pp_hw_reg_read(ctx, command.Bank, command.Reg, &dummy);
			
			for(j=0; j<8; j++)
			{
				if(((command.Val.tmask.bitmask>>j) & 0x01))// if the bit should be change
				{
					if(((command.Val.tmask.val>>j) & 0x01) == 1) //set bit
					{
						dummy |= (0x01<<j);
					}
					else if(((command.Val.tmask.val>>j) & 0x01) == 0) //reset bit
					{
						dummy &= ((0x01<<j)^0xFF);
					}
				}
			}
			status += pp_hw_reg_write(ctx, command.Bank, command.Reg, dummy);
			break;
			
		case T_RESET :
			pp_hw_rst(command.Val.t8);
			break;
			
		case T_PWR :
			pp_hw_power(command.Val.tpwr);
			break;
			
		case T_STOP :
			break;
			
		default :
			OSAL_trace(TRACE_ERROR,"SEQ_WRITE:BAD SEQ TYPE %d",command.Type);
			break;
	}
	
	return status;
}

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------
