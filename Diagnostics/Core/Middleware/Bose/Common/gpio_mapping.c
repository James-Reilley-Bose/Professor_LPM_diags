/*************************************************************************//**
* GPIO interfacing.
*****************************************************************************/
#include "configuresystem.h"
#include "instrument.h"

static BOOL GPIO_IsOutputType(GPIO_MODE_t mode);

void ConfigureGpioPin(GPIO_TypeDef* a_pGpioBank, unsigned a_pinNum,
                      GPIO_MODE_t mode, uint8_t af)
{
    if (a_pinNum > GPIO_PinSource15)
    {
        debug_assert (0);
        return;
    }

    BOOL        doAF    = FALSE;
    uint16_t    pinMask = (0x0001) << a_pinNum;

    /* GPIO_InitTypeDef given in stm32f2xx_gpio.h
    *
    *  GPIO_InitStructure.GPIO_Mode
    *  GPIO_InitStructure.GPIO_Speed
    *  GPIO_InitStructure.GPIO_OType
    *  GPIO_InitStructure.GPIO_PuPd
    *  GPIO_InitStructure.GPIO_Pin
    */
    GPIO_InitTypeDef  initStruct;

    initStruct.GPIO_Pin = pinMask;

    switch (mode)
    {
        case GPIO_MODE_OUT_PP:
            initStruct.GPIO_Mode  = GPIO_Mode_OUT;
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_50MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            break;

        case GPIO_MODE_OUT_PP_UP:
            initStruct.GPIO_Mode  = GPIO_Mode_OUT;
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_50MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_UP;
            break;

        case GPIO_MODE_OUT_PP_DOWN:
            initStruct.GPIO_Mode  = GPIO_Mode_OUT;
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_50MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;
            break;

        case GPIO_MODE_OUT_OD:
            initStruct.GPIO_Mode  = GPIO_Mode_OUT;
            initStruct.GPIO_OType = GPIO_OType_OD;
            initStruct.GPIO_Speed = GPIO_Speed_50MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            break;

        case GPIO_MODE_OUT_OD_PULLUP:
            initStruct.GPIO_Mode  = GPIO_Mode_OUT;
            initStruct.GPIO_OType = GPIO_OType_OD;
            initStruct.GPIO_Speed = GPIO_Speed_50MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_UP;
            break;

        case GPIO_MODE_IN_ANALOG:
            initStruct.GPIO_Mode  = GPIO_Mode_AIN;
            initStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            break;

        case GPIO_MODE_IN_FLOATING:
            initStruct.GPIO_Mode  = GPIO_Mode_IN;
            initStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            break;

        case GPIO_MODE_IN_PULLUP:
            initStruct.GPIO_Mode  = GPIO_Mode_IN;
            initStruct.GPIO_PuPd  = GPIO_PuPd_UP;
            break;

        case GPIO_MODE_IN_PULLDN:
            initStruct.GPIO_Mode  = GPIO_Mode_IN;
            initStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;
            break;

        case GPIO_MODE_AF_OUT_PP:
        case GPIO_MODE_AF_OUT_PP_2MHZ:
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_2MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;

            doAF = TRUE;
            break;

        case GPIO_MODE_AF_OUT_PP_100MHZ:
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_100MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;

            doAF = TRUE;
            break;

        case GPIO_MODE_AF_OUT_PP_50MHZ:
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_50MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;

            doAF = TRUE;
            break;

        case GPIO_MODE_AF_OUT_PP_PULLDOWN_100MHZ:
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_100MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;

            doAF = TRUE;
            break;

        case GPIO_MODE_AF_OUT_OD_50MHZ:
            initStruct.GPIO_OType = GPIO_OType_OD;
            initStruct.GPIO_Speed = GPIO_Speed_50MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_UP;

            doAF = TRUE;
            break;

        case GPIO_MODE_AF_OUT_OD:
        case GPIO_MODE_AF_OUT_OD_2MHZ:
            initStruct.GPIO_OType = GPIO_OType_OD;
            initStruct.GPIO_Speed = GPIO_Speed_2MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_UP;

            doAF = TRUE;
            break;

        case GPIO_MODE_AF_OUT_OD_25MHZ:
            initStruct.GPIO_OType = GPIO_OType_OD;
            initStruct.GPIO_Speed = GPIO_Speed_25MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_UP;

            doAF = TRUE;
            break;

        case GPIO_MODE_AF_IN_FLOATING:
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_2MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;

            doAF = TRUE;
            break;

        case GPIO_MODE_AF_IN_PULLUP:
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_2MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_UP;

            doAF = TRUE;
            break;

        case GPIO_MODE_AF_IN_PULLDOWN:
            initStruct.GPIO_OType = GPIO_OType_PP;
            initStruct.GPIO_Speed = GPIO_Speed_2MHz;
            initStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;

            doAF = TRUE;
            break;


        /* These are not written yet as nothing needs them  - they are here for
        * completeness.
        */
        case GPIO_MODE_AF_OUT_PP_25MHZ:
        case GPIO_MODE_AF_OUT_OD_100MHZ:
        default:
            debug_assert (0);
    }

    if (doAF)
    {
        initStruct.GPIO_Mode = GPIO_Mode_AF;
        GPIO_PinAFConfig (a_pGpioBank, a_pinNum, af);
    }

    __disable_irq();
    GPIO_Init (a_pGpioBank, &initStruct);
    __enable_irq();
}


/*****************************************************************//**
* @brief    Get current GPIO configuration
*
* @param    GPIOx - Pointer to system's GPIO bank structure
* @param    a_pinNum - Pin number
* @param    pGpioInitTypeDef - Pointer to GPIO's init structure
*
* @return   TRUE if succeeded, the current configuration is pointed to by pGpioInitTypeDef;
*           FALSE otherwise
*
**********************************************************************/
BOOL ReadGpioConfiguration(GPIO_TypeDef* GPIOx, unsigned a_pinNum,
                           GPIO_InitTypeDef* pGpioInitTypeDef)
{
    if (pGpioInitTypeDef == NULL)
    {
        return FALSE;
    }

    if (IS_GPIO_ALL_PERIPH(GPIOx) == FALSE)
    {
        return FALSE;
    }

    if (a_pinNum > GPIO_PinSource15)
    {
        return FALSE;
    }

    pGpioInitTypeDef->GPIO_Pin = 1 << a_pinNum;

    // Read mode register
    uint32_t dummy = GPIOx->MODER;

    pGpioInitTypeDef->GPIO_Mode = (GPIOMode_TypeDef)((dummy >> (a_pinNum * 2)) & 0x00000003);

    if ((pGpioInitTypeDef->GPIO_Mode == GPIO_Mode_OUT) || (pGpioInitTypeDef->GPIO_Mode == GPIO_Mode_AF))
    {
        // Read Speed parameters
        dummy = GPIOx->OSPEEDR;
        pGpioInitTypeDef->GPIO_Speed = (GPIOSpeed_TypeDef)((dummy >> (a_pinNum * 2)) & 0x00000003);

        // Read Output type
        dummy = GPIOx->OTYPER;
        pGpioInitTypeDef->GPIO_OType = (GPIOOType_TypeDef)((dummy >> a_pinNum) & 0x00000001);
    }

    // Read Pull-up Pull down resistor
    dummy = GPIOx->PUPDR;
    pGpioInitTypeDef->GPIO_PuPd = (GPIOPuPd_TypeDef)((dummy >> (a_pinNum * 2)) & 0x00000003);

    return TRUE;
}

void ConfigureGpioPins(const GPIO_INIT_t* pins, unsigned int length)
{
    debug_assert(pins);
    for (unsigned int i = 0; i < length; i++)
    {
        if (GPIO_IsOutputType(pins[i].mode) && (pins[i].initialState != GPIO_NONE))
        {
            BitAction action = (pins[i].initialState == GPIO_LOW) ? Bit_RESET : Bit_SET;
            GPIO_WriteBit(pins[i].bank, (1 << pins[i].pin), action);
        }
        ConfigureGpioPin(pins[i].bank, pins[i].pin, pins[i].mode, pins[i].af);
    }
}

void WriteGPIO(GPIO_TypeDef* bank, uint16_t pin, GPIO_LOGIC_t activeState, BOOL assert)
{
    debug_assert(activeState != GPIO_NONE);
    BitAction bit = Bit_RESET;
    if(activeState == GPIO_HIGH)
    {
        bit = assert ? Bit_SET : Bit_RESET;
    }
    else
    {
        bit = assert ? Bit_RESET : Bit_SET;
    }
    GPIO_WriteBit(bank, (1 << pin), bit);
}

static BOOL GPIO_IsOutputType(GPIO_MODE_t mode)
{
    return ((mode == GPIO_MODE_OUT_PP) ||
            (mode == GPIO_MODE_OUT_OD) ||
            (mode == GPIO_MODE_OUT_OD_PULLUP));
}
