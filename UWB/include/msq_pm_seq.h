/**
 * @file msq_pm_seq.h
 * @brief This file is used to sort sequence in DMED and to switch PM by MSQ
 * @author gpa@bespoon.com
 * @date 01/02/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _MSQ_POWER_MODE_SEQ_H_
#define _MSQ_POWER_MODE_SEQ_H_

 typedef enum{
 /*	|*/SEQ_PM2_PM0=0,
/*	|	|*/SEQ_PM0_STDBY,
/*	|	|	|*/
/*	|	|	|*/SEQ_STDBY_RXIDLE,
/*	|	|	|*/SEQ_STDBY_TXIDLE,
//////////////////////////////////////////////////
/*	|	|	|*/SEQ_TXIDLE_STDBY,
/*	|	|	|*/SEQ_RXIDLE_STDBY,
/*	|	|	|*/
/*	|	|*/SEQ_STDBY_PM0,
 /*	|*/SEQ_PM0_PM2,
 /*	|	|	|*/SEQ_STDBY_TXRXIDLE,
 /*	|	|	|*/SEQ_TXRXIDLE_STDBY,
 SEQ_SPI_MSQ_BNK_CPY,
 MSQ_SEQUENCES_NB,
 } OSAL_PACKED_ENUM msq_pm_seq;

#endif