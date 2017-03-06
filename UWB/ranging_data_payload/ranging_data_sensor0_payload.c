/**
 * @file ranging_data_sensor0_payload.c
 * @brief definition to function uses to manage sensor 0 packet that will be framed and send through UWB
 * @note THIS FILE IS IMPLEMENTATION DEPENDANT, AND MUST MATCH SYSTEM, HW, ENVIRONMENT,ETC...
 * @author ore@bespoon.com
 * @date 24/02/2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifdef CONFIG_RTLS_PROTOCOL


#include <ranging_data_payload.h>
#include "ADC.h"
#include "module_api.h"

/************************************** SENSOR 0 SPECIFIC IMPLEMENTATION *************/
OSAL_void sensor0_payload_init(){}

OSAL_void sensor0_payload_deinit(){}

OSAL_void sensor0_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits)
{
	//Default implementation: sensor 0 = battery level
	sys_get_battery_data(data,dataSizeinBits);
}

OSAL_void sensor0_payload_updateData(){}

#endif /*#ifdef CONFIG_RTLS_PROTOCOL*/
