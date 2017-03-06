/**
 * @file  uwbmodule_com.h
 * @brief  module communication definition and tools
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef UWBMODULE_COM_H
#define UWBMODULE_COM_H

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Default size of a request buffer
*/
#define UWB_C_DEFAULT_REQ_SIZE		3

/**
 * @brief UWB_C_DEFAULTRETRY_NB : Default retry count when receive a #UWB_C_ANSWER_RETRY
*/
#define UWB_C_DEFAULTRETRY_NB		5

/**
 * @brief uwb_c_dflt_retry_delay_us 
 * @note Default delays between 2 retry
*/
typedef enum
{
	UWB_C_DFLT_RETRY_DELAY1_US	=1000,	/**< user should wait for this value in case of a first retry before resending command */
	UWB_C_DFLT_RETRY_DELAY2_US	=1000,	/**< user should wait for this value in case of a second retry before resending command */
	UWB_C_DFLT_RETRY_DELAY3_US	=1000,	/**< user should wait for this value in case of a third retry before resending command */
	UWB_C_DFLT_RETRY_DELAY4_US	=1000,	/**< user should wait for this value in case of a fourth retry before resending command */
	UWB_C_DFLT_RETRY_DELAY5_US	=1000	/**< user should wait for this value in case of a fifth(and lmast) retry before resending command */
} OSAL_PACKED_ENUM uwb_c_dflt_retry_delay_us;

/**
 * @brief UWB_C_DEFAULTBUSY_RETRY_NB : Default retry count when receive a #UWB_C_ANSWER_BUSY
*/
#define UWB_C_DEFAULTBUSY_RETRY_NB 5

/**
 *  @brief uwb_c_dflt_busy_delay_ms
 *  @note Default delays between 2 retry
*/
typedef enum
{
	UWB_C_DFLT_BUSY_DELAY1_MS	=1,	/**< user should wait for this value in case of a first retry before resending command */
	UWB_C_DFLT_BUSY_DELAY2_MS	=10,	/**< user should wait for this value in case of a second retry before resending command */
	UWB_C_DFLT_BUSY_DELAY3_MS	=20,	/**< user should wait for this value in case of a third retry before resending command */
	UWB_C_DFLT_BUSY_DELAY4_MS	=30,	/**< user should wait for this value in case of a fourth retry before resending command */
	UWB_C_DFLT_BUSY_DELAY5_MS	=40	/**< user should wait for this value in case of a fifth(and lmast) retry before resending command */
} OSAL_PACKED_ENUM uwb_c_dflt_busy_delay_ms;

/**
 *  @brief uwb_com_answer_e:
 *  @note Com Scheme for linux<-> module communication
*/
typedef enum
{
	UWB_C_ANSWER_MASK	=0xC0,		/**< Mask to catch module answer  */
	UWB_C_ANSWER_RETRY	=0x0,	   	/**< module did no answer to the command yet */
	UWB_C_ANSWER_ACK	=0x40,		/**< module receive request, and enqueued the command */
	UWB_C_ANSWER_BUSY	=0x80,		/**< module receive request, but is busy. Command must be resent later */
	UWB_C_ANSWER_UNSUPPORTED=0xC0		/**< module receive request, but command is not supported */
} OSAL_PACKED_ENUM uwb_com_answer_e;

/**
 * @brief uwb_com_scheme_e
 * @note Com Scheme for linux<-> module communication
*/
typedef enum
{
	UWB_CS_COMMAND_UNKNOWN=0xFF,	/**< no scheme */
	UWB_CS_COMMAND_NONE=0,		/**< module send something unsupported */
	UWB_CS_COMMAND_N_ACK=1,		/**< module send a request and wait for an acknowledgmenent */
	UWB_CS_COMMAND_N_ANSWER,	/**< module send a request and wait for an acknowledgmenent and some data */
	UWB_CS_COMMAND_N_DATA,		/**< module send a request and then additional data, expecting an ack */
	UWB_CS_COMMAND_N_DATA_NANSWER	/**< module send a request and then additional data, expecting an ack and data*/
} OSAL_PACKED_ENUM uwb_com_scheme_e;

/**
 * @brief uwb_com_type_e
 * @note Command type for a scheme
*/
typedef enum
{
	UWB_CT_NONE = 0xFF,		/**< no command */
	UWB_CT_COMMAND=0,		/**< The command consist of a simple value */
	UWB_CT_COMMAND_WITH_SUB,	/**< The command consist of a simple value and a subcommand*/
	UWB_CT_COMMAND_WITH_SUB_N_PRM,	/**< The command consist of a simple value, a subcommand and a 8 bits parameter*/
	UWB_CT_COMMAND_WITH_8BITSPRM,	/**< The command consist of a simple value and a 8 bits parameter*/
	UWB_CT_COMMAND_WITH_16BITSPRM	/**< The command consist of a simple value and a 16 bits parameter*/
} OSAL_PACKED_ENUM uwb_com_type_e;

/**
 * @brief uwb_com_domain_e
 * @note Com Domain of command
*/
typedef enum
{
	UWB_CD_UNKNOWN=0xFF,	/**< The command isnt supported */
	UWB_CD_SYSTEM=0,	/**< The command is a system one  */
	UWB_CD_RADIO,		/**< The command is a radio one  */
	UWB_CD_SEQUENCER,	/**< The command is a sequencer one  */
	UWB_CD_PROTOCOL		/**< The command is a protocol one  */
} OSAL_PACKED_ENUM uwb_com_domain_e;

/**
 *  @brief uwb_com_command_t : to store command that need to be sent to module
 *  @note This structure is aligned and sized 20 bytes
 */
typedef struct uwb_command_type
{
	OSAL_u8*		additionalData;		/**< pointer to additional data (scheme #UWB_CS_COMMAND_N_DATA only)*/
	OSAL_u8*		outData;		/**< pointer to output data that will be filled(scheme #UWB_CS_COMMAND_N_ANSWER only)*/
	OSAL_u16		additionalDataLen;	/**< size of additionnal data (scheme #UWB_CS_COMMAND_N_DATA only)*/
	OSAL_u16		outDataLen;		/**< size of output data (scheme #UWB_CS_COMMAND_N_DATA only)*/
	OSAL_u8			commandId;		/**< Value of request, depending on type of command*/
	OSAL_u8			subCommand;		/**< sub Command*/
	OSAL_u16		prms;			/**< 8bits or 16 bits parameters*/
	uwb_com_domain_e 	domain;			/**< Domain for this command */
	uwb_com_scheme_e 	scheme;			/**< Scheme for this command */
	uwb_com_type_e   	cmdType;		/**< Type of this command */
	uwb_com_answer_e 	cmdAnswer;		/**< indicate the command status once processed (answer only)*/
} uwb_com_command_t;

/**
 * @brief uwb_com_get_command_info
 * @note create a command filled with sheme and cmdType, to have info on command
 * @note you cannot call #uwb_com_send_command with cmd after thhis fucntion, it will failed
 * @param cmd [out] a structure to fill with info on this command
 * @param commandId [in] command to send to module (uwb_com_system_cmd_e,uwb_com_radio_cmd_e,uwb_com_seq_cmd_e or uwb_com_protocol_cmd_e)
 * @return the name of the command
 */
const OSAL_u8* uwb_com_get_command_info(uwb_com_command_t* cmd,OSAL_u8   commandId);

/**
 * @brief uwb_com_create_command
 * @note create a command that cna be sent to module
 * @note this function only fill structure uwb_com_command_t depending on command,
 * @note You can then call #uwb_com_send_command
 * @note but for a direct transmission, you'de better use #uwb_com_create_and_send_command
 * @param cmd [out] a structure to fill with info on this command
 * @param commandId [in] command to send to module (uwb_com_system_cmd_e,uwb_com_radio_cmd_e,uwb_com_seq_cmd_e or uwb_com_protocol_cmd_e)
 * @param subCommand [in] sub command needed to precise action
 * @param parameter [in] parameter for this command (can be 8bits or 16bits)
 * @param additionalData [in] Additionnal Data to sent to the module (command that have a #UWB_CS_COMMAND_N_DATA scheme only)
 * @param additionalDataLen [in] Additionnal Data length
 * @param outData [out] data sent by the module ( commands that have a #UWB_CS_COMMAND_N_ANSWER scheme only)
 * @param outDataLen [out] data sent length
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given(error in code), #OSAL_ENOMEM if ask for unsupported cmd
 */
OSAL_error_t uwb_com_create_command(	uwb_com_command_t* cmd,
					OSAL_u8   commandId,
					OSAL_u8   subCommand,
					OSAL_u32  parameter,
					OSAL_u8*  additionalData,
					OSAL_s32  additionalDataLen,
					OSAL_u8*  outData,
					OSAL_s32  outDataLen);


/**
 * @brief uwb_com_send_command
 * @note sent a command to module
 * @param contexte [in] specific contexte used for transmission
 * @param cmd [in] a structure previously created by #uwb_com_create_command
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given(error in code), #OSAL_EAGAIN if communication with module failed
 */
OSAL_error_t uwb_com_send_command(  	OSAL_void *contexte,
					uwb_com_command_t* cmd);

/**
 * @brief uwb_com_create_and_send_command
 * @note create a sent a request to module
 * @param contexte [in] specific contexte used for transmission
 * @param commandId [in] command to send to module (uwb_com_system_cmd_e,uwb_com_radio_cmd_e,uwb_com_seq_cmd_e or uwb_com_protocol_cmd_e)
 * @param subCommand [in] sub command needed to precise action
 * @param parameter [in] parameter for this command (can be 8bits or 16bits)
 * @param additionalData [in] Additionnal Data to sent to the module (command that have a #UWB_CS_COMMAND_N_DATA scheme only)
 * @param additionalDataLen [in] Additionnal Data length
 * @param outData [out] data sent by the module ( commands that have a #UWB_CS_COMMAND_N_ANSWER scheme only)
 * @param outDataLen [out] data sent length
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given(error in code), #OSAL_ENOMEM if ask for unsupported cmd, #OSAL_EAGAIN if communication with module failed
 */
OSAL_error_t uwb_com_create_and_send_command(	OSAL_void *contexte,
						OSAL_u8   commandId,
						OSAL_u8   subCommand,
						OSAL_u32  parameter,
						OSAL_u8*  additionalData,
						OSAL_u32  additionalDataLen,
						OSAL_u8*  outData,
						OSAL_u32  outDataLen);

/**
 * @brief uwb_com_analyze_gotcommand
 * @note analyze a command that sent to module
 * @note this function only fill structure uwb_com_command_t depending on command,
 * @note a call to #uwb_com_answer_gotcommand is neede after this
 * @note but for a direct transmission, you'de better use #uwb_com_analyze_and_answer_gotcommand
 * @param cmd [out] a structure to fill with info on this command
 * @param data_spi_in [in] buffer containing the 3 received data
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given(error in code), #OSAL_ENOMEM if ask for unsupported cmd
 */
OSAL_error_t uwb_com_analyze_gotcommand(	uwb_com_command_t* cmd,
						OSAL_u8 data_spi_in[3]);

/**
 * @brief uwb_com_answer_gotcommand
 * @note answer a to an incoming request
 * @param contexte [in] specific contexte used for transmission
 * @param cmd [in] a structure previously created by #uwb_com_create_command
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given(error in code), #OSAL_ENOMEM if ask for unsupported cmd, #OSAL_EAGAIN if communication with module failed
*/
OSAL_error_t uwb_com_answer_gotcommand(		OSAL_void *contexte,
						uwb_com_command_t* cmd );

/**
 * @brief uwb_com_analyze_and_answer_gotcommand
 * @note analyze and answer to a command sent to module
 * @param contexte [in] specific contexte used for transmission
 * @param data_spi_in [in] buffer containing the 3 received data
 * @return #OSAL_OK on success, #OSAL_ERROR if bad parameters given(error in code), #OSAL_ENOMEM if ask for unsupported cmd
 */
OSAL_error_t uwb_com_analyze_and_answer_gotcommand(	OSAL_void *contexte,
							OSAL_u8 data_spi_in[3]);

/**
 * @brief uwb_enqueue_cmd
 * @note enqueue a command in a fifo, to let it be runned later
 * @param cmd [in] a structure to enqueue in a fifo of cmd
 * @return nb of cmd enqueued (i.e. 1). 0 mean command has not be enqueued. -1, that fifo overflowed
 */
OSAL_s32 uwb_enqueue_cmd(uwb_com_command_t* cmd);

/**
 * @brief uwb_dequeue_cmd
 * @note dequeue a command previously enqueued by #uwb_enqueue_cmd to run it asap
 * @param cmd [in/out] copy the the first cmd in fifo into cmd
 * @return nb of cmd dequeued, 0 mean nothing left on fifo
 */
OSAL_s32 uwb_dequeue_cmd(uwb_com_command_t* cmd);

/**
 * @brief uwb_get_queue_info
 * @note return info on queu ( nb of item in queue, nb of free place in, queu, max number of item that can be stored in queue)
 * @param usedItemInQueue [out] nb of left place in queue ( 0 if not free).Not filled if NULL
 * @param freeItemInQueue [out] nb of left place in queue ( 0 if not free).Not filled if NULL
 * @param maxItemInqueue [out] max number of item in queue: = freeItemInQueue+usedItemInQueue.Not filled if NULL
 */
OSAL_void uwb_get_queue_info(OSAL_s32* usedItemInQueue, OSAL_s32* freeItemInQueue, OSAL_s32* maxItemInqueue);

/**
 * @brief uwb_is_queue_full
 * @note return info on queu ( nb of item in queue, nb of free place in queue, max number of item that can be stored in queue)
 * @param freeItemInQueue [out] nb of left place in queue ( 0 if not free).
 */
OSAL_void uwb_is_queue_full(OSAL_s32* freeItemInQueue);


/**
 * @brief uwb_clean_queue
 * @note clean the queue, by removing a given number of item
 * @param maxItemInqueue [out] nb item left in queue after clean.should be max number of item
 */
OSAL_void uwb_clean_queue(OSAL_s32* maxItemInqueue);

/**
 * @brief uwb_com_printpckt
 * @note print packet ( debug purpose)
 * @note this function must be implemented regarding the running os, environment, etc..
 * @note it will output on screen, uart, printlk... the 3 first byte of a RX/TX/DATA buffer
 * @param desc [in] description of the buffer for a clear trace
 * @param buf [in] RX/TX/DATA buffer to show
 */
OSAL_void uwb_com_printpckt(	const OSAL_u8* desc,
				OSAL_u8* buf);

/**
 * @brief uwb_com_print
 * @note print trace ( debug purpose)
 * @note this function must be implemented regarding the running os, environment, etc..
 * @note it will output on screen, uart, printk... string in a fprintf like
 * @note due to real time, this function should be linked to void as default.
 * @param dbgLevel [in] debug level of the buffer for a clear trace
 * @param fmt [in] fprintf fmt
 * @param ... variable arguments
 */
OSAL_void uwb_com_print(	int dbgLevel,
				const OSAL_u8* fmt, ...);

/**
 * @brief uwb_com_addDelay
 * @note Extra delay called between the two step of a spi request.
 *  @note this function must be implemented regarding the running os, environment, etc..
 * @param delayInUs [in] delay in micro second to apply. If <0, a default delay ( implementation specific) will be applied
 */
OSAL_void uwb_com_addDelay(OSAL_s32 delayInUs);

/**
 * @brief uwb_com_addPostDelay
 * @note Extra delay called after a complete spi request has ben done
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param delayInUs [in] post delay in micro second  to apply.  If <0, a default post delay ( implementation specific) will be applied */
OSAL_void uwb_com_addPostDelay(OSAL_s32 delayInUs);

/**
 * @brief uwb_com_addRetryDelay
 * @note delay implementation between 2 retry
 * @note this function must be implemented regarding the running os, environment, etc..
 */
OSAL_void uwb_com_addRetryDelay(uwb_c_dflt_retry_delay_us delay);

/**
 * @brief uwb_com_addBusyDelay
 * @note delay implementation between 2 retry, in case of busy
 * @note this function must be implemented regarding the running os, environment, etc..
 */
OSAL_void uwb_com_addBusyDelay(uwb_c_dflt_busy_delay_ms delay);

/**
 * @brief uwb_com_open
 * @note open an instance for spi transfer
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 * @return 0 on success, negative value otherwise
 */
OSAL_s32  uwb_com_open (OSAL_void* contexte);

/**
 * @brief uwb_com_close
 * @note close an instance for spi transfer
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 */
OSAL_void  uwb_com_close (OSAL_void* contexte);

/**
 * @brief uwb_com_init
 * @note initialize hardware for preparing uwbmodule com
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 * @param driver [out] if passed, pointer on specific driver
 * @return 0 on success, negative value otherwise
 */
OSAL_s32  uwb_com_init (OSAL_void* contexte,OSAL_void** driver);

/**
 * @brief uwb_com_exit
 * @note de-initialize hardware, must be called last
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 */
OSAL_void  uwb_com_exit (OSAL_void* contexte);

/**
 * @brief uwb_com_lock
 * @note lock the spi transfer, to avoid multiple spi transfer
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 * @return 0 on success, negative value otherwise
 */
OSAL_s32 uwb_com_lock(OSAL_void* contexte);

/**
 * @brief uwb_com_unlock
 * @note unlock the spi transfer, to let further spi transfer run
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 */
OSAL_void uwb_com_unlock(OSAL_void* contexte);

/**
 * @brief uwb_com_checkModuleIT
 * @note check if IT of the module is down. This allow to that ther wasn't any other IT coming while we were processing the previous one
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 * @return  0 Line is down ( IT available), 1, line is up ( no more IT)
 */
OSAL_u8 uwb_com_checkModuleIT(OSAL_void* contexte);

/**
 * @brief uwb_com_ModuleSETPIN
 * @note set module's a defined pin/gpio to another state
 * @note this function must be implemented regarding the running os, environment, etc...IT may be not needed, so must return 0
 * @param contexte [in] specific contexte used for transmission
 * @param pinID [in] id of the pin. specific implementation dependant
 * @param val [in] new value to set. Ignored if getOrSet is 0
 * @return nesw value of PIN(0 or 1), -1 on error
 */
OSAL_u8 uwb_com_ModuleSETPIN(OSAL_void* contexte, OSAL_u8 pinID, OSAL_u8 val);

/**
 * @brief uwb_com_ModuleGETPIN
 * @note get a module's pin/gpio state
 * @note this function must be implemented regarding the running os, environment, etc...IT may be not needed, so must return 0
 * @param contexte [in] specific contexte used for transmission
 * @param pinID [in] id of the pin. specific implementation dependant
 * @return nesw value of PIN(0 or 1), -1 on error
 */
OSAL_u8 uwb_com_ModuleGETPIN(OSAL_void* contexte, OSAL_u8 pinID);

/**
 * @brief uwb_com_spi_send
 * @note sent data through spi
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 * @param tx_buff [in] buffer to transmit
 * @param in_len [in] len of buffer to transmit
 * @return 0 on success, negative value otherwise
 */
OSAL_s32 uwb_com_spi_send(	OSAL_void* contexte,
				OSAL_u8	*tx_buff,
				OSAL_u16   in_len);
/**
 * @brief uwb_com_spi_receive
 * @note receive data through spi
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 * @param acknowledge [out] value of acknowledge ( should be cmd + 0x40)
 * @param rx_buff [out] buffer used to save received data
 * @param out_len [in] len of data to receive
 * @return 0 on success, negative value otherwise
 */
OSAL_s32 uwb_com_spi_receive(	OSAL_void* contexte,
				OSAL_u8	*acknowledge,
				OSAL_u8	*rx_buff,
				OSAL_u16   out_len);

/**
 * @brief uwb_com_spi_sendExtra
 * @note sent Extra data through spi and receive answer /Data at the same time
 * @note this function must be implemented regarding the running os, environment, etc..
 * @param contexte [in] specific contexte used for transmission
 * @param acknowledge [out] value of acknowledge ( should be cmd + 0x40)
 * @param rx_buff [out] buffer used to save received data
 * @param out_len [in] len of data to receive
 * @param tx_buff [in] buffer to transmit
 * @param in_len [in] len of buffer to transmit
 * @return 0 on success, negative value otherwise
 */
OSAL_s32 uwb_com_spi_sendExtra(	OSAL_void* contexte,
				OSAL_u8	*acknowledge,
				OSAL_u8	*rx_buff,
				OSAL_u16   out_len,
				OSAL_u8	*tx_buff,
				OSAL_u16   in_len);
/**
 * @brief uwb_com_process_command
 * @note process the received command.
 * @note this function must be implemented regarding the running os, environment, etc..
 * @note this is called by #uwb_com_analyze_gotcommand or #uwb_com_analyze_and_answer_gotcommand to manage event contined in got command
 * @note it should not be called from external
 * @note in case we receive a request of type #UWB_CS_COMMAND_N_ANSWER, fill cmd->outData and len of uwb_com_command_t with right address and size
 * @note in case we receive a request of type #UWB_CS_COMMAND_N_DATA, fill cmd->additionalData and len of uwb_com_command_t with right address and size
 * @note this function must fill cmd->cmdAnswer with the right enum
 * @param contexte [in] specific contexte used for transmission
 * @param cmd [in] a uwb_com_command_t structure, completely filled.
 */
OSAL_void uwb_com_process_command(OSAL_void* contexte,uwb_com_command_t* cmd);

#ifdef __cplusplus
}
#endif
#endif //#define UWBMODULE_COM_H

