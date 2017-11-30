#ifndef INCLUDE_GPIO_MAPPING
#define INCLUDE_GPIO_MAPPING

#include "project.h"

#define NO_ALT_FUNC  0

typedef enum
{
    GPIO_MODE_OUT_PP,
    GPIO_MODE_OUT_PP_UP,
    GPIO_MODE_OUT_PP_DOWN,
    GPIO_MODE_OUT_OD,
    GPIO_MODE_OUT_OD_PULLUP,

    GPIO_MODE_AF_OUT_PP,
    GPIO_MODE_AF_OUT_PP_2MHZ,
    GPIO_MODE_AF_OUT_PP_25MHZ,
    GPIO_MODE_AF_OUT_PP_50MHZ,
    GPIO_MODE_AF_OUT_PP_100MHZ,
    GPIO_MODE_AF_OUT_PP_PULLDOWN_100MHZ,
    GPIO_MODE_AF_OUT_OD,
    GPIO_MODE_AF_OUT_OD_2MHZ,
    GPIO_MODE_AF_OUT_OD_25MHZ,
    GPIO_MODE_AF_OUT_OD_50MHZ,
    GPIO_MODE_AF_OUT_OD_100MHZ,
    GPIO_MODE_AF_IN_FLOATING,
    GPIO_MODE_AF_IN_PULLUP,
    GPIO_MODE_AF_IN_PULLDOWN,

    GPIO_MODE_IN_ANALOG,

    GPIO_MODE_IN_FLOATING,
    GPIO_MODE_IN_PULLUP,
    GPIO_MODE_IN_PULLDN

} GPIO_MODE_t;

typedef struct
{
    GPIO_TypeDef* Bank;
    uint8_t Pin;
    GPIO_MODE_t Mode;
    uint8_t AF;
} GPIO_t;

typedef enum
{
    GPIO_LOW,
    GPIO_HIGH,
    GPIO_NONE
} GPIO_LOGIC_t;

typedef struct
{
    GPIO_TypeDef* bank;
    unsigned int pin;
    GPIO_MODE_t mode;
    uint8_t af;
    GPIO_LOGIC_t initialState;
} GPIO_INIT_t;

void ConfigureGpioPin(GPIO_TypeDef* a_pGpioBank, unsigned a_pinNum,
                      GPIO_MODE_t mode, uint8_t af);

void ConfigureGpioPins(const GPIO_INIT_t* pins, unsigned int length);

void WriteGPIO(GPIO_TypeDef* bank, uint16_t pin, GPIO_LOGIC_t activeState, BOOL assert);

BOOL ReadGpioConfiguration(GPIO_TypeDef* a_pGpioBank, unsigned a_pinNum,
                           GPIO_InitTypeDef* pGpioInitTypeDef);

#endif
