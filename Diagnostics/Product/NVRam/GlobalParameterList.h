//////////////////////////////////////////////////////////////////////////////
///   @file          globalParameterList.h
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

#ifndef GLOBAL_PARAMETER_LIST_H
#define GLOBAL_PARAMETER_LIST_H

#include "nv_mfg.h"
#include "RivieraLPM_IpcProtocol.h"

void GP_initializeGlobalParameters(PRODUCT_VARIANT product, REGION_VARIANT region);

BOOL     GP_supportCenterSpearkerOption(void);
BOOL     GP_supportSurroundSpearkerOption(void);
BOOL     GP_supportAdaptIQCurveBStorage(void);
uint32_t GP_availableLanguageMask(void);
uint32_t GP_defaultCECFeatures(void);
const char* GP_CECDisplayName(void);

#endif //GLOBAL_PARAMETER_LIST_H
