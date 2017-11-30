//
// SystemReboot.c
//

#include "project.h"

// Disable a few things and reboot
void system_core_reboot(void)
{
    // Shut down the UART so we don't see
    // junk on the TAP output
    USART_DeInit(TAP_UART);

    // not really necessary, but doesn't hurt
    __disable_irq();

    // resets EXTI (which I think we need here)
    // and the other alternate functions
    SYSCFG_DeInit();

    // Now reboot
    NVIC_SystemReset();
}

// Override this with a variant specific routine
// if other functionality is needed.
__weak void system_reboot(void)
{
    system_core_reboot();
}
