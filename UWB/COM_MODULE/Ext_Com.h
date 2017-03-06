/**
 * @file Ext_Com.h
 * @brief Ext_Com.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXT_COM_H
#define __EXT_COM_H

#include "types.h"
#include "uwbmodule_com.h"

//align to 32 bits
#define EXTCOM_BUF_STEP_INDICATOR 0
#define EXTCOM_BUF_TX_BEGIN 4
#define EXTCOM_BUF_RX_BEGIN 8
#define EXTCOM_BUF_DFLT_TXSIZE 3
#define EXTCOM_BUF_DFLT_RXSIZE 3
#define EXTCOM_BUF_SIZE 12

extern unsigned char EXTCOM_BUF_PCKTTRACE;
extern int EXTCOM_BUF_TxLen;
extern int EXTCOM_BUF_RxLen;

extern unsigned char EXTCOM_BUF[EXTCOM_BUF_SIZE];
#define GET_DEFAULT_EXTCOM_RXFRAME(_buf) do{_buf=&EXTCOM_BUF[EXTCOM_BUF_RX_BEGIN];}while(0)
#define GET_DEFAULT_EXTCOM_TXFRAME(_buf) do{_buf=&EXTCOM_BUF[EXTCOM_BUF_TX_BEGIN];}while(0)

extern unsigned char* EXTCOM_BUF_TxFrame;
extern unsigned char* EXTCOM_BUF_RxFrame;


//manage command comming from external of mmoduel (spi, uart, usb...)
void uwb_com_ext(void);
#endif
