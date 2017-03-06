/**
 * @file release_note_protocol.h
 * @brief release note specific for protocol
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2015-2016. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

/** @page SWVERSION
 *  
 * @section SWVERSION_PROTOCOL BeSpoon Protocol
 * 
  <b>Software Release v1.9</b>
 * ASSET TRACKING: 
 * - First asset tracking release, mainly integrated in module_api_prot.c
 * - Specific asset tracking board required (external 32kHz osc + PP LDO)
 * - Support locrate up to 30720 (2h, 40 megaframe)
 * - 7uA consumption in low power, ~500ms wakeup to range
 * 
 * 
 * 
 * IMPROVEMENT: 
 * - Add DOS frame in embedded smac test to see a tag rangin on all slot
 * - Scanrf sleep time scaled on maxListenBeaconRate
 * - afc refactoring 
 * - PHS effective in protocol 
 * - Add CRC in all UWB frames
 * - Speed-up the process in protocol_create_new_frame(), when looping to look for a frame to schedule when doing loc_rate >= 6
 * - New way to compute if we should listen to beacon and/or do tx ranging. Now it's computed once for each superframe and cached into contexte->evenement->now_{beacon,rng}
 * - New "frame blocker" mechanism
 * 
 * 
 * 
 * FIXES:
 * - Fixed multiple TX on each wake up
 * - Update for fine ranging extended range
 * - Patch for dummy frame delay scaled on PHS
 * - Let Minitag ROMCOde be able to use sfi without rach slot
 * - Avoid force shadow bug
 * - BeSpoon protocol timing upgrade
 * - Fix EVB DeltaT problem on protocol overlap , and variable PHS compliant
 * - Fixed TX glitch detection
 * - Fixed conditions to switch to base_ts 32
 * - Changed TSAConfidence behavior
 * - Fix tag listen all beacon when not compiled in tag-only mode
 * - Fix rngDataType wrong when sending SPR
 * - Fix a bug where a tag was not scheduling the ack of a frame at the right place when doing loc_rate
 * 
 *  
 * <b>Software Release v1.8</b>
 * 
 * PROTOCOL:
 * - Prevent using null pointers in lists  
 * - Prevent any call to protocol_start() if protocol already started
 * - Added external sync option. Disabled by default
 * - Support beacon metrics retrieval
 * - Multiple fixes in framer
 * - Added sensor 2 support: used by tag to return its "TSA confidence"
 * - Asset tracking capability added.
 * - rf_path in p2s since we need it to compute offset, and p2s is in charge of shifting TX of specified offset
 * - Allow PM2 only if we use B1toB2 AFC, other cases are not compatible with PM2
 * - Reset tag info (ranging slot) when reseting rach in asset tracking.
 * - Add flag CONFIG_FAST_BEACON_SWITCH to all coproj using protocol
 * 
 * 
 * 
 * SEQUENCER:
 * - Added an error when we fall in the unsolved case where TX is programed at TS > 2045
 * - Fixed -1TS bug on ranging with EVB.
 * - Better way to manage forbidden programmation (end of sf): we now shift all sf on coord of 4 TS
 * AFC : 
 * - fix bug in afc that could cause afc divergence in locrate mode (2 4 6..)
 * - Use macro to convert values to good precision. In getter, conversion is done after the shift (or division in previous version) to avoid overflows
 * - adapt afc filter to loc rate and measure precision
 * - Use union in afc_filter. 
 * - Fixed AFC not working with locrate 4 & 6 (overflow)
 * 
 * 
 * 
 * ARTLS:
 * - Added hw_offset support in ARTLS. This is used to set hw_offset directly from server
 * - Added protection in get_artls_out in case both pool_artls and buff_artls_out are empty. This fixes some random kernel crash
 * - Better handling of artls_linked_t list
 * - Various clean and cosmetics
 * - Use smbuff_t::peer_hash to store ref base hash
 * - Process ARTLS_ * _DOWN command synchroniously
 * - hw_offset support in ARTLS. This is used to set hw_offset directly from server
 * 
 * 
 * 
 * STATS:
 * - ioctl PP_IOC_G_STATS now export (almost current) timestamp
 * - Store protcol_stats_t structure in a simple array of OSAL_u32[19]
 * 
 * 
 * 
 * 
 * <b>Software Release v1.7</b>
 * - Tag SPR bugs corrections
 * - Hybrid TOA calculation to get a better toa
 * - Random backoff when attach request failed added
 * - Add stats STATS_SEQUENCER_TSA
 * - Add 3 info in STATS_LOCRATE_INFO: dynamic, rate (legacy value) and sf_start
 * - offset adjustement
 * - Trouble when attached to beacon 5 correction
 * - End of superframe correction
 * - Better LTA algorithm
 * - Fixed scan problem
 * - clean and cosmetics
 * 
 * 
 * 
 * 
 * <b>Software Release v1.6</b> 
 * - Cosmetics and factorisation
 * - Run the dissociation callback after the RDV_DISSOCIATION_REQUEST is acknoledged, otherwise tag will never ack it.
 * 
 * 
 * 
 * 
 * <b>Software Release v1.5</b> 
 * - Added stats api, but currently disabled (no ram/rom/stack modification)
 * - Dynamic PREAMBLE_TIMEOUT_SCAN
 * - Raised MAX_DRIFT_INIT_AFC to 2ppm since 1ppm was too just
 * - Disabled DLL_FINE, not a good idea to enable it with first above threshold leading edge
 * - Removed hw_offset zone dependence. Value is fixed to -500ps for the moment on both side
 * - Updated all times in protocol. Now in NTOA / IW instead of 125ps / 1ns
 * - Added fine ranging flag in config.h (enabled)
 * - Added first max local as leading edge, option in config.h (disabled)
 * - Doing rx_process_estimate on estimation_rdy instead of rx_done, this optimize timings
 * - Fix in AFC: beacons were mistakenly trashed during init AFC
 * - New framer config
 * - Get link quality from fg1nrj/threshold
 * - Moved finger position reading, to optimize process ( about 30us) 
 * - Fill payload of rdv frame for ack frame of RDV_PAIRING_REQUES
 * - Block schedule of rdv frame before receiving beacon 2
 * - Allow to run specific actions after an rdv frame was acknowledged (after TX ACK is sent)
 * - Cosmetics and factorisation
 * 
 * 
 * 
 * 
 * <b>Software Release v1.4</b>
 * - None
 * 
 * 
 * 
 * 
 * <b>Software Release v1.3</b> 
 * - Add bespoon dependancies into bespoon library instead of in general CoIDE project
 * - cosmetics and trace cleaning in embedded smac test
 * - upgrade internal module api to match UM100 Specification Protocol Addendum Datasheet
 * 
 * 
 * 
 * 
 * <b>Software Release v1.2</b>
 * - Add a subcommand UWB_SC_POP to PROT_GET_ARTLS_PKT to allow handling of multiple ARTLS packet
 * - Timing macro accuracy correction
 * - Clean and cosmetics
 * - Better protocol caps management, first 2 bytes are reserved for hardware capabilities, 2 last bytes for uwb capabilities (tag / coord...)
 * - Added accelerometer support on USART3
 * - Better time management for leds / accelero
 * - Raised max drift limits in AFC when doing low locrate
 * 
 * 
 * 
 * 
 * <b>Software Release v1.1</b>
 * - SFI parameters can be set through bs_loader_stm_distributed ( PSN, PSS, first ranging slot, rach slot, zone beacon / ranging / rach, channel)
 * - Protocol autostart can be set through bs_loader_stm_distributed
 * - Battery Led Measure correction
 * - Protocol check metrics function stack overflow correction
 * 
 *
 *
 * <b>Software Release v1.0</b>
 * - Check hardware capabilities of module before starting protocol
 * - Bad prod Section HW offset signature correction
 * - Add support of battery led status. 
 * - Cosmetics
 * 
 *
 *
 * 
 * <b>Software Release v0.2</b>
 * - First version with Protocol firmware
 * 
 *
 * @latexonly 
 * \newpage
 * @endlatexonly
*/


