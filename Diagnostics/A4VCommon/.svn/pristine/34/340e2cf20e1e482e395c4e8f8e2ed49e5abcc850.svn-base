
#ifndef REMOTE_SPI_H
#define REMOTE_SPI_H

#include "RemoteDefs.h"

#define Remote_ZeroSPIPacket(ptr) ( memset(ptr, 0, sizeof(Remote_SPIPacket_t)) )

extern Remote_SPIPacket_t pkt;

void Remote_SPI_Init(void);
BOOL Remote_SPI_Write(const void* data, unsigned int length);
BOOL Remote_SPI_Read(void* data, unsigned int length);

void Remote_SPI_ReadPackets(void);
uint32_t Remote_SPI_ComputeXOR(Remote_SPIPacket_t *pkt);
void Remote_SPI_EnableIRQ(BOOL);

#endif //  REMOTE_TASK_H
