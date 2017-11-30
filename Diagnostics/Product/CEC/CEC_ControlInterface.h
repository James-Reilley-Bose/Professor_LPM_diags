#ifndef CEC_CONTRL_INTERFACE_H
#define CEC_CONTRL_INTERFACE_H

#include "KeyData.h"
#include "CEC_KeyIndex.h"
#include "nv_system.h"
#include "cec.h"
#include "CECTask.h"
#include "sourceInterface.h"

#define WAIT_FOR_TV_SYSTEM_AUDIO 1100  // msec
#define WAIT_FOR_TV_SYSTEM_AUDIO_EVENT_BITS_WAIT 1500 //msec
#define AUDIO_STATUS_HOLDOFF 500  // msec
#define DEVICE_INFO_POLL 10000  // msec

#define CEC_MODE_DISABLE_ALL 0x00
#define CEC_MODE_GENERAL_FUNCTIONALITY 0x01
#define CEC_MODE_HANDLE_ROUTING_MESSAGES 0x02
#define CEC_MODE_SEND_ROUTING_MESSAGES 0x04
#define CEC_MODE_HANDLE_ALL_SYS_AUD_MESSAGES 0x08

#define CEC_MODE_FULL_COMPLIANCE (CEC_MODE_GENERAL_FUNCTIONALITY | CEC_MODE_HANDLE_ROUTING_MESSAGES | CEC_MODE_SEND_ROUTING_MESSAGES | CEC_MODE_HANDLE_ALL_SYS_AUD_MESSAGES)
#define CEC_MODE_ALT_ON_1 (CEC_MODE_GENERAL_FUNCTIONALITY | CEC_MODE_SEND_ROUTING_MESSAGES | CEC_MODE_HANDLE_ALL_SYS_AUD_MESSAGES)
#define CEC_MODE_ALT_ON_2 (CEC_MODE_GENERAL_FUNCTIONALITY | CEC_MODE_HANDLE_ALL_SYS_AUD_MESSAGES)
#define CEC_MODE_MINIMAL (CEC_MODE_GENERAL_FUNCTIONALITY)

#define CEC_FEATURE_ARC 0x1
#define CEC_FEATURE_DEVICE_POLLING 0x2
#define CEC_FEATURE_GLOBAL_STANDBY 0x4
#define CEC_FEATURE_ON_SCREEN_NAME 0x8
#define CEC_FEATURE_SYSTEM_AUDIO 0x10
#define CEC_FEATURE_KEY_TX 0x20
#define CEC_FEATURE_KEY_RX 0x40
#define CEC_FEATURE_ROUTING_CONTROLS 0x80
#define CEC_FEATURE_ONE_TOUCH_PLAY 0x100
#define CEC_IMAGE_VIEW_ON 0x200
#define CEC_SPECIAL_ROUTING_CONTROLS 0x400 /* Added since new samsungs like to announce their last port and that made us switch */

#define CEC_FEATURES_ALL (CEC_FEATURE_ARC | CEC_FEATURE_DEVICE_POLLING | CEC_FEATURE_GLOBAL_STANDBY \
                         | CEC_FEATURE_ON_SCREEN_NAME | CEC_FEATURE_SYSTEM_AUDIO | CEC_FEATURE_KEY_TX \
                         | CEC_FEATURE_KEY_RX | CEC_FEATURE_ROUTING_CONTROLS | CEC_FEATURE_ONE_TOUCH_PLAY | CEC_IMAGE_VIEW_ON)

#define CEC_FEATURES_SPECIAL_ROUTING (CEC_FEATURE_ARC | CEC_FEATURE_DEVICE_POLLING | CEC_FEATURE_GLOBAL_STANDBY \
                         | CEC_FEATURE_ON_SCREEN_NAME | CEC_FEATURE_SYSTEM_AUDIO | CEC_FEATURE_KEY_TX \
                         | CEC_FEATURE_KEY_RX | /*CEC_FEATURE_ROUTING_CONTROLS | */ CEC_FEATURE_ONE_TOUCH_PLAY | CEC_IMAGE_VIEW_ON | CEC_SPECIAL_ROUTING_CONTROLS)

#define CEC_FEATURES_ARC_ONLY (CEC_FEATURE_SYSTEM_AUDIO | CEC_FEATURE_ARC | CEC_FEATURE_KEY_RX | CEC_IMAGE_VIEW_ON | CEC_FEATURE_GLOBAL_STANDBY)

typedef struct
{
    uint16_t physicalAddress;
    uint8_t logicalAddress;
    struct
    {
        CEC_VERSION cecVersion;
        uint16_t physicalAddress;
        uint8_t osd[CEC_MAX_OSD_LEN + 1];
        uint8_t vendorID[VENDOR_ID_LEN];
    } cec_devices[CEC_DEVICE_NUM_DEVICES];
} cecState_t;

extern cecState_t cecState;

BOOL CEC_CanWeARC(uint16_t us, uint16_t them);
BOOL CEC_GetSystemAudioPermitted(void);
BOOL CEC_GetWaitingForTVSysAudio(void);
BOOL CEC_IsArcInitialized(void);
BOOL CEC_IsEnabledAllSystemAudio(void);
BOOL CEC_IsEnabledHandleRoutingMessages(void);
BOOL CEC_IsEnabledMinimalFunction(void);
BOOL CEC_IsEnabledSendRoutingMessages(void);
BOOL CEC_IsUpstreamDevice(uint16_t addr1, uint16_t addr2);
BOOL CEC_SystemAudioModeStatus(void);
uint16_t CEC_GetPhyAddrInMessage(uint8_t* msg, uint8_t position);
uint16_t CEC_GetPhysicalAddress(void);
uint16_t CEC_GetPhysicalAddressFromLogAddr(uint8_t logAddr);
uint8_t CEC_GetArrayIndex(uint16_t phyAddr);
uint8_t CEC_GetOpcode(uint8_t* msg);
uint8_t CEC_GetSrcLogicalAddress(uint8_t* msg);
uint8_t CEC_GetTVLogicalAddress(void);
uint8_t CEC_GetUIKey(uint8_t* msg);
void CEC_AnnounceActiveSource(void);
void CEC_AcquireLogicalAddress(uint8_t logicalAddress);
void CEC_ConsumeKey(KEY_DATA_t key);
void CEC_GiveAudioStatus(uint8_t addr);
void CEC_ReportSystemAudioModeStatus(uint8_t addr);
void CEC_GivePowerStatus(uint8_t addr);
void CEC_GoToStandby(void);
void CEC_HandleActiveSourceSetStrmPath(uint16_t destPhyAddr);
void CEC_HandleSystemAudioModeRequest(CEC_t* cecMsg);
void CEC_HandleRouteChangeRouteInfo(CEC_t* cecMsg);
void CEC_ImageViewOn(void);
void CEC_PowerOnDevice(hdmi_port port);
void CEC_ReportPhysicalAddress(void);
void CEC_SendFeatureAbort(uint8_t opcode, uint8_t reason, uint8_t addr);
void CEC_SendGlobalStandby(void);
void CEC_SendInactiveSource(void);
void CEC_SendInitiateARC(uint8_t addr);
void CEC_SendOSDName(uint8_t addr, const char* osd, int osd_len);
void CEC_SendRequestActiveSource(void);
void CEC_SendRoutingChange(hdmi_port oldPort, hdmi_port newPort);
void CEC_SendRoutingInformation(SOURCE_ID source);
void CEC_SendSystemAudioMode(BOOL onOff);
void CEC_SendSystemAudioModeResponse(BOOL onOff);
void CEC_SendTerminateARC(uint8_t addr);
void CEC_SetAddressField(uint8_t* msg, uint8_t src, uint8_t dst);
void CEC_SetARCInitialized(BOOL initialized);
void CEC_SetMode(CEC_MODE mode, BOOL save);
CEC_MODE CEC_GetMode();
void CEC_SetOpcode(uint8_t* msg, uint8_t opcode);
void CEC_SetPhyAddrInMessage(uint8_t* msg, uint16_t phyAddr, uint8_t position);
void CEC_SetSystemAudioModeStatus(BOOL status);
void CEC_SetSystemAudioPermitted(BOOL permitted);
void CEC_SetUIKey(uint8_t* msg, uint8_t uiKey);
void CEC_SetVolumeAndMuteStatus(uint8_t* msg, uint8_t volume, uint8_t mute);
void CEC_SetWaitingForTVSysAudio(BOOL waiting);
void CEC_GivePhysicalAddress(uint8_t addr);
void CEC_GiveOSDName(uint8_t addr);
void CEC_GetCECVersion(uint8_t addr);
void CEC_CECVersion(uint8_t addr);
void CEC_DeviceVendorID(void);
void CEC_GetDeviceVendorID(uint8_t addr);
void CEC_SetEnabledFeatures(uint32_t features);
uint32_t CEC_GetEnabledFeatures();
BOOL CEC_IsEnabledFeature(uint32_t featureMask);
BOOL CEC_IsConfigurationSource(SOURCE_ID sourceID);
#endif
