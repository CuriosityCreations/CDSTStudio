/**
 * @file osal_type.h
 * @brief Definitions for <b>O</b>perating <b>S</b>ystem <b>A</b>bstraction <b>L</b>ayer  type
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include "osal_type_STM32.h"
/** @brief OSAL_s8 @note signed char  (8bits)definition. Use STM32 typedef definition */
typedef __OSAL_s8 OSAL_s8;
/** @brief OSAL_u8 @note unsigned char (8bits) definition. Use STM32 typedef definition */
typedef __OSAL_u8 OSAL_u8;
/** @brief OSAL_s16 @note signed short (16bits) definition. Use STM32 typedef definition */
typedef __OSAL_s16 OSAL_s16;
/** @brief OSAL_u16 @note unsigned short (16bits) definition. Use STM32 typedef definition */
typedef __OSAL_u16 OSAL_u16;
/** @brief OSAL_s32 @note signed int (32bits) definition. Use STM32 typedef definition */
typedef __OSAL_s32 OSAL_s32;
/** @brief OSAL_u32 @note unsigned int (32bits) definition. Use STM32 typedef definition */
typedef __OSAL_u32 OSAL_u32;
#ifdef CONFIG_HAVE_64BITS
/** @brief OSAL_s64 @note signed long (64bits) definition. Use STM32 typedef definition */
typedef __OSAL_s64 OSAL_s64;
/** @brief OSAL_u64 @note unsigned long (64bits) definition. Use STM32 typedef definition */
typedef __OSAL_u64 OSAL_u64;
#endif
/** @brief OSAL_float @note float (32bits) definition. Use STM32 typedef definition */
typedef __OSAL_float OSAL_float;
/** @brief OSAL_double @note double (64bits) definition. Use STM32 typedef definition */
typedef __OSAL_double OSAL_double;
/** @brief OSAL_stackLevel @note stack level definition. ID for allocation */
typedef __OSAL_u32 OSAL_stackLevel;
/** @brief OSAL_void @note void pointer definition.*/
typedef __OSAL_void OSAL_void;

#ifndef _OSAL_TYPE_H_
#define _OSAL_TYPE_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include <stdbool.h>

/**
 *  @brief OSAL_timeval time val definition
 *  @note This structure is aligned and sized 8 bytes
 */
typedef struct  {
	OSAL_s32	tv_sec;     /**< Indicate the second of a timer with micro second precision*/
	OSAL_s32	tv_usec;    /**< Indicate the micro second of a timer with micro second precision*/
}OSAL_timeval;

/**
 *  @brief OSAL_timezone time zone definition
 *  @note This structure is aligned and sized 8 bytes
 */
typedef struct {
	OSAL_s32	tz_minuteswest;     /**< minutes west of Greenwich */
	OSAL_s32	tz_dsttime;         /**< type of DST correction */
}OSAL_timezone;

/** @brief max_s8 maximum value for a signed char */
#define max_s8  127 
/** @brief max_u8 maximum value for a unsigned char */
#define max_u8  255 
/** @brief max_s16 maximum value for a signed short */
#define max_s16  32767 
/** @brief min_s16 minimum value for a signed short */
#define min_s16  -32768
/** @brief max_u16 maximum value for a unsigned short */
#define max_u16  65535 
/** @brief max_s32 maximum value for a signed int */
#define max_s32  2147483647L
/** @brief min_s32 minimum value for a unsigned int */
#define min_s32  -2147483648L
/** @brief max_u32 maximum value for a unsigned int */
#define max_u32  4294967295UL
/** @brief max_s64 maximum value for a signed long */
#define max_s64  9223372036854775807LL
/** @brief max_u64 maximum value for a unsigned long */
#define max_u64  18446744073709551615ULL

/** @brief OSAL_READ_ONLY macro to force Read only to a variable */
#define OSAL_READ_ONLY const
/** @brief OSAL_PROD_SECT macro to force Read only to a variable, belonging to rpodsection */
#define OSAL_PROD_SECT const
/** @brief OSAL_prodsection macro to force link to a variable, belonging to rpodsection */
#define OSAL_prodsection extern const

/** @brief OSAL_PACKED_STRUCTURE macro to force packed attribute to a structure */
#define OSAL_PACKED_STRUCTURE __attribute__((__packed__))
/** @brief OSAL_PACKED_ENUM macro to force packed attribute to an enum */
#define OSAL_PACKED_ENUM __attribute__((__packed__))
//#define OSAL_ALIGNED_STRUCTURE __attribute__((__aligned __))

/** @brief OSAL_bool_t @note boolean definition. Use STM32 typedef definition */
typedef bool OSAL_bool_t;
//typedef OSAL_u8 OSAL_bool_t;
/** @brief OSAL_true TRUE boolean */
#define OSAL_true true
/** @brief OSAL_false FALSE boolean */
#define OSAL_false false

/** @brief OSAL_error_t */
typedef enum {
	OSAL_OK=0,    /**< Function/Method succeed */
	OSAL_ERROR,   /**< Function/Method failed */
	OSAL_ENOMEM,  /**< No more memory on device to do the Function/Method */
	OSAL_EAGAIN   /**< Function/Method not available right now, try again later. */
}OSAL_PACKED_ENUM OSAL_error_t;

/** @brief to_s8 macro to force conversion to signed char */
#define to_s8(x)  ((OSAL_s8)(x))
/** @brief to_u8 macro to force conversion to unsigned char */
#define to_u8(x)  ((OSAL_u8)(x))
/** @brief to_s16 macro to force conversion to signed short */
#define to_s16(x) ((OSAL_s16)(x))
/** @brief to_u16 macro to force conversion to unsigned short */
#define to_u16(x) ((OSAL_u16)(x))
/** @brief to_s32 macro to force conversion to signed int */
#define to_s32(x) ((OSAL_s32)(x))
/** @brief to_u32 macro to force conversion to unsigned int */
#define to_u32(x) ((OSAL_u32)(x))
#ifdef CONFIG_HAVE_64BITS
/** @brief to_s64 macro to force conversion to signed long */
#define to_s64(x) ((OSAL_s64)(x))
/** @brief to_u64 macro to force conversion to unsigned long */
#define to_u64(x) ((OSAL_u64)(x))
#endif

/** @brief OSAL_ARRAY_SIZE macro to determinate size of an array */
#define OSAL_ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#ifndef NULL
/** @brief NULL Nil pointer definition */
#define NULL 0
#endif

#ifdef __cplusplus
  }
#endif

#endif //_OSAL_TYPE_H_
