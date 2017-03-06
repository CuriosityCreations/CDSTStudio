/**
 * @file module_api_seq.h
 * @brief api for module (sequencer part)
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef __MODULE_API_SEQ_H__
#define __MODULE_API_SEQ_H__

#ifdef CONFIG_GENERIC

/** @page module_api  Module API
 * 
 * @section module_api_sequencer Sequencer API
 * Sequencer api's allow to program UM100 Module sequencer \n
 * 
 * <tt><small><table width="400" border="1"  align="center">
 * <tr align="center">
 * <td align="center"><pre>                      Uses                     </pre></td>
 * <td align="center"><pre>     Function(s)    </pre></td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module sequencer initialization control</td>
 * <td align="center">#seq_init</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Sequencer general configuration control</td>
 * <td align="center">#seq_set_cfg, #seq_get_cfg_buffer, #seq_get_cfg, #seq_set_cfg_busy , #seq_is_cfg_busy</td>
 * </tr> 
 * <tr align="center">
 * <td align="center">Sequencer action to enqueue control</td>
 * <td align="center">#seq_queue_single_act, #seq_get_sequence_data</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Sequencer control</td>
 * <td align="center">#seq_ctrl_prepare, #seq_ctrl</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Sequencer process. \n THIS MUST BE CALLED IN MAINLOOP</td>
 * <td align="center">#seq_process</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Cache of sequencer api check (debug purpose)</td>
 * <td align="center">#seq_checkcache</td>
 * </tr>
 * </table></small></tt>
 * 
 * @latexonly 
 * \newpage
 * @endlatexonly
*/

/**
 * @brief seq_get_cfg_buffer
 * @note retrieve general sequencer configuration data location
 * @note this should be followed by #seq_set_cfg
 * @param cfgBuf data were to find cfg0,cfg1,cfg2
 * @param cfgBufSize size of data should be 3
*/
OSAL_void seq_get_cfg_buffer(OSAL_u8** cfgBuf, OSAL_u16* cfgBufSize);
/**
 * @brief seq_get_sequence_data
 * @note retrieve next sequencer cfg to enqueue
 * @note this should eb followed by #seq_queue_single_act
 * @param seqBuf data were to find sequence
 * @param seqBufSize size of data should be 12
*/
OSAL_void seq_get_sequence_data(OSAL_u8** seqBuf, OSAL_u16* seqBufSize);
/**
 * @brief seq_init
 * @note This command initialize sequencer
 * @return 1 if radio init failed, 0 otherwise
 */
OSAL_u8 seq_init();
/**
 * @brief seq_manage_hardFault
 * @note used when hardfault detected
 */
OSAL_void seq_manage_hardFault();
/**
 * @brief seq_is_cfg_busy
 * @note This command check if a setting of general sequencer configuration. is ongoing, but no yet changed
 * @return 1 if general sequencer configuration is ongoing
 */
OSAL_u8 seq_is_cfg_busy();
/**
 * @brief radio_set_rx_cfg_busy
 * @note call this command before changing sequencer configuration , to prevent conflict
 * @param busyState  sequencer configuration is ongoing
 */
OSAL_void seq_set_cfg_busy(OSAL_u8 busyState);
/**
 * @brief seq_get_cfg
 * @note This command is used to get the current general sequencer configuration.
 * @param cfg0 = 32K_TIMER_PERIOD_W0
 * @param cfg1 = 32K_TIMER_PERIOD_W1
 * @param cfg2 = 32K_FRAMERATE_LENGTH
 */
OSAL_void seq_get_cfg(OSAL_u8* cfg0, OSAL_u8* cfg1, OSAL_u8* cfg2);
/**
 * @brief seq_set_cfg
 * @note This command is used to set the current general sequencer configuration.
 * @note This should be called right after having programmed data retrieved through #seq_get_cfg_buffer
 * @note after this call, user should call for #seq_set_cfg_busy with 0
 */
OSAL_void seq_set_cfg();
/**
 * @brief seq_queue_single_act
 * @note This command is used to queue a single action to the sequencer.
 * @note This should be called right after having programmed data retrieved through #seq_get_sequence_data
 */
OSAL_void seq_queue_single_act();
/**
 * @brief seq_ctrl_prepare
 * @note This command is prepare #seq_ctrl (HOST spi api oriented).
 * @param ctrl type of sequencer ctrl to do @see uwb_com_subcmd_e
 */
OSAL_void seq_ctrl_prepare(OSAL_u8* ctrl);
/**
 * @brief seq_ctrl
 * @note This command and its sub command are used to start/stop the sequencer once configured. 
 * @param ctrl type of sequencer ctrl to do @see uwb_com_subcmd_e
 */
OSAL_void seq_ctrl(OSAL_u8 ctrl);
/**
 * @brief seq_process
 * @note sequencer process.  \n THIS MUST BE CALLED IN MAINLOOP
 */
OSAL_void seq_process();
/**
 * @brief seq_checkcache
 * @note Check if value in sys cache is the same than on hardware ( test only)
 * @param checkId  id of the cache value to check (relativelly to SEQUENCER)
 */
OSAL_void seq_checkcache(OSAL_u8 checkId);

#endif //#ifdef CONFIG_GENERIC

#endif //__MODULE_API_SEQ_H__

