/**
 * @file RCC.h
 * @brief RCC.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_RCC_H
#define __HAL_RCC_H

/* Exported Variables ------------------------------------------------------- */


/* Exported functions ------------------------------------------------------- */
void HAL_RCC_LowClockInit (void);
void HAL_RCC_Init (unsigned char State);
void HAL_MCU_Sleep_Mode (unsigned char wfiCond);
void HAL_MCU_Stop_Mode (unsigned char wfiCond);
void HAL_MCU_Standby_Mode (void);
void HAL_Out_of_LowPower_Mode (void);

#endif /* __HAL_TIMER_H */
