/**
 * @file ADC.h
 * @brief ADC.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_ADC_H
#define __HAL_ADC_H
typedef enum
{
	VBAT_LOW_THRES	  = 380000, // 3.8V
	VBAT_LEVEL1_THRES = 383333, // 3.825V
	VBAT_LEVEL2_THRES = 386666, // 3.85V
	VBAT_LEVEL3_THRES = 389999, // 3.875V
	VBAT_LEVEL4_THRES = 393332, // 3.9V
	VBAT_LEVEL5_THRES = 396665, // 3.925V
	VBAT_HIGH_THRES	  = 400000 // 4V
}vbat_level_e;

void HAL_ADC_DeInit (void);
void HAL_ADC_Init (void);

/* Public variables ---------------------------------------------------------*/
#define     VDDA    250000	// 2.5V scale 10µV regul

extern long Intern_Temperature;
extern unsigned long Vbat_Module;
void 	HAL_ADC_Read_Values(void);

#endif /* __HAL_ADC_H */
