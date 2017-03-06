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
//I2C CONFIG
//************************************************************
//#define 	USE_I2C
#define 	I2C1_OPEN
//#define 	I2C2_OPEN
#define 	I2C_SCL_SPEED       (400000)    // SCL in Hz
#define 	I2C_OWN_ADDR        (0x00)      // use 0x00 for master
#define 	OK					0
#define 	NOK					-1

/**
 * @brief Definition for I2C1
 */
#if defined	I2C1_OPEN
	#define Open_I2C                        I2C1
	#define Open_I2C_CLK                    RCC_APB1Periph_I2C1

	#define Open_I2C_SCL_PIN                GPIO_Pin_6
	#define Open_I2C_SCL_GPIO_PORT          GPIOB
	#define Open_I2C_SCL_GPIO_CLK           RCC_AHBPeriph_GPIOB
	#define Open_I2C_SCL_SOURCE             GPIO_PinSource6
	#define Open_I2C_SCL_AF                 GPIO_AF_3

	#define Open_I2C_SDA_PIN                GPIO_Pin_7
	#define Open_I2C_SDA_GPIO_PORT          GPIOB
	#define Open_I2C_SDA_GPIO_CLK           RCC_AHBPeriph_GPIOB
	#define Open_I2C_SDA_SOURCE             GPIO_PinSource7
	#define Open_I2C_SDA_AF                 GPIO_AF_3

	#define Open_I2C_IRQn                   I2C1_EV_IRQn
	#define I2Cx_IRQHANDLER                 I2C1_EV_IRQHandler

#elif defined I2C2_OPEN

	#define Open_I2C                        I2C2
	#define Open_I2C_CLK                    RCC_APB1Periph_I2C2

	#define Open_I2C_SCL_PIN                GPIO_Pin_10
	#define Open_I2C_SCL_GPIO_PORT          GPIOB
	#define Open_I2C_SCL_GPIO_CLK           RCC_AHB1Periph_GPIOB
	#define Open_I2C_SCL_SOURCE             GPIO_PinSource10
	#define Open_I2C_SCL_AF                 GPIO_AF_I2C2

	#define Open_I2C_SDA_PIN                GPIO_Pin_11
	#define Open_I2C_SDA_GPIO_PORT          GPIOB
	#define Open_I2C_SDA_GPIO_CLK           RCC_AHB1Periph_GPIOB
	#define Open_I2C_SDA_SOURCE             GPIO_PinSource11
	#define Open_I2C_SDA_AF                 GPIO_AF_I2C2

	#define Open_I2C_IRQn                   I2C2_EV_IRQn
	#define I2Cx_IRQHANDLER                 I2C2_EV_IRQHandler

#else
	#error "Please select the I2C-Device to be used (in i2c.h)"
#endif


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

#define GPIO_CS_PP GPIOB
#define GPIO_PIN_CS_PP GPIO_Pin_9

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

#if defined(USE_I2C) && defined(I2C1_OPEN)
//no usart1 possible in case of I2C
#define 	GPIOB_6_CONF 	GPIO_Mode_AF,	GPIO_OType_OD,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_3 //i2c  sdl/sda in alternate function tab 3, pin6=sdl, pin7 = sda
#define 	GPIOB_7_CONF 	GPIO_Mode_AF,	GPIO_OType_OD,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_3
#define 	GPIOB_6_CONF_SHELL 	GPIO_Mode_AF,	GPIO_OType_OD,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_3 //i2c  sdl/sda in alternate function tab 3, pin6=sdl, pin7 = sda
#define 	GPIOB_7_CONF_SHELL 	GPIO_Mode_AF,	GPIO_OType_OD,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_3
#else
  #define 	GPIOB_6_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
  #define 	GPIOB_7_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
  #define 	GPIOB_6_CONF_SHELL 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //usart1 in alternate function tab 0, rx=pin6, tx=pin7
  #define 	GPIOB_7_CONF_SHELL 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0 //usart1 in alternate function tab 0, rx=pin6, tx=pin7
#endif


//#define 	GPIOB_8_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
#define 	GPIOB_9_CONF 	GPIO_Mode_OUT,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
#if defined(USE_I2C) && defined(I2C2_OPEN)
#define 	GPIOB_10_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_I2C2
#define 	GPIOB_11_CONF 	GPIO_Mode_AF,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_I2C2
#else
#define 	GPIOB_10_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
#define 	GPIOB_11_CONF 	GPIO_Mode_IN,	GPIO_OType_PP,	GPIO_Speed_Level_3,	GPIO_PuPd_NOPULL,	GPIO_AF_0
#endif
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
