#ifndef _DCDC_COMMS_H_
#define _DCDC_COMMS_H_

#include "RS485Controller.h"
//#include "DCDCPacket.h"
#include "DCDCModel.h"

// since it is a slave only 1 endpoint (master) will exist
#define N_ENDPOINTS 1

#define DCDC_DEAD_TIMEOUT 300

#define DCDC_ABORT_RECEIVE_TIMEOUT 300

/** Abstracted communications initialization
 *
 * Init comms and return handle if applicable
 */
void * DCDC_Comms_Init(uint8_t dcdc_id);

/** Abstracted communications main runtime loop
 *
 * Service communications
 *
 * @param comms_handle - handle returned by DCDC_Comms_Init()
 */
void DCDC_Comms_Run(void * comms_handle);

/** Bind the model to the controller for caching global state */
void DCDC_Comms_BindDCDCModel(rs485ctrlr_handle_t controller, dcdc_model_handle_t model);

#endif
