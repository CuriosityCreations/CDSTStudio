/**
 * @file pp_conf_common.c
 * @brief Common functions to configure pinpointer
 * @author cca@bespoon.com
 * @date 10/10/2014
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include <osal_type.h>
#include <osal_trace.h>
#include <osal_math.h>
#include <conf_common_api.h>

OSAL_void pp_conf_get_sync_code_repetition_from_zone(OSAL_u8 zone, OSAL_u8* spread_code, OSAL_u8* repetition)
{
	// This part must be the same as in bsp_prodSectionManager: importProdSection.cpp: patchCSV()
	OSAL_u8 my_spread_code, my_repetition;
	switch(zone)
	{
		case 1 :
			my_spread_code = 15;
			my_repetition = 1; 
			break;
		case 2 :
			my_spread_code = 127;
			my_repetition = 1;
			break;
		case 3 :
			my_spread_code = 127;
			my_repetition = 5;
			break;
		case 4 :
		default:
			my_spread_code = 127;
			my_repetition = 8;
			break;
		case 5 :
			my_spread_code = 127;
			my_repetition = 64; 
			break;
	}

	if(spread_code != NULL)
		*spread_code = my_spread_code;

	if(repetition != NULL)
		*repetition = my_repetition;
}

OSAL_void pp_conf_get_demod_code_repetition_from_zone(OSAL_u8 zone, OSAL_u8* spread_code, OSAL_u8* repetition)
{
	// This part must be the same as in bsp_prodSectionManager: importProdSection.cpp: patchCSV()
	OSAL_u8 my_spread_code, my_repetition;
	switch(zone)
	{
		case 1 :
			my_spread_code = 7;
			my_repetition = 1; 
			break;
		case 2 :
			my_spread_code = 31;
			my_repetition = 1;
			break;
		case 3 :
			my_spread_code = 127;
			my_repetition = 1;
			break;
		default:
		case 4 :
			my_spread_code = 127;
			my_repetition = 1;
			break;
		case 5 :
			my_spread_code = 127;
			my_repetition = 8; 
			break;
	}

	if(spread_code != NULL)
		*spread_code = my_spread_code;

	if(repetition != NULL)
		*repetition = my_repetition;
}

OSAL_u8 pp_conf_get_preamble_length(OSAL_u8 synch_code_lgth, OSAL_u8 synch_repetition_nb)
{
	if (synch_code_lgth < 7 || synch_code_lgth > 127 || synch_repetition_nb > 64)
	{
		OSAL_trace(TRACE_ERROR, "GET:%d/%d:BAD SYNC CODE OR REP", synch_code_lgth, synch_repetition_nb);
		return 0;
	}

	else if ((synch_code_lgth * synch_repetition_nb) <= 7)
		return 63;

	else if ((synch_code_lgth * synch_repetition_nb) <= 15)
		return 58;

	else if ((synch_code_lgth * synch_repetition_nb) <= 31)
		return 56;

	else if ((synch_code_lgth * synch_repetition_nb) <= 63)
		return 55;

	else if ((synch_code_lgth * synch_repetition_nb) <= 127)
		return 54;

	else if ((synch_code_lgth * synch_repetition_nb) <= 127*4)
		return 51;
	else
		return 50;
}

// auto swap to n
static OSAL_u16 auto_swap( OSAL_u16 w )
{
	OSAL_u16 check = 1;
	if (*((OSAL_u8*)&check))
	{   // little endian : checked ok on PC and Android.
		return w;
	}
	else
	{   // big endian : not yet checked.
		return ( ( w << 8 ) | ( w >> 8 ) );
	}
}

// uint16_t crc_rtu(const uint8_t* buf, size_t len), from RTU Modbus
// this fonction compute an polynomial euclidian division rest for generator
/*
 *  usage : crc(const uint8_t* buf, size_t len)
 *  buff : data, raw data in any format.
 *  len : data lenth to compute crc on.
 *
 *
 * eg : compute crc rest
 * crc16 = crc(&data, sizeof(data) -2 )
 *
 * you can simply check the crc with a syndrom compulation.
 *  Syndrom is "compute all data, including crc field, at end, in correct endianess order.
 *  Syndrom must be null if crc is correct, not null on error.
 *
 * eg : compute syndrom
 * syndrm = crc(&data, sizeof(data) )
 * if ( syndrm != 0 )
 *  ERROR() !
 *
  */
OSAL_u16 pp_conf_crc16(OSAL_u8 OSAL_PROD_SECT * buf, OSAL_u16 len)
{
	OSAL_u16 crc = 0xFFFF;
	OSAL_u16 generator = 0xA001;
	OSAL_u16 i = 0;
	OSAL_u16 pos;

	for (pos = 0 ; pos < len ; pos++)
	{
		crc ^= (OSAL_u16)buf[pos];          // XOR byte into least sig. byte of crc
		for (i = 8; i != 0; i--)
		{    // Loop over each bit
			if (crc & 0x0001)
			{      // If the LSB is set
				crc >>= 1;                    // Shift right and XOR 0xA001
				crc ^= generator;
			}
			else                            // Else LSB is not set
				crc >>= 1;                    // Just shift right
		}
	}
	// Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
	return auto_swap(crc);
}

OSAL_u32 threshold_model(OSAL_u8 sync_code, OSAL_u8 sync_rep)
{
#define NB_COEFFS 7
	OSAL_u8 i;
	OSAL_u32 C;
	OSAL_u32 R, R2;
	OSAL_s32 tmp_th = 0;
	OSAL_u32 th = 0;
	OSAL_u32 factors[NB_COEFFS];
	
	//model parameters
	//each line corresponds to one channel line 1 channel 1, line 2 channel 2 etc...)
	//model is a polynom of fourth order in the rep and snd order in code
	//the coeffs are multiplied by prec_factor, result should be devided by prec_factor
	OSAL_s32 model_params[NB_COEFFS] = {-1, -1, 287, 1450, -1, 331, 1670};

	//precompute factors
	C = sync_code;
	R = sync_rep;
	R2 = R * R;
	
	//maybe not the best way to do that
	factors[0] = C*C;
	factors[1] = R2*C;
	factors[2] = R*C;
	factors[3] = C;
	factors[4] = R2;
	factors[5] = R;
	factors[6] = 1;
	
	//compute the resulting threshold in lin val
	for(i = 0; i < NB_COEFFS; i++)
		tmp_th += model_params[i]*factors[i];

	th = tmp_th;	//convert s32 to u32

	return th;
}

OSAL_u32 threshold_from_pg_model( OSAL_u8 sync_code, OSAL_u8 sync_rep, 
				  OSAL_u32 sync_thld_ref, OSAL_u8 sync_code_ref, 
				  OSAL_u8 sync_rep_ref)

{
	OSAL_u32 prec_factor =  10; //max possible shift with this model is 11
	OSAL_u32 th_model_at_ref_coord = 0;
	OSAL_u64 th_64 = 0;
	OSAL_u32 th_model = 0; 
	OSAL_u32 th = 0;

	//get the value that model gives for reference code and rep,
	//usefull to compute the scale factor
	th_model_at_ref_coord = threshold_model(sync_code_ref, sync_rep_ref);

	//get the thld from model at desired channel, code and rep
	th_model = threshold_model(sync_code, sync_rep);

	//compute new thld, using the scale factor sync_thld_ref/th_model_at_ref_coord
	//note : scale factor equivalent to offset in dB 
	//be carefull with operation order

	// Note: bit shifting is equivalent of multiplication/division by a power of 2
	// However with u64, it is way faster to do bit shifting than division

	th = th_model << prec_factor;
	th /= th_model_at_ref_coord;
	th_64 = (OSAL_u64)th*(OSAL_u64)sync_thld_ref;
	th = (OSAL_u32)(th_64 >> prec_factor);

	return th;
}
