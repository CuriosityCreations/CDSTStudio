/**
 * @file module_api_sys.h
 * @brief api for module (sys part)
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef __MODULE_API_SYS_H__
#define __MODULE_API_SYS_H__


 #ifdef CONFIG_GENERIC
/** @page module_api  Module API
 * 
 * This chapter gives explanation on internal api to control module.
 *  
 * For this version of UWB module, commands are grouped in three categories:\n
 * 
 * <tt><small><table width="400" border="1"  align="center">
 * <tr align="center">
 * <td align="center"><pre>      Type of command     </pre></td>
 * <td align="center"><pre>      Description     </pre></td>
 * <td align="center"><pre>      Firmware required     </pre></td>
 * <td align="center"><pre>      module_api_sys     </pre></td>
 * </tr>
 * <tr align="center">
 * <td align="center">System</td>
 * <td align="center">Global UM100 module configuration and status</td>
 * <td align="center">GENERIC or PROTOCOL</td>
 * <td align="center">@ref module_api_sys</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Radio</td>
 * <td align="center">UM100 Radio control (complete)</td>
 * <td align="center">GENERIC</td>
 * <td align="center">@ref module_api_radio</td>
 * </tr>
  * <tr align="center">
 * <td align="center">Sequencer</td>
 * <td align="center">UM100 Sequencer control</td>
 * <td align="center">GENERIC</td>
 * <td align="center">@ref module_api_sequencer</td>
 * </tr>
 * </table></small></tt>
 */
#else
/** @page module_api  Module API
 * 
 * This chapter gives explanation on internal api to control module.
 *  
 * For this version of UWB module, commands are grouped in three categories:\n
 * 
 * <tt><small><table width="400" border="1"  align="center">
 * <tr align="center">
 * <td align="center"><pre>      Type of command     </pre></td>
 * <td align="center"><pre>      Description     </pre></td>
 * <td align="center"><pre>      Firmware required     </pre></td>
 * <td align="center"><pre>      module_api_sys     </pre></td>
 * </tr>
 * <tr align="center">
 * <td align="center">System</td>
 * <td align="center">Global UM100 module configuration and status</td>
 * <td align="center">GENERIC or PROTOCOL</td>
 * <td align="center">@ref module_api_sys</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Radio</td>
 * <td align="center">UM100 Radio control(reduced)</td>
 * <td align="center">PROTOCOL</td>
 * <td align="center">@ref module_api_radio</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Protocol</td>
 * <td align="center">UPosition/BeSpoon protocol control</td>
 * <td align="center">PROTOCOL <small>(1)</small></td>
 * <td align="center">@ref module_api_prot</td>
 * </tr>
 * </table></small></tt>
 * <small>(1) Available under specific license with BeSpoon only.</small>
 */
#endif
 /** @page module_api  Module API
 * 
 * @section module_api_sys System API
 * System api's allow to control MCU, and UWB chip.\n
 * It allow initialization, Configuration of IT, deep slepp mode control
 * Sys api proposes some other convenient functions for timing, and share memory.
 * 
 * <tt><small><table width="400" border="1"  align="center">
 * <tr align="center">
 * <td align="center"><pre>                      Uses                     </pre></td>
 * <td align="center"><pre>        Function(s)     </pre></td>
 * </tr>
 * <tr align="center">
 * <td align="center">UM100 Module initialization</td>
 * <td align="center">#sys_init, #sys_reinit</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Get information on MCU software or hardware</td>
 * <td align="center">#sys_get_info</td>
 * </tr>
 * <tr align="center">
 * <td align="center">MSQ Control</td>
 * <td align="center">#sys_enable_msq, #sys_enable_pm</td>
 * </tr>
 * <tr align="center">
 * <td align="center">MCU Deep sleep wake up condition</td>
 * <td align="center">#sys_get_wake_up_reason, #sys_set_wake_up_reason, #sys_reset_wake_up_reason</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Check and set MCU to enter in deep sleep mode</td>
 * <td align="center">#sys_set_mcu_state, #sys_is_mcu_stopped, #sys_stop_mcu, #sys_force_mcu_run</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Deep sleep dimmer control</td>
 * <td align="center">#sys_authorizesleep_after_X_sec, #sys_authorizesleep_after_X_msec, #sys_authorizesleep_after_X_usec</td>
 * </tr>
 * <tr align="center">
 * <td align="center">DAC value control</td>
 * <td align="center">#sys_set_dac_val, #sys_get_dac_val, #sys_set_dac_val_busy, #sys_is_dac_val_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">CAPATUNE control</td>
 * <td align="center">#sys_set_capatune_val, #sys_get_capatune_val, #sys_is_capatune_busy, #sys_set_capatune_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">RX VFrame config control</td>
 * <td align="center">#sys_set_rxframe_cfg, #sys_get_rxframe_cfg, #sys_is_rxframe_cfg_busy, #sys_set_rxframe_cfg_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Status mask control</td>
 * <td align="center">#sys_set_status_mask, #sys_get_status_mask, #sys_is_statusmsk_busy, #sys_set_statusmsk_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Status control</td>
 * <td align="center">#sys_get_status, #sys_set_status, #sys_reset_status, #sys_check_status, #sys_reset_status</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Cache of sys api check (debug purpose)</td>
 * <td align="center">#sys_checkcache, #sys_get_checkcache, #sys_set_checkcache_busy, #sys_is_checkcache_busy</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Process any system action. THIS MUST BE CALLED IN MAIN-LOOP</td>
 * <td align="center">#sys_process</td>
 * </tr>
 * <tr align="center">
 * <td align="center">Switch ON/OFF LED depending on protocol state(PROTOCOL RELEASE ONLY)</td>
 * <td align="center">#sys_led_management</td>
 * </tr>
 * </table></small></tt>
 * 
 * @latexonly 
 * \newpage
 * @endlatexonly
*/

/**
 * @brief sys_get_mcuid
 * @note get stm32/bespoon id
 * @param unique_id a table at least sized 12.If NULL, this function is used to check if it match with prodSection
 * @return 0 if unique_id not NULL or if unique_id is NULL and mcuid match with prodsection, 1 if unique_id is NULL and mcuid do not match with prodsection 
 */
OSAL_u8 sys_get_mcuid(OSAL_u8* unique_id);
/**
 * @brief sys_enable_msq
 * @note enable msq 
 * @param dfltMSQ_EN  msq enabled if >0, disabled otherwise 
 */
OSAL_void sys_enable_msq(OSAL_bool_t dfltMSQ_EN);
/**
 * @brief sys_enable_pm
 * @note enable PM done by MSQ 
 * @param dfltPM_EN  msq enabled if >0, disabled otherwise 
 */
OSAL_void sys_enable_pm(OSAL_bool_t dfltPM_EN);
/**
* @brief sys_enable_lta
 * @note enable live threshold adjustment ( default: on)
 * @param dfltLTA_EN: OSAL_true to enable LTA, OSAL_false to disable it
  */
OSAL_void sys_enable_lta(OSAL_bool_t dfltLTA_EN);
/**
 * @brief sys_is_init_busy
 * @note This command check if sys is initiliasing. In this case, no further action can be done
 * @return 1 if sys init/reinit is ongoing
 */
OSAL_u8 sys_is_init_busy();
/**
 * @brief sys_set_init_busy
 * @note call this command before init, to prevent conflict
 * @param busyState  init is ongoing
 */
OSAL_void sys_set_init_busy(OSAL_u8 busyState);
/**
 * @brief sys_reinit
 * @note reinitialize the MCU, and UWB chip.
 * @return 1 if reinit failed, 0 otehrwise
 */
OSAL_u8 sys_reinit();
/**
 * @brief sys_init
 * @note initialize the MCU, and UWB chip. IT must be called in main loop. Otehr init must use #sys_reinit
 * @return 1 if reinit failed, 0 otehrwise
 */
OSAL_u8 sys_init(OSAL_u8** errMsg);
/**
 * @brief sys_manage_hardFault
 * @note used when hardfault detected
 * @param hardfault_args
 */
OSAL_void sys_manage_hardFault(unsigned long *hardfault_args);
/**
 * @brief sys_authorizesleep_after_X_sec
 * @note Wait for a certain dimmer before authorizing entering in deep sleep mode. 
 * @param timeInSec  time in second before authorizing sleep. cannot be more or equal to 3600 seconds ( 1 hour)
 * @param wake_up_reason  indicate which dimmer is selected ( one dimmer by wake up reason)
 */
OSAL_void sys_authorizesleep_after_X_sec(OSAL_u32 timeInSec, OSAL_u8 wake_up_reason );
/**
 * @brief sys_authorizesleep_after_X_msec
 * @note Wait for a certain dimmer before authorizing entering in deep sleep mode. 
 * @param timeInMSec  time in millisecond before authorizing sleep. cannot be more than 3600000 seconds ( 1 hour)
 * @param wake_up_reason  indicate which dimmer is selected ( one dimmer by wake up reason)
 */
OSAL_void sys_authorizesleep_after_X_msec(OSAL_u32 timeInMSec, OSAL_u8 wake_up_reason );
/**
 * @brief sys_authorizesleep_after_X_usec
 * @note Wait for a certain dimmer before authorizing entering in deep sleep mode. 
 * @param timeInUSec  time in micorsecond before authorizing sleep. cannot be more than 3600000000 seconds ( 1 hour)
 * @param wake_up_reason  indicate which dimmer is selected ( one dimmer by wake up reason)
 */
OSAL_void sys_authorizesleep_after_X_usec(OSAL_u32 timeInUSec, OSAL_u8 wake_up_reason );
/**
 * @brief sys_get_wake_up_reason
 * @note get reason why MCU has been awaken
 * @param reason  masked: bitfield of wake up reason @see uwb_com_wake_up_reason_e
 */
OSAL_void sys_get_wake_up_reason(OSAL_u8* reason);
/**
 * @brief sys_set_wake_up_reason
 * @note set reason why MCU has been awaken
 * @param mask  wkae up reason @see uwb_com_wake_up_reason_e
 */
OSAL_void sys_set_wake_up_reason(OSAL_u8 mask);
/**
 * @brief sys_reset_wake_up_reason
 * @note reset a reason ( or various reason) why MCU has been awaken
 * @param mask  wkae up reason @see uwb_com_wake_up_reason_e
 */
OSAL_void sys_reset_wake_up_reason(OSAL_u8 mask);
/**
 * @brief sys_get_info
 * @note get information on MCU software or hardware
 * @param infoType type of info to retrieve @see uwb_com_subcmd_e
 * @param infoPrm parameter for info ( used only if infoType = #UWB_SC_INFO_THRESH)
 * @param infoBuf pointer on retirved info
 * @param infoBufLen size of info
 */
OSAL_void sys_get_info(OSAL_u8 infoType, OSAL_u8 infoPrm, OSAL_u8** infoBuf, OSAL_u16* infoBufLen);
/**
 * @brief sys_is_mcu_stopped
 * @note check if mcu is under deep sleep ( can be used under IT only)
 * @return 1 if mcu is in deep sleep mode
 */
OSAL_u8 sys_is_mcu_stopped();
/**
 * @brief sys_stop_mcu
 * @note check if mcu can enter in deep sleep, and put it in deep sleep
 */
OSAL_void sys_stop_mcu();

/**
 * @brief sys_force_mcu_run
 * @note forbid mcu to enter in deepo sleep, overriding #sys_set_mcu_state
 * @note can be used typically if a hardware line forbid deep sleep (hardware configuration dependant)
 * @param forceRun 1:forbid deep sleep
 */
OSAL_void sys_force_mcu_run(OSAL_u8 forceRun);
/**
 * @brief sys_set_mcu_state
 * @note This command is used to set the module MCU comportment regarding sleep mode. 
 * @note This command is used to do a reset
 * @param state deep sleep choice/reset type
 */
OSAL_void sys_set_mcu_state(uwb_com_subcmd_e state);
/**
 * @brief sys_rtc_wake_up_after
 * @note Enter in deep sleep, let it be awaken after some seconds/milliseconds.all IT are still managed (SPI, uart, AND pinpointer)
 * @note so, prior to enter in this deep sleep, pinpointer is in low power mode, or it will consummate
 * @param seconds   seconds after which sleep must stop
 * @param useconds  micro seconds after which sleep must stop. It will be rounded to the nearest multiple of (1/(256*factor) seconds)
 */
OSAL_void sys_rtc_wake_up_after(OSAL_u32 seconds, OSAL_u32 useconds);
/**
 * @brief sys_rtc_set_fineCorrection
 * @note set fine tune correction of RTC  sleep precision
 * @note correction are Q32 on 64bits, aka mean 32 MSB bits tcode integer part, 32lsb bits code floating part
 * @param fineTune [in] current integer tune value to set. Between 0 and (0xFFFFFFFFffffffff).
 */
OSAL_void sys_rtc_set_fineCorrection(OSAL_u64 fineTune);
/**
 * @brief sys_rtc_get_corrections
 * @note get fine tune correction of RTC  sleep precision and base frequencie correction ( =1 for LSE, various for LSI)
 * @note corrections are Q32 on 64bits, aka mean 32 MSB bits tcode integer part, 32lsb bits code floating part
 * @param freqCorrection [out] if given, it store the base frequencie correction ( =1 for LSE, various for LSI)
 * @param tuneCorrection [out] if given, it store the fine tune correction of RTC  sleep precision
 */
OSAL_void sys_rtc_get_corrections(OSAL_u64* freqCorrection, OSAL_u64* tuneCorrection);
/**
 * @brief sys_rtc_set_precisionFactor
 * @note set current precision factor.  that will be set once RTC has been reinitialized. defautl value 16.
 * @note The higher the value is, the higher subsecond precision we will have ( precision = 1 / (256*factor) seconds)
 * @note but cunsumption will be increased
 * @param factor [in] precision factor value to set. A power of 2 value in 1,2,4,8,...128,
 */
OSAL_void sys_rtc_set_precisionFactor(OSAL_u8 factor);
/**
 * @brief sys_rtc_get_precisionFactor
 * @note get current precision factor used.
 * @param factor [out] current precision factor value. A power of 2 value in 1,2,4,8,...128,
 */
OSAL_void sys_rtc_get_precisionFactor(OSAL_u8* factor);
/**
 * @brief sys_is_dac_val_busy
 * @note This command check if dac is about to be changed, but no yet changed
 * @return 1 if dac change is ongoing
 */
OSAL_u8 sys_is_dac_val_busy();
/**
 * @brief sys_set_dac_val_busy
 * @note call this command before changing dac val ( dac change is ongoing), to prevent conflict
 * @param busyState  dac change is ongoing
 */
OSAL_void sys_set_dac_val_busy(OSAL_u8 busyState);
/**
 * @brief sys_get_dac_val
 * @note The UWB module can control the 26 MHz frequency thanks to a 12-bit DAC. This command is used to get the current DAC configuration.
 * @param dacVal get current dac
 */
OSAL_void sys_get_dac_val(OSAL_u16* dacVal);
/**
 * @brief sys_set_dac_val
 * @note set current dac val.Call sys_set_dac_val_busy with 0  after call to this
 * @param dacVal new dac val to set
 */
OSAL_void sys_set_dac_val(OSAL_u16 dacVal);
/**
 * @brief sys_is_capatune_busy
 * @note This command check if capatune is about to be changed, but no yet changed
 * @return 1 if capatune change is ongoing
 */
OSAL_u8 sys_is_capatune_busy();
/**
 * @brief sys_set_capatune_busy
 * @note call this command before changing capatune val ( capatune change is ongoing), to prevent conflict
 * @param busyState  capatune change is ongoing
 */
OSAL_void sys_set_capatune_busy(OSAL_u8 busyState);
/**
 * @brief sys_get_capatune_val
 * @note The UWB module can control the 32 KHz oscillator frequency thanks to CAPATUNE register. This command is used to get the current CAPATUNE register value.
 * @param capatune get capatune dac
 */
OSAL_void sys_get_capatune_val(OSAL_u8* capatune);
/**
 * @brief sys_set_capatune_val
 * @note set current capatune val.Call sys_set_capatune_busy with 0  after call to this
 * @param capatune new capatune val to set
 */
OSAL_void sys_set_capatune_val(OSAL_u8 capatune);
/**
 * @brief sys_is_rxframe_cfg_busy
 * @note This command check if rxframe change is about to be changed, but no yet changed
 * @return 1 if rxframe change is ongoing
 */
OSAL_u8 sys_is_rxframe_cfg_busy();
/**
 * @brief sys_set_rxframe_cfg_busy
 * @note call this command before changing rxframe val ( rxframe change is ongoing), to prevent conflict
 * @param busyState  rxframe change is ongoing
 */
OSAL_void sys_set_rxframe_cfg_busy(OSAL_u8 busyState);
/**
 * @brief sys_get_rxframe_cfg
 * @note This command is used to get the type of RX data sent by the module to the Host MCU.
 * @param cfg get rxframe dac
 */
OSAL_void sys_get_rxframe_cfg(OSAL_u8* cfg);
/**
 * @brief sys_set_rxframe_cfg
 * @note set current rxframe val.Call sys_set_rxframe_cfg_busy with 0  after call to this
 * @param cfg new rxframe val to set
 */
OSAL_void sys_set_rxframe_cfg(OSAL_u8 cfg);
/**
 * @brief sys_is_statusmsk_busy
 * @note This command check if statusmask is about to be changed, but no yet changed
 * @return 1 if statusmask change is ongoing
 */
OSAL_u8 sys_is_statusmsk_busy();
/**
 * @brief sys_set_statusmsk_busy
 * @note call this command before changing statusmask val ( statusmask change is ongoing), to prevent conflict
 * @param busyState  statusmask change is ongoing
 */
OSAL_void sys_set_statusmsk_busy(OSAL_u8 busyState);
/**
 * @brief sys_get_status_mask
 * @note This command is used to get the current event mask module register. Default is 0, which means all events will trig the interrupt line.
 * @param mask get statusmask dac
 */
OSAL_void sys_get_status_mask(OSAL_u16* mask);
/**
 * @brief sys_set_status_mask
 * @note set current statusmask val.Call sys_set_statusmsk_busy with 0  after call to this
 * @param mask new statusmask val to set
 */
OSAL_void sys_set_status_mask(OSAL_u16 mask);
/**
 * @brief sys_check_status
 * @note check if status has changed, and generate an IT in this case
 */
OSAL_void sys_check_status();
/**
 * @brief sys_reset_status
 * @note  Reading module status automatically clears interrupt line 
 */
OSAL_void sys_reset_status();
/**
 * @brief sys_get_status
 * @note This command is used to get the current module status.
 * @param status get status dac
 */
OSAL_void sys_get_status(OSAL_u16* status);
/**
 * @brief sys_set_status
 * @note set (add) current status val. Tobe used in case of IT
 * @param status new status val to set/add( bitfield)
 */
OSAL_void sys_set_status(OSAL_u16 status);
/**
 * @brief sys_is_checkcache_busy
 * @note This command check if cache is about to be checked, but no yet checked
 * @return 1 if cache change is ongoing
 */
OSAL_u8 sys_is_checkcache_busy();
/**
 * @brief sys_set_checkcache_busy
 * @note call this command before checking cache val ( checking change is ongoing), to prevent conflict
 * @param busyState  checking cache is ongoing
 */
OSAL_void sys_set_checkcache_busy(OSAL_u8 busyState);
/**
 * @brief sys_checkcache
 * @note Check if value in sys cache is the same than on hardware ( test only)
 * @param checkId  id of the cache value to check (relativelly to SYS)
 */
OSAL_void sys_checkcache(OSAL_u8 checkId);
/**
 * @brief sys_get_checkcache
 * @note get latest check cahce test
 * @param ok cache check is success if ok=1, 0 otherwise
 */
OSAL_void sys_get_checkcache(OSAL_u8* ok);

/**
 * @brief sys_process
 * @note process any system action.  \n THIS MUST BE CALLED IN MAINLOOP
 */
OSAL_void sys_process();
/**
 * @brief sys_led_management
 * @note Switch ON/OFF a LED
 * @param led_Mask: mask on led to chnage state
 * @param newState: new state of the led: true:on,false:off(default)
 */
OSAL_void sys_led_management(OSAL_u8 led_Mask, OSAL_bool_t newState);
/**
 * @brief sys_is_module_moving
 * @note indicate module is moving or not
 * @param movingstate: module is moving or not
 */
OSAL_void sys_is_module_moving(OSAL_bool_t* movingstate);
/**
 * @brief sys_update_battery_and_temperature_info
 * @note update module information regarding temperature and battery
 */
OSAL_void sys_update_battery_and_temperature_info();
/**
 * @brief sys_get_battery_data
 * @note get pointer on battery level
 * @param data pointer where battery level is stored
 * @param dataSizeinBits: size of this buffer
 */
OSAL_void sys_get_battery_data(OSAL_u8** data,OSAL_u16* dataSizeinBits);
/**
 * @brief sys_get_battery_level
 * @note get battery level
 * @param level current level of battery
 * @param levelLOW min level of battery. If level = levelLOW , battery is low
 * @param levelHIGH max level of battery. If level = levelHIGH , battery is OK
 * @param levelmV real level of battery in mV
 */
OSAL_void sys_get_battery_level(OSAL_u8* level, OSAL_u8* levelLOW, OSAL_u8* levelHIGH,OSAL_u32* levelmV);
/**
 * @brief sys_get_temperature_level
 * @note get temperature
 * @param temp temperature in milliCelcius
 * * @param tempRounded temperature rounded in Celcius
 */
OSAL_void sys_get_temperature_level(OSAL_s32* temp,OSAL_s32* tempRounded);

OSAL_s32 sys_noise_or_treshold_test(OSAL_u8 channel, OSAL_u8 zone, OSAL_u8 path,OSAL_s32 *min, OSAL_s32* max,OSAL_u8 thTest, OSAL_u32* processTimeUS);
OSAL_void sys_test_mcuid(st_uwb_test_mcuid* result);
OSAL_void sys_test_soldering(st_uwb_test_soldering *result);
OSAL_void sys_test_io(uwb_test_status_e* result);
OSAL_void sys_test_voltage(st_uwb_test_voltage* result);
OSAL_void sys_test_txtune(st_uwb_test_txtune* result);
OSAL_void sys_test_lna(st_uwb_test_lna *result);
OSAL_void sys_test_rxtune(st_uwb_test_rxtune* result);
OSAL_void sys_test_txidle(uwb_test_status_e* result);
OSAL_void sys_test_rxidle(uwb_test_status_e* result);
OSAL_void sys_test_pm2(uwb_test_status_e* result);
OSAL_void sys_test_txcont(uwb_test_status_e* result);
OSAL_void sys_test_sensitivity(uwb_test_status_e* result);
OSAL_void sys_test_ranging(uwb_test_status_e* result);
OSAL_void sys_test_rfdesign(st_uwb_test_rfdesign* result,int measnb);
OSAL_void sys_test_production(st_uwb_test_results** result);

/**
 * @brief sys_is_prod_busy
 * @note this function is defined as soon as special firmware are used. Code not available to custommers
 * @note This command check if a setting of limits is already ongoign  but no yet done
 * @return 1 if a spi read/write is ongoing
 */
OSAL_u8 sys_is_prod_busy();
/**
 * @brief sys_set_prod_busy
 * @note this function is defined as soon as special firmware are used. Code not available to custommers
 * @note call this command before doing any prod limit settings, to prevent conflict
 * @param busyState setting of limits is ongoing
 */
OSAL_void sys_set_prod_busy(OSAL_u8 busyState);
/**
 * @brief sys_prod_limits_get_buf
 * @note this function is defined as soon as special firmware are used. Code not available to custommers
 * @note get buffer where to store limits for production test
 * @param data  limits data buffer
 * @param datalen lmimits data buffer len
 */
OSAL_void sys_prod_limits_get_buf(OSAL_u8** data,OSAL_u16* datalen);
/**
 * @brief sys_prod_result_get_buf
 * @note this function is defined as soon as special firmware are used. Code not available to custommers
 * @note get buffer where result are stored
 * @param data result data buffer
 * @param datalen  result data buffer len
 */
OSAL_void sys_prod_result_get_buf(OSAL_u8** data,OSAL_u16* datalen);
/**
 * @brief sys_prod_ctrl
 * @note This command and its sub command are used to start/stop/get status the prod test process
 * @note once its started, it will stop automatically, and will send an IT.
 * @note it can be stopped through #sys_prod_ctrl, spi command, but once started, mainloop will be frozen until test stops
 * @note this function is defined as soon as special firmware are used. Code not available to custommers
 * @note so too much spi command may be rejected
 * @param ctrl type of sequencer ctrl to do @see uwb_com_subcmd_e
 */
OSAL_void sys_prod_ctrl(OSAL_u8 ctrl);


#endif //__MODULE_API_SYS_H__

