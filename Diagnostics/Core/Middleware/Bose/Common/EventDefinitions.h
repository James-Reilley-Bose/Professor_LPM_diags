#ifndef EVENTDEFINITIONS_H
#define EVENTDEFINITIONS_H

#include "EnumGenerator.h"
#include "VariantEvents.h"

//Note: the ranges below are exclusive and must start at index 0 in order for string table to be correct
//      **ALSO - remember that TAP provides a facility to post this event for test purposes - if doing that
//               would leave the system in a bad way, you need to update EventShouldNotBePostedViaTAP() in
//               SystemEventsTAP.c to prevent that from happening...

#define FOREACH_CORE_EVENT(EVENT) \
        EVENT(POWER_MESSAGE_ID_Start)  \
        EVENT(POWER_MESSAGE_ID_TransitionColdBootToLowPower)  \
        EVENT(POWER_MESSAGE_ID_TransitionLowPowerToNetworkStandby)  \
        EVENT(POWER_MESSAGE_ID_TransitionNetworkStandbyToAutoWakeStandby)  \
        EVENT(POWER_MESSAGE_ID_TransitionFullPowerToAutoWakeStandby)  \
        EVENT(POWER_MESSAGE_ID_TransitionAutoWakeStandbyToNetworkStandby)  \
        EVENT(POWER_MESSAGE_ID_TransitionAutoWakeStandbyToFullPower)  \
        EVENT(POWER_MESSAGE_ID_TransitionNetworkStandbyToLowPower)  \
        EVENT(POWER_MESSAGE_ID_PowerTransitionComplete)  \
        EVENT(POWER_MESSAGE_ID_PeriodicThermsRead)\
        EVENT(POWER_MESSAGE_ID_End)\
\
        EVENT(SYSTEM_MESSAGE_ID_SystemStateTimerExpired) \
        EVENT(SYSTEM_MESSAGE_ID_SystemStateSet) \
\
        EVENT(TAPPARSER_MESSAGE_ID_Start)  \
        EVENT(TAPPARSER_MESSAGE_ID_WaitTimerExpired)  \
        EVENT(TAPPARSER_MESSAGE_ID_ProcessCommand)  \
        EVENT(TAPPARSER_MESSAGE_ID_TapKeyPressAndHold) \
        EVENT(TAPPARSER_MESSAGE_ID_PassThroughDone) \
        EVENT(TAPPARSER_MESSAGE_ID_End)  \
\
        EVENT(TAPLISTENER_MESSAGE_ID_Start)  \
        EVENT(TAPLISTENER_MESSAGE_ID_Print)  \
        EVENT(TAPLISTENER_MESSAGE_ID_PrintBuffer)  \
        EVENT(TAPLISTENER_MESSAGE_ID_OverrunError)  \
        EVENT(TAPLISTENER_MESSAGE_ID_End)  \
\
        EVENT(KEYHANDLER_MESSAGE_ID_Start) \
        EVENT(KEYHANDLER_MESSAGE_ID_KeyPress) \
        EVENT(KEYHANDLER_MESSAGE_ID_KeyRelease) \
        EVENT(KEYHANDLER_MESSAGE_ID_PrintKeyList) \
        EVENT(KEYHANDLER_MESSAGE_ID_ResetKeyList) \
        EVENT(KEYHANDLER_MESSAGE_ID_CK_Timer_KeyScan) \
        EVENT(KEYHANDLER_MESSAGE_ID_CK_Timer_KeyDebounce) \
        EVENT(KEYHANDLER_MESSAGE_ID_CK_Timer_KeyVulcan) \
        EVENT(KEYHANDLER_MESSAGE_ID_End) \
\
        EVENT(GENERIC_TIMER_MESSAGE_ID_Start) \
        EVENT(GENERIC_TIMER_MESSAGE_ID_End) \
\
        EVENT(IPCROUTER_MESSAGE_ID_Start) \
        EVENT(IPCROUTER_MESSAGE_ID_RxMessage) \
        EVENT(IPCROUTER_MESSAGE_ID_BeginRTWTest) \
        EVENT(IPCROUTER_MESSAGE_ID_EndRTWTest) \
        EVENT(IPCROUTER_MESSAGE_ID_BulkTimerExpired) \
        EVENT(IPCROUTER_MESSAGE_ID_BulkTransferComplete) \
        EVENT(IPCROUTER_MESSAGE_ID_BulkTransferError) \
        EVENT(IPCROUTER_MESSAGE_ID_BulkTransferRequest) \
        EVENT(IPCROUTER_MESSAGE_ID_BulkTransferAcknowledge) \
        EVENT(IPCROUTER_MESSAGE_ID_BulkTransferFullBuffer) \
        EVENT(IPCROUTER_MESSAGE_ID_BulkTransferAbort) \
        EVENT(IPCROUTER_MESSAGE_ID_BulkTransferReturnBuffer) \
        EVENT(IPCROUTER_MESSAGE_ID_PassThroughTimerExpired) \
        EVENT(IPCROUTER_MESSAGE_ID_End) \
\
        EVENT(IPC_TX_MESSAGE_ID_Start) \
        EVENT(IPC_TX_MESSAGE_ID_Send) \
        EVENT(IPC_TX_MESSAGE_ID_Hammer) \
        EVENT(IPC_TX_MESSAGE_ID_Cancel) \
        EVENT(IPC_I2C_MESSAGE_ID_Receive) \
        EVENT(IPC_TX_MESSAGE_ID_End) \
\
        EVENT(DEVICE_MODEL_MESSAGE_ID_Start) \
        EVENT(DEVICE_MODEL_MESSAGE_ID_WatchdogExpired) \
        EVENT(DEVICE_MODEL_MESSAGE_ID_MessageRecieved) \
        EVENT(DEVICE_MODEL_MESSAGE_ID_EnterStandby) \
        EVENT(DEVICE_MODEL_MESSAGE_ID_Reboot) \
        EVENT(DEVICE_MODEL_MESSAGE_ID_SendLPMHealthAndStatus) \
        EVENT(DEVICE_MODEL_MESSAGE_ID_End) \
        EVENT(DEVICE_MODEL_DSP_MESSAGE_ID_TemperatureTimerExpired)  \
\
        EVENT(RFTASK_MESSAGE_ID_Start) \
        EVENT(RFTASK_MESSAGE_ID_IPCPacketAvailable) \
        EVENT(RFTASK_MESSAGE_ID_CC2500WatchdogTimer) \
        EVENT(RFTASK_MESSAGE_ID_LinkStatusTimer) \
        EVENT(RFTASK_MESSAGE_ID_Statistics) \
        EVENT(RFTASK_MESSAGE_ID_ChipconSpiTransferComplete) \
        EVENT(RFTASK_MESSAGE_ID_IPC_SendMessageToRemote) \
        EVENT(RFTASK_MESSAGE_ID_IPC_ReceivedMessageFromRemote) \
        EVENT(RFTASK_MESSAGE_ID_IPC_TerminateUpdate) \
        EVENT(RFTASK_MESSAGE_ID_TransportTimer) \
        EVENT(RFTASK_MESSAGE_ID_End) \
\
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_Start) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_Init) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_Notify) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_Rx) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_Tx) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_RFTimer) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_GDO2Interrupt) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_TransmitTest) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_TransmitCWTest) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_TestDutyCycle) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_LockChannel) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_TestTimerDone) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_PacketTimerDone) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_TransmitIntervalTimerDone) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_EnablePairing) \
        EVENT(RFPROTOCOLSTATEMACHINE_MESSAGE_ID_End) \
\
        EVENT(UPDATE_MESSAGE_ID_Start) \
        EVENT(UPDATE_MESSAGE_ID_RebootRequestReceived) \
        EVENT(UPDATE_MESSAGE_ID_IPCPacketAvailable) \
        EVENT(UPDATE_MESSAGE_ID_SetState) \
        EVENT(UPDATE_MESSAGE_ID_SetAutoUpdateMode) \
        EVENT(UPDATE_MESSAGE_ID_SetWaitForExtPeriphMode) \
        EVENT(UPDATE_MESSAGE_ID_DoEnterState) \
        EVENT(UPDATE_MESSAGE_ID_WatchdogExpired) \
        EVENT(UPDATE_MESSAGE_ID_SetUpdateFileName) \
        EVENT(UPDATE_MESSAGE_ID_CompleteState) \
        EVENT(UPDATE_MESSAGE_ID_SetForceUpdate) \
        EVENT(UPDATE_MESSAGE_ID_DoSingleUpdate) \
        EVENT(UPDATE_MESSAGE_ID_End) \
\
        EVENT(REMOTEUI_MESSAGE_ID_Start) \
        EVENT(REMOTEUI_MESSAGE_ID_SendVariable) \
        EVENT(REMOTEUI_MESSAGE_ID_RequestVariable) \
        EVENT(REMOTEUI_MESSAGE_ID_HandleReceivedVariable) \
        EVENT(REMOTEUI_MESSAGE_ID_RequestAction) \
        EVENT(REMOTEUI_MESSAGE_ID_PacketReceived) \
        EVENT(REMOTEUI_MESSAGE_ID_FileTransfer) \
        EVENT(REMOTEUI_MESSAGE_ID_PairSuccessful) \
        EVENT(REMOTEUI_MESSAGE_ID_LinkStatus) \
        EVENT(REMOTEUI_MESSAGE_ID_TestPacket) \
        EVENT(REMOTEUI_MESSAGE_ID_End) \
\
        EVENT(REMOTEFILETRANS_MESSAGE_ID_Start) \
        EVENT(REMOTEFILETRANS_MESSAGE_ID_SendFile) \
        EVENT(REMOTEFILETRANS_MESSAGE_ID_SendFileFromBlob) \
        EVENT(REMOTEFILETRANS_MESSAGE_ID_End) \
\
        EVENT(SpiIpc_Msg_ID_Start) \
        EVENT(SpiIpc_Msg_ID_InitializeSpi) \
        EVENT(SpiIpc_Msg_ID_DeInitSpi) \
        EVENT(SpiIpc_Msg_ID_CancelTransmit) \
        EVENT(SpiIpc_Msg_ID_End) \
\
        EVENT(WA_MSG_ID_Start) \
        EVENT(WA_MSG_ID_TimerExpired) \
        EVENT(WA_MSG_ID_HandleEXTI) \
        EVENT(WA_MSG_ID_Init) \
        EVENT(WA_MSG_ID_Update_Darr) \
        EVENT(WA_MSG_ID_Enter_Update_Transfer) \
        EVENT(WA_MSG_ID_Data_Request_Timeout) \
        EVENT(WA_MSG_ID_Quiesce) \
        EVENT(WA_MSG_ID_ST_Radio_Status) \
        EVENT(WA_MSG_ID_StartPair) \
        EVENT(WA_MSG_ID_FactoryDefault) \
        EVENT(WA_MSG_ID_EraseCoco) \
        EVENT(WA_MSG_ID_ReconfigureRFBand) \
        EVENT(WA_MSG_ID_SendTestMessage) \
        EVENT(WA_MSG_ID_ToggleDataMessageDump) \
        EVENT(WA_MSG_ID_SetRFBand) \
        EVENT(WA_MSG_ID_QuickToFullBoot) \
        EVENT(WA_MSG_ID_OpenPairingWindow) \
        EVENT(WA_MSG_ID_GetSpeakerVersion) \
        EVENT(WA_MSG_ID_End) \
\
        EVENT(LBCS_MSG_ID_Start) \
        EVENT(LBCS_MSG_ID_ReadEvent) \
        EVENT(LBCS_MSG_ID_HandleIPC) \
        EVENT(LBCS_MSG_ID_End) \
\
        EVENT(NUM_EVENTS) \
\
        EVENT(INVALID_EVENT) \


typedef enum
{
    FOREACH_CORE_EVENT (GENERATE_ENUM)
    FOREACH_VARIANT_EVENT (GENERATE_ENUM)
} MESSAGE_ID_t;

const char* GetEventString(MESSAGE_ID_t id);
MESSAGE_ID_t GetEventFromString(const char* eventString);

#endif /* EVENTDEFINITIONS_H */
