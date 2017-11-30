#include "project.h"
#include "cec.h"
#include "CEC_ControlInterface.h"
#include "CECTask.h"
#include "buffermanager.h"
//#include "UITask.h"
#include "rottenlog.h"
#include "IpcRouterTask.h"
//#include "SourceRecipe.h"
#include "CEC_Utilities.h"
//#include "systemBehaviors.h"
#include "GlobalParameterList.h"
#include "SourceChangeTask.h"
#include "sourceInterface.h"
#include "viewNotify.h"
#include "nv_system.h"

// Local Prototypes
static BOOL CEC_IsDeviceConnected(uint16_t sourceAddr);
static void CEC_TransmitKeyReleaseToDevice(void);
static void CEC_TransmitKeyToDevice(KEY_VALUE boseKey);
static BOOL CEC_IsDuplicateSourceChange(SOURCE_ID sourceID);
static BOOL CEC_IsSourceChangePermitted(SOURCE_ID sourceID);


/*
===============================================================================
 Logs
===============================================================================
*/
SCRIBE_DECL(cec);

/*
===============================================================================
 Local Variables
===============================================================================
*/
cecState_t cecState;

static BOOL arcInitialized = FALSE;
static BOOL bSystemAudioStatus = FALSE;
static BOOL bWaitingForTvSysAudio = FALSE;
static uint8_t tvLogicalAddr = 0x0;  // Should ALWAYS (almost) be 0x0
static uint8_t currentCECMode = CEC_MODE_FULL_COMPLIANCE;
static uint32_t currentCECFeatures = CEC_FEATURES_ALL;
// Feature control
static BOOL bSystemAudioPermitted = TRUE;

/*
===============================================================================
@fn CEC_GetTVLogicalAddress
@brief Accessor for the TV's logical address.
===============================================================================
*/
uint8_t CEC_GetTVLogicalAddress(void)
{
    return tvLogicalAddr;
}

/************************************************************************************************************/
/*********************************************Audio Return Channel*******************************************/
/************************************************************************************************************/
/*
===============================================================================
@fn    CEC_SendInitiateARC
@brief Send the initiate ARC message to the TV. Set up Audio channel.
===============================================================================
*/
void CEC_SendInitiateARC(uint8_t addr)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_ARC))
    {
        if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t msg[3] = {0, 0, 0};
            uint8_t len = 2;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
            CEC_SetOpcode(msg, CEC_MESSAGE_INITIATE_ARC);
            CEC_SendMessage(msg, len);
        }
    }
}

/*
===============================================================================
@fn    CEC_SendTerminateARC
@brief Send terminate ARC to the TV. Restore audio channel.
===============================================================================
*/
void CEC_SendTerminateARC(uint8_t addr)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_ARC))
    {
        if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t msg[3] = {0, 0, 0};
            uint8_t len = 2;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
            CEC_SetOpcode(msg, CEC_MESSAGE_TERMINATE_ARC);
            CEC_SendMessage(msg, len);
        }
    }
}

/*
===============================================================================
@fn CEC_SetARCInitialized
@brief
===============================================================================
*/
void CEC_SetARCInitialized(BOOL initialized)
{
    arcInitialized = initialized;
}

/*
===============================================================================
@fn CEC_IsArcInitialized
@brief Are we supposedly in ARC or not
===============================================================================
*/
BOOL CEC_IsArcInitialized(void)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_ARC))
    {
        return arcInitialized;
    }
    else
    {
        return FALSE;
    }
}

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

/************************************************************************************************************/
/*********************************************System Audio Control*******************************************/
/************************************************************************************************************/
/*
===============================================================================
@fn CEC_GetSystemAudioPermitted
@brief
===============================================================================
*/
BOOL CEC_GetSystemAudioPermitted(void)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_SYSTEM_AUDIO))
    {
        return bSystemAudioPermitted;
    }
    else
    {
        return FALSE;
    }
}

/*
===============================================================================
@fn CEC_SetSystemAudioPermitted
@brief
===============================================================================
*/
void CEC_SetSystemAudioPermitted(BOOL permitted)
{
    bSystemAudioPermitted = permitted;
}

/*
===============================================================================
@fn CEC_SystemAudioModeStatus
@brief Accessor for whether or not system audio is currently on
===============================================================================
*/
BOOL CEC_SystemAudioModeStatus(void)
{
    return bSystemAudioStatus;
}

/*
===============================================================================
@fn CEC_SetSystemAudioModeStatus
@brief Mutator for current system audio status
===============================================================================
*/
void CEC_SetSystemAudioModeStatus(BOOL status)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_SYSTEM_AUDIO))
    {
        bSystemAudioStatus = status;
    }
}

/*
===============================================================================
@fn CEC_GetWaitingForTVSysAudio
@brief Get whether or not we're waiting on the TV's <Feature Abort> for
 System Audio
===============================================================================
*/
BOOL CEC_GetWaitingForTVSysAudio(void)
{
    return bWaitingForTvSysAudio;
}
/*
===============================================================================
@fn CEC_SetWaitingForTVSysAudio
@brief Set whether or not we're waiting for the TV's <Feature Abort> for
 system audio. The high level interface where CEC messages are received is
 checking all <Feature Aborts> for this one if we're waiting.
===============================================================================
*/
void CEC_SetWaitingForTVSysAudio(BOOL waiting)
{
    bWaitingForTvSysAudio = waiting;
}

/*
===============================================================================
@fn CEC_SendSystemAudioMode
@brief Tell the TV that we're the audio system. TV will mute.
===============================================================================
*/
void CEC_SendSystemAudioMode(BOOL enabled)
{
    if (TRUE == CEC_GetSystemAudioPermitted())
    {
        if (CEC_IsEnabledAllSystemAudio() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t msg[3] = {0, 0, 0};
            uint8_t len = 3;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), tvLogicalAddr);
            CEC_SetOpcode(msg, CEC_MESSAGE_SET_SYSTEM_AUDIO_MODE);
            msg[2] = enabled;
            CEC_SendMessage(msg, len);
            if (enabled)
            {
                CECPostMsg(CEC_MESSAGE_ID_START_SystemAudioBroadcastTimer, NOP_CALLBACK, TRUE);
                CEC_SetWaitingForTVSysAudio(TRUE);
            }
            else
            {
                CEC_SetSystemAudioModeStatus(FALSE);
            }
        }
    }
}

/*
===============================================================================
@fn CEC_SendSystemAudioModeResponse
@brief Used to reply to a System audio mode request with address.
===============================================================================
*/
void CEC_SendSystemAudioModeResponse(BOOL onOff)
{
    if (TRUE == CEC_GetSystemAudioPermitted())
    {
        if (CEC_IsEnabledAllSystemAudio() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t msg[3] = {0, 0, 0};
            uint8_t len = 3;

            CEC_SetSystemAudioModeStatus(onOff);
            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), CEC_DEVICE_UNREG_BCAST);
            CEC_SetOpcode(msg, CEC_MESSAGE_SET_SYSTEM_AUDIO_MODE);
            msg[2] = onOff;
            CEC_SendMessage(msg, len);
        }
    }
}

/*
===============================================================================
@fn CEC_GiveAudioStatus
@brief This is a request to send volume and mute status. Respond directly to
        the requesting address.
===============================================================================
*/
void CEC_GiveAudioStatus(uint8_t addr)
{
#if 0 //TODO: Bring in with volume code
    if (TRUE == CEC_GetSystemAudioPermitted())
    {
        if (CEC_IsEnabledAllSystemAudio() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t msg[4] = {0, 0, 0, 0};
            uint8_t len = 3;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr); /* Directly addressed */
            CEC_SetOpcode(msg, CEC_MESSAGE_REPORT_AUDIO_STATUS);
            CEC_SetVolumeAndMuteStatus(msg, VolumeModel_GetAudioParam(VM_VOLUME), VolumeModel_GetAudioParam(VM_USER_MUTE));
            CEC_SendMessage(msg, len);
        }
    }
#endif
}

/*
===============================================================================
@fn CEC_ReportSystemAudioModeStatus
@brief This is a request for standby status of the audio system (us).
       The response can be directly addressed or broadcast.
===============================================================================
*/
void CEC_ReportSystemAudioModeStatus(uint8_t addr)
{
    if (TRUE == CEC_GetSystemAudioPermitted())
    {
        if (CEC_IsEnabledAllSystemAudio() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t msg[5] = {0, 0, 0, 0, 0};
            uint8_t len = 3;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr); /* Directly addressed */
            CEC_SetOpcode(msg, CEC_MESSAGE_SYSTEM_AUDIO_MODE_STATUS);
            msg[2] = CEC_SystemAudioModeStatus();
            CEC_SendMessage(msg, len);
        }
    }
}

/*
===============================================================================
@fn CEC_SendSystemAudioRequest
@brief
===============================================================================
*/
void CEC_SendSystemAudioRequest(uint8_t addr)
{
    if (TRUE == CEC_GetSystemAudioPermitted())
    {
        if (CEC_IsEnabledAllSystemAudio() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t msg[5] = {0, 0, 0, 0, 0};
            uint8_t len = 4;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
            CEC_SetOpcode(msg, CEC_MESSAGE_SYSTEM_AUDIO_MODE_REQUEST);
            CEC_SetPhyAddrInMessage(msg, CEC_GetPhysicalAddress(), ADDR1_INDEX);

            CEC_SendMessage(msg, len);
        }
    }
}
/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

/************************************************************************************************************/
/*********************************************Remote Control Pass Through************************************/
/************************************************************************************************************/
/*
===============================================================================
@func CEC_ConsumeKey
@brief API to receive and send keys
@param KEY_DATA_t key, used to get key value and state.
===============================================================================
 */
void CEC_ConsumeKey(KEY_DATA_t key)
{
    if (LPM_KEY_STATE_PRESSED == key.state)
    {
        CEC_TransmitKeyToDevice(key.value);
    }
    else if (LPM_KEY_STATE_RELEASED == key.state)
    {
        CEC_TransmitKeyReleaseToDevice();
    }
}

/*
===============================================================================
@fn    CEC_TransmitKeyToDevice
@brief Sends the 3 byte message to the ADI driver.
       index 0 is the address field. From us (0x5) to the dest.
       index 1 is the state. Key up or down. (only two options)
       index 2 is the opcode (play, pause, up, down, etc)
===============================================================================
*/
void CEC_TransmitKeyToDevice(KEY_VALUE boseKey)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_KEY_TX))
    {
        if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            hdmi_port port = SourceInterface_GetHDMIPortFromSourceID(SourceChangeAPI_GetCurrentSource());

            if (!SourceInterface_IsInternalSource(SourceChangeAPI_GetCurrentSource()) && HDMI_PORT_NONE != port)
            {
                CEC_USER_CONTROL_CODE key = CEC_BOSE_KEY_MAPPING[boseKey];

                if (CEC_UCC_INVALID != key)
                {
                    uint8_t logicalAddress = CEC_GetDeviceListLogicalAddress(port);
                    if (CEC_DEVICE_DISABLED != logicalAddress)
                    {
                        uint8_t msg[3] = {0, 0, 0};
                        uint8_t len = 3;
                        // Match boseKey to a CEC opcode
                        CEC_SetUIKey(msg, key);

                        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), logicalAddress);
                        CEC_SetOpcode(msg, CEC_KEY_PRESS);
                        CEC_SendMessage(msg, len);
                    }
                }
            }
        }
    }
}

/*
===============================================================================
@fn    CEC_TransmitKeyReleaseToDevice
@brief Sends the 3 byte message to the ADI driver.
       index 0 is the address field. From us (0x5) to the dest.
       index 1 is the state. Key up or down. (only two options)
       index 2 is the opcode (play, pause, up, down, etc)
===============================================================================
*/
void CEC_TransmitKeyReleaseToDevice(void)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_KEY_TX))
    {
        SOURCE_ID current_source = SourceChangeAPI_GetCurrentSource();
        if (SourceInterface_IsInternalSource(current_source) || HDMI_PORT_NONE == SourceInterface_GetHDMIPortFromSourceID(current_source))
        {
            return;
        }
        if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t logicalAddress = CEC_GetDeviceListLogicalAddress(SourceInterface_GetHDMIPortFromSourceID(current_source));
            if (CEC_DEVICE_DISABLED != logicalAddress)
            {
                uint8_t msg[2] = {0, 0};
                uint8_t len = 2;

                CEC_SetAddressField(msg, CEC_GetLogicalAddress(), logicalAddress);
                CEC_SetOpcode(msg, CEC_KEY_RELEASE);
                CEC_SendMessage(msg, len);
            }
        }
    }
}

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

/************************************************************************************************************/
/***********************************************Routing Control**********************************************/
/***********************************************and One Touch Play*******************************************/
/*
===============================================================================
@fn CEC_AnnounceActiveSource
@brief Sends <Active Source> with our logical address or the sources.
===============================================================================
*/
void CEC_AnnounceActiveSource(void)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_ONE_TOUCH_PLAY))
    {
        SOURCE_ID next_source = SourceChangeAPI_GetNextSource();
        if (CEC_IsEnabledSendRoutingMessages() &&
                (CEC_GetPhysicalAddress() != DEVICE_DISABLED_PHYSICAL_ADDRESS) &&
                (SourceInterface_IsHDMISource(next_source) || SourceInterface_IsInternalSource(next_source)))
        {
            uint8_t srcLa = CEC_DEVICE_DISABLED;
            hdmi_port dest_port = SourceInterface_GetHDMIPortFromSourceID(next_source);

            if (dest_port != HDMI_PORT_NONE)
            {
                srcLa = CEC_GetDeviceListLogicalAddress(dest_port);
            }

            // So if we switch to a source attached to us that doesn't have a logical address
            // there is no downside for us to send ourselves as the active source otherwise we
            // let them announce themselves as active
            if (srcLa == CEC_DEVICE_DISABLED || srcLa == CEC_GetLogicalAddress())
            {
                uint8_t msg[5] = {0, 0, 0, 0, 0};
                uint8_t len = 4;
                CEC_SetAddressField(msg, CEC_GetLogicalAddress(), CEC_DEVICE_UNREG_BCAST);
                CEC_SetOpcode(msg, CEC_MESSAGE_ACTIVE_SRC);
                CEC_SetPhyAddrInMessage(msg, CEC_GetPhysicalAddress(), ADDR1_INDEX);
                CEC_SendMessage(msg, len);
            }
        }
    }
}

/*
===============================================================================
@fn CEC_ImageViewOn
@brief Tell the TV to turn on its screen when we switch to a source.
 Should we use <Text View On> it removes menus and text from the screen. --jd71069
===============================================================================
*/
void CEC_ImageViewOn(void)
{
    if (CEC_IsEnabledFeature(CEC_IMAGE_VIEW_ON))
    {
        if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t msg[3] = {0, 0, 0};
            uint8_t len = 2;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), tvLogicalAddr);
            CEC_SetOpcode(msg, CEC_MESSAGE_IMAGE_VIEW_ON);
            CEC_SendMessage(msg, len);
        }
    }
}

/*
===============================================================================
@fn CEC_SendInactiveSource
@brief Send an inactive source message when we are no long active.
===============================================================================
*/
void CEC_SendInactiveSource(void)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_ROUTING_CONTROLS))
    {
        if (CEC_IsEnabledSendRoutingMessages() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
        {
            uint8_t msg[5] = {0, 0, 0, 0, 0};
            uint8_t len = 4;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), tvLogicalAddr);
            CEC_SetOpcode(msg, CEC_MESSAGE_INACTIVE_SOURCE);
            CEC_SetPhyAddrInMessage(msg, CEC_GetPhysicalAddress(), ADDR1_INDEX);
            CEC_SendMessage(msg, len);
        }
    }
}

/*
===============================================================================
@fn CEC_SendRoutingInformation
@brief Send the routing information message to all CEC devices.
===============================================================================
*/
void CEC_SendRoutingInformation(SOURCE_ID source)
{
    hdmi_port port = SourceInterface_GetHDMIPortFromSourceID(source);
    if (CEC_IsEnabledFeature(CEC_FEATURE_ROUTING_CONTROLS))
    {
        if (CEC_IsEnabledSendRoutingMessages() && (port != 0))
        {
            uint8_t msg[5] = {0, 0, 0, 0, 0};
            uint8_t len = 4;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), CEC_DEVICE_UNREG_BCAST);
            CEC_SetOpcode(msg, CEC_MESSAGE_ROUTE_INFO);
            CEC_SetPhyAddrInMessage(msg, CEC_BuildPhysicalAddressForPort(port), ADDR1_INDEX);
            CEC_SendMessage(msg, len);
        }
    }
}

/*
===============================================================================
@fn CEC_SendRoutingChange
@brief Send the routing change message to all CEC devices.
===============================================================================
*/
void CEC_SendRoutingChange(hdmi_port oldPort, hdmi_port newPort)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_ROUTING_CONTROLS))
    {
        if (CEC_IsEnabledSendRoutingMessages() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()) && (DEVICE_DISABLED_PHYSICAL_ADDRESS != CEC_GetPhysicalAddress()))
        {
            uint8_t msg[6] = {0, 0, 0, 0, 0, 0};
            uint8_t len = 6;
            if (oldPort != newPort)
            {
                CEC_SetAddressField(msg, CEC_GetLogicalAddress(), CEC_DEVICE_UNREG_BCAST);
                CEC_SetOpcode(msg, CEC_MESSAGE_ROUTE_CHANGE);
                if (oldPort == HDMI_PORT_NONE)
                {
                    CEC_SetPhyAddrInMessage(msg, CEC_GetPhysicalAddress(), ADDR1_INDEX);
                }
                else
                {
                    CEC_SetPhyAddrInMessage(msg, CEC_BuildPhysicalAddressForPort(oldPort), ADDR1_INDEX);
                }

                if (newPort == HDMI_PORT_NONE)
                {
                    CEC_SetPhyAddrInMessage(msg, CEC_GetPhysicalAddress(), ADDR2_INDEX);
                }
                else
                {
                    CEC_SetPhyAddrInMessage(msg, CEC_BuildPhysicalAddressForPort(newPort), ADDR2_INDEX);
                }

                CEC_SendMessage(msg, len);
            }
        }
    }
}

/*
===============================================================================
@fn CEC_SendRequestActiveSource
@brief Send the <Request Active Source> messsage (discovery message).
===============================================================================
*/
void CEC_SendRequestActiveSource(void)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_ROUTING_CONTROLS))
    {
        if (CEC_IsEnabledMinimalFunction())
        {
            uint8_t msg[2] = {0, 0};
            uint8_t len = 2;

            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), CEC_DEVICE_UNREG_BCAST);
            CEC_SetOpcode(msg, CEC_MESSAGE_REQ_ACTIVE_SRC);
            CEC_SendMessage(msg, len);
        }
    }
}
/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

/************************************************************************************************************/
/***********************************************System Information*******************************************/
/************************************************************************************************************/

/*
===============================================================================
@fn Give Physical Address
@brief send out phyiscal/logical address pair
===============================================================================
*/
void CEC_ReportPhysicalAddress(void)
{
    if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()) && (DEVICE_DISABLED_PHYSICAL_ADDRESS != CEC_GetPhysicalAddress()))
    {
        uint8_t msg[5] = {0, 0, 0, 0, 0};
        uint8_t len = 5;

        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), CEC_DEVICE_UNREG_BCAST);
        CEC_SetOpcode(msg, CEC_MESSAGE_REPORT_PHYS_ADDR);
        CEC_SetPhyAddrInMessage(msg, CEC_GetPhysicalAddress(), ADDR1_INDEX);
        msg[4] = 0x5;

        CEC_SendMessage(msg, len);
    }
}

/*
===============================================================================
@fn CEC_GivePhysicalAddress
@brief Ask another device for its physical address.
===============================================================================
*/
void CEC_GivePhysicalAddress(uint8_t addr)
{
    if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
    {
        uint8_t msg[2] = {0, 0};
        uint8_t len = 2;

        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
        CEC_SetOpcode(msg, CEC_MESSAGE_GIVE_PHYS_ADDR);

        CEC_SendMessage(msg, len);
    }
}

/*
===============================================================================
@fn CEC_GiveOSDName
@brief Ask another device for its osd name
===============================================================================
*/
void CEC_GiveOSDName(uint8_t addr)
{
    if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
    {
        uint8_t msg[2] = {0, 0};
        uint8_t len = 2;

        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
        CEC_SetOpcode(msg, CEC_MESSAGE_GIVE_OSD_NAME);

        CEC_SendMessage(msg, len);
    }
}

/*
===============================================================================
@fn CEC_GetCECVersion
@brief Ask another device for its cec version
===============================================================================
*/
void CEC_GetCECVersion(uint8_t addr)
{
    if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
    {
        uint8_t msg[2] = {0, 0};
        uint8_t len = 2;

        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
        CEC_SetOpcode(msg, CEC_MESSAGE_GET_CEC_VERSION);

        CEC_SendMessage(msg, len);
    }
}

/*
===============================================================================
@fn CEC_GetCECVersion
@brief Give another device our cec version
===============================================================================
*/
void CEC_CECVersion(uint8_t addr)
{
    if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
    {
        uint8_t msg[3] = {0, 0, 0};
        uint8_t len = 3;

        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
        CEC_SetOpcode(msg, CEC_MESSAGE_CEC_VERSION);
        msg[2] = CEC_GetDeviceListCECVersion(CEC_GetLogicalAddress());

        CEC_SendMessage(msg, len);
    }
}

/*
===============================================================================
@fn CEC_SendOSDName
@brief Send the OSD name to the address that requested it.
===============================================================================
*/
void CEC_SendOSDName(uint8_t addr, const char* osd, int osd_len)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_ON_SCREEN_NAME))
    {
        uint8_t msg[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t len = 16;
        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
        CEC_SetOpcode(msg, CEC_MESSAGE_SET_OSD_NAME);

        debug_assert(osd);
        debug_assert(osd_len <= CEC_MAX_OSD_LEN);
        memcpy(&msg[2], osd, osd_len);
        CEC_SendMessage(msg, len);
    }
}

/*
===============================================================================
@fn CEC_DeviceVendorID
@brief Send our IEEE OUI aka Vendor ID
===============================================================================
*/
void CEC_DeviceVendorID(void)
{
    uint8_t msg[5] = {0, 0, BOSE_IEEE_OUI_0, BOSE_IEEE_OUI_1, BOSE_IEEE_OUI_2};
    uint8_t len = 5;
    CEC_SetAddressField(msg, CEC_GetLogicalAddress(), CEC_DEVICE_UNREG_BCAST);
    CEC_SetOpcode(msg, CEC_MESSAGE_DEVICE_VENDOR_ID);
    CEC_SendMessage(msg, len);
}

/*
===============================================================================
@fn CEC_GetDeviceVendorID
@brief Ask another device for its IEEE OUI aka Vendor ID
===============================================================================
*/
void CEC_GetDeviceVendorID(uint8_t addr)
{
    uint8_t msg[2] = {0, 0};
    uint8_t len = 2;
    CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
    CEC_SetOpcode(msg, CEC_MESSAGE_GET_VENDOR_ID);
    CEC_SendMessage(msg, len);
}

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

/************************************************************************************************************/
/***********************************************System Standby***********************************************/
/************************************************************************************************************/

/*
===============================================================================
@fn CEC_SendGlobalStandby
@brief Send global standby to all CEC devices.
===============================================================================
*/
void CEC_SendGlobalStandby(void)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_GLOBAL_STANDBY))
    {
        if (CEC_IsEnabledMinimalFunction())
        {
            uint8_t msg[2] = {0, 0};
            uint8_t len = 2;
            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), CEC_DEVICE_UNREG_BCAST);
            CEC_SetOpcode(msg, CEC_MESSAGE_STANDBY);
            CEC_SendMessage(msg, len);
        }
    }
}

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

/*
===============================================================================
@fn CEC_PowerOnDevice
@brief Takes an HDMI port and send the CEC power on user control key.
===============================================================================
*/
void CEC_PowerOnDevice(hdmi_port port)
{
    if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
    {
        uint8_t msg[3] = {0, 0, 0};
        uint8_t len = 3;
        uint8_t logicalAddr = CEC_GetDeviceListLogicalAddress(port);
        if (CEC_DEVICE_DISABLED != logicalAddr)
        {
            CEC_SetAddressField(msg, CEC_GetLogicalAddress(), logicalAddr);
            CEC_SetOpcode(msg, CEC_KEY_PRESS);
            CEC_SetUIKey(msg, CEC_UCC_POWER_ON_FUNCTION);
            CEC_SendMessage(msg, len);
        }
    }
}

/*
===============================================================================
@fn CEC_SendFeatureAbort
@brief Give a <Feature Abort> to a device for reasons.
===============================================================================
*/
void CEC_SendFeatureAbort(uint8_t opcode, uint8_t reason, uint8_t addr)
{
    if (CEC_IsEnabledMinimalFunction())
    {
        uint8_t msg[4] = {0, 0, 0, 0};
        uint8_t len = 4;

        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
        CEC_SetOpcode(msg, CEC_MESSAGE_FEATURE_ABORT);
        msg[2] = opcode;
        msg[3] = reason;
        CEC_SendMessage(msg, len);
    }
}

/*
===============================================================================
@fn CEC_GivePowerStatus
@brief give current power status. Directly addressed.
===============================================================================
*/
void CEC_GivePowerStatus(uint8_t addr)
{
    if (CEC_IsEnabledMinimalFunction() && (CEC_DEVICE_UNREG_BCAST != CEC_GetLogicalAddress()))
    {
        uint8_t desired = SOURCE_HDMI_3;
        uint8_t actual = SOURCE_HDMI_3;
        uint8_t msg[3] = {0, 0, 0};
        uint8_t len = 3;

        CEC_SetAddressField(msg, CEC_GetLogicalAddress(), addr);
        CEC_SetOpcode(msg, CEC_MESSAGE_REPORT_PWR_STATUS);
        if ((desired == actual) && (desired != SOURCE_STANDBY))
        {
            msg[2] = 0;
        }
        else if ((SOURCE_STANDBY == desired) && (SOURCE_STANDBY == actual))
        {
            msg[2] = 1;
        }
        else if ((SOURCE_STANDBY != desired) && (SOURCE_STANDBY == actual))
        {
            msg[2] = 2;
        }
        else if ((SOURCE_STANDBY == desired) && (SOURCE_STANDBY != actual))
        {
            msg[2] = 3;
        }

        CEC_SendMessage(msg, len);
    }
}

/*
===============================================================================
@fn BOSE_GetCECEnabled
@brief Interface function used by ADI's atv to determine if CEC is enabled
===============================================================================
*/
BOOL BOSE_GetCECEnabled(void)
{
    return CEC_IsEnabledMinimalFunction();
}

/*
===============================================================================
@fn CEC_SetMode
@brief Used by NV to tell CEC its current mode per the user selection.
===============================================================================
*/
void CEC_SetMode(CEC_MODE mode, BOOL save)
{
    switch (mode)
    {
        case CEC_MODE_COMPLIANCE:  // 0x0F - Minimal functionality + System Audio + Send Routing + Receive Routing (Full CEC compliance)
            currentCECMode = CEC_MODE_FULL_COMPLIANCE;
            currentCECFeatures = GP_defaultCECFeatures();
            break;
        case CEC_MODE_USABILITY_1:  // 0x0D - Minimal functionality + System Audio + Send Routing
            currentCECMode = CEC_MODE_ALT_ON_1;
            currentCECFeatures = CEC_FEATURES_SPECIAL_ROUTING;
            break;
        case CEC_MODE_USABILITY_2:  // 0x09 - Minimal functionality + System Audio
            currentCECMode = CEC_MODE_ALT_ON_2;
            currentCECFeatures = CEC_FEATURES_ARC_ONLY;
            break;
        case CEC_MODE_USABILITY_3:  // 0x01 - Minimal functionality
            currentCECMode = CEC_MODE_MINIMAL;
            break;
        case CEC_MODE_DISABLED:
        default:
            currentCECMode = CEC_MODE_DISABLE_ALL;
            break;
    }

    CEC_SetEnabledFeatures(currentCECFeatures);

    if (save)
    {
        /*NV_SetCECMode(mode);
        NV_SetCECFeatures(currentCECFeatures);
        NV_CommitSystemParams(); TODO (lb1032816): Pull in with nv_system */
    }

    UI_ViewNotify(CEC_MODE_CHANGED);
}

/*
===============================================================================
@fn CEC_GetMode
@brief Determines what the current NV compatible setting is and returns it.
===============================================================================
*/
CEC_MODE CEC_GetMode()
{
    if (currentCECMode == (CEC_MODE_GENERAL_FUNCTIONALITY | CEC_MODE_HANDLE_ROUTING_MESSAGES | CEC_MODE_SEND_ROUTING_MESSAGES | CEC_MODE_HANDLE_ALL_SYS_AUD_MESSAGES))
    {
        return CEC_MODE_COMPLIANCE;  // 0x0F - Minimal functionality + System Audio + Send Routing + Receive Routing (Full CEC compliance)
    }
    else if (currentCECMode == (CEC_MODE_GENERAL_FUNCTIONALITY | CEC_MODE_SEND_ROUTING_MESSAGES | CEC_MODE_HANDLE_ALL_SYS_AUD_MESSAGES))
    {
        return CEC_MODE_USABILITY_1;  // 0x0D - Minimal functionality + System Audio + Send Routing
    }
    else if (currentCECMode == (CEC_MODE_GENERAL_FUNCTIONALITY | CEC_MODE_HANDLE_ALL_SYS_AUD_MESSAGES))
    {
        return CEC_MODE_USABILITY_2;  // 0x09 - Minimal functionality + System Audio
    }
    else if (currentCECMode == (CEC_MODE_GENERAL_FUNCTIONALITY))
    {
        return CEC_MODE_USABILITY_3;  // 0x01 - Minimal functionality
    }
    else
    {
        return CEC_MODE_DISABLED;
    }

}

/*
===============================================================================
@fn CEC_IsEnabledHandleRoutingMessages
@brief Are all CEC functions enabled (Handle routing only  full compliance)
===============================================================================
*/
BOOL CEC_IsEnabledHandleRoutingMessages(void)
{
    return (currentCECMode & CEC_MODE_HANDLE_ROUTING_MESSAGES) ? TRUE : FALSE;
}

/*
===============================================================================
@fn CEC_IsEnabledSendRoutingMessages
@brief Are all CEC functions or Minimal functionality + System Audio + Send Routing
===============================================================================
*/
BOOL CEC_IsEnabledSendRoutingMessages(void)
{
    return (currentCECMode & CEC_MODE_SEND_ROUTING_MESSAGES) ? TRUE : FALSE;
}
/*
===============================================================================
@fn CEC_IsEnabledAllSystemAudio
@brief Are all CEC functions enabled or Minimal functionality + System Audio
===============================================================================
*/
BOOL CEC_IsEnabledAllSystemAudio(void)
{
    return (currentCECMode & CEC_MODE_HANDLE_ALL_SYS_AUD_MESSAGES) ? TRUE : FALSE;
}

/*
===============================================================================
@fn CEC_IsEnabledMinimalFunction
@brief Is CEC enabled to at least minimal.
===============================================================================
*/
BOOL CEC_IsEnabledMinimalFunction(void)
{
    return (currentCECMode & CEC_MODE_GENERAL_FUNCTIONALITY) ? TRUE : FALSE;
}

/*
===============================================================================
@fn CEC_IsEnabledFeature
@brief Is CEC enabled to permit the feature
===============================================================================
*/
BOOL CEC_IsEnabledFeature(uint32_t featureMask)
{
    return (currentCECFeatures & featureMask) ? TRUE : FALSE;
}

/*
===============================================================================
@fn CEC_SetEnabledFeatures
@brief Change current allowed features.
===============================================================================
*/
void CEC_SetEnabledFeatures(uint32_t features)
{
    currentCECFeatures = features;
}

/*
===============================================================================
@fn CEC_SetEnabledFeatures
@brief Change current allowed features.
===============================================================================
*/
uint32_t CEC_GetEnabledFeatures()
{
    return currentCECFeatures;
}

/*
===============================================================================
@fn CEC_SetPhyAddrInMessage
@brief Put the physical address parameter into the cec message.
===============================================================================
*/
void CEC_SetPhyAddrInMessage(uint8_t* msg, uint16_t phyAddr, uint8_t position)
{
    msg[position] = (uint8_t)(phyAddr >> 8);
    msg[position + 1] = (uint8_t)phyAddr & 0xff;
}

/*
===============================================================================
@fn CEC_GetPhyAddrInMessage
@brief
===============================================================================
*/
uint16_t CEC_GetPhyAddrInMessage(uint8_t* msg, uint8_t position)
{
    return (msg[position] << 8 | msg[position + 1]);
}

/*
===============================================================================
@fn CEC_SetOpcode
@brief Put the opcode parameter into the cec message.
===============================================================================
*/
void CEC_SetOpcode(uint8_t* msg, uint8_t opcode)
{
    msg[OPCODE_INDEX] = opcode;
}

/*
===============================================================================
@fn CEC_SetAddressField
@brief Put the logical address parameters into the cec message.
===============================================================================
*/
void CEC_SetAddressField(uint8_t* msg, uint8_t src, uint8_t dst)
{
    msg[SR_DST_INDEX] = (((src) << 4) | (dst));
}

/*
===============================================================================
@fn CEC_SetUIKey
@brief Put the remote control key parameter into the cec message.
===============================================================================
*/
void CEC_SetUIKey(uint8_t* msg, uint8_t uiKey)
{
    msg[UI_KEY_INDEX] = uiKey;
}

/*
===============================================================================
@fn CEC_GetUIKey
@brief Get the remote control key parameter from the cec message.
===============================================================================
*/
uint8_t CEC_GetUIKey(uint8_t* msg)
{
    return msg[UI_KEY_INDEX];
}

void CEC_SetVolumeAndMuteStatus(uint8_t* msg, uint8_t volume, uint8_t mute)
{
    debug_assert(1 >= mute);            // Has to be 1 or 0
    msg[VOLUME_MUTE_INDEX] = mute << 7; /* bit 7 */
    msg[VOLUME_MUTE_INDEX] = msg[VOLUME_MUTE_INDEX] | (volume & 0x7F);
}

/*
===============================================================================
@fn  CEC_IsDeviceConnected
@brief TRUE if the device is plugged directly into the console.
===============================================================================
*/
BOOL CEC_IsDeviceConnected(uint16_t sourceAddr)
{
    uint8_t a_ConsoleAddr[4];
    uint8_t a_SourceAddr[4];
    CEC_MakePhyAddrArray(CEC_GetPhysicalAddress(), a_ConsoleAddr);
    CEC_MakePhyAddrArray(sourceAddr, a_SourceAddr);

    if (sourceAddr == CEC_GetPhysicalAddress())
    {
        // Shouldn't be our address.
        return FALSE;
    }
    else if (a_ConsoleAddr[0] != a_SourceAddr[0])
    {
        // First digits have to match.
        return FALSE;
    }
    else if (0 == a_ConsoleAddr[1] && 0 == a_ConsoleAddr[2] && 0 == a_ConsoleAddr[3])
    {
        // If our last three digits are 0, their second digit must be
        // different and last two must be 0
        return (0 != a_SourceAddr[1] && 0 == a_SourceAddr[2] && 0 == a_SourceAddr[3]);
    }
    else if (0 == a_ConsoleAddr[2] == a_ConsoleAddr[3])
    {
        // If our last two digits are 0, their third
        // digit can be different, last must be zero.
        return (0 != a_SourceAddr[2] && (0 == a_SourceAddr[3]));
    }
    else if (0 == a_ConsoleAddr[3])
    {
        // If only our last digit is 0, their's must not be.
        return (0 != a_SourceAddr[3]);
    }
    else
    {
        // Invalid case.
        return FALSE;
    }
}

/*
===============================================================================
@fn CEC_CanWeARC
@brief Check if the two addresses are adjacent on the HDMI network
       Assumes that the first address is zero. If not, it switches them before
       doing the other checks.
===============================================================================
*/
BOOL CEC_CanWeARC(uint16_t us, uint16_t them)
{
    uint8_t a_them[4] = {0, 0, 0, 0};
    uint8_t a_us[4] = {0, 0, 0, 0};

    CEC_MakePhyAddrArray(them, a_them);
    CEC_MakePhyAddrArray(us, a_us);

    if (us == them || CEC_IsDeviceConnected(them))
    {
        return FALSE;
    }
    else if (CEC_IsValidAddress(us) && CEC_IsValidAddress(them))
    {
        // address 1 is the TV (0.0.0.0)
        // adjacent would be x.0.0.0
        if (0 == them)
        {
            return 0 != a_us[0] && 0 == a_us[1] && 0 == a_us[2] && 0 == a_us[3];
        }
        // address 1 is x.0.0.0
        // adjacent would be x.y.0.0
        else if (a_them[0] == a_us[0])
        {
            return 0 != a_us[1] && 0 == a_us[2] && 0 == a_us[3] && 0 == a_them[1] && 0 == a_them[2] && 0 == a_them[3];
        }
        // address 1 is x.y.0.0
        // adjacent would be x.y.z.0
        else if (a_them[0] == a_us[0] && a_them[1] == a_us[1])
        {
            return 0 != a_us[2] && 0 == a_us[3] && 0 == a_them[2] && 0 == a_them[3];
        }
        // address 1 is w.x.y.0
        // ajacent would be w.x.y.z
        else if (a_them[0] == a_us[0] && a_them[1] == a_us[1] && a_them[2] == a_us[2])
        {
            return 0 != a_us[3] && 0 == a_them[3];
        }
        else
        {
            return FALSE;
        }
    }
    return FALSE;
}

/*
===============================================================================
@fn CEC_GetArrayIndex
@brief Discover which port the device is connected to.
===============================================================================
*/
uint8_t CEC_GetArrayIndex(uint16_t phyAddr)
{
    uint8_t a_ConsoleAddr[4] = {0, 0, 0, 0};
    uint8_t a_SourceAddr[4] = {0, 0, 0, 0};

    uint16_t physicalAddress = CEC_GetPhysicalAddress();
    BOOL connected = CEC_IsDeviceConnected(phyAddr);

    if (phyAddr == physicalAddress)
    {
        // Shouldn't be our address.
        return 0xFF;
    }
    else if (connected)
    {
        CEC_MakePhyAddrArray(physicalAddress /*ours*/, a_ConsoleAddr);
        CEC_MakePhyAddrArray(phyAddr /*theirs*/, a_SourceAddr);

        for (int i = 1; i < 4; ++i)
        {
            if (a_ConsoleAddr[i] == 0 && a_SourceAddr[i] != 0)
            {
                return (a_SourceAddr[i]);
            }
        }
    }
    // Shouldn't get here.
    return 0xFF;
}

/*
===============================================================================
@fn CEC_HandleSystemAudioModeRequest
@brief
===============================================================================
*/
void CEC_HandleSystemAudioModeRequest(CEC_t* cecMsg)
{
    uint16_t destPhyAddr = CEC_GetPhyAddrInMessage((uint8_t*)cecMsg, ADDR1_INDEX);

    if (CEC_IsEnabledFeature(CEC_FEATURE_ROUTING_CONTROLS)) // Check permissions
    {
        SOURCE_ID actualSource = SourceChangeAPI_GetCurrentSource();
        SOURCE_ID desiredSource = SourceChangeAPI_GetNextSource();

        /*
         * @comment
         * jd71069
         * Toshiba TV sends us a <System Audio Mode Request> in response to us powering down. This causes us to
         * power back up. This is not a desirable feature for the user. So, when we're going to Standby (desired source)
         * we are sending a <Feature Abort>[Wrong mode] so that we can power down.
         *
         * TRIODE-980 (ported to Bardeen/Ginger)
         *
         */
        if (SourceInterface_IsStandbySource(desiredSource) && (FALSE == SourceInterface_IsStandbySource(actualSource)))
        {
            CEC_SendFeatureAbort(CEC_MESSAGE_SYSTEM_AUDIO_MODE_REQUEST, CEC_ABRT_NOT_IN_CORRECT_MODE, cecMsg->initiator);
        }
        else
        {
            if (destPhyAddr != CEC_GetPhysicalAddress() && FALSE == CEC_IsUpstreamDevice(CEC_GetPhysicalAddress(), destPhyAddr)/*Device Downstream*/)
            {
                if (CEC_IsSourceChangePermitted(SOURCE_TV) && CEC_IsEnabledHandleRoutingMessages())
                {
                    // The address in the message is downstream
                    LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "CEC Source Change: <System Audio Mode Request>, %s --> %s", UITask_GetUnifySrcName(actualSource), UITask_GetUnifySrcName(SOURCE_TV));
                    //UI_SetUnifySource(UI_GetUnifySourceIndex(SOURCE_TV)); TODO (lb1032816): Bring in with source switching
                }
            }
            else // The address is either ours or downstream.
            {
                if (SourceInterface_IsStandbySource(actualSource) && CEC_IsEnabledHandleRoutingMessages())
                {
                    // We are for sure in standby
                    // Come out of standby to last source.
                    //LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "CEC Source Change: <System Audio Mode Request>, %s --> %s", UITask_GetUnifySrcName(actualSource), UITask_GetUnifySrcName(UI_GetLastSource()));
                    //UI_SetUnifySource(UI_GetUnifySourceIndex(UI_GetLastSource())); TODO (lb1032816): Bring in with source switching
                }
                // If we're powered on and the address is ours or upstream, do nothing else.
            }

            // Send appropriate response out:
            // TV gets a <Set System Audio Mode>["on"], broadcast
            if (CEC_GetTVLogicalAddress() == cecMsg->initiator)
            {
                CEC_SendSystemAudioModeResponse(TRUE /*ON*/);
            }
            // If someone else initiated, we have to ask the TV first
            // <System Audio Mode Request> to the TV.
            else if (CEC_DEVICE_UNREG_BCAST != cecMsg->initiator)
            {
                CECPostMsg(CEC_MESSAGE_ID_SendSystemAudioRequest, NOP_CALLBACK, TRUE);
            }

            // Also inform that we're not muted
            CEC_GiveAudioStatus(cecMsg->initiator);
        }
    }
}

/*
===============================================================================
@fn CEC_HandleActiveSourceSetStrmPath
@brief
===============================================================================
*/
void CEC_HandleActiveSourceSetStrmPath(uint16_t destPhyAddr)
{
    if (CEC_IsEnabledFeature(CEC_FEATURE_ROUTING_CONTROLS) && CEC_IsEnabledHandleRoutingMessages())
    {
        SOURCE_ID newSource = INVALID_SOURCE;

        if (destPhyAddr == CEC_GetPhysicalAddress())
        {
            // Why did we receive one with our address in it? Don't know.
            // Do nothing.
            return;
        }
        else if (0x0000 == destPhyAddr || FALSE == CEC_IsUpstreamDevice(CEC_GetPhysicalAddress(), destPhyAddr))
        {
            newSource = SOURCE_TV;
        }
        else if (CEC_IsUpstreamDevice(CEC_GetPhysicalAddress(), destPhyAddr))
        {
            hdmi_port port = CEC_GetPortFromPhyAddress(destPhyAddr);

            if (HDMI_PORT_START <= port && port < HDMI_PORT_NONE)
            {
                //newSource = SystemBehavior_GetSourceIDFromPort(port); TODO (lb1032816): Bring in with source switching

                if (FALSE/* == SystemBehavior_CECCanSwitchToSource(newSource)*/)
                {
                    //newSource = SystemBehavior_GetCECFirstInput(); TODO (lb1032816): Bring in with source switching
                }
            }
            else
            {
                //newSource = SystemBehavior_GetCECFirstInput(); TODO (lb1032816): Bring in with source switching
            }

        }
        if (INVALID_SOURCE != newSource && CEC_IsSourceChangePermitted(newSource))
        {
            LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "CEC Source Change: <Active Source>/<Set Stream Path>, %s --> %s", UITask_GetUnifySrcName(SourceChangeAPI_GetCurrentSource()), UITask_GetUnifySrcName(newSource));
            //UI_SetUnifySource(UI_GetUnifySourceIndex(newSource)); TODO (lb1032816): Bring in with source switching
        }
    }
}

/*
===============================================================================
@fn CEC_HandleRouteChangeRouteInfo
@brief
===============================================================================
*/
void CEC_HandleRouteChangeRouteInfo(CEC_t* cecMsg)
{
    uint8_t addressIndex = cecMsg->length == ROUTE_CHANGE_LEN ? ADDR2_INDEX : ADDR1_INDEX;
    uint16_t destPhyAddr = CEC_GetPhyAddrInMessage((uint8_t*)cecMsg, addressIndex);
    SOURCE_ID newSource = INVALID_SOURCE;

    // Way that makes the world happier
    // TODO (lb1032816): This setting shouldn't be applied with professor.
    if (CEC_IsEnabledFeature(CEC_SPECIAL_ROUTING_CONTROLS))
    {
        // WE don't have to come out of standby for the <Routing Change> or <Routing Information>
        if (!SourceInterface_IsStandbySource(SourceChangeAPI_GetCurrentSource()))
        {
            // If it is trying to go to an upstream port lets try
            if (destPhyAddr != CEC_GetPhysicalAddress() && CEC_IsUpstreamDevice(CEC_GetPhysicalAddress(), destPhyAddr))
            {
                //newSource = SystemBehavior_GetSourceIDFromPort(CEC_GetPortFromPhyAddress(destPhyAddr)); TODO (lb1032816): Bring in with source switching
            }
            // If its downstream or us we switch to tv source
            else if (!CEC_IsUpstreamDevice(CEC_GetPhysicalAddress(), destPhyAddr) || destPhyAddr == CEC_GetPhysicalAddress())
            {
                newSource = SOURCE_TV;
            }
        }
    } // Compliant way
    else if (CEC_IsEnabledFeature(CEC_FEATURE_ROUTING_CONTROLS) && CEC_IsEnabledHandleRoutingMessages())
    {
        if (destPhyAddr == CEC_GetPhysicalAddress())
        {
            newSource = (SOURCE_TV == SourceChangeAPI_GetCurrentSource()) ? SOURCE_HDMI_1 : SourceChangeAPI_GetCurrentSource();
        }
        else if (!CEC_IsUpstreamDevice(CEC_GetPhysicalAddress(), destPhyAddr))
        {
            newSource = SOURCE_TV;
        }
    }

    if (SourceInterface_IsHDMISource(newSource))
    {
        CEC_SendRoutingInformation(newSource);
    }

    if (INVALID_SOURCE != newSource && CEC_IsSourceChangePermitted(newSource))
    {
        // The address in the message is downstream
        LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "CEC Source Change: <Routing Change>, %s --> %s", UITask_GetUnifySrcName(SourceChangeAPI_GetCurrentSource()), UITask_GetUnifySrcName(newSource));
        //UI_SetUnifySource(UI_GetUnifySourceIndex(newSource)); TODO (lb1032816): Bring in source switching
    }
}

/*
===============================================================================
@fn
@brief We need to ajudicate which source switches we must honor and which ones
    we won't because they're not expected.
===============================================================================
*/
BOOL CEC_IsSourceChangePermitted(SOURCE_ID sourceID)
{
    if (FALSE == SourceChangeAPI_GetSourceChangePending())
    {
        // Always come out of standby
        if (SourceInterface_IsStandbySource(SourceChangeAPI_GetCurrentSource()))
        {
            return TRUE;
        }

        // When we're in setup, don't switch away.
        if (CEC_IsConfigurationSource(SourceChangeAPI_GetCurrentSource()))
        {
            return FALSE;
        }

        // don't duplicate source switches from CEC
        // There are several times when multiple messages
        // mean the same source change.
        if (CEC_IsDuplicateSourceChange(sourceID))
        {
            return FALSE;
        }

        // Switch when in TV source
        if (SourceInterface_IsTVSource(SourceChangeAPI_GetCurrentSource()))
        {
            return TRUE;
        }

        // Don't switch if we're in a non-video source
        // In Ginger, some sources don't have video that in Bardeen have a spalsh screen.
        if (SourceInterface_IsInternalSource(SourceChangeAPI_GetNextSource()))
        {
            return FALSE;
        }

        // If none of the checks passes, we can probably switch.
        return TRUE;
    }
    return FALSE;
}
/*
===============================================================================
@fn
@brief
===============================================================================
*/
BOOL CEC_IsDuplicateSourceChange(SOURCE_ID sourceID)
{
    return (sourceID == SourceChangeAPI_GetCurrentSource() || sourceID == SourceChangeAPI_GetNextSource());
}

/*
===============================================================================
@fn CEC_IsUpstreamDevice
@brief determines if addr2 is upstream of addr1. TRUE for upstream,
       FALSE for downstream
 Upstream: Connected to us or farther from the TV. Ex: us: 1.0.0.0, them: 1.1.1.0
 Downstream: Connected elsewhere, maybe to the TV. Ex: us: 1.0.0.0, them: 3.1.0.0
 Precondition: addr1 and addr2 are different, neither is 0.0.0.0
 The port to which is it connected is passed out using uint8_t* port.
 It's only valid if the device is upstream.
===============================================================================
*/
BOOL CEC_IsUpstreamDevice(uint16_t addr1, uint16_t addr2)
{
    BOOL upstream = TRUE;

    uint8_t a_addr1[4] = {0, 0, 0, 0};
    uint8_t a_addr2[4] = {0, 0, 0, 0};

    CEC_MakePhyAddrArray(addr1, a_addr1);
    CEC_MakePhyAddrArray(addr2, a_addr2);

    // If the first digit is the same, the device is downstream.
    // if the addresses are the same, the device is not upstream.
    if (a_addr1[0] != a_addr2[0] || addr1 == addr2)
    {
        upstream = FALSE;
    }
    else
    {
        for (int i = 1; i < 4; ++i)
        {
            // We're a repeater, so our address will have a zero
            // and upstream devices will have a non-zero
            // the rest of the address doesn't matter at that point,
            // we know it's upstream.
            if (a_addr1[i] != a_addr2[i] && 0 == a_addr1[i])
            {
                // Device is connected beneath us (AKA upstream)
                upstream = TRUE;
                break;
            }
            // We only get here if at least the first digits are the same.
            // If the are not the same at this point, the device is not
            // connected beneath us. It's downstream.
            else if (a_addr1[i] != a_addr2[i])
            {
                upstream = FALSE;
                break;
            }
            else
            {
                // The two are equal at that spot, scroll to the next spot.
            }
        }
    }
    return upstream;
}


/*
===============================================================================
@fn  CEC_GoToStandby
@brief If we're not in Shelby, Bluetooth, Update,AdaptIQ, or Side Aux,
        go to standby.
===============================================================================
*/
void CEC_GoToStandby(void)
{
    if (!CEC_IsConfigurationSource(SourceChangeAPI_GetCurrentSource()))
    {
        LOG(cec, ROTTEN_LOGLEVEL_NORMAL, "CEC Source Change: <Standby>, %s --> %s",UITask_GetUnifySrcName(SourceChangeAPI_GetCurrentSource()),UITask_GetUnifySrcName(SOURCE_STANDBY));
        //UI_SetUnifySource(UI_GetUnifySourceIndex(SOURCE_STANDBY)); TODO (lb1032816): Pull in or remove...
    }
}

void CEC_AcquireLogicalAddress(uint8_t logicalAddress)
{
    IPCRouter_Send(IPC_DEVICE_F0, IPC_CEC_LOGICAL_ADDRESS, NULL, &logicalAddress, sizeof(logicalAddress));
}

/*
============================================================================================
@func CEC_IsConfigurationSource
@brief Check sources that should not be interrupted.
============================================================================================
*/
BOOL CEC_IsConfigurationSource(SOURCE_ID sourceID)
{
    /*
      return ((UI_IsUpdateSource(sourceID)) ||
              (UI_IsUnifySource(sourceID)) ||
              (UI_IsFactoryDefaultSource(sourceID)) ||
              (UI_IsAdaptIQSource(sourceID)));
    */
    return TRUE;
}
