/*
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

#pragma once

#include "project.h"

//Animation database header structure
typedef __packed struct
{
    uint16_t majorVersion;
    uint16_t minorVersion;
    uint16_t numberOfAnimations;
    uint16_t animationPatternSize;
    uint32_t animationIndexDataStartOffset; //in bytes
    uint32_t animationIndexDataSize; //in bytes
    uint32_t animationDataStartOffset; //in bytes
    uint32_t animationDataSize; //in bytes
    uint8_t numberOfLEDs;
    uint8_t singleLedIntensityValueSize; //in bytes
    uint8_t animationPatternDurationSize; //in bytes
    char lightBarLEDArrangement[61]; //For e.g. CCCWWW…WWNNNNNN
} LBAnimationDBHeader_t;

//Maximum number of animations supported by LPM parser
#define NUMBER_OF_ANIMATIONS_MAX 64

//Fix Checksum buffer size to limit SPI flash reads
#define CHECKSUM_BUFFER_SIZE 256

//Animation database index entry structure
typedef __packed struct
{
    uint16_t animationId;
    uint32_t animationOffset; //in bytes
    uint32_t animationSize; //in bytes
} LBAnimationDBIndex_t;

//Functions exposed by Animation database parser
BOOL AnimationDBInit(void);
BOOL GetAnimationDBChecksumStatus(void);
BOOL LookupAnimationFromIndexData(uint16_t animationId, uint32_t * animationOffset, uint32_t * animationSize);
uint32_t GetNumOfPatternsInAnimation(uint32_t animationSize);
void ReadPatternFromAnimationData(uint32_t patternIdx, uint8_t * patternData, uint32_t animationOffset, uint32_t animationSize);
void GetAnimationDBVersion(uint16_t * major_version, uint16_t * minor_version);
