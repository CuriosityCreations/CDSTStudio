/**
 * @file osal_comp.h
 * @brief definitions for Pinpointer comparison functions
 * @author bgi@bespoon.com
 * @date 4/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _OSAL_COMP_H_
#define _OSAL_COMP_H_

#include "osal_type.h"
/******* definitions of main PinPointer comparison functions  ********/


/** OSAL_islower checks for a lower-case s8acter.*/
OSAL_s32          OSAL_islower(OSAL_s32 i);

/** OSAL_isupper checks for an uppercase letter.*/
OSAL_s32          OSAL_isupper(OSAL_s32 i);


/* to translate a 16 bits to its Big Endian value*/
#define OSAL_s3216_TO_BE(ui16_host) ((((ui16_host) & (unsigned __s3216) 0x00ff) << 8) | \
                                       (((ui16_host) & (unsigned __s3216) 0xff00) >> 8) )

/* to translate a 32 bits to its Big Endian value */
#define OSAL_s3232_TO_BE(ui32_host) ((((ui32_host) & (unsigned __s3232) 0x000000ff) << 24) | \
                                       (((ui32_host) & (unsigned __s3232) 0x0000ff00) <<  8) | \
                                       (((ui32_host) & (unsigned __s3232) 0x00ff0000) >>  8) | \
                                       (((ui32_host) & (unsigned __s3232) 0xff000000) >>  24))


/* to translate a Big Endian 16 bits to its host representation */
#define OSAL_BE_TO_s3216(ui16_net) ((((ui16_net) & (unsigned __s3216) 0x00ff) << 8) | \
                                      (((ui16_net) & (unsigned __s3216) 0xff00) >> 8) )

/* to translate a Big Endian 32 bits to its host representation*/
#define OSAL_BE_TO_s3232(ui32_net) ((((ui32_net) & (unsigned __s3232) 0x000000ff) << 24) | \
                                      (((ui32_net) & (unsigned __s3232) 0x0000ff00) <<  8) | \
                                      (((ui32_net) & (unsigned __s3232) 0x00ff0000) >>  8) | \
                                      (((ui32_net) & (unsigned __s3232) 0xff000000) >>  24))

#define  OSAL_MAX(a,b) (((a) > (b)) ? (a) : (b))

#define  OSAL_MIN(a,b) (((a) < (b)) ? (a) : (b))

#define  OSAL_ABS(x) ((x<0) ? -(x) : (x))


#endif
