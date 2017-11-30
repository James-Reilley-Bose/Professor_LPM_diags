//
// UpdateModuleRadio.h
//

#ifndef UPDATE_MODULE_RADIO_H
#define UPDATE_MODULE_RADIO_H

void UpdateModuleWAM_Init(void);
BOOL UpdateModuleWAM_HandleMessage(GENERIC_MSG_t* msg);
void UpdateModuleWAM_EnterDoWAM(void);
void UpdateModuleWAM_ExitDoWAM(void);
void UpdateModule_EnterPostWam(void);

#endif // UPDATE_MODULE_RADIO_H
