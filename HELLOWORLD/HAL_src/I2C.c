/**
 * @file I2C.c
 * @brief I2C.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#include "HAL_config.h"
#include "I2C.h"

#pragma GCC optimize("Os")

void HAL_I2C_DeInit(void){}
void HAL_I2C_Init(void){}
int HAL_I2C_ReadTransfer(unsigned char dev_addr, unsigned char *buffer, int cnt, unsigned long ptr, unsigned char ptrlen)
{
	return -1;
}
int  HAL_I2C_WriteTransfer(unsigned char dev_addr, unsigned char *buffer, int cnt, unsigned long ptr, unsigned char ptrlen)
{
	return -1;
}

void I2C1_EV_IRQHandler(void)
{
}

void I2C2_EV_IRQHandler(void)
{
}
