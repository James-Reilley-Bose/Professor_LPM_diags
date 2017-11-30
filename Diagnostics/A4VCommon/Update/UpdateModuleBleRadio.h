#ifndef UPDATE_MODULE_BLE_RADIO_H
#define UPDATE_MODULE_BLE_RADIO_H

//BleRadio_Init(void);
void UpdateModuleBleRadio_Enter(void);
void UpdateModuleBleRadio_Exit(void);
BOOL UpdateModuleBleRadio_HandleMessage(GENERIC_MSG_t* msg);
#endif