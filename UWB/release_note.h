/**
 * @file release_note.h
 * @brief release note for module firmware
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2015-2016. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

/**
  @def SW_VERSION_MAJ
  Current Sw version major indicator. On 4bits, [0...15].
*/
#define SW_VERSION_MAJ 1
/**
  @def SW_VERSION_MIN
  Current Sw version minor indicator. On 4bits, [0...15].
*/
#define SW_VERSION_MIN 9

/** @page SWVERSION
 * 
 * @section SWVERSION_GENERIC UM100
 * 
 * <b>Software Release v1.9</b>
 * 
 * IMPROVEMENT: 
 * - Reorganize project and code into a more elegant tree architecture
 * - Add tool to let user build UM100 ROM Code on Ubuntu
 * - DFU Loader : various graphical change and stability improved
 * - Better precision in RTC
 * - Manage RTC with LSI and/Or LSE, under some HW modification condition
 * - Manage UM100 module timing information with STM32 RTC
 * - Backup UM100 prodsection with serialName and Mcu ID Long address, to be able to retrieve lost configuraiton ,even if S/N is not available
 * - ROM Upgrade
 * - UART shell routine cosmetics and factorization
 * - cosmetics in UART shell
 * - add module configuration argument on modprobe to deactivate lta/msq/pm
 * - Don't use GPIO directly in module layer, but use the one defined in HAL. This will allow to clean dependancies from abstracted layer
 * 
 * 
 *
 * BUG Fixes:
 * - RAM/ROM optimization
 * - PM3 -> PM2 sequence upgrade:32K need 50ms delay to be correctly initialized
 * - Updated offsets, modules are now set to 0cm while other devices are aligned to this value
 * 
 * 
 * <b>Software Release v1.8</b>
 *
 * - prodsectionStruct update to 1.0.6
 * - hw_offset are now stored in prodsection by rfpath. That mean 4 hw_offset (2 internal rf_path * 2 external)
 * - Splitted hw_offset in RX and TX instead of RX only before. This should have no impact on any configuration
 * - improve ROM size
 * - add RTC, with RTC test in module shell
 * - shell modification to let appear, in help, only mandatory function
 * - problem with div s64 correction
 * 
 *
 *
 * <b>Software Release v1.7</b>
 *
 * - Add toa correction, with up to picosecond precision.Now Toa will be composed of toa_le_raw (in nanosecond), and toa_le_corr (in picoseconds, value in [0...999])
 * - 2way ranging test scanning part correction
 * 
 *
 *
 *
 * <b>Software Release v1.6</b>
 *
 * - upgrade default spreading code
 * 
 *
 *
 * <b>Software Release v1.5</b>
 *
 * - Updated all times. Now use NTOA/IW instead of 125ps / 1ns, to gain in precision
 * - Add VERBOSE level of trace to be able to distinguish Real time traces and info only. Decrease trace string size to gain ROM
 * - Remove unused code form distributed project
 * - add help shell command that will display little howto on each shell command
 * - decrease size of ROM by removing some unused command
 * - cosmetics and Trace changes.
 * - Prodsection update to 1.3: added spreading codes, update management, alerts for CRC errors / version mismatch
 * - Splitted tests in 4 categories: system, radio, misc & prod
 * - Added test_RF_design and threshold test (noise measure)
 * - Add Live Threshold adjustment feature added.
 * - Add spi api tests in C.
 * 
 *
 *
 *
 * <b>Software Release v1.4</b>
 *
 * - bs_loader_distributed upgrade, to avoid data loose in case of problems with Windows usb/dfu drivers
 * - Other modification for production
 * 
 *
 *
 * 
 * 
 * <b>Software Release v1.3</b>
 *
 * - Improve spi queue method, to avoid some dead lock in case of interruption while dequeing previous command
 * - Decrease generic firmware size
 * - Add security and backup in loader, to avoid loosing configuration
 * - Used Auto-threshold in bash Spi Script test
 * 
 *
 * 
 * 
 * <b>Software Release v1.2</b>
 *
 * - Clock drift value correction. Previous version used to return absolute value of clock drift.
 * - Add two way ranging test in embedded shell. 
 * - Let module be able to switch from/to PM3.
 * - Cosmetics in structure header
 * - Added HAL_ADC_Calibrate()
 * - Better time management for adc
 * - SYS_GET_INFO api bad threshold correction
 * - Improve inter delay in SPI (500µs -> 50µs)
 * 
 * 
 * 
 *
 * <b>Software Release v1.1</b>
 *
 * - Add shell in firmware generic ( not activated as default, can be activated through  bs_loader_stm_distributed.exe)
 * - Better SW version management. Now SDK version follow SW version.
 * - TOA value returned through READ_RXDATA Correction: It's now scaled as nanosecond to avoid overflow with some limit case. 
 * - Maximum time between 2 sequences without Reset Counter is now 2147millisecond. Above 2.1second, it is important to reset the counter to avoid overflow.
 * - Sys GET_INFO api upgrade, to get more information on Sw, hw, and chip version of the module (through shell or spi api)
 * 
 *
 *
 *
 * <b>Software Release v1.0</b>
 *
 * - Fix CoIDE RAM/ROM memory settings (Configuration=>Link tab)
 * - Support of trace level change and shell activation, through bs_loader_stm_distributed.exe
 * - Set default UART2 baudrate to 57600, changeable through  bs_loader_stm_distributed.exe
 * - Bad values in buffer returned through READ_RX_DATA correction, rx_data_size_msb and rx_data_size_lsb were missing
 * - Cosmetics and doxygen documentation upgrade.
 * - Add possibility to support traces through UART1 instead of UART2 (configurable through HAL_config.h)
 * 
 *
 *
 *
 * <b>Software Release v0.2</b>
 *
 * - First version with Generic firmware
 * 
 * @latexonly 
 * \newpage
 * @endlatexonly
*/


