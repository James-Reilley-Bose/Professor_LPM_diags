/*
 * KeyHandler.c - Handles timers to keep track of key pres`s and releases.
 *
 * Author: Dillon Johnson
 */

#include "KeyHandler.h"
#include "KeyHandlerTask.h"
#include "HwMacros.h"
#include "etap.h"
#include "etapKeys.h"
#include "KeyVariant.h"

SCRIBE_DECL(keys);


/* Variables 'kl' TAP command */
#define NUM_RECORDED_KEYS 8
static uint8_t numRecordedKeys = 0;
static uint8_t recordedKeysIndex = NUM_RECORDED_KEYS - 1;
static KEY_DATA_t lastKeys[NUM_RECORDED_KEYS];

/* Local functions */
static void SendKey(KEY_DATA_t key);
static void AddKeyToList (KEY_DATA_t* newKey);

/*
 * @func SendKey
 *
 * @brief Sends key up for routing. For now, just prints a log message until the
 *        downstream key pipe is in place.
 *
 * @param void KEY_DATA_t key - the key to send.
 *
 * @return n/a
 */
static void SendKey(KEY_DATA_t key)
{
#if 0
    if (key.state == LPM_KEY_STATE_PRESSED)
    {
        LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "0x%x key pressed.", key.value);
    }
    if (key.state == LPM_KEY_STATE_RELEASED)
    {
        LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "0x%x key released.", key.value);
    }
#endif
    Product_HandleKey(&key);
}

/*
 * @func HandleKey
 *
 * @brief Posts to the Key Handler Task with a new key to be processed.
 *
 * @param KEY_VALUE held_key - the key that was pressed
 * @param KEY_PRODUCER_ATTRIBUTES* producer - the module that produced the key (TAP, CEC, etc.)
 *
 * @return n/a
 */
void HandleKey(KEY_VALUE held_key, KEY_PRODUCER_ATTRIBUTES* producer)
{
    KeyHandlerPostMsg(KEYHANDLER_MESSAGE_ID_KeyPress, held_key, (uint32_t) producer);
}

/*
 * @func HandleKeyRelease
 *
 * @brief Posts to the Key Handler Task with a new key release to be processed.
 *
 * @param KEY_VALUE held_key - the key that was pressed
 * @param KEY_PRODUCER_ATTRIBUTES* producer - the module that produced the key (TAP, CEC, etc.)
 *
 * @return n/a
 */
void HandleKeyRelease(KEY_VALUE held_key, KEY_PRODUCER_ATTRIBUTES* producer)
{
    KeyHandlerPostMsg(KEYHANDLER_MESSAGE_ID_KeyRelease, held_key, (uint32_t) producer);
}


/*
 * @func InternalHandleKeyPress
 *
 * @brief Initializes/resets timers for pressed keys.
 *
 * @param KEY_VALUE held_key - the key that was pressed
 * @param KEY_PRODUCER_ATTRIBUTES* producer - the module that produced the key (TAP, CEC, etc.)
 *
 * @return n/a
 */
void InternalHandleKeyPress(KEY_VALUE held_key, KEY_PRODUCER_ATTRIBUTES* producer)
{
    //LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "InternalHandleKey: Key: 0x%02X.", held_key)
    /* If this is a valid key */
    if ((held_key < NUM_BOSE_KEYS) && (Key_Info[held_key].group != KEY_GROUP_INVALID))
    {
        KEY_DATA_t key =
        {
            .timestamp = GET_SYSTEM_UPTIME_MS(),
            .value = held_key,
            .state = LPM_KEY_STATE_PRESSED,
            .group = Key_Info[held_key].group,
            .producer = producer->ProducerID,
        };

        /* The timer callbacks could be modifying the key values. Block until it's our turn. */
        if (xSemaphoreTake(producer->KeyMutex, portMAX_DELAY) == pdTRUE)
        {
            /* If it's the same key */
            if (producer->HeldKey.value == key.value)
            {
                /* Block repeats for 300ms, then arms the repeat timer */
                if ((key.timestamp - producer->HeldKey.timestamp >= producer->HoldInterval) &&
                        (xTimerIsTimerActive(producer->Timers->RepeatTimer) == pdFALSE))
                {
#if LPM_SUPPORT_KEY_REPEAT                    
                    if (xTimerReset(producer->Timers->RepeatTimer, 0) != pdPASS)
                    {
                        LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to arm %s.", producer->Timers->RepeatTimerName)
                    }
#endif
                }

                if (xTimerReset(producer->Timers->ReleaseTimer, 0) != pdPASS)
                {
                    LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to reset %s.", producer->Timers->ReleaseTimerName)
                }
            }
            /* Else if a different key is already held, send a release first */
            else if (producer->HeldKey.value != BOSE_INVALID_KEY)
            {
                /* Send a release for the old key */
                producer->HeldKey.state = LPM_KEY_STATE_RELEASED;
                producer->HeldKey.timestamp = GET_SYSTEM_UPTIME_MS();
                producer->HeldKey.ph_timestamp = GET_SYSTEM_UPTIME_MS();
                SendKey(producer->HeldKey);

                /* Send a press for the new key */
                producer->HeldKey = key;
                SendKey(key);

                AddKeyToList(&key); // record to list

                /* Reset the release timer */
                if (xTimerReset(producer->Timers->ReleaseTimer, 0) != pdPASS)
                {
                    LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to reset %s.", producer->Timers->ReleaseTimerName);
                }
#if LPM_SUPPORT_KEY_REPEAT 
                /* Disable this until the block time has passed */
                if (xTimerStop(producer->Timers->RepeatTimer, 0) != pdPASS)
                {
                    LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to reset %s.", producer->Timers->RepeatTimerName);
                }
#endif
            }
            /* Else set up a new key */
            else
            {
                producer->HeldKey = key;
                producer->HeldKey.timestamp = GET_SYSTEM_UPTIME_MS();
                producer->HeldKey.ph_timestamp = GET_SYSTEM_UPTIME_MS();
                SendKey(key);

                AddKeyToList(&key); // record to list

                if (xTimerReset(producer->Timers->ReleaseTimer, 0) != pdPASS)
                {
                    LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to start %s.", producer->Timers->ReleaseTimerName);
                }
                if (Key_Info[producer->HeldKey.value].press_type != SINGLE_PRESS)
                {
#if LPM_SUPPORT_KEY_REPEAT   
                    if (xTimerReset(producer->Timers->RepeatTimer, 0) != pdPASS)
                    {
                        LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to start %s.", producer->Timers->RepeatTimerName);
                    }
#endif
                }
            }

            xSemaphoreGive( producer->KeyMutex);
        }
    }
    else
    {
        LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Key handler received invalid key %02X.", held_key);
    }
}


/*
 * @func InternalHandleKeyRelease
 *
 * @brief Releases any pressed keys for the producer.
 *
 * @param KEY_PRODUCER_ATTRIBUTES* producer - the module that produced the key (TAP, CEC, etc.)
 *
 * @return n/a
 */
void InternalHandleKeyRelease(KEY_PRODUCER_ATTRIBUTES* producer)
{
    /* The timer callbacks could be modifying the key values. Block until it's our turn. */
    if (xSemaphoreTake(producer->KeyMutex, portMAX_DELAY) == pdTRUE)
    {
        if (producer->HeldKey.value != BOSE_INVALID_KEY)
        {
            /* Send a release for the old key */
            producer->HeldKey.state = LPM_KEY_STATE_RELEASED;
            producer->HeldKey.timestamp = GET_SYSTEM_UPTIME_MS();
            producer->HeldKey.ph_timestamp = GET_SYSTEM_UPTIME_MS();
            SendKey(producer->HeldKey);

            /* Reset the key */
            producer->PassedHoldInterval = FALSE;
            producer->HeldKey.value = BOSE_INVALID_KEY;

            /* Disable the release timer */
            if (xTimerStop(producer->Timers->ReleaseTimer, 0) != pdPASS)
            {
                LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to stop %s.", producer->Timers->ReleaseTimerName);
            }
#if LPM_SUPPORT_KEY_REPEAT 
            /* Disable the repeat timer */
            if (xTimerStop(producer->Timers->RepeatTimer, 0) != pdPASS)
            {
                LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to stop %s.", producer->Timers->RepeatTimerName);
            }
#endif
        }
        xSemaphoreGive(producer->KeyMutex);
    }
}


/*
 * @func KeyReleaseCallback
 *
 * @brief The generic callback to send a key release when a "release timer" expires.
 *
 * @param KEY_PRODUCER_ATTRIBUTES* producer - the module that produced the key (TAP, CEC, etc.).
 *                        NOTE: This structure contains held key information
 *
 * @return n/a
 */
void KeyReleaseCallback (KEY_PRODUCER_ATTRIBUTES* producer)
{
    /* Do nothing if we can't take the semaphore. KeyHandler must have a new
     * key press and will adjust the timers for us. */
    if (xSemaphoreTake(producer->KeyMutex, (TickType_t) 0))
    {
        if (producer->HeldKey.value != BOSE_INVALID_KEY)
        {
            /* Send the release */
            producer->HeldKey.state_before_release = producer->HeldKey.state;
            producer->HeldKey.state = LPM_KEY_STATE_RELEASED;
            producer->HeldKey.timestamp = GET_SYSTEM_UPTIME_MS();
            producer->HeldKey.ph_timestamp = GET_SYSTEM_UPTIME_MS();
            SendKey(producer->HeldKey);

            /* Reset the key */
            producer->PassedHoldInterval = FALSE;
            producer->HeldKey.value = BOSE_INVALID_KEY;
        }
        else
        {
            LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Key release for invalid key.");
        }
#if LPM_SUPPORT_KEY_REPEAT 
        /* Stop the repeats */
        if (pdFAIL == xTimerStop(producer->Timers->RepeatTimer, 0))
        {
            LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to stop Key Repeat Timer.");
        }
#endif
        xSemaphoreGive(producer->KeyMutex);
    }
}

/*
 * @func KeyRepeatCallback
 *
 * @brief The generic callback to send a new key press when a "repeat timer" expires.
 *
 * @param KEY_PRODUCER_ATTRIBUTES* producer - the module that produced the key (TAP, CEC, etc.).
 *                        NOTE: This structure contains held key information
 *
 * @return n/a
 */
void KeyRepeatCallback (KEY_PRODUCER_ATTRIBUTES* producer)
{
#if LPM_SUPPORT_KEY_REPEAT
    /* Do nothing if we can't take the semaphore. KeyHandler must have a new
     * key press and will adjust the timers for us. */
    if (xSemaphoreTake(producer->KeyMutex, (TickType_t) 0))
    {
        if (producer->HeldKey.value != BOSE_INVALID_KEY)
        {
            uint32 system_uptime = GET_SYSTEM_UPTIME_MS();

            // Always send the key if it is a volume key
            if (Key_Info[producer->HeldKey.value].press_type == CONTINUOUS_PRESS)
            {
                if ((producer->PassedHoldInterval) ||
                        ((system_uptime - producer->HeldKey.timestamp) > producer->HoldInterval))
                {
                    producer->PassedHoldInterval = TRUE;
                    producer->HeldKey.timestamp = GET_SYSTEM_UPTIME_MS();
                    SendKey(producer->HeldKey);
                }
            }
            /* If the pressed key has press and hold functionality  */
            /* check to see if we have passed the press and hold time */
            if (Key_Info[producer->HeldKey.value].press_type == PRESS_AND_HOLD)
            {
                switch (producer->HeldKey.state)
                {
                    case LPM_KEY_STATE_PRESSED:
                        if ((system_uptime - producer->HeldKey.ph_timestamp) >= SHORT_PRESS_AND_HOLD_START)
                        {
                            producer->HeldKey.state = LPM_KEY_STATE_SHORT_PRESS_AND_HOLD;
                            SendKey(producer->HeldKey);

                        }
                        break;
                    case LPM_KEY_STATE_SHORT_PRESS_AND_HOLD:
                        if ((system_uptime - producer->HeldKey.ph_timestamp) >= PRESS_AND_HOLD_START)
                        {
                            producer->HeldKey.state = LPM_KEY_STATE_PRESS_AND_HOLD;
                            SendKey(producer->HeldKey);

                        }
                        break;
                    case LPM_KEY_STATE_PRESS_AND_HOLD:
                        if ((system_uptime - producer->HeldKey.ph_timestamp) >= VENTI_PRESS_AND_HOLD_START)
                        {
                            producer->HeldKey.state = LPM_KEY_STATE_VENTI_PRESS_AND_HOLD;
                            SendKey(producer->HeldKey);
                        }
                        break;
                    case LPM_KEY_STATE_VENTI_PRESS_AND_HOLD:
                        if ((system_uptime - producer->HeldKey.ph_timestamp) >= LONG_PRESS_AND_HOLD_START)
                        {
                            producer->HeldKey.state = LPM_KEY_STATE_LONG_PRESS_AND_HOLD;
                            SendKey(producer->HeldKey);
                        }
                        break;
                    case LPM_KEY_STATE_LONG_PRESS_AND_HOLD:
                        if ((system_uptime - producer->HeldKey.ph_timestamp) >= XL_PRESS_AND_HOLD_START)
                        {
                            producer->HeldKey.state = LPM_KEY_STATE_XL_PRESS_AND_HOLD;
                            SendKey(producer->HeldKey);
                        }
                        break;
                    default:
                        break;
                }
            }
            if (xTimerReset(producer->Timers->RepeatTimer, 0) != pdPASS)
            {
                LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Key repeat fail %s.", producer->Timers->RepeatTimerName)
            }
        }
        else
        {
            /* Stop sending repeats since the key is invalid... */
            if (pdFAIL == xTimerStop(producer->Timers->RepeatTimer, 0))
            {
                LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Failed to stop invalid Key Repeat Timer.");
            }
            LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "Key repeat for invalid key.")
        }
        xSemaphoreGive(producer->KeyMutex);
    }
#endif
}

/*
 * @func ResetKeyList
 *
 * @brief Resets the list of recorded keys.
 *
 * @param n/a
 *
 * @return n/a
 */
void ResetKeyList (void)
{
    numRecordedKeys = 0;
    recordedKeysIndex = NUM_RECORDED_KEYS - 1;
    TAP_Printf("Key list cleared.\r\n");
    GiveKeyListSemaphore();
}

/*
 * @func PrintKeyList
 *
 * @brief Prints the list of recorded keys to TAP
 *
 * @param n/a
 *
 * @return n/a
 */
void PrintKeyList (void)
{
    uint8_t i;
    uint8_t j;

    for (i = numRecordedKeys; i > 0; i--)
    {
        j = (recordedKeysIndex - (numRecordedKeys - i)); // interestingly, if you do the modulo on this line the compiler
        j = j % NUM_RECORDED_KEYS;                       // generates code that doesn't actually do the modulo....
        TAP_Printf("\r\n\tValue: %x,Key: %s, Producer: %s", lastKeys[j].value, Key_Info[lastKeys[j].value].name, KeyProducerNames[lastKeys[j].producer]);
    }
    TAP_Printf("\r\n");
    GiveKeyListSemaphore();
}

/*
 * @func AddKeyToList
 *
 * @brief Appends a new key to the list of recorded keys.
 *
 * @param KEY_DATA_t* newKey - the key to append
 *
 * @return n/a
 */
static void AddKeyToList (KEY_DATA_t* newKey)
{
    if (numRecordedKeys < NUM_RECORDED_KEYS)
    {
        numRecordedKeys++;
    }
    recordedKeysIndex = (recordedKeysIndex + 1) % NUM_RECORDED_KEYS;
    lastKeys[recordedKeysIndex] = *newKey;
}
