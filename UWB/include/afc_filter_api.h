/**
 * @file afc_filter_api.h
 * @brief Various filter implementation
 * @author cca@bespoon.com
 * @date 26/03/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */


#include <osal_type.h>

#ifndef _AFC_FILTER_API_H_
#define _AFC_FILTER_API_H_

#define FILTER_AVERAGE 1
#define FILTER_KALMAN  2
#define FILTER_SLIDING_AVERAGE 3

typedef enum {
	KALMAN_SPEED_LOW = 0,
	KALMAN_SPEED_MEDIUM,
	KALMAN_SPEED_FAST,
} kalman_speed;

typedef struct filter filter_t;

typedef struct kalman_init
{
	OSAL_u32 filter_type;
	OSAL_s64 x_init;	        //initial estimated value
	OSAL_u64 P_init;              //initial covariance value of estimate
	OSAL_u64 P_reset;             //covariance value of estimate set at reset
	OSAL_u64 P_th;				// Thresold value for P before saying the filter is ready
	OSAL_u64 Q;                   //covariance value of evolution model
	OSAL_u64 R;                   //covariance value of measurement model
} OSAL_PACKED_STRUCTURE kal_init_t;

union filter_init_types
{
	//avg_init_t avg; //todo when needed
	kal_init_t kalman;
	//slid_init_t sliding; //todo when needed
};

typedef struct filter_init{
	OSAL_u32 filter_type;
	union filter_init_types i;
} filter_init_t;

OSAL_u32 filter_get_alloc_Size(OSAL_u32 filter_type, OSAL_u32 filter_size);
filter_t* filter_create(OSAL_u32 filter_type, OSAL_u32 filter_size);
OSAL_void filter_destroy(filter_t* filter);
filter_t* filter_create_inPool(OSAL_u32 filter_type, OSAL_u32 filter_size,OSAL_void* memPool);
OSAL_void filter_destroy_fromPool(filter_t* filter,OSAL_void* memPool);
OSAL_void filter_add(filter_t* filter, OSAL_s32 value);
OSAL_bool_t filter_is_ready(filter_t* filter);
OSAL_s32 filter_get_value(filter_t* filter);
OSAL_u32 filter_get_type(filter_t* filter);
OSAL_void filter_reset(filter_t* filter, OSAL_s32 offset_value, OSAL_bool_t reset_p_init_value);
OSAL_void filter_init(filter_t* filter);
OSAL_bool_t filter_set_init_values(filter_t *filter, filter_init_t *init);
OSAL_void filter_set_R_coeff(filter_t *filter, OSAL_u32 coeff);
OSAL_void filter_set_Q_coeff(filter_t *filter, OSAL_u32 coeff);
OSAL_void filter_set_x_init(filter_t *filter, OSAL_s64 x_init);


// Hugly function: Do not use it
OSAL_s64 filter_get_kalman_raw_value(filter_t* filter);

#endif // _AFC_FILTER_API_H_

