/**
 * @file framer_api.h
 * @brief header for framer and deframer functions
 * @author bgi@bespoon.com
 * @date 11/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _FRAMER_API_H_
#define _FRAMER_API_H_
#include <types.h>

/**
* @brief make the frame for the timer register
*
* @param smbuff : structure which contained the data use to make the frame
* @param seq_cmd: structure which will contained the framed data
*
* @return OSAL_error_t: say if the framer succeeded or not
*/
OSAL_error_t framer(smbuff_t * smbuff, seq_command_t* seq_cmd);

/**
* @brief set the size of the frame
*
* @param smbuff: structure which contained the data use to make the frame
* @param seq_cmd: structure which will contained the expected frame size
*
* @return OSAL_error_t: say if the framer succeeded or not
*
*/
OSAL_error_t set_size_frame(smbuff_t * smbuff_temp, seq_command_t* seq_cmd);

/**
* @brief extract the data from a frame
*
* @param smbuff : structure which store the data extract from the frame
* @param seq_cmd: structure which contained framed data
* @param deframed_tt: the type of frame that was deframed
* @param deframed_rdv: the type of sub-frame that was deframed if applicable
* @param ignore_size: boolean to says if  deframer should return an error if size of the received frames does not match
*
* @return OSAL_error_t: say if the deframer succeeded or not
*/
OSAL_error_t deframer(smbuff_t * smbuff, seq_command_t* seq_cmd, type_trame_t* deframed_tt, type_rdv_t* deframed_rdv, OSAL_bool_t ignore_size);


#endif
