///////////////////////////////////////////////////////////////////////////////
///   @file         internalFlash.h
///   @brief
///   @version       $ID: $
///   @author        dx1007255
///   @date          Modified on 12/16/2014 from its counterpart in Triode
///   Language:      C
///
///   Description:   Common defines for STM32F2xx internal flash
///
///   Copyright:     (C) 2014 Bose Corporation, Framingham, MA
///
///////////////////////////////////////////////////////////////////////////////
#ifndef INTERNAL_FLASH_H
#define INTERNAL_FLASH_H

#include "project.h"



// Exported APIs
// Those APIs provide an interface to "NVSAM" -- Non Volatile Sequential Access Memory.
// It is an abstraction of the internal flash which is very different from Random Access Memory (RAM).
// A few important tips regarding erase/program operations.
// (1) A sector must be erased before being written;
// (2) Each portion of a sector can only be written once after the sector is erased;
// (3) Erase is by sector only, different sectors may have different erase sizes;
// (4) The user must track sequential access in writing to avoid excessive erases or potential data loss;
//      Particularly, the user cannot have two subsequent writes with overlapped address ranges
//      unless there is an erase in between.
//
// The read flash operation is much like accessing a RAM. DMA usage in read is default disabled.
// To enable DMA to read from the internal flash, the user must call enableDmaToReadInternalFlash.

void InternalFlashInit(void);

uint32_t InternalFlashRead(const uint32_t address,  uint8_t* pData, const uint32_t length);

uint32_t InternalFlashWrite(const uint32_t address,  uint8_t* pData, const uint32_t length);

BOOL InternalFlashEraseSectorByAddress(const uint32_t address);

uint32_t InternalFlashGetSectorSize(const uint32_t address);

#endif //INTERNAL_FLASH_H
