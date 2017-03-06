/**
 * @file time_helper.c
 * @brief helper functions for time computation
 * @author cca@bespoon.com
 * @date 28/05/2013
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include <types.h>

#ifndef _TIME_HELPER_API_H_
#define _TIME_HELPER_API_H_



/* SEQUENCER_TIME_T HELPERS */

OSAL_bool_t stime_add(sequencer_time_t* a,sequencer_time_t* b, sequencer_time_t* r);
OSAL_bool_t stime_sub(sequencer_time_t* a,sequencer_time_t* b, sequencer_time_t* r);
OSAL_bool_t stime_equal(sequencer_time_t* a, sequencer_time_t* b);

OSAL_bool_t stime_add_ts(sequencer_time_t* a, OSAL_u32 ts, sequencer_time_t* r);
OSAL_bool_t stime_sub_ts(sequencer_time_t* a, OSAL_u32 ts, sequencer_time_t* r);
OSAL_bool_t stime_add_ntoa(sequencer_time_t* a, OSAL_u32 ntoa, sequencer_time_t* r);
OSAL_bool_t stime_sub_ntoa(sequencer_time_t* a, OSAL_u32 ntoa, sequencer_time_t* r);
OSAL_u32 stime2it(sequencer_time_t* t);
OSAL_u32 stime2ts(sequencer_time_t* t);
OSAL_u32 stime2ntoa(sequencer_time_t* t);

OSAL_bool_t stime_normalize(sequencer_time_t* t);
OSAL_bool_t stime_normalize_safe(sequencer_time_t* t);

// protocol_time_t helpers
OSAL_bool_t ptime_add(protocol_time_t* a,protocol_time_t* b, protocol_time_t* r);
OSAL_bool_t ptime_sub(protocol_time_t* a,protocol_time_t* b, protocol_time_t* r);
OSAL_bool_t ptime_equal(protocol_time_t* a, protocol_time_t* b);

OSAL_bool_t ptime_add_ts(protocol_time_t* a, OSAL_u32 ts, protocol_time_t* r);
OSAL_bool_t ptime_sub_ts(protocol_time_t* a, OSAL_u32 ts, protocol_time_t* r);
OSAL_bool_t ptime_add_super_frame(protocol_time_t* a, OSAL_u32 super_frame, protocol_time_t* r);
OSAL_bool_t ptime_sub_super_frame(protocol_time_t* a, OSAL_u32 super_frame, protocol_time_t* r);
OSAL_bool_t ptime_add_hyper_frame(protocol_time_t* a, OSAL_u32 hyper_frame, protocol_time_t* r);
OSAL_bool_t ptime_sub_hyper_frame(protocol_time_t* a, OSAL_u32 hyper_frame, protocol_time_t* r);
OSAL_u32 ptime2it(protocol_time_t* t);
OSAL_u32 ptime2ts(protocol_time_t* t);
OSAL_u32 ptime_delta_in_ts(protocol_time_t* from, protocol_time_t* to);
OSAL_u32 ptime_delta_in_it(protocol_time_t* from, protocol_time_t* to);

OSAL_bool_t ptime_normalize(protocol_time_t* t);
OSAL_bool_t ptime_opposit(protocol_time_t*a, protocol_time_t* r);



/* 1 EP = 62.4MHz */
/* 1 TS = 2^15Hz = 32768Hz */
/* 1 NTOA != 125ps, 1 IW != 1ns */

/* EP RELATED CONVERSION HELPERS */

#define EP_PER_PRP		to_s32(4)
#define NTOA_PER_EP		to_s32(128)
#define IW_PER_EP			to_s32(16)
#define NTOA_PER_IW		to_s32(8)

//@brief Convert EP (62.4Mhz period) to NTOA, i.emult EP by 128(16*8)
//on s32, works if EP <0xFF9724 (0.21474 second), saturated otherwise
#define EP_TO_NTOA(ep)  		( ((to_s32(ep)) > 0xFF9724) ? 0x7FFFFFC5 : to_s32(ep)*to_s32(128) )

//@brief Convert EP (62.4Mhz period) to NTOA, using s64  i.e mult by EP*80000/624
#ifdef CONFIG_HAVE_64BITS
#define EP_TO_NTOA_64(ep) 		(to_s64(ep)*to_s64(80000)/to_s64(624))
#endif

//@brief Convert NTOA (~125ps) to EP (62.4Mhz period)
#define NTOA_TO_EP(ntoa) 		(to_s32(ntoa) / NTOA_PER_EP)

//@brief Convert EP (62.4Mhz period) to IW (~1ns)
#define EP_TO_IW(ep)		(to_s32(ep) * IW_PER_EP)

//@brief Convert EP (62.4Mhz period) to IW (~1ns), using s64
#ifdef CONFIG_HAVE_64BITS
#define EP_TO_IW_64(ep)		(to_s64(ep) * IW_PER_EP)
#endif

//@brief Convert PRP (62.4Mhz period) to EP (62.4Mhz period)
#define PRP_TO_EP(prp)		(to_s32(prp) * EP_PER_PRP)

//@brief Convert NTOA (~125ps) to IW (~1ns)
#define NTOA_TO_IW(ntoa)		(to_s32(ntoa) / NTOA_PER_IW)

//@brief Convert IW (~1ns) to NTOA (~125ps)
#define IW_TO_NTOA(iw)		(to_s32(iw) * NTOA_PER_IW)



/* EP / SECONDS CONVERSION HELPERS */

//@brief Convert EP (62.4Mhz period) to nanoseconds,  i.e mult EP by 16, adding 1/39 EP
//on s32, works if EP <0x7FCB923 (2.1474 second), saturated otherwise
#define EP_TO_NANO(EP)  		( ((to_s32(EP))>0x7FCB923)?0x7FFFFFF5:to_s32(EP)*to_s32(16) + to_s32(EP)/to_s32(39) )

//@brief Convert EP (62.4Mhz period) to nanoseconds, using s64  i.e mult by EP*10000/624
#ifdef CONFIG_HAVE_64BITS
#define EP_TO_NANO_64(EP) 		((to_s64(EP)*to_s64(10000))/to_s32(624))
#endif

//@brief Convert EP (62.4Mhz period) to picoseconds,  i.emult EP by 128(16*8), adding 8/39 EP
//on s32, works if EP <0x20B72 (2.1474 usecond), 
#define EP_TO_PICO(EP)  		( ((to_s32(EP))>0x20B72)?0x7FFFC2AC:to_s32(EP)*to_s32(16000) + to_s32(EP)*to_s32(1000)/to_s32(39) )

//@brief Convert EP (62.4Mhz period) to nanoseconds, using s64  i.e mult by EP*80000/624
#ifdef CONFIG_HAVE_64BITS
#define EP_TO_PICO_64(EP) 		(to_s64(EP)*to_s64(10000000)/to_s64(624))
#endif

//@brief Convert EP (62.4Mhz period) to femtoseconds, i.e mult by EP*10000000000/624 (result in s64 only) 
#ifdef CONFIG_HAVE_64BITS
#define EP_TO_FEMTO_64(EP) 	(to_s64(EP)*to_s64(10000000000)/to_s64(624))
#endif

//@brief Convert nTOA (~125ps) to nanoseconds
#define NTOA_TO_NANO(NTOA) 	( (to_s32(NTOA)*to_s32(625))/to_s32(4992) )

//@brief Convert NTOA (~125ps) to picoseconds
#define NTOA_TO_PICO(ntoa)		( (to_s32(ntoa) > 0x105B97D) ? 0x7FFFFFCE : to_s32(ntoa)*to_s32(125) + to_s32(ntoa)*to_s32(125)/to_s32(624) )

//@brief Convert picoseconds to nTOA (~125ps)
#define PICO_TO_NTOA(pico) 	( (to_s32(pico) > 0x6906) ? 0xD6 : (to_s32(pico)*to_s32(79872))/to_s32(10000000) )

//@brief Convert picoseconds to nTOA (~125ps)
#ifdef CONFIG_HAVE_64BITS
#define PICO_TO_NTOA_64(PICO) 	((to_s64(PICO)*to_s64(79872))/to_s64(10000000))
#endif

//@brief Convert nanoseconds to EP (62.4Mhz period)
#define NANO_TO_EP(nano) 		(to_s32(nano)*to_s32(39)/to_s32(625))

//@brief Convert IW (~1ns) to nanoseconds
#define IW_TO_NANO(iw) 		( (to_s32(iw) > 0x7FCB923A) ? 0x7FFFFFFF : to_s32(iw) + to_s32(iw)/to_s32(624) )

//@brief Convert IW (~1ns) to nanoseconds
#define IW_TO_NANO_PPCM 388752//=624*623

//@brief IW_TO_NANOANDPICO_PPCM into nanoseconds
#define IW_TO_NANO_PPCM_NS 389375//=624*623

//@brief Convert nano to IW (~1ns)
#define NANO_TO_IW(nano) 		( to_s32(nano) - to_s32(nano)/to_s32(625) )

//@brief Convert IW (~1ns) to picoseconds
#define IW_TO_PICO(iw) 		( (to_s32(iw) > 0x20B72F) ? 0x7FFFFD5C : to_s32(iw)*to_s32(1000) + to_s32(iw)*to_s32(1000)/to_s32(624) )

//@brief Convert IW (~1ns) to picoseconds, using s64
#ifdef CONFIG_HAVE_64BITS
#define IW_TO_PICO_64(iw) 		( to_s64(iw)*to_s64(1000) + to_s64(iw)*to_s64(1000)/to_s64(624) )
#endif



/* TS / EP RELATED CONVERSION HELPERS */

#define NTOA_PER_TS		to_s32(243750) // 1 TS / 1 NTOA = (1/32768) / (1/(62.4e6*128))

//@brief Convert TS (32Khz period) to EP (62.4Mhz period)
#define TS_TO_EP(ts) 		(to_s32(ts) * (NTOA_PER_TS/to_s32(2)) / to_s32(64)) // (NTOA_PER_TS / 2) limits overflow

//@brief Convert TS (32Khz period) to EP (62.4Mhz period), using s64
#ifdef CONFIG_HAVE_64BITS
#define TS_TO_EP_64(ts) 	(to_s64(ts) * (NTOA_PER_TS/to_s64(2)) / to_s64(64)) // (NTOA_PER_TS / 2) limits overflow
#endif

//@brief Convert EP (62.4Mhz period) to TS (32Khz period)
#define EP_TO_TS(ep) 		(to_s32(ep) * to_s32(64) / (NTOA_PER_TS/to_s32(2)))

//@brief Convert TS (32Khz period) to NTOA (~125ps)
#define IW_TO_TS(iw)		(to_s32(iw) * to_s32(4) / (NTOA_PER_TS/to_s32(2)))

//@brief Convert NTOA (~125ps) to TS (32Khz period)
#define NTOA_TO_TS(ntoa)		(to_s32(ntoa) / NTOA_PER_TS)

//@brief Convert TS (32Khz period) to NTOA (~125ps)
#define TS_TO_NTOA(ts)		(to_s32(ts) * NTOA_PER_TS)

//@brief Convert TS (32Khz period) to IW (~1ns)
#define TS_TO_IW(ts)		((to_s32(ts) * NTOA_PER_TS) / NTOA_PER_IW)



/* TS / SECONDS RELATED CONVERSION HELPERS */

//@brief Convert TS (32Khz period) to nanoseconds
#define TS_TO_NANO(ts) 		(EP_TO_NANO(TS_TO_EP(ts)))

#endif

