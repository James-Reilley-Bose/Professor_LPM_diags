/*
    File    :    internalFlash.c
    Author    :    dx1007255
    Created    :    12/22/2014
    Updated    :    4/5/2016
    Language:    C
    Copyright:    (C) 2016 Bose Corporation, Framingham, MA

    Description:   Operations on internal flash of STM32F2xx

        The internal flash is abstracted as an NVRAM for read,
        and “NVSAM” -- Non Volatile Sequential Access Memory -- for write.
        The recommended write operation will be (applicable to all sectors):
        1.    Erase all required sectors.
        2.    Write data.

===============================================================================
*/
#include "project.h"
#include "stm32f2xx.h"
#include "internalFlash.h"

SCRIBE_DECL(flash);

// Erase operation depends on board supply voltage to the CPU
#define BOARD_SUPPLY_VOLTAGE_CPU        VoltageRange_3 // 2.7 - 3.6 V
#define FLASH_STATUS_ERROR_BITS         (FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR | \
                                        FLASH_FLAG_WRPERR | FLASH_FLAG_OPERR)
#define FLASH_MAX_WRITE_ERRORS          10

// Arrange all sectors into groups
typedef struct
{
    uint32_t groupStartAddress;
    uint32_t groupEndAddress;
    uint32_t sectorSize;        // Sector size
    uint16_t sectorStartIndex;  // Sector index of the first sector in this group
    uint16_t numberSectors;     // Number of sectors in this group
} SECTOR_GROUP_RECORD_t;

// Local prototypes
static void internalFlash_HandleError(void);
static uint32_t InternalFlashGetSectorByAddress(uint32_t address);
static uint32_t InternalFlashGetWPSectorBySector(uint32_t sector);
static BOOL InternalFlashUnlockWPSectorByAddress(uint32_t address);
static void InternalFlashLockWPSectors(void);
static BOOL internalFlash_isValidAddress(uint32_t address);

static const SECTOR_GROUP_RECORD_t sectorTable[NUMBER_SECTOR_GROUPS] = { SECTOR_TABLE };

void InternalFlashInit(void)
{
    // Lock WP sectors
    InternalFlashLockWPSectors();
}

/*****************************************************************//**
* @brief    Read data from the internal flash
*           Wait-State already set in system clock initialization
*
* @param    address  - address of flash to read from
* @param    pData    - pointer to a buffer to put resulting data.
* @param    length   - number of data bytes to get.
*
*           Note: Read can be performed anywhere in the flash
*
* @return   Number of bytes read, 0 means either nothing read or error
**********************************************************************/
uint32_t InternalFlashRead(const uint32_t address,  uint8_t* pData, const uint32_t length)
{
    //Ensure parameters are valid
    if ((pData == NULL) || (length == 0))
    {
        return 0;
    }

    // Sanity check address range including boot loader area
    if (!internalFlash_isValidAddress(address))
    {
        return 0;
    }

    // Ensure that length is valid
    uint32_t myLength;
    if ((address + length) < INTERNAL_FLASH_END_ADDR)
    {
        myLength = length;
    }
    else // correct the length
    {
        myLength = (INTERNAL_FLASH_END_ADDR - address);
    }

    // Access 1 byte a time for flexibility no matter whatever length is
    for (uint32_t i = 0; i < myLength; i++)
    {
        pData[i] = *(uint8_t*)(address + i);
    }

    return myLength;

}


/*****************************************************************//**
* @brief    Write data to internal flash
*
* @param    address  - address to write, it also defines the sector.
* @param    pData    - pointer to a buffer containing data.
* @param    length   - number of data bytes to write.
*
*           Note: This function will write data across sector boundaries.
*                 It is up to the user to ensure that the flash has been
*                 properly erased for all needed sectors before calling
*                 this function.
*
* @return   Number of bytes written, 0 means either nothing written or error
**********************************************************************/
uint32_t InternalFlashWrite(const uint32_t address, uint8_t* pData, const uint32_t length)
{
    if ((pData == NULL) || (length == 0))
    {
        LOG(flash, ROTTEN_LOGLEVEL_NORMAL, "Flash write args bad: pData = %08X, length = %d", pData, length);
        return 0;
    }

    if (!internalFlash_isValidAddress(address))
    {
        return 0;
    }

    // Allow write
    FLASH_Unlock();

    uint32_t currentAddress = address;
    const uint32_t endAddress = address + length;
    FLASH_Status status = FLASH_COMPLETE;
    uint8_t errorCount = 0;
    while (currentAddress < endAddress)
    {
        uint32_t bytesLeft = endAddress - currentAddress;
        uint8_t* data = &pData[length - bytesLeft];
        uint8_t bytesWritten = 0;

        // Disable sector write protects
        if (!InternalFlashUnlockWPSectorByAddress(currentAddress))
        {
            status = FLASH_ERROR_WRP;
            break;
        }

        if ((currentAddress % sizeof(uint32_t) == 0) && (bytesLeft >= sizeof(uint32_t)))
        {
            status = FLASH_ProgramWord(currentAddress, *(uint32_t*) data);
            bytesWritten = sizeof(uint32_t);
        }
        else if ((currentAddress % sizeof(uint16_t) == 0) && (bytesLeft >= sizeof(uint16_t)))
        {
            status = FLASH_ProgramHalfWord(currentAddress, *(uint16_t*) data);
            bytesWritten = sizeof(uint16_t);
        }
        else
        {
            status = FLASH_ProgramByte(currentAddress, *data);
            bytesWritten = sizeof(uint8_t);
        }

        // Enable sector write protects
        InternalFlashLockWPSectors();

        if (status == FLASH_COMPLETE)
        {
            currentAddress += bytesWritten;
        }
        else
        {
            internalFlash_HandleError();
            if (errorCount++ > FLASH_MAX_WRITE_ERRORS)
            {
                LOG(flash, ROTTEN_LOGLEVEL_NORMAL, "Flash write failed %d times, giving up.", FLASH_MAX_WRITE_ERRORS);
                break;
            }
        }
    }

    FLASH_Lock();

    return (status == FLASH_COMPLETE) ? length : 0;
}

static BOOL InternalFlashUnlockWPSectorByAddress(uint32_t address)
{
    if (!internalFlash_isValidAddress(address))
    {
        return FALSE;
    }
    uint32_t sector = InternalFlashGetSectorByAddress(address);
    if (!IS_FLASH_SECTOR(sector))
    {
        return FALSE;
    }
    uint32_t wpsector = InternalFlashGetWPSectorBySector(sector);
    if (!IS_OB_WRP(wpsector))
    {
        return FALSE;
    }
    FLASH_OB_Unlock();
    FLASH_OB_WRPConfig(wpsector, DISABLE);
    FLASH_OB_Lock();
    return TRUE;
}

static void InternalFlashLockWPSectors(void)
{
    // Enable sector write protects
    FLASH_OB_Unlock();
    FLASH_OB_WRPConfig(OB_WRP_Sector_All, ENABLE);
    FLASH_OB_Lock();
}

static uint32_t InternalFlashGetWPSectorBySector(uint32_t sector)
{
    switch (sector)
    {
        case FLASH_Sector_0:
            return OB_WRP_Sector_0;
        case FLASH_Sector_1:
            return OB_WRP_Sector_1;
        case FLASH_Sector_2:
            return OB_WRP_Sector_2;
        case FLASH_Sector_3:
            return OB_WRP_Sector_3;
        case FLASH_Sector_4:
            return OB_WRP_Sector_4;
        case FLASH_Sector_5:
            return OB_WRP_Sector_5;
        case FLASH_Sector_6:
            return OB_WRP_Sector_6;
        case FLASH_Sector_7:
            return OB_WRP_Sector_7;
        case FLASH_Sector_8:
            return OB_WRP_Sector_8;
        case FLASH_Sector_9:
            return OB_WRP_Sector_9;
        case FLASH_Sector_10:
            return OB_WRP_Sector_10;
        case FLASH_Sector_11:
            return OB_WRP_Sector_11;
        default:
            return 0;
    }
}

static uint32_t InternalFlashGetSectorByAddress(uint32_t address)
{
    static const uint16_t flashSectorParameters[INTERNAL_FLASH_TOTAL_SECTORS] = { FLASH_SECTOR_IDS };
    // Get the sector's index from address
    uint32_t FLASH_Sector = 0xFFFFFFFF;
    for (int i = 0; i < NUMBER_SECTOR_GROUPS; i++)
    {
        if ((address >= sectorTable[i].groupStartAddress) && (address < sectorTable[i].groupEndAddress))
        {
            for (uint16_t j = 0; j < sectorTable[i].numberSectors; j++)
            {
                if (address < sectorTable[i].groupStartAddress + (j + 1)*sectorTable[i].sectorSize)
                {
                    FLASH_Sector = flashSectorParameters[sectorTable[i].sectorStartIndex + j];
                    break; // j-loop
                }
            }

            if (FLASH_Sector != 0xFFFFFFFF)
            {
                break; // i-loop
            }
        }
    }
    return FLASH_Sector;
}


/*****************************************************************//**
* @brief    Erase a sector of the internal flash which contains a given address.
*
* @param    address  - address within the sector that is to be erased,
*
* @return   TRUE for success, FALSE otherwise
**********************************************************************/
BOOL InternalFlashEraseSectorByAddress(const uint32_t address)
{
    BOOL retval = TRUE;

    if (!internalFlash_isValidAddress(address))
    {
        return FALSE;
    }

    //get sector by address
    uint32_t FLASH_Sector = InternalFlashGetSectorByAddress(address);
    if (!IS_FLASH_SECTOR(FLASH_Sector))
    {
        return FALSE;
    }

    // Since we are not sure if this sector has any thing in either
    // Data or Instruction Cache, we simply flush both
    BOOL needReEnableDataCache = FALSE;
    BOOL needReEnableInstructionCache = FALSE;

    portENTER_CRITICAL();
    uint32_t internalFlashACR = FLASH->ACR;

    if (internalFlashACR & FLASH_ACR_DCEN)
    {
        // Data Cache was enabled, disable it and then flush
        FLASH_DataCacheCmd(DISABLE);
        needReEnableDataCache = TRUE;
        FLASH_DataCacheReset();
    }

    if (internalFlashACR & FLASH_ACR_ICEN)
    {
        // Instruction Cache was enabled, disable it and then flush
        FLASH_InstructionCacheCmd(DISABLE);
        needReEnableInstructionCache = TRUE;
        FLASH_InstructionCacheReset();
    }
    portEXIT_CRITICAL();

    // unlock flash (must lock before returning)
    FLASH_Unlock();

    // Disable sector write protects (must reenable before returning)
    if (!InternalFlashUnlockWPSectorByAddress(address))
    {
        retval = FALSE;
    }

    // This is hardware specific, i.e., CPU supply voltage is 3.3V
    if (retval)
    {
        if (FLASH_EraseSector(FLASH_Sector, BOARD_SUPPLY_VOLTAGE_CPU) != FLASH_COMPLETE)
        {
            retval = FALSE;
        }
    }

    // Enable sector write protects
    InternalFlashLockWPSectors();

    // Restore the lock
    FLASH_Lock();

    // Re-enable D/I caches if needed
    portENTER_CRITICAL();
    if (needReEnableDataCache == TRUE)
    {
        FLASH_DataCacheCmd(ENABLE);
    }

    if (needReEnableInstructionCache == TRUE)
    {
        FLASH_InstructionCacheCmd(ENABLE);
    }
    portEXIT_CRITICAL();

    return retval;
}

/*****************************************************************//**
* @brief    Get size of a sector that contains a given address.
*
* @param    address  - address within a sector
*
* @return   Number of bytes in the sector, 0 means error
**********************************************************************/
uint32_t InternalFlashGetSectorSize(const uint32_t address)
{
    // Validate address
    if (!internalFlash_isValidAddress(address))
    {
        return FALSE;
    }

    // Get the sector's index from address
    for (uint8_t i = 0; i < NUMBER_SECTOR_GROUPS; i++)
    {
        if ((address >= sectorTable[i].groupStartAddress) && (address < sectorTable[i].groupEndAddress))
        {
            for (uint16_t j = 0; j < sectorTable[i].numberSectors; j++)
            {
                if (address < sectorTable[i].groupStartAddress + (j + 1)*sectorTable[i].sectorSize)
                {
                    return sectorTable[i].sectorSize;
                }
            }
        }
    }

    return 0;
}


/*****************************************************************//**
* @brief    Clear error bits in the flash status register (FLASH_SR)
*
* @return   void
**********************************************************************/
static void internalFlash_HandleError(void)
{
    LOG(flash, ROTTEN_LOGLEVEL_NORMAL, "Internal flash error detected (Flash->SR: 0x%X). Clearing flags.", FLASH->SR);
    FLASH_ClearFlag(FLASH_STATUS_ERROR_BITS);
}

static BOOL internalFlash_isValidAddress(uint32_t address)
{
    if ((INTERNAL_FLASH_START_ADDR <= address) && (address < INTERNAL_FLASH_END_ADDR))
    {
        return TRUE;
    }
    return FALSE;
}
