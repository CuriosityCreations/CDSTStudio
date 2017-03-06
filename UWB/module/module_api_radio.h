/**
 * @file module_api_radio.h
 * @brief api for module (radio part)
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef __MODULE_API_RADIO_H__
#define __MODULE_API_RADIO_H__

#ifdef CONFIG_GENERIC
/** @page module_api  Module API
 * 
 * @section module_api_radio Radio API
 * Radio api's allows to control UWB chip configuration and physical settings\n
 * 
 * <tt><small><table width="400" border="1"  align="center">
 * <tr align="center">
 * <td align="center"><pre>                      Uses                     </pre></td>
 * <td align="center"><pre>     Function(s)     </pre></td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module radio initialization control</td>
 * <td align="center">#radio_is_init_busy,#radio_msq_init,#radio_init</td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module SPI communication directly to pinpointer control</td>
 * <td align="center">#radio_spi_write, #radio_spi_read, #radio_spi_write_get_buf, #radio_set_spiwr_busy, #radio_is_spiwr_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module PM State control</td>
 * <td align="center">#radio_set_pmstate, #radio_get_pmstate, #radio_is_pmstate_busy, #radio_set_pmstate_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module Phy config control</td>
 * <td align="center">#radio_set_phy_cfg, #radio_get_phy_cfg, #radio_get_phy_cfg_data, #radio_is_phy_cfg_busy, #radio_set_phy_cfg_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module RX Config control</td>
 * <td align="center">#radio_set_rx_cfg, #radio_get_rx_cfg_data, #radio_get_rx_cfg, #radio_set_rx_cfg_busy, #radio_is_rx_cfg_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Reset the 16ns granularity main ranging time</td>
 * <td align="center">#radio_clear_rng_timer</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Radio TX Buffer control</td>
 * <td align="center">#radio_get_tx_data_buf, #radio_set_tx_data_buf_size, #radio_write_tx_data</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Radio RX Buffer control</td>
 * <td align="center">#radio_get_rx_data_size, #radio_get_rx_data_checksum, #radio_get_rx_buf</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Radio state control</td>
 * <td align="center">#radio_phy_ctrl_prepare, #radio_phy_ctrl</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Radio process. \n THIS MUST BE CALLED IN MAINLOOP</td>
 * <td align="center">#radio_process</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Cache of radio api check (debug purpose)</td>
 * <td align="center">#radio_checkcache</td>
 * </tr>
 * </table></small></tt>
 * 
 * @latexonly 
 * \newpage
 * @endlatexonly
*/
#else
/** @page module_api  Module API
 * 
 * @section module_api_radio Radio API
 * Radio api's allows to control UWB chip configuration and physical settings\n
 * 
 * <tt><small><table width="400" border="1"  align="center">
 * <tr align="center">
 * <td align="center"><pre>                  Uses                 </pre></td>
 * <td align="center"><pre>                  Function(s)                 </pre></td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module radio initialization control</td>
 * <td align="center">#radio_is_init_busy,\n
 * #radio_msq_init,\n
 * #radio_init</td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module SPI communication directly to pinpointer control</td>
 * <td align="center">#radio_spi_write,\n
 * #radio_spi_read,\n
 * #radio_spi_write_get_buf,\n
 * #radio_set_spiwr_busy,\n
 * #radio_is_spiwr_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module PM State control</td>
 * <td align="center">#radio_set_pmstate,\n
 * #radio_get_pmstate,\n
 * #radio_is_pmstate_busy,\n
 * #radio_set_pmstate_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module Phy config control</td>
 * <td align="center">#radio_set_phy_cfg,\n
 * #radio_get_phy_cfg,\n
 * #radio_get_phy_cfg_data,\n
 * #radio_is_phy_cfg_busy,\n
 * #radio_set_phy_cfg_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module RX Config control</td>
 * <td align="center">#radio_set_rx_cfg,\n
 * #radio_get_rx_cfg_data ,\n
 * #radio_get_rx_cfg ,\n
 * #radio_set_rx_cfg_busy ,\n
 * #radio_is_rx_cfg_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Cache of radio api check (debug purpose)</td>
 * <td align="center">#radio_checkcache</td>
 * </tr>
 * </table></small></tt>
 * @latexonly 
 * \newpage
 * @endlatexonly
*/
#endif
/**
 * @brief radio_is_init_busy
 * @note This command check if radio is initiliasing. In this case, no further action can be done
 * @return 1 if radio init is ongoing
 */
OSAL_u8 radio_is_init_busy();
/**
 * @brief radio_set_init_busy
 * @note call this command before init, to prevent conflict
 * @param busyState  init is ongoing
 */
OSAL_void radio_set_init_busy(OSAL_u8 busyState);
/**
 * @brief radio_msq_init
 * @note This command initialize msq, backing up previous msq state
 * @return 1 if radio_msq_init failed, 0 otherwise
 */
OSAL_u8 radio_msq_init(OSAL_bool_t previous_msq);
/**
 * @brief radio_showCurPhyAndDac
 * @note This command show current radio phy config and dac value
 * @note when radio init terminate, #radio_is_init_busy should return 0
 * @param forceShow if true, show it, whatever trace level. Otherwise, show it as trace_info level ( trace activated needed)
 */
OSAL_void radio_showCurPhyAndDac(int forceShow);
/**
 * @brief radio_init
 * @note This command initialize radio
 * @note when radio init terminate, #radio_is_init_busy should return 0
 * @return 1 if radio init failed, 0 otherwise
 */
OSAL_u8 radio_init();
/**
 * @brief radio_manage_hardFault
 * @note used when hardfault detected
 */
OSAL_void radio_manage_hardFault();
/**
 * @brief radio_is_spiwr_busy
 * @note This command check if a spi (read/write to pinpointer) read/write is about to be done, but no yet changed
 * @return 1 if a spi read/write is ongoing
 */
OSAL_u8 radio_is_spiwr_busy();
/**
 * @brief radio_set_spiwr_busy
 * @note call this command before doing a spi read/write, to prevent conflict
 * @param busyState  spi read/write change is ongoing
 */
OSAL_void radio_set_spiwr_busy(OSAL_u8 busyState);
/**
 * @brief radio_spi_write_get_buf
 * @note get buffer where to store next spi write data 
 * @note this funciton must be called prior to call for #radio_spi_write (com with host)
 * @param data  spi write data buffer
 * @param burstLen  spi write data buffer len
 */
OSAL_void radio_spi_write_get_buf(OSAL_u8** data,OSAL_u16 burstLen);
/**
 * @brief radio_spi_write
 * @note write a buffer into pinpointer (burst)
 * @note This command is used to perform a direct write to UWB chip registers.
 * @param data  spi write data buffer
 * @param burstLen  spi write data buffer len
 */
OSAL_void radio_spi_write(OSAL_u8* data,OSAL_u16 burstLen);
/**
 * @brief radio_spi_read
 * @note get buffer where to read data of pinpointer can be retrieved.This command is used to directly read UWB chip registers.
 * @note a read can be done only after a write
 * @param data  spi read data buffer
 * @param burstLen  spi read data buffer len
 * @param dataLenWithAck  spi read data buffer len +1 for acknoledge (spi communication with host oriented)
 */
OSAL_void radio_spi_read(OSAL_u8** data,OSAL_u16 burstLen,OSAL_u16* dataLenWithAck);
/**
 * @brief radio_is_pmstate_busy
 * @note This command check if pm state change is ongoing, but no yet changed
 * @return 1 if pm state change is ongoing
 */
OSAL_u8 radio_is_pmstate_busy();
/**
 * @brief radio_set_pmstate_busy
 * @note call this command before changing  pm state ( changing pm state is ongoing), to prevent conflict
 * @param busyState  changing pm state is ongoing
 */
OSAL_void radio_set_pmstate_busy(OSAL_u8 busyState);
/**
 * @brief radio_get_pmstate
 * @note This command is used to get the current pm state.This command is used to get the current UWB chip state
 * @param state get statusmask dac
 */
OSAL_void radio_get_pmstate(uwb_com_subcmd_e* state);
/**
 * @brief radio_set_pmstate
 * @note set new pm state.This command is used to set new UWB chip state
 * @note pm can be :
 * - UWB_SC_PM_3         Power managment state 3 (used only at startup, dont use this) \n
 * - UWB_SC_PM_2         Power managment state 2 \n
 * - UWB_SC_PM_0         Power managment state 0 \n
 * - UWB_SC_PM_STDBY     Power managment state STANDBY  \n
 * - UWB_SC_PM_RX_IDLE   Power managment state RX_IDLE  \n
 * - UWB_SC_PM_TX_IDLE   Power managment state TX_IDLE  \n
 * - UWB_SC_PM_TXRX_IDLE Power managment state TXRX_IDLE  \n
 * @param state new  pm state val to set
 */
OSAL_void radio_set_pmstate(uwb_com_subcmd_e state);
/**
 * @brief radio_is_phy_cfg_busy
 * @note This command check if phy config setting is ongoign, but no yet changed
 * @return 1 if phy config setting is ongoing
 */
OSAL_u8 radio_is_phy_cfg_busy();
/**
 * @brief radio_set_phy_cfg_busy
 * @note call this command before changing  phy config setting ( changing phy config setting is ongoing), to prevent conflict
 * @param busyState  changing phy config setting is ongoing
 */
OSAL_void radio_set_phy_cfg_busy(OSAL_u8 busyState);
/**
 * @brief radio_get_phy_cfg_data
 * @note get buffer where phy config is stored (spi communication wiht host)
 * @param buf phy config setting buffer
 */
OSAL_void radio_get_phy_cfg_data(OSAL_u8** buf);
/**
 * @brief radio_get_phy_cfg
 * @note This command is used to get current UWB chip configuration.
 * @param phy_cfg current phy config
 * @param phy_cfg_size phy config size
 */
OSAL_void radio_get_phy_cfg(OSAL_u8** phy_cfg,OSAL_u16 phy_cfg_size);
/**
 * @brief radio_set_phy_cfg
 * @note This command is used to set current UWB chip configuration.
 * @param phy_cfg new phy config to set
 * @param phy_cfg_size phy config size
 */
OSAL_void radio_set_phy_cfg(OSAL_u8* phy_cfg,OSAL_u16 phy_cfg_size);
/**
 * @brief radio_is_rx_cfg_busy
 * @note This command check if rx config is ongoign, but no yet changed
 * @return 1 if phy config setting is ongoing
 */
OSAL_u8 radio_is_rx_cfg_busy();
/**
 * @brief radio_set_rx_cfg_busy
 * @note call this command before changing  rx config ( changing rx config is ongoing), to prevent conflict
 * @param busyState  changing rx config is ongoing
 */
OSAL_void radio_set_rx_cfg_busy(OSAL_u8 busyState);
/**
 * @brief radio_get_rx_cfg_data
 * @note get buffer where rx config is stored (spi communication wiht host)
 * @param rx_cfg rx config setting buffer
 * @param rxcfgSize rx config size
 */
OSAL_void radio_get_rx_cfg_data(OSAL_u8** rx_cfg, OSAL_u16* rxcfgSize);
/**
 * @brief radio_get_rx_cfg
 * @note get rx config.This command is used to get the type of data send by the module to the Host MCU.
 * @param rx_cfg rx config setting
 * @param rx_dataSizeInBits rx config size
 */
OSAL_void radio_get_rx_cfg(OSAL_u8* rx_cfg, OSAL_u16* rx_dataSizeInBits);
/**
 * @brief radio_set_rx_cfg
 * @note get rx config.This command is used to set the type of data send by the module to the Host MCU. 
 * @param rx_cfg rx config setting
 * @param rx_dataSizeInBits rx config size
 */
OSAL_void radio_set_rx_cfg(OSAL_u8 rx_cfg, OSAL_u16 rx_dataSizeInBits);

#ifdef CONFIG_GENERIC
/**
 * @brief radio_clear_rng_timer
 * @note This command is used to reset the 16ns granularity main ranging time
 */
OSAL_void radio_clear_rng_timer();
/**
 * @brief radio_set_tx_data_buf_size
 * @note This command is used to set sent TX data size
 * @param sizeInBytes TX data size in bytes
 */
OSAL_void radio_set_tx_data_buf_size(OSAL_u16 sizeInBytes);
/**
 * @brief radio_get_tx_data_buf
 * @note Get buffer whedre to store TX data  (spi communication wiht host)
 * @param data TX data buffer
 * @param sizeInBytes TX data size in bytes
 */
OSAL_void radio_get_tx_data_buf(OSAL_u8** data,OSAL_u16* sizeInBytes);
/**
 * @brief radio_write_tx_data
 * @note This command is used to write data to the UWB radio TX FIFO. This data will be sent at next call to RADIO_PHY_CTRL (start) command.
 * @param data TX data buffer. If NULL, the write TX will be base on previsou deata set through radio_get_tx_data_buf
 * @param sizeInBits TX data size in BITS
 */
OSAL_void radio_write_tx_data(OSAL_u8* data,OSAL_u16 sizeInBits);
/**
 * @brief radio_get_rx_data_size
 * @note This command is used to get the size of the RX transmission.
 * @param dataSizeWithoutAck retrieve RX size, or error
 */
OSAL_void radio_get_rx_data_size(OSAL_u16* dataSizeWithoutAck);
/**
 * @brief radio_get_rx_data_checksum
 * @note This command is used to get the checksum for the RANGING data read before sending to HOST
 * @param checksum retrieve RX data checksum
 */
OSAL_void radio_get_rx_data_checksum(OSAL_u16* checksum);
/**
 * @brief radio_get_rx_buf
 * @note This command is used to get the buffer where RX data are stored
 * @param buf buffer with RXData+1 (ack for ost communication).Rx_data are then in &buf[1]
 * @param dataSizeWithAck size of RX data to transmit to host. Rx data size(in bytes) is dataSizeWithAck-1
 */
OSAL_void radio_get_rx_buf(OSAL_u8** buf,OSAL_u16* dataSizeWithAck);
/**
 * @brief radio_phy_ctrl_prepare
 * @note This command is prepare radio ctrl (HOST spi api oriented). 
 * @param ctrl type of radio ctrl to do @see uwb_com_subcmd_e
 * @param rxOrTx 1 is TX, 2 is RX, 0, canceled
 */
OSAL_void radio_phy_ctrl_prepare(OSAL_u8 ctrl,OSAL_u16* rxOrTx);
/**
 * @brief radio_phy_ctrl
 * @note This command, coupled with its sub-command is used to start UWB radio TX and RX transmission
 * @param ctrl type of radio ctrl to do @see uwb_com_subcmd_e
 * @param txOrx 1 is TX, 2 is RX, 0, canceled
 */
OSAL_void radio_phy_ctrl(OSAL_u8 ctrl,OSAL_u8 txOrx);
/**
 * @brief radio_process
 * @note radio process.  \n THIS MUST BE CALLED IN MAINLOOP
 */
OSAL_void radio_process();
#endif //#ifdef CONFIG_GENERIC

/**
 * @brief radio_checkcache
 * @note Check if value in sys cache is the same than on hardware ( test only)
 * @param checkId  id of the cache value to check (relativelly to RADIO)
 */
OSAL_void radio_checkcache(OSAL_u8 checkId);



#endif //__MODULE_API_RADIO_H__

