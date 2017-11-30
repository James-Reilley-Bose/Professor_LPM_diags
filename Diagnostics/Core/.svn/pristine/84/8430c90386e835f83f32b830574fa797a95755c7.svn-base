/*
    File    :    internalFlashAPI.c
    Author    :    ar1016648
    Created    :    4/5/2016
    Updated    :    4/5/2016
    Language:    C
    Copyright:    (C) 2016 Bose Corporation, Framingham, MA

    Description:   Wrapper for FreeRTOS around Internal Flash Driver


===============================================================================
*/
#include "internalFlashAPI.h"

#include "internalFlash.h"


SCRIBE_DECL(flash);

static xSemaphoreHandle s_InternalFlashMutex;

void FLASH_IRQHandler(void)
{
    debug_assert(FALSE);
}

void InternalFlashAPI_Init(void)
{
    s_InternalFlashMutex = xSemaphoreCreateMutex();
    debug_assert(s_InternalFlashMutex);

    // Enable IT for Flash Error
    FLASH_Unlock();
    FLASH_ITConfig(FLASH_IT_ERR, ENABLE);
    FLASH_Lock();
    Interrupt_RegisterISR(FLASH_IRQn, FLASH_IRQHandler);
    Interrupt_Enable(FLASH_IRQn, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);

    InternalFlashInit();
}

uint32_t InternalFlashAPI_Write(const uint32_t address, uint8_t* pData, const uint32_t length)
{
    if (xSemaphoreTake(s_InternalFlashMutex, INTERNAL_FLASH_MUTEX_WAIT_TICKS) == pdFALSE)
    {
        LOG(flash, ROTTEN_LOGLEVEL_NORMAL, "write failed to take.");
        return 0;
    }
    uint32_t result = InternalFlashWrite(address, pData, length);
    xSemaphoreGive(s_InternalFlashMutex);
    return result;
}

BOOL InternalFlashAPI_EraseSectorByAddress(const uint32_t address)
{
    if (xSemaphoreTake(s_InternalFlashMutex, INTERNAL_FLASH_MUTEX_WAIT_TICKS) == pdFALSE)
    {
        LOG(flash, ROTTEN_LOGLEVEL_NORMAL, "erase failed to take.");
        return FALSE;
    }
    BOOL result = InternalFlashEraseSectorByAddress(address);
    xSemaphoreGive(s_InternalFlashMutex);
    return result;
}

uint32_t InternalFlashAPI_GetSectorSize(const uint32_t address)
{
    return InternalFlashGetSectorSize(address);
}

uint32_t InternalFlashAPI_Read(const uint32_t address,  uint8_t* pData, const uint32_t length)
{
    if (xSemaphoreTake(s_InternalFlashMutex, INTERNAL_FLASH_MUTEX_WAIT_TICKS) == pdFALSE)
    {
        LOG(flash, ROTTEN_LOGLEVEL_NORMAL, "read failed to take.");
        return 0;
    }
    uint32_t result = InternalFlashRead(address, pData, length);
    xSemaphoreGive(s_InternalFlashMutex);
    return result;
}
