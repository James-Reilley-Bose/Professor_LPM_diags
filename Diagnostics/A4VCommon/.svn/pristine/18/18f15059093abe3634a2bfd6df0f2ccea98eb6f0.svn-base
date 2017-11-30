/*
  File  : AccessortManager.c
  Title :
  Author  : dr1005920
  Created : 10/06/2016
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:   Implementation for handling wireless wired accessorts

===============================================================================
*/

#include "project.h"
#include "UITask.h"
#include "genTimer.h"
#include "systemBehaviors.h"
#include "viewNotify.h"
#include "AccessoryManager.h"
#include "nv_mfg_struct.h"
#include "DeviceModelTask.h"
#include "DeviceModel_DSP.h"
#include "systemBehaviors.h"
#include "WirelessAudioUtilities.h"
#include "IPCBulk.h"
#include "versionlib.h"
#include "UpdateModuleSpeakers.h"
#include "buffermanager.h"
#include "WirelessAudioData.h"
#include "WirelessAudioTask.h"
#include <stdio.h>
#include "WiredIDConsoleTask.h"
#include "AccessoryVariant.h"
#include "nv_system.h"
#include "ProductSystemParams.h"

#define ACCESSORY_STATE_CHANGED (1<<10)

// These will all get swallowed into one struct soon i hope?
static IpcAccessoryFlags_t currentAccessoryFlags;
static SpeakerConfig_t currentSpeakerConfig;
static IpcSpeakerPackage_t currentSpeakerVariant;

static AccesoryManagerUI_State_t state = ACCESSORY_STATE_INVALID;

static TimerHandle_t accessoryWD = NULL;
static TimerHandle_t initialHoldOffTimer = NULL;
static BOOL pairingActive = FALSE;
static BOOL initialHoldOff = TRUE;
static AccessoryVersion accVersions[ACCESSORY_POSITION_NUM_OF_INTERNAL] = {0};

static uint32_t speakerWatchdogs[ACCESSORY_POSITION_NUM_OF_INTERNAL] = {0};
static IpcAccessoryList_t accList = {0};
static BOOL isUpdatePending = FALSE;
static PairingToneState_t accessoryToneState = ACCESSORY_TONE_IDLE;
static SpeakerPackageVariant_t const* baseSpkrVar = NULL;
// End struct plan

static uint16_t accListTransferID = 0;

SCRIBE_DECL(uitask);
SCRIBE_DECL(sys_events);

static const char* speakerConfigStrings[] ={
    "BARDEEN_OMNIVO",
    "BARDEEN_JEWELCUBES",
    "GINGER",
    "GINGERSKIPPER",
    "GINGERMAXWELL",
    "GINGERSKIPPERMAXWELL",
    "GINGERLOVEY",
    "GINGERLOVEYMAXWELL",
    "PROFESSOR",
    "PROFESSORSKIPPER",
    "PROFESSORMAXWELL",
    "PROFESSORSKIPPERMAXWELL",
    "PROFESSORLOVEY",
    "PROFESSORLOVEYMAXWELL",
};


////////////////////////////////////////////////////////////////////////////////////
///////////// MISC FUNCTIONS TO HELP WITH SOME INTERNAL HOUSE KEEPING //////////////
////////////////////////////////////////////////////////////////////////////////////
static uint32_t AccessoryManager_InitListToLastKnownConfig(void);
static void AccessoryManager_SetAllSpeakersActive(void);
static void AccessoryManager_ClearAccessory(uint32_t listIndex, BOOL fullClear);
static void AccessoryManager_ClearAllWithStatus(AccessoryConnectionStatus_t status);
static char const* AccessoryManager_GetSpeakerVariantString(IpcSpeakerPackage_t package);

////////////////////////////////////////////////////////////////////////////////////
////// SOME FUNCTIONS TO SET THE NV PARAMS BASED ON WHAT WE HAVE CURRENTLY /////////
////////////////////////////////////////////////////////////////////////////////////
static void AccessoryManager_UpdateSpeakerConfig(BOOL persist);
static void AccessoryManager_UpdateDSPSpeakerConfig(void);

////////////////////////////////////////////////////////////////////////////////////
//////// FUNCTIONS TO CONVERT ACCESSORY LIST TO THE FORMATS WE SAVE+SHARE //////////
////////////////////////////////////////////////////////////////////////////////////
static IpcThermistorLocation_t AccessoryManager_GetThermistorLocationFromChannel(WA_ChannelID_t channel);
static SpeakerConfig_t AccessoryManager_GetSpeakerConfig(void);
static IpcAccessoryFlags_t AccessoryManager_GetAccessoryFlags(void);

////////////////////////////////////////////////////////////////////////////////////
/////// SOME FUNCTIONS TO TRANSLATE FROM THE WA TASK TO ACCESSORT MANAGER //////////
////////////////////////////////////////////////////////////////////////////////////
static AccessoryPosition_t AccessoryManager_DarrChannel_2_AccPosition(uint8_t channel);
static AccessoryType_t AccessoryManager_DarrChannel_2_AccType(uint8_t channel);
static uint32_t AccessoryManager_FindNextOpenListLocation(uint8_t channel);

////////////////////////////////////////////////////////////////////////////////////
//////// SOME FUNCTIONS TO SEND THE FULL SIZE ACCESSORY LIST TO WHOMEVER ///////////
////////////////////////////////////////////////////////////////////////////////////
static void AccessoryManager_ListTransfer_HandleBulkTransferComplete(uint16_t transferID);


////////////////////////////////////////////////////////////////////////////////////
////////////// API FOR THE WA CODE TO CHECK AND DO BACKGROUND UPDATE  //////////////
////////////////////////////////////////////////////////////////////////////////////
BOOL AccessoryManager_IsUpdatePending()
{
    return isUpdatePending;
}

////////////////////////////////////////////////////////////////////////////////////
///////////// API FOR THE WORLD TO ASK WHAT CURRENT SPEAKER SETUP IS ///////////////
////////////////////////////////////////////////////////////////////////////////////
IpcAccessoryFlags_t AccessoryManager_GetCurrentAccessoryFlags(void)
{
    return currentAccessoryFlags;
}

SpeakerConfig_t AccessoryManager_GetCurrentSpeakerConfig(void)
{
    return currentSpeakerConfig;
}

IpcSpeakerPackage_t AccessoryManager_GetCurrentSpeakerVariant(void)
{
    return currentSpeakerVariant;
}

////////////////////////////////////////////////////////////////////////////////////
////////////// API FOR THE WORLD TO GET VERSION INFO FROM SPEAKERS /////////////////
////////////////////////////////////////////////////////////////////////////////////
void AccessoryManager_GetSpeakerLPMVersion(AccessoryIndex index, char* buffer, uint32_t len)
{
    if(len >= SIZE_OF_VERSION)
    {
        memcpy(buffer, accList.accessory[index].version, SIZE_OF_VERSION);
    }
}

void AccessoryManager_GetSpeakerWAVersion(AccessoryIndex index, char* buffer, uint32_t len)
{
    snprintf(buffer, len, "%d", accVersions[index].darr_ver);
}



////////////////////////////////////////////////////////////////////////////////////
///////////// API FOR THE UI TO CHECK FOR RELEVANT ACCESSORY STATES ////////////////
////////////////////////////////////////////////////////////////////////////////////
AccesoryManagerUI_State_t AccessoryManager_GetUI_State()
{
    return state;
}

////////////////////////////////////////////////////////////////////////////////////
///////////// API PRESENTED TO UI TASK FOR MESSAGE HANDLING AND INIT ///////////////
////////////////////////////////////////////////////////////////////////////////////
void AccessoryManager_Init()
{
    accessoryWD = createTimer(ACCESSORY_WD_TIMEOUT,
                                        NULL,
                                        UI_MSG_ID_CHECK_ACCESSORIES,
                                        TRUE,
                                        tBlockIdCheckAccessoriesTimer,
                                        "CheckAccessoriesTimer",
                                        NULL);

    initialHoldOffTimer = createTimer(ACCESSORY_HOLD_OFF_TIMEOUT,
                               NULL,
                               UI_MSG_ID_HoldOffDone,
                               FALSE,
                               tBlockIdAccessoryMuteTimer,
                               "accessoryMuteTimer",
                               NULL);

    debug_assert(initialHoldOffTimer);
    debug_assert(accessoryWD);
    
    baseSpkrVar = AccessoryVariant_GetSpeakerPackageVaraint();
    debug_assert( baseSpkrVar );

    currentAccessoryFlags = (IpcAccessoryFlags_t)NV_GetSpeakerAccessories();
    currentSpeakerConfig =  (SpeakerConfig_t)NV_GetExpectedSpeakerConfig();
    currentSpeakerVariant = (IpcSpeakerPackage_t)NV_GetSpeakerPackageVariant();
   
    AccessoryManager_InitListToLastKnownConfig();
}

void AccessoryManager_On(void)
{    
    isUpdatePending = FALSE;

    uint32_t numExpected = 0;

    // Set all that are expected to missing if they still are when shutting off
    for(uint32_t listIndex = ACCESSORY_POSITION_START; listIndex < ACCESSORY_POSITION_NUM_OF_INTERNAL; listIndex++)
    {
        // these never change... could only be used if we Atmos and have dual use maxwells
        accList.accessory[listIndex].position = listIndex;

        // If status is still expected it has yet to be heard from...
        if(accList.accessory[listIndex].status == ACCESSORY_CONNECTION_EXPECTED)
        {
            numExpected++;
        }
        else if(accList.accessory[listIndex].status == ACCESSORY_CONNECTION_NONE)
        {
            accList.accessory[listIndex].type = ACCESSORY_INVALID;
        }
    }

    accList.accessory[ACC_POS_REAR_1].type = ACCESSORY_MAXWELL;
    accList.accessory[ACC_POS_REAR_2].type = ACCESSORY_MAXWELL;

    WiredBassDetectEnable(TRUE);

    if(numExpected > 0)
    {
        initialHoldOff = TRUE;
        state = ACCESSORY_STATE_CONNECTED;
        UI_ViewNotify(ACCESSORY_STATE_CHANGED);

    }
    else
    {
        AccessoryManager_InitialHoldOffDone();
    }

    // we come up blind so trust yesterdays weather till we step outside
    currentAccessoryFlags = (IpcAccessoryFlags_t)AccessoryManager_GetAccessoryFlags();
    currentSpeakerConfig =  (SpeakerConfig_t)AccessoryManager_GetSpeakerConfig();
    currentSpeakerVariant = (IpcSpeakerPackage_t)AccessoryVariant_GetSpeakerVariant();

    // Only call dsp as it uses the above values from nv
    AccessoryManager_UpdateDSPSpeakerConfig();
}

void AccessoryManager_Off(void)
{
    timerStop(accessoryWD, 0);
    WiredBassDetectEnable(FALSE);

    // Set all that are expected to missing if they still are when shutting off
    for(uint32_t listIndex = ACCESSORY_POSITION_START; listIndex < ACCESSORY_POSITION_NUM_OF_INTERNAL; listIndex++)
    {
        // List index is set when a slot is configured so don't touch till it is
        if(accList.accessory[listIndex].position != ACCESSORY_POSITION_INVALID)
        {
            // If status is still expected it has yet to be heard from...
            if(accList.accessory[listIndex].status == ACCESSORY_CONNECTION_EXPECTED)
            {
                // thus it is dead to us
                AccessoryManager_ClearAccessory(listIndex, TRUE);
            }
            else if( IS_ACCESSORY_WIRELESS(accList.accessory[listIndex]) )
            {
                accList.accessory[listIndex].status = ACCESSORY_CONNECTION_EXPECTED;
            }
        }
    }

    AccessoryManager_UpdateSpeakerConfig(AccessoryVariant_SaveSpeakerPackageOnShutdown());
    
    AccessoryManager_SetAllSpeakersActive();

    state = ACCESSORY_STATE_INVALID;
    UI_ViewNotify(ACCESSORY_STATE_CHANGED);
    
    initialHoldOff = TRUE;
}

void AccessoryManager_InitialHoldOffDone(void)
{
    initialHoldOff = FALSE;

    // Clear all that are expected but have yet to report
    for(uint32_t listIndex = ACCESSORY_POSITION_START; listIndex < ACCESSORY_POSITION_NUM_OF_INTERNAL; listIndex++)
    {
        // If status is still expected it has yet to be heard from...
        if(accList.accessory[listIndex].status == ACCESSORY_CONNECTION_EXPECTED)
        {
            // thus it is dead to us
            AccessoryManager_ClearAccessory(listIndex, FALSE);
        }
    }
    // Check what we have
    AccessoryManager_UpdateSpeakerConfig(FALSE);
    // Start speaker watchdog
    timerStart(accessoryWD, 0, &uiTaskHandle->Queue);
    // Request speaker versions
    WirelessAudioPostMsg(WA_MSG_ID_GetSpeakerVersion, NOP_CALLBACK, NULL);
}

void AccessoryManager_HandleWatchdog(void)
{
    if(accessoryToneState != ACCESSORY_TONE_IDLE)
    {
        // TODO : work through pairing
        // SystemBehavior_PlayToneToConnected(accessoryToneState);
        accessoryToneState++;
        if(accessoryToneState == ACCESSORY_TONE_INVALID)
        {
          accessoryToneState = ACCESSORY_TONE_IDLE;
        }
    }

    uint32_t numExpected = 0;
    for(uint32_t listIndex = ACCESSORY_POSITION_START; listIndex < ACCESSORY_POSITION_NUM_OF_INTERNAL; listIndex++)
    {
        // Only wireless speakers need to be monitored
        if(accList.accessory[listIndex].status == ACCESSORY_CONNECTION_WIRELESS)
        {
            speakerWatchdogs[listIndex]++;
            if(speakerWatchdogs[listIndex] > ACCESSORY_MAX_PINGS)
            {
                AccessoryManager_ClearAccessory(listIndex, FALSE);
                AccessoryManager_UpdateSpeakerConfig(FALSE);
            }
        }
        
        if(accList.accessory[listIndex].status == ACCESSORY_CONNECTION_EXPECTED)
        {
            numExpected++;
        }
    }
    
    // This should always be the case if WD is running
    if(state != ACCESSORY_STATE_INVALID && state != ACCESSORY_STATE_PAIRING)
    {
        AccesoryManagerUI_State_t newState = ACCESSORY_STATE_INVALID;
        if(numExpected > 0)
        {
            newState = ACCESSORY_STATE_DISCONNECTED;
        }
        else if(IS_ACCESSORY_MISCONFIG(accList.accessory[ACC_POS_REAR_1], accList.accessory[ACC_POS_REAR_2]))
        {
            newState = ACCESSORY_STATE_MISCONFIG;
        }
        else if((accList.accessory[ACC_POS_SUB].type == ACCESSORY_LOVEY && accList.accessory[ACC_POS_SUB_2].type == ACCESSORY_SKIPPER) ||
                (accList.accessory[ACC_POS_SUB_2].type == ACCESSORY_LOVEY && accList.accessory[ACC_POS_SUB].type == ACCESSORY_SKIPPER))
        {
            newState = ACCESSORY_STATE_MISCONFIG;
        }
        else
        {
            newState = ACCESSORY_STATE_CONNECTED;
        }
                
        if(newState != state)
        {
            state = newState;
            UI_ViewNotify(ACCESSORY_STATE_CHANGED);
        }
    }
}

void AccessoryManager_HandleLlwInitialized(void)
{
    // If the ON code decided to do hold off do it
    if(initialHoldOff)
    {
        // Start hold off timer
        timerStart(initialHoldOffTimer, 0, &uiTaskHandle->Queue);
    }
}

void AccessoryManager_HandleLlwPairing(BOOL enabled)
{
    pairingActive = enabled;

    state = (enabled) ? ACCESSORY_STATE_PAIRING : ACCESSORY_STATE_CONNECTED;
    UI_ViewNotify(ACCESSORY_STATE_CHANGED);

    if(enabled)
    {
        timerStop(accessoryWD, 0);
        initialHoldOff = FALSE;

        // DR_TODO - do we want to clear old pairings
    }
    else
    {
        // Clear out any "partially paired" legacy speakers.
        AccessoryManager_ClearAllWithStatus(ACCESSORY_CONNECTION_LEGACY_PAIRING);

        // Done pairing so figure out what we have
        AccessoryManager_UpdateSpeakerConfig(TRUE);

        AccessoryVariant_HandleLlwPairingClosed();

        // play tones flag
        accessoryToneState = ACCESSORY_TONE_START;

        // restart speaker watchdog
        timerStart(accessoryWD, 0, &uiTaskHandle->Queue);
    }
}


IpcAccessoryList_t* AccessoryManager_GetAccList(void)
{
   return &accList;
}

AccessoryVersion* AccessoryManager_GetAccVersion(void)
{
   return (AccessoryVersion*)&accVersions;
}

// We only allow others to set active state in accessory list
void AccessoryManager_SetSpeakerActiveInformation(IpcAccessoryList_t* activeStates)
{
    // This needs to be legacy as its consumers only send the 3 item list
    for(uint32_t i = ACCESSORY_POSITION_START; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        accList.accessory[i].active = activeStates->accessory[i].active;
    }

    // Need to think more on how this should be done
    AccessoryManager_UpdateDSPSpeakerConfig();
}

////////////////////////////////////////////////////////////////////////////////////
///////////// MISC FUNCTIONS TO HELP WITH SOME INTERNAL HOUSE KEEPING //////////////
////////////////////////////////////////////////////////////////////////////////////

// DR_TODO - For this function I want to replace it by saving off whole acc list
static uint32_t AccessoryManager_InitListToLastKnownConfig(void)
{
    uint32_t numExpected = 0;
    
    // Set position to invalid on init so that we can tell if we have booted once
    for(uint32_t i = ACCESSORY_POSITION_START; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        accList.accessory[i].position = ACCESSORY_POSITION_INVALID;
        accList.accessory[i].type = ACCESSORY_INVALID;
    }

    if(NV_GetExpectedSpeakerConfig() & (UI_SPEAKER_WIRED_BASS | UI_SPEAKER_WIRELESS_BASS))
    {
        accList.accessory[ACCESSORY_POSITION_SUB].status = ACCESSORY_CONNECTION_EXPECTED;
        numExpected += 1;
        if(UI_SPEAKER_SUB_IS_PBB & NV_GetExpectedSpeakerConfig())
        {
            accList.accessory[ACCESSORY_POSITION_SUB].type = ACCESSORY_LOVEY;
        }
    }
    else
    {
        accList.accessory[ACCESSORY_POSITION_SUB].status = AccessoryVariant_GetSpeakerNeverConfig();
    }

    if(NV_GetExpectedSpeakerConfig() & (UI_SPEAKER_SECOND_BASS))
    {
        accList.accessory[ACCESSORY_POSITION_SUB_2].status = ACCESSORY_CONNECTION_EXPECTED;
        numExpected += 1;
    }
    else
    {
        // This is purely optional so always reset to none
        accList.accessory[ACCESSORY_POSITION_SUB_2].status = ACCESSORY_CONNECTION_NONE;
    }

    if(NV_GetExpectedSpeakerConfig() & UI_SPEAKERS_LEFT_RIGHT)
    {
        accList.accessory[ACCESSORY_POSITION_LEFT_REAR].status = ACCESSORY_CONNECTION_EXPECTED;
        accList.accessory[ACCESSORY_POSITION_RIGHT_REAR].status = ACCESSORY_CONNECTION_EXPECTED;
        numExpected += 2;
    }
    else
    {
        accList.accessory[ACCESSORY_POSITION_LEFT_REAR].status = AccessoryVariant_GetSpeakerNeverConfig();
        accList.accessory[ACCESSORY_POSITION_RIGHT_REAR].status = AccessoryVariant_GetSpeakerNeverConfig();
    }

    return numExpected;
}

static void AccessoryManager_SetAllSpeakersActive(void)
{
    for(uint32_t i = ACCESSORY_POSITION_START; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        accList.accessory[i].active = ACCESSORY_ACTIVATED;
    }

    AccessoryManager_UpdateDSPSpeakerConfig();
}


static void AccessoryManager_ClearAccessory(uint32_t listIndex,  BOOL fullClear)
{
    if(fullClear == TRUE)
    {
        if(listIndex != ACCESSORY_POSITION_SUB_2)
        {
            accList.accessory[listIndex].status = AccessoryVariant_GetSpeakerNeverConfig();
        }
        else
        {
            accList.accessory[listIndex].status = ACCESSORY_CONNECTION_NONE;
        }
    }
    else
    {
        if(listIndex != ACCESSORY_POSITION_SUB_2)
        {
            accList.accessory[listIndex].status = AccessoryVariant_GetSpeakerDefault((AccessoryType_t)accList.accessory[listIndex].type);
        }
        else
        {
            accList.accessory[listIndex].status = ACCESSORY_CONNECTION_NONE;
            accList.accessory[listIndex].type = ACCESSORY_INVALID;
        }
    }

    // DR_TODO - we allegedly want to save type between boots for lovey vs skipper
    // commented out at request of DR. 
    // accList.accessory[listIndex].type = ACCESSORY_INVALID;
    // Don't clear position it never changes and PTS uses it
    // accList.accessory[listIndex].position = ACCESSORY_POSITION_INVALID;
    memset(accList.accessory[listIndex].sn, 0, WA_SERIAL_NO_LENGTH);
    memset(accList.accessory[listIndex].version, 0, 8);
    memset(&accVersions[listIndex], 0, sizeof(AccessoryVersion));
}

static void AccessoryManager_ClearAllWithStatus(AccessoryConnectionStatus_t status)
{
    for(uint_fast8_t i = ACCESSORY_POSITION_START; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        if(accList.accessory[i].status == status)
        {
            AccessoryManager_ClearAccessory(i, FALSE);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
//////// FUNCTIONS TO CONVERT ACCESSORY LIST TO THE FORMATS WE SAVE+SHARE //////////
////////////////////////////////////////////////////////////////////////////////////
static IpcThermistorLocation_t AccessoryManager_GetThermistorLocationFromChannel(WA_ChannelID_t channel)
{
    switch(channel)
    {
        case WA_CHANNEL_SURROUND_LEFT:
            return IPC_THERM_REAR_SURROUND_L;
        case WA_CHANNEL_SURROUND_RIGHT:
            return IPC_THERM_REAR_SURROUND_R;
        case WA_CHANNEL_BASS_SKIPPER:
        case WA_CHANNEL_BASS_PBB:    
            return IPC_THERM_BASS;
    }
    return IPC_THERM_LOCATION_TYPES;
}

static IpcAccessoryFlags_t AccessoryManager_GetAccessoryFlags(void)
{
    //Accessory Bits only care about whether something is wired or not.
    //For ginger, speaker package convey's most of the information.
    IpcAccessoryFlags_t accessoryFlags = IPC_NO_ACCESSORY;

    if((accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_WIRED) ||
       (accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_BOTH))
    {
        accessoryFlags |= IPC_BASSBOX_WIRED;
    }
    if (accList.accessory[ACC_POS_REAR_1].status == ACCESSORY_CONNECTION_WIRED &&
        accList.accessory[ACC_POS_REAR_2].status == ACCESSORY_CONNECTION_WIRED)
    {
        accessoryFlags |= IPC_REAR_SPEAKERS_WIRED;
    }

    return accessoryFlags;
}

static SpeakerConfig_t AccessoryManager_GetSpeakerConfig(void)
{
    SpeakerConfig_t retVal = (SpeakerConfig_t)NV_GetExpectedSpeakerConfig();
   
    if((accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_WIRED) ||
       (accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_BOTH))
    {
        // These are mutually exclusive from config standpoint wired wins
        retVal |= UI_SPEAKER_WIRED_BASS;
        retVal &= ~(UI_SPEAKER_WIRELESS_BASS);
    }
    else if(accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_WIRELESS)
    {
        // These are mutually exclusive
        retVal &= ~(UI_SPEAKER_WIRED_BASS);
        retVal |= UI_SPEAKER_WIRELESS_BASS;
    }
    else
    {
        retVal &= ~(UI_SPEAKERS_BOTH_BASS);
    }

    if((accList.accessory[ACC_POS_SUB_2].status == ACCESSORY_CONNECTION_WIRELESS))
    {
        // These are mutually exclusive from config standpoint wired wins
        retVal |= UI_SPEAKER_SECOND_BASS;
    }
    else
    {
        retVal &= ~(UI_SPEAKER_SECOND_BASS);
    }

    if(accList.accessory[ACC_POS_SUB].type == ACCESSORY_LOVEY)
    {
        retVal |= UI_SPEAKER_SUB_IS_PBB;
    }
    else if(accList.accessory[ACC_POS_SUB].type == ACCESSORY_SKIPPER)
    {
        retVal &= ~UI_SPEAKER_SUB_IS_PBB;
    }
	// else we leave the same if still "in EXPECTED state"

    // If rears are connected and valid
    if (IS_ACCESSORY_CONNECTED(accList.accessory[ACC_POS_REAR_1]) &&
        IS_ACCESSORY_CONNECTED(accList.accessory[ACC_POS_REAR_2]))
    {
        retVal |= UI_SPEAKERS_LEFT_RIGHT;
    }
    else
    {
        retVal &= ~(UI_SPEAKERS_LEFT_RIGHT);
    }

    return retVal;
}

////////////////////////////////////////////////////////////////////////////////////
/////// TWO WAYS OF FINDING A SPEAKER IN OUR ACCESSORY LIST LEGACY OR NEW //////////
////////////////////////////////////////////////////////////////////////////////////

static uint32_t AccessoryManager_GetSpeakerIndexFromSerialCRC(uint32_t serialNumberCRC)
{
    for(uint32_t i = ACCESSORY_POSITION_START; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        if(serialNumberCRC == WirelessAudioUtilities_CalculateSerialCRC(accList.accessory[i].sn))
        {
            return i;
        }
    }
    return ACCESORY_INDEX_INVALID;
}


static uint32_t AccessoryManager_GetSpeakerIndexFromChannel(uint8_t channel)
{
    AccessoryPosition_t position = AccessoryManager_DarrChannel_2_AccPosition(channel);
    for(uint32_t i = ACCESSORY_POSITION_START; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        // Picks the first so ya...
        if(accList.accessory[i].position == position)
        {
            return i;
        }
    }
    return ACCESORY_INDEX_INVALID;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////// API TO THE WIRED BASS CODE //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void AccessoryManager_HandleWiredSpeakerUpdate(AccessoryDescription_t* speaker)
{
    LOG(sys_events, ROTTEN_LOGLEVEL_VERBOSE, "Speaker: %d, Connected = %d", speaker->position, speaker->status);

    if(speaker->position == ACCESSORY_POSITION_SUB)
    {
        if(accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_BOTH)
        {
            if(speaker->status == ACCESSORY_CONNECTION_NONE)
            {
              // If it was both we switch to wireless and give our trust to the watch dog
              // The wired model is binary wired or dead
              accList.accessory[ACC_POS_SUB].status = ACCESSORY_CONNECTION_WIRELESS;
            }
            else
            {
                // switch to wired pre timeout to another bass type.
                accList.accessory[ACC_POS_SUB].type = speaker->type;
            }
        }
        else if(accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_WIRELESS)
        {
            if(speaker->status == ACCESSORY_CONNECTION_WIRED)
            {
                // Since we will mux to wired we should respect its type
                accList.accessory[ACC_POS_SUB].type = speaker->type;
                accList.accessory[ACC_POS_SUB].status = ACCESSORY_CONNECTION_BOTH;
            }
            // else nothing
        }
        else // aka only wired or none
        {
            // Only change type if solely wired
            accList.accessory[ACC_POS_SUB].type = speaker->type;
            accList.accessory[ACC_POS_SUB].status = speaker->status;
        }

        accList.accessory[ACC_POS_SUB].position = speaker->position;
        // DR - We don't set serial here incase connected both we will recall our friend

        AccessoryManager_UpdateSpeakerConfig(TRUE);
    }
}


////////////////////////////////////////////////////////////////////////////////////
////// SOME FUNCTIONS TO SET THE NV PARAMS BASED ON WHAT WE HAVE CURRENTLY /////////
////////////////////////////////////////////////////////////////////////////////////
static void AccessoryManager_UpdateDSPSpeakerConfig(void)
{
    SpeakerPackageVariant_t spVar;
    spVar.numOfAccessories = baseSpkrVar->numOfAccessories;
    
    // Copy in const ones
    for(uint8_t i = 0; i < spVar.numOfAccessories; i++)
    {
      memcpy(&spVar.accessory[i], &baseSpkrVar->accessory[i], sizeof(AccessoryDescriptionShort_t));
    }

    // Figure out dynamic ones
    for(uint8_t i = 0; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        BOOL accValid = FALSE;
        AccessoryDescriptionShort_t accDesc = AccessoryVariant_GetDescShort( &accList.accessory[i], &accValid );
        if( accValid )
        {
            memcpy(&spVar.accessory[spVar.numOfAccessories], &accDesc, sizeof(AccessoryDescriptionShort_t));
            spVar.numOfAccessories++;
        }
    }

    uint8_t params[2] = { 0, 0 };
    IPCRouter_Send(IPC_DEVICE_DSP, IPC_AUDIO_SPEAKER_PACKAGE, params, &spVar, sizeof(SpeakerPackageVariant_t));
    
    UI_ViewNotify(SPEAKER_CONFIG_CHANGED);
}

static char const* AccessoryManager_GetSpeakerVariantString(IpcSpeakerPackage_t package)
{
    if(package >= IPC_SPEAKER_PACKAGE_BARDEEN_OMNIVO && package <= IPC_SPEAKER_PACKAGE_PROFESSORLOVEYMAXWELL)
    {
        return speakerConfigStrings[package - IPC_FIRST_VALID_SPEAKER_PACKAGE];
    }
    else
    {
        return "Invalid!";
    }
}

static void AccessoryManager_UpdateSpeakerConfig(BOOL persist)
{
    if((initialHoldOff || pairingActive) && !persist)
    {
        // We don't want to thrash this at first
        // and if in pairing we will figure it out after
        // for persist we still gotta do the thing
        LOG(uitask, ROTTEN_LOGLEVEL_VERBOSE, "In acc hold off or pair, skip updating NV/DSP!");
        return;
    }

    currentAccessoryFlags = AccessoryManager_GetAccessoryFlags();
    currentSpeakerConfig = AccessoryManager_GetSpeakerConfig();
    currentSpeakerVariant = AccessoryVariant_GetSpeakerVariant();

    if(persist)
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Update speaker variant: %s Persistence: TRUE", AccessoryManager_GetSpeakerVariantString(currentSpeakerVariant));
    }
    else
    {
        LOG(uitask, ROTTEN_LOGLEVEL_VERBOSE, "Update speaker variant: %s Persistence: FALSE", AccessoryManager_GetSpeakerVariantString(currentSpeakerVariant));
    }

    if(state == ACCESSORY_STATE_DISCONNECTED)
    {
        // we should probably adjust and save accounting for missing since if they become missing
        // and then you switch to wired bass for example we will save off something wrong
    }

    if(persist)
    {
        // Still debating whether this is better suited for external flash and save whole ACC list
        NV_SetExpectedSpeakerConfig(currentSpeakerConfig);
        NV_SetSpeakerAccessories(currentAccessoryFlags);
        NV_CommitSystemParams();

        // Double check so we don't do this too often
        if(currentSpeakerVariant != (IpcSpeakerPackage_t)NV_GetSpeakerPackageVariant())
        {
            NV_SetSpeakerPackageVariant(currentSpeakerVariant);
            NV_CommitMfgParams(); // I hate this why do we use mfg for speaker var
        }

    }

    AccessoryManager_UpdateDSPSpeakerConfig();
}

////////////////////////////////////////////////////////////////////////////////////
/////// SOME FUNCTIONS TO TRANSLATE FROM THE WA TASK TO ACCESSORT MANAGER //////////
////////////////////////////////////////////////////////////////////////////////////
static AccessoryPosition_t AccessoryManager_DarrChannel_2_AccPosition(uint8_t channel)
{
    switch(channel)
    {
        case WA_CHANNEL_BASS_SKIPPER:
        case WA_CHANNEL_BASS_PBB:    
            return ACCESSORY_POSITION_SUB;

        case WA_CHANNEL_SURROUND_LEFT:
            return ACCESSORY_POSITION_LEFT_REAR;

        case WA_CHANNEL_SURROUND_RIGHT:
            return ACCESSORY_POSITION_RIGHT_REAR;

        default:
            debug_assert(0);
    }
    return NULL;
}

static AccessoryType_t AccessoryManager_DarrChannel_2_AccType(uint8_t channel)
{
    switch(channel)
    {
        case WA_CHANNEL_BASS_SKIPPER:
            return ACCESSORY_SKIPPER;
       //For historical reason, the bassbox after Skipper was named LOVEY, but later changed to
       //PBB (Professor Bass Box). They are equivalent
        case WA_CHANNEL_BASS_PBB:
            return ACCESSORY_LOVEY;

        case WA_CHANNEL_SURROUND_LEFT:
        case WA_CHANNEL_SURROUND_RIGHT:
            return ACCESSORY_MAXWELL;

        default:
            debug_assert(0);
    }
    return NULL;
}

static uint32_t AccessoryManager_FindNextOpenListLocation(uint8_t channel)
{
    if ( FALSE /* DR_TODO !SystemBehavior_SupportedAccessory(channel) */ )
    {
        return ACCESORY_INDEX_INVALID;
    }
    if((channel == WA_CHANNEL_BASS_SKIPPER) || (channel == WA_CHANNEL_BASS_PBB))
    {
        if((accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_NONE) ||
           (accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_WIRED /* DR_TODO && SystemBehavior_OverrideWiredAccessory() */) ||
           (accList.accessory[ACC_POS_SUB].status == ACCESSORY_CONNECTION_EXPECTED))
        {
            return ACC_POS_SUB;
        }
        else if(((accList.accessory[ACC_POS_SUB_2].status == ACCESSORY_CONNECTION_NONE) ||
                (accList.accessory[ACC_POS_SUB_2].status == ACCESSORY_CONNECTION_WIRED) || 
                (accList.accessory[ACC_POS_SUB_2].status == ACCESSORY_CONNECTION_EXPECTED)) &&
                (accList.accessory[ACC_POS_SUB].status != ACCESSORY_CONNECTION_WIRED) && // If first is wired we wont ack a second one
                (accList.accessory[ACC_POS_SUB].type == AccessoryManager_DarrChannel_2_AccType(channel)) && // Don't allow second if first is diff type
                (channel != WA_CHANNEL_BASS_PBB)) // No dual PBB
        {
            return ACC_POS_SUB_2;
        }
        else
        {
            return ACCESORY_INDEX_INVALID;
        }
    }

    if(channel == WA_CHANNEL_SURROUND_LEFT || channel == WA_CHANNEL_SURROUND_RIGHT)
    {
        if((accList.accessory[ACC_POS_REAR_1].status == ACCESSORY_CONNECTION_NONE) ||
           (accList.accessory[ACC_POS_REAR_1].status == ACCESSORY_CONNECTION_WIRED) || 
           (accList.accessory[ACC_POS_REAR_1].status == ACCESSORY_CONNECTION_EXPECTED))
        {
            return ACC_POS_REAR_1;
        }
        else if((accList.accessory[ACC_POS_REAR_2].status == ACCESSORY_CONNECTION_NONE)||
                (accList.accessory[ACC_POS_REAR_2].status == ACCESSORY_CONNECTION_WIRED) || 
                (accList.accessory[ACC_POS_REAR_2].status == ACCESSORY_CONNECTION_EXPECTED))
        {
            return ACC_POS_REAR_2;
        }
        else
        {
            return ACCESORY_INDEX_INVALID;
        }
    }

    // unsupported channels
    return ACCESORY_INDEX_INVALID;
}

////////////////////////////////////////////////////////////////////////////////////
//////////////// API FROM THE WA TASK TO ACCESSORT MANAGER /////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void AccessoryManager_UpdateSpeakers_ID(WAPacket_ID_t* packet)
{
    uint32_t i = ACCESORY_INDEX_INVALID;
    if(packet->serialNumberCRC != 0)
    {
        i = AccessoryManager_GetSpeakerIndexFromSerialCRC(packet->serialNumberCRC);
    }
    else if(WA_CHANNEL_IS_SURROUND(packet->channel) &&
            (accList.accessory[ACC_POS_REAR_1].position == accList.accessory[ACC_POS_REAR_2].position) &&
            (accList.accessory[ACC_POS_REAR_1].position != ACCESSORY_POSITION_INVALID))
    {
        // There's no way to correlate this ID with a specific serial number. So, about
        // the only thing we can do is fill in the empty one. Chances are
        // the speakers will be running the same version of code anyway. ¯\_(?)_/¯
        if(accVersions[ACC_POS_REAR_1].darr_ver == 0)
        {
            i = ACC_POS_REAR_1;
        }
        else if(accVersions[ACC_POS_REAR_2].darr_ver == 0)
        {
            i = ACC_POS_REAR_2;
        }
        else
        {
            i = ACCESORY_INDEX_INVALID;
            speakerWatchdogs[ACC_POS_REAR_1] = 0;
            speakerWatchdogs[ACC_POS_REAR_2] = 0;
        }
    }
    else if(WA_CHANNEL_IS_SURROUND(packet->channel) &&
            (accList.accessory[ACC_POS_REAR_1].position == AccessoryManager_GetSpeakerIndexFromChannel(packet->channel)) &&
            (accList.accessory[ACC_POS_REAR_2].position == ACCESSORY_POSITION_INVALID))
    {
        if(accVersions[ACC_POS_REAR_1].darr_ver == 0)
        {
            i = ACC_POS_REAR_1;
        }
        else if(accVersions[ACC_POS_REAR_2].darr_ver == 0)
        {
            i = ACC_POS_REAR_2;
        }
        else
        {
            i = ACCESORY_INDEX_INVALID;
            speakerWatchdogs[ACC_POS_REAR_1] = 0;
            speakerWatchdogs[ACC_POS_REAR_2] = 0;
        }
    }
    else
    {
        i = AccessoryManager_GetSpeakerIndexFromChannel(packet->channel);
    }

    if(i != ACCESORY_INDEX_INVALID)
    {
        speakerWatchdogs[i] = 0;
        if(i != ACC_POS_SUB_2)
        {
            accList.accessory[i].position = AccessoryManager_DarrChannel_2_AccPosition(packet->channel);
        }
        else
        {
            accList.accessory[i].position = ACC_POS_SUB_2;
        }

        // copy version stuff
        char tmpVerBuf[9] = {0}; // used to absorb null from sprintf
        snprintf(tmpVerBuf, sizeof(tmpVerBuf), "%02d.%02d.%02d", packet->lpm_major, packet->lpm_minor, packet->lpm_patch);
        memcpy(&accList.accessory[i].version, tmpVerBuf, SIZE_OF_VERSION); // copy all but the terminator
        
        accVersions[i].darr_ver = packet->darr_ver;
        accVersions[i].lpm_major = packet->lpm_major;
        accVersions[i].lpm_minor = packet->lpm_minor;
        accVersions[i].lpm_patch = packet->lpm_patch;
        accVersions[i].lpm_build = packet->lpm_build;

    }
}

BOOL AccessoryManager_UpdateSpeakers_SN(WAPacket_SerialNum_t* packet)
{
    uint32_t i = AccessoryManager_GetSpeakerIndexFromSerialCRC(WirelessAudioUtilities_CalculateSerialCRC((char*)packet->serialNumber));

    // is it new friend
    if(i == ACCESORY_INDEX_INVALID)
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "New serial received: %s Channel: 0x%02X", packet->serialNumber, packet->channel);
        i = AccessoryManager_FindNextOpenListLocation(packet->channel);
    }

    // Did we find a vaild index, if not log and bail
    if(i != ACCESORY_INDEX_INVALID)
    {
        // has home so lets copy in now
        if(i != ACC_POS_SUB_2)
        {
            accList.accessory[i].position = AccessoryManager_DarrChannel_2_AccPosition(packet->channel);
        }
        else
        {
            accList.accessory[i].position = ACC_POS_SUB_2;
        }
        
        memcpy(accList.accessory[i].sn, packet->serialNumber, WA_SERIAL_NO_LENGTH);
        accList.accessory[i].type = AccessoryManager_DarrChannel_2_AccType(packet->channel);

        // Special case where speaker is wireless then wired (we will still be paired)
        if(IsWiredBassConnected() && ((accList.accessory[i].type == ACCESSORY_SKIPPER) || (accList.accessory[i].type == ACCESSORY_LOVEY)))
        {
            accList.accessory[i].status = ACCESSORY_CONNECTION_BOTH;
        }
        else
        {
            accList.accessory[i].status = ACCESSORY_CONNECTION_WIRELESS;
        }

        // Reset the WD
        speakerWatchdogs[i] = 0;

        // This is safe to call in pairing and initial hold off
        AccessoryManager_UpdateSpeakerConfig(FALSE);
    }
    else
    {
        // Failed to get open slot so exit
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Failed to find accessory slot for MU: Serial = %s Channel - 0x%02X", packet->serialNumber, packet->channel);
    }

    if(initialHoldOff && AccessoryManager_GetSpeakerConfig() == NV_GetExpectedSpeakerConfig())
    {
        // if we found everyone lets not wait
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Found all speakers so initial hold of is done!");
        timerStop(initialHoldOffTimer, 0);
        AccessoryManager_InitialHoldOffDone();
    }
    return (i < ACCESSORY_POSITION_NUM_OF_INTERNAL);
}

void AccessoryManager_UpdateSpeakers_TL(WAPacket_Telemetry_t* t)
{
    uint32_t i = ACCESORY_INDEX_INVALID;

    if(t->serialNumberCRC == 0)
    {
        // If sub it's easy (unless someone wants double skippers then idk)
        if((WA_CHANNEL_IS_BASS(t->channel)))
        {
            i = ACC_POS_SUB;
            // Nothing we can do but clear both
            speakerWatchdogs[ACC_POS_SUB_2] = 0;
        }
        // If the rears are configured to the same channel, there's no way to distinguish
        // them. We'll just alternate slots.
        else if(IS_ACCESSORY_MISCONFIG(accList.accessory[ACC_POS_REAR_1], accList.accessory[ACC_POS_REAR_2]))
        {
            static AccessoryIndex lastMisconfigIndex = ACC_POS_REAR_2;
            lastMisconfigIndex = (lastMisconfigIndex == ACC_POS_REAR_1) ? ACC_POS_REAR_2 : ACC_POS_REAR_1;
            i = lastMisconfigIndex;
        }
        // normal opperation so we can use channel
        // if a channel is changed to wrong here we will let watchdog signal the error
        else
        {
            i = AccessoryManager_GetSpeakerIndexFromChannel(t->channel);
        }
    }
    else
    {
        // If not legacy MU we can be smart... rrr
        i = AccessoryManager_GetSpeakerIndexFromSerialCRC(t->serialNumberCRC);
    }

    if(i != ACCESORY_INDEX_INVALID)
    {
        accList.accessory[i].status = ACCESSORY_CONNECTION_WIRELESS;
        
        // Reset watchdog counter
        speakerWatchdogs[i] = 0;
        LOG(uitask, ROTTEN_LOGLEVEL_INSANE, "Telemetry: af %d, dc %d, ch %d, temp %d, ss %d",
            t->ampFault,
            t->dcOffset,
            t->channel,
            t->ampTemp,
            t->serialNumberCRC);

        if (t->ampFault || t->dcOffset)
        {
            LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Channel: %d Amp fault: %d or offset %d",t->channel, t->ampFault, t->dcOffset);
            // send a message to DSP or to UI?
        }

        // We overwrite this just incase it changed (will be used to rectify two of same gender)
        if(i != ACC_POS_SUB_2)
        {
            accList.accessory[i].position = AccessoryManager_DarrChannel_2_AccPosition(t->channel);
        }
        else
        {
            accList.accessory[i].position = ACC_POS_SUB_2;
        }

        // Feed dsp the temp data
        IpcThermistorData_t temperature = {.tempDegC = t->ampTemp};
        temperature.thermLocation = AccessoryManager_GetThermistorLocationFromChannel((WA_ChannelID_t)t->channel);

        DeviceModel_DSP_Temperature_Set(&temperature);
    }
    else
    {
        LOG(uitask, ROTTEN_LOGLEVEL_VERBOSE, "Unknown telemetry: channel - 0x%02X, SNS - %d",
                                            t->channel,
                                            t->serialNumberCRC);
    }
}

uint32_t AccessoryManager_NumberOfTypeConnected(AccessoryType_t type)
{
    uint32_t retVal = 0;
    for(uint32_t i = ACCESSORY_POSITION_START; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        if(accList.accessory[i].type == type && IS_ACCESSORY_WIRELESS(accList.accessory[i]))
        {
            retVal++;
        }
    }
    return retVal;
}

uint32_t AccessoryManager_NumberOfWirelessSpeakersConnected(void)
{
    uint32_t retVal = 0;
    for(uint32_t i = ACCESSORY_POSITION_START; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        if(IS_ACCESSORY_WIRELESS(accList.accessory[i]))
        {
            retVal++;
        }
    }

    return retVal;
}

uint32_t AccessoryManager_NumberWithStatusConnected(AccessoryConnectionStatus_t status)
{
    uint32_t retVal = 0;
    for(uint32_t i = ACCESSORY_POSITION_START; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
    {
        if(accList.accessory[i].status == status)
        {
            retVal++;
        }
    }
    return retVal;
}

BOOL AccessoryManager_IsSerialNew(WAPacket_ID_t* packet)
{
    // if old dumb MU
    if(packet->serialNumberCRC == 0)
    {
        return (AccessoryManager_GetSpeakerIndexFromChannel(packet->channel) == ACCESORY_INDEX_INVALID);
    }
    else
    {
        return (AccessoryManager_GetSpeakerIndexFromSerialCRC(packet->serialNumberCRC) == ACCESORY_INDEX_INVALID);
    }
}

BOOL AccessoryManager_UpdateSpeakers_ID_LegacyPairing(WAPacket_ID_t* packet)
{
    uint32_t i = AccessoryManager_GetSpeakerIndexFromChannel(packet->channel);

    // Some special logic to try and determine if the surround speakers are configured
    // for the same channel. 
    if(WA_CHANNEL_IS_SURROUND(packet->channel))
    {
        if((i != ACCESORY_INDEX_INVALID) &&
           (accVersions[i].darr_ver != 0) &&
           !(IS_LEGACY_SURROUND_PAIRING(accList.accessory[ACC_POS_REAR_1]) && IS_LEGACY_SURROUND_PAIRING(accList.accessory[ACC_POS_REAR_2])))
        {
            LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Already have ID, misconfig assumed");
            i = ACCESORY_INDEX_INVALID;
        }
        else if((i == ACCESORY_INDEX_INVALID) &&
                IS_ACCESSORY_MISCONFIG(accList.accessory[ACC_POS_REAR_1], accList.accessory[ACC_POS_REAR_2]) &&
                (accList.accessory[ACC_POS_REAR_1].position != AccessoryManager_DarrChannel_2_AccPosition(packet->channel)))
        {
            LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Misconfig cleared. Add channel 0x%02X", packet->channel);
            i = (accList.accessory[ACC_POS_REAR_1].sn[0] == '\0') ? ACC_POS_REAR_1 : ACC_POS_REAR_2;
        }
    }

    // is it new friend
    if(i == ACCESORY_INDEX_INVALID)
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "New legacy ID channel 0x%02X", packet->channel);
        i = AccessoryManager_FindNextOpenListLocation(packet->channel);
    }

    if(i != ACCESORY_INDEX_INVALID)
    {
        speakerWatchdogs[i] = 0;
        if(i != ACC_POS_SUB_2)
        {
            accList.accessory[i].position = AccessoryManager_DarrChannel_2_AccPosition(packet->channel);
        }
        else
        {
            accList.accessory[i].position = ACC_POS_SUB_2;
        }
        accList.accessory[i].type = AccessoryManager_DarrChannel_2_AccType(packet->channel);
        
        // Special case where speaker is wireless then wired (we will still be paired)
        if(IsWiredBassConnected() && \
            ((accList.accessory[i].type == ACCESSORY_SKIPPER) || (accList.accessory[i].type == ACCESSORY_LOVEY)))
        {
            accList.accessory[i].status = ACCESSORY_CONNECTION_BOTH;
        }
        else
        {
            accList.accessory[i].status = ACCESSORY_CONNECTION_LEGACY_PAIRING;
        }

        // copy version stuff
        char tmpVerBuf[9] = {0}; // used to absorb null from sprintf
        snprintf(tmpVerBuf, sizeof(tmpVerBuf), "%02d.%02d.%02d", packet->lpm_major, packet->lpm_minor, packet->lpm_patch);
        memcpy(&accList.accessory[i].version, tmpVerBuf, SIZE_OF_VERSION); // copy all but the terminator
        
        accVersions[i].darr_ver = packet->darr_ver;
        accVersions[i].lpm_major = packet->lpm_major;
        accVersions[i].lpm_minor = packet->lpm_minor;
        accVersions[i].lpm_patch = packet->lpm_patch;
        accVersions[i].lpm_build = packet->lpm_build;
    }
    else
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Failed to find accessory slot for legacy MU: Channel - 0x%02X", packet->channel);
    }
    
    return (i < ACCESSORY_POSITION_NUM_OF_INTERNAL);
}

// Returns true if the serial number should be ack'd
BOOL AccessoryManager_UpdateSpeakers_SN_LegacyPairing(WAPacket_SerialNum_t* packet)
{
    uint_fast8_t i = AccessoryManager_GetSpeakerIndexFromSerialCRC(WirelessAudioUtilities_CalculateSerialCRC((char*)packet->serialNumber));

    // is it new friend
    if(i == ACCESORY_INDEX_INVALID)
    {
        i = ACCESSORY_POSITION_START;
        for(; i < ACCESSORY_POSITION_NUM_OF_INTERNAL; i++)
        {
            if((accList.accessory[i].position == AccessoryManager_DarrChannel_2_AccPosition(packet->channel)) &&
               (accList.accessory[i].status == ACCESSORY_CONNECTION_LEGACY_PAIRING))
            {
                break;
            }
        }
    }

    if(i < ACCESSORY_POSITION_NUM_OF_INTERNAL)
    {
        memcpy(accList.accessory[i].sn, packet->serialNumber, WA_SERIAL_NO_LENGTH);
        AccessoryManager_UpdateSpeakers_SN(packet);
    }

    return (i < ACCESSORY_POSITION_NUM_OF_INTERNAL);
}

void AccessoryManager_ListTransfer_Start(Ipc_Device_t device)
{
    if(accListTransferID != 0)
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Failed to bulk transfer list, already in progress with id = %d!", accListTransferID);
        return;
    }
    else if (!IpcIsValidDestination(device))
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Failed to bulk transfer list, invalid destination = %d!", device);
        return;
    }

    accListTransferID = IPCBulk_BulkTransfer(device, (uint8_t*)&accList, sizeof(IpcAccessoryList_t),
                               BULK_TYPE_ACCESSORY_LIST, NULL, "AccList", &(GetManagedTaskPointer("UITask")->Queue));

    if(accListTransferID != 0)
    {
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Sending full accessory list to %s!", IpcTaskNames[device]);
    }
}

void AccessoryManager_ListTransfer_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case IPCROUTER_MESSAGE_ID_BulkTransferError:
            LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Failed to send full accessory list to %d!", msg->params[0]);
        case IPCROUTER_MESSAGE_ID_BulkTransferComplete:
            AccessoryManager_ListTransfer_HandleBulkTransferComplete(msg->params[0]);
            break;
    }
}

static void AccessoryManager_ListTransfer_HandleBulkTransferComplete(uint16_t transferID)
{
    if (transferID == accListTransferID)
    {
        IPCBulk_DeactivateSocket(accListTransferID);
        accListTransferID = 0;
    }
}

void AccessoryManagerSetUpdate(BOOL updateState)
{
    isUpdatePending = updateState;
}

BOOL AccessoryManagerGetUpdate(void)
{
    return isUpdatePending;
}

#if defined(BARDEEN)
void AccessoryManager_MUReportedVer(char* ver)
{
    if (ver)
    {
      const char* v2 = UpdateModuleSpeakerPrep_GetCUSpeakerBlobVer();
	  
      if(v2 && (VERSION_COMPARE_SAME_VERSION != VersionCompareVersions((const char *)ver, v2)))
      {
        AccessoryManagerSetUpdate(TRUE);
        LOG(uitask, ROTTEN_LOGLEVEL_NORMAL, "Speaker update required");
      }

      BufferManagerFreeBuffer((void *)ver);
    }
}
#endif
