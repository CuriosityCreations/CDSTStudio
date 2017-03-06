/**
 * @file osal_dac.h
 * @brief Definitions for <b>O</b>perating <b>S</b>ystem <b>A</b>bstraction <b>L</b>ayer access to <b>D</b>igital to <b>A</b>nalog <b>C</b>onverter
 * @author BeSpoon
 * @date 09/04/2015
 */
/*
 * Copyright (C) Bespoon 2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#include "HAL/dac.h"

/**
 * @brief OSAL_dac_get_value() get the value of Digital to Analog Converter in microvolt (uV)
 * @param out_dac_value pointer to a u32 value to store tension of DAC, in microvolt
 */
#define OSAL_dac_get_value(out_dac_value) dac_get_valueUV(out_dac_value)

/**
 * @brief OSAL_dac_set_value() get the value of Digital to Analog Converter in microvolt (uV)
 * @param dac_value new tension value to set to the DAC in microvolt
 */
#define OSAL_dac_set_value(dac_value) dac_set_valueUV(dac_value)
