/**
 * @file  uwbmodule_com_specific.c
 * @brief  module spi transfer  definition and tools implementation. Specific to lin,uxConform with spec Module_specification.pdf @rev1.4
 * @author ore@bespoon.com
 * @date 14/04/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */
#include "osal_trace.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_exti.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_usart.h"
#include "osal_math.h"
#include "HAL_config.h"
#include "USART.h"
#include "uwbmodule_com.h"
#include "Ext_Com.h" //for EXTCOM_BUF_PCKTTRACE
#include "module_api.h"


/**
 * @brief UWB_C_DEFAULTDELAY_US : Default delay between a request and an answer
*/
#define UWB_C_DEFAULTDELAY_US		500

/**
 * @brief UWB_C_DEFAULTPOSTDELAY_US : Default delay the last a spi request command
*/
#define UWB_C_DEFAULTPOSTDELAY_US	50

//use this under condition you should hugly increase delay on linux side ( from 500u, to 10ms at least)
//to avoid uart trace perturbation
//so define EXTCOM_BUF_PCKTTRACE in uwbmodule too

/**
 * @brief uwb_com_printpckt
 * @note print packet ( debug purpose)
 * MODULE ARM IMPLEMENTATION
 */
OSAL_void uwb_com_printpckt(const OSAL_u8* desc, OSAL_u8* buf) {
	if (EXTCOM_BUF_PCKTTRACE)
		//print only 4 first value
		uwb_com_print(TRACE_DEBUG,(OSAL_u8*) "<%s>0X{%x,%x,%x,%x}", desc, buf[0], buf[1], buf[2], buf[3]);
		//print only 5 first value
		//uwb_com_print(TRACE_DEBUG,(OSAL_u8*) "<%s>0X{%x,%x,%x,%x,%x}", desc, buf[0], buf[1], buf[2], buf[3],buf[4]);
		//print only 10 first value
		//uwb_com_print(TRACE_DEBUG,(OSAL_u8*) "<%s>0X{%x,%x,%x,%x,%x,%x,%x,%x,%x,%x}", desc, buf[0], buf[1], buf[2], buf[3],buf[4],buf[5], buf[6], buf[7], buf[8],buf[9]);

}

/**
 * @brief uwb_com_print
 * @note print trace ( debug purpose)
 * MODULE ARM IMPLEMENTATION
 */
OSAL_void uwb_com_print(int dbgLevel, const OSAL_u8* fmt, ...) {
	if (EXTCOM_BUF_PCKTTRACE) {
		va_list ap;
		va_start(ap, fmt);
		switch (dbgLevel) {
		case TRACE_VERBOSE:
			OSAL_vtrace(TRACE_VERBOSE, fmt, ap);
			break;
		case TRACE_DEBUG:
			OSAL_vtrace(TRACE_DEBUG, fmt, ap);
			break;
		case TRACE_INFO:
			OSAL_vtrace(TRACE_INFO, fmt, ap);
			break;
		case TRACE_WARN:
			OSAL_vtrace(TRACE_WARN, fmt, ap);
			break;
		case TRACE_ERROR:
			OSAL_vtrace(TRACE_ERROR, fmt, ap);
			break;
		default:
			break;
		}
		va_end(ap);
	}
}

#ifdef UWB_COM_CAN_SEND

/**
 * @brief uwb_com_addDelay
 * MODULE ARM IMPLEMENTATION
 */
OSAL_void uwb_com_addDelay(OSAL_s32 delayInUs)
{
	OSAL_s32 delay = (delayInUs>0)?delayInUs:UWB_C_DEFAULTDELAY_US;
	if(EXTCOM_BUF_PCKTTRACE)
	{
		//fix to 50ms or 1second ( debug only
		if(EXTCOM_BUF_PCKTTRACE==1)
			OSAL_msleep(50);
		else
			OSAL_msleep(1000);
	}
	else
		OSAL_udelay(delay);
}

/**
 * @brief uwb_com_addPostDelay
 * MODULE ARM IMPLEMENTATION
*/
OSAL_void uwb_com_addPostDelay(OSAL_s32 delayInUs)
{
	OSAL_s32 delay = (delayInUs>0)?delayInUs:UWB_C_DEFAULTPOSTDELAY_US;
	OSAL_udelay(delay);
}

/**
 * @brief uwb_com_addRetryDelay
 * MODULE ARM IMPLEMENTATION
*/
OSAL_void uwb_com_addRetryDelay(uwb_c_dflt_retry_delay_us delayInUs)
{
	if(EXTCOM_BUF_PCKTTRACE)
	{
		if(EXTCOM_BUF_PCKTTRACE==1)
			OSAL_msleep(50);
		else
			OSAL_msleep(1000);
	}
	else
		OSAL_udelay(delayInUs);
}

/**
 * @brief uwb_com_addBusyDelay
 * MODULE ARM IMPLEMENTATION
*/
OSAL_void uwb_com_addBusyDelay(uwb_c_dflt_busy_delay_ms delay)
{
	if(EXTCOM_BUF_PCKTTRACE)
	{
		if(EXTCOM_BUF_PCKTTRACE==1)
			OSAL_msleep(50);
		else
			OSAL_msleep(1000);
	}
	else
		OSAL_msleep(delay);
}

/**
 * @brief uwb_com_open
 * @note open an instance for spi transfer
 * MODULE ARM IMPLEMENTATION
 */
OSAL_s32  uwb_com_open (OSAL_void* contexte)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	HAL_USART3_Init(ENABLE);
	/*----- init GIO to Receive external IT FROM CONNECTED MODULE -----*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RECEIVE_M1_IT;
	GPIO_Init(GPIO_RECEIVE_M1_IT, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_RECEIVE_M1_IT, GPIO_PIN_RECEIVE_M1_IT);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RECEIVE_M2_IT;
	GPIO_Init(GPIO_RECEIVE_M2_IT, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_RECEIVE_M2_IT, GPIO_PIN_RECEIVE_M2_IT);
	return 0;
}

/**
 * @brief uwb_com_close
 * MODULE ARM IMPLEMENTATION
 */
OSAL_void  uwb_com_close (OSAL_void* contexte)
{
}

/**
 * @brief uwb_com_lock
 * @note lock the spi transfer, to avoid multiple spi transfer
 * MODULE ARM IMPLEMENTATION
 */
OSAL_s32 uwb_com_lock(OSAL_void* contexte)
{
	return 0;
}

/**
 * @brief uwb_com_unlock
 * @note unlock the spi transfer, to let further spi transfer run
 * MODULE ARM IMPLEMENTATION
 */
OSAL_void uwb_com_unlock(OSAL_void* contexte)
{
}

/**
 * @brief uwb_com_checkModuleIT
 * @note check if IT of the moduel is down. This allow to that ther wasn't any other IT coming while we were processing the previous one
 * MODULE ARM IMPLEMENTATION
 */
OSAL_u8 uwb_com_checkModuleIT(OSAL_void* contexte)
{
	return 0;
}

static void uwb_com_invertBitToBit(OSAL_u8* buf, OSAL_u16 bufLen)
{
	OSAL_u16 len=bufLen;
	OSAL_u8* p=buf;
	OSAL_u8 tmp;
	do
	{
		tmp=*p;
		tmp = (tmp << 4 ) | (tmp >> 4);
		tmp = ((tmp << 2 )&0xCC) | ((tmp >> 2)&0x33);
		tmp = ((tmp << 1 )&0xAA) | ((tmp >> 1)&0x55);
		*p++=tmp;
	}while(len-->0);
}

static OSAL_u8 lastcmd=0;
/**
 * @brief uwb_com_spi_send
 * @note send data trough spi
 * MODULE ARM IMPLEMENTATION
 */
OSAL_s32 uwb_com_spi_send(	OSAL_void* contexte,
				OSAL_u8    *tx_buff,
				OSAL_u16   in_len)
{
	unsigned char rx[UWB_C_DEFAULT_REQ_SIZE+1];
	OSAL_memset(&rx[0],UWB_CS_COMMAND_NONE,UWB_C_DEFAULT_REQ_SIZE+1);
	lastcmd=tx_buff[0];
	uwb_com_printpckt((OSAL_u8*) "0RqRX",rx);
	uwb_com_printpckt((OSAL_u8*) "0RqTX",tx_buff);	
	uwb_com_invertBitToBit(tx_buff,in_len);
	HAL_USART3_Sync_read_buffer(tx_buff,in_len,rx,UWB_C_DEFAULT_REQ_SIZE);
	//reinvert txw, it could be used somewhere in SW
	uwb_com_invertBitToBit(tx_buff,in_len);
	if (EXTCOM_BUF_PCKTTRACE)
	{
		uwb_com_printpckt((OSAL_u8*) "1RqRX",rx);
		uwb_com_printpckt((OSAL_u8*) "1RqTX",tx_buff);
	}
	OSAL_udelay(5000);
	return 0;
}

/**
 * @brief uwb_com_spi_sendExtra
 * @note send data trough spi
 * MODULE ARM IMPLEMENTATION
 */
OSAL_s32 uwb_com_spi_sendExtra(	OSAL_void* contexte,
				OSAL_u8    *acknowledge,
				OSAL_u8    *rx_buff,
				OSAL_u16   out_len,
				OSAL_u8    *tx_buff,
				OSAL_u16   in_len)
{
	OSAL_u8 gotcmd=0;
	OSAL_memset(&rx_buff[0],UWB_CS_COMMAND_NONE,out_len);
	uwb_com_printpckt((OSAL_u8*) "0A0RX",rx_buff);
	uwb_com_printpckt((OSAL_u8*) "0E0TX",tx_buff);
	uwb_com_invertBitToBit(tx_buff,in_len);
	HAL_USART3_Sync_read_buffer(tx_buff,in_len,rx_buff,out_len);
	//put tx back to normal. It could be used by sw
	uwb_com_invertBitToBit(tx_buff,in_len);
	//invert rx buf
	uwb_com_invertBitToBit(rx_buff,out_len);
	if (EXTCOM_BUF_PCKTTRACE)
	{
		uwb_com_printpckt((OSAL_u8*) "1A0RX",rx_buff);
		uwb_com_printpckt((OSAL_u8*) "1E0TX",tx_buff);
	}
	gotcmd = rx_buff[0] &(~UWB_C_ANSWER_MASK);
	uwb_com_print(TRACE_DEBUG,(OSAL_u8*) "cmd = %x/%x",lastcmd,gotcmd);
	if(gotcmd == lastcmd) *acknowledge = rx_buff[0];
	else *acknowledge = UWB_C_ANSWER_RETRY;
	OSAL_udelay(5000);
	return 0;
}

/**
 * @brief uwb_com_spi_receive
 * @note receive data trough spi
 * MODULE ARM IMPLEMENTATION
*/
OSAL_s32 uwb_com_spi_receive(	OSAL_void* contexte,
				OSAL_u8    *acknowledge,
				OSAL_u8    *rx_buff,
				OSAL_u16   out_len)
{
	OSAL_u8 gotcmd=0;
	unsigned char tx[UWB_C_DEFAULT_REQ_SIZE+1];
	OSAL_memset(&tx[0],UWB_CS_COMMAND_NONE,UWB_C_DEFAULT_REQ_SIZE+1);
	OSAL_memset(&rx_buff[0],UWB_CS_COMMAND_NONE,out_len);
	//dont invert, its zeros
	uwb_com_printpckt((OSAL_u8*) "0A1RX",rx_buff);
	uwb_com_printpckt((OSAL_u8*) "0A1TX",tx);
	HAL_USART3_Sync_read_buffer(tx,UWB_C_DEFAULT_REQ_SIZE,rx_buff,out_len);
	uwb_com_invertBitToBit(rx_buff,out_len);
	if (EXTCOM_BUF_PCKTTRACE) //reinvert to show trace
	{
		uwb_com_invertBitToBit(tx,UWB_C_DEFAULT_REQ_SIZE);
		uwb_com_printpckt((OSAL_u8*) "1A1RX",rx_buff);
		uwb_com_printpckt((OSAL_u8*) "1A1TX",tx);
	}	
	gotcmd = rx_buff[0] &(~UWB_C_ANSWER_MASK);
	uwb_com_print(TRACE_DEBUG,(OSAL_u8*) "cmd = %x/%x",lastcmd,gotcmd);
	if(gotcmd == lastcmd) *acknowledge = rx_buff[0];
	else *acknowledge = UWB_C_ANSWER_RETRY;
	OSAL_udelay(5000);
	return 0;
}

/**
 * @brief uwb_com_init
 * @note initialize hardware for preparing uwbmodule com
 * MODULE ARM IMPLEMENTATION
 */
OSAL_s32 uwb_com_init(OSAL_void* contexte,OSAL_void** driver)
{
	return 0;
}

/**
 * @brief uwb_com_exit
 * @note de-initialize hardware, must be called last
 * LINUX IMPLEMENTATION
 */
OSAL_void uwb_com_exit(OSAL_void* contexte)
{

}
#endif //#ifdef UWB_COM_CAN_SEND
#ifdef UWB_COM_CAN_ANSWER
/**
 * @brief uwb_com_process_sys_command
 * MODULE ARM IMPLEMENTATION
 */
static OSAL_void uwb_com_process_sys_command(OSAL_void* contexte,uwb_com_command_t* cmd)
{
	if(sys_is_init_busy())
	{
		cmd->cmdAnswer = UWB_C_ANSWER_BUSY;	
		return;
	}
	switch (cmd->commandId)
	{
		//case SYS_RSVD0:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		case SYS_GET_INFO:
			if(
				(UWB_SC_INFO_VERSION > cmd->subCommand)||
				#ifndef PROD
				(UWB_SC_INFO_SPVTUNE < cmd->subCommand)
				#else
				(UWB_SC_INFO_PROD_RESULT < cmd->subCommand)
				#endif
			)
			{
				cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
				break;
			}
			sys_get_info(cmd->subCommand,cmd->prms,&cmd->outData,&cmd->outDataLen);
			if(cmd->outData == NULL) cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
			break;
		//case SYS_RSVD2:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		case SYS_SET_MCU_STATE: 
			if(
				(UWB_SC_MCU_STANDBY > cmd->subCommand)||
				(UWB_SC_MCU_RESET_000 < cmd->subCommand)
			)				
			{
				cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
				break;
			}
			if(cmd->subCommand>=UWB_SC_MCU_RESET)//prevent from further access. Init must be finished before doing anything else
				sys_set_init_busy(OSAL_true);
			break;//rest of procedure will be done on dequeue, after ack
		case SYS_GET_DAC_VAL:
			if(sys_is_dac_val_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			sys_get_dac_val(&cmd->prms);
			U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[1],cmd->outData[2]);
			break;
		case SYS_SET_DAC_VAL:
			sys_set_dac_val_busy(OSAL_true);
			cmd->cmdAnswer = UWB_C_ANSWER_ACK;
			break;//rest of procedure will be done on dequeue, after ack
		case SYS_GET_CAPATUNE_VAL:
			if(sys_is_capatune_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			sys_get_capatune_val(&cmd->outData[1]);
			break;
		case SYS_SET_CAPATUNE_VAL:
			sys_set_capatune_busy(OSAL_true);
			cmd->cmdAnswer = UWB_C_ANSWER_ACK;
			break;//rest of procedure will be done on dequeue, after ack
		case SYS_GET_RXFRAME_CFG:
			if(sys_is_rxframe_cfg_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			sys_get_rxframe_cfg(&cmd->outData[1]);
			break;
		case SYS_SET_RXFRAME_CFG:
			sys_set_rxframe_cfg_busy(OSAL_true);
			break;//rest of procedure will be done on dequeue, after ack
		case SYS_GET_STATUS_MSK:
			if(sys_is_statusmsk_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			sys_get_status_mask(&cmd->prms);
			U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[1],cmd->outData[2]);
			break;
		case SYS_SET_STATUS_MSK:
			sys_set_statusmsk_busy(OSAL_true);
			break;//rest of procedure will be done on dequeue, after ack
		case SYS_GET_STATUS:
			sys_get_status(&cmd->prms);
			sys_reset_status();
			U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[1],cmd->outData[2]);
			break;
		#ifdef PROD
			case SYS_PROD_LIMITS:
				sys_set_prod_busy(OSAL_true);
				cmd->additionalDataLen = cmd->prms;
				sys_prod_limits_get_buf(&cmd->additionalData,&cmd->additionalDataLen);
				break;
			case SYS_PROD_CTRL:
				if(cmd->subCommand!=UWB_SC_START)
					cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
				break;
		#else
		//case SYS_RSVDD:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case SYS_RSVDE:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		#endif
		case SYS_CHECKCACHE://check if a value from cache is ok against hw
			if(sys_is_checkcache_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			cmd->outDataLen = UWB_C_DEFAULT_REQ_SIZE;
			if(cmd->subCommand == UWB_SC_ASKCHECKCACHE)
			{
				sys_set_checkcache_busy(OSAL_true);				
			}
			else if(cmd->subCommand == UWB_SC_CHECKCACHE)
			{
				sys_get_checkcache(&cmd->outData[1]);
			}
			else cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//bad subcommand id
			break;//used to read real value on system , sucj as dac, capatune, ...and any other value
		default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //all reserved are plugged here, but are written in code, commented (gain of firm size)
	}
}

/**
 * @brief uwb_com_process_radio_command
 * MODULE ARM IMPLEMENTATION
 */
static OSAL_void uwb_com_process_radio_command(OSAL_void* contexte,uwb_com_command_t* cmd)
{
	if(sys_is_init_busy())
	{
		cmd->cmdAnswer = UWB_C_ANSWER_BUSY;	
		return;
	}
	if(radio_is_init_busy())
	{
		cmd->cmdAnswer = UWB_C_ANSWER_BUSY;	
		return;
	}
	switch (cmd->commandId)
	{
		case RADIO_INIT:		
				radio_set_init_busy(OSAL_true);
				break;//rest of procedure will be done on dequeue, after ack
		case RADIO_SPI_READ:
			if(radio_is_spiwr_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			radio_spi_read(&cmd->outData,cmd->prms,&cmd->outDataLen);
			break;
		case RADIO_SPI_WRITE://rest of procedure will be done on dequeue, after ack
			radio_set_spiwr_busy(OSAL_true);
			radio_spi_write_get_buf(&cmd->additionalData,cmd->prms);
			cmd->additionalDataLen = cmd->prms;
			break;
		case RADIO_GET_PM_STATE:
			if(radio_is_pmstate_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			radio_get_pmstate(&cmd->outData[1]);
			break;
		case RADIO_SET_PM_STATE:
			if(
				(UWB_SC_PM_3 > cmd->subCommand)||
				(UWB_SC_PM_TXRX_IDLE < cmd->subCommand)
			)				
			{
				cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
				break;
			}
			radio_set_pmstate_busy(OSAL_true);
			break;//rest of procedure will be done on dequeue, after ack
		case RADIO_GET_PHY_CFG:
			if(cmd->prms>sizeof(config_phy_t))
			{
				cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
				break;
			}
			if(radio_is_phy_cfg_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			radio_get_phy_cfg(&cmd->outData,cmd->prms);
			cmd->outDataLen = cmd->prms;
			break;
		case RADIO_SET_PHY_CFG:
			if(cmd->prms>sizeof(config_phy_t))
			{
				cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
				break;
			}
			radio_set_phy_cfg_busy(OSAL_true);
			radio_get_phy_cfg_data(&cmd->additionalData);
			cmd->additionalDataLen = cmd->prms;
			break;//rest of procedure will be done on dequeue, after ack
		case RADIO_GET_RX_CFG:
			if(radio_is_rx_cfg_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			cmd->outDataLen = 4;
			radio_get_rx_cfg(&cmd->outData[1],&cmd->prms);
			U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[2],cmd->outData[3]);
			break;
		case RADIO_SET_RX_CFG://rest of procedure will be done on dequeue, after ack
			radio_set_rx_cfg_busy(OSAL_true);
			radio_get_rx_cfg_data(&cmd->additionalData,&cmd->additionalDataLen);
			break;
		#ifdef CONFIG_GENERIC
		case RADIO_CLR_RNG_TIMER://rest of procedure will be done on dequeue, after ack
			break;
		case RADIO_WRITE_TX_DATA:
			//size is in bits
			if(cmd->prms>1024)
			{
				cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
				break;
			}
			radio_set_tx_data_buf_size(ROUND_BITS_TO_BYTES(cmd->prms));
			radio_get_tx_data_buf(&cmd->additionalData,&cmd->additionalDataLen);
			break;//rest of procedure will be done on dequeue, after ack
		case RADIO_READ_RX_DATA:
			switch (cmd->subCommand)
			{
				case UWB_SC_SIZE:
					radio_get_rx_data_size(&cmd->prms);
					U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[1],cmd->outData[2]);
				break;
				case UWB_SC_CHECKSUM:
					radio_get_rx_data_checksum(&cmd->prms);
					U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[1],cmd->outData[2]);
				break;
				case UWB_SC_DATA:
					radio_get_rx_buf(&cmd->outData,&cmd->outDataLen);
				break;
				default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//bad subcommand id
			}
			break;
		case RADIO_PHY_CTRL:
			switch (cmd->subCommand)
			{
				case UWB_SC_START:
				case UWB_SC_STOP:
					//in start, if already ongoing, change cmd->prm to zero (bypass). Otherwise a rela start will be done
					if(
						(cmd->prms!=UWB_SC_STARTSTOP_TX)&&
						(cmd->prms!=UWB_SC_STARTSTOP_RX)
					)
					{
						
						cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
						break;
					}
					radio_phy_ctrl_prepare(cmd->subCommand,&cmd->prms);
					break;//rest of procedure will be done on dequeue, after ack
				default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//bad subcommand id
			}
			break;
		#endif //#ifdef CONFIG_GENERIC

		//case RADIO_RSVDD:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case RADIO_RSVDE:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case RADIO_RSVDF:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //all reserved are plugged here, but are written in code, commented (gain of firm size)
	}
}

#ifdef CONFIG_GENERIC

/**
 * @brief uwb_com_process_seq_command
 * MODULE ARM IMPLEMENTATION
 */
static OSAL_void uwb_com_process_seq_command(OSAL_void* contexte,uwb_com_command_t* cmd)
{
	if(sys_is_init_busy())
	{
		cmd->cmdAnswer = UWB_C_ANSWER_BUSY;	
		return;
	}
    if(radio_is_init_busy())
	{
		cmd->cmdAnswer = UWB_C_ANSWER_BUSY;	
		return;
	}
	switch (cmd->commandId)
	{
		case SEQ_INIT:cmd->cmdAnswer = UWB_C_ANSWER_ACK;break;//rest of procedure will be done on dequeue, after ack
		case SEQ_GET_CFG:
			if(seq_is_cfg_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			cmd->outDataLen = 4;
			seq_get_cfg(&cmd->outData[1],&cmd->outData[2],&cmd->outData[3]);
			break;
		case SEQ_SET_CFG:
			seq_set_cfg_busy(OSAL_true);
			seq_get_cfg_buffer(&cmd->additionalData,&cmd->additionalDataLen);
			break;//rest of procedure will be done on dequeue, after ack
		case SEQ_QUEUE_SINGLE_ACT:
			seq_get_sequence_data(&cmd->additionalData,&cmd->additionalDataLen);
			break;//rest of procedure will be done on dequeue, after ack
		//case SEQ_RSVD4:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case SEQ_RSVD5:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case SEQ_RSVD6:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case SEQ_RSVD7:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case SEQ_RSVD8:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case SEQ_RSVD9:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case SEQ_RSVDA:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case SEQ_RSVDB:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		case SEQ_CTRL:
			switch (cmd->subCommand)
			{
				case UWB_SC_START:
				case UWB_SC_STOP:
					seq_ctrl_prepare(&cmd->subCommand);
					break;//rest of procedure will be done on dequeue, after ack
				default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//bad subcommand id
			}
			break;//rest of procedure will be done on dequeue, after ack
		case SEQ_RSVDD:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		case SEQ_RSVDE:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		case SEQ_RSVDF:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //all reserved are plugged here, but are written in code, commented (gain of firm size)
	}
}
#endif //#ifdef CONFIG_GENERIC

#ifdef CONFIG_RTLS_PROTOCOL
/**
 * @brief uwb_com_process_prot_command
 * MODULE ARM IMPLEMENTATION
 */
static OSAL_void uwb_com_process_prot_command(OSAL_void* contexte,uwb_com_command_t* cmd)
{
	if(sys_is_init_busy())
	{
		cmd->cmdAnswer = UWB_C_ANSWER_BUSY;	
		return;
	}
	if(radio_is_init_busy())
	{
		cmd->cmdAnswer = UWB_C_ANSWER_BUSY;	
		return;
	}
	if(prot_is_init_busy())
	{
		cmd->cmdAnswer = UWB_C_ANSWER_BUSY;	
		return;
	}
	switch (cmd->commandId)
	{
		case PROT_INIT:
			prot_set_init_busy(OSAL_true);
			break;//rest of procedure will be done on dequeue, after ack
		case PROT_GET_CAPS:
			if(prot_is_caps_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			prot_get_caps_buffer(&cmd->outData,&cmd->outDataLen);
		break;
		case PROT_SET_CAPS:
			prot_set_caps_busy(OSAL_true);
			prot_get_caps_buffer(&cmd->additionalData,&cmd->additionalDataLen);
			if(cmd->additionalData == NULL)//unsupported in live
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				prot_set_caps_busy(OSAL_false);
			}
		break;//rest of procedure will be done on dequeue, after ack
	#ifdef CONFIG_RTLS_FULL_LOC //otherwise, it 's unsupported
		case PROT_GET_SFRAME_SCH://copy (module->out) wil be done on next dma it
			if(prot_is_superframe_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			prot_get_cur_superframe_shema_buffer(&cmd->outData);
			if(cmd->outData == NULL) cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
			else cmd->outDataLen = cmd->prms;
		break;
		case PROT_SET_SFRAME_SCH://copy (out->module) wil be done on next dma it
			if(cmd->prms>sizeof(superframe_info_t))
			{
				cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
				break;
			}
			prot_set_superframe_busy(OSAL_true);
			prot_get_superframe_shema_buffer(&cmd->additionalData);
			if(cmd->additionalData == NULL)//unsupported in live
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				prot_set_superframe_busy(OSAL_false);
			}
			else
				cmd->additionalDataLen = cmd->prms;
		break;//rest of procedure will be done on next dma access, (copy into superframe)
	#endif //#ifdef CONFIG_RTLS_FULL_LOC
	#ifdef CONFIG_HAVE_STATS
		case PROT_GET_STATS:
			if(prot_is_stats_busy())//avoid non reliable returned value if a reset is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			prot_get_stats_data(&cmd->outData,&cmd->outDataLen);
			break;
		case PROT_RESET_STATS://will be done in mainloop
			prot_set_stats_busy(OSAL_true);
			break;
	#endif //#ifdef CONFIG_HAVE_STATS	
	#ifdef CONFIG_RTLS_FULL_LOC
		case PROT_GET_ARTLS_PKT:
			if(prot_is_artls_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			switch (cmd->subCommand)
			{
				case UWB_SC_SIZE:
					prot_get_artls_out_buffer(NULL,&cmd->prms);
					U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[1],cmd->outData[2]);
				break;
				case UWB_SC_CHECKSUM:
					prot_get_artls_out_data_checksum(&cmd->prms);
					U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[1],cmd->outData[2]);
				break;
				case UWB_SC_DATA:
					prot_get_artls_out_buffer(&cmd->outData,&cmd->outDataLen);
				break;
				case UWB_SC_POP:
					// pop operation will be done on in the mainloop
					prot_set_artls_busy(OSAL_true);
					break;
				default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//bad subcommand id
			}
		break;
		case PROT_SET_ARTLS_PKT:
			if(prot_is_artls_busy())//avoid non reliable returned value if a set is done in mainloop
			{
				cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
				break;
			}
			prot_set_artls_busy(OSAL_true);
			prot_get_artls_in_buffer(&cmd->additionalData,&cmd->additionalDataLen);
		break;//copy (out->module) wil be doen on next dma it
		case PROT_GET_RANGING_PKT:
			switch (cmd->subCommand)
			{
				case UWB_SC_SIZE:
					prot_get_inactiv_rxFrame_swap(NULL,&cmd->prms);
					U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[1],cmd->outData[2]);
				break;
				case UWB_SC_CHECKSUM:
					prot_get_inactiv_rxFrame_swap_checksum(&cmd->prms);
					U16VAL_TO_MSBLSB(cmd->prms,cmd->outData[1],cmd->outData[2]);
				break;
				case UWB_SC_DATA:
					prot_get_inactiv_rxFrame_swap_buf(&cmd->outData,&cmd->outDataLen);
				break;
				default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//bad subcommand id
			}
		break;
	#endif //#ifdef CONFIG_RTLS_FULL_LOC
		//case PROT_RSVDA:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case PROT_RSVDB:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//RESERVED = NOT SUPPORTED
		case PROT_CTRL:
			switch (cmd->subCommand)
			{
				case UWB_SC_START:
				case UWB_SC_STOP:
					cmd->outDataLen=1;
					break;//rest of procedure will be done on dequeue, after ack
				case UWB_SC_STATUS:{
					OSAL_u16 status=0;
					prot_ctrl_status(cmd->subCommand,&status);
					U16VAL_TO_MSBLSB(status,cmd->outData[1],cmd->outData[2]);
					cmd->outDataLen=3;}
					break;//get status asap
				case UWB_SC_STATE:{
					OSAL_u16 state=0;
					prot_ctrl_status(cmd->subCommand,&state);
					cmd->outData[1] = (OSAL_u8) state;
					cmd->outDataLen=2;}
					break;//get status asap
				default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//bad subcommand id
			}
			break;//rest of procedure will be done on dequeue, after ack
		//case PROT_RSVDD:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case PROT_RSVDE:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//RESERVED = NOT SUPPORTED
		//case PROT_RSVDF:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//RESERVED = NOT SUPPORTED
		default:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//shoudl not arrive
	}
}
#endif //#ifdef CONFIG_RTLS_PROTOCOL

/**
 * @brief uwb_com_process_command
 * MODULE ARM IMPLEMENTATION
 */
OSAL_void uwb_com_process_command(OSAL_void* contexte,uwb_com_command_t* cmd)
{
	//by default, data always point on default TX write buffer, but may change in the differnet use case
	GET_DEFAULT_EXTCOM_TXFRAME(cmd->outData);
	cmd->outDataLen = UWB_C_DEFAULT_REQ_SIZE;
	//se answer to ack by default, in cas of error, it will be modified by other sub functions
	cmd->cmdAnswer = UWB_C_ANSWER_ACK;
	#ifdef PROD
	if(sys_is_prod_busy())
		cmd->cmdAnswer = UWB_C_ANSWER_BUSY;
	else
	{
	#endif
	//check the scheme and fill data or additionnldata
	switch (cmd->domain)
	{
		case UWB_CD_SYSTEM:
			uwb_com_process_sys_command(contexte,cmd);
			break;
		case UWB_CD_RADIO:
			uwb_com_process_radio_command(contexte,cmd);
			break;
	#ifdef CONFIG_GENERIC
		case UWB_CD_SEQUENCER:
			uwb_com_process_seq_command(contexte,cmd);
			break;
	#endif //#ifdef CONFIG_GENERIC
	#ifdef CONFIG_RTLS_PROTOCOL
			case UWB_CD_PROTOCOL:
			uwb_com_process_prot_command(contexte,cmd);
			break;
	#endif //#ifdef CONFIG_RTLS_PROTOCOL
		default: //must be case UWB_CS_COMMAND_N_ACK, other have been already threated
			cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;
			break;
	}
	#ifdef PROD
	}
	#endif
	if(cmd->cmdAnswer != UWB_C_ANSWER_ACK)
	{
		//reset outdata
		GET_DEFAULT_EXTCOM_TXFRAME(cmd->outData);
		cmd->outDataLen = UWB_C_DEFAULT_REQ_SIZE;
	}
	if(cmd->outData != NULL)
		cmd->outData[0] = cmd->commandId | cmd->cmdAnswer;

}

#endif //UWB_COM_CAN_ANSWER

