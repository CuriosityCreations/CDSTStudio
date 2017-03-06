/**
 * @file ProdSectionDef.h
 * @brief ProdSectionDef.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/

#ifndef _PRODSECTIONDEF_H_
#define _PRODSECTIONDEF_H_

#include "prodSectionStruct.h"


#define persistantProdSection		(*((prodSectionStruct_t *) 0x0801F000))

#ifdef PROD
	prodSectionStruct_t			volatileProdSection;
	#define prodSection			volatileProdSection
#else
	#define prodSection			persistantProdSection
#endif


#endif
