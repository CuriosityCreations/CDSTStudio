/**
 * @file  uwbmodule_com_common.c
 * @brief  module communication definition and tools implementation. Common part. Conform with spec Module_specification.pdf @rev1.4
 * @author ore@bespoon.com
 * @date 14/08/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */
#include "osal_type.h"
#include "osal_trace.h"
#include "osal_stdlib.h"
#include "uwbmodule_com.h"
#include "module_api_def.h"


typedef struct{
	uwb_com_scheme_e s;
	uwb_com_type_e   t;
}uwb_scheme_and_type;

//define the sheme and command type for each supported system command
static OSAL_READ_ONLY uwb_scheme_and_type sysCommandScheme[SYS_CMD_NB]={
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SYS_RSVD0
  { .s=UWB_CS_COMMAND_N_ANSWER, 	.t=UWB_CT_COMMAND_WITH_SUB_N_PRM},//SYS_GET_INFO
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SYS_RSVD2
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND_WITH_SUB	},//SYS_SET_MCU_STATE
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//SYS_GET_DAC_VAL
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//SYS_SET_DAC_VAL
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//SYS_GET_CAPATUNE_VAL
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND_WITH_8BITSPRM	},//SYS_SET_CAPATUNE_VAL
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//SYS_GET_RXFRAME_CFG
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND_WITH_8BITSPRM	},//SYS_SET_RXFRAME_CFG
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//SYS_GET_STATUS_MSK
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//SYS_SET_STATUS_MSK
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//SYS_GET_STATUS
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND_WITH_16BITSPRM	},//SYS_PROD_LIMITS::Reserved for production, valid only with some specific firmware
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND_WITH_SUB			},//SYS_PROD_CTRL::Reserved for production, valid only with some specific firmware
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND_WITH_SUB_N_PRM} //SYS_CHECKCACHE
};


//define the sheme and command type for each supported radio command
static OSAL_READ_ONLY uwb_scheme_and_type radioCommandScheme[RADIO_CMD_NB]={
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND		},//RADIO_INIT
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//RADIO_SPI_READ
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//RADIO_SPI_WRITE
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//RADIO_GET_PM_STATE
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND_WITH_SUB	},//RADIO_SET_PM_STATE
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//RADIO_GET_PHY_CFG
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//RADIO_SET_PHY_CFG
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//RADIO_GET_RX_CFG
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND		},//RADIO_SET_RX_CFG
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND		},//RADIO_CLR_RNG_TIMER
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//RADIO_WRITE_TX_DATA
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND_WITH_SUB	},//RADIO_READ_RX_DATA
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND_WITH_SUB_N_PRM},//RADIO_PHY_CTRL
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//RADIO_RSVDD
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//RADIO_RSVDE
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			} //RADIO_RSVDF
};

//define the sheme and command type for each supported sequencer command
static OSAL_READ_ONLY uwb_scheme_and_type seqCommandScheme[SEQ_CMD_NB]={
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND		},//SEQ_INIT
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//SEQ_GET_CFG
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND		},//SEQ_SET_CFG
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND		},//SEQ_QUEUE_SINGLE_ACT
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVD4
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVD5
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVD6
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVD7
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVD8
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVD9
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVDA
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVDB
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND_WITH_SUB	},//SEQ_CTRL
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVDD
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//SEQ_RSVDE
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			} //SEQ_RSVDF
};

#ifdef CONFIG_RTLS_PROTOCOL
//define the sheme and command type for each supported protocol command
static OSAL_READ_ONLY uwb_scheme_and_type protCommandScheme[PROT_CMD_NB]={
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND		},//PROT_INIT
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//PROT_GET_CAPS
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND		},//PROT_SET_CAPS
#ifdef CONFIG_RTLS_FULL_LOC
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//PROT_GET_SFRAME_SCH
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//PROT_SET_SFRAME_SCH
#else
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//PROT_GET_SFRAME_SCH == rsvd in full loc deactivated
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//PROT_SET_SFRAME_SCH == rsvd in full loc deactivated
#endif //#ifdef CONFIG_RTLS_FULL_LOC
#ifdef CONFIG_HAVE_STATS
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND		},//PROT_GET_STATS
  { .s=UWB_CS_COMMAND_N_ACK,		.t=UWB_CT_COMMAND		},//PROT_RESET_STATS
#else
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//PROT_GET_STATS  == rsvd without CONFIG_HAVE_STATS
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//PROT_RESET_STATS  == rsvd without CONFIG_HAVE_STATS
#endif //#ifdef CONFIG_HAVE_STATS
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND_WITH_SUB	},//PROT_GET_ARTLS_PKT
  { .s=UWB_CS_COMMAND_N_DATA,		.t=UWB_CT_COMMAND_WITH_16BITSPRM},//PROT_SET_ARTLS_PKT
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND_WITH_SUB	},//PROT_GET_RANGING_PKT
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//PROT_RSVDA
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//PROT_RSVDB
  { .s=UWB_CS_COMMAND_N_ANSWER,		.t=UWB_CT_COMMAND_WITH_SUB	},//PROT_CTRL
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//PROT_RSVDD
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			},//PROT_RSVDE
  { .s=UWB_CS_COMMAND_NONE,		.t=UWB_CT_NONE			} //PROT_RSVDF
};

#endif //#ifdef CONFIG_RTLS_PROTOCOL

#ifndef CONFIG_HIDE_COMCMD_INFO
//define the name for each supported system command. cast to avoid stupid warning
static OSAL_READ_ONLY OSAL_u8* sysCommandName[SYS_CMD_NB]={
	(OSAL_u8*) "0",
	(OSAL_u8*) "SGET_INFO",
	(OSAL_u8*) "2",
	(OSAL_u8*) "SSET_MCU_STATE",
	(OSAL_u8*) "SGET_DAC_VAL",
	(OSAL_u8*) "SSET_DAC_VAL",
	(OSAL_u8*) "SGET_CAPATUNE_VAL",
	(OSAL_u8*) "SSET_CAPATUNE_VAL",
	(OSAL_u8*) "SGET_RXFRAME_CFG",
	(OSAL_u8*) "SSET_RXFRAME_CFG",
	(OSAL_u8*) "SGET_STATUS_MSK",
	(OSAL_u8*) "SSET_STATUS_MSK",
	(OSAL_u8*) "SGET_STATUS",
	(OSAL_u8*) "LIMITS",
	(OSAL_u8*) "PROD",
	(OSAL_u8*) "SCHECKCACHE"
};
//define the name for each supported system command. cast to avoid stupid warning
static OSAL_READ_ONLY OSAL_u8* radioCommandName[RADIO_CMD_NB]={
	(OSAL_u8*) "RINIT",
	(OSAL_u8*) "RSPI_READ",
	(OSAL_u8*) "RSPI_WRITE",
	(OSAL_u8*) "RGET_PM_STATE",
	(OSAL_u8*) "RSET_PM_STATE",
	(OSAL_u8*) "RGET_PHY_CFG",
	(OSAL_u8*) "RSET_PHY_CFG",
	(OSAL_u8*) "RGET_RX_CFG",
	(OSAL_u8*) "RSET_RX_CFG",
	(OSAL_u8*) "RCLR_RNG_TIMER",
	(OSAL_u8*) "RWRITE_TX_DATA",
	(OSAL_u8*) "RREAD_RX_DATA",
	(OSAL_u8*) "RPHY_CTRL",
	(OSAL_u8*) "D",
	(OSAL_u8*) "E",
	(OSAL_u8*) "F"
};

//define the name for each supported sequencer command. cast to avoid stupid warning
static OSAL_READ_ONLY OSAL_u8* seqCommandName[SEQ_CMD_NB]={
	(OSAL_u8*) "SQINIT",
	(OSAL_u8*) "SQGET_CFG",
	(OSAL_u8*) "SQSET_CFG",
	(OSAL_u8*) "SQQUEUE_SINGLE_ACT",
	(OSAL_u8*) "4",
	(OSAL_u8*) "5",
	(OSAL_u8*) "6",
	(OSAL_u8*) "7",
	(OSAL_u8*) "8",
	(OSAL_u8*) "9",
	(OSAL_u8*) "A",
	(OSAL_u8*) "B",
	(OSAL_u8*) "SQCTRL",
	(OSAL_u8*) "D",
	(OSAL_u8*) "E",
	(OSAL_u8*) "F"

};

#ifdef CONFIG_RTLS_PROTOCOL
//define the name for each supported system command. cast to avoid stupid warning
static OSAL_READ_ONLY OSAL_u8* protCommandName[PROT_CMD_NB]={
	(OSAL_u8*) "PINIT",
	(OSAL_u8*) "PGET_CAPS",
	(OSAL_u8*) "PSET_CAPS",
#ifdef CONFIG_RTLS_FULL_LOC
	(OSAL_u8*) "PGET_SFRAME_SCH",
	(OSAL_u8*) "PSET_SFRAME_SCH",
#else
	(OSAL_u8*) "3",
	(OSAL_u8*) "4",
#endif //#ifdef CONFIG_RTLS_FULL_LOC
    (OSAL_u8*) "PSET_ST",
    (OSAL_u8*) "PRESET_ST",
	(OSAL_u8*) "PGET_ARTLS_PKT",
	(OSAL_u8*) "PSET_ARTLS_PKT",
	(OSAL_u8*) "PGET_RANGING_PKT",
	(OSAL_u8*) "A",
	(OSAL_u8*) "B",
	(OSAL_u8*) "PCTRL",
	(OSAL_u8*) "D",
	(OSAL_u8*) "E",
	(OSAL_u8*) "F"
};
#endif //#ifdef CONFIG_RTLS_PROTOCOL

/**
 * @brief uwb_com_get_command_info
 */
const OSAL_u8* uwb_com_get_command_info(uwb_com_command_t* cmd,OSAL_u8   commandId)
{
	//get the domain of the request. use const table to deduce scheme and cmdType AND NAME
	if(commandId<=SYS_LAST_CMD)
	{
		cmd->domain = UWB_CD_SYSTEM;
		cmd->scheme = sysCommandScheme[commandId-SYS_BASE_CMD].s;
		cmd->cmdType= sysCommandScheme[commandId-SYS_BASE_CMD].t;
		return sysCommandName[commandId-SYS_BASE_CMD];
	}
	else if((commandId>=RADIO_BASE_CMD)&&(commandId<=RADIO_LAST_CMD))
	{
		cmd->domain = UWB_CD_RADIO;
		cmd->scheme = radioCommandScheme[commandId-RADIO_BASE_CMD].s;
		cmd->cmdType= radioCommandScheme[commandId-RADIO_BASE_CMD].t;
		return radioCommandName[commandId-RADIO_BASE_CMD];
	}
	else if((commandId>=SEQUENCER_BASE_CMD)&&(commandId<=SEQ_LAST_CMD))
	{
		cmd->domain = UWB_CD_SEQUENCER;
		cmd->scheme = seqCommandScheme[commandId-SEQUENCER_BASE_CMD].s;
		cmd->cmdType= seqCommandScheme[commandId-SEQUENCER_BASE_CMD].t;
		return seqCommandName[commandId-SEQUENCER_BASE_CMD];
	}
	#ifdef CONFIG_RTLS_PROTOCOL
	else if((commandId>=PROTOCOL_BASE_CMD)&&(commandId<=PROT_LAST_CMD))
	{
		cmd->domain = UWB_CD_PROTOCOL;
		cmd->scheme = protCommandScheme[commandId-PROTOCOL_BASE_CMD].s;
		cmd->cmdType= protCommandScheme[commandId-PROTOCOL_BASE_CMD].t;
		return protCommandName[commandId-PROTOCOL_BASE_CMD];
	}
	#endif //#ifdef CONFIG_RTLS_PROTOCOL
	cmd->domain = UWB_CD_UNKNOWN;
	cmd->scheme = UWB_CS_COMMAND_NONE;
	cmd->cmdType= UWB_CT_NONE;
	return (OSAL_u8*) "NC";
}
#endif //#ifndef CONFIG_HIDE_COMCMD_INFO

/**
 * @brief uwb_com_create_command
 */
OSAL_error_t uwb_com_create_command(	uwb_com_command_t* cmd,
					OSAL_u8   commandId,
					OSAL_u8   subCommand,
					OSAL_u32  parameter,
					OSAL_u8*  additionalData,
					OSAL_s32  additionalDataLen,
					OSAL_u8*  outData,
					OSAL_s32  outDataLen)
{
	OSAL_u16 errLine=__LINE__;
	OSAL_bool_t	  isprms;			/**< indicate there is a parameter */

	OSAL_memset(cmd,0,sizeof(uwb_com_command_t));
	//get the domain of the request. use const table to deduce scheme and cmdType
	if(commandId<=SYS_LAST_CMD)
	{
		cmd->domain = UWB_CD_SYSTEM;
		cmd->scheme = sysCommandScheme[commandId-SYS_BASE_CMD].s;
		cmd->cmdType= sysCommandScheme[commandId-SYS_BASE_CMD].t;
	}
	else if((commandId>=RADIO_BASE_CMD)&&(commandId<=RADIO_LAST_CMD))
	{
		cmd->domain = UWB_CD_RADIO;
		cmd->scheme = radioCommandScheme[commandId-RADIO_BASE_CMD].s;
		cmd->cmdType= radioCommandScheme[commandId-RADIO_BASE_CMD].t;
	}
	else if((commandId>=SEQUENCER_BASE_CMD)&&(commandId<=SEQ_LAST_CMD))
	{
		cmd->domain = UWB_CD_SEQUENCER;
		cmd->scheme = seqCommandScheme[commandId-SEQUENCER_BASE_CMD].s;
		cmd->cmdType= seqCommandScheme[commandId-SEQUENCER_BASE_CMD].t;
	}
	#ifdef CONFIG_RTLS_PROTOCOL
	else if((commandId>=PROTOCOL_BASE_CMD)&&(commandId<=PROT_LAST_CMD))
	{
		cmd->domain = UWB_CD_PROTOCOL;
		cmd->scheme = protCommandScheme[commandId-PROTOCOL_BASE_CMD].s;
		cmd->cmdType= protCommandScheme[commandId-PROTOCOL_BASE_CMD].t;
	}
	#endif //#ifdef CONFIG_RTLS_PROTOCOL
	else
	{
		cmd->domain = UWB_CD_UNKNOWN;
		cmd->scheme = UWB_CS_COMMAND_NONE;
		cmd->cmdType= UWB_CT_NONE;
		goto unsupported;
	}

	cmd->commandId = commandId;
	if((additionalDataLen>0)&&(additionalData != NULL))
	{
	  cmd->additionalData = additionalData;
	  cmd->additionalDataLen = additionalDataLen;
	}
	else
		cmd->additionalDataLen=0;
	if((outDataLen>0)&&(outData != NULL))
	{
		cmd->outData = outData;
		cmd->outDataLen = outDataLen;
	}
	else
		cmd->outDataLen=0;

	if(parameter == UWB_P_NOPARAMETER)
		isprms = OSAL_false;
	else
	{
		cmd->prms = (OSAL_u16) parameter&0xFFFF;
		isprms = OSAL_true;
	}

	cmd->subCommand = subCommand;

	//check the scheme
	switch(cmd->scheme)
	{
		case UWB_CS_COMMAND_N_ACK:
			if(cmd->outDataLen>0) { errLine=__LINE__;goto error_in_code;}
			if(cmd->additionalDataLen>0) { errLine=__LINE__;goto error_in_code;}
			break;
		case UWB_CS_COMMAND_N_ANSWER:
			if(cmd->outDataLen==0) { errLine=__LINE__;goto error_in_code;}
			if(cmd->additionalDataLen>0) { errLine=__LINE__;goto error_in_code;}
			break;
		case UWB_CS_COMMAND_N_DATA:
			if(cmd->outDataLen>0) { errLine=__LINE__;goto error_in_code;}
			if(cmd->additionalDataLen==0) { errLine=__LINE__;goto error_in_code;}
			break;
		case UWB_CS_COMMAND_N_DATA_NANSWER:
			if(cmd->outDataLen==0) { errLine=__LINE__;goto error_in_code;}
			if(cmd->additionalDataLen==0) { errLine=__LINE__;goto error_in_code;}
			break;
		case UWB_CS_COMMAND_NONE:goto unsupported;//ask for reserved id
		default:goto unsupported;//should not happen, but in case, this mean unsupported cmd
	}

	//check the type
	switch(cmd->cmdType)
	{
		case UWB_CT_NONE:goto unsupported;//ask for reserved (should not happen this has previsouly been check through previous switch)
		case UWB_CT_COMMAND:
			if(cmd->subCommand != UWB_SC_NONE)//there should not be any subcommand
				{ errLine=__LINE__;goto error_in_code;}
			if(isprms == OSAL_true)//there should not be any parameter
				{ errLine=__LINE__;goto error_in_code;}
			break;//nothing to check
		case UWB_CT_COMMAND_WITH_SUB:
			if(cmd->subCommand == UWB_SC_NONE)//there should be a subcommand
				{ errLine=__LINE__;goto error_in_code;}
			if(isprms == OSAL_true)//there should not be any parameter
				{ errLine=__LINE__;goto error_in_code;}
			break;
		case UWB_CT_COMMAND_WITH_SUB_N_PRM:
			if(cmd->subCommand == UWB_SC_NONE)//there should be a subcommand
				{ errLine=__LINE__;goto error_in_code;}
			if(isprms == OSAL_false)//there should be a parameter
				{ errLine=__LINE__;goto error_in_code;}
			break;
		case UWB_CT_COMMAND_WITH_8BITSPRM:
		case UWB_CT_COMMAND_WITH_16BITSPRM:
			if(cmd->subCommand != UWB_SC_NONE)//there should not be any subcommand
				{ errLine=__LINE__;goto error_in_code;}
			if(isprms == OSAL_false)//there should be a parameter
				{ errLine=__LINE__;goto error_in_code;}
			break;
		default:goto unsupported;//should not happen, but in case, this mean unsupported cmd
	}

	//don't panic, everything is under control
	return OSAL_OK;


	error_in_code:
		//call to function uwb_com_send_request failed
		uwb_com_print(	TRACE_ERROR,
				(const OSAL_u8*) "CODE PB:Line[%d]",
				errLine);

		return OSAL_ERROR;


	unsupported:
		//unsupported command. may be protocol id call?
		uwb_com_print(	TRACE_ERROR,
				(const OSAL_u8*) "CMD %d NOT SUPPORTED",
				commandId);
		return OSAL_ENOMEM;

}
#ifdef UWB_COM_CAN_SEND
//dont need to compile this if its is never called
/**
 * @brief uwb_com_send_command
 */
OSAL_error_t uwb_com_send_command(	OSAL_void *contexte,
					uwb_com_command_t* cmd)
{
	OSAL_u8* answer = NULL;
	OSAL_u16 answerlen = 0;
	OSAL_u8 spi_data_in[4]={0};
	OSAL_u8 spi_data_out[4]={0};
	OSAL_u16 errLine=__LINE__;
	OSAL_s16 retry = 0;
	OSAL_s16 busyretry = 0;
	//in a brave new world, a retry should not happen, but maybe module is long to answer....
	OSAL_error_t err = OSAL_OK;

	spi_data_in[0] = cmd->commandId;
	switch(cmd->cmdType)
	{
		case UWB_CT_COMMAND_WITH_SUB://use sub command
			spi_data_in[1] = cmd->subCommand;
			break;
		case UWB_CT_COMMAND_WITH_SUB_N_PRM://use sub command and a u8 prm
			spi_data_in[1] = cmd->subCommand;
			spi_data_in[2] = (OSAL_u8) (cmd->prms &0xFF);
			break;
		case UWB_CT_COMMAND_WITH_8BITSPRM://use a u8 prm
			spi_data_in[1] = (OSAL_u8) (cmd->prms &0x00FF);
			break;
		case UWB_CT_COMMAND_WITH_16BITSPRM://use a u16 prm
			U16VAL_TO_MSBLSB(cmd->prms,spi_data_in[1],spi_data_in[2]);
			break;
		default://dont need anything
			break;
	}
	//now send the request
retry:
	if(uwb_com_lock(contexte)!=0)
	{
		err = OSAL_EAGAIN;
		errLine=__LINE__;
		goto beach;
	}
	if(uwb_com_spi_send(contexte,spi_data_in,3) !=0)
	{
		err = OSAL_EAGAIN;
		errLine=__LINE__;
		goto beach;
	}
	//add delay
	uwb_com_addDelay(-1);
	answer = &spi_data_out[0];
	answerlen = UWB_C_DEFAULT_REQ_SIZE;
	//consider the type of scheme and send the request
	switch(cmd->scheme)
	{
		case UWB_CS_COMMAND_N_ANSWER:
			answer = cmd->outData;
			answerlen = cmd->outDataLen;
			//and then its the same than UWB_CS_COMMAND_N_ACK
		case UWB_CS_COMMAND_N_ACK:
			//we must check for acknowledgement
			if(uwb_com_spi_receive(contexte,&spi_data_out[0], answer,answerlen) !=0)
			{
				err = OSAL_EAGAIN;
				errLine=__LINE__;
				goto beach;
			}
			break;
		case UWB_CS_COMMAND_N_DATA_NANSWER:
			answer = cmd->outData;
			answerlen = cmd->outDataLen;
			//and then its the same than UWB_CS_COMMAND_N_DATA
		case UWB_CS_COMMAND_N_DATA:
			if(uwb_com_spi_sendExtra(contexte,&spi_data_out[0],answer,answerlen,cmd->additionalData,cmd->additionalDataLen) !=0)
			{
				err = OSAL_EAGAIN;
				errLine=__LINE__;
				goto beach;
			}
			break;
		default:
			err = OSAL_ERROR;//should not happen, sheme is already checked in uwb_com_createCmd
			errLine=__LINE__;
			goto beach;
	}
	switch(spi_data_out[0]&UWB_C_ANSWER_MASK)
	{
		case UWB_C_ANSWER_RETRY:
			retry++;
			if(retry>UWB_C_DEFAULTRETRY_NB)
			{
				uwb_com_print(TRACE_WARN,(const OSAL_u8*) "%x NO ANS",cmd->commandId);
				err = OSAL_EAGAIN;
				errLine=__LINE__;
				goto beach;
			}
			uwb_com_print(TRACE_DEBUG,(const OSAL_u8*) "RETRY %x",cmd->commandId);
			switch(retry){
				case 1:uwb_com_addRetryDelay(UWB_C_DFLT_RETRY_DELAY1_US);break;
				case 2:uwb_com_addRetryDelay(UWB_C_DFLT_RETRY_DELAY2_US);break;
				case 3:uwb_com_addRetryDelay(UWB_C_DFLT_RETRY_DELAY3_US);break;
				case 4:uwb_com_addRetryDelay(UWB_C_DFLT_RETRY_DELAY4_US);break;
				default:uwb_com_addRetryDelay(UWB_C_DFLT_RETRY_DELAY5_US);break;
			}
			uwb_com_unlock(contexte);
			goto retry;
		case UWB_C_ANSWER_BUSY:
			busyretry++;
			if(busyretry>UWB_C_DEFAULTBUSY_RETRY_NB)
			{
				uwb_com_print(TRACE_WARN,(const OSAL_u8*) "%x NO ANS",cmd->commandId);
				err = OSAL_EAGAIN;
				errLine=__LINE__;
				goto beach;
			}
			uwb_com_print(TRACE_WARN,(const OSAL_u8*) "BUSY %x",cmd->commandId);
			switch(busyretry){
				case 1:uwb_com_addBusyDelay(UWB_C_DFLT_BUSY_DELAY1_MS);break;
				case 2:uwb_com_addBusyDelay(UWB_C_DFLT_BUSY_DELAY2_MS);break;
				case 3:uwb_com_addBusyDelay(UWB_C_DFLT_BUSY_DELAY3_MS);break;
				case 4:uwb_com_addBusyDelay(UWB_C_DFLT_BUSY_DELAY4_MS);break;
				default:uwb_com_addBusyDelay(UWB_C_DFLT_BUSY_DELAY5_MS);break;
			}
			uwb_com_unlock(contexte);
			goto retry;
		case UWB_C_ANSWER_ACK:
			break;
		case UWB_C_ANSWER_UNSUPPORTED:
			err = OSAL_EAGAIN;
			errLine=__LINE__;
			goto beach;
	}
	uwb_com_addPostDelay(-1);
	beach:
	uwb_com_unlock(contexte);
	if(err != OSAL_OK)
	{
	  uwb_com_print(TRACE_ERROR,
			(const OSAL_u8*) "?? Line%d",
			errLine);
	}
	return err;
}


/**
 * @brief uwb_com_create_and_send_command
 */
OSAL_error_t uwb_com_create_and_send_command(	OSAL_void *contexte,
						OSAL_u8   commandId,
						OSAL_u8   subCommand,
						OSAL_u32  parameter,
						OSAL_u8*  additionalData,
						OSAL_u32  additionalDataLen,
						OSAL_u8*  outData,
						OSAL_u32  outDataLen)
{
	uwb_com_command_t cmd;
	OSAL_error_t err= OSAL_OK;

	//create the command struct and check for parameters
	if((err=uwb_com_create_command(	&cmd,
					commandId,
					subCommand,
					parameter,
					additionalData,
					additionalDataLen,
					outData,
					outDataLen)
				)!= OSAL_OK)
		return err;
	return uwb_com_send_command(contexte, &cmd);
}
#endif //#ifdef UWB_COM_CAN_REQUEST

#ifdef UWB_COM_CAN_ANSWER
//dont need to compile this if its is never called
/**
 * @brief uwb_com_analyze_gotcommand
 */
OSAL_error_t uwb_com_analyze_gotcommand( uwb_com_command_t* cmd, OSAL_u8 data_spi_in[3])
{
	cmd->commandId = data_spi_in[0];
	//get the domain of the request. use const table to deduce scheme and cmdType
	if(cmd->commandId<=SYS_LAST_CMD)
	{
		cmd->domain = UWB_CD_SYSTEM;
		cmd->scheme = sysCommandScheme[cmd->commandId-SYS_BASE_CMD].s;
		cmd->cmdType= sysCommandScheme[cmd->commandId-SYS_BASE_CMD].t;
	}
	else if((cmd->commandId>=RADIO_BASE_CMD)&&(cmd->commandId<=RADIO_LAST_CMD))
	{
		cmd->domain = UWB_CD_RADIO;
		cmd->scheme = radioCommandScheme[cmd->commandId-RADIO_BASE_CMD].s;
		cmd->cmdType= radioCommandScheme[cmd->commandId-RADIO_BASE_CMD].t;
	}
	else if((cmd->commandId>=SEQUENCER_BASE_CMD)&&(cmd->commandId<=SEQ_LAST_CMD))
	{
		cmd->domain = UWB_CD_SEQUENCER;
		cmd->scheme = seqCommandScheme[cmd->commandId-SEQUENCER_BASE_CMD].s;
		cmd->cmdType= seqCommandScheme[cmd->commandId-SEQUENCER_BASE_CMD].t;
	}
	#ifdef CONFIG_RTLS_PROTOCOL
	else if((cmd->commandId>=PROTOCOL_BASE_CMD)&&(cmd->commandId<=PROT_LAST_CMD))
	{
		cmd->domain = UWB_CD_PROTOCOL;
		cmd->scheme = protCommandScheme[cmd->commandId-PROTOCOL_BASE_CMD].s;
		cmd->cmdType= protCommandScheme[cmd->commandId-PROTOCOL_BASE_CMD].t;
	}
	#endif //#ifdef CONFIG_RTLS_PROTOCOL
	else
	{
		goto unsupported;
	}
	cmd->subCommand=UWB_SC_NONE;
	//check the sheme
	switch(cmd->scheme)
	{
		case UWB_CS_COMMAND_N_ANSWER://we need to knwo the buffer of out data
		case UWB_CS_COMMAND_N_DATA:
		case UWB_CS_COMMAND_N_DATA_NANSWER:
		case UWB_CS_COMMAND_N_ACK:
			break;
		default:goto unsupported;//should not happen, exept for reserver id, but in case, this mean unsupported cmd
	}
	//check the type
	switch(cmd->cmdType)
	{
		case UWB_CT_NONE: goto unsupported;//ask for reserved if. WXon't happen, it has been check on previous switch
		case UWB_CT_COMMAND:
			break;//nothing to check
		case UWB_CT_COMMAND_WITH_SUB:
			cmd->subCommand=data_spi_in[1];
			if(cmd->subCommand == UWB_SC_NONE)//there should be a subcommand
				goto unsupported;
			break;
		case UWB_CT_COMMAND_WITH_SUB_N_PRM:
			cmd->subCommand=data_spi_in[1];
			if(cmd->subCommand == UWB_SC_NONE)//there should be a subcommand
				goto unsupported;
			cmd->prms = (OSAL_u16) data_spi_in[2];
			break;
		case UWB_CT_COMMAND_WITH_8BITSPRM:
			cmd->prms = (OSAL_u16) data_spi_in[1];
			break;
		case UWB_CT_COMMAND_WITH_16BITSPRM:
			cmd->prms = (OSAL_u16) data_spi_in[1];
			cmd->prms <<=8;
			cmd->prms |=(OSAL_u16) data_spi_in[2];
			break;
		default:goto unsupported;//should not happen, but in case, this mean unsupported cmd
	}

	cmd->additionalData = NULL;
	cmd->additionalDataLen = 0;
	cmd->outData = NULL;
	cmd->outDataLen = 0;

	return OSAL_OK;
	unsupported:
		cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
		//unsupported command. may be protocol id call while not activated ?
		uwb_com_print(TRACE_WARN,(const OSAL_u8*) "%x N.C.",cmd->commandId);
		//return ok, because we will answer caller that this is not supported
		return OSAL_OK;
}

/**
 * @brief uwb_com_answer_gotcommand
*/
OSAL_error_t uwb_com_answer_gotcommand(OSAL_void *contexte,uwb_com_command_t* cmd )
{
	if(cmd->cmdAnswer != UWB_C_ANSWER_UNSUPPORTED)//otherhwise, don't need for process, answer is
	{
		uwb_com_process_command(contexte,cmd);
		//debug purpose
		switch(cmd->cmdAnswer)
		{
			default:
			case UWB_C_ANSWER_RETRY:
			case UWB_C_ANSWER_BUSY:
				uwb_com_print(TRACE_DEBUG,(const OSAL_u8*) "RETRY %x",cmd->commandId);
				break;
			case UWB_C_ANSWER_ACK://its ok
				uwb_com_print(TRACE_DEBUG,(const OSAL_u8*) "%x OK",cmd->commandId);
				break;
			case UWB_C_ANSWER_UNSUPPORTED:
				uwb_com_print(TRACE_WARN,(const OSAL_u8*) "%x N.C.",cmd->commandId);
				break;
		}
	}
	return OSAL_OK;
}


/**
 * @brief uwb_com_analyze_and_answer_gotcommand
 */
OSAL_error_t uwb_com_analyze_and_answer_gotcommand(	OSAL_void *contexte, OSAL_u8 data_spi_in[3])
{
	uwb_com_command_t cmd;
	OSAL_error_t err= OSAL_OK;

	//create the command struct and check for parameters
	if((err=uwb_com_analyze_gotcommand(&cmd,data_spi_in))!= OSAL_OK)
		return err;
	return uwb_com_answer_gotcommand(contexte, &cmd);
}

//specific to fifo of cmd:
#define MAX_CMD_FIFO_LEN 8
static uwb_com_command_t cmd_fifo[MAX_CMD_FIFO_LEN];
OSAL_u32 read_from_cmd_fifo=0;
OSAL_u32 write_to_cmd_fifo=0;

/**
 * @brief uwb_enqueue_cmd
 */
OSAL_s32 uwb_enqueue_cmd(uwb_com_command_t* cmd)
{
	OSAL_u32 cmd_in_fifo=0;
	if(read_from_cmd_fifo >= write_to_cmd_fifo)
	{
		uwb_clean_queue(NULL);
	}
	cmd_in_fifo = write_to_cmd_fifo-read_from_cmd_fifo;
	if(cmd == NULL) return 0;
	//count number of element	
	if((cmd_in_fifo+1)>MAX_CMD_FIFO_LEN)
	{
		OSAL_trace(TRACE_ERROR, "!FIFO");//need to incr MAX_CMD_FIFO_LEN?
		return -1;
	}
	else
	{
		uwb_com_command_t* fifocmd=&cmd_fifo[write_to_cmd_fifo%MAX_CMD_FIFO_LEN];
		OSAL_memcpy(fifocmd,cmd,sizeof(uwb_com_command_t));
		write_to_cmd_fifo++;
		uwb_com_print(TRACE_DEBUG,(const OSAL_u8*) "Enqueue(%d)",cmd_in_fifo+1);
	}
	//need some unlock mutex here
	return 1;//1 cmd enqueued
}

/**
 * @brief uwb_dequeue_cmd
 */
OSAL_s32 uwb_dequeue_cmd(uwb_com_command_t* cmd)
{
	//need some mutex here
	uwb_com_command_t* fifocmd=&cmd_fifo[read_from_cmd_fifo%MAX_CMD_FIFO_LEN];
	if(read_from_cmd_fifo >= write_to_cmd_fifo) return 0;//no more item in queue
	OSAL_memcpy(cmd,fifocmd,sizeof(uwb_com_command_t));
	read_from_cmd_fifo++;                     
	uwb_com_print(TRACE_DEBUG,(const OSAL_u8*) "Dequeu(%d)",write_to_cmd_fifo+read_from_cmd_fifo);
	return 1;//1 cmd dequeued
}

/**
 * @brief uwb_get_queue_info
*/
OSAL_void uwb_get_queue_info(OSAL_s32* usedItemInQueue, OSAL_s32* freeItemInQueue, OSAL_s32* maxItemInqueue)
{
	if(usedItemInQueue) 	*usedItemInQueue = write_to_cmd_fifo-read_from_cmd_fifo;
	if(freeItemInQueue) 	*freeItemInQueue = MAX_CMD_FIFO_LEN-write_to_cmd_fifo+read_from_cmd_fifo;
	if(maxItemInqueue) 	*maxItemInqueue = MAX_CMD_FIFO_LEN;
}

/**
 * @brief uwb_is_queue_full
 */
OSAL_void uwb_is_queue_full(OSAL_s32* freeItemInQueue)
{
	uwb_get_queue_info(NULL,freeItemInQueue,NULL);
}


/**
 * @brief uwb_clean_queue
 */
OSAL_void uwb_clean_queue(OSAL_s32* maxItemInqueue)
{
	//reset rd/wr index
	write_to_cmd_fifo=0;
	read_from_cmd_fifo=0;
	uwb_get_queue_info(NULL,NULL,maxItemInqueue);
}

#endif //#ifdef UWB_COM_CAN_ANSWER

