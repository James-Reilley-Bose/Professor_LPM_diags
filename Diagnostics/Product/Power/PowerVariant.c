//
// PowerVariant.c - Product specific power state transitions.
//

#include "project.h"
#include "PowerTask.h"
#include "PowerRiviera.h"
#include "IPCRouterTask.h"
#include "UEITask.h"
#include "WirelessAudioAPI.h"
#include "DeviceModel_DSP.h"
#include "DeviceModelTask.h"
#include "SpiIpcTask.h"
#include "WiredIDTask.h"
#include "WiredIDConsoleTask.h"

SCRIBE_DECL(power);

#define DSP_POWER_OFF_TIMEOUT_MS 1000

#if (PRODUCT_HARDWARE_VARIANT == 0)
static void PowerVariant_SetHDMIState(FunctionalState state);
#endif

static const GPIO_INIT_t PowerPinsInit[] =
{
    {F0_CTRL_RST_L_GPIO_BANK, F0_CTRL_RST_L_GPIO_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {POWER_LP_STANDBY_L_BANK, POWER_LP_STANDBY_L_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {POWER_NW_STANDBY_L_BANK, POWER_NW_STANDBY_L_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {POWER_VLED_BANK, POWER_VLED_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_HIGH},
#if (PRODUCT_HARDWARE_VARIANT != 0)
    {POWER_HDMI_5V_TVOUT_EN_BANK, POWER_HDMI_5V_TVOUT_EN_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {POWER_HDMI_EN_BANK, POWER_HDMI_EN_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {POWER_RIVIERA_EN_BANK, POWER_RIVIERA_EN_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_HIGH},
    {RIVIERA_SYSCLK_EN_BANK, RIVIERA_SYSCLK_EN_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_LOW},
    {DSP_RECOVERY_L_BANK, DSP_RECOVERY_L_PIN, GPIO_MODE_OUT_PP, NO_ALT_FUNC, GPIO_HIGH},
#endif
};

/*
===============================================================================
@fn QS_RST_L_SetState
@brief Enable/Disable RST L
===============================================================================
*/
void QS_RST_L_SetState( FunctionalState state )
{
    if(state == ENABLE)
    {
        ASSERT_GPIO_PIN_FG( QS_RST_L_GPIO_BANK, QS_RST_L_GPIO_PIN );
    }
    else
    {
        DEASSERT_GPIO_PIN_FG( QS_RST_L_GPIO_BANK, QS_RST_L_GPIO_PIN );
    }
}

void PowerVariant_Init(void)
{
    PowerRiviera_Init();

    // TODO - take this out. It's a hack until we pull in source change & UI tasks
#if 0
    PowerTaskPostMsg(POWER_MESSAGE_ID_TransitionLowPowerToNetworkStandby, 0);
    PowerTaskPostMsg(POWER_MESSAGE_ID_TransitionNetworkStandbyToAutoWakeStandby, 0);
    PowerTaskPostMsg(POWER_MESSAGE_ID_TransitionAutoWakeStandbyToFullPower, 0);
#endif
}

void PowerVariant_HandleMessage(GENERIC_MSG_t* message)
{
    switch (message->msgID)
    {
        default:
            LOG(power, ROTTEN_LOGLEVEL_NORMAL, "PV unhandled message %d", message->msgID);
            break;
    }
}

void PowerVariant_TransitionColdBootToLowPower(void)
{
    ConfigureGpioPins(PowerPinsInit, sizeof(PowerPinsInit) / sizeof(GPIO_INIT_t));
    Delay_ms(250);
    ASSERT_GPIO_PIN_FG(F0_CTRL_RST_L_GPIO_BANK, F0_CTRL_RST_L_GPIO_PIN);
    Delay_ms(500);
}

void PowerVariant_TransitionLowPowerToNetworkStandby(void)
{
    vTaskDelay(TIMER_SEC_TO_TICKS(1));
    ASSERT_GPIO_PIN_FG(POWER_LP_STANDBY_L_BANK, POWER_LP_STANDBY_L_PIN);
    ASSERT_GPIO_PIN_FG(POWER_VLED_BANK, POWER_VLED_PIN);
#if (PRODUCT_HARDWARE_VARIANT != 0)
    ASSERT_GPIO_PIN_FG(POWER_RIVIERA_EN_BANK, POWER_RIVIERA_EN_PIN);
    vTaskDelay(TIMER_MSEC_TO_TICKS(10));
#endif
    PowerRiviera_Enable(TRUE);
    SpiIpcPostMsg(SpiIpc_Msg_ID_InitializeSpi, 0, 0);
    QS_RST_L_SetState( ENABLE );
    UEIBlasterModule_PowerInit();
}

void PowerVariant_TransitionNetworkStandbyToAutoWakeStandby(void)
{
#if (PRODUCT_HARDWARE_VARIANT != 0)
    ASSERT_GPIO_PIN_FG(RIVIERA_SYSCLK_EN_BANK, RIVIERA_SYSCLK_EN_PIN);
    vTaskDelay(TIMER_MSEC_TO_TICKS(5)); // not sure if this is necessary
#endif

    DeviceExitStandby(IPC_DEVICE_DSP);    
    vTaskDelay(TIMER_MSEC_TO_TICKS(100));
    ASSERT_GPIO_PIN_FG(POWER_NW_STANDBY_L_BANK, POWER_NW_STANDBY_L_PIN);

#if (PRODUCT_HARDWARE_VARIANT == 0)
    PowerVariant_SetHDMIState(ENABLE);
#else
    ASSERT_GPIO_PIN_FG(POWER_HDMI_EN_BANK, POWER_HDMI_EN_PIN);
    vTaskDelay(TIMER_MSEC_TO_TICKS(30)); // Delay taken from F0 power sequence
    ASSERT_GPIO_PIN_FG(POWER_HDMI_5V_TVOUT_EN_BANK, POWER_HDMI_5V_TVOUT_EN_PIN);
#endif
}

void PowerVariant_TransitionAutoWakeStandbyToFullPower(void)
{
    WirelessAudio_SetStandby(FALSE, FALSE);
    
    // TODO - queue off dsp message
    WiredBassDetectEnable(TRUE);
    WiredIDPostMsg(WIREDID_CONSOLE_MESSAGE_ID_START_SCAN, NOP_CALLBACK, NULL);

    // Start power monitoring

}

void PowerVariant_TransitionFullPowerToAutoWakeStandby(void)
{
    WirelessAudio_SetStandby(TRUE, TRUE);
    
    // TODO - queue off dsp message
    WiredBassDetectEnable(FALSE);
    WiredIDPostMsg(WIREDID_CONSOLE_MESSAGE_ID_BASS_DISABLE, 
                           NOP_CALLBACK, NULL);

    IpcPowerState_t dspPowerState = IPC_POWER_STATE_AUTO_WAKE;
    DeviceModel_DSP_PowerState_Set(&dspPowerState);
    vTaskDelay(TIMER_MSEC_TO_TICKS(250));
}

void PowerVariant_TransitionAutoWakeStandbyToNetworkStandby(void)
{
    DEASSERT_GPIO_PIN_FG(POWER_NW_STANDBY_L_BANK, POWER_NW_STANDBY_L_PIN);

#if (PRODUCT_HARDWARE_VARIANT == 0)
    PowerVariant_SetHDMIState(DISABLE);
#else
    DEASSERT_GPIO_PIN_FG(POWER_HDMI_EN_BANK, POWER_HDMI_EN_PIN);
    DEASSERT_GPIO_PIN_FG(POWER_HDMI_5V_TVOUT_EN_BANK, POWER_HDMI_5V_TVOUT_EN_PIN);
#endif

    IpcPowerState_t dspPowerState = IPC_POWER_STATE_OFF;
    DeviceModel_DSP_PowerState_Set(&dspPowerState);

#if 0 //TODO bring is variant system behaviors
    SystemBehavior_PollDspPowerState(IPC_POWER_STATE_OFF, DSP_POWER_OFF_TIMEOUT_MS);
#endif

    DeviceEnterStandby(IPC_DEVICE_DSP);
    vTaskDelay(TIMER_MSEC_TO_TICKS(400));

#if (PRODUCT_HARDWARE_VARIANT != 0)
    DEASSERT_GPIO_PIN_FG(RIVIERA_SYSCLK_EN_BANK, RIVIERA_SYSCLK_EN_PIN);
#endif
}

void PowerVariant_TransitionNetworkStandbyToLowPower(void)
{
    SpiIpcPostMsg(SpiIpc_Msg_ID_DeInitSpi, 0, 0);
    DEASSERT_GPIO_PIN_FG(POWER_LP_STANDBY_L_BANK, POWER_LP_STANDBY_L_PIN);
    DEASSERT_GPIO_PIN_FG(POWER_VLED_BANK, POWER_VLED_PIN);
    PowerRiviera_Enable(FALSE);
#if (PRODUCT_HARDWARE_VARIANT != 0)
    DEASSERT_GPIO_PIN_FG(POWER_RIVIERA_EN_BANK, POWER_RIVIERA_EN_PIN);
#endif
}

#if (PRODUCT_HARDWARE_VARIANT == 0)
static void PowerVariant_SetHDMIState(FunctionalState state)
{
    F0_PowerStatus_t power =
    {
        .device = POWER_DEVICE_HDMI,
        .enabled = state,
    };
    IPCRouter_Send(IPC_DEVICE_F0, IPC_POWER_STATE_SET, NULL, &power, sizeof(F0_PowerStatus_t));
}
#endif

