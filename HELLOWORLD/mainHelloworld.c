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
#include "module_api.h"

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
        volatile unsigned int stacked_r0 ;
        volatile unsigned int stacked_r1 ;
        volatile unsigned int stacked_r2 ;
        volatile unsigned int stacked_r3 ;
        volatile unsigned int stacked_r12 ;
        volatile unsigned int stacked_lr ;
        volatile unsigned int stacked_pc ;
        volatile unsigned int stacked_psr ;


        stacked_r0 = ((unsigned long)hardfault_args[0]) ;
        stacked_r1 = ((unsigned long)hardfault_args[1]) ;
        stacked_r2 = ((unsigned long)hardfault_args[2]) ;
        stacked_r3 = ((unsigned long)hardfault_args[3]) ;
        stacked_r12 = ((unsigned long)hardfault_args[4]) ;
        stacked_lr = ((unsigned long)hardfault_args[5]) ;
        stacked_pc = ((unsigned long)hardfault_args[6]) ;
        stacked_psr = ((unsigned long)hardfault_args[7]) ;

	printline("[Hard fault]");
	printline("R0: 0x%x", stacked_r0);
	printline("R1: 0x%x", stacked_r1);
	printline("R2: 0x%x", stacked_r2);
	printline("R3: 0x%x", stacked_r3);
	printline("R12:0x%x", stacked_r12);
	printline("LR[R14]: 0x%x  subroutine call return address", stacked_lr);
	printline("PC[R15]: 0x%x  program counter", stacked_pc);
	printline("PSR: 0x%x", stacked_psr);
	printline("BFAR:0x%x", (unsigned int)(*((volatile unsigned long *)(0xE000ED38))));
	printline("CFSR:0x%x", (unsigned int)(*((volatile unsigned long *)(0xE000ED28))));
	printline("HFSR:0x%x", (unsigned int)(*((volatile unsigned long *)(0xE000ED2C))));
	printline("DFSR:0x%x", (unsigned int)(*((volatile unsigned long *)(0xE000ED30))));
	printline("AFSR:0x%x", (unsigned int) (*((volatile unsigned long *)(0xE000ED3C))));
	printline("SCB_SHCSR:0x%x", (unsigned int) SCB->SHCSR);

        __asm("BKPT #0\r\n") ; // Break into the debugger
}

// ########################################################
// firmware main entry point
// ########################################################
int main(void)
{
    OSAL_u8* error=NULL;
    if(sys_init(&error) != OSAL_OK)
    {
        do{
            if(error)
                OSAL_trace(TRACE_ERROR,"%s",error);
            else
                OSAL_trace(TRACE_ERROR,"SYS INIT FAIL");
            OSAL_msleep(1000);
        }while(1);        
    }

    do{
        
        //PLEASE CODE ME        
        
    }while(1);

    return 0;
}



