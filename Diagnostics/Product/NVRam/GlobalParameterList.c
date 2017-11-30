//////////////////////////////////////////////////////////////////////////////
///   @file          globalParameterList.c
///   @brief
///   @author        Brian White
///   @date          Creation Date: 06/26/2013
///   Language:      C
///
///
///   Description:
///
///
///
///
///
///
///
///
///
///
///
///   Copyright:     (C) 2013 Bose Corporation, Framingham, MA
///////////////////////////////////////////////////////////////////////////////
#include "project.h"
#include "globalParameterList.h"
#include "CEC_ControlInterface.h"

#pragma pack(1)
typedef struct
{
  uint32_t availableLanguageMask;
} LanguageParameterList;
#pragma pack()

#pragma pack(1)
typedef struct
{
  BOOL     supportCenterOption;
  BOOL     supportSurroundOption;
  BOOL     supportAdaptIQCurveB;
} SpeakerParameterList;
#pragma pack()

static const SpeakerParameterList globalSpeakerParameters =
{
      .supportCenterOption   = TRUE,
      .supportSurroundOption = FALSE,
      .supportAdaptIQCurveB  = TRUE,
};

static const uint32_t globalCECFeatures =
{
        (CEC_FEATURE_ONE_TOUCH_PLAY | CEC_FEATURE_ARC | CEC_FEATURE_GLOBAL_STANDBY | CEC_FEATURE_ON_SCREEN_NAME | CEC_FEATURE_SYSTEM_AUDIO | CEC_FEATURE_KEY_RX | CEC_FEATURE_ROUTING_CONTROLS | CEC_IMAGE_VIEW_ON),
};

static const char* globalCECDisplayName =
{
    "SoundTouch1337", //TODO: Update when marketing says so...
};

//Indexed by region and variant
static const LanguageParameterList globalLanguageParameters[REGION_NUM] =
{
  { NULL }, // Undefined
  {// US & AIM
      .availableLanguageMask = AIM_LANGUAGE_SUPPORT,
  },
  {// EURO
      .availableLanguageMask = EURO_LANGUAGE_SUPPORT,
  },
  {// Asia Pacific
      .availableLanguageMask = AP_LANGUAGE_SUPPORT,
  },
  {//JAPAN
      .availableLanguageMask = JAPAN_LANGUAGE_SUPPORT,
  },
};

static PRODUCT_VARIANT globalProductVariant = PRODUCT_UNDEFINED;
static REGION_VARIANT globalRegionVariant = REGION_UNDEFINED;
// TODO mmoretti rfid stuff - add country code here and in initialize and nvram etc etc

void GP_initializeGlobalParameters(PRODUCT_VARIANT product, REGION_VARIANT region)
{
    globalProductVariant = product;
    globalRegionVariant = region;
}

BOOL GP_supportCenterSpearkerOption(void)
{
    return globalSpeakerParameters.supportCenterOption;
}

BOOL GP_supportSurroundSpearkerOption(void)
{
    return globalSpeakerParameters.supportSurroundOption;
}

BOOL GP_supportAdaptIQCurveBStorage(void)
{
    return globalSpeakerParameters.supportAdaptIQCurveB;
}

uint32_t GP_availableLanguageMask(void)
{
    return globalLanguageParameters[1].availableLanguageMask;
}

uint32_t GP_defaultCECFeatures(void)
{
  return globalCECFeatures;
}

const char* GP_CECDisplayName(void)
{
  return globalCECDisplayName;
}

