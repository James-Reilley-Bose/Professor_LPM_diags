///////////////////////////////////////////////////////////////////////////////
///   @file         internalFlashAPI.h
///   @brief
///   @version       $ID: $
///   @author        ar1016648
///   @date          4/5/2016
///   Language:      C
///
///   Description:   Wrapper for FreeRTOS around Internal Flash Driver
///
///   Copyright:     (C) 2014 Bose Corporation, Framingham, MA
///
///////////////////////////////////////////////////////////////////////////////
#ifndef INTERNAL_FLASH_API_H
#define INTERNAL_FLASH_API_H

#include "project.h"

#define INTERNAL_FLASH_MUTEX_WAIT_TICKS    100

void InternalFlashAPI_Init(void);
uint32_t InternalFlashAPI_Write(const uint32_t address, uint8_t* pData, const uint32_t length);
BOOL InternalFlashAPI_EraseSectorByAddress(const uint32_t address);
uint32_t InternalFlashAPI_GetSectorSize(const uint32_t address);
uint32_t InternalFlashAPI_Read(const uint32_t address,  uint8_t* pData, const uint32_t length);

#endif //INTERNAL_FLASH_API_H
