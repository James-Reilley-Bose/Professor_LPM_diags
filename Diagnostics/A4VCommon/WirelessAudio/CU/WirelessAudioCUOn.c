//
// WirelessAudioCUOn.c
//

#include "WirelessAudioTask.h"
#include "WirelessAudioVariant.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioCUOn.h"
#include "WirelessAudioData.h"
#include "WirelessAudioI2C.h"
#include "UITask.h"
#include "AccessoryManager.h"
#include "UpdateModuleSpeakers.h"
#include "buffermanager.h"

SCRIBE_DECL(wa_task);

#define WA_TIMER_ON_PERIOD TIMER_SEC_TO_TICKS(5)
#define WA_MISSED_TELEMTRY_BEFORE_DISCONNECT 3

static void WirelessAudioCUOn_HandleEventOccurred(void);
static void WirelessAudioCUOn_ProcessDataMessage(WA_DataMessage_t* message);
static void WirelessAudioCUOn_HandleIDPacket(WA_DataMessage_t* message);
static void WirelessAudioCUOn_HandleSNPacket(WA_DataMessage_t* message);
static void WirelessAudioCUOn_HandleTelemetry(WA_DataMessage_t* message);
static void WirelessAudioCUOn_HandleStartPair(void);

#if defined(BARDEEN)
static void WirelessAudioCUOn_HandleGetSpeakerVersion(WA_DataMessage_t* message);
static void WirelessAudioCUOn_StartGetSpeakerVersion(void);

static uint8_t speakerVerMsgCnt = 0;
#endif

BOOL WirelessAudioCUOn_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;

    switch(msg->msgID)
    {
        case WA_MSG_ID_HandleEXTI:
            WirelessAudioCUOn_HandleEventOccurred();
            break;
        case WA_MSG_ID_StartPair:
            WirelessAudioCUOn_HandleStartPair();
            break;
#if defined(BARDEEN)
        case WA_MSG_ID_GetSpeakerVersion:
            WirelessAudioCUOn_StartGetSpeakerVersion();
            break;
#endif
        default:
            handled = FALSE;
            break;
    }

    return handled;
}

static void WirelessAudioCUOn_HandleEventOccurred(void)
{
    WirelessAudioUtilities_ReadAllDataMessages(WirelessAudioCUOn_ProcessDataMessage);
}

static void WirelessAudioCUOn_ProcessDataMessage(WA_DataMessage_t* message)
{
    switch(message->opcode)
    {
        case WA_BCMD_ID:
             WirelessAudioCUOn_HandleIDPacket(message);
            break;
        case WA_BCMD_SERIAL_NO:
            WirelessAudioCUOn_HandleSNPacket(message);
            break;
        case WA_BCMD_TELEMETRY:
            WirelessAudioCUOn_HandleTelemetry(message);
            break;
        case WA_BCMD_UPD_VERSION:
#if defined(BARDEEN)
            WirelessAudioCUOn_HandleGetSpeakerVersion(message);
#endif
            break;
        default:
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Dropping packet 0x%02X", message->opcode);
            break;
    }
}

static void WirelessAudioCUOn_HandleIDPacket(WA_DataMessage_t* message)
{
    WAPacket_ID_t* id = (WAPacket_ID_t*) message->data;

    LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "ID - channel %d, darr_ver %d, coco 0x%02X%02X%02X, ss %d",
                                          id->channel,
                                          id->darr_ver,
                                          id->coco[0],
                                          id->coco[1],
                                          id->coco[2],
                                          id->serialNumberCRC);

    uint8_t myCoco[WA_COCO_LEN] = {0};
    WirelessAudioUtilities_GetCurrentCoco(myCoco);

    WA_ChannelID_t channel = (WA_ChannelID_t) id->channel;
    uint16_t channelMask = WirelessAudioUtilities_GetChannelBitMask(channel);

    if((channelMask != 0) && (memcmp(myCoco, id->coco, WA_COCO_LEN) == 0))
    {
        // Only ack if we have already received the serial number
        if(!AccessoryManager_IsSerialNew(id))
        {
            WirelessAudioUtilities_SendIdAck(channel);
            AccessoryManager_UpdateSpeakers_ID(id);
        }
        else
        {
            LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "No serial recv for this ID yet. 0x%02X", channel);
        }
    }
    else
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "ID - unknown channel 0x%02X", channel);
    }
}

static void WirelessAudioCUOn_HandleSNPacket(WA_DataMessage_t* message)
{
    WAPacket_SerialNum_t* serial = (WAPacket_SerialNum_t*) message->data;

    // DR -> DJ does this mean old MUs with coco sum will never pass SN up?
    if((serial->cocoSum != 0) &&
       (serial->cocoSum != WirelessAudioUtilities_GetCocoSum()))
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "On, coco not match %d", serial->cocoSum);
        return;
    }

    WA_ChannelID_t channel = (WA_ChannelID_t) serial->channel;
    uint16_t channelMask = WirelessAudioUtilities_GetChannelBitMask(channel);

    if(channelMask)
    {
        WirelessAudioUtilities_SendSNAck(channel, serial->serialNumber);
        AccessoryManager_UpdateSpeakers_SN(serial);
    }
    else
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "SN - unknown channel 0x%02X", channel);
    }
}

static void WirelessAudioCUOn_HandleTelemetry(WA_DataMessage_t* message)
{
    WAPacket_Telemetry_t* t = (WAPacket_Telemetry_t*) message->data;
    WA_ChannelID_t channel = (WA_ChannelID_t) t->channel;
    uint16_t channelMask = WirelessAudioUtilities_GetChannelBitMask(channel);

    if(channelMask)
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_INSANE, "Telemetry: af %d, dc %d, ch %d, temp %d, ss %d",
            t->ampFault,
            t->dcOffset,
            t->channel,
            t->ampTemp,
            t->serialNumberCRC);

        // pass telemetry to accessort model
        AccessoryManager_UpdateSpeakers_TL(t);
    }
    else
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_VERBOSE, "Tel - unknown channel 0x%02X", channel);
    }
}

static void WirelessAudioCUOn_HandleStartPair(void)
{
    if(WirelessAudioUtilities_IsPairingNeeded())
    {
        WirelessAudioVariant_GoToState(WA_STATE_PAIRING);
    }
    else
    {
        // We didn't listen so assert pairing as closed
        // This can happen if your skipper pairs at same time as maxwells on Bardeen
        UIPostMsg(UI_MSG_ID_LLW_PairingClosed, NOP_CALLBACK, 0);
    }
}

#if defined(BARDEEN)
static void WirelessAudioCUOn_StartGetSpeakerVersion(void)
{
    if (AccessoryManager_NumberOfWirelessSpeakersConnected())
    {
      if (UpdateModuleSpeakerPrep_IsManifestAvail())
      {
          speakerVerMsgCnt = 0;
          LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Query speakers for blob version");
          WirelessAudioTask_StartVerTimer();
          WirelessAudio_I2C_SendDataMessage(WA_BCMD_UPD_VERSION, 0, 0);
      }
    }
}

static void WirelessAudioCUOn_HandleGetSpeakerVersion(WA_DataMessage_t* message)
{
    uint8_t *spkrVerBuff;
    speakerVerMsgCnt++;
      
    LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "MU reported blob version %s", message->data);

    if (NULL == (spkrVerBuff = BufferManagerGetBuffer(WA_DATA_PKT_PAYLOAD_SIZE)))
    {
        LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Failure to obtain a buffer from Buffer manager");
        WirelessAudioTask_StopVerTimer();
        return;
    }
    memcpy(spkrVerBuff, message->data, WA_DATA_PKT_PAYLOAD_SIZE);
    UIPostMsg(UI_MSG_ID_MUReportedVer, NOP_CALLBACK, (uint32_t)spkrVerBuff);

    if (speakerVerMsgCnt >= AccessoryManager_NumberOfWirelessSpeakersConnected())
    {
        WirelessAudioTask_StopVerTimer();
    }
}
#endif
