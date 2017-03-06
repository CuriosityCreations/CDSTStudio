/**
 * @file init_config.h
 * @author jla@bespoon.com
 * @date 13/01/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef INIT_CONFIG_H
#define INIT_CONFIG_H
#include "types.h"
#include "ProdSectionDef.h"

OSAL_PROD_SECT struct setting init_setting_tab[] =
{
	{T_PWR, 		0x00, 			    0x00, 					    0,{.tpwr = OSAL_true}},

	/* RESET */
	{T_RESET, 		0x00, 			    0x00, 					    0,{.treset = OSAL_false}},
	{T_DELAY, 		0x00,			    0x00,					    0,{.tdelay = 100}},
	{T_RESET, 		0x00, 			    0x00,					    0,{.treset = OSAL_true}},

	/* Init MSQ */
	{T_REG8,		BAR_CFG_MSQ, 		BAR_RUN_MSQ, 			    0,{.t8 = 0x00}},
	{T_PROD_BURST,	BAR_CFG_MUX_REG,	BAR_MUX_REG_W00,		    0,{.tprodburst = { prodSection.burstSection.mux_reg, MUX_REG_SIZE }}},
	{T_PROD_BURST,	BAR_TRIG_TABL,		BAR_TRIG_TABL_W00,		    0,{.tprodburst = { prodSection.burstSection.trig_tabl, TRIG_TABLE_SIZE }}},

	/* Init DAC */
	{T_DAC_EN, 		0 /* disable DAC */, 0 /* unused */,		    0,{.tdac = &(prodSection.userSection.radioCfg.dacDefault)}},
	
	/* Init PP */
	{T_PROD_BURST, 	BAR_CFG_ANALOG, 	BAR_FS_ENABLE, 			    0,{.tprodburst = { prodSection.burstSection.cfg_analog, CONF_ANALOG_SIZE }}},
	
	{T_PROD_BURST,	BAR_CFG_DIGITAL,	BAR_PHY_CTRL_CMD,		    0,{.tprodburst = { prodSection.burstSection.cfg_digital, CONF_DIGIT_SIZE }}},
	{T_PROD_BURST,	BAR_CFG_SYSTEM,		BAR_INTR_VECT1_MSK_W0,	    0,{.tprodburst = { prodSection.burstSection.cfg_system, CONF_SYSTEM_SIZE }}},

	{T_STOP,		0x00,			    0x00,					    0,{.t8 = 0x00}}
};

OSAL_PROD_SECT struct setting exit_setting_tab[] =
{
	{T_RESET, 		0x00, 			    0x00, 					    0,{.treset = OSAL_false}},
	{T_PWR, 		0x00, 			    0x00, 					    0,{.tpwr = OSAL_false}},
	{T_STOP,		0x00,			    0x00,					    0,{.t8 = 0x00}}
};

#endif //INIT_CONFIG_H
