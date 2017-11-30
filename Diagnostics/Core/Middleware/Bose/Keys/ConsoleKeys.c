/**
  ******************************************************************************
  * @file    ConsoleKeys.c
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

#include "ConsoleKeys.h"
#include "genTimer.h"
#include "timers.h"
#include "KeyHandler.h"
#include "KeyHandlerTask.h"
#include "KeyData.h"


SCRIBE_DECL(console_keys);

/* Local functions */
static void CK_HandleNewKeyMask(const uint32_t mask);

static uint32_t CK_GetButtonState(void);
static void CK_LookForChanges(void);

/* Timers */
static TimerHandle_t s_CKScanTimerHandle;
static TimerHandle_t s_CKTimerDebounceHandle;
static TimerHandle_t s_CKVulcanKeysTimerHandle;

static KEY_DATA_t s_CKLastKey =
{
    .value = BOSE_INVALID_KEY
} ; // The LastKey received (for press and holds)

// key held masks
static uint32_t s_CurrentHeldMask = 0;
static uint32_t s_CurrentDebounceMask = 0;
static uint32_t s_CurrentGPIOMask = 0;

static int s_CKComboHeld = NUM_CONSOLE_VULCAN_KEYPRESSES;

CREATE_KEY_PRODUCER(kpConsole, KEY_PRODUCER_CONSOLE, CONSOLE_HOLD_TIME, CONSOLE_RELEASE_TIME, CONSOLE_REPEAT_TIME, TIMER_ID_ConsoleReleaseOne, TIMER_ID_ConsoleRepeatOne);

/*
===============================================================================
@fn CK_Init
@brief Init for console keys
===============================================================================
*/
void CK_Init(void)
{
    // Function generated by above macro
    kpConsole_init();

    ConfigureGpioPin (BUTTON1_GPIO_BANK, BUTTON1_GPIO_PIN,
                      GPIO_MODE_IN_FLOATING, NO_ALT_FUNC);
    ConfigureGpioPin (BUTTON2_GPIO_BANK, BUTTON2_GPIO_PIN,
                      GPIO_MODE_IN_FLOATING, NO_ALT_FUNC);
    ConfigureGpioPin (BUTTON3_GPIO_BANK, BUTTON3_GPIO_PIN,
                      GPIO_MODE_IN_FLOATING, NO_ALT_FUNC);
    ConfigureGpioPin (BUTTON4_GPIO_BANK, BUTTON4_GPIO_PIN,
                      GPIO_MODE_IN_FLOATING, NO_ALT_FUNC);

    /* Create timers used */
    /* Key scanning timer */
    s_CKScanTimerHandle = createTimer(TIMER_MSEC_TO_TICKS (CK_CONSOLE_SCAN_RATE_MSEC), \
                                      NULL, \
                                      KEYHANDLER_MESSAGE_ID_CK_Timer_KeyScan, \
                                      FALSE, \
                                      tBlockCKScanTimer, \
                                      "CK_ScanTmr", \
                                      NULL);
    debug_assert(s_CKScanTimerHandle != NULL);

    /* Debounce timer */
    s_CKTimerDebounceHandle = createTimer(TIMER_MSEC_TO_TICKS (CK_DEBOUNCE_POLL_INTERVAL_MSEC), \
                                          NULL, \
                                          KEYHANDLER_MESSAGE_ID_CK_Timer_KeyDebounce, \
                                          FALSE, \
                                          tBlockCKDebounceTimer, \
                                          "CK_DebounceTmr", \
                                          NULL);
    debug_assert(s_CKTimerDebounceHandle != NULL);

    /* Vulcan timer - multiple key press */
    s_CKVulcanKeysTimerHandle = createTimer(TIMER_MSEC_TO_TICKS (CK_VULCAN_CHECK_FOR_BOOT_MS), \
                                            NULL, \
                                            KEYHANDLER_MESSAGE_ID_CK_Timer_KeyVulcan, \
                                            FALSE, \
                                            tBlockVulcanKeysTimer, \
                                            "CK_VulcanTmr", \
                                            NULL);
    debug_assert(s_CKVulcanKeysTimerHandle != NULL);

    /* Start the key scanning timer, subsequent timer is started if no key is pressed or debouncing */
    timerStart(s_CKScanTimerHandle, 0, &ManagedKeyHandlerTask->Queue);

}


/*
===============================================================================
@fn CK_KeyScanTimerExpired
@brief Scan keys for changes
===============================================================================
*/
void CK_KeyScanTimerExpired (void)
{
    static uint32_t pressedMask = 0;
    /* check the gpio-based keys for a change in state */
    uint32_t newMask = CK_GetButtonState();

    if (newMask != pressedMask)
    {
        s_CurrentGPIOMask = newMask;
        pressedMask = newMask;
        CK_HandleNewKeyMask(s_CurrentGPIOMask);
    }
    else
    {
        /* Sent the last single key pressed repeatedly */
        if ( (s_CKLastKey.value != BOSE_INVALID_KEY) &&
                (s_CKComboHeld == NUM_CONSOLE_VULCAN_KEYPRESSES) )
        {
            HandleKey((KEY_VALUE)s_CKLastKey.value, &kpConsole);
        }

        /* Restart the scan timer */
        timerStart(s_CKScanTimerHandle, 0, &ManagedKeyHandlerTask->Queue);
    }
}

/*
===============================================================================
@fn CK_VulcanTimer
@brief Handling for multiple keys
===============================================================================
*/
void CK_VulcanTimer(void)
{
    KEY_DATA_t key =
    {
        .producer = KEY_PRODUCER_CONSOLE
    };


    if ((s_CKVulcanPresses[s_CKComboHeld].KeyMask & s_CurrentHeldMask) ==
            s_CKVulcanPresses[s_CKComboHeld].KeyMask)
    {

        key.value = (KEY_VALUE)s_CKVulcanPresses[s_CKComboHeld].KeyValue;
        key.state = LPM_KEY_STATE_PRESSED;

        HandleKey((KEY_VALUE)key.value, &kpConsole);
    }

}

/*
===============================================================================
@fn CK_HandleNewKeyMask
@brief
===============================================================================
*/
static void CK_HandleNewKeyMask(const uint32_t mask)
{
    if (mask != s_CurrentDebounceMask)
    {
        s_CurrentDebounceMask = mask;

        timerStart(s_CKTimerDebounceHandle, 0, &ManagedKeyHandlerTask->Queue);
    }
}

/*
===============================================================================
@fn CK_GetButtonState
@brief Buttons pressed will be shorted to ground
===============================================================================
*/
static uint32_t CK_GetButtonState(void)
{
    uint32_t newMask = 0x0000;
    newMask |= (GPIO_PIN_ASSERTED(BUTTON1_GPIO_BANK, BUTTON1_GPIO_PIN) ? 0x0000 : KEYPAD_BOSE_BUTTON_1);
    newMask |= (GPIO_PIN_ASSERTED(BUTTON2_GPIO_BANK, BUTTON2_GPIO_PIN) ? 0x0000 : KEYPAD_BOSE_BUTTON_2);
    newMask |= (GPIO_PIN_ASSERTED(BUTTON3_GPIO_BANK, BUTTON3_GPIO_PIN) ? 0x0000 : KEYPAD_BOSE_BUTTON_3);
    newMask |= (GPIO_PIN_ASSERTED(BUTTON4_GPIO_BANK, BUTTON4_GPIO_PIN) ? 0x0000 : KEYPAD_BOSE_BUTTON_4);
    return newMask;
}

/*
===============================================================================
@fn CK_DebounceTimerFired
@brief Timer call back for key debouncing handling
===============================================================================
*/
void CK_DebounceTimerFired(void)
{

    /* we have a debounced and stable state of the keys */
    CK_LookForChanges();
    /* store off the current state */
    s_CurrentHeldMask = s_CurrentDebounceMask;

    /* Restart the scan timer */
    timerStart(s_CKScanTimerHandle, 0, &ManagedKeyHandlerTask->Queue);
}

/*
===============================================================================
@fn CK_IsVulcanPressActive
@brief Check if a combo press is active.

===============================================================================
*/
BOOL CK_IsVulcanPressActive(void)
{
    return s_CKComboHeld < NUM_CONSOLE_VULCAN_KEYPRESSES ? TRUE : FALSE;
}

/*
===============================================================================
@fn CK_LookForChanges
@brief Check if the newly aquired state differs from the current one, and
       send key message for each key that has changed  it also checks if there
       is a vulcan key press in effect
===============================================================================
*/
static void CK_LookForChanges(void)
{
    uint32_t changedBits = s_CurrentHeldMask ^ s_CurrentDebounceMask;

    KEY_DATA_t key =
    {
        .producer = KEY_PRODUCER_CONSOLE
    };
    uint32_t thisScan;

    if (!changedBits )
    {
        return;
    }
    // Combo keys handling - check if multiple key press is detected and activate the
    // some delays later
    //
    // check if the currently held Vulcan press has changed
    // if changed, stop the vulcan key handling timer
    if ((s_CKComboHeld < NUM_CONSOLE_VULCAN_KEYPRESSES ) &&
            (s_CurrentDebounceMask != s_CKVulcanPresses[s_CKComboHeld].KeyMask))
    {
        s_CKComboHeld = NUM_CONSOLE_VULCAN_KEYPRESSES;
        timerStop(s_CKVulcanKeysTimerHandle, 0);
        return;
    }


    // Look for combo key press
    if (s_CKComboHeld == NUM_CONSOLE_VULCAN_KEYPRESSES)
    {
        /* check if a combo is held */
        for (int i = 0;
                (i < NUM_CONSOLE_VULCAN_KEYPRESSES) &&
                (s_CKComboHeld == NUM_CONSOLE_VULCAN_KEYPRESSES);
                i++)
        {
            if (s_CKVulcanPresses[i].KeyMask == s_CurrentDebounceMask)
            {

                /* LEGACY code: Multiple key press disabled after boot - disabled for now */
                /*if((s_CKVulcanPresses[i].AfterbootWindow == VULCAN_VALID_ANY_TIME) ||
                    (s_CKVulcanPresses[i].AfterbootWindow >= GET_SYSTEM_UPTIME_MS()))
                 { */
                s_CKComboHeld = i;
                s_CKLastKey.value = BOSE_INVALID_KEY;
                /* arm the timer for the length of time this press and hold lasts for */
                changeTimerPeriod( s_CKVulcanKeysTimerHandle, s_CKVulcanPresses[i].HoldTime , 0 );
                timerStart(s_CKVulcanKeysTimerHandle, 0, &ManagedKeyHandlerTask->Queue);
                /* } */
            }
        }
    }

    // if the user is pressing a combo, then the indivuidual keys mean nothing.
    //   return from here
    if (s_CKComboHeld < NUM_CONSOLE_VULCAN_KEYPRESSES)
    {
        return;
        // thisScan = 0;
        // changedBits = s_CurrentHeldMask;
    }
    else
    {
        thisScan = s_CurrentDebounceMask;
    }


    // send key up  for the changed keys
    for (int i = 0; i < NUM_CONSOLE_KEYS; i++)
    {
        // If there is difference between the current key press and last key press,
        // and it is one of the console key
        if (changedBits & CKKeyVals[i].KeyMask)
        {
            // Check if it is being pressed or released
            // Change the key state. Only key_press is necessary for Bardeen
            // key press
            if (thisScan & CKKeyVals[i].KeyMask)
            {
                // set value and state
                key.value = (KEY_VALUE)CKKeyVals[i].KeyValue;
                key.state = LPM_KEY_STATE_PRESSED;

                /* Remember the key press to send the keys repeatedly until it was released */
                s_CKLastKey = key;

            }
            // key release
            else
            {

                // Stop sending the last keys repeatedly
                s_CKLastKey.value = BOSE_INVALID_KEY;

                return;
            }


            /* tell key handler about the change */

            HandleKey((KEY_VALUE)key.value, &kpConsole);
        }
    }

}
