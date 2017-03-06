/**
 * @file ACCELERO.h
 * @brief ACCELERO.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_ACCEL_H
#define __HAL_ACCEL_H


//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------
typedef enum MOVING_STATUS
{
  	IS_MOVING = 0x00,
	NOT_MOVING,
  	MOV_ERR,
} MOVING_STATUS;

//-----------------------------------------------------------------------------
// Extern Variable Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define HYSTERESIS_TIME 500UL //ms
#define LONG_SLEEP_TIME 100000UL //ms

#define ACC_WRITE 0x00
#define ACC_READ 0x01

#define AUTO_INCREMENT_DISABLE 0x00
#define AUTO_INCREMENT_ENABLE 0x02

//Register address map
#define LIS2DM_STATUS_REG_AUX  		0x07
#define LIS2DM_OUT_TEMP_L  		0x0C
#define LIS2DM_OUT_TEMP_H  		0x0D
#define LIS2DM_INT_COUNTER_REG  	0x0E
#define LIS2DM_WHO_AM_I  		0x0F
#define LIS2DM_TEMP_CFG_REG  		0x1F
#define LIS2DM_CTRL_REG1  		0x20
#define LIS2DM_CTRL_REG2  		0x21
#define LIS2DM_CTRL_REG3  		0x22
#define LIS2DM_CTRL_REG4  		0x23
#define LIS2DM_CTRL_REG5  		0x24
#define LIS2DM_CTRL_REG6  		0x25
#define LIS2DM_REFERENCE  		0x26
#define LIS2DM_STATUS_REG  		0x27
#define LIS2DM_FIFO_READ_START  	0x28
#define LIS2DM_OUT_X  			0x29
#define LIS2DM_OUT_Y  			0x2B
#define LIS2DM_OUT_Z  			0x2D
#define LIS2DM_FIFO_CTRL_REG  		0x2E
#define LIS2DM_FIFO_SRC_REG  		0x2F
#define LIS2DM_INT1_CFG  		0x30
#define LIS2DM_INT1_SRC 		0x31
#define LIS2DM_INT1_THS  		0x32
#define LIS2DM_INT1_DURATION  		0x33
#define LIS2DM_INT2_CFG  		0x34
#define LIS2DM_INT2_SRC  		0x35
#define LIS2DM_INT2_THS  		0x36
#define LIS2DM_INT2_DURATION  		0x37
#define LIS2DM_CLICK_CFG  		0x38
#define LIS2DM_CLICK_SRC  		0x39
#define LIS2DM_CLICK_THS  		0x3A
#define LIS2DM_TIME_LIMIT  		0x3B
#define LIS2DM_TIME_LATENCY  		0x3C
#define LIS2DM_TIME_WINDOW  		0x3D
#define LIS2DM_ACT_THS  		0x3E
#define LIS2DM_ACT_DUR  		0x3F



//-----------------------------------------------------------------------------
#define LIS2DH_STATUS_REG_AUX 	0x07
#define LIS2DH_OUT_TEMP_L 	0x0C
#define LIS2DH_OUT_TEMP_H 	0x0D
#define LIS2DH_INT_COUNTER_REG 	0x0E
#define LIS2DH_WHO_AM_I 	0x0F
#define LIS2DH_TEMP_CFG_REG 	0x1F
#define LIS2DH_CTRL_REG1 	0x20
#define LIS2DH_CTRL_REG2 	0x21
#define LIS2DH_CTRL_REG3 	0x22
#define LIS2DH_CTRL_REG4 	0x23
#define LIS2DH_CTRL_REG5 	0x24
#define LIS2DH_CTRL_REG6 	0x25
#define LIS2DH_REFERENCE 	0x26
#define LIS2DH_STATUS_REG2 	0x27
#define LIS2DH_OUT_X_L 		0x28
#define LIS2DH_OUT_X_H 		0x29
#define LIS2DH_OUT_Y_L 		0x2A
#define LIS2DH_OUT_Y_H 		0x2B
#define LIS2DH_OUT_Z_L 		0x2C
#define LIS2DH_OUT_Z_H 		0x2D
#define LIS2DH_FIFO_CTRL_REG 	0x2E
#define LIS2DH_FIFO_SRC_REG 	0x2F
#define LIS2DH_INT1_CFG 	0x30
#define LIS2DH_INT1_SOURCE 	0x31
#define LIS2DH_INT1_THS 	0x32
#define LIS2DH_INT1_DURATION 	0x33
#define LIS2DH_INT2_CFG 	0x34
#define LIS2DH_INT2_SOURCE 	0x35
#define LIS2DH_INT2_THS 	0x36
#define LIS2DH_INT2_DURATION 	0x37
#define LIS2DH_CLICK_CFG 	0x38
#define LIS2DH_CLICK_SRC 	0x39
#define LIS2DH_CLICK_THS 	0x3A
#define LIS2DH_TIME_LIMIT 	0x3B
#define LIS2DH_TIME_LATENCY 	0x3C
#define LIS2DH_TIME_WINDOW 	0x3D
#define LIS2DH_Act_THS 		0x3E
#define LIS2DH_Act_DUR 		0x3F
//-----------------------------------------------------------------------------
// Internal Routine PROTOTYPES
//-----------------------------------------------------------------------------
void LIS2DM_read(unsigned char addr);
void LIS2DM_write(unsigned char addr, unsigned char value);
void LIS2DM_ISR(void);
//-----------------------------------------------------------------------------
// Extern Function PROTOTYPES
//-----------------------------------------------------------------------------
void LIS2DM_DeInit(void);
unsigned char LIS2DM_init(void);
void LIS2DM(unsigned char enable); // Enable/Disable Accelerometer
void LIS2DM_EnableINT(unsigned char state);

unsigned char LIS2DM_isMoving(void);
short LIS2DM_X(void);
short LIS2DM_Y(void);
short LIS2DM_Z(void);

void LIS2DM_test(void);
signed char LIS2DM_self_test(void);

#endif
