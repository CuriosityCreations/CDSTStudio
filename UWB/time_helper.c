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

#include <config_generic.h>
#include <types.h>
#include <time_helper_api.h>
#include <config_api.h>

/**
 * @brief add two sequencer_time_t
 * @param a left operand of addition
 * @param b right operand of addition
 * @param r return value:  r = a + b
 *
 * @note r must be different from a and b (no in-place addition)
 * @return true on success, false otherwise
 */
OSAL_bool_t stime_add(sequencer_time_t* a,sequencer_time_t* b, sequencer_time_t* r)
{
	if(a == NULL || b == NULL || r == NULL)
		return OSAL_false;

	OSAL_memset(r,0, sizeof(sequencer_time_t));

	// FIXME: does modulo operations work correectly on IAR?
	r->ntoa = a->ntoa + b->ntoa;

	r->ts = r->ntoa/NTOA_PER_TS;
	r->ntoa %= NTOA_PER_TS;

	r->ts += a->ts + b->ts;

	r->it = r->ts / get_nbTS();
	r->ts %= get_nbTS();

	r->it += a->it + b->it;

	r->it %= get_PHS();

	return OSAL_true;	
}

/**
 * @brief substract two sequencer_time_t
 * @param a left operand of substraction
 * @param b right operand of substraction
 * @param r return value:  r = a - b
 *
 * @note r must be different from a and b (no in-place substraction)
 * @return true on success, false otherwise
 */
OSAL_bool_t stime_sub(sequencer_time_t* a,sequencer_time_t* b, sequencer_time_t* r)
{

	OSAL_u32 carry_ntoa = 0, carry_ts = 0, carry_it = 0;

	if(a == NULL || b == NULL || r == NULL)
		return OSAL_false;

	OSAL_memset(r, 0, sizeof(sequencer_time_t));

	while( (a->ntoa + carry_ntoa) < b->ntoa)
	{
		carry_ntoa += NTOA_PER_TS;
		carry_ts--; //Ya, I'm decrementing a unsigned int...
	}
	r->ntoa = a->ntoa + carry_ntoa - b->ntoa;

	while( (a->ts + carry_ts) < b->ts || (a->ts + carry_ts - b->ts) > (get_nbTS()-1))
	{
		carry_ts += get_nbTS();
		carry_it--;
	}

	r->ts = a->ts + carry_ts - b->ts;

	while( (a->it + carry_it) < b->it || (a->it + carry_it - b->it) >= get_PHS())
	{
		carry_it += get_PHS();
	}

	r->it = a->it + carry_it - b->it;

	/*if( r->it > get_PHS()-1)
	{
		r->it -= (r->it / get_PHS()) * get_PHS();
	}*/

	return OSAL_true;	
}


/**
 * @brief tell wheter or not two sequencer_time_t refere to the same time
 * @param a first sequencer_time_t to compare
 * @param b second sequencer_time_t to compare
 * @return true if both sequencer_time_t refere to the same time, false otherwise
 */
OSAL_bool_t stime_equal(sequencer_time_t* a, sequencer_time_t* b)
{
	return a != NULL && b != NULL && a->it == b->it && a->ts == b->ts && a->ntoa == b->ntoa;
}

OSAL_bool_t stime_add_ts(sequencer_time_t* a, OSAL_u32 ts, sequencer_time_t* r)
{
	sequencer_time_t b = {
		.it = 0,
		.ts = ts,
		.ntoa = 0,
	};
	stime_normalize(&b);
	return stime_add(a,&b,r);
}

OSAL_bool_t stime_sub_ts(sequencer_time_t* a, OSAL_u32 ts, sequencer_time_t* r)
{
	sequencer_time_t b = {
		.it = 0,
		.ts = ts,
		.ntoa = 0,
	};
	stime_normalize(&b);
	return stime_sub(a,&b,r);
}

OSAL_bool_t stime_add_ntoa(sequencer_time_t* a, OSAL_u32 ntoa, sequencer_time_t* r)
{
	sequencer_time_t b = {
		.it = 0,
		.ts = 0,
		.ntoa = ntoa,
	};
	stime_normalize(&b);
	return stime_add(a,&b,r);
}

OSAL_bool_t stime_sub_ntoa(sequencer_time_t* a, OSAL_u32 ntoa, sequencer_time_t* r)
{
	sequencer_time_t b = {
		.it = 0,
		.ts = 0,
		.ntoa = ntoa,
	};
	stime_normalize(&b);
	return stime_sub(a,&b,r);
}

/**
 * @brief convert sequencer_time_t to it
 * @param t time to convert
 * @return number in it
 */
OSAL_u32 stime2it(sequencer_time_t* t)
{
	return t != NULL ? t->it : 0;
}

/**
 * @brief convert sequencer_time_t to ts
 * @param t time to convert
 * @return number in ts
 */
OSAL_u32 stime2ts(sequencer_time_t* t)
{
	return t != NULL ? t->it*get_nbTS() + t->ts : 0;
}

/**
 * @brief convert sequencer_time_t to ntoa
 * @param t time to convert
 * @return number in ntoa
 */
OSAL_u32 stime2ntoa(sequencer_time_t* t)
{
	return t != NULL ? stime2ts(t)*NTOA_PER_TS + t->ntoa : 0;
}

OSAL_bool_t stime_normalize(sequencer_time_t* t)
{
	OSAL_u32 carry;

	if(t == NULL)
		return OSAL_false;

	
	carry = t->ntoa / NTOA_PER_TS;
	t->ntoa -= carry * NTOA_PER_TS;
	t->ts += carry;

	carry = t->ts / get_nbTS();
	t->ts -= carry * get_nbTS();
	t->it += carry;

	carry = t->it / get_PHS();
	t->it -= carry * get_PHS();	

	return OSAL_true;
}

//This function normalize a sequencer_time_t and enforce ts to be multiple of 64
//So that the time can be safely used with 32KHz generated from 62.4MHz
OSAL_bool_t stime_normalize_safe(sequencer_time_t* t)
{
	if(t == NULL)
		return OSAL_false;

	if(!stime_normalize(t))
		return OSAL_false;
	t->ntoa += NTOA_PER_TS*(t->ts%64);
	t->ts &= 0xFFFFFFC0;

	return OSAL_true;
}

/**
 * @brief add two protocol_time_t
 * @param a left operand of addition
 * @param b right operand of addition
 * @param r return value:  r = a + b
 *
 * @note r must be different from a and b (no in-place addition)
 * @note a and b must have the same zone
 * @return true on success, false otherwise
 */
OSAL_bool_t ptime_add(protocol_time_t* a,protocol_time_t* b, protocol_time_t* r)
{
	if(a == NULL || b == NULL || r == NULL)
		return OSAL_false;


	OSAL_memset(r,0, sizeof(protocol_time_t));
	r->zone = a->zone; // FIXME: what if a->zone != b->zone ?


	// FIXME: does modulo operations work correectly on IAR?
	r->ts = a->ts + b->ts;

	r->super_frame = r->ts / get_nbTS();
	r->ts %= get_nbTS();

	r->super_frame += a->super_frame + b->super_frame;
	r->hyper_frame = r->super_frame / get_PHS();
	r->super_frame %= get_PHS();

	r->hyper_frame += a->hyper_frame + b->hyper_frame;

	r->mega_frame = r->hyper_frame / to_u32(MAX_HYPER_FRAME+1);
	r->hyper_frame %= to_u32(MAX_HYPER_FRAME + 1);

	r->mega_frame += a->mega_frame + b->mega_frame;

	return OSAL_true;	
}

/**
 * @brief get the opposite of a protocol time. E.g. the protocol_time_t such that a + r = 0
 * @param a left operand of substraction
 * @param r return value:  r = -a, e.g. r is such that a + r = 0
 *
 * @return true on success, false otherwise
 */
OSAL_bool_t ptime_opposit(protocol_time_t*a, protocol_time_t* r)
{
	if(a == NULL || r == NULL)
		return OSAL_false;

	r->zone = a->zone;
	r->mega_frame = max_u32 - a->mega_frame + 1;
	r->hyper_frame = to_u32(MAX_HYPER_FRAME) - a->hyper_frame;
	r->super_frame = get_PHS()-1 - a->super_frame;
	r->ts = get_nbTS() - a->ts;
	return OSAL_true;
}

/**
 * @brief substract two protocol_time_t
 * @param a left operand of substraction
 * @param b right operand of substraction
 * @param r return value:  r = a - b
 *
 * @note r must be different from a and b (no in-place substraction)
 * @note a and b must have the same zone
 * @return true on success, false otherwise
 */
OSAL_bool_t ptime_sub(protocol_time_t* a,protocol_time_t* b, protocol_time_t* r)
{
	protocol_time_t opposit_of_b;

	if(a == NULL || b == NULL || r == NULL)
		return OSAL_false;

	if(!ptime_opposit(b, &opposit_of_b))
		return OSAL_false;

	return ptime_add(a, &opposit_of_b, r);
}


OSAL_bool_t ptime_normalize(protocol_time_t* t)
{
	OSAL_u32 carry;

	if(t == NULL)
		return OSAL_false;

	carry = t->ts / to_u32(get_nbTS());
	t->ts -= carry * to_u32(get_nbTS());
	t->super_frame += carry;

	carry = t->super_frame / to_u32(get_PHS());
	t->super_frame -= carry * to_u32(get_PHS());
	t->hyper_frame += carry;

	carry = t->hyper_frame / to_u32(MAX_HYPER_FRAME+1);
	t->hyper_frame -= carry * to_u32(MAX_HYPER_FRAME+1);	

	return OSAL_true;
}

/**
 * @brief tell wheter or not two protocol_time_t refere to the same time
 * @param a first protocol_time_t to compare
 * @param b second protocol_time_t to compare
 * @return true if both protocol_time_t refere to the same time, false otherwise
 */
OSAL_bool_t ptime_equal(protocol_time_t* a, protocol_time_t* b)
{
	return 	a != NULL && b != NULL &&
		a->zone == b->zone && 
		a->hyper_frame == b->hyper_frame &&
		a->super_frame == b->super_frame &&
		a->ts ==  b->ts;
}

OSAL_bool_t ptime_add_ts(protocol_time_t* a, OSAL_u32 ts, protocol_time_t* r)
{
	protocol_time_t b = {
		.ts = ts,
		.super_frame = 0,
		.hyper_frame = 0,
		.zone = a != NULL ? a->zone : 0,
	};
	ptime_normalize(&b);
	return ptime_add(a,&b,r);
}

OSAL_bool_t ptime_sub_ts(protocol_time_t* a, OSAL_u32 ts, protocol_time_t* r)
{
	protocol_time_t b = {
		.ts = ts,
		.super_frame = 0,
		.hyper_frame = 0,
		.zone = a != NULL ? a->zone : 0,
	};
	ptime_normalize(&b);
	return ptime_sub(a,&b,r);
}

OSAL_bool_t ptime_add_super_frame(protocol_time_t* a, OSAL_u32 super_frame, protocol_time_t* r)
{
	protocol_time_t b = {
		.ts = 0,
		.super_frame = super_frame,
		.hyper_frame = 0,
		.zone = a != NULL ? a->zone : 0,
	};
	return ptime_add(a,&b,r);
}

OSAL_bool_t ptime_sub_super_frame(protocol_time_t* a, OSAL_u32 super_frame, protocol_time_t* r)
{
	protocol_time_t b = {
		.ts = 0,
		.super_frame = super_frame,
		.hyper_frame = 0,
		.zone = a != NULL ? a->zone : 0,
	};
	return ptime_sub(a,&b,r);
}

OSAL_bool_t ptime_add_hyper_frame(protocol_time_t* a, OSAL_u32 hyper_frame, protocol_time_t* r)
{
	protocol_time_t b = {
		.ts = 0,
		.super_frame = 0,
		.hyper_frame = hyper_frame,
		.zone = a != NULL ? a->zone : 0,
	};
	return ptime_add(a,&b,r);
}

OSAL_bool_t ptime_sub_hyper_frame(protocol_time_t* a, OSAL_u32 hyper_frame, protocol_time_t* r)
{
	protocol_time_t b = {
		.ts = 0,
		.super_frame = 0,
		.hyper_frame = hyper_frame,
		.zone = a != NULL ? a->zone : 0,
	};
	return ptime_sub(a,&b,r);
}

/**
 * @brief convert protocol_time_t to it
 * @param t time to convert
 * @return number in it
 */
OSAL_u32 ptime2it(protocol_time_t* t)
{
	return t != NULL ? t->hyper_frame*get_PHS() + t->super_frame : 0;
}

/**
 * @brief convert protocol_time_t to ts
 * @param t time to convert
 * @return number in ts
 */
OSAL_u32 ptime2ts(protocol_time_t* t)
{
	return t != NULL ? ptime2it(t)*get_nbTS() + t->ts : 0;
}


/**
 * @brief get duration in ts beteen two protocol_time_t of the same zone
 * @param from starting protocol_time_t
 * @param to end protocol_time_t
 * @return number of ts between two protocol_time_t
 */
OSAL_u32 ptime_delta_in_ts(protocol_time_t* from, protocol_time_t* to)
{
	protocol_time_t delta;
	if(from == NULL || to == NULL)
		return 0;

	if(!ptime_sub(to,from,&delta))
		return 0;

	return ptime2ts(&delta);
}
/**
 * @brief get duration in it (super_frame) beteen two protocol_time_t of the same zone
 * @param from starting protocol_time_t
 * @param to end protocol_time_t
 * @return number of superframe between two protocol_time_t
 */
OSAL_u32 ptime_delta_in_it(protocol_time_t* from, protocol_time_t* to)
{
	protocol_time_t delta;
	if(from == NULL || to == NULL)
		return 0;

	if(!ptime_sub(to,from,&delta))
		return 0;

	return ptime2it(&delta);
}
