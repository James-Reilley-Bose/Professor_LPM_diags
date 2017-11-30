/*
  File  : CECTask.c
  Title :
  Author  : jd71069
  Created : 06/24/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:  The task responsible for receiving and transmitting of CEC messages.
  It connects directly to the CEC interface which in the case of Bardeen is the STM32F0
  over an in-house i2c protocol.
==============================================================================*/
#include "project.h"
#include "CECTask.h"
#include "cec.h"
#include "TaskDefs.h"
#include "TaskManager.h"
#include "CEC_ControlInterface.h"
#include "genTimer.h"
//#include "UITask.h"
#include <stdio.h>
#include "IPCRouterTask.h"
#include "BufferManager.h"
#include "IpcTxTask.h"
#include "CEC_Utilities.h"
#include "viewNotify.h"
#include "nv_system.h"
#include "GlobalParameterList.h"
#include "KeyHandler.h"
#include "Sii9437Driver.h"
#include "SourceChangeTask.h"

#define SII_DEBUG   3

/* Logs
===============================================================================*/
SCRIBE(cec, ROTTEN_LOGLEVEL_NORMAL);

/* Global Variable
===============================================================================*/
ManagedTask* CECTaskHandle = NULL;
EventGroupHandle_t s_CECEventGroupHandle = NULL;

/* Local Defines
===============================================================================*/
/* Local Variable
===============================================================================*/
static BOOL audioStatusTimerActive = FALSE;
static TimerHandle_t CECSystemAudioTimeoutTimerHandle = NULL;
static TimerHandle_t CECAudioStatusHoldoffTimerHandle = NULL;
static const char* professorCECDisplayName = "Professor"; // Temp solution/name. Must remove

/* Local Prototypes
===============================================================================*/
static void CEC_LogicalAddressAcquired(IpcPacket_t* packet);
static void CEC_Message_Abort(CEC_t* cecMsg);
static void CEC_Message_ActiveSrc(CEC_t* cecMsg);
static void CEC_Message_FeatureAbort(CEC_t* cecMsg);
static void CEC_Message_GiveAudioStatus(CEC_t* cecMsg);
static void CEC_Message_GiveOSDName(CEC_t* cecMsg);
static void CEC_Message_GivePhysAddr(CEC_t* cecMsg);
static void CEC_Message_GivePwrStatus(CEC_t* cecMsg);
static void CEC_Message_GiveSystemAudioModeStatus(CEC_t* cecMsg);
static void CEC_Message_ReportArcInitiated(CEC_t* cecMsg);
static void CEC_Message_ReportArcTerminated(CEC_t* cecMsg);
static void CEC_Message_ReportPhysAddr(CEC_t* cecMsg);
static void CEC_Message_ReqActiveSrc(CEC_t* cecMsg);
static void CEC_Message_RequestArcInitiation(CEC_t* cecMsg);
static void CEC_Message_RequestArcTermination(CEC_t* cecMsg);
static void CEC_Message_RouteChangeRouteInfo(CEC_t* cecMsg);
static void CEC_Message_SetStrmPath(CEC_t* cecMsg);
static void CEC_Message_Standby(CEC_t* cecMsg);
static void CEC_Message_SystemAudioModeRequest(CEC_t* cecMsg);
static void CEC_Message_UserControl(CEC_t* cecMsg);
static void CEC_NewPhysAddrFromEdid(uint16_t physicalAddr);
static void CEC_GiveAudioStatusMessage(void);
static void CEC_GiveAudioStatusTimer(void);
static void CEC_InitiateARC(uint8_t addr);
static void CEC_Rx_Message(CEC_t* cecMsg);
static void CEC_Rx_Message_IPC(IpcPacket_t* ipc);
static void CEC_Rx_Message_IPC_CB(uint32_t* params, uint8_t numParams);
static void CEC_StartSystemAudioBroadcastTimer(BOOL start);
static void CEC_SystemAudioTimeoutExpired(void);
static void CEC_TerminateARC(uint8_t addr);
static void CEC_TimerInit(void);
static void CEC_Message_DeviceVendorID(CEC_t* cecMsg);
static void CEC_Message_SetOSDName(CEC_t* cecMsg);
static void CEC_RequestInfo(uint8_t logical);
static void CEC_Message_GetCECVersion(CEC_t* cecMsg);
static void CEC_Message_CEC_Version(CEC_t* cecMsg);
static void CEC_PollDevices(void);
static void CEC_Message_CEC_DeviceVendorID(CEC_t* cecMsg);
static void CEC_PurgeDeviceList(void);

/* Create CEC key producer
===============================================================================*/
CREATE_KEY_PRODUCER(kpCEC, KEY_PRODUCER_CEC, CEC_HOLD_TIME, CEC_RELEASE_TIME, CEC_REPEAT_TIME, TIMER_ID_CECRepeat, TIMER_ID_CECRelease)

/*
===============================================================================
@func CECTask
@brief Main loop for thread/task.
===============================================================================
*/
void CECTask(void* pvParamaters)
{
    debug_assert(CECTaskHandle != NULL);
    CEC_MODE temp = CEC_MODE_COMPLIANCE;
    CEC_SetMode(temp, FALSE);

    while (1)
    {
        TaskManagerPollQueue(CECTaskHandle);
    }
}

/*
===============================================================================
@func CEC_TaskInit
@brief Initialize things that can be done pre-scheduler.
===============================================================================
*/
void CEC_TaskInit(void* p)
{
    UI_ViewFunctionRegistration(CECViewNotify, USER_VOLUME_CHANGE | USER_MUTE_CHANGE);

    CEC_TimerInit();

    if (s_CECEventGroupHandle == NULL)
    {
        s_CECEventGroupHandle = xEventGroupCreate();
        debug_assert(s_CECEventGroupHandle);
    }

    CEC_SetPhysicalAddress(DEVICE_DISABLED_PHYSICAL_ADDRESS);
    CEC_SetLogicalAddress(CEC_DEVICE_UNREG_BCAST);

    CEC_SetDeviceListPhysicalAddress(0x0, 0x0000);
    CEC_SetDeviceListOSD(0x0, "TV", 2);

    for (uint8_t i = 1; i < CEC_DEVICE_NUM_DEVICES; i++)
    {
        CEC_DeviceListRemoveEntry(i);
    }

    kpCEC_init();

    CECTaskHandle = GetManagedTaskPointer("CECTask");
}

/*
===============================================================================
@func CEC_TimerInit
@brief  Initialize all necessary timers.
===============================================================================
*/
void CEC_TimerInit(void)
{
    CECSystemAudioTimeoutTimerHandle = createTimer(TIMER_MSEC_TO_TICKS(WAIT_FOR_TV_SYSTEM_AUDIO), NULL, CEC_MESSAGE_ID_SystemAudioTimeoutExpired, FALSE,
                                       tBlockIdCECSystemAudioTimeout, "CECSystemAudioTimeout", NULL);
    debug_assert(CECSystemAudioTimeoutTimerHandle != NULL);

    CECAudioStatusHoldoffTimerHandle = createTimer(TIMER_MSEC_TO_TICKS(AUDIO_STATUS_HOLDOFF), NULL, CEC_MESSAGE_ID_GiveAudioStatusTimer, FALSE,
                                       tBlockIdCECAudioStatusHoldoffTimer, "CECAudioStatusHoldoffTimer", NULL);
    debug_assert(CECAudioStatusHoldoffTimerHandle != NULL);
}

/*
===============================================================================
@func CEC_HandleMessage
@brief  Handles messages for this task.
===============================================================================
*/
void CEC_HandleMessage(GENERIC_MSG_t* message)
{
    switch (message->msgID)
    {
        case CEC_MESSAGE_ID_NewPhysAddrFromEDID:
            CEC_NewPhysAddrFromEdid(0x3000); //Remove hard coded address when able to capture EDID
            break;
        case CEC_MESSAGE_ID_InitiateARC:
            CEC_InitiateARC(message->params[0]);
            break;
        case CEC_MESSAGE_ID_TerminateARC:
            CEC_TerminateARC(message->params[0]);
            break;
        case CEC_MESSAGE_ID_EnableSystemAudio:
            CEC_SetSystemAudioPermitted(message->params[0]);
            break;
        case CEC_MESSAGE_ID_START_SystemAudioBroadcastTimer:
            CEC_StartSystemAudioBroadcastTimer(message->params[0]);
            break;
        case CEC_MESSAGE_ID_SystemAudioTimeoutExpired:
            CEC_SystemAudioTimeoutExpired();
            break;
        case CEC_MESSAGE_ID_GiveAudioStatus:
            CEC_GiveAudioStatusMessage();
            break;
        case CEC_MESSAGE_ID_GiveAudioStatusTimer:
            CEC_GiveAudioStatusTimer();
            break;
        case CEC_MESSAGE_ID_SendSystemAudioRequest:
            CEC_SendSystemAudioMode((BOOL)message->params[0]);
            break;
        case CEC_MESSAGE_ID_RX_Message:
            CEC_Rx_Message((CEC_t*)message->params[0]);
            break;
        case CEC_MESSAGE_ID_RX_Message_IPC:
            CEC_Rx_Message_IPC((IpcPacket_t*)message->params[0]);
            break;
        case CEC_MESSAGE_ID_LogicalAddressAcquired:
            CEC_LogicalAddressAcquired((IpcPacket_t*)message->params[0]);
            break;
        case CEC_MESSAGE_ID_SendRoutingChange:
            CEC_SendRoutingChange((hdmi_port)message->params[0], (hdmi_port)message->params[1]);
            break;
        case CEC_MESSAGE_ID_SendDeviceVendorID:
            CEC_DeviceVendorID();
            break;
        case CEC_MESSAGE_ID_ImageViewOn:
            CEC_ImageViewOn();
            break;
        case CEC_MESSAGE_ID_ReportPhysicalAddress:
            CEC_ReportPhysicalAddress();
            break;
        case CEC_MESSAGE_ID_SendInactiveSource:
            CEC_SendInactiveSource();
            break;
        case CEC_MESSAGE_ID_AnnounceActiveSource:
            CEC_AnnounceActiveSource();
            break;
        case CEC_MESSAGE_ID_PowerOnDevice:
            CEC_PowerOnDevice((hdmi_port)message->params[0]);
            break;
        case CEC_MESSAGE_ID_PollDevices:
            CEC_PollDevices();
            break;
        case CEC_MESSAGE_ID_SendStandby:
            CEC_SendGlobalStandby();
            break;
        default:
            LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "\tCEC\t%s was invalid message", GetEventString(message->msgID));
            break;
    }
}

static void CEC_NewPhysAddrFromEdid(uint16_t physicalAddr)
{
    // set physical address
    CEC_SetPhysicalAddress(physicalAddr);

    // attempt to acquire logical address CEC_DEVICE_AUDIO
    CEC_AcquireLogicalAddress(CEC_DEVICE_AUDIO);
}

static void CEC_LogicalAddressAcquired(IpcPacket_t* packet)
{
    uint8_t addr =  packet->s.data.b[0];
    LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "Logical address acquired: %d", addr);

    // Add ourselves to the device list since we've got an address now
    CEC_SetLogicalAddress(addr);
    CEC_SetDeviceListPhysicalAddress(addr, CEC_GetPhysicalAddress());
    CEC_SetDeviceListOSD(addr, professorCECDisplayName/*GP_CECDisplayName()*/, strlen(professorCECDisplayName));
    CEC_SetDeviceListCECVersion(addr, CEC_VERSION_14);
    CEC_SetDeviceListVendorID(addr, BOSE_IEEE_OUI_0, BOSE_IEEE_OUI_1, BOSE_IEEE_OUI_2);

    // Report our address and vendor ID
    CECPostMsg(CEC_MESSAGE_ID_ReportPhysicalAddress, NOP_CALLBACK, NULL);
    CECPostMsg(CEC_MESSAGE_ID_SendDeviceVendorID, NOP_CALLBACK, NULL);


    // Begin polling devices. (Only repeats on hotplugs and power cycles)
    //CECPostMsg(CEC_MESSAGE_ID_PollDevices, NOP_CALLBACK, NULL);

    IPCRouter_ReturnRxBuffer(packet);

    // If we missed a source switch due to no logical address, catch up.
    //SourceChangePostMsg(SOURCE_CHANGE_MESSAGE_ID_ReassertCurrentCEC);
}

/*
===============================================================================
@func CEC_PollDevices
@brief Poll all devices for presence.
===============================================================================
*/
void CEC_PollDevices(void)
{
    // Remove inative devices
    CEC_PurgeDeviceList();

    if (CEC_IsEnabledFeature(CEC_FEATURE_DEVICE_POLLING))
    {
        uint8_t msg[2] = {0, 0};
        uint8_t len = 2;

        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), 0/*Address doesn't matter, it's bulk*/);
        CEC_SetOpcode(msg, CEC_MESSAGE_GIVE_PHYS_ADDR);
        CEC_SendBulkMessage(msg, len);
        CEC_SendBulkMessage(msg, len - 1);
    }
}

/*
===============================================================================
@func CEC_PurgeDeviceList
@brief Remove inactive devices from the list.
===============================================================================
*/
void CEC_PurgeDeviceList(void)
{
    for (hdmi_port port = HDMI_PORT_START; port < HDMI_PORT_NONE; ++port)
    {
        BOOL hpd_state = FALSE;
        if (FALSE == Sii9437Driver_GetHotPlugDetected(&hpd_state) && hpd_state == FALSE)
        {
            uint8_t logical = CEC_GetDeviceListLogicalAddress(port);

            if (logical < CEC_DEVICE_NUM_DEVICES)
            {
                CEC_DeviceListRemoveEntry(logical);
            }
        }
    }
}

/*
===============================================================================
@func CEC_InitiateARC
@brief Ask the recipient at addr to initiate arc.
===============================================================================
*/
void CEC_InitiateARC(uint8_t addr)
{
    CEC_SendInitiateARC(addr);
}

/*
===============================================================================
@func CEC_TerminateARC
@brief Turn off ARC.
===============================================================================
*/
void CEC_TerminateARC(uint8_t addr)
{
    CEC_SendTerminateARC(addr);
    CEC_SetARCInitialized(FALSE);
}

/*
===============================================================================
@func CEC_START_SystemAudioBroadcastTimer
@brief  Starts the timer for the period in which the sink can <Feature Abort> our
request to turn on System Audio Control. If the TV remains silent, we're allowed to
proceed.
===============================================================================
*/
void CEC_StartSystemAudioBroadcastTimer(BOOL start)
{
    if (TRUE == start)
    {
        timerStart(CECSystemAudioTimeoutTimerHandle, 0, &CECTaskHandle->Queue);
        xEventGroupClearBits(s_CECEventGroupHandle, SYS_AUDIO_TIMER_BIT);
    }
    else
    {
        timerStop(CECSystemAudioTimeoutTimerHandle, 0);
        xEventGroupSetBits(s_CECEventGroupHandle, SYS_AUDIO_TIMER_BIT);
    }
}

/*
===============================================================================
@func CEC_SystemAudioTimeoutExpired
@brief  When the system audio broadcast timer expires, we're allowed to start
System Audio Control.
The TV didn't <Feature Abort> our directly addressed System Audio request.
===============================================================================
*/
void CEC_SystemAudioTimeoutExpired(void)
{
    CEC_SendSystemAudioModeResponse(TRUE /*ON*/);
    CEC_SetSystemAudioModeStatus(TRUE /*ON*/);
    CEC_SetWaitingForTVSysAudio(FALSE /*No longer waiting*/);
}

/*
===============================================================================
@func CEC_GiveAudioStatus
@brief  We're only allowed to send these up to 1 every 500ms. If the timer is already
running, we don't queue up the message, we just drop it.
===============================================================================
*/
void CEC_GiveAudioStatusMessage(void)
{
    if (FALSE == audioStatusTimerActive)
    {
        timerStart(CECAudioStatusHoldoffTimerHandle, 0, &CECTaskHandle->Queue);
        audioStatusTimerActive = TRUE;
    }
}

/*
===============================================================================
@func CEC_GiveAudioStatusTimer
@brief When the timer expires, we can send an audio status.
===============================================================================
*/
void CEC_GiveAudioStatusTimer(void)
{
    timerStop(CECAudioStatusHoldoffTimerHandle, 0);
    audioStatusTimerActive = FALSE;
    CEC_GiveAudioStatus(CEC_GetTVLogicalAddress());
}

/*
===============================================================================
@fn CEC_SendMessage
@brief Allocates a buffer, queues up the message for sending, release buffer.
===============================================================================
*/
void CEC_SendMessage(uint8_t* msg, uint8_t len)
{
    CEC_t cecMsg = {0};
    cecMsg.length = len;
    memcpy(&cecMsg, msg, len);
    CEC_PrintToLog(&cecMsg);
    cecMsg.length--;
    IPCRouter_Send(IPC_DEVICE_F0, IPC_CEC_MSG, NULL, &cecMsg, sizeof(CEC_t));
}


/*
===============================================================================
@fn CEC_SendMessage
@brief Allocates a buffer, queues up the message for sending, release buffer.
===============================================================================
*/
void CEC_SendBulkMessage(uint8_t* msg, uint8_t len)
{
    CEC_t cecMsg;
    cecMsg.length = len;
    memcpy(&cecMsg, msg, len);
    cecMsg.length--;
    IPCRouter_Send(IPC_DEVICE_F0, IPC_CEC_BULK_MSG, NULL, &cecMsg, sizeof(CEC_t));
}
/*
===============================================================================
@func CEC_Rx_Message_IPC
@brief Receive the CEC messages from the STM32F0 via IPC
===============================================================================
*/
void CEC_Rx_Message_IPC(IpcPacket_t* ipc)
{
    CEC_t* cecMsg;
    cecMsg = (CEC_t*)ipc->s.data.b;
    //todo: remove off by one nonsense
    cecMsg->length++;
    if (cecMsg->length < CEC_MSG_MAX_LEN_BYTES)
    {
        // NULL out extra bytes
        uint8_t* endOfMsg = (uint8_t*)cecMsg;
        endOfMsg += cecMsg->length;
        memset(endOfMsg, 0, (CEC_MSG_MAX_LEN_BYTES - cecMsg->length));
    }
    CECPostMsg(CEC_MESSAGE_ID_RX_Message, CEC_Rx_Message_IPC_CB, (uint32_t)cecMsg, (uint32_t)ipc);
}

/*
===============================================================================
@func CEC_Rx_Message_IPC_CB
@brief Callback to free the buffer malloc'd in the message call
===============================================================================
*/
void CEC_Rx_Message_IPC_CB(uint32_t* params, uint8_t numParams)
{
    IPCRouter_ReturnRxBuffer((IpcPacket_t*)params[1]);
}

/*
===============================================================================
@func CEC_Rx_Message
@brief Main entry point for a CEC message. This decides how it is handled.
===============================================================================
*/
void CEC_Rx_Message(CEC_t* cecMsg)
{
    // minimal cec funcationlity?
    if (CEC_IsEnabledMinimalFunction() == FALSE)
    {
        return;
    }

    if (cecMsg->length > CEC_MSG_MAX_LEN_BYTES)
    {
        LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "Oversized CEC message (%d bytes - %02x %02x)", cecMsg->length,
            ((uint8_t*)cecMsg)[0], ((uint8_t*)cecMsg)[1]);
        return;
    }

    CEC_PrintToLog(cecMsg);

    // polling mesage, ignore
    if (cecMsg->length == 1)
    {
        if (cecMsg->initiator == CEC_GetLogicalAddress())
        {
            CEC_RequestInfo(cecMsg->destination);
        }
        return;
    }

    LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "CEC recv. Opcode = %x" , cecMsg->payload.opcode);
    switch (cecMsg->payload.opcode)
    {
        case CEC_MESSAGE_REPORT_PHYS_ADDR:
            CEC_Message_ReportPhysAddr(cecMsg);
            break;
        case CEC_MESSAGE_GIVE_PHYS_ADDR:
            CEC_Message_GivePhysAddr(cecMsg);
            break;
        case CEC_MESSAGE_INACTIVE_SOURCE:  // some devices do this randomly so reassert active
            CEC_AnnounceActiveSource(); // is safe since we will only send if we are active
            break;
        case CEC_MESSAGE_REQ_ACTIVE_SRC:
            CEC_Message_ReqActiveSrc(cecMsg);
            break;
        case CEC_MESSAGE_SET_STRM_PATH:  // treat like active source.
            CEC_Message_SetStrmPath(cecMsg);
            break;
        case CEC_MESSAGE_ACTIVE_SRC:
            CEC_Message_ActiveSrc(cecMsg);
            break;
        case CEC_MESSAGE_ROUTE_CHANGE:
        case CEC_MESSAGE_ROUTE_INFO:
            CEC_Message_RouteChangeRouteInfo(cecMsg);
            break;
        case CEC_MESSAGE_GIVE_AUDIO_STATUS:
            CEC_Message_GiveAudioStatus(cecMsg);
            break;
        case CEC_MESSAGE_GIVE_SYSTEM_AUDIO_MODE_STATUS:
            CEC_Message_GiveSystemAudioModeStatus(cecMsg);
            break;
        case CEC_MESSAGE_USER_CONTROL_PRESSED:
        case CEC_MESSAGE_USER_CONTROL_RELEASED:
            CEC_Message_UserControl(cecMsg);
            break;
        case CEC_MESSAGE_SYSTEM_AUDIO_MODE_REQUEST:
            CEC_Message_SystemAudioModeRequest(cecMsg);
            break;
        case CEC_MESSAGE_REQUEST_ARC_INITIATION:
            CEC_Message_RequestArcInitiation(cecMsg);
            break;
        case CEC_MESSAGE_REQUEST_ARC_TERMINATION:
            CEC_Message_RequestArcTermination(cecMsg);
            break;
        case CEC_MESSAGE_REPORT_ARC_INITIATED:
            CEC_Message_ReportArcInitiated(cecMsg);
            break;
        case CEC_MESSAGE_REPORT_ARC_TERMINATED:
            CEC_Message_ReportArcTerminated(cecMsg);
            break;
        case CEC_MESSAGE_STANDBY:
            CEC_Message_Standby(cecMsg);
            break;
        case CEC_MESSAGE_GIVE_PWR_STATUS:
            CEC_Message_GivePwrStatus(cecMsg);
            break;
        case CEC_MESSAGE_GIVE_OSD_NAME:
            CEC_Message_GiveOSDName(cecMsg);
            break;
        case CEC_MESSAGE_ABORT:
            CEC_Message_Abort(cecMsg);
            break;
        case CEC_MESSAGE_FEATURE_ABORT:
            CEC_Message_FeatureAbort(cecMsg);
            break;
        case CEC_MESSAGE_GET_CEC_VERSION:
            CEC_Message_GetCECVersion(cecMsg);
            break;
        case CEC_MESSAGE_CEC_VERSION:
            CEC_Message_CEC_Version(cecMsg);
            break;
        case CEC_MESSAGE_GET_VENDOR_ID:
            CEC_Message_DeviceVendorID(cecMsg);
            break;
        case CEC_MESSAGE_DEVICE_VENDOR_ID:
            CEC_Message_CEC_DeviceVendorID(cecMsg);
            break;
        case CEC_MESSAGE_SET_OSD_NAME:
            CEC_Message_SetOSDName(cecMsg);
            break;
        default:
            break;
    }
}

/*
===============================================================================
@func CEC_Message_ReportPhysAddr
@brief When we receive a physical address report (from anyone) we log that data
    for things like remote control pass through or device power on.
===============================================================================
*/
static void CEC_Message_ReportPhysAddr(CEC_t* cecMsg)
{
    // I got a message { 0F 84 40 00 04 } from a rogue samsung tv that screwed up device list
    // So we always drop physical address reports from TV
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->initiator != CEC_DEVICE_TV)
    {
        uint16_t addr = CEC_GetPhyAddrInMessage((uint8_t*) cecMsg, ADDR1_INDEX);
        hdmi_port currentPort = SourceInterface_GetHDMIPortFromSourceID(SourceChangeAPI_GetNextSource());

        CEC_SetDeviceListPhysicalAddress(cecMsg->initiator, addr);

        // If the port we are on reports physical address late after a purge
        // we can try to power it on again since we failed to do so before
        if(currentPort != HDMI_PORT_NONE && CEC_GetPortFromPhyAddress(addr) == currentPort)
        {
            CECPostMsg(CEC_MESSAGE_ID_PowerOnDevice, NOP_CALLBACK, currentPort);
        }

    }
}

/*
===============================================================================
@func CEC_Message_GivePhysAddr
@brief Respond to a query (to us) for our physical address report.
===============================================================================
*/
static void CEC_Message_GivePhysAddr(CEC_t* cecMsg)
{
    LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "CEC physical address requested. 0x84");
    if (cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/)
    {
        CEC_ReportPhysicalAddress();
    }
}

/*
===============================================================================
@func CEC_Message_ReqActiveSrc
@brief Respond to a global query for the current active source. If we're the active
source, we respond with an <Active Source> message with our physical address.
Must be 2 bytes.
===============================================================================
*/
static void CEC_Message_ReqActiveSrc(CEC_t* cecMsg)
{
    SOURCE_ID actualSource = SourceChangeAPI_GetCurrentSource();
    SOURCE_ID nextSource = SourceChangeAPI_GetNextSource();
    // Some (not very many) TVs send this and then do bad things when no one replies. So we reply.
    // PAELLA-6512
    if (cecMsg->destination == CEC_DEVICE_UNREG_BCAST && cecMsg->initiator != CEC_GetLogicalAddress() && (cecMsg->length == 2) && !SourceInterface_IsTVSource(SourceChangeAPI_GetCurrentSource()))
    {
        CEC_AnnounceActiveSource();
    }
}

/*
===============================================================================
@func CEC_Message_SetStrmPath
@brief The TV has directed us to switch to a particular video path. Must be to
the broadcast address and must be 4 bytes.
===============================================================================
*/
static void CEC_Message_SetStrmPath(CEC_t* cecMsg)
{
    SOURCE_ID actualSource = SourceChangeAPI_GetCurrentSource();
    if (cecMsg->destination == CEC_DEVICE_UNREG_BCAST && (cecMsg->length == 4) /*SrcDst Op AA AA*/)
    {
        if (SourceInterface_IsHDMISource(actualSource) || SourceInterface_IsInternalSource(actualSource) || SourceInterface_IsStandbySource(actualSource) || SourceInterface_IsTVSource(actualSource))
        {
            if (CEC_GetPhyAddrInMessage((uint8_t*)cecMsg,  ADDR1_INDEX) == CEC_GetPhysicalAddress())
            {
                CEC_AnnounceActiveSource();
            }

            CEC_HandleActiveSourceSetStrmPath(CEC_GetPhyAddrInMessage((uint8_t*)cecMsg, ADDR1_INDEX));
        }
    }
}

/*
===============================================================================
@func CEC_Message_ActiveSrc
@brief Another source on the CEC network wishes to show video. Switch to the
corresponding HDMI RX port OR to TV source if the source is "downstream".
Must be 4 bytes.
===============================================================================
*/
static void CEC_Message_ActiveSrc(CEC_t* cecMsg)
{
    SOURCE_ID actualSource = SourceChangeAPI_GetCurrentSource();
    SOURCE_ID desiredSource = SourceChangeAPI_GetNextSource();
    if (cecMsg->initiator != CEC_GetLogicalAddress() &&  cecMsg->destination == CEC_DEVICE_UNREG_BCAST && (cecMsg->length == 4) /*SrcDst Op AA AA*/)
    {
        if (SourceInterface_IsHDMISource(actualSource) || SourceInterface_IsInternalSource(actualSource) || SourceInterface_IsStandbySource(actualSource) || SourceInterface_IsTVSource(actualSource))
        {
            CEC_HandleActiveSourceSetStrmPath(CEC_GetPhyAddrInMessage((uint8_t*)cecMsg, ADDR1_INDEX));
        }
    }
}

/*
===============================================================================
@func CEC_Message_RouteChange
@brief A switch downstream of us (mostly the TV) has indicated a port switch.
If we are in the path, we switch to our first RX port. Must be 6 bytes.
===============================================================================
*/
static void CEC_Message_RouteChangeRouteInfo(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_DEVICE_UNREG_BCAST
            && (cecMsg->length == ROUTE_CHANGE_LEN || cecMsg->length == ROUTE_INFO_LEN) /*SrcDst Op AA AA | AA AA*/ )
    {
        SOURCE_ID current_source = SourceChangeAPI_GetCurrentSource();
        if (SourceInterface_IsStandbySource(current_source) || SourceInterface_IsHDMISource(current_source) || SourceInterface_IsInternalSource(current_source) || SourceInterface_IsTVSource(current_source))
        {
            CEC_HandleRouteChangeRouteInfo(cecMsg);
        }
    }
}


/*
===============================================================================
@func CEC_Message_GiveAudioStatus
@brief A device has asked us for our volume and mute status. Must be 2 bytes.
===============================================================================
*/
static void CEC_Message_GiveAudioStatus(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/ &&
            (cecMsg->length == 2) /*SrcDst Op*/)
    {
        CECPostMsg(CEC_MESSAGE_ID_GiveAudioStatus, NOP_CALLBACK, 0);
    }
}

/*
===============================================================================
@func CEC_Message_GiveSystemAudioModeStatus
@brief A device wants to know if System Audio is on or off. Must be 2 bytes.
===============================================================================
*/
static void CEC_Message_GiveSystemAudioModeStatus(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() &&  cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/ &&
            (cecMsg->length == 2) /*SrcDst Op*/)
    {
        CEC_ReportSystemAudioModeStatus(cecMsg->initiator);
    }
}

/*
===============================================================================
@func CEC_Message_UserControl
@brief The user is using a different remote. Typically just volume and mute, maybe more.
===============================================================================
*/
static void CEC_Message_UserControl(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress())
    {
        SOURCE_ID actualSource = SourceChangeAPI_GetCurrentSource();
        SOURCE_ID desiredSource = SourceChangeAPI_GetNextSource();
        // We don't want to accept CEC user controls if we're not in an HDMI source. --JD71069
        // Also, TV source. We need to take (at least) volume and mute when in TV mode. --JD71069
        if (SourceInterface_IsHDMISource(actualSource) || SourceInterface_IsInternalSource(actualSource) || SourceInterface_IsTVSource(actualSource))
        {
            if (cecMsg->payload.data[0] == CEC_MESSAGE_USER_CONTROL_PRESSED)
            {
                HandleKey(BOSE_KEY_CEC_MAPPING[cecMsg->payload.data[1]], &kpCEC);
            }
            else
            {
                HandleKeyRelease(BOSE_KEY_CEC_MAPPING[cecMsg->payload.data[1]], &kpCEC);
            }
        }
    }
}

/*
===============================================================================
@func CEC_Message_SystemAudioModeRequest
@brief This is the most compliacted CEC message.

If the length is 2, and it's sent directly to us, it means: Turn off System Audio.
If the length is 4, first we want to make sure we're not headed to Standby (Thanks Toshiba)
  if we *ARE* headed to standby, <Feature Abort> because we want to go to standby.
  if we *ARE NOT* headed to standby, and we're in an HDMI source (or internal),
  then, the initiator must be thet TV. After that we are then clear to
  broadcast <System Audio Mode> [ON]. If it came from someone else,  then we initiate
  system audio from our side (check then broadcast method).

Finally, the request contains a physical address, we switch to it.
===============================================================================
*/
static void CEC_Message_SystemAudioModeRequest(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/)
    {
        if (2 == cecMsg->length)
        {
            CEC_SendSystemAudioModeResponse(FALSE /*OFF*/);
            CEC_SetSystemAudioModeStatus(FALSE /*OFF*/);
        }
        else if (4 == cecMsg->length)
        {
            CEC_HandleSystemAudioModeRequest(cecMsg);
        }
    }
}

/*
===============================================================================
@func CEC_Message_RequestArcInitiation
@brief Another device wants us to render ARC. If the device is adjacent, we can
respond with a <Initialize ARC>
===============================================================================
*/
static void CEC_Message_RequestArcInitiation(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/)
    {
        if (TRUE /*== CEC_CanWeARC(CEC_GetPhysicalAddress(), CEC_GetDeviceListPhysicalAddress(cecMsg->initiator))*/)
        {
            CECPostMsg(CEC_MESSAGE_ID_InitiateARC, NOP_CALLBACK, 0);
        }
        else
        {
            LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "Can't ARC! Us = %d, Them = %d", CEC_GetPhysicalAddress(), CEC_GetDeviceListPhysicalAddress(cecMsg->initiator));
            CEC_SendFeatureAbort(CEC_MESSAGE_REQUEST_ARC_INITIATION, CEC_ABRT_REFUSED, cecMsg->initiator);
        }
    }
}

/*
===============================================================================
@func CEC_Message_RequestArcTermination
@brief The device who is sending us ARC wants us to send <Terminate ARC>
===============================================================================
*/
static void CEC_Message_RequestArcTermination(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/)
    {
        CECPostMsg(CEC_MESSAGE_ID_TerminateARC, NOP_CALLBACK, 0);  // No advantage to telling the TV we've turned off ARC.
    }
}

/*
===============================================================================
@func CEC_Message_ReportArcInitiated
@brief If the adjacent device replies poisitvely to our <Initiate ARC>, we change
our internal state to ARC on.
===============================================================================
*/
static void CEC_Message_ReportArcInitiated(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/)
    {
        CEC_SetARCInitialized(TRUE);
        //HDMIPostMsg(HDMI_MESSAGE_ID_SetAudioInput, NOP_CALLBACK, HDMI_PRIMARY_INSTANCE_ID, HDMI_AUDIO_INPUT_ARC); TODO: Bring in later..
    }
}

/*
===============================================================================
@func CEC_Message_ReportArcTerminated
@brief When the adjacent device replies to our <Terminate ARC>, we change
our internal state to ARC off.
===============================================================================
*/
static void CEC_Message_ReportArcTerminated(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/)
    {
        CEC_SetARCInitialized(FALSE);
    }
}

/*
===============================================================================
@func CEC_Message_Standby
@brief Go to standby.
===============================================================================
*/
static void CEC_Message_Standby(CEC_t* cecMsg)
{
    if (cecMsg->destination == CEC_GetLogicalAddress() || cecMsg->destination == CEC_DEVICE_UNREG_BCAST)
    {
        // Don't want to follow a <Standby> message if we're in an internal audio source.
        if (!SourceInterface_IsInternalSource(SourceChangeAPI_GetCurrentSource()) || SourceInterface_IsUnifySource(SourceChangeAPI_GetCurrentSource()))
        {
            CEC_GoToStandby();
        }
    }
}

/*
===============================================================================
@func CEC_Message_GivePwrStatus
@brief A device asks for our power status.
===============================================================================
*/
static void CEC_Message_GivePwrStatus(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/)
    {
        CEC_GivePowerStatus(cecMsg->initiator);
    }
}

/*
===============================================================================
@func CEC_Message_GiveOSDName
@brief Report our OSD.
===============================================================================
*/
static void CEC_Message_GiveOSDName(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/
            && cecMsg->initiator != CEC_DEVICE_UNREG_BCAST /*And don't accept from Unregistered Address*/)
    {
        const char* osd = GP_CECDisplayName();
        debug_assert(strlen(osd) <= CEC_MAX_OSD_LEN);

        if (strlen(osd) == 0)
        {
            CEC_SendFeatureAbort(CEC_MESSAGE_GIVE_OSD_NAME, CEC_ABRT_REFUSED, cecMsg->initiator);
        }
        else
        {
            CEC_SendOSDName(cecMsg->initiator, osd, strlen(osd));
        }
    }
}

/*
===============================================================================
@func CEC_Message_Abort
@brief <Abort> is used for test only.
===============================================================================
*/
static void CEC_Message_Abort(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress() /*Respond only if we are the DST*/
            && cecMsg->initiator != CEC_DEVICE_UNREG_BCAST /*And don't accept from Unregistered Address*/)
    {
        CEC_SendFeatureAbort(CEC_MESSAGE_ABORT, CEC_ABRT_REFUSED, cecMsg->initiator);
    }
}

/*
===============================================================================
@func CEC_Message_FeatureAbort
@brief The one time we care about a <Feature Abort> is when we're waiting for the
TV to not send one when starting up System Audio mode.
===============================================================================
*/
static void CEC_Message_FeatureAbort(CEC_t* cecMsg)
{
    if (cecMsg->destination == CEC_GetLogicalAddress())
    {
        // If the TV sends a <Feature Abort> while we're trying to turn on System Audio, we're supposed to quit.
        if (CEC_MESSAGE_SET_SYSTEM_AUDIO_MODE == cecMsg->payload.params[0] && TRUE == CEC_GetWaitingForTVSysAudio())
        {
            CECPostMsg(CEC_MESSAGE_ID_START_SystemAudioBroadcastTimer, NOP_CALLBACK, FALSE);
            CEC_SetWaitingForTVSysAudio(FALSE /*No longer waiting*/);
        }

    }
}

/*
===============================================================================
@func CEC_Message_DeviceVendorID
@brief A device wants to know our Vendor ID (IEEE OUI:08DF1F)
===============================================================================
*/
static void CEC_Message_DeviceVendorID(CEC_t* cecMsg)
{
    if (cecMsg->initiator != CEC_GetLogicalAddress() && cecMsg->destination == CEC_GetLogicalAddress() && CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress())
    {
        CEC_DeviceVendorID();
    }
}

/*
===============================================================================
@func CEC_Message_SetOSDName
@brief A device reports its OSD name. We snoop that.
===============================================================================
*/
static void CEC_Message_SetOSDName(CEC_t* cecMsg)
{
    // You can't really have an OSD name if you're unregistered
    if (cecMsg->initiator != CEC_DEVICE_UNREG_BCAST)
    {
        CEC_SetDeviceListOSD(cecMsg->initiator, (char*) &cecMsg->payload.params[0], cecMsg->length - 2 /*OSD is what's left after address and opcode are gone.*/);
    }
}

/*
===============================================================================
@func CEC_RequestInfo
@brief A bundle of requests for a device that gave a physical address report.
===============================================================================
*/
static void CEC_RequestInfo(uint8_t logical)
{
    CEC_GivePhysicalAddress(logical);
    CEC_GiveOSDName(logical);
    CEC_GetCECVersion(logical);
    CEC_GetDeviceVendorID(logical);
}

/*
===============================================================================
@func CEC_Message_CEC_Version
@brief A device reports its CEC version. We snoop that.
===============================================================================
*/
static void CEC_Message_CEC_Version(CEC_t* cecMsg)
{
    // You can't really have a CEC version if you're unregistered
    if (cecMsg->initiator != CEC_DEVICE_UNREG_BCAST)
    {
        CEC_SetDeviceListCECVersion(cecMsg->initiator, (CEC_VERSION) cecMsg->payload.params[0]);
    }
}


/*
===============================================================================
@func CEC_Message_GetCECVersion
@brief A device reports its CEC version. We snoop that.
===============================================================================
*/
static void CEC_Message_GetCECVersion(CEC_t* cecMsg)
{
    if (cecMsg->destination == CEC_GetLogicalAddress())
    {
        CEC_CECVersion(cecMsg->initiator);
    }
}


/*
===============================================================================
@func CEC_Message_CEC_DeviceVendorID
@brief
===============================================================================
*/
static void CEC_Message_CEC_DeviceVendorID(CEC_t* cecMsg)
{
    // You can't really have a vendor ID if you're unregistered
    if (cecMsg->initiator != CEC_DEVICE_UNREG_BCAST)
    {
        CEC_SetDeviceListVendorID(cecMsg->initiator, cecMsg->payload.params[0], cecMsg->payload.params[1], cecMsg->payload.params[2]);
    }

}

/*
===============================================================================
@func CECViewNotify
@brief When the user volume changes, send <Report Audio Status> to the TV.
The CEC View is registered for changes in USER_MUTE_CHANGE and USER_VOLUME_CHANGE
===============================================================================
*/
void CECViewNotify(uint32_t notificationEvent)
{
    if (notificationEvent & (USER_MUTE_CHANGE | USER_VOLUME_CHANGE))
    {
        CECPostMsg(CEC_MESSAGE_ID_GiveAudioStatus, NOP_CALLBACK, 0);
    }
}
