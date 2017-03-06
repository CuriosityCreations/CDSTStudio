/**
 * @file RCC.c
 * @brief RCC.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#include "HAL_config.h"

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_adc.h"
#include "stm32f0xx_dac.h"
#include "stm32f0xx_pwr.h"
#include "stm32f0xx_gpio.h"
#include "ProdSectionDef.h"
#include "RCC.h"
#include "ADC.h"
#pragma GCC optimize("Os")

static void HAL_Enable_PLL (unsigned char Clock)
{
	if (Clock)	// PLL
	{
		/* Enable HSI */
		RCC_HSICmd(ENABLE);

		/* Wait till HSI is ready */
		while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

		RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_6);

		/* Enable PLL */
		RCC_PLLCmd(ENABLE);

		/* Wait till PLL is ready */
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Wait till PLL is used as system clock source */
		while (RCC_GetSYSCLKSource() != 0x08);
	}
	else	// USB Clock
	{
		/* Enable HSI48 oscillator */
		RCC_HSI48Cmd(ENABLE);

		/* Wait till HSI48RDYF is set */
		while(RCC_GetFlagStatus(RCC_FLAG_HSI48RDY) == RESET)
		{
		}

		/* Select HSI48 as system clock */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI48);

		/* Wait till PLL is used as system clock source */
		while (RCC_GetSYSCLKSource() != 0x0C);
	}
}

void HAL_RCC_LowClockInit()
{
	PWR_DeInit();
	
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	RCC_BackupResetCmd( ENABLE );
	RCC_BackupResetCmd( DISABLE );

	if((prodSection.manufacturerSection.hardwareCapabilities & CAN_ASSET_TRACKING_CAPS_MASK) == 0)
	{
		// LSI used as RTC source clock
		// The RTC Clock may varies due to LSI frequency dispersion.
		// Enable the LSI OSC
		RCC_LSICmd(ENABLE);
		// Wait till LSI is ready
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
		// Select the RTC Clock Source
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	}
	else
	{
		// LSE used as RTC source clock
		// Configure crystal drive strength to low (to lower consuption)
		RCC_LSEDriveConfig(RCC_LSEDrive_Low);
		// Enable LSE OSC
		RCC_LSEConfig(RCC_LSE_ON);
		// Wait till LSI is ready
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		// Select the RTC Clock Source
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	}

	// Enable RTC clock
	RCC_RTCCLKCmd(ENABLE);
}

void HAL_RCC_Init (unsigned char State)
{
	// enable the syscfg clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, State);
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, State);
	// Enable CRC clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, State);

	// Enable HSI48 OSC
	HAL_Enable_PLL(0);
}

void HAL_MCU_Sleep_Mode (unsigned char wfiCond)
{
	// Disable ADC
	HAL_ADC_DeInit();

	// Stop all peripherals clocks
	HAL_RCC_Init(DISABLE);

	// Enter sleep mode
	PWR_EnterSleepMode((wfiCond)?PWR_SLEEPEntry_WFI:PWR_SLEEPEntry_WFE);
}

void HAL_MCU_Stop_Mode (unsigned char wfiCond)
{
	// Stop all peripherals clocks
	HAL_RCC_Init(DISABLE);

	// Enter stop mode
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, (wfiCond)?PWR_STOPEntry_WFI:PWR_STOPEntry_WFE);
}

void HAL_MCU_Standby_Mode (void)
{
	// Stop all peripherals clocks
	HAL_RCC_Init(DISABLE);

	// Enter standby mode
	PWR_EnterSTANDBYMode();
}

void HAL_Out_of_LowPower_Mode (void)
{
	//we can sleep only if:
	//the time before sleep is authorizedf has elapsed
	PWR_ClearFlag(PWR_FLAG_WU);
	// enable all the peripherals clocks on wake up
	HAL_RCC_Init(ENABLE);
}
