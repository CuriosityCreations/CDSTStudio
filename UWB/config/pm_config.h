/**
 * @file pm_config.h
 * @author gpa@bespoon.com
 * @date 16/05/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef PM_CONFIG_H
#define PM_CONFIG_H

#include <types.h>
#include <pinpointer_regs.h>
#include <power_mode_types.h>
#include <msq_pm_seq.h>

struct pm_switch
{
	const char Name[16];
	pm_state_t Src;
	pm_state_t Dest;
	OSAL_PROD_SECT struct setting* Setting;
	OSAL_PROD_SECT struct setting* Setting_MSQ;
};


/**************************************************************************************************/
/****************************** POWER MANAGEMENT TAB SWITCH BLOCK ON ******************************/
/**************************************************************************************************/

/*************** COMMON PM SEQUENCES ***************/
OSAL_PROD_SECT struct setting PM3toPM2Setting[] = {
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TM_W0,			0,{.t8 = 0xFE}}, //enable IRQ (DIO0)
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_EN_W0,			0,{.t8 = 0xFE}},

		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_POWER_SUPPLY,		0,{.t8 = 0x0F}}, 	// enable 32K
		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_TUNE_OSCQ32K_IBOOST,	0,{.t8 = 0x01}}, // 32K boost for low 1v2 RTC voltage
		
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x40}},
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_DIVA,			0,{.t8 = 0x01}}, // configure DIVA for initialization

		#ifdef HYBRID_TOA
		{T_REG8,	BAR_CFG_DIGITAL,	BAR_LEAD_EDGE_SFW_PRP,			0,{.t8 = 0x02}},
		{T_REG8,	BAR_CFG_DIGITAL,	BAR_LEAD_EDGE_SFW_EP_IWP,		0,{.t8 = 0x20}},
		{T_REG8,	BAR_CFG_DIGITAL,	BAR_LEAD_EDGE_SBW_PRP,			0,{.t8 = 0x82}},
		{T_REG8,	BAR_CFG_DIGITAL,	BAR_LEAD_EDGE_SBW_EP_IWP,		0,{.t8 = 0x20}},
		#endif

#ifndef CONFIG_ASSET_TRACKING // Asset tracking use ext 32k
		{T_DELAY, 	0x00,			0x00,				0,{.tdelay = 80000}},  //delay for 32K regulator start/stable + XTAL ON
#endif
		
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}} 	
};
OSAL_PROD_SECT struct setting PM3toPM2Setting_MSQ[] = {
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}} 	
};
OSAL_PROD_SECT struct pm_switch PM3toPM2 = 
{
	"PM3toPM2",		//.Name
	PM_3,			//.Src		
	PM_2,			//.Dest
	PM3toPM2Setting,
	PM3toPM2Setting_MSQ,
};


OSAL_PROD_SECT struct setting PM2toPM0Setting[] = {
		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_TUNE_VDD1V2_RTC,  	0,{.t8 = 0x00}}, // regu 1v2 tuning
		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_TUNE_OSCQ32K_IBOOST,	0,{.t8 = 0x00}}, // 32K boost for low 1v2 RTC voltage
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x80}}, // Enable 2v5 with DIO19

		// FLA TODO: check vs conso if some regulators are worth to be powered up in other modes
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_POWER_SUPPLY,		0,{.t8 = 0x00}}, 
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_POWER_BANDGAP,		0,{.t8 = 0x05}}, // New BG2V5FS ON, new BG1V2FS ON, BG2V5 not shared
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_POWER_SUPPLY,		0,{.t8 = 0x00}}, 
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_POWER_BANDGAP,	0,{.t8 = 0x01}}, // New BG1V2A ON
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_POWER_SUPPLY,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_POWER_BANDGAP,		0,{.t8 = 0x01}}, // New BG2V5A ON, BG2V5 not shared
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDFE_POWER_SUPPLY,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDFE_POWER_BANDGAP,	0,{.t8 = 0x01}}, // New BG2V5D ON, BG2V5 not shared
		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_POWER_SUPPLY,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_POWER_SUPPLY,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_POWER_BANDGAP,		0,{.t8 = 0x01}}, // New BG1V2TX ON
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_BB_ISOLATION,		0,{.t8 = 0x00}},
		{T_DAC_EN,	0x01, /* Enable DAC */	0x00, /* Unused */		0,{.tdac = 0}}, 	// Enable DAC without modifying its value

		/* PM1 -> PM0 */
		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_ENABLE,		0,{.t8 = 0x03}},
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x60}}, //enable 26MHz
		{T_DELAY,	0x00,			0x00,				0,{.tdelay = 10}},

#ifdef CONFIG_RF_BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_ENABLE,			0,{.t8 = 0x09}},
#endif

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_SEL_DIO_TA_2,		0,{.t8 = 0x2F}}, //set DIO2 phy_STATE
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TEN_W0,			0,{.t8 = 0xF1}}, //configure DIO1 & DIO3 for RFSW_Ctrl, DIO2 for PHY_state FLA TODO: might be done somewhere else

		{T_REG16,	BAR_CFG_SYSTEM,		BAR_INTR_VECT2_MSK_W0,		0,{.t16 = ENABLED_IRQ_VECT2}}, // In case of PM written by host

		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct setting PM2toPM0Setting_MSQ[] = {	
		{T_DAC_EN,	0x01, /* Enable DAC */	0x00, /* Unused */		0,{.tdac = 0}}, 	// Enable DAC without modifying its value
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x80}}, // Enable 2v5 with DIO19
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_PM2_PM0}},
		{T_REG16,	BAR_CFG_SYSTEM,		BAR_INTR_VECT2_MSK_W0,		0,{.t16 = ENABLED_IRQ_VECT2}},

		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct pm_switch PM2toPM0 =
{	
	"PM2toPM0",		//.Name
	PM_2,			//.Src
	PM_0,			//.Dest
	PM2toPM0Setting,
	PM2toPM0Setting_MSQ,
};


OSAL_PROD_SECT struct setting PM0toSTDBYSetting[] = {
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_ENABLE_GENREF,		0,{.t8 = 0x06}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDLL_ENABLE,		0,{.t8 = 0x03}}, //FLA : bit 0 is "Enable 125ps-DLL RX or Tx mode" / 1: Enable; 0: disable, need for clktrk
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDLL_ENABLE_GENREF,	0,{.t8 = 0x01}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDLL_SP_TXRXDEP,		0,{.t8 = 0x03}}, 	//FLA: DLL activation split in TX_STBY & RX_STBY not possible
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDFE_ENABLE_W0,		0,{.t8 = 0x6F}},
		{T_DELAY, 	0x00, 			0x00,				0,{.tdelay = 40}},

		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_RESET,			0,{.t8 = 0x03}},
		{T_DELAY, 	0x00, 			0x00,				0,{.tdelay = 40}},

		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDLL_RESET,		0,{.t8 = 0x00}},
		{T_DELAY, 	0x00, 			0x00,				0,{.tdelay = 40}},

		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDFE_RESET,		0,{.t8 = 0x03}},
		{T_DELAY, 	0x00, 			0x00,				0,{.tdelay = 40}},

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x68}}, //enable 62.4MHz

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_PPVX_ASIC_NRST,		0,{.t8 = 0x0F}},
		{T_DELAY, 	0x00, 			0x00,				0,{.tdelay = 10}},
		
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_CLR,			0,{.t8 = 0xFF}},
		{T_DELAY, 	0x00, 			0x00,				0,{.tdelay = 10}},
		
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_CLR,			0,{.t8 = 0x00}},
		{T_DELAY,	0x00, 			0x00,				0,{.tdelay = 10}},

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_EN,			0,{.t8 = 0x20}}, // Enable Phy controller

		{T_REG16,	BAR_CFG_SYSTEM,		BAR_INTR_VECT0_MSK_W0,		0,{.t16 = ENABLED_IRQ_VECT0}},	// In case of PM written by host

		{T_STOP,	0x00, 			0x00,				0,{.t8 = 0x00}}	
};
OSAL_PROD_SECT struct setting PM0toSTDBYSetting_MSQ[] = {									
		{T_REG8,	BAR_CFG_DIGITAL,	BAR_CTRL_CFG,			0,{.t8 = 0x00}},		// Workaround: RST may enable RX_ACTIVE, thus msq will copy CTRL_CFG value from host
		{T_DELAY,	0x00,			0x00,				0,{.tdelay = 2000}}, 		// Wait end of PM0, this is due to synchronization problem between msq and host
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_PM0_STDBY}},
		{T_DELAY,	0x00, 			0x00,				0,{.tdelay = 500}},
		{T_REG16,	BAR_CFG_SYSTEM,		BAR_INTR_VECT0_MSK_W0,		0,{.t16 = ENABLED_IRQ_VECT0}},
		{T_REG8,	BAR_CFG_DIGITAL,	BAR_CTRL_CFG,			0,{.t8 = 0x1B}},		// End of workaround, from now msq can safely copy CTRL_CFG from host

		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct pm_switch PM0toSTDBY =
{
	"PM0toSTDBY",		//.Name	
	PM_0,			//.Src
	PM_STDBY,		//.Dest
	PM0toSTDBYSetting,
	PM0toSTDBYSetting_MSQ
};


/*************** TX PM SEQUENCES ***************/
OSAL_PROD_SECT struct setting STDBYtoTXIDLESetting[] = {
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x82}}, 	//0x82: external RFSW_Ctl path1 (DIO1 = 1, DIO3 = 0)
#ifdef CONFIG_RF_BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_SELECT_BGDEP_TXRXDEP,	0,{.t8 = 0x8A}}, 	//BG1
#else
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_SELECT_BGDEP_TXRXDEP,	0,{.t8 = 0x45}}, 	//BG0
#endif
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_SP_TXRXDEP,		0,{.t8 = 0x02}},  //Tx1 path, FLA to check: not necessary, shall be already set in STDBY
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_RESET,			0,{.t8 = 0x07}},	
		{T_DELAY,	0x00,			0x00,				0,{.tdelay = 40}},

		/* TXSTDBY -> TXIDLE */
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_ENABLE_GENREF,		0,{.t8 = 0x01}},	
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_EN,			0,{.t8 = 0x21}},	//Force Tx module

		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct setting STDBYtoTXIDLESetting_MSQ[] = {	
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_STDBY_TXIDLE}},
		
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct pm_switch STDBYtoTXIDLE =
{
	"STDBYtoTXIDLE",	//.Name	
	PM_STDBY,		//.Src
	PM_TX_IDLE,		//.Dest
	STDBYtoTXIDLESetting,
	STDBYtoTXIDLESetting_MSQ
};


/*************** RX PM SEQUENCES ***************/
OSAL_PROD_SECT struct setting STDBYtoRXIDLESetting[] = {
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_ENABLE_GENREF,		0,{.t8 = 0x07}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_TXRXDEP,		0,{.t8 = 0x7F}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_ENABLE,		0,{.t8 = 0x07}},	//enable integrator & 4-bit ADC		
	
		// Rx1 / Rx2 config	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_SP_TXRXDEP,		0,{.t8 = 0x04}},	//Ant dep: 0x01: Rx1, 0x04: Rx2
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x88}},	// 0x82: external RFSW_Ctl path1; 0x88: external RFSW_Ctl path2; 
		{T_DELAY, 	0x00,			0x00,				0,{.tdelay = 10}},// watchout for path2, need enough time for LNA to be stable before Rx (VMMK-3803: 5us OK)
	
#ifdef CONFIG_RF_BG1	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_BGDEP,		0,{.t8 = 0x06}}, 	//BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_SP_BGDEP,		0,{.t8 = 0x22}}, 	//BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_SP_BGDEP,		0,{.t8 = 0x02}}, 	//BG1
#else
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_BGDEP,		0,{.t8 = 0x01}}, 	//BG0
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_SP_BGDEP,		0,{.t8 = 0x11}}, 	//BG0
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_SP_BGDEP,		0,{.t8 = 0x01}}, 	//BG0
#endif

		/* RXSTDBY -> RXIDLE */
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x6C}},	//enable 62.4MHz + 125Mhz
		{T_DELAY, 	0x00, 			0x00,				0,{.tdelay = 10}},
#ifdef CONFIG_RF_BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_BGDEP_PD,	0,{.t8 = 0x01}},	//BG1
#else
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_BGDEP_PD,	0,{.t8 = 0x02}},	//BG0
#endif
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_GENREF,		0,{.t8 = 0x01}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_ENABLE_GENREF,	0,{.t8 = 0x01}},	
		
		// CEA patch to save 1mA	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_VREF_MIXER2,	0,{.t8 = 0x56}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_INTEGTRANS,	0,{.t8 = 0x46}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_INTEGOPA,	0,{.t8 = 0x32}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_VREF_INTEG,	0,{.t8 = 0x96}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_IBUFREF,	0,{.t8 = 0x32}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_ADCOPA,	0,{.t8 = 0x4C}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_ADC,	0,{.t8 = 0x4C}},

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_EN,			0,{.t8 = 0x2E}},	//Force Rx module

		{T_STOP, 	0x00, 			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct setting STDBYtoRXIDLESetting_MSQ[] = {	
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_STDBY_RXIDLE}},
		
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct pm_switch STDBYtoRXIDLE =
{
	"STDBYtoRXIDLE",	//.Name	
	PM_STDBY,		//.Src
	PM_RX_IDLE,		//.Dest
	STDBYtoRXIDLESetting,
	STDBYtoRXIDLESetting_MSQ
};


/*************** TX-RX PM SEQUENCE (for test purpose) ***************/
OSAL_PROD_SECT struct setting STDBYtoTXRXIDLESetting[] = {
		//FLA: Enable Rx
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_ENABLE_GENREF,		0,{.t8 = 0x07}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_TXRXDEP,		0,{.t8 = 0x7F}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_ENABLE,		0,{.t8 = 0x07}}, //enable integrator & 4-bit ADC	

#ifdef CONFIG_RF_BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_BGDEP,		0,{.t8 = 0x06}}, //BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_SP_BGDEP,		0,{.t8 = 0x22}}, //BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_SP_BGDEP,		0,{.t8 = 0x02}}, //BG1
#else
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_BGDEP,		0,{.t8 = 0x01}}, //BG0
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_SP_BGDEP,		0,{.t8 = 0x11}}, //BG0
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_SP_BGDEP,		0,{.t8 = 0x01}}, //BG0
#endif

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x6C}},
		{T_DELAY, 	0x00,			0x00,				0,{.tdelay = 10}},

#ifdef CONFIG_RF_BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_BGDEP_PD,	0,{.t8 = 0x01}}, //BG1
#else
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_BGDEP_PD,	0,{.t8 = 0x02}}, //BG0
#endif

		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_GENREF,		0,{.t8 = 0x01}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_ENABLE_GENREF,	0,{.t8 = 0x01}},

		//FLA: Enable Tx
#ifdef CONFIG_RF_BG1
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_SELECT_BGDEP_TXRXDEP,	0,{.t8 = 0x8A}}, //BG1
#else
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_SELECT_BGDEP_TXRXDEP,	0,{.t8 = 0x45}}, //BG0
#endif
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_RESET,			0,{.t8 = 0x07}},
		{T_DELAY, 	0x00,			0x00,				0,{.tdelay = 40}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_ENABLE_GENREF,		0,{.t8 = 0x01}},

		// Tx/Rx1-Rx2 internal switches config
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_SP_TXRXDEP,		0,{.t8 = 0x04}}, //Ant dep: 0x01: Rx1, 0x04: Rx2
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x82}}, // 0x82: external RFSW_Ctl path1 (implies LNA OFF); 0x88: external RFSW_Ctl path2 (implies LNA ON)
		
		// CEA patch to save 1mA	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_VREF_MIXER2,	0,{.t8 = 0x56}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_INTEGTRANS,	0,{.t8 = 0x46}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_INTEGOPA,	0,{.t8 = 0x32}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_VREF_INTEG,	0,{.t8 = 0x96}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_IBUFREF,	0,{.t8 = 0x32}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_ADCOPA,	0,{.t8 = 0x4C}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_ADC,	0,{.t8 = 0x4C}},

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_EN,			0,{.t8 = 0x2F}}, //Force Tx & Rx module	
		
		{T_STOP, 	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct setting STDBYtoTXRXIDLESetting_MSQ[] = {								
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_STDBY_TXRXIDLE}},
				
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}} 	
};
OSAL_PROD_SECT struct pm_switch STDBYtoTXRXIDLE =
{
	"STDBYtoTXRXIDLE",	//.Name	
	PM_STDBY,		//.Src
	PM_TXRX_IDLE,		//.Dest
	STDBYtoTXRXIDLESetting,
	STDBYtoTXRXIDLESetting_MSQ
};

/***************************************************************************************************/
/****************************** POWER MANAGEMENT TAB SWITCH BLOCK OFF ******************************/
/***************************************************************************************************/

/*************** TX-RX PM SEQUENCE (for test purpose) ***************/
OSAL_PROD_SECT struct setting TXRXIDLEtoSTDBYSetting[] = {
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_EN,			0,{.t8 = 0x20}}, //disable Tx & Rx modules, keep phy controller

		//FLA: Disable Tx
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_ENABLE_GENREF,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_SELECT_BGDEP_TXRXDEP,	0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_RESET,			0,{.t8 = 0x06}},

		//FLA: Disable Rx
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_GENREF,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_ENABLE_GENREF,	0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_BGDEP_PD,	0,{.t8 = 0x03}},
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x80}}, //set RFSW_Ctl 0-0
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_TXRXDEP,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_ENABLE_GENREF,		0,{.t8 = 0x06}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_SP_TXRXDEP,		0,{.t8 = 0x02}}, // Tx1 path ; FLA to check
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_BGDEP,		0,{.t8 = 0x00}}, //buffer off
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x68}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_ENABLE,		0,{.t8 = 0x08}}, //disable integrator & 4-bit ADC		

		// CEA patch to save 1mA	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_VREF_MIXER2,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_INTEGTRANS,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_INTEGOPA,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_VREF_INTEG,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_IBUFREF,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_ADCOPA,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_ADC,	0,{.t8 = 0x00}},	

		{T_STOP, 	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct setting TXRXIDLEtoSTDBYSetting_MSQ[] = {									
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_TXRXIDLE_STDBY}},
		
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}} 	
};
OSAL_PROD_SECT struct pm_switch TXRXIDLEtoSTDBY =
{
	"TXRXIDLEtoSTDBY",	//.Name	
	PM_TXRX_IDLE,		//.Src
	PM_STDBY,		//.Dest
	TXRXIDLEtoSTDBYSetting,
	TXRXIDLEtoSTDBYSetting_MSQ
};


/*************** RX PM SEQUENCES ***************/
OSAL_PROD_SECT struct setting RXIDLEtoSTDBYSetting[] = {
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_EN,			0,{.t8 = 0x20}}, // Rx module OFF, keep phy-controller
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_GENREF,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_ENABLE_BGDEP_PD,	0,{.t8 = 0x03}},

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x68}},

		/* RXSTDBY -> STDBY */
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x80}}, //set RFSW_Ctl 0-0
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_SELECT_BGDEP_TXRXDEP,	0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_TXRXDEP,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_ENABLE_GENREF,		0,{.t8 = 0x06}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_SP_TXRXDEP,		0,{.t8 = 0x02}}, // Tx1 path (default value)
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_SP_BGDEP,		0,{.t8 = 0x00}}, //buffer off
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_ENABLE,		0,{.t8 = 0x08}}, //disable integrator & 4-bit ADC		

		// CEA patch to save 1mA	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_VREF_MIXER2,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_INTEGTRANS,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_INTEGOPA,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_VREF_INTEG,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_IBUFREF,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_ADCOPA,	0,{.t8 = 0x00}},	
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_TUNE_IPOL_ADC,	0,{.t8 = 0x00}},

		{T_STOP, 	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct setting RXIDLEtoSTDBYSetting_MSQ[] = {	
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_RXIDLE_STDBY}},
		
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct pm_switch RXIDLEtoSTDBY =
{
	"RXIDLEtoSTDBY",	//.Name	
	PM_RX_IDLE,		//.Src
	PM_STDBY,		//.Dest
	RXIDLEtoSTDBYSetting,
	RXIDLEtoSTDBYSetting_MSQ
};


/*************** TX PM SEQUENCES ***************/
OSAL_PROD_SECT struct setting TXIDLEtoSTDBYSetting[] = {
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_EN,			0,{.t8 = 0x20}}, //disable Tx module, keep phy-controller
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_ENABLE_GENREF,		0,{.t8 = 0x00}},

		/* TXSTDBY -> STDBY */
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_SELECT_BGDEP_TXRXDEP,	0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_RESET,			0,{.t8 = 0x06}},
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x80}}, //set RFSW_Ctl 0-0

		{T_STOP, 	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct setting TXIDLEtoSTDBYSetting_MSQ[] = {	
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_TXIDLE_STDBY}},
		
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}} 	
};
OSAL_PROD_SECT struct pm_switch TXIDLEtoSTDBY =
{
	"TXIDLEtoSTDBY",	//.Name	
	PM_TX_IDLE,		//.Src
	PM_STDBY,		//.Dest
	TXIDLEtoSTDBYSetting,
	TXIDLEtoSTDBYSetting_MSQ
};


OSAL_PROD_SECT struct setting STDBYtoPM0Setting[] = {
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x60}},

		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_RESET,			0,{.t8 = 0x02}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDLL_RESET,		0,{.t8 = 0x01}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDFE_RESET,		0,{.t8 = 0x02}},
		
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDFE_ENABLE_W0,		0,{.t8 = 0x40}}, // enable en_adcbuclock_n
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDLL_ENABLE_GENREF,	0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDLL_ENABLE,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDLL_SP_TXRXDEP,		0,{.t8 = 0x00}},
	  	{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_ENABLE_GENREF,		0,{.t8 = 0x00}},

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_FORCE_EN,			0,{.t8 = 0x00}}, // cca fla: clear phy controller
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_PPVX_ASIC_NRST,		0,{.t8 = 0x07}}, //FLA to check -> DBB reset for PMs < stdby ?

		{T_REG16,	BAR_CFG_SYSTEM,		BAR_INTR_VECT0_MSK_W0,		0,{.t16 = ENABLED_IRQ_VECT0_PM2}}, // In case of PM written by host

		{T_STOP, 	0x00,			0x00,				0,{.t8 = 0x00}}	
};
OSAL_PROD_SECT struct setting STDBYtoPM0Setting_MSQ[] = {	
		{T_DAC_EN,	0x00, /* Disable DAC */	0x00, /* Unused */		0,{.tdac = 0}},	// Disable DAC without modifying its value
		{T_REG16,	BAR_CFG_SYSTEM,		BAR_INTR_VECT0_MSK_W0,		0,{.t16 = ENABLED_IRQ_VECT0_PM2}}, // We need to mask vect0 as PM2 can trig unexpected things (sfd/preamble TO)
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_STDBY_PM0}},
		
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}} 	
};
OSAL_PROD_SECT struct pm_switch STDBYtoPM0 =
{
	"STDBYtoPM0",		//.Name	
	PM_STDBY,		//.Src
	PM_0,			//.Dest
	STDBYtoPM0Setting,
	STDBYtoPM0Setting_MSQ
};


OSAL_PROD_SECT struct setting PM0toPM2Setting[] = {
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x40}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_ENABLE,		0,{.t8 = 0x01}},

		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_POWER_SUPPLY,		0,{.t8 = 0x03}}, 	// 1V8 OFF
		{T_DAC_EN,	0x00, /* Disable DAC */	0x00, /* Unused */		0,{.tdac = 0}},	// Disable DAC without modifying its value

		/* PM1 -> PM2 */
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_BB_ISOLATION,		0,{.t8 = 0x01}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_POWER_BANDGAP,		0,{.t8 = 0x03}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_TX_POWER_SUPPLY,		0,{.t8 = 0x03}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_POWER_SUPPLY,		0,{.t8 = 0x0F}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDFE_POWER_BANDGAP,	0,{.t8 = 0x03}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXDFE_POWER_SUPPLY,		0,{.t8 = 0x03}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_POWER_BANDGAP,	0,{.t8 = 0x03}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXABB_POWER_SUPPLY,		0,{.t8 = 0x0F}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_POWER_BANDGAP,		0,{.t8 = 0x03}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_RXFE_POWER_SUPPLY,		0,{.t8 = 0x0F}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_POWER_BANDGAP,		0,{.t8 = 0x0F}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_FS_POWER_SUPPLY,		0,{.t8 = 0xFF}},

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x00}}, // disable 2v5 with DIO19

		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_TUNE_OSCQ32K_IBOOST,	0,{.t8 = 0x01}}, // 32K boost for low 1v2 RTC voltage
		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_TUNE_VDD1V2_RTC,	0,{.t8 = 0x03}}, // regu 1v2 tuning to reduce power consumption; FLA: 0x02 limit for timer-sequencer T>50C

		{T_REG16,	BAR_CFG_SYSTEM,		BAR_INTR_VECT2_MSK_W0,		0,{.t16 = ENABLED_IRQ_VECT2_PM2}}, // In case of PM written by host
	
		{T_STOP, 	0x00,			0x00,				0,{.t8 = 0x00}}	
};
OSAL_PROD_SECT struct setting PM0toPM2Setting_MSQ[] = {
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TA_VAL,			0,{.t8 = 0x00}}, // disable 2v5 with DIO19
		{T_REG16,	BAR_CFG_MSQ,		BAR_HOST_TRIG_W0,		0,{.t16 =  SEQ_PM0_PM2}},
		{T_REG16,	BAR_CFG_SYSTEM,		BAR_INTR_VECT2_MSK_W0,		0,{.t16 = ENABLED_IRQ_VECT2_PM2}},
		
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}} 	
};
OSAL_PROD_SECT struct pm_switch PM0toPM2 =
{
	"PM0toPM2",		//.Name	
	PM_0,			//.Src
	PM_2,			//.Dest
	PM0toPM2Setting,
	PM0toPM2Setting_MSQ
};


OSAL_PROD_SECT struct setting PM2toPM3Setting[] = {
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_TM_W0,			0,{.t8 = 0xFF}}, //disable IRQ (DIO0)
		{T_REG8,	BAR_CFG_SYSTEM,		BAR_DIO_EN_W0,			0,{.t8 = 0xFF}},

		{T_REG8,	BAR_CFG_SYSTEM,		BAR_CLK_SRC_EN_W0,		0,{.t8 = 0x00}},
		{T_REG8,	BAR_CFG_ANALOG,		BAR_MISC_POWER_SUPPLY,		0,{.t8 = 0x1F}}, // disable 32K

		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}}
};
OSAL_PROD_SECT struct setting PM2toPM3Setting_MSQ[] = {									
		{T_STOP,	0x00,			0x00,				0,{.t8 = 0x00}} 	
};
OSAL_PROD_SECT struct pm_switch PM2toPM3 =
{
	"PM2toPM3",		//.Name	
	PM_2,			//.Src
	PM_3,			//.Dest
	PM2toPM3Setting,
	PM2toPM3Setting_MSQ
};


/*************************************************************************************************************************
 ** This Array indicate the sequence to apply to move in the PM tree from a state to another.				**
 ** You choose the direction you want to follow and it gives you the sequence to apply. 				**
 ** You have to do it until you're not in the wanted mode, the move work step by step.				   	**
 *************************************************************************************************************************/ 
static const struct pm_switch OSAL_PROD_SECT* PowerModeSwitch[POWER_MODE_NB][POWER_MODE_NB] = {
/* WANTED_MODE		PM3			PM2			PM0			STDBY			RX_IDLE			TX_IDLE			TXRX_IDLE					*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* CURRENT MODE	*/
/*PM3		*/  {	NULL,			&PM3toPM2,		&PM3toPM2,		&PM3toPM2,		&PM3toPM2,		&PM3toPM2,		&PM3toPM2		},	/* PM3		*/
/*PM2		*/  {	&PM2toPM3,		NULL,			&PM2toPM0,		&PM2toPM0,		&PM2toPM0,		&PM2toPM0,		&PM2toPM0		},	/* PM2		*/
/*PM0		*/  {	&PM0toPM2,		&PM0toPM2,		NULL,			&PM0toSTDBY,		&PM0toSTDBY,		&PM0toSTDBY,		&PM0toSTDBY		},	/* PM0		*/
/*STDBY		*/  {	&STDBYtoPM0,		&STDBYtoPM0,		&STDBYtoPM0,		NULL,			&STDBYtoRXIDLE,		&STDBYtoTXIDLE,		&STDBYtoTXRXIDLE	},	/* STDBY	*/
/*RX_IDLE	*/  {	&RXIDLEtoSTDBY,		&RXIDLEtoSTDBY,		&RXIDLEtoSTDBY,		&RXIDLEtoSTDBY,		NULL,	 		&RXIDLEtoSTDBY,		&RXIDLEtoSTDBY		},	/* RX_IDLE	*/
/*TX_IDLE	*/  {	&TXIDLEtoSTDBY,		&TXIDLEtoSTDBY,		&TXIDLEtoSTDBY,		&TXIDLEtoSTDBY,		&TXIDLEtoSTDBY,		NULL,			&TXIDLEtoSTDBY		},	/* TX_IDLE	*/
/*TXRX_IDLE	*/  {	&TXRXIDLEtoSTDBY,	&TXRXIDLEtoSTDBY,	&TXRXIDLEtoSTDBY,	&TXRXIDLEtoSTDBY,	&TXRXIDLEtoSTDBY,	&TXRXIDLEtoSTDBY,	NULL		}	};	/* TXRX_IDLE	*/


#ifdef NOT_DEF

KEEP ME ALIGNED PLEASE !!!!

****************************MUX CONFIG MSQ ACCES****************************
CFG_ANALOG:
	BAR_FS_ENABLE_GENREF		//B0.6 B0.7 B1.0
	BAR_FS_POWER_SUPPLY		//B1.1 B1.2 B1.3 B1.4 B1.5 B1.6 B1.7 B2.0
	BAR_FS_POWER_BANDGAP		//B2.1 B2.2 B2.3 B2.4	
	BAR_FS_RESET 			//B4.5 B4.6
	BAR_FS_SP_TXRXDEP		//B4.7 B5.0 B5.1 B5.2 B5.3 B5.4 B5.5	
	BAR_FS_SP_BGDEP			//B5.6 B5.7 B6.0
	BAR_RXFE_ENABLE_BGDEP_PD	//B8.3 B8.4
	BAR_RXFE_ENABLE_GENREF		//B8.5	
	BAR_RXFE_POWER_SUPPLY 		//B8.6 B8.7 B9.0 B9.1
	BAR_RXFE_POWER_BANDGAP 		//B9.2 B9.3
	BAR_RXFE_SP_TXRXDEP		//B10.0 B10.1 B10.2 B10.3
	BAR_RXFE_SP_BGDEP		//B10.4 B10.5 B10.6 B10.7 B11.0 B11.1
	BAR_RXDLL_ENABLE 		//B12.2 B12.3 B12.4
	BAR_RXDLL_ENABLE_GENREF 	//B12.5
	BAR_RXDLL_RESET 		//B12.6
	BAR_RXDLL_SP_TXRXDEP		//B12.7 B13.0
	BAR_RXABB_ENABLE 		//B13.4 B13.5 B13.6 B13.7 B14.0	
	BAR_RXABB_ENABLE_GENREF		//B14.1
	BAR_RXABB_POWER_SUPPLY 		//B14.2 B14.3 B14.4 B14.5
	BAR_RXABB_POWER_BANDGAP 	//B14.6 B14.7
	BAR_RXABB_SP_BGDEP		//B15.0 B15.1	
	BAR_RXABB_TUNE_VREF_MIXER2	//B15.4
	BAR_RXABB_TUNE_IPOL_INTEGTRANS	//B15.5
	BAR_RXABB_TUNE_IPOL_INTEGOPA	//B15.6
	BAR_RXABB_TUNE_VREF_INTEG	//B15.7
	BAR_RXABB_TUNE_IPOL_IBUFREF	//B16.0
	BAR_RXABB_TUNE_IPOL_ADCOPA	//B16.1
	BAR_RXABB_TUNE_IPOL_ADC		//B16.2
	BAR_RXDFE_ENABLE_W0		//B16.7 B17.0 B17.1 B17.2 B17.3 B17.4 B17.5 B17.6
	BAR_RXDFE_POWER_SUPPLY		//B18.0 B18.1
	BAR_RXDFE_POWER_BANDGAP 	//B18.2 B18.3
	BAR_RXDFE_RESET 		//B18.7 B19.0
	BAR_MISC_ENABLE 		//B19.3 B19.4 B19.5
	BAR_MISC_POWER_SUPPLY		//B19.6 B19.7 B20.0 B20.1 B20.2
	BAR_MISC_TUNE_VDD1V2_RTC	//B21.2
	BAR_MISC_TUNE_OSCQ32K_IBOOST	//B21.6
	BAR_TX_ENABLE_GENREF		//B22.4
	BAR_TX_POWER_SUPPLY 		//B22.5 B22.6
	BAR_TX_POWER_BANDGAP 		//B22.7 B23.0
	BAR_TX_SELECT_BGDEP_TXRXDEP	//B23.1 B23.2 B23.3 B23.4 B23.5 B23.6 B23.7 B24.0
	BAR_TX_RESET			//B25.0 B25.1 B25.2

CFG_SYSTEM:			
	BAR_FORCE_EN					//B38.2
	BAR_FORCE_CLR					//B38.3
	BAR_CLK_SRC_EN_W0, BAR_CLK_SRC_EN_W1		//B41.0	not in PM2
	BAR_CLK_DIVA					//B41.3
	BAR_DIO_TM_W0, BAR_DIO_TM_W1,	BAR_DIO_TM_W2	//B41.5
	BAR_DIO_EN_W0, BAR_DIO_EN_W1, BAR_DIO_EN_W2	//B41.6
	BAR_DIO_TEN_W0, BAR_DIO_TEN_W1, BAR_DIO_TEN_W2	//B41.7
	BAR_SEL_DIO_TA_2 				//B42.4
	BAR_DIO_TA_VAL					//B43.2
	BAR_PPVX_ASIC_NRST				//B43.6
	BAR_BB_ISOLATION 				//B44.2

	/*00*//*01*//*02*//*03*//*04*//*05*//*06*//*07*//*08*//*09*/
/*00*/	0xC0, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0x00, 0x00, 0xF8, 0x0F,
/*10*/	0xFF, 0x03, 0xFC, 0xF1, 0xFF, 0xF3, 0x87, 0x7F, 0x8F, 0xF9,  
/*20*/	0x07, 0x44, 0xF0, 0xFF, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 
/*30*/	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 
/*40*/	0x00, 0xE9, 0x10, 0x44, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
/*50*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
/*60*/	0x00, 0x00, 0x00, 0x00,

#endif

#endif //PM_CONFIG_H
