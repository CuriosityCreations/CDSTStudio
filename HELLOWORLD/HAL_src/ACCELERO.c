/**
 * @file ACCELERO.c
 * @brief ACCELERO.c
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
#include "ACCELERO.h"
#pragma GCC optimize("Os")

void LIS2DM_DeInit(void){}

unsigned char LIS2DM_init(void){return 0;}

void LIS2DM(unsigned char enable){}

unsigned char LIS2DM_isMoving(void){return 0;}

void LIS2DM_EnableINT(unsigned char state){}

short LIS2DM_X(void){return 0;}

short LIS2DM_Y(void){return 0;}

short LIS2DM_Z(void){return 0;}

void LIS2DM_test(void){}

unsigned char LIS2DM_init_test(void){return 0;}

signed char LIS2DM_self_test(void){return 0;}
//-----------------------------------------------------------------------------
// Internal Routine
//-----------------------------------------------------------------------------

void LIS2DM_read(unsigned char addr){}

void LIS2DM_write(unsigned char addr, unsigned char value){}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------







