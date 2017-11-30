/*
    File    :   etapInternalFlash.c
    Author  :   dx1007255
    Created :   02/03/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Testing InternalFlashs of STM32F2xx from etap

===============================================================================
*/
#include "project.h"

#include "etapInternalFlash.h"
#include "etap.h"
#include "internalFlashAPI.h"
#include "stdlib.h"

SCRIBE_DECL(tap_test_results);

static BOOL erase_sector_of_flash(uint32_t address);
static BOOL test_write_read_flash(uint16_t numberBytes, uint32_t address);


TAPCommand(TAP_TestInternalFlash)
{
    BOOL valid[3] = {FALSE, FALSE, FALSE};
    char action;
    uint32_t address;
    uint32_t length;
    BOOL testResult = FALSE;

    if (CommandLine->numArgs < 2)
    {
        TAP_Printf(TAP_InvalidArg);
        return;
    }

    action = TAP_ToLowerCase(CommandLine->args[0][0]);

    address = TAP_HexArgToInt(CommandLine, 1, &valid[0]);

    if (((action != 'e') && (action != 't')) || (valid[0] == FALSE)
            || (address < INTERNAL_FLASH_START_ADDR) ||
            (address >= INTERNAL_FLASH_END_ADDR))
    {
        TAP_Printf(TAP_InvalidArg);
        return;
    }

    if (action == 'e')
    {
        testResult = erase_sector_of_flash(address);
        if (testResult == FALSE)
            LOG(tap_test_results, ROTTEN_LOGLEVEL_NORMAL,
                "Internal Flash erase sector Failed (address 0x%x)", address);

    }
    else
    {
        length = TAP_DecimalArgToInt(CommandLine, 2, &valid[1]);

        if ((CommandLine->numArgs == 3) &&
                (valid[1] == TRUE) &&
                (length <= INTERNAL_FALSH_BUFFER_SIZE))
        {
            testResult = test_write_read_flash(length, address);
            if (testResult == FALSE)
                LOG(tap_test_results, ROTTEN_LOGLEVEL_NORMAL,
                    "Internal Flash test Failed (address 0x%x)", address);
        }
        else
        {
            TAP_Printf(TAP_InvalidArg);
            return;
        }
    }


    if (testResult == TRUE)
    {
        TAP_Printf(TAP_OKText);
    }
    else
    {
        TAP_Printf(TAP_FailedText);
    }
}



/*****************************************************************//**
* @brief    Erase a sector of the internal flash
*
* @param    address   -  address to erase
*
* @return   Assert if failed before completion
**********************************************************************/
static BOOL erase_sector_of_flash(uint32_t address)
{
    if (InternalFlashAPI_EraseSectorByAddress(address) == FALSE)
    {
        return FALSE;
    }

    uint32_t sectorBytes = InternalFlashAPI_GetSectorSize(address);

    uint16_t numberChunks = sectorBytes / INTERNAL_FALSH_BUFFER_SIZE;

    if (numberChunks == 0)
    {
        return FALSE;
    }

    // Create buffers to compare
    uint8_t buffer[INTERNAL_FALSH_BUFFER_SIZE];
    uint8_t temp[INTERNAL_FALSH_BUFFER_SIZE];
    uint32_t retval, sectorStartAddr;
    BOOL status = TRUE;

    // Init the temp buffer to 0xFF
    memset(temp, 0xFF, INTERNAL_FALSH_BUFFER_SIZE);

    sectorStartAddr = address & (~(sectorBytes - 1));

    for (uint16_t n = 0; n < numberChunks; n++)
    {
        // Clear the buffer
        memset(buffer, 0, INTERNAL_FALSH_BUFFER_SIZE);

        uint32_t myAddress = sectorStartAddr + n * INTERNAL_FALSH_BUFFER_SIZE;

        // Read flash into the buffer
        retval = InternalFlashAPI_Read(myAddress, buffer, INTERNAL_FALSH_BUFFER_SIZE);

        if (retval != INTERNAL_FALSH_BUFFER_SIZE)
        {
            status = FALSE;
            break;
        }

        // Compare with expected data
        if (memcmp(buffer, temp, INTERNAL_FALSH_BUFFER_SIZE))
        {
            status = FALSE;
            break;
        }
    }

    return status;
}


/*****************************************************************//**
* @brief    Test Erase/Write/Read data in the internal flash
*
* @param    numberBytes  - number of bytes to test
* @param    address   -  address to test
* @param    dma - 1 = Enable DMA, 0 = Disable DMA
*
* @return   Assert if failed before completion
**********************************************************************/
static BOOL test_write_read_flash(uint16_t numberBytes, uint32_t address)
{
    uint8_t buffer[INTERNAL_FALSH_BUFFER_SIZE];

    // Clear the buffer
    memset(buffer, 0, numberBytes);

    // Write a new pattern
    for ( int i = 0; i < numberBytes; i++)
    {
        buffer[i] = i + 0x5A;
    }

    // Write to flash
    uint32_t retval = InternalFlashAPI_Write(address, buffer, numberBytes);

    // Check the numebr actually written
    if (retval != numberBytes)
    {
        return FALSE;
    }

    // Create another buffer to compare
    uint8_t temp[INTERNAL_FALSH_BUFFER_SIZE];

    memset(temp, 0, INTERNAL_FALSH_BUFFER_SIZE);

    // Read back all bytes
    retval = InternalFlashAPI_Read(address, temp, numberBytes);

    // Check the numebr actually read
    if (retval != numberBytes)
    {
        return FALSE;
    }

    // Compare with what was written
    if (memcmp(buffer, temp, numberBytes))
    {
        return FALSE;
    }

    return TRUE;
}
