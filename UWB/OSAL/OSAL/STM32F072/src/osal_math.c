/**
 * @file osal_math.c
 * @brief implementation for Pinpointer mathematic functions
 * @author pbo@bespoon.com
 * @date 01/03/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */


#include "../../../include/osal_type.h"
#include "../../../include/osal_math.h"

#ifdef CONFIG_HAVE_64BITS
/* 64bit divisor, dividend and result. dynamic precision */
OSAL_u64 OSAL_div_u64_u64(OSAL_u64 dividend, OSAL_u64 divisor)
{
	return dividend/divisor;
}

OSAL_s64 OSAL_div_s64_s64(OSAL_s64 dividend, OSAL_s64 divisor)
{
	return dividend/divisor;
}
#endif

/**
 * Copyright (C) 2013 Davidlohr Bueso <davidlohr.bueso@hp.com>
 *
 *  Based on the shift-and-subtract algorithm for computing integer
 *  square root from Guy L. Steele.
 * int_sqrt - rough approximation to sqrt
 * @x: integer of which to calculate the sqrt
 *
 * A very rough approximation to the sqrt() function.
 */
OSAL_u32 OSAL_int_sqrt(OSAL_u32 x)
{
         OSAL_u32 b, m, y = 0;

         if (x <= 1)
                 return x;

         m = 1UL << (sizeof(OSAL_u32)*8- 2);
         while (m != 0) {
                 b = y + m;
                 y >>= 1;

                 if (x >= b) {
                         x -= b;
                         y += m;
                 }
                 m >>= 2;
         }

         return y;
}

OSAL_u64 OSAL_int_sqrt64(OSAL_u64 x)
{
         OSAL_u64 b, m, y = 0;

         if (x <= 1)
                 return x;

         m = 1ULL << (sizeof(OSAL_u64)*8- 2);
         while (m != 0) {
                 b = y + m;
                 y >>= 1;

                 if (x >= b) {
                         x -= b;
                         y += m;
                 }
                 m >>= 2;
         }

         return y;
}
