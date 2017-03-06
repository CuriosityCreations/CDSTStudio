/**
 * @file module_api_internal.h
 * @brief internal fucntion for module control
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */
#include "module_api.h"

#ifndef __MODULE_API_INTERNAL_H__
#define __MODULE_API_INTERNAL_H__

//manage ping pong transmission buffer
OSAL_void sys_get_shared_buf(OSAL_u8 sharedBufNb,OSAL_u8** buf, OSAL_u16* totalsizeWithAck);
OSAL_void sys_get_shared_data(OSAL_u8 sharedBufNb,OSAL_u8** data, OSAL_u16* sizeWithoutAck);
OSAL_void sys_set_shared_data_size(OSAL_u8 sharedBufNb,OSAL_u16 size);
OSAL_void sys_set_shared_data_checksum(OSAL_u8 sharedBufNb,OSAL_u16 chckSize);
OSAL_void sys_get_shared_data_checksum(OSAL_u8 sharedBufNb,OSAL_u16* chcksum);

//RADIO intern to this directory API
OSAL_void radio_tx_start();
OSAL_void radio_rx_start();
OSAL_u16 radio_is_tx_finished(uwb_com_sysstatus_e* endStatus,OSAL_u8 IRQ0_Vector_W0);
OSAL_u16 radio_is_rx_finished(uwb_com_sysstatus_e* endStatus,OSAL_u8 IRQ0_Vector_W0, OSAL_u8 IRQ0_Vector_W1);
OSAL_void radio_tx_end();
OSAL_void radio_rx_end();

//SEQUENCER intern to this directory API
OSAL_void seq_start();
OSAL_u8 seq_run();
OSAL_void seq_stop();

//PROTOCOL intern to this directory API

#endif //__MODULE_API_INTERNAL_H__

