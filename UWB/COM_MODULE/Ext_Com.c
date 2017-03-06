/**
 * @file Ext_Com.c
 * @brief Ext_Com.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#include "stm32f0xx_spi.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_gpio.h"
#include "SPI.h"
#include "types.h"
#include "Ext_Com.h"
#include "uwbmodule_com.h"
#include "module_api.h"

/* Global variables */


unsigned char EXTCOM_BUF_PCKTTRACE=0;
int EXTCOM_BUF_TxLen = UWB_C_DEFAULT_REQ_SIZE;
int EXTCOM_BUF_RxLen = UWB_C_DEFAULT_REQ_SIZE;
#define EXTCOM_BUF_SIZE 12
unsigned char EXTCOM_BUF[EXTCOM_BUF_SIZE]={UWB_CS_COMMAND_NONE};
unsigned char* EXTCOM_BUF_TxFrame = &EXTCOM_BUF[EXTCOM_BUF_TX_BEGIN];
unsigned char* EXTCOM_BUF_RxFrame = &EXTCOM_BUF[EXTCOM_BUF_RX_BEGIN];

#ifdef UWB_COM_CAN_ANSWER
static int commandComplete=0;
static uwb_com_command_t* next_cmd_to_enqueue=NULL;
static uwb_com_command_t next_cmd;
static void resetComm()
{
	//enqueue cmd for next management
	if(commandComplete!=0) //treaten, dont need to forbid sleep, but let some 500u to be sure dma transfer terminated
	{
		sys_authorizesleep_after_X_usec(500,SYS_WAKE_UP_SPI_MASK);
		commandComplete=0;
	}
	uwb_enqueue_cmd(next_cmd_to_enqueue);//if NULL, nothing will happen
	next_cmd_to_enqueue = NULL;
	OSAL_memset(EXTCOM_BUF,0,EXTCOM_BUF_SIZE);
	EXTCOM_BUF_TxFrame = &EXTCOM_BUF[EXTCOM_BUF_TX_BEGIN];
	EXTCOM_BUF_TxLen =  UWB_C_DEFAULT_REQ_SIZE;
	EXTCOM_BUF_RxFrame = &EXTCOM_BUF[EXTCOM_BUF_RX_BEGIN];
	EXTCOM_BUF_RxLen =  UWB_C_DEFAULT_REQ_SIZE;
	EXTCOM_BUF_TxFrame[0] = UWB_C_ANSWER_RETRY;
	//next command is UWB_CS_COMMAND_NONE
	EXTCOM_BUF[EXTCOM_BUF_STEP_INDICATOR] = UWB_CS_COMMAND_NONE;
}
#endif //#ifdef UWB_COM_CAN_ANSWER

void uwb_com_ext( void)
{
	#ifdef UWB_COM_CAN_ANSWER
	uwb_com_command_t cmd;
	OSAL_s32 queuefull = 0;
	OSAL_u8 step = EXTCOM_BUF[EXTCOM_BUF_STEP_INDICATOR];
	
	//the last char in EXTCOM_BUF table (EXTCOM_BUF[7]) will be use to indicate step in communication
	//we always have 2 step1, linux->sendRequest, step2: linux get answer or send additionnal
	//the value in EXTCOM_BUF[7] is UWB_CS_NONE when we don't know about the incomming command
	//then, after treatment , it becomes a uwb_com_scheme_e value

	//for a reason or another, it can ocur that step are desynchronized.
	//eg, we consider a step 1 , wereas, its a step0 from linux base pointofview
	//try to correct this asap
	switch(step)//first step
	{
		case UWB_CS_COMMAND_NONE:
			//prepare for the next transfer
			EXTCOM_BUF_TxFrame = &EXTCOM_BUF[EXTCOM_BUF_TX_BEGIN];
			EXTCOM_BUF_TxLen =  UWB_C_DEFAULT_REQ_SIZE;
			EXTCOM_BUF_RxFrame = &EXTCOM_BUF[EXTCOM_BUF_RX_BEGIN];
			EXTCOM_BUF_RxLen =  UWB_C_DEFAULT_REQ_SIZE;
			EXTCOM_BUF[EXTCOM_BUF_STEP_INDICATOR] = UWB_CS_COMMAND_UNKNOWN;
			uwb_com_printpckt((OSAL_u8*) "RqRx",EXTCOM_BUF_RxFrame);
			uwb_com_printpckt((OSAL_u8*) "RqTx",EXTCOM_BUF_TxFrame);
			if(EXTCOM_BUF_RxFrame[0]==0)//forbidden value, can appear only in second step
			{
				uwb_com_print(TRACE_ERROR,(OSAL_u8*) "FORBIDDEN VALUE,RESYNC");
				resetComm();
				return;
			}
			commandComplete=1;
			next_cmd_to_enqueue = NULL;
			uwb_is_queue_full(&queuefull);
			if(queuefull<1)
			{
				//too much requets in pipe. Module is busy
				uwb_com_print(TRACE_ERROR,(OSAL_u8*) "QUEUE FULL,BUSY");
				EXTCOM_BUF_TxFrame[0] = EXTCOM_BUF_RxFrame[0]|UWB_C_ANSWER_BUSY;
			}
			else if(uwb_com_analyze_gotcommand(&cmd,&EXTCOM_BUF_RxFrame[0])==OSAL_OK)
			{
				uwb_com_answer_gotcommand(NULL,&cmd);
				EXTCOM_BUF[EXTCOM_BUF_STEP_INDICATOR] = cmd.scheme;
				EXTCOM_BUF_TxFrame[0] = EXTCOM_BUF_RxFrame[0]|cmd.cmdAnswer;
				if(cmd.cmdAnswer == UWB_C_ANSWER_ACK)
				{
					next_cmd_to_enqueue = &next_cmd;
					OSAL_memcpy(next_cmd_to_enqueue,&cmd,sizeof(uwb_com_command_t));
					switch(cmd.scheme)
					{
						case UWB_CS_COMMAND_N_ANSWER:
							EXTCOM_BUF_TxFrame = cmd.outData;
							EXTCOM_BUF_TxLen =  cmd.outDataLen;
							//buffer could have changed, re-set answer
							EXTCOM_BUF_TxFrame[0] = EXTCOM_BUF_RxFrame[0]|cmd.cmdAnswer;
							break;
						case UWB_CS_COMMAND_N_DATA:
							EXTCOM_BUF_RxFrame = cmd.additionalData;
							EXTCOM_BUF_RxLen =  cmd.additionalDataLen;
							break;
						case UWB_CS_COMMAND_N_DATA_NANSWER:
							EXTCOM_BUF_TxFrame = cmd.outData;
							EXTCOM_BUF_TxLen =  cmd.outDataLen;
							EXTCOM_BUF_RxFrame = cmd.additionalData;
							EXTCOM_BUF_RxLen =  cmd.additionalDataLen;
							//buffer could have changed, re-set answer
							EXTCOM_BUF_TxFrame[0] = EXTCOM_BUF_RxFrame[0]|cmd.cmdAnswer;
							break;
						default://case UWB_CS_COMMAND_N_ACK:
							break;
					}
				}
			}
			else //failure, set unsupported
			{
				EXTCOM_BUF_TxFrame[0] = EXTCOM_BUF_RxFrame[0]|UWB_C_ANSWER_UNSUPPORTED;
			}
		break;
		case UWB_CS_COMMAND_N_DATA: //we cannot check for desynchronization, rx can haev wahetver
		case UWB_CS_COMMAND_N_DATA_NANSWER: //we cannot check for desynchronization, rx can haev wahetver
			//here we cannot check for desynchronization
			uwb_com_print(TRACE_DEBUG,(OSAL_u8*) "cmd %d",step);
			uwb_com_printpckt((OSAL_u8*) "E0Rx",EXTCOM_BUF_RxFrame);
			uwb_com_printpckt((OSAL_u8*) "A0Tx",EXTCOM_BUF_TxFrame);
			resetComm();
			break;
		case UWB_CS_COMMAND_UNKNOWN:
			//ignore and resynchonize only
			uwb_com_print(TRACE_DEBUG,(OSAL_u8*) "Resync.");
			next_cmd_to_enqueue = NULL;
			resetComm();
			break;
		default: //case UWB_CS_COMMAND_N_ACK ://we expect RX as a 3sized buffer with <0>. If not, mean we're desynchronized
			//case UWB_CS_COMMAND_N_ANSWER ://we expect RX as a 3sized buffer with <0>. If not, mean we're desynchronized
			uwb_com_printpckt((OSAL_u8*) "A1Rx",EXTCOM_BUF_RxFrame);
			uwb_com_printpckt((OSAL_u8*) "A1Tx",EXTCOM_BUF_TxFrame);
			if(
				(EXTCOM_BUF_RxFrame[0] != 0)||
				(EXTCOM_BUF_RxFrame[1] != 0)||
				(EXTCOM_BUF_RxFrame[2] != 0)
			)
			{
				//shoudl not happen.This mean this is a step 1, not step 2 Resync
				uwb_com_print(TRACE_ERROR,(OSAL_u8*) "BAD STEP,RESYNC");
				next_cmd_to_enqueue = NULL;
				resetComm();
				EXTCOM_BUF[EXTCOM_BUF_STEP_INDICATOR] = UWB_CS_COMMAND_UNKNOWN;
				return;
			}
			uwb_com_print(TRACE_DEBUG,(OSAL_u8*) "cmd %d",step);			
			resetComm();
			break;
	}
	#endif //#ifdef UWB_COM_CAN_ANSWER
}

