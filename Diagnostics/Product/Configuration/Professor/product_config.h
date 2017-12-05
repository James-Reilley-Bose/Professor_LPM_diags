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

#define VARIANT_NAME "Professor"
#define PROFESSOR
#define PRODUCT_VARIANT_DEFAULT PRODUCT_PROFESSOR
#define PRODUCT_HARDWARE_VARIANT 1
#define LPM_UPDATE_FILENAME "professor.bin"
#define LPM_MFG_UPDATE_FILENAME "professor_mfg.bin"
#define LPM_BL_UPDATE_FILENAME "professor_bl.bin"
#define WA_VARIANT_DARR_FILE_NAME "darr.bin"
#define BLOB_IN_NVRAM
#define LPM_HAS_LIGHTBARCAPSENSE

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
    RCC_AHB1Periph_GPIOI  | \
    RCC_AHB1Periph_CRC )

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
                                  RCC_APB2Periph_SYSCFG )

void EXTI1_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void PSoCTask_ConfigureEXTI(void);
void PSoCInt_en_dis(int en);
void Psoc_Int_Handler(void);

/* HDMI Configurations */
typedef enum
{
    HDMI_PORT_START = 0,
    NUM_HDMI_PORTS = HDMI_PORT_START,
    HDMI_PORT_NONE,
} hdmi_port;

#define LIGHTBAR_NUM_OF_LEDS           (24)
#define LIGHTBAR_LED_INTENSITY_SIZE    (2) //in bytes
#define LIGHTBAR_PATTERN_DURATION_SIZE (2) //in bytes
#define LIGHTBAR_PATTERN_SIZE          ((LIGHTBAR_NUM_OF_LEDS * LIGHTBAR_LED_INTENSITY_SIZE) + LIGHTBAR_PATTERN_DURATION_SIZE) //in bytes

/* Include Debugging Strings */
#define INCLUDE_EVENT_STRINGS
#define INCLUDE_UPDATE_MANAGER_STRINGS

// Board-specific features
#define LPM_HAS_TAP
#define LPM_HAS_LOG_SERIAL_STORE
#define LPM_HAS_POWER_MANAGER
#define LPM_HAS_KEY_HANDLER
#define LPM_HAS_UPDATE_MANAGER
#define LPM_HAS_INTER_PROCESSOR_COMM
#define LPM_HAS_SPI_IPC
#define LPM_HAS_STANDBY_CONTROLLER
#define LPM_HAS_DSP
#define LPM_HAS_USER_INTERFACE
#define LPM_HAS_SOURCE_CHANGE
#define LPM_HAS_SII9437_CHIP
#define LPM_HAS_UEI_BLASTER
#define LPM_HAS_WIRELESS_AUDIO
//#define LPM_HAS_LIGHTBARCAPSENSE
#define LPM_HAS_WIRED_BASS_DETECT
#define LPM_HAS_RF_REMOTE
#define INCLUDE_DIAGS_TEST
#define HAS_SYSPARAMS_NVRAM 1
#define IS_CONSOLE 1
#define SUPPORT_DSPWATCHDOG  1

#define PON_RESET_DISCONNECTED 1
    
#define LPM_SUPPORT_KEY_REPEAT  0    

#endif  // PRODUCT_CONFIG_H
