/**
 *
 * @author Shelby Apps Team
 *
 * @attention
 *     BOSE CORPORATION.
 *     COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
 *     This program may not be reproduced, in whole or in part in any
 *     form or any means whatsoever without the written permission of:
 *         BOSE CORPORATION
 *         The Mountain,
 *         Framingham, MA 01701-9168
 */

#include "project.h"
#include "product_config.h"
#include "LightBarCapSenseTask.h"
#include "etap.h"
#include "etapLBCS.h"
#include "product_ext_flash_map.h"
#include "nvram.h"
#include "LightBarAnimationDB.h"

static unsigned int currentAnimDBExtFlashAddress = 0;

/*Please Note: In the actual product, the animation database is loaded as part
of LPM component SW update (LPM BOS file) */
//Functions for loading animation database into SPI external flash via TAP commands
static void LoadAnimationDBStart(void)
{
    //Set current animation DB ext flash address to start of anim DB memory section
    currentAnimDBExtFlashAddress = EXT_FLASH_ADDR_ANIMATIONS_START;
}

static BOOL LoadAnimationDB(uint8_t *data)
{
    uint16_t bytesToWrite = 0;
    uint16_t bytesWritten = 0;
    uint8_t writeBuffer[128];
    uint16_t inputStringLen = 0;
    uint8_t * writeBufferPtr = NULL;

    inputStringLen = strlen((const char *)data);

    //check if input string greater than 256 bytes
    if(inputStringLen > 256)
    {
        TAP_Printf("Input string greater than 256 bytes");
        return FALSE;
    }

    //Each Hex nibble will be represented as one char
    bytesToWrite = (inputStringLen)/2;

    writeBufferPtr = writeBuffer;

    if (!TAP_HexStringToUInt8Array((char*)data, writeBufferPtr, bytesToWrite))
    {
        return FALSE;
    }

    if(currentAnimDBExtFlashAddress + bytesToWrite > EXT_FLASH_ADDR_ANIMATIONS_END)
    {
        TAP_Printf("Animation DB size greater than the size of allocated External SPI flash");
        return FALSE;
    }
    else
    {
        if (bytesToWrite > 0)
        {
            /* If it is staying in current sector */
            if ((currentAnimDBExtFlashAddress / SPIFLASH_SECTOR_SIZE) == ((currentAnimDBExtFlashAddress + bytesToWrite) / SPIFLASH_SECTOR_SIZE))
            {
                /* Need to erase for first right no matter what so this handles that */
                if ((currentAnimDBExtFlashAddress % SPIFLASH_SECTOR_SIZE) == 0)
                {
                    nvram_erase(currentAnimDBExtFlashAddress, currentAnimDBExtFlashAddress + SPIFLASH_SECTOR_SIZE - 1);
                }
                nvram_write(currentAnimDBExtFlashAddress, bytesToWrite, writeBufferPtr + bytesWritten);
                currentAnimDBExtFlashAddress += bytesToWrite;
            }
            /* If it crosses sector boundary we have to erase next sector seperately*/
            else
            {
                /* Fill rest of sector */
                uint32_t bytesLeftInSector = SPIFLASH_SECTOR_SIZE - (currentAnimDBExtFlashAddress % SPIFLASH_SECTOR_SIZE);
                nvram_write(currentAnimDBExtFlashAddress, bytesLeftInSector, writeBufferPtr + bytesWritten);
                currentAnimDBExtFlashAddress += bytesLeftInSector;
                bytesToWrite -= bytesLeftInSector;
                bytesWritten += bytesLeftInSector;

                /* Erase and start new sector */
                nvram_erase(currentAnimDBExtFlashAddress, currentAnimDBExtFlashAddress + SPIFLASH_SECTOR_SIZE - 1);
                nvram_write(currentAnimDBExtFlashAddress, bytesToWrite, writeBufferPtr + bytesWritten);
                currentAnimDBExtFlashAddress += bytesToWrite;
            }
        }
    }

    return TRUE;
}

static void LoadAnimationDBEnd()
{
    //Reset current animation DB ext flash address to zero
    currentAnimDBExtFlashAddress = 0;
}

/*
 * TAP processing
 */

#define LBCS_HELP_GETVERSION "lb ver"
#define LBCS_HELP_LEDS_CLEARALL "lb lc"
#define LBCS_HELP_LEDS_SETALL "lb ls,48-byte-hex-intensities"
#define LBCS_HELP_LEDS_SETONE "lb l1,led,16-bit-hex-intensity"
#define LBCS_HELP_ANIMATION_LOADSTART_FROM_DB "lb alsfromdb,animationid,autostart,loop"
#define LBCS_HELP_ANIMATION_LOADSTART "lb als,patterncount,autostart,loop"
#define LBCS_HELP_ANIMATION_LOADPATTERN "lb alp,duration,48-byte-hex-intensities"
#define LBCS_HELP_ANIMATION_START "lb as"
#define LBCS_HELP_ANIMATION_STOPIMMEDIATE "lb asi"
#define LBCS_HELP_ANIMATION_STOPATEND "lb ase"
#define LBCS_HELP_ANIMATION_RESUME "lb ar"
#define LBCS_HELP_RESET "lb r"
#define LBCS_HELP_UPDATE_START "lb us"
#define LBCS_HELP_UPDATE_SEND_HEADER "lb uh,cyacd-header"
#define LBCS_HELP_UPDATE_SEND_DATA "lb ud,cyacd-data-line"
#define LBCS_HELP_UPDATE_END "lb ue"
#define LBCS_HELP_LOAD_ANIM_DB_START "lb ladbs"
#define LBCS_HELP_LOAD_ANIM_DB "lb ladb,upto-128bytes-of-hex-animation-DB-data"
#define LBCS_HELP_LOAD_ANIM_DB_END "lb ladbe"

TAPCommand(TAP_LBCS_GetVersion)
{
    LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_GETVERSION);

    uint8_t majorVersion, minorVersion;

    //Wait for PSoC SW version response
    while(!GetPsocSwVersion(&majorVersion, &minorVersion));

    //Print PSoC SW version on LPM TAP
    TAP_Printf("\nPSoC SW Version (major.minor): %d.%d \n",majorVersion,minorVersion);
}

TAPCommand(TAP_LBCS_LEDsClearAll)
{
    LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_LEDS_CLEARALL);
}

TAPCommand(TAP_LBCS_LEDsSetAll)
{
    uint8_t pattern[48];

    if (CommandLine->numArgs < 1)
    {
        TAP_PrintString(LBCS_HELP_LEDS_SETALL);
        return;
    }
    if (!TAP_HexStringToUInt8Array(CommandLine->args[0], &pattern[0], 48))
    {
        TAP_PrintString(LBCS_HELP_LEDS_SETALL);
        return;
    }

    LBCS_LEDsSetAll(&pattern[0], 48);
}

TAPCommand(TAP_LBCS_LEDsSetOne)
{
    uint8_t led_id;
    uint16_t led_intensity;

    if (CommandLine->numArgs < 2)
    {
        TAP_PrintString(LBCS_HELP_LEDS_SETONE);
        return;
    }

    BOOL valid = FALSE;
    led_id = TAP_DecimalArgToInt(CommandLine, 0, &valid);
    if (!valid)
    {
        TAP_PrintString(LBCS_HELP_LEDS_SETONE);
        return;
    }

    led_intensity = TAP_HexArgToInt(CommandLine, 1, &valid);
    if (!valid)
    {
        TAP_PrintString(LBCS_HELP_LEDS_SETONE);
        return;
    }

    led_intensity = ((led_intensity << 8) & 0xFF00) | ((led_intensity >> 8) & 0xFF);
    LBCS_LEDsSetOne(led_id, led_intensity);
}

TAPCommand(TAP_LBCS_AnimationLoadStartFromDB)
{
    uint8_t autostart, loop;
    uint16_t animationId;

    if (CommandLine->numArgs < 3)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADSTART_FROM_DB);
        return;
    }

    BOOL valid = FALSE;
    animationId = TAP_DecimalArgToInt(CommandLine, 0, &valid);
    if (!valid)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADSTART_FROM_DB);
        return;
    }
    autostart = TAP_DecimalArgToInt(CommandLine, 1, &valid);
    if (!valid)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADSTART_FROM_DB);
        return;
    }
    loop = TAP_DecimalArgToInt(CommandLine, 2, &valid);
    if (!valid)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADSTART_FROM_DB);
        return;
    }

    //Load and start animation from database
    LBCS_AnimationLoadStartFromDB(animationId, autostart, loop);
}

TAPCommand(TAP_LBCS_AnimationLoadStart)
{
    uint8_t numOfPatterns, autostart, loop;

    if (CommandLine->numArgs < 3)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADSTART);
        return;
    }

    BOOL valid = FALSE;
    numOfPatterns = TAP_DecimalArgToInt(CommandLine, 0, &valid);
    if (!valid)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADSTART);
        return;
    }
    autostart = TAP_DecimalArgToInt(CommandLine, 1, &valid);
    if (!valid)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADSTART);
        return;
    }
    loop = TAP_DecimalArgToInt(CommandLine, 2, &valid);
    if (!valid)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADSTART);
        return;
    }

    LBCS_AnimationLoadStart(numOfPatterns, autostart, loop);

}

TAPCommand(TAP_LBCS_AnimationLoadPattern)
{
    uint8_t pattern[48];
    uint16_t duration;

    if (CommandLine->numArgs < 2)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADPATTERN);
        return;
    }

    BOOL valid = FALSE;
    duration = TAP_DecimalArgToInt(CommandLine, 0, &valid);
    if (!valid)
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADPATTERN);
        return;
    }

    if (!TAP_HexStringToUInt8Array(CommandLine->args[1], &pattern[0], 48))
    {
        TAP_PrintString(LBCS_HELP_ANIMATION_LOADPATTERN);
        return;
    }

    LBCS_AnimationLoadPattern(duration, &pattern[0], 48);
}

TAPCommand(TAP_LBCS_AnimationStart)
{
    LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_ANIMATION_START);
}

TAPCommand(TAP_LBCS_AnimationStopImmediate)
{
    LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_ANIMATION_STOPIMMEDIATE);
}

TAPCommand(TAP_LBCS_AnimationStopAtEnd)
{
    LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_ANIMATION_STOPATEND);
}

TAPCommand(TAP_LBCS_AnimationResume)
{
    LBCS_SendSimpleCommand(PSOC_COMMS_COMMAND_ANIMATION_RESUME);
}

TAPCommand(TAP_LBCS_Reset)
{
    PsocReset();
//    LBCSPostMsg(LBCS_MSG_ID_HandleTAP);
}

TAPCommand(TAP_LBCS_UpdateStart)
{
    PsocStartBootload();
}

TAPCommand(TAP_LBCS_UpdateSendHeader)
{
    if (CommandLine->numArgs < 1)
    {
        TAP_PrintString(LBCS_HELP_UPDATE_SEND_HEADER);
        return;
    }
    PsocBootloadHeader((unsigned char *)CommandLine->args[0]);
}

TAPCommand(TAP_LBCS_UpdateSendData)
{
    if (CommandLine->numArgs < 1)
    {
        TAP_PrintString(LBCS_HELP_UPDATE_SEND_DATA);
        return;
    }
    PsocBootloadData((unsigned char *)CommandLine->args[0]);
}

TAPCommand(TAP_LBCS_UpdateEnd)
{
    PsocEndBootload();
}

TAPCommand(TAP_LBCS_LoadAnimDBStart)
{
    LoadAnimationDBStart();
}

TAPCommand(TAP_LBCS_LoadAnimDB)
{
    if (CommandLine->numArgs < 1)
    {
        TAP_PrintString(LBCS_HELP_LOAD_ANIM_DB);
        return;
    }
    LoadAnimationDB((unsigned char *)CommandLine->args[0]);
}

TAPCommand(TAP_LBCS_LoadAnimDBEnd)
{
    LoadAnimationDBEnd();
}

const struct TAP_Command LBCS_Commands[] =
{
    {
        {"ver", "version"},
        TAP_LBCS_GetVersion,
        LBCS_HELP_GETVERSION,
    },
    {
        {"lc", "ledsclearall"},
        TAP_LBCS_LEDsClearAll,
        LBCS_HELP_LEDS_CLEARALL,
    },
    {
        {"ls", "ledsetall"},
        TAP_LBCS_LEDsSetAll,
        LBCS_HELP_LEDS_SETALL,
    },
    {
        {"l1", "ledsetone"},
        TAP_LBCS_LEDsSetOne,
        LBCS_HELP_LEDS_SETONE,
    },
    {
        {"alsfromdb", "animloadstartfromdb"},
        TAP_LBCS_AnimationLoadStartFromDB,
        LBCS_HELP_ANIMATION_LOADSTART_FROM_DB,
    },
    {
        {"als", "animloadstart"},
        TAP_LBCS_AnimationLoadStart,
        LBCS_HELP_ANIMATION_LOADSTART,
    },
    {
        {"alp", "animloadpat"},
        TAP_LBCS_AnimationLoadPattern,
        LBCS_HELP_ANIMATION_LOADPATTERN,
    },
    {
        {"as", "animstart"},
        TAP_LBCS_AnimationStart,
        LBCS_HELP_ANIMATION_START,
    },
    {
        {"ase", "animstopatend"},
        TAP_LBCS_AnimationStopAtEnd,
        LBCS_HELP_ANIMATION_STOPATEND,
    },
    {
        {"asi", "animstopimm"},
        TAP_LBCS_AnimationStopImmediate,
        LBCS_HELP_ANIMATION_STOPIMMEDIATE,
    },
    {
        {"ar", "animresume"},
        TAP_LBCS_AnimationResume,
        LBCS_HELP_ANIMATION_RESUME,
    },
    {
        {"r", "reset"},
        TAP_LBCS_Reset,
        LBCS_HELP_RESET,
    },
    {
        {"us", "updatestart"},
        TAP_LBCS_UpdateStart,
        LBCS_HELP_UPDATE_START,
    },
    {
        {"uh", "updateheader"},
        TAP_LBCS_UpdateSendHeader,
        LBCS_HELP_UPDATE_SEND_HEADER,
    },
    {
        {"ud", "updatedata"},
        TAP_LBCS_UpdateSendData,
        LBCS_HELP_UPDATE_SEND_DATA,
    },
    {
        {"ue", "updateend"},
        TAP_LBCS_UpdateEnd,
        LBCS_HELP_UPDATE_END,
    },
    {
        {"ladbs", "loadanimdbstart"},
        TAP_LBCS_LoadAnimDBStart,
        LBCS_HELP_LOAD_ANIM_DB_START,
    },
    {
        {"ladb", "loadanimdb"},
        TAP_LBCS_LoadAnimDB,
        LBCS_HELP_LOAD_ANIM_DB,
    },
    {
        {"ladbe", "loadanimdbend"},
        TAP_LBCS_LoadAnimDBEnd,
        LBCS_HELP_LOAD_ANIM_DB_END,
    },
};
const int Num_LBCS_Commands = sizeof(LBCS_Commands) / sizeof(struct TAP_Command);

//
// @func TAP_LBCS
// @brief TAP command to control the lightbar / capsense component.
//
//
TAPCommand(TAP_LBCS)
{
    if (TAP_ProcessSubCommand(LBCS_Commands, Num_LBCS_Commands, CommandLine) == FALSE)
    {
        TAP_PrintString(LBCS_HELP_TEXT);
        TAP_PrintSubCommands(LBCS_Commands, Num_LBCS_Commands);
    }
}
