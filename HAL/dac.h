/**
 * @file dac.h
 * @brief dac.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_DAC_H
#define __HAL_DAC_H

void HAL_DAC_DeInit (void);
void HAL_DAC_Init (void);
void dac_get_valueUV(unsigned long *cur_value_inuV);
void dac_get_value(unsigned short *cur_value_on12bits);
void dac_set_valueUV(unsigned long new_value_inuV);
void dac_set_value(unsigned short new_value_on12bits);
void dac_enable(unsigned char enable);

#endif /* __HAL_DAC_H */
