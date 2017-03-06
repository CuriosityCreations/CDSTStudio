/**
 * @file module_api.h
 * @brief api for module
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */
#include "module_api_def.h"
#include "module_api_sys.h"
#include "module_api_radio.h"
#ifdef CONFIG_GENERIC
#include "module_api_seq.h"
#endif
#ifdef CONFIG_RTLS_PROTOCOL
#include "module_api_prot.h"
#endif
#ifdef PROD
#include "module_api_prod.h"
#endif

#ifndef __MODULE_API_H__
#define __MODULE_API_H__
//common global variable defined in sys part, extern for other
extern context_t ctx; // Include all Context Variable for software

#endif //__MODULE_API_H__

