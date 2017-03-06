/**
 * @file ShellCommand_radio.h
 * @brief ShellCommand_radio.h
 * @author  bespoon.com
 * @date 22/2/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#ifndef _SHELL_COMMAND_RADIO_H_
#define _SHELL_COMMAND_RADIO_H_

//-----------------------------------------------------------------------------
// This part of shell fucntion
//-----------------------------------------------------------------------------

signed char get_phy_fct(int argc, char** argv);
signed char set_phy_fct(int argc, char** argv);
signed char pm_fct(int argc, char** argv);
signed char tx_cont_fct(int argc, char** argv);
signed char cw_sweep_fct(int argc, char** argv);

#ifdef GLOBEXT
#undef GLOBEXT
#endif

#ifdef _SHELL_COMMAND_C_
#define GLOBEXT const
#else
#define GLOBEXT extern 
#endif
				//each of your help line cannot be bigger then this
				// "**********************************************************************"
				//which is 70 char length, including \r,\n or \0
				//aka SHELL_HELP_MAX_LINE_STRLEN 70
				//you can put ITSA_GET_CMD => it will show GET <funcname> in help
				//you can put ITSA_DO_CMD=> it will show DO <funcname> in help
				//you can put ITSA_TEST_CMD=> it will show TEST <funcname> in help
GLOBEXT ui_cmd_t cmd_array_radio[]
#ifdef _SHELL_COMMAND_C_
 = {
	{"get_phy",	&get_phy_fct,
				ITSA_GET_CMD
	},
	{"set_phy",	&set_phy_fct,
				#ifndef SHELL_SHOW_HELP
				ITSA_DO_CMD
				#else
				"(ARG)C SC DC SR DR Ct FC ST RT Tg Rg Ac Pt\n"
				"C: ch,SC/SR:Scode/Srep\n"
				"DC/DR:Dcode/Drep\n"
				"Ct:clk track,FC:Framer,ST/RT:Sthrehsold/Rthreshold\n"
				"Tg/Rg:tx/rxgain, AC: act, Pt: preamble tout"
				#endif //#ifndef SHELL_SHOW_HELP
	},
	{"pm",		&pm_fct,
				#ifndef SHELL_SHOW_HELP
				ITSA_DO_CMD
				#else
				"(ARG):\n"
				"PM3\tPM2\tPM0\n"
				"STDBY\tRX_I\tTX_I\n"
				"TXRX_I"
				#endif //#ifndef SHELL_SHOW_HELP
	}
 }
#endif //_SHELL_COMMAND_C_
;

#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
GLOBEXT ui_cmd_t cmd_array_radio_full[]
#ifdef _SHELL_COMMAND_C_
 = {
	{"tx_cont",	&tx_cont_fct,
				#ifndef ADVANCEDSHELL_SHOW_HELP
				ITSA_DO_CMD
				#else
				"ON(1)/OFF(0)"
				#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"cw_sweep",&cw_sweep_fct,
				#ifndef ADVANCEDSHELL_SHOW_HELP
				ITSA_DO_CMD
				#else
				"(ARG) MinReg MaxReg StepReg Delay"
				#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	}
 }
#endif //_SHELL_COMMAND_C_
;
#endif //#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL

#endif // _SHELL_COMMAND_RADIO_H_

