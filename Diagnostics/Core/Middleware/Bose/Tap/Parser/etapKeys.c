#include "etap.h"
#include "etapKeys.h"
#include "KeyHandler.h"
#include "KeyHandlerTask.h"
#include "EventDefinitions.h"
#include "TapParserAPI.h"
#include "ir.h"

SCRIBE_DECL(keys);

CREATE_KEY_PRODUCER(tap1, KEY_PRODUCER_TAP, TAP_HOLD_TIME, TAP_RELEASE_TIME, TAP_REPEAT_TIME, TIMER_ID_TAP1Release, TIMER_ID_TAP1Repeat);
CREATE_KEY_PRODUCER(tap2, KEY_PRODUCER_TAP, TAP_HOLD_TIME, TAP_RELEASE_TIME, TAP_REPEAT_TIME, TIMER_ID_TAP2Release, TIMER_ID_TAP2Repeat);

/* Private functions */
static void TapKeyTimerCallback (xTimerHandle pxTimer);

/* Globals */
static TimerHandle_t TapKeyTimer = NULL; // Timer to signal repeats
static uint32_t HoldTime = 0; // The TAP user's requested press and hold time
static uint32_t PressTime = 0; // The system uptime when the 'kp' command was handled (for determing when to release)
static KEY_VALUE LastKey = BOSE_INVALID_KEY; // The LastKey received (for press and holds)
static KEY_PRODUCER_ATTRIBUTES* LastProducer = &tap1;
static SemaphoreHandle_t KeyListSemaphore = NULL; // To hold back the prompt until the keylist is printed.

TAPCommand(TAP_IR_Command)
{
    if (CommandLine->numArgs == 0)
    {
        if (IR_GetAllowIr())
        {
            TAP_PrintString("\nIR Enabled\n");
        }
        else
        {
            TAP_PrintString("\nIR Disabled\n");
        }
        return;
    }
    else if (CommandLine->numArgs == 1)
    {
        if ( !strcmp(CommandLine->args[0], "1"))
        {
            IR_SetAllowIr(TRUE);
            TAP_PrintString("\nIR Enabled\n");
        }
        else if ( !strcmp(CommandLine->args[0], "0") )
        {
            IR_SetAllowIr(FALSE);
            TAP_PrintString("\nIR Disabled\n");
        }
        else
        {
            TAP_PrintString(TAP_IR_HELP_TEXT);
        }
    }
    else
    {
        TAP_PrintString(TAP_IR_HELP_TEXT);
    }
}

/*
 * @func TAP_KeyPress
 *
 * @brief Enables key presses through TAP.
 */
TAPCommand(TAP_KeyPress)
{
    BOOL valid = FALSE; // valid flag for parsing various arguments

    /* Set the globals to some defaults... */
    HoldTime = 0;
    PressTime = GET_SYSTEM_UPTIME_MS();
    LastKey = BOSE_INVALID_KEY;
    LastProducer = &tap1;

    /* Lets go ahead and stop anything already being held... */
    if (pdFAIL == xTimerStop(TapKeyTimer, TIMER_MSEC_TO_TICKS(10)))
    {
        LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to stop TapKeyTimer in %s", __func__);
    }

    switch (CommandLine->numArgs)
    {
        case 3: // they've specified producer 1 or 2
            {
                uint8_t producerNum = TAP_DecimalArgToInt(CommandLine, 2, &valid);
                if (!valid || (producerNum < 1) || (producerNum > 2))
                {
                    TAP_PrintString("Invalid producer number.");
                    break;
                }
                else if (producerNum == 2)
                {
                    LastProducer = &tap2;
                }
            }
        // intentional fall-through
        case 2: // they've specified a hold time
            {
                valid = FALSE;
                HoldTime = TAP_DecimalArgToInt(CommandLine, 1, &valid);
                if (!valid)
                {
                    TAP_PrintString("Invalid hold time.");
                    break;
                }

                if (HoldTime > TAP_KEYS_MAX_HOLD_TIME) // an arbitrarily chosen max hold time...
                {
                    HoldTime = TAP_KEYS_MAX_HOLD_TIME;
                }
            }
        // intentional fall-through
        case 1: // The key - check if it's valid and send!
            {
                BOOL KeySent = FALSE;
                valid = FALSE;

                KEY_VALUE key = (KEY_VALUE) TAP_HexArgToInt(CommandLine, 0, &valid);

                if (valid) // they entered a hex value
                {
                    HandleKey(key, LastProducer);
                    KeySent = TRUE;
                    LastKey = key;
                }
                else // let's check if they entered the string name
                {
                    uint16_t i;
                    for (i = 0; i < NUM_BOSE_KEYS; i++)
                    {
                        if (strncasecmp(Key_Info[i].name, CommandLine->args[0], strlen(CommandLine->args[0])) == 0)
                        {
                            HandleKey((KEY_VALUE) i, LastProducer);
                            KeySent = TRUE;
                            LastKey = (KEY_VALUE) i;
                            break;
                        }
                    }

                    if (i >= NUM_BOSE_KEYS) // didn't find it
                    {
                        TAP_PrintString("Invalid key.");
                    }
                }

                if (KeySent && (HoldTime > 0)) // set the timer if they've request a hold
                {
                    if (xTimerReset(TapKeyTimer, 0) != pdPASS)
                    {
                        LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to reset TapKeyTimer.");
                    }
                }
            }
            break;
        default:
            {
                TAP_PrintString(KEY_PRESS_HELP_TEXT);
            }
            break;
    }
}

/*
 * @func TAP_KeyList
 *
 * @brief Prints the last eight pressed keys to TAP.
 */
TAPCommand(TAP_KeyList)
{
    switch (CommandLine->numArgs)
    {
        case 0:
            {
                KeyHandlerPostMsg(KEYHANDLER_MESSAGE_ID_PrintKeyList, NULL, NULL);

                uint32_t start = GET_INSTRUMENT_TIME_NOW();
                xSemaphoreTake(KeyListSemaphore, portMAX_DELAY);

                // If it takes less than 3 msec to Take the semaphore, we got out of sync
                // If we got out of sync, perform another Take
                if (MEASURE_TIME_SINCE(start) < 3)
                {
                    xSemaphoreTake(KeyListSemaphore, 0);
                }
            }
            break;
        case 1:
            {
                if (strcasecmp(CommandLine->args[0], "clear") == 0)
                {
                    KeyHandlerPostMsg(KEYHANDLER_MESSAGE_ID_ResetKeyList, NULL, NULL);
                    xSemaphoreTake(KeyListSemaphore, portMAX_DELAY);
                }
                else
                {
                    TAP_PrintString(KEY_LIST_HELP_TEXT);
                }
            }
            break;
        default:
            {
                TAP_PrintString(KEY_LIST_HELP_TEXT);
            }
            break;
    }
}

/*
 * @func TAP_KeyInfo
 *
 * @brief Prints all keys and metadata. Or, searches and prints keys with specified substring.
 */
TAPCommand(TAP_KeyInfo)
{
    uint16_t i = 0;
    BOOL CheckForSubstring = FALSE;
    uint16_t NumCommandsFound = 0;

    if (CommandLine->numArgs > 0)
    {
        CheckForSubstring = TRUE;
    }

    while (i < NUM_BOSE_KEYS)
    {
        if (Key_Info[i].group != KEY_GROUP_INVALID)
        {
            if (!CheckForSubstring) // print the whole list
            {
                TAP_Printf("\r\n\t0x%02x: %s, %s", i, Key_Info[i].name, KeyGroupNames[Key_Info[i].group]);
            }
            else if (strstr(Key_Info[i].name, CommandLine->args[0]) != 0) // search for command
            {
                TAP_Printf("\r\n\t0x%02x: %s, %s", i, Key_Info[i].name, KeyGroupNames[Key_Info[i].group]);
                NumCommandsFound++;
            }
        }
        i++;
    }

    if (CheckForSubstring)
    {
        TAP_Printf("\r\n\n\tFound %d matching commands.\r\n", NumCommandsFound);
    }
    else
    {
        TAP_PrintString("\r\n");
    }
}

#if 0 // TODO - dj1005472, do we want to pull this back in?
TAPCommand(TAP_KeyHook)
{
    if (CommandLine->numArgs < 1)
    {
        TAP_PrintString(KEY_HOOK_HELP_TEXT);
        return;
    }

    KH_HOOK_MAP_ID mapId = KH_HOOK_MAP_ECHO;

    // Parse subcommand (echo/ignore)
    if (TAP_ToLowerCase(*CommandLine->args[0]) == 'e')
    {
        mapId = KH_HOOK_MAP_ECHO;
    }
    else if (TAP_ToLowerCase(*CommandLine->args[0]) == 'i')
    {
        mapId = KH_HOOK_MAP_IGNORE;
    }
    else
    {
        TAP_PrintString(KEY_HOOK_HELP_TEXT);
        return;
    }

    // No args after subcommand, dump keymask
    if (CommandLine->numArgs == 1)
    {
        uint32_t keyMask[KH_MASK_WORDS];
        UI_GetKeyMask(mapId, keyMask);
        for (uint8_t i = 0; i < KH_MASK_WORDS; i++)
        {
            TAP_Printf("%08x ", keyMask[i]);
        }
        TAP_Printf("\n\r");
        return;
    }

    // The !,X case - stop echoing/ignoring a specific key
    if (CommandLine->numArgs == 3)
    {
        if (*CommandLine->args[1] != '!')
        {
            TAP_PrintString(KEY_HOOK_HELP_TEXT);
            return;
        }

        BOOL valid = FALSE;
        KEY_VALUE key = (KEY_VALUE)(TAP_HexArgToInt(CommandLine, 2, &valid) & BOSE_INVALID_KEY);
        if (!valid)
        {
            TAP_PrintString(KEY_HOOK_HELP_TEXT);
            return;
        }
        UI_ClearKeyHook(mapId, key);
        TAP_PrintString(TAP_OKText);
        return;
    }

    // All
    if (TAP_ToLowerCase(*CommandLine->args[1]) == 'a')
    {
        UI_HookMapSetAll(mapId);
        TAP_PrintString(TAP_OKText);
        return;
    }

    // None
    if (TAP_ToLowerCase(*CommandLine->args[1]) == 'n')
    {
        UI_HookMapClearAll(mapId);
        TAP_PrintString(TAP_OKText);
        return;
    }

    // echo/ignore a specific key
    BOOL valid = FALSE;
    KEY_VALUE key = (KEY_VALUE)(TAP_HexArgToInt(CommandLine, 1, &valid) & BOSE_INVALID_KEY);
    if (!valid)
    {
        TAP_PrintString(KEY_HOOK_HELP_TEXT);
        return;
    }
    UI_SetKeyHook(mapId, key);
    TAP_PrintString(TAP_OKText);
}
#endif

/*
 * @func InitializeTAPKeys
 *
 * @brief Initialization routine to set up the timer that handles press and holds and creates the
 *        mutex used to protect access to global held-key info.
 *
 * @param n/a
 *
 * @return n/a
 */
void InitializeTAPKeys (void)
{
    TapKeyTimer = xTimerCreate (
                      "TapKeyTimer",                  // Timer name
                      TIMER_MSEC_TO_TICKS(110),       // 110ms is the NEC repeat interval
                      pdFALSE,                         // Recurring
                      (void*) TIMER_ID_TAPKeyTimer,   // Timer ID
                      TapKeyTimerCallback             // Callback
                  );

    KeyListSemaphore = xSemaphoreCreateBinary();
    debug_assert(KeyListSemaphore);

    // init key producers
    tap1_init();
    tap2_init();
}

/*
 * @func TapKeyHandlePressAndHold
 *
 * @brief Sends a key press (repeat) or stops the timer if the hold time has expired.
 *
 * @param n/a
 *
 * @return n/a
 */
void TapKeyHandlePressAndHold (void)
{
    uint32 system_uptime = GET_SYSTEM_UPTIME_MS();

    if ((system_uptime - PressTime) > HoldTime) // do we need to stop the timer?
    {
        if (pdFAIL == xTimerStop(TapKeyTimer, TIMER_MSEC_TO_TICKS(10)))
        {
            LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to stop TapKeyTimer in %s", __func__);
        }

        /* Reset our variables */
        PressTime = 0;
        HoldTime = 0;
        LastKey = BOSE_INVALID_KEY;
    }
    else // send a "repeat"
    {
        HandleKey(LastKey, LastProducer);
        if (xTimerReset(TapKeyTimer, 0) != pdPASS)
        {
            LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to reset TapKeyTimer.");
        }
    }
}

/*
 * @func TapKeyTimerCallback
 *
 * @brief Callback for press and holds. Posts a message requesting either a repeat be sent or the timer
 *        cancelled.
 *
 * @param xTimerHandle pxTimer - The TapKeyTimer handle (not used)
 *
 * @return n/a
 */
static void TapKeyTimerCallback (xTimerHandle pxTimer)
{
    ETAPPostMsg(TAPPARSER_MESSAGE_ID_TapKeyPressAndHold, NULL);
}

void GiveKeyListSemaphore (void)
{
    xSemaphoreGive(KeyListSemaphore);
}

#ifdef SUPPORT_ASSIGN_KEY_TO_HDMI_INPUT
//TODO: This probably belongs with a large set of product specific commands and functions
BOOL SetAssignableHDMIInputID(KEY_VALUE keyValue);
KEY_VALUE GetKeyAssignedToHDMIInput();

/*
 * @func TAP_KeyPress
 *
 * @brief Enables key presses through TAP.
 */
TAPCommand(TAP_AssignHDMIInputKey)
{
    BOOL success = FALSE;
    if (0 == CommandLine->numArgs)
    {
        success = TRUE;
        TAP_Printf("HDMI input is assigned to key %x\r\n", GetKeyAssignedToHDMIInput());
    }
    if (1 == CommandLine->numArgs)
    {
        BOOL valid = FALSE;
        KEY_VALUE key = (KEY_VALUE) TAP_HexArgToInt(CommandLine, 0, &valid);
        if (valid)
        {
            success = SetAssignableHDMIInputID(key);
        }
    }
    if (success)
    {
        TAP_PrintString(TAP_OKText);
    }
    else
    {
        TAP_PrintString(TAP_InvalidArg);
    }
}

#endif
