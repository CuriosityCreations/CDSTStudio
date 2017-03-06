/**
 * @file HAL_config.h
 * @brief HAL_config.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/


#ifndef _HAL_CONFIG_H
#define _HAL_CONFIG_H


//************************************************************
//TIMER CONFIG
//************************************************************
#define USE_TIMER2
#define USE_TIMER3

//************************************************************
//USART CONFIG
//************************************************************
#define USART_SYNC_MODE_DISABLED 0
#define USART_SYNC_MODE_ENABLED 1

//USART1, USART2, both activated, choice done in prodsection
#define 	USART1_MODE	USART_SYNC_MODE_DISABLED
#define 	USART2_MODE	USART_SYNC_MODE_DISABLED

//************************************************************
//SPI CONFIG
//************************************************************
#define SPI_MASTER 1
#define SPI_SLAVE 0

#define 	USE_SPI1 	SPI_SLAVE
#define 	USE_SPI2 	SPI_MASTER


//************************************************************
// TEMPERATURE
//************************************************************
#define USE_TEMP

//************************************************************
//GPIO NAME ABSTRACTION
//************************************************************
#define GPIO_RESET_PP GPIOA
#define GPIO_PIN_RESET_PP GPIO_Pin_1

#define GPIO_MODULEIT GPIOA
#define GPIO_PIN_MODULEIT GPIO_Pin_10

//************************************************************
//GPIO CONFIG
//************************************************************
#define 	USE_GPIOA
#define 	USE_GPIOB
#define 	USE_GPIOC
//#define 	USE_GPIOD
//#define 	USE_GPIOE
//#define 	USE_GPIOF

//FOR EACH GPIO DEFINE 
// #define GPIOX_N_CONF		PINNB,MODE,OTYPE,SPEED,PuPd,sourceNb,AlternateFunction nb
// last  argument are sensfull only if mode == GPIO_Mode_AF, let iot to AF0 otherwise
// in code call GPIOCONFIGURE(X,N,GPIOX_N_CONF)
// THIS WILL BE USED ONLY IF, FOR A SAME GROUP, USE_GPIOX IS DEFINED 
//For STM32F072: (0..15) for GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, (0..10) for GPIOF.
// if a conf is not defined, default is GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0


//************************************************************
//GPIO CONFIG GROUP A
//************************************************************

#define 	GPIOA_0_CONF	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //in all firmware, it s used as input to wake up the module, in pord its used as input for io test
#define 	GPIOA_1_CONF 	GPIO_Mode_OUT,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 // Reset the PP
#define 	GPIOA_2_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_1 // usart2tx
#define 	GPIOA_3_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_1 // usart2rx

//#define 	GPIOA_4_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 

#define 	GPIOA_5_CONF 	GPIO_Mode_AN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //ADC
#define 	GPIOA_6_CONF 	GPIO_Mode_AN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //ADC
#define 	GPIOA_7_CONF 	GPIO_Mode_AN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //ADC
//#define 	GPIOA_8_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOA_9_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
#define 	GPIOA_10_CONF 	GPIO_Mode_OUT,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 // Out IT
//#define 	GPIOA_11_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOA_12_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0

//#define 	GPIOA_13_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOA_14_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0

#define 	GPIOA_15_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //spi 1 NSS in alternate function tab 0


//************************************************************
//GPIO CONFIG GROUP B
//************************************************************

//#define 	GPIOB_0_CONF	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOB_1_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOB_2_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0

#define 	GPIOB_3_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
#define 	GPIOB_4_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //spi 1 NSS in alternate function tab 0, pin3=NSS, pin4 = MISO, pin5 = MOSI
#define 	GPIOB_5_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0

#define 	GPIOB_6_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
#define 	GPIOB_7_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
#define 	GPIOB_6_CONF_SHELL 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //usart1 in alternate function tab 0, rx=pin6, tx=pin7
#define 	GPIOB_7_CONF_SHELL 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //usart1 in alternate function tab 0, rx=pin6, tx=pin7

//#define 	GPIOB_8_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
#define 	GPIOB_9_CONF 	GPIO_Mode_OUT,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOB_10_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOB_11_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOB_12_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0

#define 	GPIOB_13_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //spi 2 alternate function tab 0 pin13=sck, pin14=MISO, pin15=MOSI
#define 	GPIOB_14_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //spi 2 alternate function tab 0 pin13=sck, pin14=MISO, pin15=MOSI
#define 	GPIOB_15_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //spi 2 alternate function tab 0 pin13=sck, pin14=MISO, pin15=MOSI



//************************************************************
//GPIO CONFIG GROUP C
//************************************************************

//#define 	GPIOC_0_CONF	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_1_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_2_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_3_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_4_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_5_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_6_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_7_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_8_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_9_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_10_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_11_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_12_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0

#define 	GPIOC_13_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //PP wake up
//#define 	GPIOC_14_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOC_15_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0



//************************************************************
//GPIO CONFIG GROUP D
//************************************************************
//#define 	GPIOD_0_CONF	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_1_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_2_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_3_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_4_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_5_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_6_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_7_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_8_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_9_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_10_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_11_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_12_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_13_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_14_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOD_15_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0



//************************************************************
//GPIO CONFIG GROUP E
//************************************************************
//#define 	GPIOE_0_CONF	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_1_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_2_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_3_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_4_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_5_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_6_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_7_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_8_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_9_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_10_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_11_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_12_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_13_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_14_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOE_15_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0



//************************************************************
//GPIO CONFIG GROUP F
//************************************************************
//#define 	GPIOF_0_CONF	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_1_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_2_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_3_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_4_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_5_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_6_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_7_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_8_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_9_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
//#define 	GPIOF_10_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0


#endif /*_HAL_CONFIG_H*/
