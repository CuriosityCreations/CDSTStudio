/**
 * @file TIMER.h
 * @brief TIMER.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_TIMER_H
#define __HAL_TIMER_H

/* Exported functions ------------------------------------------------------- */
void HAL_TIMER_DeInit();
void HAL_TIMER_Init();

void HAL_TIMER2_Init ();
void HAL_TIMER3_Init (unsigned int rate_ms);//0 for default value
void HAL_TIMER_LSI_Autocalibration();

void msleep(unsigned short Time);
void udelay(unsigned long Delay);
void reset_time(void);


#endif /* __HAL_TIMER_H */
