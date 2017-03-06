/**
 * @file coreId.h
 * @brief define stacklevel ID to trace calls to OSAL
 * @author pbo@bespoon.com
 * @date 08/01/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _COREID_H_
#define _COREID_H_

/*TODO may add module and sub-module ID */

#define COREID_PROTOCOL     10
#define COREID_BASE         20
#define COREID_TAG          30	
#define COREID_COORD        40
#define COREID_SEQ          50
#define COREID_SEQ_SCHED    60
#define COREID_CONFIG       70
#define COREID_FRAMER       80
#define COREID_FILTER       90
#define COREID_PM           100

#endif
