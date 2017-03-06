/**
 * @file ShellCommand_system.h
 * @brief ShellCommand_system.h
 * @author  bespoon.com
 * @date 22/2/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#ifndef _SHELL_COMMAND_SYSTEM_H_
#define _SHELL_COMMAND_SYSTEM_H_

#define MAX_TEST_FRAME_SIZE 64 // keep it below SPI buffer 
#define TIMEOUT_MAX 180000

//reinit all global from all tests
void test_reinit();
//-----------------------------------------------------------------------------
// This part of shell fucntion
//-----------------------------------------------------------------------------
const char* testResultToString(int rslt);

signed char help_fct(int argc, char** argv);
signed char reset_fct(int argc, char** argv);
signed char mcu_id_fct(int argc, char** argv);
signed char version_fct(int argc, char** argv); 	// [ "version" error(dec) major(dec) minor(dec) CS(hex) ]
signed char serial_fct(int argc, char** argv); 	// [ "serial" error(dec) major(dec) minor(dec) CS(hex) ]
signed char macAddress_fct(int argc, char** argv);	// [ "mac" error(dec) word[0](hex) word[0](hex) .. word[15](hex) CS(hex) ]				
signed char dac_fct(int argc, char** argv);
signed char dac_en_fct(int argc, char** argv);
signed char soldering_fct(int argc, char** argv);	// [ "soldering" error(dec) status(dec) CS(hex) ]
signed char io_conf_fct(int argc, char** argv);
signed char temperature_fct(int argc, char** argv);
signed char battery_fct(int argc, char** argv);
signed char spiTrace_fct(int argc, char** argv);
signed char rfdesign_fct(int argc, char** argv);
signed char lse_en_fct(int argc, char** argv);
signed char lse_drv_cfg_fct(int argc, char** argv);
#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
signed char rtcpres_fct(int argc, char** argv);
signed char rtcTime_fct(int argc, char** argv);
signed char rtc_fct(int argc, char** argv);
signed char rtctune_fct(int argc, char** argv);
signed char rtcclock_fct(int argc, char** argv);
signed char periph_lp_fct(int argc, char** argv);
signed char io_ctrl_fct(int argc, char** argv);
signed char io_read_fct(int argc, char** argv);
signed char lta_fct(int argc, char** argv);
signed char msq_en_fct(int argc, char** argv);
signed char w_fct(int argc, char** argv);
signed char r_fct(int argc, char** argv);
signed char thld_fct(int argc, char** argv);
signed char read_thld_fct(int argc, char** argv);
signed char thld_test_fct(int argc, char** argv);
signed char noise_test_fct(int argc, char** argv);
signed char alloclevel_fct(int argc, char** argv);
signed char allocate_fct(int argc, char** argv);
#ifdef UWB_SPI_TEST_API
    #ifdef UWB_COM_CAN_ANSWER
    signed char api_fct(int argc, char** argv);
    #endif
    #ifdef UWB_COM_CAN_SEND
    signed char apiExt_fct(int argc, char** argv);
    #endif //#ifdef UWB_COM_CAN_SEND
#endif //UWB_SPI_TEST_API
signed char testBitfiled_fct(int argc, char** argv); 
#endif  //CONFIG_ACTIVATE_ADVANCEDUART_SHELL

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
GLOBEXT ui_cmd_t cmd_array_system[]
#ifdef _SHELL_COMMAND_C_
 = {
	{"help",		&help_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_GET_CMD
					#else
					"(ARG) -f full"
					#endif //#ifndef SHELL_SHOW_HELP
	},
	{"reset",		&reset_fct,
					ITSA_DO_CMD
	},
	{"mcu_id",		&mcu_id_fct,
					ITSA_GET_CMD
	},
	{"version",		&version_fct,
					ITSA_GET_CMD
	},
	{"serial",		&serial_fct,
					ITSA_GET_CMD
	},
	{"macAddress",	&macAddress_fct,
					ITSA_GET_CMD
	},
	{"dac",			&dac_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) val(uV) to set"
					#endif //#ifndef SHELL_SHOW_HELP
	},
	{"dac_en",		&dac_en_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"ON(1)/OFF(0)"
					#endif //#ifndef SHELL_SHOW_HELP
	},
	{"soldering",	&soldering_fct,
					ITSA_TEST_CMD
	},
	{"io_conf",		&io_conf_fct,
					ITSA_GET_CMD
	}, 
	{"temperature",	&temperature_fct,
					ITSA_GET_CMD
	},
	{"battery",		&battery_fct,
					ITSA_GET_CMD
	},
	{"spiTrace",	&spiTrace_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"ON(1)/OFF(0)"
					#endif //#ifndef SHELL_SHOW_HELP
	},
	{"rfdesign",	&rfdesign_fct,
					#ifndef SHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) nb. (100 if not given, or if superior)\n"
					"Meas nb noise on z5\n"
					"Fail if max/min ratio>2dB"
					#endif //#ifndef SHELL_SHOW_HELP
	}
}
#endif //_SHELL_COMMAND_C_
;

#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
GLOBEXT ui_cmd_t cmd_array_system_full[]
#ifdef _SHELL_COMMAND_C_
 = {
	{"rtcpres",		&rtcpres_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"RTC factor\n"
					"(ARG) P set precision"
					"Get value if no ARG"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"rtcTime",		&rtcTime_fct,
					ITSA_GET_CMD
	},
	{"rtc",			&rtc_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"Sleep for Nsec,Mmsec\n"
					"(ARG) N M"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"periph_lp",	&periph_lp_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"lp stop"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"rtctune",		&rtctune_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) N M\n"
					"RTC Fine Tune N.M pcent\n"
					"Get if no arg"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"rtcclock",	&rtcclock_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) loopnb STEP(us)."
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"io_ctrl",		&io_ctrl_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG):Group(A,B,C) IO(1..15) state(1/0)"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"io_read", 	&io_read_fct,
	  				#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG):Group IO type(in/out)"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"msq_en",		&msq_en_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) MSQ(1/0) pm(1/0)"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"lta",			&lta_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"ON(1)/OFF(0)"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"w",			&w_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) bank reg val"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"r",			&r_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) bank reg"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"thld", 		&thld_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) zone"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"read_thld",	&read_thld_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) zone"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},  
	{"thld_test",	&thld_test_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"For channel/zone/path\n"
					"[OPT]: c z p (RETURN): Thld(s) value(s)\n"
					"NOTE: if c, z or path are 0/not given\n"
					"it test all possibilities"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"noise_test",	&noise_test_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) ch zo freq len rfpath"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"alloclevel",	&alloclevel_fct,
					ITSA_TEST_CMD
	},
	{"allocate",	&allocate_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"(ARG) bytes"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
#ifdef UWB_SPI_TEST_API
	#ifdef UWB_COM_CAN_ANSWER
		{"api",		&api_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"API Command Simu\n"
					"(ARG)c=CMD s=SUB p=PRM a=A ol=XXXX E=<aa[1] ... aa[N]>\n"
					"All value in HEXA without '0x'\n"
					"PRM=16bits/8bits parameters)\n"
					"E=<aa[1] ... aa[N]> exdata, ',' separated values[N<=24 values max]\n"
					"A=y show output, ol=size size of output(decimal)"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
		},
	#endif //#ifdef UWB_COM_CAN_ANSWER
	#ifdef UWB_COM_CAN_SEND
		{"apiExt",	&apiExt_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"API CMD to another UM100 connected with SPI\n"
					"(ARG)c=CMD s=SUB p=PRM a=A ol=XXXX E=<aa[1] ... aa[N]>\n"
					"All value in HEXA without '0x'\n"
					"PRM=16bits/8bits parameters)\n"
					"E=<aa[1] ... aa[N]> exdata, ',' separated values[N<=24 values max]\n"
					"A=y show output, ol=size size of output(decimal)"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
		},
	#endif //#ifdef UWB_COM_CAN_SEND
#endif //UWB_SPI_TEST_API
	{"testBitfiled",&testBitfiled_fct,
					ITSA_GET_CMD
	},
	{"lse_en",		&lse_en_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"ON(1)/OFF(0)"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	},
	{"lse_drv_cfg",	&lse_drv_cfg_fct,
					#ifndef ADVANCEDSHELL_SHOW_HELP
					ITSA_DO_CMD
					#else
					"Set crystal osc strength (0/lower<=x<=3/higher)"
					#endif //#ifndef ADVANCEDSHELL_SHOW_HELP
	}
}
#endif //_SHELL_COMMAND_C_
;
#endif //#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL


#endif // _SHELL_COMMAND_SYSTEM_H_
