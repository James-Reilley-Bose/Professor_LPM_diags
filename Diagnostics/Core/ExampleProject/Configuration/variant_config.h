/*
    File    :   variant_config.h
    Author  :   Dillon Johnson
    Created :   12/19/16
    Language:   C
    Copyright:  (C) 2016 Bose Corporation, Framingham, MA

    Description:   All GPIO pin descriptions, clocks, buses, and
                   any other board level definitions. Initially taken
                   from the software trunk.

===============================================================================
*/

#ifndef VARIANT_CONFIG_H
#define VARIANT_CONFIG_H

#include "example_pin_mapping.h"
#include "example_memory_map.h"
#include "example_ext_flash_map.h"

#define VARIANT_NAME "Example"
#define EXAMPLE_VARIANT

/* PLLI2S will be used to provide 27MHz output via MCO2 */
// to get 27Mhz out, CLK_IN / PLLM * PLLI2S_N / PLLI2S_R = 54 Mhz
// If we keep CLK_IN / PLLM = 1MHz, then these values will always work. Then use RCC_MCO2Div_2 to get 27MHz
#define PLLI2S_N               (216)
#define PLLI2S_R               (4)

/**********************************************************
* System measurement timer (free running timer)
**********************************************************/
#define TIME_MEASURE_TIMER                  TIM2

/**********************************************************
* Peripheral clocks (enabled at bootup)
**********************************************************/
#define BUS_AHB1_PERIPHERAL_SET ( \
    RCC_AHB1Periph_DMA1   | \
    RCC_AHB1Periph_DMA2   | \
    RCC_AHB1Periph_GPIOA  | \
    RCC_AHB1Periph_GPIOB  | \
    RCC_AHB1Periph_GPIOC  | \
    RCC_AHB1Periph_GPIOD  | \
    RCC_AHB1Periph_GPIOE  | \
    RCC_AHB1Periph_GPIOF  | \
    RCC_AHB1Periph_GPIOG  | \
    RCC_AHB1Periph_GPIOH  | \
    RCC_AHB1Periph_GPIOI  )

#define BUS_APB1_PERIPHERAL_SET ( RCC_APB1Periph_TIM2   | \
                                  RCC_APB1Periph_TIM4   | \
                                  RCC_APB1Periph_TIM5   | \
                                  RCC_APB1Periph_I2C1   | \
                                  RCC_APB1Periph_I2C2   | \
                                  RCC_APB1Periph_I2C3   | \
                                  RCC_APB1Periph_USART2 | \
                                  RCC_APB1Periph_UART5 )

#define BUS_APB2_PERIPHERAL_SET ( RCC_APB2Periph_TIM1   | \
                                  RCC_APB2Periph_TIM11  | \
                                  RCC_APB2Periph_USART6 | \
                                  RCC_APB2Periph_SPI1   | \
                                  RCC_APB2Periph_USART1 | \
                                  RCC_APB2Periph_SYSCFG )

/* Include Debugging Strings */
//#define INCLUDE_EVENT_STRINGS

// Board-specific features
#define LPM_HAS_TAP
#define LPM_HAS_LOG_SERIAL_STORE
#define LPM_HAS_POWER_MANAGER
#define LPM_HAS_KEY_HANDLER
#define LPM_HAS_TIMER_EXAMPLE

#endif  // VARIANT_CONFIG_H
