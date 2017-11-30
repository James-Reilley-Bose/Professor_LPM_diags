#include "IPCRouterTask.h"
#include "IpcKeys.h"
#include "KeyHandler.h"
#include "RivieraLPM_IpcProtocol.h"

SCRIBE_DECL(keys);

CREATE_KEY_PRODUCER(kpIPC, KEY_PRODUCER_IPC, DEFAULT_HOLD_TIME, DEFAULT_RELEASE_TIME, DEFAULT_REPEAT_TIME, TIMER_ID_IpcRelease, TIMER_ID_IpcRepeat);
/*
 * @func IpcKeys_Initialize
 *
 * @brief Function to init IpcKeys
 *
 * @param void
 *
 * @return void
 */
void IpcKeys_Initialize(void)
{
    // ?
}

/*
 * @func IpcKeys_SendKey
 *
 * @brief Send a key to destination
 *
 * @param uint8_t dest   - who to send key to
 *        KEY_DATA_t key - the key press to send
 *
 * @return void
 */
void IpcKeys_SendKey(Ipc_Device_t dest, KEY_DATA_t key)
{
    IpcKeyPayload payload =
    {
        REV_WORD_BYTES((uint32_t)key.timestamp),
        REV_WORD_BYTES((uint32_t)key.producer),
        REV_WORD_BYTES((uint32_t)NULL),
        REV_WORD_BYTES((uint32_t)key.value),
        REV_WORD_BYTES((uint32_t)key.state)
    };

    IPCRouter_Send(dest, IPC_KEY, NULL, &payload, sizeof(IpcKeyPayload));
}

/*
 * @func IpcKeys_KeyProducerToOrigin
 *
 * @brief convert LPM key producer to IPC key origin
 *
 * @param producer - Key Producer
 *
 * @return IPC key origin
 */

static KeyOrigin_t IpcKeys_KeyProducerToOrigin(KEY_PRODUCER producer)
{
    KeyOrigin_t origin;

    switch (producer)
    {
    case KEY_PRODUCER_CONSOLE:
        origin = KEY_ORIGIN_CONSOLE_BUTTON;
        break;
    case KEY_PRODUCER_RF:
        origin = KEY_ORIGIN_RF;
        break;
    case KEY_PRODUCER_IR:
        origin = KEY_ORIGIN_IR;
        break;
    case KEY_PRODUCER_TAP:
        origin = KEY_ORIGIN_TAP;
        break;
    case KEY_PRODUCER_CEC:
        origin = KEY_ORIGIN_CEC;
        break;
    default:
        LOG(keys, ROTTEN_LOGLEVEL_NORMAL, "unsupported key producer %d", producer);
        break;
    }
    return origin;
}

/*
 * @func IpcKeys_LPMKeyStateToIpcKeyState
 *
 * @brief convert LPM key state to IPC key state
 *
 * @param state - LPM key state
 *
 * @return Ipc Key state
 */
static KeyState_t IpcKeys_LPMKeyStateToIpcKeyState(LPM_KEY_STATE state)
{
    KeyState_t key_state = KEY_RELEASED;

    switch (state)
    {
    case LPM_KEY_STATE_PRESSED:
        key_state = KEY_PRESSED;
        break;
    case LPM_KEY_STATE_RELEASED:
        key_state = KEY_RELEASED;
        break;
    default:
        LOG(keys, ROTTEN_LOGLEVEL_NORMAL,"unsupported key state %d", state);
        break;
    }
    return key_state;
}

/*
 * @func    IpcKeys_SendKeyInfo
 *
 * @brief   Send Key information from LPM to another device (mainly Riviera)
 *
 * @param   dest - Destination device
 *          key  - key data
 *
 * @return void
 */
void IpcKeys_SendKeyInfo(Ipc_Device_t dest, KEY_DATA_t key)
{
    IpcKeyInformation_t keyInfo =
    {
        .keyOrigin   = (KeyOrigin_t)REV_WORD_BYTES(IpcKeys_KeyProducerToOrigin(key.producer)),
        .keyState    = (KeyState_t)REV_WORD_BYTES(IpcKeys_LPMKeyStateToIpcKeyState(key.state)),
        .keyId       = REV_HALF_WORD_BYTES(key.value),
    };

    IPCRouter_Send(dest, IPC_KEY, NULL, &keyInfo, sizeof(IpcKeyInformation_t));
}

/*
 * @func IpcKeys_KeyHookHandleRequest
 *
 * @brief Register a ipc device to recieve keys
 *
 * @param IpcPacket_t* packet - key hook request recieved
 *
 * @return void
 */
void IpcKeys_KeyHookHandleRequest(IpcPacket_t* packet)
{
    // TODO - Functionality dropped
}

/*
 * @func IpcKeys_HandleKey
 *
 * @brief Recieve a key from ipc device
 *
 * @param IpcPacket_t* packet - packet with key data recieved
 *
 * @return void
 */
void IpcKeys_HandleKey(IpcPacket_t* packet)
{
    IpcKeyPayload* key = (IpcKeyPayload*) packet->s.data.w;
    HandleKey((KEY_VALUE) REV_WORD_BYTES(key->keyValue), &kpIPC);
}
