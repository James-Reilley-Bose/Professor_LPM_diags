//
// etapFlash.c
//

#include "project.h"
#include "etap.h"
#include "BufferManager.h"
#include <ctype.h>
#include "etapFlash.h"

#define SF_B_SIZE 512

static void FlashDump_PrintBuffer(uint8_t* buffer, uint32_t length, uint32_t start_address, BOOL prettyprint);

SCRIBE_DECL(flash);

//
// @func TAP_LogDump
// @brief TAP command to dump log to a store.
//
// Args: args[0] = from
//       args[1] = to
//
// Note: unimplemented in etapcoms.c
//
TAPCommand(TAP_Flash)
{
    BOOL valid[3] = {FALSE, FALSE, FALSE};
    BOOL pass = TRUE;
    uint32_t count = 0, xferAmount = 0, errors = 0;

    /* Read in params */
    uint32_t address = TAP_HexArgToInt(CommandLine, 0, &valid[0]);
    uint32_t length = TAP_DecimalArgToInt(CommandLine, 1, &valid[1]);
    uint8_t watermark = TAP_HexArgToInt(CommandLine, 2, &valid[2]) & 0xFF;
    if (!valid[0] || !valid[1] || !valid[2])
    {
        TAP_PrintString(FLASH_HELP_TEXT);
        return;
    }

    /* Grab some memory and generate the test pattern. */
    uint8_t* buffer = BufferManagerGetBuffer(SF_B_SIZE);
    if (!buffer)
    {
        LOG(flash, ROTTEN_LOGLEVEL_NORMAL, "Error retrieving buffer for test.");
        return;
    }
    memset(buffer, watermark, SF_B_SIZE);

    /* Write the data */
    nvram_erase(address, address + length - 1);
    count = length;
    while (count)
    {
        xferAmount = MIN(count, SF_B_SIZE);
        nvram_write(address + (length - count), xferAmount, buffer);
        count -= xferAmount;
    }

    /* Read it back and compare */
    count = length;
    while (count)
    {
        xferAmount = MIN(count, SF_B_SIZE);
        memset(buffer, ~watermark, xferAmount);
        nvram_read(address + (length - count), xferAmount, buffer);
        for (uint32_t i = 0; i < xferAmount; i++)
        {
            if (buffer[i] != watermark)
            {
                pass = FALSE;
                errors++;
                break;
            }
        }
        count -= xferAmount;
    }

    if (pass)
    {
        TAP_PrintString("Pass.");
    }
    else
    {
        TAP_Printf("Fail: %d page errors.", errors);
    }

    BufferManagerFreeBuffer(buffer);
}

TAPCommand(TAP_FlashErase)
{
    uint32_t address;
    uint32_t length;
    if (!strcmp(CommandLine->args[0], "all"))
    {
        address = 0;
        length = SPIFLASH_SIZE;
    }
    else
    {
        BOOL valid[2] = {FALSE, FALSE};
        /* Read in params */
        address = TAP_HexArgToInt(CommandLine, 0, &valid[0]);
        length = TAP_HexArgToInt(CommandLine, 1, &valid[1]);
        if (!valid[0] || !valid[1])
        {
            TAP_PrintString(FLASH_ERASE_HELP_TEXT);
            return;
        }
    }

    TAP_Printf("Erasing flash chip: 0x%08X -> 0x%08X...\n", address, address + length - 1);
    nvram_erase(address, address + length - 1);
    TAP_PrintString("Erasing flash chip complete!\n");
}

TAPCommand(TAP_FlashDump)
{
    BOOL valid[2] = {FALSE, FALSE};

    /* Read in params */
    uint32_t address = TAP_HexArgToInt(CommandLine, 0, &valid[0]);
    uint32_t length = TAP_HexArgToInt(CommandLine, 1, &valid[1]);
    if (!valid[0] || !valid[1])
    {
        TAP_PrintString(FLASH_DUMP_HELP_TEXT);
        return;
    }
    BOOL prettyprint = FALSE;
    if (!strcmp(CommandLine->args[2], "1"))
    {
        prettyprint = TRUE;
    }

    uint8_t buffer[256];
    uint32_t bytesRead = 0;
    while (bytesRead < length)
    {
        uint32_t bytesToRead = MIN(length - bytesRead, sizeof(buffer));
        nvram_read(address + bytesRead, bytesToRead, buffer);
        FlashDump_PrintBuffer(buffer, bytesToRead, address + bytesRead, prettyprint);
        bytesRead += bytesToRead;
    }
}

static void FlashDump_PrintBuffer(uint8_t* buffer, uint32_t length, uint32_t start_address, BOOL prettyprint)
{
    if (!prettyprint)
    {
        for (uint32_t i = 0; i < length; i++)
        {
            if ((i % 16) == 0)
            {
                TAP_Printf("\r\n");
            }
            TAP_Printf("%02X", buffer[i]);
            if (i % 2)
            {
                TAP_Printf(" ");
            }
        }
    }
    else
    {
        for (uint32_t i = 0; i < length; i += 16)
        {
            TAP_Printf("\n");
            TAP_Printf("0x%08X ", start_address + i);
            for (uint32_t j = 0; j < 16; j++)
            {
                TAP_Printf("%02X ", buffer[i + j]);
            }
            TAP_Printf(" ");
            for (uint32_t j = 0; j < 16; j++)
            {
                if (isprint(buffer[i + j]))
                {
                    TAP_Printf("%c", buffer[i + j]);
                }
                else
                {
                    TAP_Printf(".");
                }
            }
        }
    }
}
