#ifndef INCLUDE_SERIALTAPLISTENER_H
#define INCLUDE_SERIALTAPLISTENER_H

void Init_Listener_UART();
void Listener_IRQHandler(void);
void Listener_ProcessRxdByte (uint8_t NextChar);

void Listener_TAPPrintChar(const char c);
void Listener_PrintString(const char* s);
#ifdef BARDEEN
void Listener_EnableLowPowerStandby(BOOL enableStby);
#endif
uint16_t TapGetChar(void);
void Listener_PutString(const char* s);



#endif // INCLUDE_SERIALTAPLISTENER_H
