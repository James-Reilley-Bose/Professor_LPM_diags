/*
  File  : DeviceModel_DSP.c
  Title :
  Author  : Derek Richardson
  Created : 10/21/2015
  Language: C
  Copyright:  (C) 2015 Bose Corporation, Framingham, MA

  Description:
        Functions to handle dsp model in system
===============================================================================
*/
#include "DeviceModelTask.h"
#include "DeviceModel_DSP.h"
#include "DeviceStatusFieldDefs.h"
#include "BoseUSART_API.h"
#include "IpcKeys.h"
#include "IpcStatus.h"
#include "IpcDsp.h"
//#include "volumeModel.h"
#include "nv_mfg.h"
#include "UITask.h"
#include "systemBehaviors.h"
#include "nv_system.h"
#include "BMutex.h"

#include "IPCRouterTask.h"
#include "IpcInterface.h"
#include "IpcTxTask.h"
#include "IpcUart.h"
#include "ProductSystemParams.h"

#define RESET_DSP_ON_TIMEOUT (0)
#define MAX_HS_MISSED_POLLS (2)
#define DSP_TEMP_TIMER_PERIOD TIMER_MSEC_TO_TICKS(1000)

SCRIBE_DECL(device_model_dsp);
Generic_Device* dspDevice;
static IpcDSPAudioMetaDataPayload_t myAudioData = {0};
static TimerHandle_t DspTemperatureTimer = NULL;

static void SendVolumeMute(void);
static void DeviceModel_DSP_TurnOff(void);
static void DeviceModel_DSP_TurnOn(void);
static void DeviceModel_DSP_SendTemperatureData(void);
static void DeviceModel_DSP_SetBootedToInstallerFlag(DSPImage_t image);

///////////////////////////////////////////////////////////
IpcDspProfilePayload_t myDspProfile = {
    .sourceSelect = LPM_IPC_SOURCE_STANDBY,
    .powerState = IPC_POWER_STATE_UNKNOWN,
    .networkLatencyMs = 0,
    .targetLatencyMs = 0,
    .audioMode = IPC_AUDIO_MODE_UNSPECIFIED,
    .aiqEnable = FALSE,
    .aiqCurveSelect = IPC_AIQ_CURVE_A,
    .dualMonoSelect = IPC_DUAL_MONO_BOTH,
    .bassLevel = 0,
    .trebleLevel = 0,
    .centerLevel = 0,
    .surroundLevel = 0,
    .languageSelect = SPOKEN_LANGUAGE_UNDEFINED,
    .region = SPEAKER_REGION_US_AIM,
    .accessoryFlags = 0,
    .frontConnector = 0, //TODO need to synchronize A4V with Sharc IPC defines
};


IpcDSPVolumePayload myDspVolMute = {
    .volume = 0,
};

static int16_t DspTemperatureData[IPC_THERM_LOCATION_TYPES] = {0};

IpcDspPlayTonePayload_t myPlayTone;

static BOOL isSourceMute = FALSE;
static BOOL isInternalMute = FALSE;
static BOOL bootAiqCurveB = FALSE;
static uint32_t hsMissedPolls = 0;
static BOOL doSoftReset = FALSE;
static BOOL doWDReset = FALSE;
static BOOL hasBootedToInstaller = FALSE;
static uint32_t curMinimumOutputLatencyMs = 0;
static uint32_t curTotalLatencyMs = 0;


static IpcInterface_t IpcInterface =
{
    .DeviceID = IPC_DEVICE_DSP,
    .type = IPC_INTERFACE_UART,
    .uart =
    {
        .BUART = DSP_BUART,
        .buartInit =
        {
            BR_921600, // 921600
            WL_8BITS,  // 8 bit
            SB_1,      // 1 stop bit
            P_NONE,    // No parity
            M_RX_TX,   // RX and TX
            FC_NONE    // No Flow Control
        },
        .RxBank = DSP_RX_GPIO_BANK,
        .RxPin = DSP_RX_PIN,
        .RxPadConfig = GPIO_MODE_AF_IN_FLOATING,
        .TxBank = DSP_TX_GPIO_BANK,
        .TxPin = DSP_TX_PIN,
        .TxPadConfig = GPIO_MODE_AF_OUT_PP,
        .AF = DSP_GPIO_AF,
        .valid = TRUE,
        .tx =
        {
            .DeviceID = IPC_DEVICE_DSP,
            .DmaTxRequest =
            {
                .selectedChannel = USART6_TX,
                .transferComplete = 1,
                .transferError = 1,
                .halfTransfer = 0,
                .directModeError = 0,
                .fifoModeError = 0,
                .dmaPriority = 1,
                .doubleBuffer = 0,
            },
            .RegisterBase = DSP_UART,
        },
        .rx =
        {
            .DmaRxRequest =
            {
                .selectedChannel = USART6_RX,
                .transferComplete = 1,
                .transferError = 1,
                .halfTransfer = 0,
                .directModeError = 0,
                .fifoModeError = 0,
                .dmaPriority = 1,
                .doubleBuffer = 1,
            },
            .RegisterBase = DSP_UART,
            ._bServiceStarted = FALSE,
            .packet = NULL,
            .nextPacket = NULL,
            ._RXDoneCallback = IPCRouter_RXDoneCallback,
            ._RXDMAResetCount = 0,
        },
    },
    .stat = {0},
    .test = {0},
};
/*
 * @func DeviceModel_DSP_Init
 *
 * @brief Initializes the dsp device model
 *
 * @param n/a
 *
 * @return n/a
 */
void DeviceModel_DSP_Init(Generic_Device* myDevice)
{
    dspDevice = myDevice;
    BMutex_Give(dspDevice->StatusMutex);
   
    if(NV_GetProductVariant() == PRODUCT_GINGER)
    {
        myDspProfile.speakerPackage = IPC_SPEAKER_PACKAGE_GINGER;
    }
    else if(NV_GetProductVariant() == PRODUCT_PROFESSOR)
    {
        myDspProfile.speakerPackage = IPC_SPEAKER_PACKAGE_PROFESSOR;
    }

    DspTemperatureTimer = createTimer(DSP_TEMP_TIMER_PERIOD,
                                      (void*) &dspDevice,
                                      DEVICE_MODEL_DSP_MESSAGE_ID_TemperatureTimerExpired,
                                      FALSE,
                                      tBlockIdDspTemperatureTimer,
                                      "Dsp_TemperatureTimer",
                                      NULL);
    debug_assert(DspTemperatureTimer);

    ConfigureGpioPin(DSP_RST_L_GPIO_BANK, DSP_RST_L_GPIO_PIN, DSP_RST_L_MODE, DSP_RST_L_AF);
    
    DEASSERT_GPIO_PIN_FG(DSP_LEVEL_SHIFT_L_BANK, DSP_LEVEL_SHIFT_L_PIN);
    ConfigureGpioPin(DSP_LEVEL_SHIFT_L_BANK, DSP_LEVEL_SHIFT_L_PIN, GPIO_MODE_OUT_PP, 0);

    Ipc_InitializeUart(&IpcInterface);
    IPCRouter_RegisterDevice(&IpcInterface, GetManagedTaskPointer(IpcTaskNames[IpcInterface.DeviceID]));
}

/*
 * @func DeviceModel_DSP_Booted
 *
 * @brief Function to send needed info to dsp once it's booted
 *
 * @param n/a
 *
 * @return n/a
 */
void DeviceModel_DSP_Booted()
{
    /// TODO mmoretti rfid stuff
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);

    REGION_VARIANT region = NV_GetRegionVariant();
    myDspProfile.region = (uint8_t)region;

#if 0 //TODO bring in variant system behaviors
    SPOKEN_LANGUAGE language = SystemBehavior_TransformLangSelectToSpokenLang(SystemBehavior_GetLanguage());
    myDspProfile.languageSelect = (uint8_t)language;
#endif

#if 0 //TODO - need to bring in the VolumeModel
    myDspProfile.audioMode = VolumeModel_GetAudioParam(VM_AUDIO_MODE);

    myDspProfile.bassLevel = VolumeModel_GetAudioParam(VM_BASS);
    myDspProfile.trebleLevel = VolumeModel_GetAudioParam(VM_TREBLE);
    myDspProfile.centerLevel = VolumeModel_GetAudioParam(VM_CENTER);
    myDspProfile.surroundLevel = VolumeModel_GetAudioParam(VM_SURROUND);
#endif
    myDspProfile.aiqEnable = NV_GetAIQEnabled();
    
    if (bootAiqCurveB == TRUE)
    {
        myDspProfile.aiqCurveSelect = IPC_AIQ_CURVE_B;
        bootAiqCurveB = FALSE;
    }

    myDspProfile.frontConnector = NV_GetHPMode();
    IpcDsp_SendProfile(&myDspProfile);
    BMutex_Give(dspDevice->StatusMutex);

    IpcDsp_SendAudioFormat(&myAudioData);

    doSoftReset = TRUE;
    SendVolumeMute();
    doSoftReset = FALSE;
    doWDReset = FALSE;

    timerStart(DspTemperatureTimer, 0, &dspDevice->Task->Queue);
}

/*
 * @func DeviceModel_DSP_Standby
 *
 * @brief Take the DSP in/out of standby
 *
 * @param BOOL enterStandby - TRUE to enter, FALSE to exit
 *
 * @return n/a
 */
void DeviceModel_DSP_Standby(BOOL enterStandby)
{
    if(enterStandby)
    {
        DeviceModel_DSP_TurnOff();
    }
    else
    {
        DeviceModel_DSP_TurnOn();
    }
}

/*
 * @func DeviceModel_DSP_Reboot
 *
 * @brief Reboot the DSP
 *
 * @param BOOL enterStandby - TRUE to enter, FALSE to exit
 *
 * @return n/a
 */
void DeviceModel_DSP_Reboot(IpcRebootType_t type, uint32_t waitB4RebootMs)
{
    // The DSP does not support he new parameters as of 20160614, therefore they are ignored, they should be 0,0,0

    DeviceModel_DSP_TurnOff();
    DeviceModel_DSP_TurnOn();
    
#if 0 //TODO - need to bring in UITask
    if (UI_GetCurrentSource() == SOURCE_FACTORY_DEFAULT)
    {
      UIPostMsg(UI_MSG_ID_DSPFactoryDefaultDone, NOP_CALLBACK, 0);
    }
    else
    {
      DeviceModel_DSP_TurnOff();
      DeviceModel_DSP_TurnOn();
    }
#endif
}

static void SendVolumeMute(void)
{
#if 0 //TODO - need to bring in Volume Model
    if ( (myDspVolMute.muteState == (VolumeModel_GetAudioParam(VM_USER_MUTE) || isSourceMute || isInternalMute)
         && myDspVolMute.volume == VolumeModel_GetAudioParam(VM_VOLUME))
         && !(doSoftReset || doWDReset))
        return;
#endif
    if( !(doSoftReset || doWDReset) && myDspVolMute.muteState == ( isSourceMute || isInternalMute ) ) return; 
    
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    
    //TODO - need to bring in Volume Model
    //myDspVolMute.muteState = VolumeModel_GetAudioParam(VM_USER_MUTE) || isSourceMute || isInternalMute;

    /* Ordered in terms of priority */
    if (isInternalMute)
    {
        myDspVolMute.muteProfile = DSP_MUTEPROFILE_INTERNAL;
    }
    else if (isSourceMute)
    {
        myDspVolMute.muteProfile = DSP_MUTEPROFILE_SOURCE;
    }  
#if 0 //TODO - bring in Volume Model
    else if (VolumeModel_GetAudioParam(VM_USER_MUTE))
    {
        myDspVolMute.muteProfile = DSP_MUTEPROFILE_USER;
    }
#endif
    else
    {
        myDspVolMute.muteProfile = DSP_MUTEPROFILE_NONE;
    }
    
    //TODO - bring in Volume Model
    //myDspVolMute.volume = VolumeModel_GetAudioParam(VM_VOLUME);
    myDspVolMute.rampProfile = 0;
    BMutex_Give(dspDevice->StatusMutex);
    IpcDsp_SendVolMuteToDSP(&myDspVolMute);
}

void DeviceModel_DSP_HandleMessage(GENERIC_MSG_t* msg)
{
    switch (msg->msgID)
    {
        case DEVICE_MODEL_DSP_MESSAGE_ID_TemperatureTimerExpired:
            DeviceModel_DSP_SendTemperatureData();
            break;
        case DEVICE_MODEL_MESSAGE_ID_SendLPMHealthAndStatus:
            IpcStatus_SendLPMHealthAndStatus(IPC_DEVICE_DSP);
            break;
        default:
            break;
    }
}

/*
 * @func DeviceModel_DSP_HandleIPCMessage
 *
 * @brief Handles an ipc message sent to dsp device model
 *
 * @param n/a
 *
 * @return n/a
 */
void DeviceModel_DSP_HandleIPCMessage(IpcPacket_t* packet)
{
    switch(packet->s.opcode)
    {
        case IPC_DSP_H_S:
        {
            // Handle the message with default behavior this way but feel
            // free to do device specific handling if needed

            DeviceModelTask_HandleHealthAndStatus(dspDevice, packet);

            BOOL dataChanged = FALSE;
            BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
            IpcDsp_HealthAndStatusReverseByteOrder(&dspDevice->CurrentStatus.dspHS);
            if (dspDevice->CurrentStatus.dspHS.minimumOutputLatencyMs != curMinimumOutputLatencyMs)
            {
                dataChanged = TRUE;
                curMinimumOutputLatencyMs = dspDevice->CurrentStatus.dspHS.minimumOutputLatencyMs;
            }
            if (dspDevice->CurrentStatus.dspHS.totalLatencyMs != curTotalLatencyMs)
            {
                dataChanged = TRUE;
                curTotalLatencyMs = dspDevice->CurrentStatus.dspHS.totalLatencyMs;
            }
            BMutex_Give(dspDevice->StatusMutex);

            if (dataChanged)
            {
                // SM2 needs to know when certain things change
                DeviceModelTask_SendLPMHealthAndStatusToAllDevices();
            }

            hsMissedPolls = 0;

            // The DSP interrupts the clocks to the DAC when it reboots so we'll let it
            // toggle the CONSOLE_DC_TX line as needed to prevent audio glitches.
#ifdef LPM_HAS_WIRED_BASS_DETECT
            // TODO - When dsp runs
            // UIPostMsg(UI_MSG_ID_DSP_BASS_ENABLE, NOP_CALLBACK, (uint32_t)dspDevice->CurrentStatus.dspHS.bassEnableRequest);
#else
            //leave Bardeen alone
            WiredBass_DC_TX_SetState(dspDevice->CurrentStatus.dspHS.bassEnableRequest);
#endif
            DeviceModel_DSP_SetBootedToInstallerFlag((DSPImage_t) dspDevice->CurrentStatus.dspHS.image);

            // Check the DSP image that is running
#if 0 //TODO - Need to bring in UI Task 
            if (SystemBehavior_Aiq_Monitoring_GetState() == TRUE)
            {
              UI_AiqMonitorDSPImage();
            }

            if(dspDevice->CurrentStatus.dspHS.ampFaultState != IPC_AMP_NO_FAULT)
            {
                UI_SetErrorState(TRUE, SYSTEM_ERROR_AMP_FAULT);
                SystemBehavior_AmpFault();
            }
            else if((UI_GetErrorState() & SYSTEM_ERROR_AMP_FAULT) == SYSTEM_ERROR_AMP_FAULT)
            {
                // Clear when it tells us all clear which should happen
                // on reboot
                UI_SetErrorState(FALSE, SYSTEM_ERROR_AMP_FAULT);
            }

            // Is it time to reboot after a factory default
            if (UI_GetCurrentSource() == SOURCE_FACTORY_DEFAULT)
            {
              if (dspDevice->CurrentStatus.dspHS.powerState == IPC_POWER_STATE_OFF)
              {
                UIPostMsg(UI_MSG_ID_DSPFactoryDefaultDone, NOP_CALLBACK, 0);
              }
            }
#endif

            #if defined(GINGER) //TODO - Should this be added? || defined(PROFESSOR)
            // Autowake handling for Ginger
            if (NV_GetAutoWakEnabled() == TRUE)
            {
              SystemBehavior_HandleAutowake();
            }
            #endif
            LOG(device_model_dsp,ROTTEN_LOGLEVEL_VERBOSE,"DSP Energy Present, %d", dspDevice->CurrentStatus.dspHS.energyPresent);
        }

            break;
        default:
            LOG(device_model_dsp,ROTTEN_LOGLEVEL_NORMAL,"Device model unhandled packet, %02x", packet->s.opcode);
            break;
    }
}

/*
 * @func DeviceModel_DSP_HandleKey
 *
 * @brief Handles a key press to send to device (OK to call in other task context)
 *
 * @param n/a
 *
 * @return n/a
 */
void DeviceModel_DSP_HandleKey(KEY_DATA_t key)
{
    IpcKeys_SendKey(dspDevice->DeviceID, key);
}

/*
 * @func DeviceModel_DSP_HandleWatchDog
 *
 * @brief Handles the device watchdog expiring
 *
 * @param n/a
 *
 * @return n/a
 */
void DeviceModel_DSP_HandleWatchDog()
{
    // In OFF state, do nothing
    if (dspDevice->HealthState == H_S_OFF)
    {
      return;
    }

#if 0 //TODO bring in variant system behaviors "SystemBehavior_Aiq_Monitoring_GetState" causes a fault
    if (SystemBehavior_Aiq_Monitoring_GetState())
    {
        // arm soft reset flag so we can restore context after aiq is complete
        doSoftReset = TRUE;
    }
#endif 
    else if(doSoftReset)
    {
        // aiq was enabled and now it isn't; force h&s back to "boot waiting"
        // and let the "booted()" function clean up for us
        // this will end up sending profile information and volume settings
        dspDevice->HealthState = H_S_BOOT_WAITING;
        IpcStatus_SendHealthAndStatusRequest(dspDevice->DeviceID, DSP_STATUS);
        changeTimerPeriod(dspDevice->Timer, TIMER_MSEC_TO_TICKS(dspDevice->Delays.values[DEVICE_H_S_PING_RESPONSE_DELAY]), 0);
        timerStart(dspDevice->Timer , 0, &dspDevice->Task->Queue);
        hsMissedPolls++;
        return;
    }

    // If normal request updated h&s
    if(dspDevice->HealthState == H_S_NORMAL)
    {
        dspDevice->HealthState = H_S_NORMAL_WAITING;
        LOG(device_model_dsp,ROTTEN_LOGLEVEL_VERBOSE,"%s : H_S_NORMAL_WAITING", dspDevice->Name);

        IpcStatus_SendHealthAndStatusRequest(dspDevice->DeviceID, DSP_STATUS);
        changeTimerPeriod(dspDevice->Timer, TIMER_MSEC_TO_TICKS(dspDevice->Delays.values[DEVICE_H_S_PING_RESPONSE_DELAY]), 0);
        timerStart(dspDevice->Timer , 0, &dspDevice->Task->Queue);
        hsMissedPolls++;
    }
    else
    {
#if 0 //TODO - bring in variant system behaviors "SystemBehavior_GetDspWDEnabled" causes a fault
        if (TRUE == SystemBehavior_GetDspWDEnabled())
        {

            if (hsMissedPolls < MAX_HS_MISSED_POLLS)
            {
                dspDevice->HealthState = H_S_NORMAL_WAITING;
                LOG(device_model_dsp,ROTTEN_LOGLEVEL_VERBOSE,"Didn't get a response from %s to H&S request (retrying): H_S_NORMAL_WAITING", dspDevice->Name);

                IpcStatus_SendHealthAndStatusRequest(dspDevice->DeviceID, DSP_STATUS);
                changeTimerPeriod(dspDevice->Timer, TIMER_MSEC_TO_TICKS(dspDevice->Delays.values[DEVICE_H_S_PING_RESPONSE_DELAY]), 0);
                timerStart(dspDevice->Timer , 0, &dspDevice->Task->Queue);
                hsMissedPolls++;
            }
            else
            {
                //Set h&s as Timeout
                dspDevice->HealthState = H_S_TIMEOUT;
                LOG(device_model_dsp,ROTTEN_LOGLEVEL_NORMAL,"%s (rebooting) : H_S_TIMEOUT", dspDevice->Name);

                //Cycle the dsp
                DeviceModel_DSP_Reboot(REBOOT_TYPE_DEVICE_ONLY, 0);
                doSoftReset = FALSE;
                doWDReset = TRUE;
            }
        }
        else
        {
            /*
             *  For development we don't want to keep resetting dsp just request a status
             */
            // Check the DSP image that is running
            dspDevice->HealthState = H_S_NORMAL_WAITING;
            LOG(device_model_dsp,ROTTEN_LOGLEVEL_VERBOSE,"Didn't get a response from %s to H&S request (devmode, just retrying): H_S_NORMAL_WAITING", dspDevice->Name);

            IpcStatus_SendHealthAndStatusRequest(dspDevice->DeviceID, DSP_STATUS);
            changeTimerPeriod(dspDevice->Timer, TIMER_MSEC_TO_TICKS(dspDevice->Delays.values[DEVICE_BOOT_DELAY]), 0);
            timerStart(dspDevice->Timer , 0, &dspDevice->Task->Queue);
        }
#endif
            dspDevice->HealthState = H_S_NORMAL_WAITING;
            LOG(device_model_dsp,ROTTEN_LOGLEVEL_VERBOSE,"Didn't get a response from %s to H&S request (devmode, just retrying): H_S_NORMAL_WAITING", dspDevice->Name);

            IpcStatus_SendHealthAndStatusRequest(dspDevice->DeviceID, DSP_STATUS);
            changeTimerPeriod(dspDevice->Timer, TIMER_MSEC_TO_TICKS(dspDevice->Delays.values[DEVICE_BOOT_DELAY]), 0);
            timerStart(dspDevice->Timer , 0, &dspDevice->Task->Queue);
    }
}

/////////////////////////////////////////////////////
// BELOW FUNCTIONS SHOULD NEVER BE CALLED DIRECTLY //
/////////////////////////////////////////////////////
void DeviceModel_DSP_Version_Get(char* buffer)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    memcpy(buffer, dspDevice->CurrentStatus.dspHS.swVersion, sizeof(dspDevice->CurrentStatus.dspHS.swVersion));
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_Version_Set(void* data)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    // Field is read only so the API is just placeholder
    BMutex_Give(dspDevice->StatusMutex);
}

DSPImage_t DeviceModel_DSP_Image_Get(void)
{
    return (DSPImage_t)dspDevice->CurrentStatus.dspHS.image;
}

void DeviceModel_DSP_Image_Set(void* data)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    // Field is read only so the API is just placeholder
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_PlayTone_Get(void* buffer)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    // Field is not needed for reading for now
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_HSMinimumOutputLatencyMs_Get(void* buffer)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    *( (uint32_t*) buffer ) = dspDevice->CurrentStatus.dspHS.minimumOutputLatencyMs;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_HSMinimumOutputLatencyMs_Set(void* data)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    // Field is read only so the API is just placeholder
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_HSTotalLatencyMs_Get(void* buffer)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    *( (uint32_t*) buffer ) = dspDevice->CurrentStatus.dspHS.totalLatencyMs;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_HSTotalLatencyMs_Set(void* data)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    // Field is read only so the API is just placeholder
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_PlayTone_Set(void* data)
{
    IpcDspPlayTonePayload_t playTone;
    playTone = *((IpcDspPlayTonePayload_t *)data);

    if ( myPlayTone.toneId != playTone.toneId )
    {
        BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
        myPlayTone.locationBits = playTone.locationBits;
        myPlayTone.maxVolume = playTone.maxVolume;
        myPlayTone.minVolume = playTone.minVolume;
        myPlayTone.repeatCount = playTone.repeatCount;
        myPlayTone.toneId  =  playTone.toneId;
        BMutex_Give(dspDevice->StatusMutex);

        IpcDsp_SendPlayTone(&myPlayTone);
    }
}


void DeviceModel_DSP_EnergyPresent_Get(void* buffer)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    *( (uint32_t*) buffer ) = dspDevice->CurrentStatus.dspHS.energyPresent;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_EnergyPresent_Set(void* data)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    // Field is read only so the API is just placeholder
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_ampFaultState_Get(void* buffer)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    *( (uint32_t*) buffer ) = dspDevice->CurrentStatus.dspHS.ampFaultState;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_ampFaultState_Set(void* data)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    // Field is read only so the API is just placeholder
    BMutex_Give(dspDevice->StatusMutex);
}


void DeviceModel_DSP_AudioFormat_Get(void* buffer)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    memcpy(buffer, &myAudioData, sizeof(IpcDSPAudioMetaDataPayload_t));
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_AudioFormat_Set(void* data)
{
    IpcDSPAudioMetaDataPayload_t* newFormat = (IpcDSPAudioMetaDataPayload_t*)data;

    if (memcmp ( &myAudioData, newFormat, sizeof(IpcDSPAudioMetaDataPayload_t)) != 0)
    {
        BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
        memcpy ( &myAudioData, newFormat, sizeof(IpcDSPAudioMetaDataPayload_t));
        BMutex_Give(dspDevice->StatusMutex);
        IpcDsp_SendAudioFormat(&myAudioData);
    }
}

void DeviceModel_DSP_TrebleLevel_Get(void *pTrebleLevel)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    * ((int8_t *)pTrebleLevel) = myDspProfile.trebleLevel;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_TrebleLevel_Set(void* dummy)
{
#if 0 //TODO - Need to bring in Volume Model
    if ( myDspProfile.trebleLevel != VolumeModel_GetAudioParam(VM_TREBLE) )
    {
        BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
        myDspProfile.trebleLevel = VolumeModel_GetAudioParam(VM_TREBLE);
        BMutex_Give(dspDevice->StatusMutex);

        IpcDsp_SendProfile(&myDspProfile);
    }
#endif
}

void DeviceModel_DSP_BassLevel_Get(void *pBassLevel)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((int8_t *)pBassLevel) = myDspProfile.bassLevel;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_BassLevel_Set(void* dummy)
{
#if 0 //TODO - Need to bring in Volume Model
    if (myDspProfile.bassLevel != VolumeModel_GetAudioParam(VM_BASS))
    {
        BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
        myDspProfile.bassLevel = VolumeModel_GetAudioParam(VM_BASS);
        IpcDsp_SendProfile(&myDspProfile);
        BMutex_Give(dspDevice->StatusMutex);
    }
#endif
}

void DeviceModel_DSP_CenterLevel_Get(void *pCenterLevel)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((int8_t *)pCenterLevel) = myDspProfile.centerLevel;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_CenterLevel_Set(void* dummy)
{
#if 0 //TODO - Need to bring in Volume Model
    if (myDspProfile.centerLevel != VolumeModel_GetAudioParam(VM_CENTER))
    {
        BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
        myDspProfile.centerLevel = VolumeModel_GetAudioParam(VM_CENTER);
        IpcDsp_SendProfile(&myDspProfile);
        BMutex_Give(dspDevice->StatusMutex);
    }
#endif
}

void DeviceModel_DSP_SurroundLevel_Get(void *pSurroundLevel)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((int8_t *)pSurroundLevel) = myDspProfile.surroundLevel;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_SurroundLevel_Set(void* dummy)
{
#if 0 //TODO - Need to bring in Volume Model
    if (myDspProfile.surroundLevel != VolumeModel_GetAudioParam(VM_SURROUND))
    {
        BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
        myDspProfile.surroundLevel = VolumeModel_GetAudioParam(VM_SURROUND);
        IpcDsp_SendProfile(&myDspProfile);
        BMutex_Give(dspDevice->StatusMutex);
    }
#endif
}

void DeviceModel_DSP_SourceSelect_Get(void *pSourceSelect)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((DSPSource_t *)pSourceSelect) = myDspProfile.sourceSelect;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_SourceSelect_Set(void *pSource)
{
#if 0 //TODO - Need to bring in Volume Model
    DSPSource_t source = * ((DSPSource_t *)pSource);
    if ( myDspProfile.sourceSelect != (uint16_t)source )
    {
        BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
        myDspProfile.sourceSelect = source;
        myDspProfile.dualMonoSelect = VolumeModel_GetAudioParam(VM_DUAL_MONO);
        myDspProfile.targetLatencyMs = VolumeModel_GetAVSync();
        IpcDsp_SendProfile(&myDspProfile);
        BMutex_Give(dspDevice->StatusMutex);
    }
#endif
}

void DeviceModel_DSP_SetSpeakerPackageAndFlags(IpcSpeakerPackage_t speakerPackage, IpcAccessoryFlags_t acessoryflags)
{
    if (myDspProfile.speakerPackage != (uint8_t)speakerPackage || 
        myDspProfile.accessoryFlags != (uint8_t)acessoryflags)
    {
        BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
        myDspProfile.speakerPackage = speakerPackage;
        myDspProfile.accessoryFlags = acessoryflags;
        IpcDsp_SendProfile(&myDspProfile);
        BMutex_Give(dspDevice->StatusMutex);
    }
}

void DeviceModel_DSP_SourceMute_Get(void *pSourceMute)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((BOOL *)pSourceMute) = isSourceMute;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_SourceMute_Set(void *pSourceMute)
{
    BOOL source_mute = * ((BOOL *)pSourceMute);
    if ( isSourceMute != source_mute )
    {
        isSourceMute = source_mute;
        SendVolumeMute ();  // This function is protected by semaphore
    }
}

void DeviceModel_DSP_InternalMute_Get(void *pInternalMute)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((BOOL *)pInternalMute) = isInternalMute;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_InternalMute_Set(void *pInternalMute)
{
    BOOL internal_mute = * ((BOOL *)pInternalMute);
    if ( isInternalMute != internal_mute )
    {
        isInternalMute = internal_mute;
        SendVolumeMute ();  // This function is protected by semaphore
    }
}

void DeviceModel_DSP_UserMute_Get(void *pUserMute)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
#if 0 //TODO - Need to bring in Volume Model
    *((BOOL *)pUserMute) = VolumeModel_GetAudioParam(VM_USER_MUTE);
#endif
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_UserMute_Set(void *pUserMute)
{
    SendVolumeMute ();  // This function is protected by semaphore
}

void DeviceModel_DSP_Volume_Get(void *pVolume)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
#if 0 //TODO - Need to bring in Volume Model
    *((uint8_t *)pVolume) = VolumeModel_GetAudioParam(VM_VOLUME);
#endif
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_Volume_Set(void *pVolume)
{
    SendVolumeMute ();  // This function is protected by semaphore
}

void DeviceModel_DSP_PowerState_Get(void *pPowerState )
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    *((Device_PowerState *)pPowerState) = dspDevice->PowerState;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_PowerState_Set(void* pPowerState)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);

    myDspProfile.powerState = * ((uint8_t *)pPowerState);
    IpcDsp_SendProfile(&myDspProfile);
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_HSPowerState_Get(void *pPowerState )
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    *((IpcPowerState_t *)pPowerState) = (IpcPowerState_t)dspDevice->CurrentStatus.dspHS.powerState;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_HSPowerState_Set(void* pPowerState)
{
    BMutex_Take(dspDevice->StatusMutex, portMAX_DELAY);
    // Field is read only so the API is just placeholder
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_SetToneControl(void)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
#if 0 //TODO - Need to bring in Volume Model
    myDspProfile.trebleLevel = VolumeModel_GetAudioParam(VM_TREBLE);
    myDspProfile.bassLevel = VolumeModel_GetAudioParam(VM_BASS);
    myDspProfile.centerLevel = VolumeModel_GetAudioParam(VM_CENTER);
    myDspProfile.surroundLevel = VolumeModel_GetAudioParam(VM_SURROUND);
#endif
    IpcDsp_SendProfile(&myDspProfile);
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_Temperature_Set(IpcThermistorData_t *pTemperature)
{
   IpcThermistorData_t* temperature = pTemperature;

   if(temperature->thermLocation >= IPC_THERM_LOCATION_TYPES) return;

   BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
   DspTemperatureData[temperature->thermLocation] = temperature->tempDegC;
   BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_LipSyncDelay_Get(void *pLipSyncDelay)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint16_t *)pLipSyncDelay) = myDspProfile.targetLatencyMs;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_LipSyncDelay_Set(void* pDelay)
{
    uint16_t delay = *(uint16_t*) pDelay;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    if (myDspProfile.targetLatencyMs != delay)
    {
        myDspProfile.targetLatencyMs = delay;
        IpcDsp_SendProfile(&myDspProfile);

        // SM2 needs to know when this changes
        DeviceModelTask_SendLPMHealthAndStatusToAllDevices();
    }
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_NetworkLatency_Get(void *pNetworkLatency)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint16_t *)pNetworkLatency) = myDspProfile.networkLatencyMs;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_NetworkLatency_Set(void* pNetworkLatency)
{
    uint16_t latency = *(uint16_t*) pNetworkLatency;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    if (myDspProfile.networkLatencyMs != latency)
    {
        myDspProfile.networkLatencyMs = latency;
        IpcDsp_SendProfile(&myDspProfile);
    }
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_AudioMode_Get(void *pAudioMode)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint8_t *)pAudioMode) = myDspProfile.audioMode;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_AudioMode_Set(void* pAudioMode)
{
    uint8_t mode = *(uint8_t*) pAudioMode;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    if (myDspProfile.audioMode != mode)
    {
        myDspProfile.audioMode = mode;
        IpcDsp_SendProfile(&myDspProfile);

        // SM2 needs to know when this changes
        DeviceModelTask_SendLPMHealthAndStatusToAllDevices();
    }
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_BootAiqCurveB_Get(void *pAiqBootAiqCurveB)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((BOOL *)pAiqBootAiqCurveB) = bootAiqCurveB;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_BootAiqCurveB_Set(void *pAiqBootAiqCurveB)
{
    BOOL enable = *(BOOL *) pAiqBootAiqCurveB;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    bootAiqCurveB = enable;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_AiqEnable_Get(void *pAiqEnable)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint8_t *)pAiqEnable) = myDspProfile.aiqEnable;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_AiqEnable_Set(void *pAiqEnable)
{
    uint8_t enable = *(uint8_t *) pAiqEnable;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    if (myDspProfile.aiqEnable != enable)
    {
        myDspProfile.aiqEnable = enable;
        NV_SetAIQEnabled(enable);
        IpcDsp_SendProfile(&myDspProfile);
    }
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_AiqCurveSelect_Get(void *pAiqCurveSelect)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint8_t *)pAiqCurveSelect) = myDspProfile.aiqCurveSelect;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_AiqCurveSelect_Set(void *pAiqCurveSelect)
{
    uint8_t curve = *(uint8_t *) pAiqCurveSelect;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    if (myDspProfile.aiqCurveSelect != curve)
    {
        myDspProfile.aiqCurveSelect = curve;
        IpcDsp_SendProfile(&myDspProfile);
    }
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_DualMonoSelect_Get(void *pDualMonoSelect)
{

    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint8_t *)pDualMonoSelect) = myDspProfile.dualMonoSelect;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_DualMonoSelect_Set(void *pDualMonoSelect)
{
    uint8_t dualMono = *(uint8_t *) pDualMonoSelect;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    if (myDspProfile.dualMonoSelect != dualMono)
    {
        myDspProfile.dualMonoSelect = dualMono;
        IpcDsp_SendProfile(&myDspProfile);
    }
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_LanguageSelect_Get(void *pLanguageSelect)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint8_t *)pLanguageSelect) = myDspProfile.languageSelect;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_LanguageSelect_Set(void *pLanguageSelect)
{
    uint8_t language = *(uint8_t *) pLanguageSelect;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    if (myDspProfile.languageSelect != language)
    {
        myDspProfile.languageSelect = language;
        IpcDsp_SendProfile(&myDspProfile);
    }
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_Region_Get(void *pRegion)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint8_t *)pRegion) = myDspProfile.region;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_Region_Set(void *pRegion)
{
    uint8_t region = *(uint8_t *) pRegion;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    if (myDspProfile.region != region)
    {
        myDspProfile.region = region;
        IpcDsp_SendProfile(&myDspProfile);
    }
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_FrontConnector_Get(void *pFrontConnector)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint8_t *)pFrontConnector) = myDspProfile.frontConnector;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_FrontConnector_Set(void *pFrontConnector)
{
    uint8_t frontConnector = *(uint8_t *) pFrontConnector;
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    if (myDspProfile.frontConnector != frontConnector)
    {
        myDspProfile.frontConnector = frontConnector;
        IpcDsp_SendProfile(&myDspProfile);
    }
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_AudioInputFormat_Get(void *format)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint32_t *)format) = dspDevice->CurrentStatus.dspHS.audioFormat;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_AudioInputFormat_Set(void *dumby)
{
}

void DeviceModel_DSP_SampleRate_Get(void *sampleRate)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint32_t *)sampleRate) = dspDevice->CurrentStatus.dspHS.sampleRate;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_SampleRate_Set(void *dumby)
{
}

void DeviceModel_DSP_FullRangeChannels_Get(void *fullRangeChannels)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint32_t *)fullRangeChannels) = dspDevice->CurrentStatus.dspHS.fullRangeChannels;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_FullRangeChannels_Set(void *dumby)
{
}

void DeviceModel_DSP_LfeChannels_Get(void *lfeChannels)
{
    BMutex_Take(dspDevice->StatusMutex, BLOCK_FOREVER);
    *((uint32_t *)lfeChannels) = dspDevice->CurrentStatus.dspHS.lfeChannels;
    BMutex_Give(dspDevice->StatusMutex);
}

void DeviceModel_DSP_LfeChannels_Set(void *dumby)
{
}

static void DeviceModel_DSP_TurnOff(void)
{
    //WiredBass_DC_TX_SetState(FALSE);
    timerStop(DspTemperatureTimer, 0);
    BUSART_Cmd(DSP_BUART, PS_DISABLE);
    BUSART_DMACmd(DSP_BUART, DMA_TX_RX, PS_DISABLE);
    vTaskDelay(TIMER_MSEC_TO_TICKS(50));
    DEASSERT_GPIO_PIN_FG( DSP_RST_L_GPIO_BANK, DSP_RST_L_GPIO_PIN);
    dspDevice->PowerState = POWERSTATE_STANDBY;
    dspDevice->HealthState = H_S_OFF;
    LOG(device_model_dsp,ROTTEN_LOGLEVEL_VERBOSE,"%s : H_S_OFF", dspDevice->Name);
}

static void DeviceModel_DSP_TurnOn(void)
{
    DEASSERT_GPIO_PIN_FG( DSP_RST_L_GPIO_BANK, DSP_RST_L_GPIO_PIN);
    vTaskDelay(TIMER_MSEC_TO_TICKS(200));
    ASSERT_GPIO_PIN_FG( DSP_RST_L_GPIO_BANK, DSP_RST_L_GPIO_PIN);

    changeTimerPeriod(dspDevice->Timer, TIMER_MSEC_TO_TICKS(dspDevice->Delays.values[DEVICE_BOOT_DELAY]), 0);
    timerStart(dspDevice->Timer , 0, &dspDevice->Task->Queue);
    
    //dspDevice->PowerState = POWERSTATE_BOOTING;
    //dspDevice->HealthState = H_S_BOOT_WAITING;
    //TODO - POWERSTATE_BOOTING & H_S_BOOT_WAITING causes a fault at nv_system.c:99
    dspDevice->PowerState = POWERSTATE_ON;
    dspDevice->HealthState = H_S_NORMAL;
    
    LOG(device_model_dsp,ROTTEN_LOGLEVEL_VERBOSE,"%s : H_S_BOOT_WAITING", dspDevice->Name);
    vTaskDelay(TIMER_MSEC_TO_TICKS(100));
    BUSART_Cmd(DSP_BUART, PS_ENABLE);
    BUSART_DMACmd(DSP_BUART, DMA_TX_RX, PS_ENABLE);
}

static void DeviceModel_DSP_SendTemperatureData(void)
{
    IpcDsp_SendSysTemperatureToDSP(DspTemperatureData);
    timerStart(DspTemperatureTimer, 0, &dspDevice->Task->Queue);
}

void DeviceModel_DSP_RebootToImage(DSPImage_t image)
{
    if(image >= DSP_NUM_IMAGES)
    {
        LOG(device_model_dsp, ROTTEN_LOGLEVEL_NORMAL, "Invalid DSP image: %d", image);
        return;
    }
    else if(image == INSTALLER_IMAGE)
    {
        doSoftReset = TRUE;
        hasBootedToInstaller = FALSE;
    }

    IpcDsp_SendRebootToImage(image);
}

BOOL DeviceModel_DSP_HasBootedToInstaller(void)
{
    return hasBootedToInstaller;
}

static void DeviceModel_DSP_SetBootedToInstallerFlag(DSPImage_t image)
{
    if(image == INSTALLER_IMAGE)
    {
        hasBootedToInstaller = TRUE;
    }
}

BOOL DeviceModel_DSP_GetBassEnableState(void)
{
  return dspDevice->CurrentStatus.dspHS.bassEnableRequest;
}