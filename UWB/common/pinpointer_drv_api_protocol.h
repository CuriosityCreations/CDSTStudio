/**
 * @file pinpointer_drv_api_protocol.h
 * @brief kernel/userspace pinpointer driver interface, specific to protocol
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include "pinpointer_common_def.h"

#ifndef PINPOINTER_DRV_API_PROTOCOL_H
#define PINPOINTER_DRV_API_PROTOCOL_H

/*********************** IO CONTROL *****************************************/

/**
 * struct dev_param - change current device parameter/capabilities
 * @caps: Capabilites to enable
 */
typedef struct dev_param {
	OSAL_u32 caps;
} dev_param_t;

/**
 * struct dev_state_t - get state of protocol
 * @state: 0:stopped, 1:stopped to running transition, 2:running, 3: running to stopped transition, 4:unknown
 */
typedef struct dev_state {
	OSAL_u8 state;
} dev_state_t;

#define PP_IOC_START_PROTOCOL		_IO  ('z',6)
#define PP_IOC_STOP_PROTOCOL		_IO  ('z',7)
#define PP_IOC_G_PARM				_IOW ('z',8,  dev_param_t)
#define PP_IOC_S_PARM				_IOW ('z',9,  dev_param_t)
#define PP_IOC_G_STATS				_IOW ('z',10, protocol_stats_t)
#define PP_IOC_RST_STATS			_IO  ('z',11)
#define PP_IOC_G_SFRAMEINF			_IOW ('z',12, superframe_info_t)
#define PP_IOC_S_SFRAMEINF			_IOW ('z',13, superframe_info_t)
#define PP_IOC_PROTOCOL_STATUS		_IOW ('z',14, dev_state_t)

#define PP_IOC_G_ARTLS				_IOW ('z',18, artls_up_t)
#define PP_IOC_S_ARTLS				_IOW ('z',19, artls_down_t)


#endif //PINPOINTER_DRV_API_PROTOCOL_H

