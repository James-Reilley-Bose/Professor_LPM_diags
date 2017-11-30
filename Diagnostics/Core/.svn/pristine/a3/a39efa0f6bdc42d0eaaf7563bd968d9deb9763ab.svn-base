#ifndef TRIODE_BOSELINK_SERIAL_H
#define TRIODE_BOSELINK_SERIAL_H

#include "stringbuffer.h"

#define TBL_NUM_STRING_BUFFERS 75

void TBLSerial_Init();
void TBLSerial_DeInit();

void TBLSerial_PrintChar(const char c);
void TBLSerial_PrintString(const char* s);
void TBLSerial_Printf(const char* formatstring, ...);
StringBufferTable_t* TBLSerial_GetBuffer(void);
#ifdef USE_USB_LOGGER
#include "usbd_cdc_logger.h"
#endif
#endif // TRIODE_BOSELINK_SERIAL_H
