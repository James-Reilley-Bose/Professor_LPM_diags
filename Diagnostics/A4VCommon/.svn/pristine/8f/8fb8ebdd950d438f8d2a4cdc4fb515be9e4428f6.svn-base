//
// WirelessAudioCUPairing.c
//

#include "WirelessAudioTask.h"
#include "WirelessAudioVariant.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioCUPairing.h"
#include "UITask.h"
#include "AccessoryManager.h"

SCRIBE_DECL(wa_task);

#define WA_TIMER_PAIR_POLL_PERIOD TIMER_SEC_TO_TICKS(5)
#define WA_PAIRING_TIMEOUT_PERIOD_MS 120000
#define WA_PAIRING_NUM_BASS 2
#define WA_PAIRING_NUM_SURROUND 2

static void WirelessAudioCUPairing_HandleEventOccurred(void);
static void WirelessAudioCUPairing_ProcessDataMessage(WA_DataMessage_t* message);
static void WirelessAudioCUPairing_HandleSNPacket(WA_DataMessage_t* message);
static void WirelessAudioCUPairing_HandleIDPacket(WA_DataMessage_t* message);
static BOOL WirelessAudioCUPairing_ContinuePairing(void);
static void WirelessAudioCUPairing_HandleTimer(void);
static void WirelessAudioCUPairing_ResetFlags(void);

static uint8_t numConnectedSurrounds = 0;
static uint8_t numConnectedBass = 0;
static uint8_t numNeededBass = 0;
static BOOL bassNeedsPair = FALSE;
static BOOL surroundNeedsPair = FALSE;
static BOOL pairingComplete = FALSE;
static uint32_t pairingStartTime = 0;

#ifdef WIRELESS_AUDIO_TWO_PHASE_ORDERED_PAIRING
#define PAIRING_NUM_PHASES 2
static unsigned int pairingPhase = 0;
#endif

void WirelessAudioCUPairing_EnterState(void)
{
    numConnectedSurrounds = AccessoryManager_NumberOfTypeConnected(ACCESSORY_MAXWELL);
    if(numConnectedSurrounds < WA_PAIRING_NUM_SURROUND) surroundNeedsPair = TRUE;

    numConnectedBass = AccessoryManager_NumberOfTypeConnected(ACCESSORY_SKIPPER) \
                       + AccessoryManager_NumberOfTypeConnected(ACCESSORY_LOVEY);
    if(numConnectedBass < WA_PAIRING_NUM_BASS)
    {
        bassNeedsPair = TRUE;
        if(numConnectedBass == 0)
        {
            numNeededBass = 1;
        }
        else
        {
            numNeededBass = 2;
        }
    }

    UIPostMsg(UI_MSG_ID_LLW_PairingOpened, NOP_CALLBACK, 0);
    WirelessAudioTask_ChangeTimerPeriod(WA_TIMER_PAIR_POLL_PERIOD);
    WirelessAudioTask_StartTimer();
    pairingStartTime = GET_SYSTEM_UPTIME_MS();
    
    LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Enter pairing");
}

void WirelessAudioCUPairing_ExitState(void)
{
    WirelessAudioTask_StopTimer();
    WirelessAudioUtilities_UpdateCoco();
    WirelessAudioCUPairing_ResetFlags();
    UIPostMsg(UI_MSG_ID_LLW_PairingClosed, NOP_CALLBACK, 0);
    WirelessAudioPostMsg(WA_MSG_ID_GetSpeakerVersion, NOP_CALLBACK, NULL);
    LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Exit pairing");
}

BOOL WirelessAudioCUPairing_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;

    switch(msg->msgID)
    {
        case WA_MSG_ID_HandleEXTI:
            WirelessAudioCUPairing_HandleEventOccurred();
            break;
        case WA_MSG_ID_TimerExpired:
            WirelessAudioCUPairing_HandleTimer();
            break;
        default:
            handled = FALSE;
            break;
    }

    return handled;
}

static void WirelessAudioCUPairing_HandleEventOccurred(void)
{
    WirelessAudioUtilities_ReadAllDataMessages(WirelessAudioCUPairing_ProcessDataMessage);
}

static void WirelessAudioCUPairing_ProcessDataMessage(WA_DataMessage_t* message)
{
    switch(message->opcode)
    {
        case WA_BCMD_SERIAL_NO:
            WirelessAudioCUPairing_HandleSNPacket(message);
            break;
        case WA_BCMD_ID:
            WirelessAudioCUPairing_HandleIDPacket(message);
            break;
        default:
            // We can just drop everything except the serial # packets
            LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "Pairing, ignore packet 0x%02X", message->opcode);
            break;
    }
}

static void WirelessAudioCUPairing_HandleSNPacket(WA_DataMessage_t* message)
{
    WAPacket_SerialNum_t* serial = (WAPacket_SerialNum_t*) message->data;

    LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "Serial - channel = 0x%02X, cs = %d",
        serial->channel,
        serial->cocoSum);

    WirelessAudioUtilities_UpdateCoco();

    // Legacy speakers don't include a cocoSum field
    if((serial->cocoSum != 0) &&
       (serial->cocoSum != WirelessAudioUtilities_GetCocoSum()))
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Pairing, coco not match %d", serial->cocoSum);
        return;
    }

    WA_ChannelID_t channel = (WA_ChannelID_t) serial->channel;
    uint16_t channelMask = WirelessAudioUtilities_GetChannelBitMask(channel);

    if(channelMask)
    {
        if(serial->cocoSum != 0)
        {
            if(AccessoryManager_UpdateSpeakers_SN(serial))
            {
                WirelessAudioUtilities_SendSNAck(channel, serial->serialNumber);
            }
        }
        else if(AccessoryManager_UpdateSpeakers_SN_LegacyPairing(serial))
        {
            WirelessAudioUtilities_SendSNAck(channel, serial->serialNumber);
        }
    }
    else
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "SN - unknown channel 0x%02X", channel);
    }
}

static void WirelessAudioCUPairing_HandleIDPacket(WA_DataMessage_t* message)
{
    WAPacket_ID_t* id = (WAPacket_ID_t*) message->data;

    if(id->serialNumberCRC != 0)
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "New speaker - ignore ID");
        return;
    }

    WirelessAudioUtilities_UpdateCoco();
    uint8_t myCoco[WA_COCO_LEN] = {0};
    WirelessAudioUtilities_GetCurrentCoco(myCoco);

    WA_ChannelID_t channel = (WA_ChannelID_t) id->channel;
    uint16_t channelMask = WirelessAudioUtilities_GetChannelBitMask(channel);

    if(channelMask && (memcmp(myCoco, id->coco, WA_COCO_LEN) == 0))
    {
        if(AccessoryManager_UpdateSpeakers_ID_LegacyPairing(id))
        {
          // This sucks to do, but if the surrounds are misconfigured and
          // we send the ack too soon, the "slower" Maxwell may never
          // send its ID. So we'll delay...
          vTaskDelay(TIMER_MSEC_TO_TICKS(100));
          WirelessAudioUtilities_SendIdAck((WA_ChannelID_t) id->channel);
        }
        else
        {
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "No slot = no ack for ID");
        }
    }
    else
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Legacy ID, wrong coco");
    }
}

static BOOL WirelessAudioCUPairing_ContinuePairing(void)
{
    BOOL continuePairing = TRUE;
    LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "Continue pair: bass %d, surround %d",
                                         bassNeedsPair,
                                         surroundNeedsPair);

    if(bassNeedsPair)
    {
        numConnectedBass = AccessoryManager_NumberOfTypeConnected(ACCESSORY_SKIPPER) \
                           + AccessoryManager_NumberOfTypeConnected(ACCESSORY_LOVEY);
        if(numConnectedBass >= numNeededBass)
        {
            continuePairing = FALSE;
        }
    }

    if(surroundNeedsPair)
    {
        numConnectedSurrounds = AccessoryManager_NumberOfTypeConnected(ACCESSORY_MAXWELL);
        if(numConnectedSurrounds >= WA_PAIRING_NUM_SURROUND)
        {
            continuePairing = FALSE;
        }
        else if(numConnectedSurrounds > 0)
        {
            continuePairing = TRUE;
        }
    }

#ifdef WIRELESS_AUDIO_TWO_PHASE_ORDERED_PAIRING
    if(!surroundNeedsPair && (pairingPhase == 0))
    {
        continuePairing = FALSE;
    }
#endif

    if(!bassNeedsPair && !surroundNeedsPair)
    {
        continuePairing = FALSE;
    }

    return continuePairing;
}

static void WirelessAudioCUPairing_HandleTimer(void)
{
    if((GET_MILLI_SINCE(pairingStartTime) > WA_PAIRING_TIMEOUT_PERIOD_MS) ||
       (pairingComplete && !WirelessAudioCUPairing_ContinuePairing()))
    {
        WirelessAudioVariant_GoToState(WA_STATE_ON);
        return;
    }

    if(!WirelessAudioCUPairing_ContinuePairing())
    {
        pairingComplete = TRUE;
    }
    else
    {
        // We want to briefly leave the pairing window closed so we don't kick freshly
        // paired speakers off the network. The skip variable is used to signal that
        // we should wait till the next watchdog period to open pairing again.
        static BOOL skip = FALSE;
        pairingComplete = FALSE;
        BOOL pairingActive = WirelessAudioUtilities_IsPairingActive();
        if((AccessoryManager_NumberWithStatusConnected(ACCESSORY_CONNECTION_LEGACY_PAIRING) == 0) &&
           !pairingActive &&
           !skip)
        {
            skip = TRUE;
            WirelessAudioUtilities_OpenPairingWindow(TRUE);
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Opening new window.");
        }
        else if(!pairingActive)
        {
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Skip - let the MUs connect.");
            skip = FALSE;
        }
    }
    WirelessAudioTask_StartTimer();
}

static void WirelessAudioCUPairing_ResetFlags(void)
{
#ifdef WIRELESS_AUDIO_TWO_PHASE_ORDERED_PAIRING
    pairingPhase = (pairingPhase + 1) % PAIRING_NUM_PHASES;
#endif
    numConnectedSurrounds = 0;
    numConnectedBass = 0;
    bassNeedsPair = FALSE;
    surroundNeedsPair = FALSE;
    pairingComplete = FALSE;
    pairingStartTime = 0;
}
