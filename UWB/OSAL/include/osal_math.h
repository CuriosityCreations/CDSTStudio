/**
 * @file osal_math.h
 * @brief definitions for Pinpointer mathematic functions
 * @author cca@bespoon.com
 * @date 25/02/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */



#include "osal_type.h"

#ifdef CONFIG_HAVE_64BITS
OSAL_u64 OSAL_div_u64_u64(OSAL_u64 dividend, OSAL_u64 divisor);
OSAL_s64 OSAL_div_s64_s64(OSAL_s64 dividend, OSAL_s64 divisor);
#endif

// equivalent of (int)ceil( (float)x/(float)y)
// http://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
#define OSAL_ceil_divide(x,y) (1 + ((x - 1) / y))

// create a sum of control of 8 bits ( proba to have samme checksum = 1/(2^8)=> dont use this)
#define GET_SIMPLE_U8CHECKSUM(_buf,_buflen,_chk) do{ \
        OSAL_u32 _c=0;\
        OSAL_u32 _l=(_buflen);\
        OSAL_u8* _b= (OSAL_u8*) (_buf);\
        while(_l--){_c += *_b++;}\
        _chk =(OSAL_u8) (_c&0xFF);\
    }while(0)
    
// create a sum of control of 16 bits ( proba to have samme checksum = 1/(2^16))
#define GET_SIMPLE_U16CHECKSUM(_buf,_buflen,_chk) do{ \
        OSAL_u32 _c=0;\
        OSAL_u32 _l=_buflen;\
        OSAL_u8* _b= (OSAL_u8*) (_buf);\
        while(_l--){_c += *_b++;}\
        _chk =(OSAL_u16) (_c&0xFFFF);\
    }while(0)

// create a sum of control of 32 bits ( proba to have samme checksum = 1/(2^32))
#define GET_SIMPLE_U32CHECKSUM(_buf,_buflen,_chk) do{ \
        OSAL_u32 _l=(_buflen);\
        OSAL_u8* _b= (OSAL_u8*) (_buf);\
        _chk=0;\
        while(_l--) {_chk += *_b++;}\
    }while(0)

//convert a nb of bits into bytes, roudnign to bigger container
#define ROUND_BITS_TO_BYTES(_bits) (((_bits)%8)?_bits/8+1:_bits/8)

//convert a nb of bytes into bits
#define BYTE_TO_BITS(_bytes)  ((_bytes)<<3)

OSAL_u32 OSAL_int_sqrt(OSAL_u32 x);
OSAL_u64 OSAL_int_sqrt64(OSAL_u64 x);

