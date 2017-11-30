/**
  ******************************************************************************
  * @file    ConsoleKeys.h
  * @author  cc1014948
  * @version V0.0.1
  * @date    13-Nov-2015
  * @brief   Console keys driver
  *          This file provides firmware functions to manage the following
  *          functionalities of the console key pad:
  *           + Detects the key press attached to the GPIO
  *          Adopted from Marconi
 @verbatim
 ===============================================================================
 **/
#ifndef _CONSOLE_KEYS_H_
#define _CONSOLE_KEYS_H_

#include "project.h"
#include "TaskManager.h"
#include "QueueManager.h"
#include "RivieraLPM_KeyValues.h"
#include "KeyHandler.h"

/**********************************************************
* External interface functions
**********************************************************/
void CK_Init(void);
void CK_KeyScanTimerExpired (void);
void CK_DebounceTimerFired(void);
void CK_Module (void* pvParamaters);
void CK_HandleMessage(GENERIC_MSG_t* msg);
void CK_VulcanTimer(void);

#if 0
void CK_CheckBootVulcan(const uint32_t mask);
void CK_ReadyToReceiveNVReset(void);
void CK_VulcanTimer(void);
void CK_VulcanMainBoardBootTimer(void);
void CK_StartMuteTimer (void);
void CK_StopMuteTimer (void);
void CK_MuteReleaseTimerFired(void);
void CK_MuteTimerFired(void);
void CK_SimulateAllOffKey (void);
BOOL CK_RunUpdateWhen7200Boots(void);
/**** for tap ****/
extern uint8_t g_LastConsoleKey;
uint32_t  CK_GetKeysHeld(void);
void CK_EnableFinalBoothTest(BOOL enable);
#endif



/**********************************************************
* Definitions
**********************************************************/
/**** Console Key pad ****/
/**** individual console key lines ****/
#define KEYPAD_BOSE_BUTTON_1            0x0001
#define KEYPAD_BOSE_BUTTON_2            0x0002
#define KEYPAD_BOSE_BUTTON_3            0x0004
#define KEYPAD_BOSE_BUTTON_4            0x0008

/**** Timer interval ****/
#define CK_CONSOLE_SCAN_RATE_MSEC          50
#define CK_DEBOUNCE_POLL_INTERVAL_MSEC     30
#define CK_VULCAN_CHECK_FOR_BOOT_MS        1000
#define CK_KEY_REPEAT_INTERVAL_MSEC        100 /* 100 ms, see Review Request 6429 */

/**** Multiple key presses ****/
#define BOSE_SPECIAL_FEATURE_COMBO_1  (KEYPAD_BOSE_BUTTON_1 | KEYPAD_BOSE_BUTTON_2)
#define BOSE_SPECIAL_FEATURE_COMBO_2  (KEYPAD_BOSE_BUTTON_3 | KEYPAD_BOSE_BUTTON_4)

// max number of console keys that can be pressed at once
#define NUM_CONSOLE_KEYS        4
#define MAX_CONSOLE_KEYS_HELD   NUM_CONSOLE_KEYS
#define VULCAN_VALID_ANY_TIME   0xffff

/**********************************************************
* Structures
**********************************************************/
/**** table to get key values from the right bits     ****/
#pragma pack(1)
typedef struct
{
    uint16_t KeyValue;
    uint32_t KeyMask;
} ConsoleKeyButton_t;
#pragma pack()

static const ConsoleKeyButton_t CKKeyVals[NUM_CONSOLE_KEYS] =
{
    {BOSE_VOLUME_DOWN, KEYPAD_BOSE_BUTTON_3},
    {BOSE_VOLUME_UP, KEYPAD_BOSE_BUTTON_4},
    {BOSE_ON_OFF, KEYPAD_BOSE_BUTTON_2},
    {BOSE_SETUP, KEYPAD_BOSE_BUTTON_1}
};

typedef enum
{
    POST_BOOT_ACTION_Nothing,
    POST_BOOT_ACTION_BoseSpecialFeature1
} POST_BOOT_ACTION_t;


// we need to handle certain console key combinations in a special way
#pragma pack(1)
typedef struct
{
    uint16_t  KeyMask; // mask of the keys held for the combo
    uint16_t  HoldTime; // hold time before execute special funciton
    system_time_t  AfterbootWindow;   //window of time after boot where combo is allowed
    uint16_t KeyValue;
} ConsoleKeyCombo_t;
#pragma pack()

typedef enum
{
    BOSE_SPECIAL_FEATURE_2_KEYPRESSES,
    BOSE_SPECIAL_FEATURE_3_KEYPRESSES,
    NUM_CONSOLE_VULCAN_KEYPRESSES
} CONSOLE_VULCAN_KEYPRESSES;


static const ConsoleKeyCombo_t s_CKVulcanPresses[NUM_CONSOLE_VULCAN_KEYPRESSES] =
{
    //     KeyMask,                        HoldTime,               AfterbootWindow,            KeyValue
    {BOSE_SPECIAL_FEATURE_COMBO_1,  LONG_PRESS_AND_HOLD_START,       200,                BOSE_SPECIAL_FEATURE_2}, // BOSE SPECIAL FEATURE 2
    {BOSE_SPECIAL_FEATURE_COMBO_2,  LONG_PRESS_AND_HOLD_START,       200,                BOSE_SPECIAL_FEATURE_3}, // BOSE SPECIAL FEATURE 3
};

BOOL CK_IsVulcanPressActive(void);
#endif //_CONSOLE_KEYS_H_
