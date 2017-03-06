/**
 * @file ShellCommand_radio.c
 * @brief ShellCommand_radio.c
 * @author  bespoon.com
 * @date 22/2/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#ifdef CONFIG_ACTIVATE_UART_SHELL
#pragma GCC optimize("Os")
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <coreId.h>
#include "pinpointer_regs.h"
#include "pinpointer_hw_api.h"
#include "osal_type.h"
#include "osal_time.h"
#include "osal_trace.h"
#include "osal_comp.h"
#include "osal_math.h"
#include <config_api.h>
#include <msq_config_api.h>
#include <power_mode_types.h>
#include "ProdSectionDef.h"
#include "release_note.h"
#include "USART.h"
#include "dac.h"
#include "RCC.h"
#include "SPI.h"
#include "TIMER.h"
#include "ADC.h"
#include "GPIO.h"
#include "Ext_Com.h"
#include "stm32f0xx_gpio.h"
#include "module_api.h"

#include "shell.h"
#include "ShellCommand_system.h"
#include "ShellCommand_radio.h"


signed char get_phy_fct(int argc, char** argv)
{
	radio_showCurPhyAndDac(1);
	send_host(argv[0], 0, 1, 0);
	return 0;
}

signed char set_phy_fct(int argc, char** argv)
{
	config_phy_t refphy;
	//set phy cfg
	if(argc<=13) return -2;
	OSAL_memset(&refphy,0,sizeof(config_phy_t));
	refphy.channel=atoi(argv[1]);
	refphy.sync_code=atoi(argv[2]);
	refphy.demod_code=atoi(argv[3]);
	refphy.sync_rep=atoi(argv[4]);
	refphy.demo_rep=atoi(argv[5]);
	refphy.clock_track=atoi(argv[6]);
	refphy.framer_config=atoi(argv[7]);
	refphy.sync_threshold=atoi(argv[8]);
	refphy.rng_threshold=atoi(argv[9]);
	refphy.tx_gain=atoi(argv[10]);
	refphy.rx_gain=atoi(argv[11]);
	radio_get_pmstate((uwb_com_subcmd_e*) &refphy.power_mode);
	refphy.act=atoi(argv[12]);
	refphy.preamble_timeout=atoi(argv[13]);
	radio_set_phy_cfg((OSAL_u8*) &refphy,sizeof(config_phy_t));
	radio_showCurPhyAndDac(1);
	send_host(argv[0], 0, 1, 0);
	return 0;
}

OSAL_READ_ONLY char PowerStateString[POWER_MODE_NB][7] = {"PM3", "PM2", "PM0", "STDBY", "RX_", "TX_", "TXRX_I"};
signed char pm_fct(int argc, char** argv)
{
	int k;
	uwb_com_subcmd_e pm_val = UWB_SC_PM_2;
	if(argc<2) return -2;
	for(k = UWB_SC_PM_3; k <= UWB_SC_PM_TXRX_IDLE; k++)
	{
		if(!strncmp(argv[1], PowerStateString[k-UWB_SC_PM_3],OSAL_MIN(strlen(argv[1]),strlen(PowerStateString[k-UWB_SC_PM_3]))))
			pm_val = (uwb_com_subcmd_e)k;
	}
	if(pm_val <= UWB_SC_PM_TXRX_IDLE)
	{
		radio_set_pmstate(pm_val);
		print("New PM:%s",PowerStateString[pm_val-UWB_SC_PM_3]);
		send_host(argv[0], 0, 1, (signed long)pm_val);
	}
	else
	{
		return -1;
	}
	return 0;
}

#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
signed char tx_cont_fct(int argc, char** argv)
{
	char ch=2;
	static unsigned char Preamble_length;
	config_phy_t  *curr_config_phy;
	config_phy_t new_config_phy;
	if(argc<2) return -2;
	curr_config_phy = get_current_config_phy(ctx.cfg_ctx);
	if(!curr_config_phy) return -1;

	ch=(char) atoi(argv[1]);
	if(ch==0)
	{
		printline("Stop");
		pp_hw_reg_write(0, BAR_CFG_DIGITAL, BAR_PHY_CTRL_CMD, 0x00);
		pp_hw_reg_write(0, BAR_CFG_DIGITAL, BAR_PREAMBLE_LGTH, Preamble_length);
		test_reinit();
		send_host(argv[0], 0, 1, 0);
		return 0;
	}
	switch(ch)
	{
		case 1:ch=2;break;
		case 11:case 12:case 13:case 14:
		case 18:case 19:case 110:case 111:
			ch=(char) atoi(&argv[1][1]);
			break;
		default:return -2;
	}
	OSAL_memcpy(&new_config_phy,curr_config_phy,sizeof(config_phy_t));
	//initialize new config phy
	new_config_phy.channel =  ch;
	radio_set_phy_cfg((OSAL_u8*) &new_config_phy,sizeof(config_phy_t));
	printline("Start(ch%d)",ch);
	radio_set_pmstate(UWB_SC_PM_TX_IDLE);
	pp_hw_reg_read(0, BAR_CFG_DIGITAL, BAR_PREAMBLE_LGTH, &Preamble_length);
	pp_hw_reg_write(0, BAR_CFG_DIGITAL, BAR_PREAMBLE_LGTH, 0);
	pp_hw_reg_write(0, BAR_CFG_DIGITAL, BAR_PHY_CTRL_CMD, 0x01);
	send_host(argv[0], 0, 1, 0);
	return 0;
}

signed char cw_sweep_fct(int argc, char** argv)
{
	int reg_val;
	int min_reg_val = 0;
	int max_reg_val = 0;
	int reg_step = 0;
	int inter_step_delay = 0;
	if(argc<4) return -2;
	// no check,no error print to save RAM
	min_reg_val =  atoi(argv[1]);
	max_reg_val =  atoi(argv[2]);
	reg_step = atoi(argv[3]);
	inter_step_delay =  atoi(argv[4]);

	radio_set_pmstate(UWB_SC_PM_TX_IDLE);

	pp_hw_reg_write(0, BAR_CFG_ANALOG, BAR_TX_ENABLE, 0x09);
	pp_hw_reg_write(0, BAR_CFG_ANALOG, BAR_TX_SELECT, 0x0A);

	reg_val = min_reg_val;

	//HW team prefers the test to never stop
	while(1)
	{
		pp_hw_reg_write(0, BAR_CFG_ANALOG, BAR_TX_SP_VTUNE, reg_val);
		reg_val += reg_step;

		if(reg_val > max_reg_val)
			reg_val = min_reg_val;

		msleep(inter_step_delay);
	}
    return 0;
}
#endif //#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
#endif //#ifdef CONFIG_ACTIVATE_UART_SHELL


