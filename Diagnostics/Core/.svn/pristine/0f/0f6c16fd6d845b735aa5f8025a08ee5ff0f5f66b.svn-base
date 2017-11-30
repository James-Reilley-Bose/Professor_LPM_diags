//
// PowerExample.c - An example power sequence. This includes mostly
//     psuedocode but provides example steps that may be taken
//     to transition between power levels.
//

#include "project.h"
#include "PowerTask.h"

SCRIBE_DECL(power);

void PowerVariant_Init(void)
{

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
    // Mute amp
    // Put DSP in reset
    // Configure power ADC
}

void PowerVariant_TransitionLowPowerToNetworkStandby(void)
{
    // Release soundtouch from standby
}

void PowerVariant_TransitionNetworkStandbyToAutoWakeStandby(void)
{
    // Some transitions can be empty
}

void PowerVariant_TransitionAutoWakeStandbyToFullPower(void)
{
    // Enable ADC for temperature detection
    // Enable ADC for power fail detection
    // Enable amps
    vTaskDelay(TIMER_MSEC_TO_TICKS(100));
    // Release "power mute"
}

void PowerVariant_TransitionFullPowerToAutoWakeStandby(void)
{
    // Assert "power mute"
    vTaskDelay(TIMER_MSEC_TO_TICKS(25));
    //Disable amps
    // Disable power fail detection
    // Disable temperature detection
}

void PowerVariant_TransitionAutoWakeStandbyToNetworkStandby(void)
{
    // empty
}

void PowerVariant_TransitionNetworkStandbyToLowPower(void)
{
    // Disable soundtouch
}
