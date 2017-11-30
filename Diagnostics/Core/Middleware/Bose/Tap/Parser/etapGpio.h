#ifndef _ETAPGPIO_H_
#define _ETAPGPIO_H_

#include "etapcoms.h"

/**
* @DOCETAP
* COMMAND:   gpio
*
* ALIAS:     io
*
* DESCRIP:   Set or get a GPIO pin state based on GPIO bank and pin number.
*            Warning: Set GPIO pin state may change its initial configuration.
*
* PARAM:     0        1         2
*            action   gpioBank  pinNumber
*
*            action takes values: h, l, t, r (for high, low, toggle, read respectively);
*            gpioBank takes values: A, B, C, D, E, F, G, H, I
*            pinNumber takes values: 0 - 15
*
* REPLY:     TAP_OK if no error or errors will be either printed or logged
*
* EXAMPLE:   io h,B,2 -  Set PortB pin 2 to high;
*            io l,D,14 - Set PortD pin 14 to low;
*            io t,A,5 -  Toggle PortA pin 5, (high to low or low to high);
*            io r,C,4 -  Read PortC pin 4 state
*
* @DOCETAPEND
*
*/

#define GPIO_HELP_TEXT "* Use this command to set/get GPIO pin state, Usage :\n\r" \
                     "\t io h,B,2\n\r" \
                     "\t io l,D,14\n\r" \
                     "\t io t,A,5 \n\r" \
                     "\t io r,C,4"

void TAP_GpioCommand(CommandLine_t*);

#define MINIMAL_GPIO_NUMBER_ARGUMENTS        3
#define MAX_GPIO_PIN_NUMBER                 16

#endif //_ETAPGPIO_H_
