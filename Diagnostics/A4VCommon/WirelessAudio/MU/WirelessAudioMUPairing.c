//
// WirelessAudioMUPairing.c
//

#include "WirelessAudioMUPairing.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioTask.h"
#include "nv_mfg.h"
#include "nv_mfg_struct.h"
#include "UITask.h"

SCRIBE_DECL(wa_task);

#define WA_MU_PREVIOUSLY_PAIRED_PAIRING_TIME_MS 1080000
#define WA_MU_NEVER_PAIRED_PAIRING_TIME_MS 7200000
#define WA_MU_PAIRING_WINDOW_PERIOD_MS 70000
#define WA_MU_PAIRING_POLL_PERIOD TIMER_SEC_TO_TICKS(1)

static void WirelessAudioMUPairing_HandleTimer(void);
static void WirelessAudioMUPairing_HandleEventOccurred(void);
static void WirelessAudioMUPairing_ProcessDataMessage(WA_DataMessage_t* message);
static void WirelessAudioMUPairing_HandleSNAck(WA_DataMessage_t* message);

static uint32_t lastPairCmdTime = 0;
static uint32_t endTime = 0;

void WirelessAudioMUPairing_EnterState(void)
{
    endTime = WirelessAudioUtilities_IsPreviouslyPaired() ? WA_MU_PREVIOUSLY_PAIRED_PAIRING_TIME_MS :
                                                            WA_MU_NEVER_PAIRED_PAIRING_TIME_MS;
    WirelessAudioUtilities_OpenPairingWindow(TRUE);
    WirelessAudioTask_ChangeTimerPeriod(WA_MU_PAIRING_POLL_PERIOD);
    WirelessAudioTask_StartTimer();
    lastPairCmdTime = GET_SYSTEM_UPTIME_MS();
}

void WirelessAudioMUPairing_ExitState(void)
{
    WirelessAudioTask_StopTimer();
}

BOOL WirelessAudioMUPairing_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;
    switch(msg->msgID)
    {
        case WA_MSG_ID_TimerExpired:
            WirelessAudioMUPairing_HandleTimer();
            break;
        case WA_MSG_ID_HandleEXTI:
            WirelessAudioMUPairing_HandleEventOccurred();
            break;
        default:
            handled = FALSE;
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Unhandled msg %d", msg->msgID);
            break;
    }
    return handled;
}

static void WirelessAudioMUPairing_HandleTimer(void)
{
    if(GET_SYSTEM_UPTIME_MS() > endTime)
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Pairing expired - %d", endTime);
        UIPostMsg(UI_MSG_ID_MuPairingClosed, NOP_CALLBACK, (uint32_t) FALSE);
        WirelessAudioVariant_GoToState(WA_STATE_ON);
        return;
    }

    if(WirelessAudioUtilities_IsLinked())
    {
        WirelessAudioUtilities_UpdateCoco();
        if(WirelessAudioUtilities_IsPreviouslyPaired())
        {
            WirelessAudioUtilities_SendSN();
        }
    }

    if(GET_MILLI_SINCE(lastPairCmdTime) >= WA_MU_PAIRING_WINDOW_PERIOD_MS)
    {
        WirelessAudioUtilities_OpenPairingWindow(TRUE);
        lastPairCmdTime = GET_SYSTEM_UPTIME_MS();
    }

    WirelessAudioTask_StartTimer();
}

static void WirelessAudioMUPairing_HandleEventOccurred(void)
{
    WirelessAudioUtilities_ReadAllDataMessages(WirelessAudioMUPairing_ProcessDataMessage);
}

static void WirelessAudioMUPairing_ProcessDataMessage(WA_DataMessage_t* message)
{
    switch(message->opcode)
    {
        case WA_BCMD_SERIAL_NO:
            WirelessAudioMUPairing_HandleSNAck(message);
            break;
        default:
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Dropping packet 0x%02X", message->opcode);
            break;
    }
}

static void WirelessAudioMUPairing_HandleSNAck(WA_DataMessage_t* message)
{
    WAPacket_SerialNum_t* sn = (WAPacket_SerialNum_t*) message->data;
    uint8_t mySN[SERIAL_NO_LEN] = {0};
    NV_GetSystemSerialNo(mySN, SERIAL_NO_LEN, 0);
    if((sn->channel == WirelessAudioUtilities_GetChannel()) &&
       (memcmp(sn->serialNumber, mySN, SERIAL_NO_LEN) == 0))
    {
        UIPostMsg(UI_MSG_ID_MuPairingClosed, NOP_CALLBACK, (uint32_t) TRUE);
        WirelessAudioVariant_GoToState(WA_STATE_ON);
    }
}
