/**
 * @file GPIO.h
 * @brief GPIO.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HAL_GPIO_H
#define __HAL_GPIO_H

void HAL_GPIO_DeInit (void);

void HAL_GPIO_Init (void);

void HAL_GPIO_Show(void);

void HAL_GPIO_LowPower(void);

void HAL_GPIO_OutOfLowPower(void);

#endif /* __HAL_GPIO_H */
