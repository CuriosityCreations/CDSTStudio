/**
 * @file sensor2.c
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

/************************************** SENSOR 2 SPECIFIC IMPLEMENTATION *************/
OSAL_s8 tsaConfidence = 0;

OSAL_void sensor2_payload_init()
{
	//TODO: This has to be called in program main loop, to init this sensor
}

OSAL_void sensor2_payload_deinit()
{
	//TODO: This has to be called in programm main loop
}

OSAL_void sensor2_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits)
{
	if(data) *data=(OSAL_u8*)&tsaConfidence;
	if(dataSizeinBits) *dataSizeinBits=7;
}

OSAL_void sensor2_payload_updateData()
{
	
}

#endif /*#ifdef CONFIG_RTLS_PROTOCOL*/
