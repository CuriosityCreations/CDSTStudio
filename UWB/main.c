/* FreeRTOS includes. */
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//#include "semphr.h"

#include "stm32f0xx.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_dbgmcu.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_usart.h"
#include "types.h"
#include <coreId.h>
#include "osal_type.h"
#include "osal_time.h"
#include "config_api.h"
#include "module_api.h"
#include "uwbmodule_com.h"
#include "Ext_Com.h"
#include "shell.h"

#ifdef PROD
//force shell uart in prod mode
#ifndef CONFIG_ACTIVATE_UART_SHELL
    #define CONFIG_ACTIVATE_UART_SHELL
#endif
#endif //PROD

void define_printf(void){
    //force compilo to define needed stdio routine 
    gets(NULL);
}

/**
 * HardFault_HandlerAsm:
 * Alternative Hard Fault handler to help debug the reason for a fault.
 * To use, edit the vector table to reference this function in the HardFault vector
 * This code is suitable for Cortex-M3 and Cortex-M0 cores
 */

// Use the 'naked' attribute so that C stacking is not used.
__attribute__((naked))
void HardFault_Handler(void){
        /*
         * Get the appropriate stack pointer, depending on our mode,
         * and use it as the parameter to the C handler. This function
         * will never return
         */

        __asm(  ".syntax unified\n"
                        "MOVS   R0, #4  \n"
                        "MOV    R1, LR  \n"
                        "TST    R0, R1  \n"
                        "BEQ    _MSP    \n"
                        "MRS    R0, PSP \n"
                        "B      HardFault_HandlerC      \n"
                "_MSP:  \n"
                        "MRS    R0, MSP \n"
                        "B      HardFault_HandlerC      \n"
                ".syntax divided\n") ;
}

/**
 * HardFaultHandler_C:
 * This is called from the HardFault_HandlerAsm with a pointer the Fault stack
 * as the parameter. We can then read the values from the stack and place them
 * into local variables for ease of reading.
 * We then read the various Fault Status and Address Registers to help decode
 * cause of the fault.
 * The function ends with a BKPT instruction to force control back into the debugger
 */
void HardFault_HandlerC(unsigned long *hardfault_args){
        sys_manage_hardFault(hardfault_args);
}
// ########################################################
// event management
// ########################################################

#ifdef UWB_COM_CAN_ANSWER

// process_sys_command
static OSAL_void process_sys_command(uwb_com_command_t* cmd)
{
    switch (cmd->commandId) 
    {
		//case SYS_RSVD0:break; //RESERVED = NOT SUPPORTED
		case SYS_GET_INFO:break;//procedure already done on enqueu side
		//case SYS_RSVD2:break; //RESERVED = NOT SUPPORTED
		case SYS_SET_MCU_STATE:
			sys_set_mcu_state(cmd->subCommand);
			#ifdef CONFIG_ACTIVATE_UART_SHELL
			if(
			(cmd->subCommand >= UWB_SC_MCU_RESET)&&
			(cmd->subCommand <= UWB_SC_MCU_RESET_000)
			)
			shell_prompt();
			#endif //CONFIG_ACTIVATE_UART_SHELL
			break;
		case SYS_GET_DAC_VAL:break;//procedure already done on enqueu side
		case SYS_SET_DAC_VAL:
			sys_set_dac_val(cmd->prms);
			sys_set_dac_val_busy(OSAL_false);
			break;
		case SYS_GET_CAPATUNE_VAL:break;//procedure already done on enqueu side
		case SYS_SET_CAPATUNE_VAL:
			sys_set_capatune_val(cmd->prms);
			sys_set_capatune_busy(OSAL_false);
			break;
		case SYS_GET_RXFRAME_CFG:break;//procedure already done on enqueu side
		case SYS_SET_RXFRAME_CFG:
			sys_set_rxframe_cfg(cmd->prms);
			sys_set_rxframe_cfg_busy(OSAL_false);
			break;
		case SYS_GET_STATUS_MSK:break;//procedure already done on enqueu side
		case SYS_SET_STATUS_MSK:
			sys_set_status_mask(cmd->prms);
			sys_set_statusmsk_busy(OSAL_false);
			break;
		case SYS_GET_STATUS:break;//procedure already done on enqueu side
		#ifdef PROD
		case SYS_PROD_LIMITS:
			sys_set_prod_busy(OSAL_false);
			break;
		case SYS_PROD_CTRL:
			sys_set_prod_busy(OSAL_true);//limits cannot be changed wil something running
			sys_prod_ctrl(cmd->subCommand);
			sys_set_prod_busy(OSAL_false);//limits cannot be changed wil something running
			break;
		#else
		//case SYS_RSVDD:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		//case SYS_RSVDE:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
		#endif
		case SYS_CHECKCACHE://check if a value from cache is ok against hw
			if(cmd->subCommand == UWB_SC_ASKCHECKCACHE)
			{
				sys_checkcache(cmd->prms);
				sys_set_checkcache_busy(OSAL_false);
			}
			break;
		default:
			OSAL_trace(TRACE_ERROR,"?");
			break;//should not arrive, warn, it mean gardening
    }
}

// process_radio_command
static OSAL_void process_radio_command(uwb_com_command_t* cmd)
{
    switch (cmd->commandId) 
    {
        case RADIO_INIT:radio_init();break;
        case RADIO_SPI_READ:break;//procedure already done on enqueu side
        case RADIO_SPI_WRITE:
            radio_spi_write(cmd->additionalData,cmd->additionalDataLen);
            radio_set_spiwr_busy(OSAL_false);
            break;
        case RADIO_GET_PM_STATE:break;//procedure already done on enqueu side
        case RADIO_SET_PM_STATE:
            radio_set_pmstate(cmd->subCommand);
            radio_set_pmstate_busy(OSAL_false);
            break;
        case RADIO_GET_PHY_CFG:break;//procedure already done on enqueu side
        case RADIO_SET_PHY_CFG:
            radio_set_phy_cfg(cmd->additionalData,cmd->additionalDataLen);
            radio_set_phy_cfg_busy(OSAL_false);
            break;
        case RADIO_GET_RX_CFG:break;//procedure already done on enqueu side
        case RADIO_SET_RX_CFG://Use subcommand to store rxcfg and prm to store size in bits
            cmd->subCommand = cmd->additionalData[0];
            MSBLSB_TO_U16VAL(cmd->additionalData[1],cmd->additionalData[2], cmd->prms);
            radio_set_rx_cfg(cmd->subCommand,cmd->prms);
            radio_set_rx_cfg_busy(OSAL_false);
            break;
	#ifdef CONFIG_GENERIC
        case RADIO_CLR_RNG_TIMER:radio_clear_rng_timer();break;
        case RADIO_WRITE_TX_DATA:radio_write_tx_data(NULL,cmd->prms);break;//data already got on enqueu side
        case RADIO_READ_RX_DATA:break;//procedure already done on enqueu side
        case RADIO_PHY_CTRL:radio_phy_ctrl(cmd->subCommand,cmd->prms);break;
        #endif //#ifdef CONFIG_GENERIC
        
        //case RADIO_RSVDD:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case RADIO_RSVDE:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case RADIO_RSVDF:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        default:
            OSAL_trace(TRACE_ERROR,"?");
            break;//should not arrive, warn, it mean gardening
    }
}

#ifdef CONFIG_GENERIC
// process_seq_command
static OSAL_void process_seq_command(uwb_com_command_t* cmd) {
    switch (cmd->commandId) 
    {
        case SEQ_INIT:seq_init();break;
        case SEQ_GET_CFG:break;//procedure already done on enqueu side
        case SEQ_SET_CFG:
            seq_set_cfg();
            seq_set_cfg_busy(OSAL_false);
            break;
        case SEQ_QUEUE_SINGLE_ACT:seq_queue_single_act();break;
        //case SEQ_RSVD4:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case SEQ_RSVD5:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case SEQ_RSVD6:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case SEQ_RSVD7:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case SEQ_RSVD8:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case SEQ_RSVD9:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case SEQ_RSVDA:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case SEQ_RSVDB:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        case SEQ_CTRL:seq_ctrl(cmd->subCommand);break;
        case SEQ_RSVDD:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        case SEQ_RSVDE:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        case SEQ_RSVDF:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        default:
            OSAL_trace(TRACE_ERROR,"?");
            break;//should not arrive, warn, it mean gardening
    }
}
#endif //#ifdef CONFIG_GENERIC

#ifdef CONFIG_RTLS_PROTOCOL
// process_prot_command
static OSAL_void process_prot_command(uwb_com_command_t* cmd) {
    switch (cmd->commandId) 
    {
        case PROT_INIT:prot_init();break;
        case PROT_GET_CAPS:break;//procedure already done on enqueu side
        case PROT_SET_CAPS:{
			OSAL_u32 caps=0;
			MSBLSB_TO_U32VAL( 	cmd->additionalData[0],
					cmd->additionalData[1],
					cmd->additionalData[2],
					cmd->additionalData[3],
					caps);
			prot_set_caps_busy(OSAL_true);
			prot_set_caps(caps);
			prot_set_caps_busy(OSAL_false);
			break;}
    #ifdef CONFIG_RTLS_FULL_LOC //otherwise, it 's unsupported
        case PROT_GET_SFRAME_SCH:break;//procedure already done on enqueue side
        case PROT_SET_SFRAME_SCH:
            prot_set_superframe_shema(cmd->additionalData);
            prot_set_superframe_busy(OSAL_false);
            break;
    #endif //#ifdef CONFIG_RTLS_FULL_LOC
    #ifdef CONFIG_HAVE_STATS
	case PROT_GET_STATS:break;//procedure already done on enqueu side
	case PROT_RESET_STATS:
		prot_reset_stats();
		prot_set_stats_busy(OSAL_false);
		break;
    #endif // #ifdef CONFIG_HAVE_STATS
    #ifdef CONFIG_RTLS_FULL_LOC
        case PROT_GET_ARTLS_PKT:
        	switch (cmd->subCommand)
        	{
        	case UWB_SC_POP:
        		prot_pop_artls_out_data();
			prot_set_artls_busy(OSAL_false);
        		break;
        	default:
        		//procedure already done on enqueu side
        		break;
        	}
        	break;
        case PROT_SET_ARTLS_PKT:
            prot_set_artls_in_data(cmd->additionalData,cmd->additionalDataLen);
            prot_set_artls_busy(OSAL_false);
            break;
        case PROT_GET_RANGING_PKT:break;//procedure already done on enqueu side
    #endif //#ifdef CONFIG_RTLS_FULL_LOC
        //case PROT_RSVDA:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case PROT_RSVDB:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//RESERVED = NOT SUPPORTED
        case PROT_CTRL:prot_ctrl(cmd->subCommand);break;
        //case PROT_RSVDD:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break; //RESERVED = NOT SUPPORTED
        //case PROT_RSVDE:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//RESERVED = NOT SUPPORTED
        //case PROT_RSVDF:cmd->cmdAnswer = UWB_C_ANSWER_UNSUPPORTED;break;//RESERVED = NOT SUPPORTED
        default:
            OSAL_trace(TRACE_ERROR,"?");
            break;//should not arrive, warn, it mean gardening
    }
}
#endif //#ifdef CONFIG_RTLS_PROTOCOL
        
static OSAL_void process_command(uwb_com_command_t* cmd)
{
    //check the scheme and do linked action
    switch (cmd->domain) 
    {
        case UWB_CD_SYSTEM:
            process_sys_command(cmd);
        break;
        case UWB_CD_RADIO:
            process_radio_command(cmd);
        break;
	#ifdef CONFIG_GENERIC
        case UWB_CD_SEQUENCER:
            process_seq_command(cmd);
        break;
	#endif //#ifdef CONFIG_GENERIC
        #ifdef CONFIG_RTLS_PROTOCOL
            case UWB_CD_PROTOCOL:
                process_prot_command(cmd);
            break;
        #endif //#ifdef CONFIG_RTLS_PROTOCOL
        default:
            OSAL_trace(TRACE_ERROR,"?");
            break;//should not arrive, warn, it mean gardening
    }
}

#endif //#ifdef UWB_COM_CAN_ANSWER

// ########################################################
// firmware mainloop
// ########################################################
void mainloop(void)
{
    #ifdef UWB_COM_CAN_ANSWER
    int dequeue=0;
    uwb_com_command_t cmd;
    #endif

    #ifdef CONFIG_ACTIVATE_UART_SHELL
    shell_prompt();
    #endif //CONFIG_ACTIVATE_UART_SHELL
    /****** main Loop ******/
	while(1)
	{
	#if defined(CONFIG_ACTIVATE_UART_SHELL) || defined(PROD) 
		shell_interface();
	#endif //CONFIG_ACTIVATE_UART_SHELL
		//manage event
		//dequeue waiting event
		#ifdef UWB_COM_CAN_ANSWER
		dequeue = uwb_dequeue_cmd(&cmd);
		if( dequeue < 1) //no event ? fall asleep
		{
		    sys_stop_mcu();
		}
		else
		{
		    process_command(&cmd);
		}
		#else
		sys_stop_mcu();
		#endif

		sys_process();

		#if defined(CONFIG_ACTIVATE_UART_SHELL) || defined(PROD)
			shell_process();
		#endif //CONFIG_ACTIVATE_UART_SHELL
	}
}

// ########################################################
// firmware main entry point
// ########################################################
int main(void)
{
	OSAL_u8* error=NULL;
	while(sys_init(&error) != OSAL_OK)
	{
		OSAL_trace(TRACE_ANY,"ERR:%s",error);
		OSAL_msleep(1000);
	}
	#ifdef UWB_COM_CAN_SEND
	uwb_com_init (NULL,NULL);
	uwb_com_open (NULL);
	#endif

	mainloop();
	#ifdef UWB_COM_CAN_SEND
	uwb_com_close(NULL);
	uwb_com_exit( NULL);
	#endif
	return 0;
}



