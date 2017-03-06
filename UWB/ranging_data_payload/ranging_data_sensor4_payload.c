/**
 * @file sensor4.c
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


/************************************** SENSOR 4 SPECIFIC IMPLEMENTATION *************/
OSAL_u8 s4_data[2]={'s','4'};

OSAL_void sensor4_payload_init()
{
	//TODO: This is called at protocol start. It inits all about sensor 4
}

OSAL_void sensor4_payload_deinit()
{
	//TODO: This is called at protocol stop. It deinits all about sensor 4
}

OSAL_void sensor4_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits)
{
	//TODO
	//This function MUST only return a buffer to stored data.
	//This function can be called in order to get fixed size of Ext data
	//sensor4_updateData will do the job to get the data
	if(data) *data=&s4_data[0];
	if(dataSizeinBits) *dataSizeinBits=16;
}

OSAL_void sensor4_payload_updateData()
{
	//TODO. Store data of sensor 4 in memory. this function is called within the main loop
}

#endif /*#ifdef CONFIG_RTLS_PROTOCOL*/
