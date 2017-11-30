#ifndef CONFIGURESYSTEM_H
#define CONFIGURESYSTEM_H

#include "product_config.h"

/**********************************************************
* defines for clocks
* These are used in SystemClock_Config() in main.c
* They are used to calculate the PLL values.
* Define USE_HSE in the compiler preprocessor
* Define DESIRED_SYS_CLK_MHZ in the compile preprocessor to change
* the desired sysclk. It's default is set in FreeRTOSConfig.h
**********************************************************/
#ifdef USE_HSE
// HSE_VALUE is defined in stm32f2xx.h or override in compiler preprocessor, it is
// used by system_stm32f2x to calculate the clock freq
#define CLK_SRC              HSE_VALUE
#define ENABLE_CLK_SOURCE()  RCC_HSEConfig(RCC_HSE_ON)
#define CLK_RDY_FLAG         (RCC_FLAG_HSERDY)
#define PLL_SRC              (RCC_PLLSource_HSE)
#else
#define CLK_SRC              HSI_VALUE
#define ENABLE_CLK_SOURCE()  RCC_HSICmd(ENABLE)
#define CLK_RDY_FLAG         (RCC_FLAG_HSIRDY)
#define PLL_SRC              (RCC_PLLSource_HSI)
#endif

/**********************************************************
* Clock configurations for the LPM board.
* (These aren't really used in the bootloader or diags code).
* The DESIRED_FLASH_ACR_LATENCY came from STM Datasheet PM0059
* "STM32F205/215, STM32F207/217 Flash programming manual", at
* voltage reange 2.7V - 3.6V.
**********************************************************/
#define SYSTEM_CLOCK_RATE_ON (configCPU_CLOCK_HZ)

#if (DESIRED_SYS_CLK_MHZ == 16)
#define DESIRED_FLASH_ACR_LATENCY FLASH_ACR_LATENCY_0WS
#error "16 Mhz not-currently supported for A4V Products"
//  #if (DESIRED_PCLK_MHZ == 16)
//    #define APB1_DIVIDER     (RCC_HCLK_Div1)
//    #define APB2_DIVIDER     (RCC_HCLK_Div1)
//  #elif (DESIRED_PCLK_MHZ == 4)
//    #define APB1_DIVIDER     (RCC_HCLK_Div4)
//    #define APB2_DIVIDER     (RCC_HCLK_Div4)
//  #else
//    #error "Invalid Peripheral Clock for A4V Products"
//  #endif
#elif (DESIRED_SYS_CLK_MHZ == 24)
#define DESIRED_FLASH_ACR_LATENCY FLASH_ACR_LATENCY_1WS
#if (DESIRED_PCLK_MHZ == 24)
#define APB1_DIVIDER     (RCC_HCLK_Div1)
#define APB2_DIVIDER     (RCC_HCLK_Div1)
#elif (DESIRED_PCLK_MHZ == 6)
#define APB1_DIVIDER     (RCC_HCLK_Div4)
#define APB2_DIVIDER     (RCC_HCLK_Div4)
#else
#error "Invalid Peripheral Clock for A4V Products"
#endif
#elif (DESIRED_SYS_CLK_MHZ == 96)
#define DESIRED_FLASH_ACR_LATENCY FLASH_ACR_LATENCY_3WS
#if (DESIRED_PCLK_MHZ == 24)
#define APB1_DIVIDER     (RCC_HCLK_Div4)
#define APB2_DIVIDER     (RCC_HCLK_Div4)
#elif (DESIRED_PCLK_MHZ == 6)
#define APB1_DIVIDER     (RCC_HCLK_Div16)
#define APB2_DIVIDER     (RCC_HCLK_Div16)
#else
#error "Invalid Peripheral Clock for A4V Products"
#endif
#elif (DESIRED_SYS_CLK_MHZ == 120)
#define DESIRED_FLASH_ACR_LATENCY FLASH_ACR_LATENCY_3WS
#if (DESIRED_PCLK_MHZ == 30)
#define APB1_DIVIDER     (RCC_HCLK_Div4)
#define APB2_DIVIDER     (RCC_HCLK_Div4)
#elif (DESIRED_PCLK_MHZ == 15)
#define APB1_DIVIDER     (RCC_HCLK_Div8)
#define APB2_DIVIDER     (RCC_HCLK_Div8)
#else
#error "Invalid Peripheral Clock for A4V Products"
#endif

#else
#error "Invalid Clock for A4V Products"
#endif

// The following need to match the dividers above
#define APB1_PERIPHERAL_CLK (DESIRED_PCLK_MHZ * 1000000)
#define APB2_PERIPHERAL_CLK (DESIRED_PCLK_MHZ * 1000000)

#define APB1_TIMER_PERIPHERAL_CLK ((APB1_PERIPHERAL_CLK) * (APB1_DIVIDER == RCC_HCLK_Div1 ? 1 : 2))
#define APB2_TIMER_PERIPHERAL_CLK ((APB2_PERIPHERAL_CLK) * (APB2_DIVIDER == RCC_HCLK_Div1 ? 1 : 2))

/* functions for others to use to configure the system */
void VariantInit(void);
uint8_t InitVariantScribes(void);

#endif
