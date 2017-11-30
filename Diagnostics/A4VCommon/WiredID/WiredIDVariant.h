
#ifndef WIRED_ID_VARIANT_H
#define WIRED_ID_VARIANT_H

#include "project.h"

void WiredIDVariant_Init(void);
void WiredIDVariant_Tasks(void);
void WiredIDVariant_HandleMessage(GENERIC_MSG_t *msg);
void WiredIDVariant_StartIDStateMachine(void);

#endif