/**
 * @file ShellCommand_misc.h
 * @brief ShellCommand_misc.h
 * @author  bespoon.com
 * @date 22/2/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#ifndef _SHELL_COMMAND_MISC_H_
#define _SHELL_COMMAND_MISC_H_

void shellcommand_misc_Process();
void shellcommand_misc_reinit();
//-----------------------------------------------------------------------------
// This part of shell fucntion
//-----------------------------------------------------------------------------
signed char tx_fct(int argc, char** argv);
signed char rx_fct(int argc, char** argv);
void twoWayRngProcess();
signed char twoWayRng(int argc, char** argv);
signed char smac_test_fct(int argc, char** argv);
signed char toa_show_fct(int argc, char** argv);
signed char rngType_fct(int argc, char** argv);
#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
signed char bttm_fct(int argc, char** argv);
signed char prot_tune_fct(int argc, char** argv);
signed char rfloop_fct(int argc, char** argv);
signed char checkMove_fct(int argc, char** argv);
#endif //#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL

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
GLOBEXT ui_cmd_t cmd_array_misc[]
#ifdef _SHELL_COMMAND_C_
 = {
	{"tx",			&tx_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) size nb Delay(ms)\n"
					"Set DAC or phy before calling this"
					#endif //#ifndef SHELL_SHOW_HELP
	},
	{"rx",			&rx_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) size nb\n"
					"Set DAC or phy before calling this"
					#endif //#ifndef SHELL_SHOW_HELP
	},
#ifdef CONFIG_GENERIC
	{"2wayrng",		&twoWayRng,
					#ifndef SHELL_SHOW_HELP
					ITSA_TEST_CMD
					#else
					"Start/Stop 2 way ranging test\n"
					"(ARG) S R T P F I/No (ARG) to stop\n"
					"S,R= Sync,demod code/rep\n"
					"T=Sync and ranging threshold\n"
					"P preamble timeout in ms\n"
					"F,I Framerate/increment in ms (neareast 64TS)"
					#endif //#ifndef SHELL_SHOW_HELP
	},
#endif //CONFIG_GENERIC
#ifdef CONFIG_RTLS_PROTOCOL
	{"smac_test",	&smac_test_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) XY\n"
					"X=0(BZ),1(T),2(C),3(TWR),4(DOS),other:STOP\n"
					"Y=0(dflt),1(Fast),2(Slow),3(WR)\n"
					"Stops if no args given"
					#endif //#ifndef SHELL_SHOW_HELP
	},
	{"toa_show",	&toa_show_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"Show(1)/Hide(0)"
					#endif //#ifndef SHELL_SHOW_HELP
	},
#endif //#ifdef CONFIG_RTLS_PROTOCOL
}
#endif //_SHELL_COMMAND_C_
;

#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
GLOBEXT ui_cmd_t cmd_array_misc_full[]
#ifdef _SHELL_COMMAND_C_
 = {
#ifdef CONFIG_RTLS_PROTOCOL
	{"bttm",		&bttm_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"COORD Adress"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"prot_tune",	&prot_tune_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"SMAC tune\n"
					"(ARG):locr0(dflt)/1 capsmask"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"rngType",		&rngType_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"RNG type\n"
					"(ARG) CFG(hex) T(*)\n"
					"(*)1:B1,2:B2,3:RNG,4:RDV,0/None:all"
					#endif //#ifndef SHELL_SHOW_HELP
	},
#endif //CONFIG_RTLS_PROTOCOL
	{"rfloop",		&rfloop_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) Size Nb\n"
					"Set DAC or phy before calling this"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"checkMove",	&checkMove_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"Show(1)/Hide(0)"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
}
#endif //_SHELL_COMMAND_C_
;
#endif //#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
#endif // _SHELL_COMMAND_MISC_H_

