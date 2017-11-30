//
// WirelessAudioMUOn.c
//

#include "WirelessAudioVariant.h"
#include "WirelessAudioUtilities.h"
#include "WirelessAudioTask.h"
#include "UITask.h"
#include "viewNotify.h"
#include "nv_mfg.h"
#include "nv_mfg_struct.h"
#include "WirelessAudioUpdateTransfer.h"

SCRIBE_DECL(wa_task);

#define WA_MU_ON_NETWORK_STATUS_PERIOD TIMER_MSEC_TO_TICKS(1100)
#define WA_MU_ON_NO_LINK_DISCONNECT_COUNT 3
#define WA_MU_ON_ENTER_PAIRING_HOLDOFF_MS 12000

typedef enum
{
    WA_MU_DISCONNECTED,
    WA_MU_WAIT_ID_ACK,
    WA_MU_CONNECTED,

    WA_MU_NUM_SUBSTATES
} WA_MUSubstates_t;

static void WirelessAudioMUOn_HandleTimer(void);
static void WirelessAudioMUOn_MonitorNetwork(void);
static void WirelessAudioMUOn_HandleEventOccurred(void);
static void WirelessAudioMUOn_ProcessDataMessage(WA_DataMessage_t* message);
static void WirelessAudioMUOn_HandleIDAck(WA_DataMessage_t* message);
static void WirelessAudioMUOn_HandleSNAck(WA_DataMessage_t* message);
static void WirelessAudioMUOn_HandleUpdateMessage(WA_DataMessage_t* message);
static void WirelessAudioMUOn_HandleNetworkDisconnected(void);

static WA_MUSubstates_t substate = WA_MU_DISCONNECTED;
static uint16_t noLinkCount = 0;

#define COLDBOOT_MAYBE_PAIR   (1<<0)
#define COLDBOOT_UPD_UI       (1<<1)
#define COLDBOOT_UPD_DARR_FW  (1<<2)
static uint8_t doOnceAtColdboot = (COLDBOOT_UPD_DARR_FW | COLDBOOT_MAYBE_PAIR | COLDBOOT_UPD_UI);

void WirelessAudioMUOn_EnterState(void)
{
    if(WirelessAudioUtilities_WasDarrQuickBooted()) return;

    WirelessAudioTask_ChangeTimerPeriod(WA_MU_ON_NETWORK_STATUS_PERIOD);
    WirelessAudioTask_StartTimer();
    substate = WA_MU_DISCONNECTED;
    noLinkCount = 0;
}

void WirelessAudioMUOn_ExitState(void)
{
    WirelessAudioTask_StopTimer();
}

BOOL WirelessAudioMUOn_HandleMessage(GENERIC_MSG_t* msg)
{
    BOOL handled = TRUE;
    switch(msg->msgID)
    {
        case WA_MSG_ID_TimerExpired:
            WirelessAudioMUOn_HandleTimer();
            break;
        case WA_MSG_ID_HandleEXTI:
            WirelessAudioMUOn_HandleEventOccurred();
            break;
        default:
            handled = FALSE;
            break;
    }
    return handled;
}

static void WirelessAudioMUOn_HandleTimer(void)
{
    if((doOnceAtColdboot & COLDBOOT_MAYBE_PAIR) &&
       !WirelessAudioVariantUtils_GetSkipPair() &&
       (GET_SYSTEM_UPTIME_MS() >= WA_MU_ON_ENTER_PAIRING_HOLDOFF_MS))
    {        
        doOnceAtColdboot &= ~(COLDBOOT_MAYBE_PAIR);
        if(WirelessAudioUtilities_DoesRegionAllowPair())
        {
            WirelessAudioVariant_GoToState(WA_STATE_PAIRING);
        }
    }
    else
    {
        WirelessAudioUtilities_PeriodicService();
        WirelessAudioMUOn_MonitorNetwork();
        WirelessAudioTask_StartTimer();
    }
}

static void WirelessAudioMUOn_HandleNetworkDisconnected(void)
{
    WirelessAudioUtilities_UpdateCoco();
    if(WirelessAudioUtilities_IsPreviouslyPaired())
    {
        WirelessAudioUtilities_SendSN();
    }
}

static void WirelessAudioMUOn_MonitorNetwork(void)
{
    if((doOnceAtColdboot & COLDBOOT_UPD_DARR_FW) && WirelessAudioVariantUtils_GetPostDarrUpdate())
    {
        // Do the damn thing
        waTransfer_DoWAMUpdate(FALSE);
        WirelessAudioVariantUtils_SetPostDarrUpdate(FALSE);
    }
    
    doOnceAtColdboot &= ~(COLDBOOT_UPD_DARR_FW);
  
    if(!WirelessAudioUtilities_IsLinked())
    {
        if(((substate != WA_MU_DISCONNECTED) &&
           (++noLinkCount >= WA_MU_ON_NO_LINK_DISCONNECT_COUNT)) || 
           (doOnceAtColdboot & COLDBOOT_UPD_UI)) // First boot after update we need to do this since we skip pair and start disconected
        {
            doOnceAtColdboot &= ~(COLDBOOT_UPD_UI);
            substate = WA_MU_DISCONNECTED;
            UIPostMsg(UI_LLW_NETWORK_STATUS_CHANGED, NULL, (uint32_t) FALSE);
            UI_ViewNotify(ACCESSORY_STATE_CHANGED);
        }
        return;
    }

    noLinkCount = 0;

    switch(substate)
    {
        case WA_MU_DISCONNECTED:
            WirelessAudioMUOn_HandleNetworkDisconnected();
            break;
        case WA_MU_WAIT_ID_ACK:
            WirelessAudioUtilities_SendID();
            break;
        case WA_MU_CONNECTED:
            WirelessAudioUtilities_SendTelemetry();
            break;
    }
}

static void WirelessAudioMUOn_HandleEventOccurred(void)
{
    WirelessAudioUtilities_ReadAllDataMessages(WirelessAudioMUOn_ProcessDataMessage);
}

static void WirelessAudioMUOn_ProcessDataMessage(WA_DataMessage_t* message)
{
    switch(message->opcode)
    {
        case WA_BCMD_ID_ACK:
            WirelessAudioMUOn_HandleIDAck(message);
            break;
        case WA_BCMD_SERIAL_NO:
            WirelessAudioMUOn_HandleSNAck(message);
            break;
        case WA_BCMD_UPD_VERSION:
        case WA_BCMD_ENTER_UPDATE:
            WirelessAudioMUOn_HandleUpdateMessage(message);
            break;
        case WA_BCMD_REBOOT_MU:
            WirelessAudioVariantUtils_Reboot();
            break;
        default:
            LOG(wa_task, ROTTEN_LOGLEVEL_NORMAL, "Dropping packet 0x%02X", message->opcode);
            break;
    }
}

static void WirelessAudioMUOn_HandleIDAck(WA_DataMessage_t* message)
{
    if(substate != WA_MU_WAIT_ID_ACK) return;

    WAPacket_IDAck_t* ack = (WAPacket_IDAck_t*) message->data;
    if(ack->channel == WirelessAudioUtilities_GetChannel())
    {
        substate = WA_MU_CONNECTED;
        UIPostMsg(UI_LLW_NETWORK_STATUS_CHANGED, NULL, (uint32_t) TRUE);
        UI_ViewNotify(ACCESSORY_STATE_CHANGED);
    }
}

static void WirelessAudioMUOn_HandleSNAck(WA_DataMessage_t* message)
{
    if(substate != WA_MU_DISCONNECTED) return;

    WAPacket_SerialNum_t* sn = (WAPacket_SerialNum_t*) message->data;
    uint8_t mySN[SERIAL_NO_LEN] = {0};
    NV_GetSystemSerialNo(mySN, SERIAL_NO_LEN, 0);
    if((sn->channel == WirelessAudioUtilities_GetChannel()) &&
       (memcmp(sn->serialNumber, mySN, SERIAL_NO_LEN) == 0))
    {
        // Dont send disconnect to UI or enter pairing if we get ack
        doOnceAtColdboot &= ~(COLDBOOT_MAYBE_PAIR | COLDBOOT_UPD_UI);
        WirelessAudioVariantUtils_SetSkipPair(FALSE);  
        substate = WA_MU_WAIT_ID_ACK;
        WirelessAudioUtilities_SendID();
    }
}

BOOL WirelessAudioMUOn_GetCachedConnectionStatus(void)
{
    return (substate == WA_MU_CONNECTED);
}

static void WirelessAudioMUOn_HandleUpdateMessage(WA_DataMessage_t* message)
{
    waTransfer_HandleDarrMessage(message);
    if(waGetUpdating()) WirelessAudioVariant_GoToState(WA_STATE_UPDATE_SPEAKERS);
}
