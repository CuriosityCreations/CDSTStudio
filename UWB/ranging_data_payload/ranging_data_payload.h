/**
 * @file ranging_data_payload.h
 * @brief Definition to function uses to manage sensors and External data packet that will be payloaded and send through UWB
 * @note This header define routine, corresponding C file are implementation dependant
 * @author www.bespoon.com
 * @date Copyright (C) Bespoon 2014-2015
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

/**  @page Spi_comm
 * @section Spi_comm_command_prot_RNGPAYLOAD Sending Sensor or external data through UWB
 * 
 * 
 * By modifiying the following files:
 * - ranging_data_sensor0_payload.c
 * - ranging_data_sensor1_payload.c
 * - ranging_data_sensor2_payload.c
 * - ranging_data_sensor3_payload.c
 * - ranging_data_sensor4_payload.c
 * - ranging_data_sensor5_payload.c
 * - ranging_data_extdata_payload.c
 * 
 * 
 * 
 * Developpers using SDK can change data sent through UWB. \n
 * 
 * Depending on implemented code, field uwbPayloadInBits in @ref Spi_comm_command_prot_PROT_SET_SFRAME_SCH must be tuned. \n
 * 
 * Each payload function works with 4 functions:
 * - sensorX_payload_init : Initialisation of user sensor. Called by BeSpoon protocol
 * - sensorX_payload_deinit : Deinitialisation of user sensor. Called by BeSpoon protocol
 * - sensorX_payload_getData : Get latest data for the sensor. Called by BeSpoon protocol
 * - sensorX_payload_updateData : update of sensor info.  Called by BeSpoon protocol
 * 
 * 
 * 
 * Most of code done by SDK users can be done in the function, in the way they like, as soon as sdk user provides <i>init</i> 
 * function, <i>deinit</i> function, <i>update</i> function, and <i>get data</i> function. \n
 * 
 * 
 * Get data function is called with  2 argument: a pointer on data (which memory must be managed internally 
 * in sensor code file), and dataSizeinBits, which allow to retrieve the size <b> in bits</b> of the sensor data. \n
 * This function can be called with <i>NULL</i> passed to <i>data</i> (or <i>dataSizeinBits</i>) argument, which allow bespoon protocol to calculate size to be sent
 * through UWB. \n
 * 
 * 
 * Default implementation, provided with BeSpoon firmware, use Sensor 0 for Battery level, adn Sensor 1 , to indicate
 * device is moving or not. \n
 * 
 * Other Sensor are left free for any purpose. \n
 * 
 * Contact BeSpoon for any support on this topic ( see @ref CONTACT)
 * 
 * 
 * @latexonly 
 * \newpage
 * @endlatexonly
 * 
 */
#ifndef _RANGING_DATA_PAYLOAD_H_
#define _RANGING_DATA_PAYLOAD_H_

#include "types.h"


/**
 * @brief fct_payload_getData
 * @note function pointer for get data of a payloader
 */
typedef void (*fct_payload_getData) (OSAL_u8**,OSAL_u16*);
/**
 * @brief fct_payload_init
 * @note function pointer for init a payloader
 */
typedef void (*fct_payload_init) ();
/**
 * @brief fct_payload_deinit
 * @note function pointer for deinit a payloader
 */
typedef void (*fct_payload_deinit) ();
/**
 * @brief fct_payload_updateData
 * @note function pointer for update data of a payloader
 */
typedef void (*fct_payload_updateData) ();


/**
* @brief PAYLOAD_INIT_ALL  Convenient macro to init all payloader. Used internally by BeSpoon protocol
*/
#define PAYLOAD_INIT_ALL do{\
	sensor0_payload_init();\
	sensor1_payload_init();\
	sensor2_payload_init();\
	sensor3_payload_init();\
	sensor4_payload_init();\
	sensor5_payload_init();\
	extdata_payload_init();\
}while(0)

/**
* @brief PAYLOAD_INIT_ALL  Convenient macro to deinit all payloader. Used internally by BeSpoon protocol
*/
#define PAYLOAD_DEINIT_ALL do{\
	sensor0_payload_deinit();\
	sensor1_payload_deinit();\
	sensor2_payload_deinit();\
	sensor3_payload_deinit();\
	sensor4_payload_deinit();\
	sensor5_payload_deinit();\
	extdata_payload_deinit();\
}while(0)

/**
* @brief PAYLOAD_UPDATE_ALL  Convenient macro to update all payloader. Used internally by BeSpoon protocol
*/
#define PAYLOAD_UPDATE_ALL do{\
	sensor0_payload_updateData();\
	sensor1_payload_updateData();\
	sensor2_payload_updateData();\
	sensor3_payload_updateData();\
	sensor4_payload_updateData();\
	sensor5_payload_updateData();\
	extdata_payload_updateData();\
}while(0)


/************************************** SENSOR 0 SPECIFIC IMPLEMENTATION *************/
/**
 * @brief sensor0_payload_init
 * @note init sensor0 payloader
 * @note in BeSpoon default implementation, this sensor correspond to battery level
 */
OSAL_void sensor0_payload_init();
/**
 * @brief sensor0_payload_deinit
 * @note deinit sensor 0 payloader
 */
OSAL_void sensor0_payload_deinit();
/**
 * @brief sensor0_payload_getData
 * @note get data and/or data size of sensor 0 payloader
 * @param data data of sensor 0 payloader
 * @param dataSizeinBits Size in bits of sensor 0 payloader
 */
OSAL_void sensor0_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits);
/**
 * @brief sensor0_payload_updateData
 * @note update data and/or data size of sensor 0 payloader
 */
OSAL_void sensor0_payload_updateData();
/************************************** SENSOR 1 SPECIFIC IMPLEMENTATION *************/
/**
 * @brief sensor1_payload_init
 * @note init sensor 1 payloader
 * @note in BeSpoon default implementation, this sensor correspond to moving detection
 */
OSAL_void sensor1_payload_init();
/**
 * @brief sensor1_payload_deinit
 * @note deinit sensor 1 payloader
 */
OSAL_void sensor1_payload_deinit();
/**
 * @brief sensor1_payload_getData
 * @note get data and/or data size of sensor 1 payloader
 * @param data data of sensor 1 payloader
 * @param dataSizeinBits Size in bits of sensor 1 payloader
 */
OSAL_void sensor1_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits);
/**
 * @brief sensor1_payload_updateData
 * @note update data and/or data size of sensor 1 payloader
 */
OSAL_void sensor1_payload_updateData();
/************************************** SENSOR 2 SPECIFIC IMPLEMENTATION *************/
/**
 * @brief sensor2_payload_init
 * @note init sensor 2 payloader
 * @note in BeSpoon default implementation, this sensor is unused
 */
OSAL_void sensor2_payload_init();
/**
 * @brief sensor2_payload_deinit
 * @note deinit sensor 2 payloader
 */
OSAL_void sensor2_payload_deinit();
/**
 * @brief sensor2_payload_getData
 * @note get data and/or data size of sensor 2 payloader
 * @param data data of sensor 2 payloader
 * @param dataSizeinBits Size in bits of sensor 2 payloader
 */
OSAL_void sensor2_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits);
/**
 * @brief sensor2_payload_updateData
 * @note update data and/or data size of sensor 2 payloader
 */
OSAL_void sensor2_payload_updateData();
/************************************** SENSOR 3 SPECIFIC IMPLEMENTATION *************/
/**
 * @brief sensor3_payload_init
 * @note init sensor 3 payloader
 * @note in BeSpoon default implementation, this sensor is unused
 */
OSAL_void sensor3_payload_init();
/**
 * @brief sensor3_payload_deinit
 * @note deinit sensor 3 payloader
 */
OSAL_void sensor3_payload_deinit();
/**
 * @brief sensor3_payload_getData
 * @note get data and/or data size of sensor 3 payloader
 * @param data data of sensor 3 payloader
 * @param dataSizeinBits Size in bits of sensor 3 payloader
 */
OSAL_void sensor3_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits);
/**
 * @brief sensor3_payload_updateData
 * @note update data and/or data size of sensor 3 payloader
 */
OSAL_void sensor3_payload_updateData();
/************************************** SENSOR 4 SPECIFIC IMPLEMENTATION *************/
/**
 * @brief sensor4_payload_init
 * @note init sensor 4 payloader
 * @note in BeSpoon default implementation, this sensor is unused
 */
OSAL_void sensor4_payload_init();
/**
 * @brief sensor4_payload_deinit
 * @note deinit sensor 4 payloader
 */
OSAL_void sensor4_payload_deinit();
/**
 * @brief sensor4_payload_getData
 * @note get data and/or data size of sensor 4 payloader
 * @param data data of sensor 4 payloader
 * @param dataSizeinBits Size in bits of sensor 4 payloader
 */
OSAL_void sensor4_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits);
/**
 * @brief sensor4_payload_updateData
 * @note update data and/or data size of sensor 4 payloader
 */
OSAL_void sensor4_payload_updateData();
/************************************** SENSOR 5 SPECIFIC IMPLEMENTATION *************/
/**
 * @brief sensor5_payload_init
 * @note init sensor 5 payloader
 * @note in BeSpoon default implementation, this sensor is unused
 */
OSAL_void sensor5_payload_init();
/**
 * @brief sensor5_payload_deinit
 * @note deinit sensor 5 payloader
 */
OSAL_void sensor5_payload_deinit();
/**
 * @brief sensor5_payload_getData
 * @note get data and/or data size of sensor 5 payloader
 * @param data data of sensor 5 payloader
 * @param dataSizeinBits Size in bits of sensor 5 payloader
 */
OSAL_void sensor5_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits);
/**
 * @brief sensor5_payload_updateData
 * @note update data and/or data size of sensor 5 payloader
 */
OSAL_void sensor5_payload_updateData();
/************************************** EXT DATA SPECIFIC IMPLEMENTATION *************/
/**
 * @brief extdata_payload_init
 * @note init sensor 2 payloader
 * @note in BeSpoon default implementation, external data are unused
 */
OSAL_void extdata_payload_init();
/**
 * @brief extdata_payload_deinit
 * @note deinit external data payloader
 */
OSAL_void extdata_payload_deinit();
/**
 * @brief extdata_payload_getData
 * @note get data and/or data size of sexternal data payloader
 * @param data data of external data payloader
 * @param dataSizeinBits Size in bits of external data payloader
 */
OSAL_void extdata_payload_getData(OSAL_u8** data,OSAL_u16* dataSizeinBits);
/**
 * @brief extdata_payload_updateData
 * @note update data and/or data size of external data payloader
 */
OSAL_void extdata_payload_updateData();

#endif //#ifndef _RANGING_DATA_PAYLOAD_H_

