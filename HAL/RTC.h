/**
 * @file RTC.h
 * @brief RTC.h
 * @author  bespoon.com
 * @date 22/07/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_RTC_H
#define __HAL_RTC_H

void HAL_RTC_DeInit();
void HAL_RTC_Init();
//time returned is NOT corrected from finetune and frequency correction
void HAL_RTC_GetTime(OSAL_timeval *tim);
void HAL_RTC_Get_RealTime(OSAL_timeval *tim);
void HAL_RTC_Get_SysTime(OSAL_timeval *tim);
void HAL_RTC_StartAlarm(OSAL_timeval* tim);
void HAL_RTC_CancelAlarm();
void HAL_RTC_ReLaunchOrStopAlarm();
void HAL_RTC_Autocalibrate();
void HAL_RTC_GetFineTune(OSAL_u64* RTCClockCorrection,OSAL_u64* fineTune);
void HAL_RTC_SetFineTune(OSAL_u64* RTCClockCorrection, OSAL_u64* fineTune);
void HAL_RTC_GetPrecisionFactor(unsigned char* factor);
void HAL_RTC_SetPrecisionFactor(unsigned char factor);

#endif /* __HAL_RTC_H */
