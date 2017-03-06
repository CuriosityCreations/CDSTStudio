/**
 * @file spi_helper.H
 * @brief Helpers to convert data from pinpointer spi burst into u8,u16,u32, ... in an hardware independant way
 * @author cca@bespoon.com
 * @date 24/01/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _SPI_HELPER_H_
#define _SPI_HELPER_H_
#include <osal_type.h>

#define spi_to_u32(a) (\
	(to_u32(((OSAL_u8*)(a))[3]) << 24) | \
	(to_u32(((OSAL_u8*)(a))[2]) << 16) | \
	(to_u32(((OSAL_u8*)(a))[1]) << 8 ) | \
	(to_u32(((OSAL_u8*)(a))[0])) \
	)

#define spi_to_s32(a) (\
	(to_s32(((OSAL_u8*)(a))[3]) << 24) | \
	(to_s32(((OSAL_u8*)(a))[2]) << 16) | \
	(to_s32(((OSAL_u8*)(a))[1]) << 8 ) | \
	(to_s32(((OSAL_u8*)(a))[0])) \
	)

#define spi_to_u16(a) (\
	(to_u16(((OSAL_u8*)(a))[1]) << 8 ) | \
	(to_u16(((OSAL_u8*)(a))[0])) \
	)

#define spi_to_s16(a) (\
	(to_s16(((OSAL_u8*)(a))[1]) << 8 ) | \
	(to_s16(((OSAL_u8*)(a))[0])) \
	)

#define spi_to_u8(a) (to_u8(((OSAL_u8*)(a))[0]))

#define spi_to_s8(a) (to_s8(((OSAL_u8*)(a))[0]))

#endif
