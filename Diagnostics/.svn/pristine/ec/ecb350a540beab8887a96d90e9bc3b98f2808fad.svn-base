/*
    File    :   product_config.h
    Author  :   Dillon Johnson
    Created :   12/19/16
    Language:   C
    Copyright:  (C) 2016 Bose Corporation, Framingham, MA

    Description:   All GPIO pin descriptions, clocks, buses, and
                   any other board level definitions. Initially taken
                   from the software trunk.

===============================================================================
*/

#ifndef PRODUCT_CONFIG_H
#define PRODUCT_CONFIG_H

#include "product_pin_mapping.h"
#include "product_memory_map.h"
#include "product_ext_flash_map.h"

#define VARIANT_NAME "Professor_diags"
#define PROFESSOR
#define PRODUCT_VARIANT_DEFAULT PRODUCT_PROFESSOR
#define PRODUCT_HARDWARE_VARIANT 0
#define LPM_UPDATE_FILENAME "professor.bin"
#define LPM_MFG_UPDATE_FILENAME "professor_mfg.bin"
#define LPM_BL_UPDATE_FILENAME "professor_bl.bin"

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
                                  RCC_APB1Periph_SPI2   | \
                                  RCC_APB1Periph_USART2 | \
 RCC_APB1Periph_USART3 | \
                                    RCC_APB1Periph_UART5 )

#define BUS_APB2_PERIPHERAL_SET ( RCC_APB2Periph_TIM1   | \
                                  RCC_APB2Periph_TIM11  | \
                                  RCC_APB2Periph_USART6 | \
                                  RCC_APB2Periph_SPI1   | \
                                  RCC_APB2Periph_USART1 | \
                                  RCC_APB2Periph_SYSCFG | \
                                  RCC_APB2Periph_ADC    | \
                                  RCC_APB2Periph_ADC1 )

void EXTI9_5_IRQHandler(void);

//#define INCLUDE_EVENT_STRINGS
#define INVERT_RIVIERA_SIGNALS 0

/* HDMI Configurations */
typedef enum
{
    HDMI_PORT_START = 0,
    NUM_HDMI_PORTS = HDMI_PORT_START,
    HDMI_PORT_NONE,
} hdmi_port;

// Board-specific features
#define LPM_HAS_TAP
#define LPM_HAS_LOG_SERIAL_STORE
#define LPM_HAS_POWER_MANAGER
#define LPM_HAS_KEY_HANDLER
//#define LPM_HAS_TIMER_EXAMPLE
#define INCLUDE_DIAGS_TEST
//#define LPM_HAS_CONSOLE_KEYS
#define LPM_HAS_INTER_PROCESSOR_COMM
#define LPM_HAS_DSP_MODEL
#define LPM_HAS_UPDATE_MANAGER
#define LPM_HAS_INTER_PROCESSOR_COMM
#define LPM_HAS_SPI_IPC

#endif  // PRODUCT_CONFIG_H
