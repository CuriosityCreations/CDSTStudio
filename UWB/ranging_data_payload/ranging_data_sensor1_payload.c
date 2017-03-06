/**
 * @file sensor1_payload_payload.c
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
#include "module_api.h"



/************************************** SENSOR 1 SPECIFIC IMPLEMENTATION *************/
OSAL_void sensor1_payload_init(){}

OSAL_void sensor1_payload_deinit(){}

OSAL_u8 ismoving_data=0;
OSAL_void sensor1_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits)
{
	//in our imlpementation, sensor1 detect moving statusget the data
	if(data) *data=&ismoving_data;
	if(dataSizeinBits) *dataSizeinBits=1;
}

OSAL_void sensor1_payload_updateData()
{
	OSAL_bool_t bMoving;
	sys_is_module_moving(&bMoving);

	ismoving_data = bMoving == OSAL_true ? 1 : 0;
}

#endif /*#ifdef CONFIG_RTLS_PROTOCOL*/
