/**
 * @file I2C.h
 * @brief I2C.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/


#ifndef _I2C_H
#define _I2C_H

void HAL_I2C_DeInit(void);
void HAL_I2C_Init(void);
int  HAL_I2C_ReadTransfer(unsigned char dev_addr, unsigned char *buffer, int cnt, unsigned long ptr, unsigned char ptrlen);
int  HAL_I2C_WriteTransfer(unsigned char dev_addr, unsigned char *buffer, int cnt, unsigned long ptr, unsigned char ptrlen);

#endif /*_I2C_H*/
