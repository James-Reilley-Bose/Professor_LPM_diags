////////////////////////////////////////////////////////////////////////////////
/// @file          SourceInterface.c
/// @brief         Header file for variant specific source change
///
/// @author        dr1005920
/// Copyright      2015 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#ifndef SOURCE_RECIPE_H
#define SOURCE_RECIPE_H

#include "sourceInterface.h"

void doAudioSourceMute(BOOL mute);
void doVideoSourceMute(BOOL mute);
void doAudioRoute(SOURCE_ID src);
void doVideoRoute(SOURCE_ID src);
void doCECRoute(SOURCE_ID current, SOURCE_ID target);
BOOL doVideoPresentCheck(void);

#endif /* SOURCE_INTERFACE_H */
