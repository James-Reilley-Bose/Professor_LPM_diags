//
// WirelessAudioISR.c
//

#include "project.h"
#include "WirelessAudioISR.h"

void WirelessAudio_ConfigureEXTI(void)
{
   ConfigureGpioPin(DWAM_INTERRUPT_BANK,
                    DWAM_INTERRUPT_PIN,
                    GPIO_MODE_IN_FLOATING,
                    0 );

   SYSCFG_EXTILineConfig (DWAM_INTERRUPT_PORT_SOURCE, DWAM_INTERRUPT_LINE_NUM);

   WirelessAudio_EnableEXTI(FALSE);

   Interrupt_RegisterISR(DWAM_INTERRUPT_CHANNEL, DWAM_INTERRUPT_HANDLER);
   Interrupt_Enable(DWAM_INTERRUPT_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
}

void WirelessAudio_EnableEXTI(BOOL enable)
{
    EXTI_InitTypeDef EXTI_InitStructure = {
        .EXTI_Mode    = EXTI_Mode_Interrupt,
        .EXTI_Line    = DWAM_INTERRUPT_PIN_MASK,
        .EXTI_Trigger = EXTI_Trigger_Falling,
    };

    EXTI_InitStructure.EXTI_LineCmd = enable ? ENABLE : DISABLE;

    EXTI_Init(&EXTI_InitStructure);
}
