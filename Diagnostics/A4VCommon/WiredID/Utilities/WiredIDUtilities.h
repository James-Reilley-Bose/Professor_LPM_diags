
#ifndef WIRED_ID_UTILITIES_H
#define WIRED_ID_UTILITIES_H

#include "WiredIDVariantUtilities.h"

void WiredIDVariantUtils_ConfigIO(void);

void WiredIDUtils_InitTimer(uint16_t period);

void WiredIDUtils_StartIDPulse(uint16_t high_time, uint16_t low_time, uint8_t init_level);

void WiredIDUtils_StopIDPulse(void);
BOOL WiredIDUtils_IsPulseEnabled(void);

void WiredIDUtils_EnablePulseIO(void);
void WiredIDUtils_DisablePulseIO(void);

uint8_t WiredIDUtils_GetInputPulseLevel(void);
uint8_t WiredIDUtils_GetOutputPulseLevel(void);
void WiredIDUtils_SetOutputPulseLevel(uint8_t val);

uint16_t WiredIDUtils_GetHiTime(void);
uint16_t WiredIDUtils_GetLowTime(void);

#endif