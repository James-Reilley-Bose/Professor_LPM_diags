/*
    File    :   etapInternalFlash.h
    Author  :   dx1007255
    Created :   02/03/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   Interface header of testing internal flash of STM32F2xx from etap

===============================================================================
*/
#ifndef ETAPINTERNALFLASH_H
#define ETAPINTERNALFLASH_H

#include "etapcoms.h"


/**
 * @DOCETAP
 *
 * COMMAND:   internalflashtest
 *
 * ALIAS:     ft
 *
 * DESCRIP:   Erase a sector of the internal flash or Test a portion of the sector.
 *            The result of erase is verified by checking if all bytes in the sector read back as 0xFF.
 *            The test is designed to (1) write "length" bytes of data to the "address" in the sector;
 *                                    (2) read back the data;
 *                                    (3) compare the result of read with data written.
 *            Note that the data pattern is internally defined and testers don't have to input data.
 *
 * PARAM:     0         1             2           3
 *            action    address       length      dma
 *
 *            action is either "e" (for erase) or "t" (for test).
 *            address must be in range of internal flash - user is responsible for sector selection.
 *            length must be <= INTERNAL_FALSH_BUFFER_SIZE;
 *            dma = 1 will enable DMA, and dma = 0 will disable DMA
 *
 * REPLY:     Success or Assert
 *
 * EXAMPLE:   ft e, 0x080E0004   -- Erase the sector that contains address 0x080E0004
 * EXAMPLE:   ft t, 0x080E0004, 4, 1 -- Test 4 bytes at 0x080E0004 with DMA enabled
 *
 * @DOCETAPEND
 *
 */

#define FLASH_TEST_HELP_TEXT "* Use this command to erase/test internal flash\n\r\tUsage : ft action, address, length, dma"

void TAP_TestInternalFlash(CommandLine_t*);

// Big buffer size needs big stack
#define INTERNAL_FALSH_BUFFER_SIZE    256

#endif //ETAPINTERNALFLASH_H
