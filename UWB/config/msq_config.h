/**
 * @file msq_config.h
 * @brief this part is usefull to configure ,load firmware, start and make a load of
 *	configuration for the integrated msp430 MSQ	
 * @author gpa@bespoon.com
 * @date 01/02/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef MSQ_CONFIG_H
#define MSQ_CONFIG_H

#include <msq_pm_seq.h>

#define DMEM_SEQ_SPACE 0x0700 //(1792)
#define NB_ACTIVATED_TRIGGER 1 //(HOST_REQUEST)

#define MSP_DELAY(udelay) to_u32(udelay*3/to_u32(1<<CLK_DIVA_HIGH))

typedef struct msq_context{
	OSAL_u16  DMEM_Addr_p;

	OSAL_u16  Seq_Addr_tab_start;
	OSAL_u16  Seq_Addr_tab[MSQ_SEQUENCES_NB];
} msq_context_t;

OSAL_s8 msq_write_eos(OSAL_void* ctx, msq_context_t* msq_ctx);
OSAL_s8 msq_write_raw_seq(OSAL_void* ctx,
			msq_context_t* msq_ctx,
			OSAL_u8* raw_seq,
			OSAL_u16 size,
			msq_pm_seq seq_nb);

OSAL_s8 msq_write_pm_seq(OSAL_void* ctx,
			msq_context_t* msq_ctx,
			pm_state_t src_pm,
			pm_state_t dest_pm,
			msq_pm_seq seq_nb);

OSAL_s8 msq_write_pm(OSAL_void* ctx,
			msq_context_t* msq_ctx,
			struct setting OSAL_PROD_SECT * tab);

#define VECT0_W0 0
#define VECT0_W1 1

#define VECT1_W0 2
#define VECT1_W1 3

#define VECT2_W0 4
#define VECT2_W1 5

#define VECT3_W0 6
#define VECT3_W1 7

#define VECT4_W0 8
#define VECT4_W1 9

/** @brief Pinpointer Registers - SPI bank */
#define SPI_BNK                                                     (0x2000)

/** @brief Pinpointer Registers - MSQ bank */
#define MSQ_BNK                                                     (0x4000)

/** @brief MSQ data memory (sequences) */
#define DMEM                                                        (0x8000)

/** @brief MSQ program memory (firmware) */
#define PMEM                                                        (0xE000)

/** @brief MSQ private registers */
#define PRIV_BNK                                                    (0x6000)

//----------------------------------------------------------
// dmem address mapping
//----------------------------------------------------------

// trigger events configured in memory (used to save memory space)
#define TRG_CFG_0                  (0x0000)
#define TRG_CFG_1                  (0x0002)
#define TRG_CFG_2                  (0x0004)
#define TRG_CFG_3                  (0x0006)
#define TRG_CFG_4                  (0x0008)

// trigger/sequence association address : locates beginning of trig/seq association table for each trigger vector
#define TRG_SQ_ADDR_0              (0x000A)
#define TRG_SQ_ADDR_1              (0x000C)
#define TRG_SQ_ADDR_2              (0x000E)
#define TRG_SQ_ADDR_3              (0x0010)
#define TRG_SQ_ADDR_4              (0x0012)

// sequence address pointer : locates beginning of sequence address table (used in turn to locate sequences)
#define SQ_ADDRTAB_ADDR            (0x0014)

//----------------------------------------------------------
// trig_table address mapping
//----------------------------------------------------------

// triggers activation features
#define TRG_EN                    (BNK_PP_REG | BAR_TRIG_TABL_W00)

// sequence activation features
#define SQ_EN                     (BNK_PP_REG | BAR_TRIG_TABL_W10)
#define SQ_SHW_BYPASS             (BNK_PP_REG | BAR_TRIG_TABL_W14)

//----------------------------------------------------------
// status table address mapping
//----------------------------------------------------------
#define NB_SPL			(BAR_STAT_TABL_W00)
#define NB_SPL_SZ		2

#define NRJ			(BAR_STAT_TABL_W02)
#define NRJ_SZ			6

#define MED			(BAR_STAT_TABL_W08)
#define MED_SZ			8

#define RMSDS			(BAR_STAT_TABL_W16)
#define RMSDS_SZ		8

#define MSQ_RUN_TEST		(BAR_STAT_TABL_W29)
#define MSQ_RUN_TEST_SZ		1

#define MSQ_STATUS_BUSY		(BAR_STAT_TABL_W30)
#define MSQ_STATUS_BUSY_SZ	2

//----------------------------------------------------------
// shadow register address mapping
//----------------------------------------------------------

#define SQ_SHW                    (BNK_PP_REG | BAR_TIMER_32K_SHADOW_NEXT)

//----------------------------------------------------------
// SEQUENCE OPCODES
//----------------------------------------------------------

#define SEQ_MEMCPY        (0x01)
#define SEQ_FIFOSET       (0x02)
#define SEQ_MEMSET        (0x03)
#define SEQ_REGSET        (0x04)
#define SEQ_BITSET        (0x05)

#define SEQ_DELAY         (0x10)
#define SEQ_METRICS       (0x20)
#define SEQ_NEXT          (0x40)
#define SEQ_END           (0x80)

//----------------------------------------------------------
// SEQUENCE FORMAT
//----------------------------------------------------------

typedef struct seq_memcpy_fmt{
	OSAL_u8  op_code;
	OSAL_u8  dest_LSB;
	OSAL_u8  dest_MSB;
	OSAL_u8  src_LSB;
	OSAL_u8  src_MSB;
	OSAL_u8  size;
} OSAL_PACKED_STRUCTURE seq_memcpy_fmt_t;

typedef struct seq_fifoset_fmt{
	OSAL_u8  op_code;
	OSAL_u8  dest_LSB;
	OSAL_u8  dest_MSB;
	OSAL_u8  size;
	OSAL_u8* data;
} OSAL_PACKED_STRUCTURE seq_fifoset_fmt_t;

typedef struct seq_memset_fmt{
	OSAL_u8  op_code;
	OSAL_u8  dest_LSB;
	OSAL_u8  dest_MSB;
	OSAL_u8  size;
	OSAL_u8* data;
} OSAL_PACKED_STRUCTURE seq_memset_fmt_t;

typedef struct seq_regset_fmt{
	OSAL_u8  op_code;
	OSAL_u8  dest_LSB;
	OSAL_u8  dest_MSB;
	OSAL_u8  value;
} OSAL_PACKED_STRUCTURE seq_regset_fmt_t;

typedef struct seq_bitset_fmt{
	OSAL_u8  op_code;
	OSAL_u8  dest_LSB;
	OSAL_u8  dest_MSB;
	OSAL_u8  mask;
	OSAL_u8  value;
} OSAL_PACKED_STRUCTURE seq_bitset_fmt_t;

typedef struct seq_delay_fmt{
	OSAL_u8  op_code;
	OSAL_u8  value_LSB;
	OSAL_u8  value_MSB;
} OSAL_PACKED_STRUCTURE seq_delay_fmt_t;

typedef struct seq_seqnext_fmt{
	OSAL_u8  op_code;
	OSAL_u8  seq_ID;
} OSAL_PACKED_STRUCTURE seq_seqnext_fmt_t;

#endif 
//MSQ_CONFIG_H
