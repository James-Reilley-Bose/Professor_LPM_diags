/*
  File  : AccessortManager.h
  Title :
  Author  : dr1005920
  Created : 10/06/2016
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:   API for handling wireless accessorts

===============================================================================
*/
#include "WirelessAudioData.h"
#include "nv_mfg.h"

#ifndef ACCESSORT_MANAGER_H
#define ACCESSORT_MANAGER_H

#define ACCESSORY_HOLD_OFF_TIMEOUT TIMER_SEC_TO_TICKS(10)
#define ACCESSORY_WD_TIMEOUT TIMER_SEC_TO_TICKS(2)

#define ACCESSORY_MAX_PINGS (30)

#define IS_ACCESSORY_CONNECTED(x) (((x).status == ACCESSORY_CONNECTION_WIRED) || \
                                   ((x).status == ACCESSORY_CONNECTION_WIRELESS) || \
                                   ((x).status == ACCESSORY_CONNECTION_BOTH) || \
                                   ((x).status == ACCESSORY_CONNECTION_EXPECTED))

#define IS_ACCESSORY_WIRELESS(x) (((x).status == ACCESSORY_CONNECTION_WIRELESS) || \
                                  ((x).status == ACCESSORY_CONNECTION_BOTH))

#define IS_ACCESSORY_MISCONFIG(x,y) (((x).position == (y).position) &&  \
                                     (((x).position != ACCESSORY_POSITION_INVALID) || \
                                      (((y).position != ACCESSORY_POSITION_INVALID))))

#define IS_LEGACY_SURROUND_PAIRING(x) (((x).status == ACCESSORY_CONNECTION_WIRELESS) || \
                                       ((x).status == ACCESSORY_CONNECTION_LEGACY_PAIRING))

typedef enum
{
  ACCESSORY_TONE_IDLE          = 0,
  ACCESSORY_TONE_START         = 1,
  ACCESSORY_PLAY_SUB_TONE      = 2,
  ACCESSORY_PLAY_LEFT_TONE     = 3,
  ACCESSORY_PLAY_RIGHT_TONE    = 4,
  ACCESSORY_TONE_INVALID       = 5,
  ACCESSORY_TONE_STATE_NUM_OF  = ACCESSORY_TONE_INVALID
}PairingToneState_t;

typedef enum
{
    UI_SPEAKER_NONE =                       0,
    UI_SPEAKER_LEFT =                       (1<<0),
    UI_SPEAKER_RIGHT =                      (1<<1),
    UI_SPEAKER_WIRED_BASS =                 (1<<2),
    UI_SPEAKER_WIRELESS_BASS =              (1<<3),
    UI_SPEAKER_SECOND_BASS =                (1<<4),
    UI_SPEAKER_SUB_IS_PBB =                 (1<<5),
    UI_SPEAKERS_LEFT_RIGHT_WIRED_BASS =     (UI_SPEAKER_LEFT|UI_SPEAKER_RIGHT|UI_SPEAKER_WIRED_BASS),
    UI_SPEAKERS_LEFT_RIGHT_WIRELESS_BASS =  (UI_SPEAKER_LEFT|UI_SPEAKER_RIGHT|UI_SPEAKER_WIRELESS_BASS),
    UI_SPEAKERS_LEFT_RIGHT =                (UI_SPEAKER_LEFT|UI_SPEAKER_RIGHT),
    UI_SPEAKERS_ALL =                       (UI_SPEAKER_LEFT|UI_SPEAKER_RIGHT|UI_SPEAKER_WIRED_BASS|UI_SPEAKER_WIRELESS_BASS),
    UI_SPEAKERS_BOTH_BASS =                 (UI_SPEAKER_WIRED_BASS|UI_SPEAKER_WIRELESS_BASS),
    UI_SPEAKER_TWO_BASS =                   (UI_SPEAKER_WIRELESS_BASS|UI_SPEAKER_SECOND_BASS),
    UI_SPEAKER_TWO_BASS_REARS =             (UI_SPEAKER_LEFT|UI_SPEAKER_RIGHT|UI_SPEAKER_WIRELESS_BASS|UI_SPEAKER_SECOND_BASS),
} SpeakerConfig_t;


typedef enum
{
    ACCESSORY_STATE_INVALID,
    ACCESSORY_STATE_CONNECTED,
    ACCESSORY_STATE_DISCONNECTED,
    ACCESSORY_STATE_PAIRING,
    ACCESSORY_STATE_MISCONFIG,
} AccesoryManagerUI_State_t;

typedef enum
{
    ACC_POS_SUB =        0,
    ACC_POS_REAR_1 =     1,
    ACC_POS_REAR_2 =     2,
    ACC_POS_SUB_2 =      3,
    ACCESORY_INDEX_INVALID =  0xFF,
} AccessoryIndex;

typedef struct
{
    uint8_t darr_ver;
    uint8_t lpm_major;
    uint8_t lpm_minor;
    uint8_t lpm_patch;
    uint16_t lpm_build;
} AccessoryVersion;

#define SIZE_OF_VERSION   (sizeof(((AccessoryDescription_t *)0)->version))

// API from UI messages
void AccessoryManager_Init(void);
void AccessoryManager_On(void);
void AccessoryManager_Off(void);
void AccessoryManager_HandleLlwPairing(BOOL enabled);
void AccessoryManager_HandleLlwInitialized(void);
void AccessoryManager_InitialHoldOffDone(void);
void AccessoryManager_HandleWatchdog(void);


// API to get background update 
BOOL AccessoryManager_IsUpdatePending();

// Api for UI state
AccesoryManagerUI_State_t AccessoryManager_GetUI_State();

// Api for current speaker setup to be reported
IpcAccessoryFlags_t AccessoryManager_GetCurrentAccessoryFlags();
SpeakerConfig_t AccessoryManager_GetCurrentSpeakerConfig();
IpcSpeakerPackage_t AccessoryManager_GetCurrentSpeakerVariant();

// version stuffs
void AccessoryManager_GetSpeakerLPMVersion(AccessoryIndex index, char* buffer, uint32_t len);
void AccessoryManager_GetSpeakerWAVersion(AccessoryIndex index, char* buffer, uint32_t len);
void AccessoryManagerSetUpdate(BOOL updateState);
BOOL AccessoryManagerGetUpdate(void);
void AccessoryManager_MUReportedVer(char* ver);

// API to device models (gabbo controls and asoc)
IpcAccessoryList_t* AccessoryManager_GetAccList(void);
AccessoryVersion* AccessoryManager_GetAccVersion(void);
void AccessoryManager_SetSpeakerActiveInformation(IpcAccessoryList_t* activeStates);

// API from misc task to handle wired bass update
void AccessoryManager_HandleWiredSpeakerUpdate(AccessoryDescription_t* speaker);

// API to WA
void AccessoryManager_UpdateSpeakers_ID(WAPacket_ID_t* packet);
BOOL AccessoryManager_UpdateSpeakers_SN(WAPacket_SerialNum_t* packet);
void AccessoryManager_UpdateSpeakers_TL(WAPacket_Telemetry_t* packet);
BOOL AccessoryManager_IsSerialNew(WAPacket_ID_t* packet);
uint32_t AccessoryManager_NumberOfTypeConnected(AccessoryType_t type);
uint32_t AccessoryManager_NumberOfWirelessSpeakersConnected(void);
uint32_t AccessoryManager_NumberWithStatusConnected(AccessoryConnectionStatus_t status);
BOOL AccessoryManager_UpdateSpeakers_ID_LegacyPairing(WAPacket_ID_t* packet);
BOOL AccessoryManager_UpdateSpeakers_SN_LegacyPairing(WAPacket_SerialNum_t* packet);

// Bulk Transfer of AccList
void AccessoryManager_ListTransfer_Start(Ipc_Device_t device);
void AccessoryManager_ListTransfer_HandleMessage(GENERIC_MSG_t* msg);

#endif // ACCESSORT_MANAGER_H
