/**
 * @file pinpointer_hw_api.h
 * @brief Pinpointer HW access main API file
 * @author pbo@bespoon.com
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _PINPOINTER_HW_API_H_
#define _PINPOINTER_HW_API_H_

#include <types.h>

OSAL_void pp_hw_rst(OSAL_u8 val);
OSAL_void pp_hw_dbg(OSAL_u8 val);

OSAL_void pp_hw_pa_enable(OSAL_bool_t val);
OSAL_bool_t pp_hw_is_pa_enable(OSAL_void);

OSAL_void pp_hw_ext_sync(OSAL_bool_t val);

OSAL_s8 pp_hw_reg_read(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8* data_);
OSAL_s8 pp_hw_reg_write(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8 data_);

OSAL_s8 pp_hw_reg16_read(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u16 *data_ );
OSAL_s8 pp_hw_reg16_write(OSAL_void *  ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u16 data_);

OSAL_s8 pp_hw_reg32_read(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u32 *data_);
OSAL_s8 pp_hw_reg32_write(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u32 data_); 

OSAL_s8 pp_hw_burst_read(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8* data_, OSAL_u16 len);
OSAL_s8 pp_hw_burst_write(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8 OSAL_READ_ONLY* data_, OSAL_u16 len);

OSAL_s8 pp_hw_prod_burst_write(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8 OSAL_PROD_SECT* data_, OSAL_u16 len);

#ifdef CONFIG_RTLS_UNITTESTS
OSAL_s8 pp_hw_seq_read(OSAL_void *ctx, struct setting Tab[]);
#endif
OSAL_s8 pp_hw_command_write(OSAL_void *ctx, struct setting command);
OSAL_s8 pp_hw_seq_write(OSAL_void *ctx, OSAL_READ_ONLY struct setting tab[]);
OSAL_s8 pp_hw_prod_seq_write(OSAL_void *ctx, struct setting OSAL_PROD_SECT * tab);

#ifdef CONFIG_RTLS_UNITTESTS
OSAL_s8 pp_hw_burst_verify(OSAL_void *ctx, OSAL_u8 bank, OSAL_u8 reg, OSAL_u8* buf_data, OSAL_u16 len);
#endif
OSAL_s8 pp_hw_seq_verify(OSAL_void *ctx, struct setting tab[]);

#endif
