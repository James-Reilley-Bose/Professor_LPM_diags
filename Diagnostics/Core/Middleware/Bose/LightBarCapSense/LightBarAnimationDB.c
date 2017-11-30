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
#include "LightBarAnimationDB.h"
#include "product_ext_flash_map.h"
#include "nvram.h"

//Static data structure to store all animation index entries in RAM
static LBAnimationDBIndex_t animationIndexTable[NUMBER_OF_ANIMATIONS_MAX];

//Static data structure for animation database header
static LBAnimationDBHeader_t animationDBHeader;

//Animation DB checksum status
static BOOL animationDBChecksumStatus = FALSE;

//Animation DB initialization status
static BOOL animationDBInitStatus = FALSE;

static BOOL ValidateAnimationDBChecksum(void)
{
    uint32_t animationDBSize, animationDBChecksum;
    uint8_t buffer[CHECKSUM_BUFFER_SIZE];

    //Calculate Animation DB size from the header
    animationDBSize = sizeof(LBAnimationDBHeader_t) + animationDBHeader.animationIndexDataSize + animationDBHeader.animationDataSize;

    //check if size of Animation DB is out of bounds
    if(animationDBSize > EXT_FLASH_ANIMATIONS_LENGTH)
    {
        return FALSE;
    }

    //Read the stored Animation DB checksum at the end of the database
    nvram_read(EXT_FLASH_ADDR_ANIMATIONS_START+animationDBSize, 4, (uint8_t *)&animationDBChecksum);

    //Calculate Animation DB checksum
    uint32_t calculatedChecksum = 0;
    uint32_t bytes_read = 0;

    do
    {
        uint32_t bytes_to_read = ((animationDBSize - bytes_read) > CHECKSUM_BUFFER_SIZE) ? CHECKSUM_BUFFER_SIZE : (animationDBSize - bytes_read);
        nvram_read((EXT_FLASH_ADDR_ANIMATIONS_START + bytes_read), bytes_to_read, &buffer[0]);
        bytes_read = bytes_read + bytes_to_read;

        for(uint32_t i = 0; i < bytes_to_read; i++)
        {
            calculatedChecksum = calculatedChecksum + buffer[i];
        }
    } while((animationDBSize - bytes_read) >  0);

    //validate calculated checksum with the checksum stored and return
    return (calculatedChecksum == animationDBChecksum);
}

void GetAnimationDBVersion(uint16_t * major_version, uint16_t * minor_version)
{
    //check if checksum is valid and Animation DB is initialized
    if((TRUE == animationDBChecksumStatus) && (TRUE == animationDBInitStatus))
    {
        *major_version = animationDBHeader.majorVersion;
        *minor_version = animationDBHeader.minorVersion;
    }

    return;
}

BOOL GetAnimationDBChecksumStatus(void)
{
    return(animationDBChecksumStatus);
}

BOOL AnimationDBInit(void)
{
    //TODO: Once Animations DB image is delivered via LPM SW update, add code here to parse the BOS software
    //update manifest file to get the offset of animation DB blob in external flash

    //Read Animation DB header
    nvram_read(EXT_FLASH_ADDR_ANIMATIONS_START, sizeof(LBAnimationDBHeader_t), (uint8_t *)&animationDBHeader);

    //validate Animation DB checksum
    if(TRUE == ValidateAnimationDBChecksum())
    {
        animationDBChecksumStatus = TRUE;
    }
    else
    {
        animationDBChecksumStatus = FALSE;
        animationDBInitStatus = FALSE;
        return FALSE;
    }

    //TODO: Add Animation DB version checking here, once Animation manifest file is integrated
    //with LPM SW

    //Check if Num of Animations in DB is less than NUMBER_OF_ANIMATIONS_MAX
    //if greater increase NUMBER_OF_ANIMATIONS_MAX #define
    if(!(animationDBHeader.numberOfAnimations <= NUMBER_OF_ANIMATIONS_MAX))
    {
        animationDBInitStatus = FALSE;
        return FALSE;
    }

    //Read Animation DB index data
    nvram_read((EXT_FLASH_ADDR_ANIMATIONS_START + animationDBHeader.animationIndexDataStartOffset), animationDBHeader.animationIndexDataSize, (uint8_t *)&animationIndexTable);

    animationDBInitStatus = TRUE;

    return TRUE;
}

BOOL LookupAnimationFromIndexData(uint16_t animationId, uint32_t * animationOffset, uint32_t * animationSize)
{
    //check if checksum is valid and Animation DB is initialized
    if((TRUE == animationDBChecksumStatus) && (TRUE == animationDBInitStatus))
    {
        for(uint32_t i=0; i < animationDBHeader.numberOfAnimations; i++)
        {
            if(animationId == animationIndexTable[i].animationId)
            {
                *animationOffset = animationIndexTable[i].animationOffset;
                *animationSize = animationIndexTable[i].animationSize;
                return TRUE;
            }
        }
    }

    //return zero for offset and size if animationid is not found in DB
    *animationOffset = 0;
    *animationSize = 0;
    return FALSE;
}

uint32_t GetNumOfPatternsInAnimation(uint32_t animationSize)
{
    if((TRUE == animationDBChecksumStatus) && (TRUE == animationDBInitStatus)) //check if checksum is valid
    {
        return (animationSize/animationDBHeader.animationPatternSize);
    }
    else
    {
        return (0); //return zero if checksum is invalid
    }
}

void ReadPatternFromAnimationData(uint32_t patternIdx, uint8_t * patternData, uint32_t animationOffset, uint32_t animationSize)
{
    //check if animation data read is not out of bounds of Animation DB section in SPI flash
    if(animationSize > 0 && ((animationOffset + animationSize) <= EXT_FLASH_ANIMATIONS_LENGTH))
    {
        uint32_t readAddress;
        readAddress =  EXT_FLASH_ADDR_ANIMATIONS_START + animationOffset + (patternIdx * animationDBHeader.animationPatternSize);
        nvram_read(readAddress, animationDBHeader.animationPatternSize, patternData);
    }
}
