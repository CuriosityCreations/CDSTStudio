/**
 * @file GPIO.c
 * @brief GPIO.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Includes ------------------------------------------------------------------*/
#include "HAL_config.h"

#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_tim.h"
#include "GPIO.h"
#include "USART.h"
#include "Ext_Com.h"
#include "module_api.h"

#pragma GCC optimize("Os")

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Global variables */
static void do_gpioconfigure(	GPIO_TypeDef* gpiox,
				uint16_t pin_nb,
				uint8_t pin_src,
				GPIOMode_TypeDef pin_mode,
				GPIOOType_TypeDef pin_otype,
				GPIOSpeed_TypeDef pin_speed,
				GPIOPuPd_TypeDef pin_pupd,
				uint8_t pin_af)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = pin_nb;
	GPIO_InitStructure.GPIO_Mode = pin_mode;
	GPIO_InitStructure.GPIO_OType = pin_otype;
	GPIO_InitStructure.GPIO_Speed = pin_speed;
	GPIO_InitStructure.GPIO_PuPd = pin_pupd;
	GPIO_Init(gpiox, &GPIO_InitStructure);

	if(pin_mode == GPIO_Mode_AF)
		GPIO_PinAFConfig(gpiox, pin_src, pin_af);
	else if(pin_mode == GPIO_Mode_OUT)
		GPIO_SetBits(gpiox, pin_nb);	// set Reset to 1
}

// in code call GPIOCONFIGURE(X,N,GPIOX_N_CONF)
#define GPIOCONFIGURE(_iogroup,_ionb,confArg) \
	do_gpioconfigure(GPIO##_iogroup,GPIO_Pin_##_ionb,GPIO_PinSource##_ionb,confArg)


void HAL_GPIO_DeInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Get default GPIO configuration
	GPIO_StructInit(&GPIO_InitStructure);
	
	// Enable GPIOs clocks to configure them
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | 
		RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOF, ENABLE);
	
	// Configuration
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	// Disable GPIOs clocks
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | 
		RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOF, DISABLE);
}

void HAL_GPIO_Init (void)
{
#ifdef USE_GPIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	#ifdef GPIOA_0_CONF
	GPIOCONFIGURE(A,0,GPIOA_0_CONF);
	#endif
	#ifdef GPIOA_1_CONF
	GPIOCONFIGURE(A,1,GPIOA_1_CONF);
	#endif
	#ifdef GPIOA_2_CONF
	#ifdef USE_ACCELERO
	if(USE_SHELL_USART_NB&0x2)
		GPIOCONFIGURE(A,2,GPIOA_2_CONF_SHELL);
	else
	#endif
		GPIOCONFIGURE(A,2,GPIOA_2_CONF);
	#endif
	#ifdef GPIOA_3_CONF
	GPIOCONFIGURE(A,3,GPIOA_3_CONF);
	#endif
	#ifdef GPIOA_4_CONF
	GPIOCONFIGURE(A,4,GPIOA_4_CONF);
	#endif
	#ifdef GPIOA_5_CONF
	GPIOCONFIGURE(A,5,GPIOA_5_CONF);
	#endif
	#ifdef GPIOA_6_CONF
	GPIOCONFIGURE(A,6,GPIOA_6_CONF);
	#endif
	#ifdef GPIOA_7_CONF
	GPIOCONFIGURE(A,7,GPIOA_7_CONF);
	#endif
	#ifdef GPIOA_8_CONF
	GPIOCONFIGURE(A,8,GPIOA_8_CONF);
	#endif
	#ifdef GPIOA_9_CONF
	GPIOCONFIGURE(A,9,GPIOA_9_CONF);
	#endif
	#ifdef GPIOA_10_CONF
	GPIOCONFIGURE(A,10,GPIOA_10_CONF);
	#endif
	#ifdef GPIOA_11_CONF
	GPIOCONFIGURE(A,11,GPIOA_11_CONF);
	#endif
	#ifdef GPIOA_12_CONF
	GPIOCONFIGURE(A,12,GPIOA_12_CONF);
	#endif
	#ifdef GPIOA_13_CONF
	GPIOCONFIGURE(A,13,GPIOA_13_CONF);
	#endif
	#ifdef GPIOA_14_CONF
	GPIOCONFIGURE(A,14,GPIOA_14_CONF);
	#endif
	#ifdef GPIOA_15_CONF
	GPIOCONFIGURE(A,15,GPIOA_15_CONF);
	#endif
#endif //#ifdef USE_GPIOA

#ifdef USE_GPIOB
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	#ifdef GPIOB_0_CONF
	GPIOCONFIGURE(B,0,GPIOB_0_CONF);
	#endif
	#ifdef GPIOB_1_CONF
	GPIOCONFIGURE(B,1,GPIOB_1_CONF);
	#endif
	#ifdef GPIOB_2_CONF
	GPIOCONFIGURE(B,2,GPIOB_2_CONF);
	#endif
	#ifdef GPIOB_3_CONF
	GPIOCONFIGURE(B,3,GPIOB_3_CONF);
	#endif
	#ifdef GPIOB_4_CONF
	GPIOCONFIGURE(B,4,GPIOB_4_CONF);
	#endif
	#ifdef GPIOB_5_CONF
	GPIOCONFIGURE(B,5,GPIOB_5_CONF);
	#endif
	#ifdef GPIOB_6_CONF
	//GPIO 6 AND 7 ARE SPECIFIC
	//IF USART1 USED FOR SHELL, use GPIOB_6_CONF_SHELL	
	//OTHERWISE, use GPIOB_6_CONF
	if(USE_SHELL_USART_NB&0x1)
		GPIOCONFIGURE(B,6,GPIOB_6_CONF_SHELL);
	else
		GPIOCONFIGURE(B,6,GPIOB_6_CONF);
	#endif
	#ifdef GPIOB_7_CONF
	if(USE_SHELL_USART_NB&0x1)
		GPIOCONFIGURE(B,7,GPIOB_7_CONF_SHELL);
	else
		GPIOCONFIGURE(B,7,GPIOB_7_CONF);
	#endif
	#ifdef GPIOB_8_CONF
	GPIOCONFIGURE(B,8,GPIOB_8_CONF);
	#endif
	#ifdef GPIOB_9_CONF
	GPIOCONFIGURE(B,9,GPIOB_9_CONF);
	#endif
	#ifdef GPIOB_10_CONF
	GPIOCONFIGURE(B,10,GPIOB_10_CONF);
	#endif
	#ifdef GPIOB_11_CONF
	GPIOCONFIGURE(B,11,GPIOB_11_CONF);
	#endif
	#ifdef GPIOB_12_CONF
	GPIOCONFIGURE(B,12,GPIOB_12_CONF);
	#endif
	#ifdef GPIOB_13_CONF
	GPIOCONFIGURE(B,13,GPIOB_13_CONF);
	#endif
	#ifdef GPIOB_14_CONF
	GPIOCONFIGURE(B,14,GPIOB_14_CONF);
	#endif
	#ifdef GPIOB_15_CONF
	GPIOCONFIGURE(B,15,GPIOB_15_CONF);
	#endif
#endif //#ifdef USE_GPIOB

	
#ifdef USE_GPIOC
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	#ifdef GPIOC_0_CONF
	GPIOCONFIGURE(C,0,GPIOC_0_CONF);
	#endif
	#ifdef GPIOC_1_CONF
	GPIOCONFIGURE(C,1,GPIOC_1_CONF);
	#endif
	#ifdef GPIOC_2_CONF
	GPIOCONFIGURE(C,2,GPIOC_2_CONF);
	#endif
	#ifdef GPIOC_3_CONF
	GPIOCONFIGURE(C,3,GPIOC_3_CONF);
	#endif
	#ifdef GPIOC_4_CONF
	GPIOCONFIGURE(C,4,GPIOC_4_CONF);
	#endif
	#ifdef GPIOC_5_CONF
	GPIOCONFIGURE(C,5,GPIOC_5_CONF);
	#endif
	#ifdef GPIOC_6_CONF
	GPIOCONFIGURE(C,6,GPIOC_6_CONF);
	#endif
	#ifdef GPIOC_7_CONF
	GPIOCONFIGURE(C,7,GPIOC_7_CONF);
	#endif
	#ifdef GPIOC_8_CONF
	GPIOCONFIGURE(C,8,GPIOC_8_CONF);
	#endif
	#ifdef GPIOC_9_CONF
	GPIOCONFIGURE(C,9,GPIOC_9_CONF);
	#endif
	#ifdef GPIOC_10_CONF
	GPIOCONFIGURE(C,10,GPIOC_10_CONF);
	#endif
	#ifdef GPIOC_11_CONF
	GPIOCONFIGURE(C,11,GPIOC_11_CONF);
	#endif
	#ifdef GPIOC_12_CONF
	GPIOCONFIGURE(C,12,GPIOC_12_CONF);
	#endif
	#ifdef GPIOC_13_CONF
	GPIOCONFIGURE(C,13,GPIOC_13_CONF);
	#endif
	#ifdef GPIOC_14_CONF
	GPIOCONFIGURE(C,14,GPIOC_14_CONF);
	#endif
	#ifdef GPIOC_15_CONF
	GPIOCONFIGURE(C,15,GPIOC_15_CONF);
	#endif
#endif //#ifdef USE_GPIOC
	
#ifdef USE_GPIOD
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	#ifdef GPIOD_0_CONF
	GPIOCONFIGURE(D,0,GPIOD_0_CONF);
	#endif
	#ifdef GPIOD_1_CONF
	GPIOCONFIGURE(D,1,GPIOD_1_CONF);
	#endif
	#ifdef GPIOD_2_CONF
	GPIOCONFIGURE(D,2,GPIOD_2_CONF);
	#endif
	#ifdef GPIOD_3_CONF
	GPIOCONFIGURE(D,3,GPIOD_3_CONF);
	#endif
	#ifdef GPIOD_4_CONF
	GPIOCONFIGURE(D,4,GPIOD_4_CONF);
	#endif
	#ifdef GPIOD_5_CONF
	GPIOCONFIGURE(D,5,GPIOD_5_CONF);
	#endif
	#ifdef GPIOD_6_CONF
	GPIOCONFIGURE(D,6,GPIOD_6_CONF);
	#endif
	#ifdef GPIOD_7_CONF
	GPIOCONFIGURE(D,7,GPIOD_7_CONF);
	#endif
	#ifdef GPIOD_8_CONF
	GPIOCONFIGURE(D,8,GPIOD_8_CONF);
	#endif
	#ifdef GPIOD_9_CONF
	GPIOCONFIGURE(D,9,GPIOD_9_CONF);
	#endif
	#ifdef GPIOD_10_CONF
	GPIOCONFIGURE(D,10,GPIOD_10_CONF);
	#endif
	#ifdef GPIOD_11_CONF
	GPIOCONFIGURE(D,11,GPIOD_11_CONF);
	#endif
	#ifdef GPIOD_12_CONF
	GPIOCONFIGURE(D,12,GPIOD_12_CONF);
	#endif
	#ifdef GPIOD_13_CONF
	GPIOCONFIGURE(D,13,GPIOD_13_CONF);
	#endif
	#ifdef GPIOD_14_CONF
	GPIOCONFIGURE(D,14,GPIOD_14_CONF);
	#endif
	#ifdef GPIOD_15_CONF
	GPIOCONFIGURE(D,15,GPIOD_15_CONF);
	#endif
#endif //#ifdef USE_GPIOD

	
#ifdef USE_GPIOE
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
	#ifdef GPIOE_0_CONF
	GPIOCONFIGURE(E,0,GPIOE_0_CONF);
	#endif
	#ifdef GPIOE_1_CONF
	GPIOCONFIGURE(E,1,GPIOE_1_CONF);
	#endif
	#ifdef GPIOE_2_CONF
	GPIOCONFIGURE(E,2,GPIOE_2_CONF);
	#endif
	#ifdef GPIOE_3_CONF
	GPIOCONFIGURE(E,3,GPIOE_3_CONF);
	#endif
	#ifdef GPIOE_4_CONF
	GPIOCONFIGURE(E,4,GPIOE_4_CONF);
	#endif
	#ifdef GPIOE_5_CONF
	GPIOCONFIGURE(E,5,GPIOE_5_CONF);
	#endif
	#ifdef GPIOE_6_CONF
	GPIOCONFIGURE(E,6,GPIOE_6_CONF);
	#endif
	#ifdef GPIOE_7_CONF
	GPIOCONFIGURE(E,7,GPIOE_7_CONF);
	#endif
	#ifdef GPIOE_8_CONF
	GPIOCONFIGURE(E,8,GPIOE_8_CONF);
	#endif
	#ifdef GPIOE_9_CONF
	GPIOCONFIGURE(E,9,GPIOE_9_CONF);
	#endif
	#ifdef GPIOE_10_CONF
	GPIOCONFIGURE(E,10,GPIOE_10_CONF);
	#endif
	#ifdef GPIOE_11_CONF
	GPIOCONFIGURE(E,11,GPIOE_11_CONF);
	#endif
	#ifdef GPIOE_12_CONF
	GPIOCONFIGURE(E,12,GPIOE_12_CONF);
	#endif
	#ifdef GPIOE_13_CONF
	GPIOCONFIGURE(E,13,GPIOE_13_CONF);
	#endif
	#ifdef GPIOE_14_CONF
	GPIOCONFIGURE(E,14,GPIOE_14_CONF);
	#endif
	#ifdef GPIOE_15_CONF
	GPIOCONFIGURE(E,15,GPIOE_15_CONF);
	#endif
#endif //#ifdef USE_GPIOE

	
#ifdef USE_GPIOF
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	#ifdef GPIOF_0_CONF
	GPIOCONFIGURE(F,0,GPIOF_0_CONF);
	#endif
	#ifdef GPIOF_1_CONF
	GPIOCONFIGURE(F,1,GPIOF_1_CONF);
	#endif
	#ifdef GPIOF_2_CONF
	GPIOCONFIGURE(F,2,GPIOF_2_CONF);
	#endif
	#ifdef GPIOF_3_CONF
	GPIOCONFIGURE(F,3,GPIOF_3_CONF);
	#endif
	#ifdef GPIOF_4_CONF
	GPIOCONFIGURE(F,4,GPIOF_4_CONF);
	#endif
	#ifdef GPIOF_5_CONF
	GPIOCONFIGURE(F,5,GPIOF_5_CONF);
	#endif
	#ifdef GPIOA_6_CONF
	GPIOCONFIGURE(F,6,GPIOF_6_CONF);
	#endif
	#ifdef GPIOF_7_CONF
	GPIOCONFIGURE(F,7,GPIOF_7_CONF);
	#endif
	#ifdef GPIOF_8_CONF
	GPIOCONFIGURE(F,8,GPIOF_8_CONF);
	#endif
	#ifdef GPIOF_9_CONF
	GPIOCONFIGURE(F,9,GPIOF_9_CONF);
	#endif
	#ifdef GPIOF_10_CONF
	GPIOCONFIGURE(F,10,GPIOF_10_CONF);
	#endif

#endif //#ifdef USE_GPIOF
}


/* Global variables */
static void do_gpioshow(	char* gpiox,
				uint16_t pin_nb,
				GPIOMode_TypeDef pin_mode,
				GPIOOType_TypeDef pin_otype,
				GPIOSpeed_TypeDef pin_speed,
				GPIOPuPd_TypeDef pin_pupd,
				uint8_t pin_af)
{
	OSAL_trace(TRACE_INFO,"-Pin%c%d settings:",gpiox[0],pin_nb);
	OSAL_trace(TRACE_INFO," %s",(pin_mode==GPIO_Mode_IN)?"IN":
				(pin_mode==GPIO_Mode_OUT)?"OUT":
				(pin_mode==GPIO_Mode_AF)?"AF":"AN");
	OSAL_trace(TRACE_INFO," %s",(pin_otype==GPIO_OType_PP)?"PP":"OD");
	OSAL_trace(TRACE_INFO," %s",(pin_speed==GPIO_Speed_Level_1)?"1(2MHz)":
				(pin_speed==GPIO_Speed_Level_2)?"2(10MHz)":"3(50MHz)");
	OSAL_trace(TRACE_INFO," %s",(pin_pupd==GPIO_PuPd_NOPULL)?"nopull":
				(pin_pupd==GPIO_PuPd_UP)?"up":"down");
	if(pin_mode == GPIO_Mode_AF)
		OSAL_trace(TRACE_INFO," alternate=%d",pin_af);
	
}

// in code call GPIOCONFIGURE(X,N,GPIOX_N_CONF)
#define GPIOSHOW(_iogroup,_ionb,confArg) \
	do_gpioshow(#_iogroup,_ionb,confArg)




void HAL_GPIO_Show(void)
{
	
#ifdef USE_GPIOA
	#ifdef GPIOA_0_CONF
	GPIOSHOW(A,0,GPIOA_0_CONF);
	#endif
	#ifdef GPIOA_1_CONF
	GPIOSHOW(A,1,GPIOA_1_CONF);
	#endif
	#ifdef GPIOA_2_CONF
	GPIOSHOW(A,2,GPIOA_2_CONF);
	#endif
	#ifdef GPIOA_3_CONF
	GPIOSHOW(A,3,GPIOA_3_CONF);
	#endif
	#ifdef GPIOA_4_CONF
	GPIOSHOW(A,4,GPIOA_4_CONF);
	#endif
	#ifdef GPIOA_5_CONF
	GPIOSHOW(A,5,GPIOA_5_CONF);
	#endif
	#ifdef GPIOA_6_CONF
	GPIOSHOW(A,6,GPIOA_6_CONF);
	#endif
	#ifdef GPIOA_7_CONF
	GPIOSHOW(A,7,GPIOA_7_CONF);
	#endif
	#ifdef GPIOA_8_CONF
	GPIOSHOW(A,8,GPIOA_8_CONF);
	#endif
	#ifdef GPIOA_9_CONF
	GPIOSHOW(A,9,GPIOA_9_CONF);
	#endif
	#ifdef GPIOA_10_CONF
	GPIOSHOW(A,10,GPIOA_10_CONF);
	#endif
	#ifdef GPIOA_11_CONF
	GPIOSHOW(A,11,GPIOA_11_CONF);
	#endif
	#ifdef GPIOA_12_CONF
	GPIOSHOW(A,12,GPIOA_12_CONF);
	#endif
	#ifdef GPIOA_13_CONF
	GPIOSHOW(A,13,GPIOA_13_CONF);
	#endif
	#ifdef GPIOA_14_CONF
	GPIOSHOW(A,14,GPIOA_14_CONF);
	#endif
	#ifdef GPIOA_15_CONF
	GPIOSHOW(A,15,GPIOA_15_CONF);
	#endif
#endif //#ifdef USE_GPIOA

#ifdef USE_GPIOB
	#ifdef GPIOB_0_CONF
	GPIOSHOW(B,0,GPIOB_0_CONF);
	#endif
	#ifdef GPIOB_1_CONF
	GPIOSHOW(B,1,GPIOB_1_CONF);
	#endif
	#ifdef GPIOB_2_CONF
	GPIOSHOW(B,2,GPIOB_2_CONF);
	#endif
	#ifdef GPIOB_3_CONF
	GPIOSHOW(B,3,GPIOB_3_CONF);
	#endif
	#ifdef GPIOB_4_CONF
	GPIOSHOW(B,4,GPIOB_4_CONF);
	#endif
	#ifdef GPIOB_5_CONF
	GPIOSHOW(B,5,GPIOB_5_CONF);
	#endif
	#ifdef GPIOB_6_CONF
	GPIOSHOW(B,6,GPIOB_6_CONF);
	#endif
	#ifdef GPIOB_7_CONF
	GPIOSHOW(B,7,GPIOB_7_CONF);
	#endif
	#ifdef GPIOB_8_CONF
	GPIOSHOW(B,8,GPIOB_8_CONF);
	#endif
	#ifdef GPIOB_9_CONF
	GPIOSHOW(B,9,GPIOB_9_CONF);
	#endif
	#ifdef GPIOB_10_CONF
	GPIOSHOW(B,10,GPIOB_10_CONF);
	#endif
	#ifdef GPIOB_11_CONF
	GPIOSHOW(B,11,GPIOB_11_CONF);
	#endif
	#ifdef GPIOB_12_CONF
	GPIOSHOW(B,12,GPIOB_12_CONF);
	#endif
	#ifdef GPIOB_13_CONF
	GPIOSHOW(B,13,GPIOB_13_CONF);
	#endif
	#ifdef GPIOB_14_CONF
	GPIOSHOW(B,14,GPIOB_14_CONF);
	#endif
	#ifdef GPIOB_15_CONF
	GPIOSHOW(B,15,GPIOB_15_CONF);
	#endif
#endif //#ifdef USE_GPIOB

	
#ifdef USE_GPIOC
	#ifdef GPIOC_0_CONF
	GPIOSHOW(C,0,GPIOC_0_CONF);
	#endif
	#ifdef GPIOC_1_CONF
	GPIOSHOW(C,1,GPIOC_1_CONF);
	#endif
	#ifdef GPIOC_2_CONF
	GPIOSHOW(C,2,GPIOC_2_CONF);
	#endif
	#ifdef GPIOC_3_CONF
	GPIOSHOW(C,3,GPIOC_3_CONF);
	#endif
	#ifdef GPIOC_4_CONF
	GPIOSHOW(C,4,GPIOC_4_CONF);
	#endif
	#ifdef GPIOC_5_CONF
	GPIOSHOW(C,5,GPIOC_5_CONF);
	#endif
	#ifdef GPIOC_6_CONF
	GPIOSHOW(C,6,GPIOC_6_CONF);
	#endif
	#ifdef GPIOC_7_CONF
	GPIOSHOW(C,7,GPIOC_7_CONF);
	#endif
	#ifdef GPIOC_8_CONF
	GPIOSHOW(C,8,GPIOC_8_CONF);
	#endif
	#ifdef GPIOC_9_CONF
	GPIOSHOW(C,9,GPIOC_9_CONF);
	#endif
	#ifdef GPIOC_10_CONF
	GPIOSHOW(C,10,GPIOC_10_CONF);
	#endif
	#ifdef GPIOC_11_CONF
	GPIOSHOW(C,11,GPIOC_11_CONF);
	#endif
	#ifdef GPIOC_12_CONF
	GPIOSHOW(C,12,GPIOC_12_CONF);
	#endif
	#ifdef GPIOC_13_CONF
	GPIOSHOW(C,13,GPIOC_13_CONF);
	#endif
	#ifdef GPIOC_14_CONF
	GPIOSHOW(C,14,GPIOC_14_CONF);
	#endif
	#ifdef GPIOC_15_CONF
	GPIOSHOW(C,15,GPIOC_15_CONF);
	#endif
#endif //#ifdef USE_GPIOC
	
#ifdef USE_GPIOD
	#ifdef GPIOD_0_CONF
	GPIOSHOW(D,0,GPIOD_0_CONF);
	#endif
	#ifdef GPIOD_1_CONF
	GPIOSHOW(D,1,GPIOD_1_CONF);
	#endif
	#ifdef GPIOD_2_CONF
	GPIOSHOW(D,2,GPIOD_2_CONF);
	#endif
	#ifdef GPIOD_3_CONF
	GPIOSHOW(D,3,GPIOD_3_CONF);
	#endif
	#ifdef GPIOD_4_CONF
	GPIOSHOW(D,4,GPIOD_4_CONF);
	#endif
	#ifdef GPIOD_5_CONF
	GPIOSHOW(D,5,GPIOD_5_CONF);
	#endif
	#ifdef GPIOD_6_CONF
	GPIOSHOW(D,6,GPIOD_6_CONF);
	#endif
	#ifdef GPIOD_7_CONF
	GPIOSHOW(D,7,GPIOD_7_CONF);
	#endif
	#ifdef GPIOD_8_CONF
	GPIOSHOW(D,8,GPIOD_8_CONF);
	#endif
	#ifdef GPIOD_9_CONF
	GPIOSHOW(D,9,GPIOD_9_CONF);
	#endif
	#ifdef GPIOD_10_CONF
	GPIOSHOW(D,10,GPIOD_10_CONF);
	#endif
	#ifdef GPIOD_11_CONF
	GPIOSHOW(D,11,GPIOD_11_CONF);
	#endif
	#ifdef GPIOD_12_CONF
	GPIOSHOW(D,12,GPIOD_12_CONF);
	#endif
	#ifdef GPIOD_13_CONF
	GPIOSHOW(D,13,GPIOD_13_CONF);
	#endif
	#ifdef GPIOD_14_CONF
	GPIOSHOW(D,14,GPIOD_14_CONF);
	#endif
	#ifdef GPIOD_15_CONF
	GPIOSHOW(D,15,GPIOD_15_CONF);
	#endif
#endif //#ifdef USE_GPIOD

	
#ifdef USE_GPIOE
	#ifdef GPIOE_0_CONF
	GPIOSHOW(E,0,GPIOE_0_CONF);
	#endif
	#ifdef GPIOE_1_CONF
	GPIOSHOW(E,1,GPIOE_1_CONF);
	#endif
	#ifdef GPIOE_2_CONF
	GPIOSHOW(E,2,GPIOE_2_CONF);
	#endif
	#ifdef GPIOE_3_CONF
	GPIOSHOW(E,3,GPIOE_3_CONF);
	#endif
	#ifdef GPIOE_4_CONF
	GPIOSHOW(E,4,GPIOE_4_CONF);
	#endif
	#ifdef GPIOE_5_CONF
	GPIOSHOW(E,5,GPIOE_5_CONF);
	#endif
	#ifdef GPIOE_6_CONF
	GPIOSHOW(E,6,GPIOE_6_CONF);
	#endif
	#ifdef GPIOE_7_CONF
	GPIOSHOW(E,7,GPIOE_7_CONF);
	#endif
	#ifdef GPIOE_8_CONF
	GPIOSHOW(E,8,GPIOE_8_CONF);
	#endif
	#ifdef GPIOE_9_CONF
	GPIOSHOW(E,9,GPIOE_9_CONF);
	#endif
	#ifdef GPIOE_10_CONF
	GPIOSHOW(E,10,GPIOE_10_CONF);
	#endif
	#ifdef GPIOE_11_CONF
	GPIOSHOW(E,11,GPIOE_11_CONF);
	#endif
	#ifdef GPIOE_12_CONF
	GPIOSHOW(E,12,GPIOE_12_CONF);
	#endif
	#ifdef GPIOE_13_CONF
	GPIOSHOW(E,13,GPIOE_13_CONF);
	#endif
	#ifdef GPIOE_14_CONF
	GPIOSHOW(E,14,GPIOE_14_CONF);
	#endif
	#ifdef GPIOE_15_CONF
	GPIOSHOW(E,15,GPIOE_15_CONF);
	#endif
#endif //#ifdef USE_GPIOE

	
#ifdef USE_GPIOF
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	#ifdef GPIOF_0_CONF
	GPIOSHOW(F,0,GPIOF_0_CONF);
	#endif
	#ifdef GPIOF_1_CONF
	GPIOSHOW(F,1,GPIOF_1_CONF);
	#endif
	#ifdef GPIOF_2_CONF
	GPIOSHOW(F,2,GPIOF_2_CONF);
	#endif
	#ifdef GPIOF_3_CONF
	GPIOSHOW(F,3,GPIOF_3_CONF);
	#endif
	#ifdef GPIOF_4_CONF
	GPIOSHOW(F,4,GPIOF_4_CONF);
	#endif
	#ifdef GPIOF_5_CONF
	GPIOSHOW(F,5,GPIOF_5_CONF);
	#endif
	#ifdef GPIOA_6_CONF
	GPIOSHOW(F,6,GPIOF_6_CONF);
	#endif
	#ifdef GPIOF_7_CONF
	GPIOSHOW(F,7,GPIOF_7_CONF);
	#endif
	#ifdef GPIOF_8_CONF
	GPIOSHOW(F,8,GPIOF_8_CONF);
	#endif
	#ifdef GPIOF_9_CONF
	GPIOSHOW(F,9,GPIOF_9_CONF);
	#endif
	#ifdef GPIOF_10_CONF
	GPIOSHOW(F,10,GPIOF_10_CONF);
	#endif

#endif //#ifdef USE_GPIOF
	
}

/**
  * @brief  Set peripherals in low power mode.
  * @note   All GPIOs are configured in analog mode, which put them in high-impedance to lower consumption.
  *         PA1, PA9 and PB9 need to be set to 0 to correctly shut down PP.
  * @retval None
  */
void HAL_GPIO_LowPower(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIOs clocks to configure them
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | 
		RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOF, ENABLE);

	// Initialize init structure in high Z (= analogic mode)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	// Set GPIOs in high-Z except specified ones to lower consumption
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~( GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_15 );
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1); // PP reset
	GPIO_ResetBits(GPIOA, GPIO_Pin_9); // PP LDO OFF

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~(GPIO_Pin_9);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_9); // PP CS

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	// Switch off unused GPIO
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOF, DISABLE);
}

/**
  * @brief  Set peripherals out of low power mode.
  * @retval None
  */
void HAL_GPIO_OutOfLowPower(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_9); 	// PP LDO ON
	OSAL_udelay(5000); 			// Wait for LDO stabilization
	GPIO_SetBits(GPIOA, GPIO_Pin_1); 	// PP reset off
}
