#include "project.h"
#include "etapGpio.h"
#include "etap.h"
#include "gpio_mapping.h"


// Local prototypes
static void TAP_SetGpioPin(GPIO_TypeDef* pGpioBank, uint8_t pinNumber, uint8_t action);
static void TAP_GetGpioPin(GPIO_TypeDef* pGpioBank, uint8_t pinNumber);
static GPIO_TypeDef* getGpioBankFromChar(char gpioBank);

TAPCommand(TAP_GpioCommand)
{
    char action, gpioBank;
    uint8_t pinNumber;

    if (CommandLine->numArgs < MINIMAL_GPIO_NUMBER_ARGUMENTS)
    {
        TAP_PrintString(TAP_InvalidArg);
        return;
    }

    BOOL valid = FALSE;

    // Get 3 args
    action = TAP_ToLowerCase(CommandLine->args[0][0]);
    gpioBank = TAP_ToLowerCase(CommandLine->args[1][0]);
    pinNumber = TAP_DecimalArgToInt(CommandLine, 2, &valid);

    GPIO_TypeDef* pGpioBank = getGpioBankFromChar(gpioBank);

    if (!valid || (pinNumber >= MAX_GPIO_PIN_NUMBER) || !pGpioBank )
    {
        TAP_PrintString(TAP_InvalidArg);
        return;
    }

    // Start action
    switch (action)
    {
        case 'h':
        case 'l':
        case 't':
            TAP_SetGpioPin(pGpioBank, pinNumber, action);
            break;
        case 'r':
            TAP_GetGpioPin(pGpioBank, pinNumber);
            break;
        default:
            TAP_PrintString(TAP_InvalidArg);
            return;
    }

    TAP_PrintString(TAP_OKText);
    TAP_PrintString("\r\n");
}

/*****************************************************************//**
* @brief    Get system's GPIO bank from input char
*
* @param    gpioBank - User input for gpio bank
*
* @return   Non-Null pointer to the systems GPIO bank structure, NULL in failure
*
**********************************************************************/
static GPIO_TypeDef* getGpioBankFromChar(char gpioBank)
{
    GPIO_TypeDef* pGpioBank;

    switch (gpioBank)
    {
        case 'a':
            pGpioBank = GPIOA;
            break;

        case 'b':
            pGpioBank = GPIOB;
            break;

        case 'c':
            pGpioBank = GPIOC;
            break;

        case 'd':
            pGpioBank = GPIOD;
            break;

        case 'e':
            pGpioBank = GPIOE;
            break;

        case 'f':
            pGpioBank = GPIOF;
            break;

        case 'g':
            pGpioBank = GPIOG;
            break;

        case 'h':
            pGpioBank = GPIOH;
            break;

        case 'i':
            pGpioBank = GPIOI;
            break;
        default:
            pGpioBank = NULL;
            break;
    }

    return pGpioBank;
}


/*****************************************************************//**
* @brief    Set a pin state according to inputs
*
* @param    pGpioBank - Pointer to system's GPIO bank structure
* @param    pinNumber - Pin number
* @param    action - what to do
*
**********************************************************************/
static void TAP_SetGpioPin(GPIO_TypeDef* pGpioBank, uint8_t pinNumber, uint8_t action)
{
    GPIO_InitTypeDef gpioInitStruct;

    if (ReadGpioConfiguration(pGpioBank, pinNumber, &gpioInitStruct) == FALSE)
    {
        TAP_Printf("Read error \n\r");
        return;
    }

    if (gpioInitStruct.GPIO_Mode != GPIO_Mode_OUT)
    {
        ConfigureGpioPin(pGpioBank, pinNumber, GPIO_MODE_OUT_PP, 0);
    }

    uint16_t pinMask = 1 << pinNumber;

    switch (action)
    {
        case 'l':
            // Set to low
            GPIO_ResetBits(pGpioBank, pinMask);
            break;
        case 'h':
            // Set to high
            GPIO_SetBits(pGpioBank, pinMask);
            break;
        case 't':
            // Toggle the pin
            GPIO_ToggleBits(pGpioBank, pinMask);
            break;
    }

}


/*****************************************************************//**
* @brief    Read a pin state according to inputs
*           Note that this function does not configure the pin as input
*
* @param    pGpioBank - Pointer to system's GPIO bank structure
* @param    pinNumber - Pin number
*
**********************************************************************/
static void TAP_GetGpioPin(GPIO_TypeDef* pGpioBank, uint8_t pinNumber)
{
    GPIO_InitTypeDef gpioInitStruct;

    if (ReadGpioConfiguration(pGpioBank, pinNumber, &gpioInitStruct) == FALSE)
    {
        TAP_Printf("Read error \n\r");
        return;
    }

    uint8_t state;

    if (gpioInitStruct.GPIO_Mode == GPIO_Mode_IN)
    {
        state = GPIO_ReadInputDataBit(pGpioBank, 0x0001 << pinNumber);
    }
    else if (gpioInitStruct.GPIO_Mode == GPIO_Mode_OUT)
    {
        state = GPIO_ReadOutputDataBit(pGpioBank, 0x0001 << pinNumber);
    }
    else
    {
        TAP_Printf("Read error: Not configured as input/output \n\r");
        return;
    }

    if (state == Bit_SET)
    {
        TAP_Printf("High \n\r");
    }
    else
    {
        TAP_Printf("Low \n\r");
    }
}
