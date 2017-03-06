/**
 * @file pp_hw_types.h
 * @brief define structures for use with pp_hw_* functions
 * @author cca@bespoon.com
 * @date 07/10/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _PP_HW_TYPES_H_
#define _PP_HW_TYPES_H_
#include <osal_type.h>

typedef enum {
	T_REG8,
	T_REG16,
	T_REG32,
	T_BURST,
	T_PROD_REG8,
	T_PROD_REG16,
	T_PROD_BURST,
	T_DELAY,
	T_MASK,
	T_RESET,
	T_DAC_EN,
	T_PWR,
	T_STOP
} OSAL_PACKED_ENUM treg_t;

struct setting
{
	treg_t	Type;
	OSAL_u8	Bank;
	OSAL_u8	Reg;
	OSAL_u8 rsvd;
	union typ
	{
		OSAL_u8  t8;
		OSAL_u16 t16;
		OSAL_u32 t32;
		OSAL_u32 tdelay;
		OSAL_PROD_SECT OSAL_u8 * tprod8;
		OSAL_PROD_SECT OSAL_u16 * tprod16;
		OSAL_PROD_SECT OSAL_u32 * tdac;
		OSAL_bool_t tpwr;
		OSAL_bool_t treset;
		struct {
			OSAL_u8* ptr;
			OSAL_u8 len;
		} tburst;
		struct {
			OSAL_u8 OSAL_PROD_SECT* ptr;
			OSAL_u16 len;
		} tprodburst;
		struct {
			OSAL_u8 val;
			OSAL_u8 bitmask;
		} tmask;
		OSAL_u8 align[8];
	}Val;
};

#endif
