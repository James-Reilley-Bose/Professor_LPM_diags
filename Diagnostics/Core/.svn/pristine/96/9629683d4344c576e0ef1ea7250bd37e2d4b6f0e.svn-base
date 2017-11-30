/*
    File    :   example_pin_mapping.h
    Author  :   Dillon Johnson
    Created :   12/19/16
    Language:   C
    Copyright:  (C) 2016 Bose Corporation, Framingham, MA

    Description:   All GPIO pin descriptions, clocks, buses, and
                   any other board level definitions.

===============================================================================
*/

#ifndef EXAMPLE_PIN_MAPPING_H
#define EXAMPLE_PIN_MAPPING_H

#include "gpio_mapping.h"

/**********************************************************
* IR
**********************************************************/
#define IR_RX_TIMER                             TIM1
#define IR_RX_TIMER_BASE                        TIM1_BASE
#define IR_RX_CC_CHANNEL                        4
#define IR_RX_CC_CHANNEL_REG                    CCR4
#define IR_RX_DATA_EDGE_SELECT                  TIM_CC4P_Set
#define IR_RX_TIMER_INTERRUPT_CHANNEL           TIM1_CC_IRQn
#define IR_RX_OVERFLOW_INTERRUPT_CHANNEL        TIM1_UP_TIM10_IRQn
#define IR_RX_TRIGGER_SOURCE                    TIM_Trigger_TI1FP1
#define IR_RX_DATA_GPIO_BANK                    GPIOE
#define IR_RX_DATA_GPIO_PIN                     14
#define IR_RX_DATA_GPIO_AF                      GPIO_AF_TIM1
#define IR_RX_PRESCALER_ON                      (APB2_TIMER_PERIPHERAL_CLK/1000000) /* 1us resolution */
#define IR_RX_PRESCALER_LP                      (APB2_TIMER_PERIPHERAL_CLK/1000000) /* 1us resolution */

/**********************************************************
* Tap Communication Interface - ginger
**********************************************************/
#define TAP_UART                               USART2
#define TAP_BUART                              BUSART2
#define TAP_UART_TX_GPIO_BANK                  GPIOD
#define TAP_TX_PIN                             5
#define TAP_IRQ                                USART2_IRQn
#define TAP_UART_RX_GPIO_BANK                  GPIOD
#define TAP_RX_PIN                             6
#define TAP_UART_RCC_PERIPH                    RCC_APB1Periph_USART2
#define TAP_UART_GPIO_AF                       GPIO_AF_USART2
#define TAP_BAUD_RATE                          115200 // for Ginger
#define TAP_BAUD                               BR_115200

/**********************************************************
* I2C1
**********************************************************/
#define I2C_ONE_SCL_GPIO_BANK                  GPIOB
#define I2C_ONE_GPIO_SCL_PIN                   8
#define I2C_ONE_SDA_GPIO_BANK                  GPIOB
#define I2C_ONE_GPIO_SDA_PIN                   7

/**********************************************************
* I2C2
**********************************************************/
#define I2C_TWO_SCL_GPIO_BANK                  GPIOB
#define I2C_TWO_GPIO_SCL_PIN                   10
#define I2C_TWO_SDA_GPIO_BANK                  GPIOB
#define I2C_TWO_GPIO_SDA_PIN                   11

/**********************************************************
* I2C3
**********************************************************/
#define I2C_THREE_SCL_GPIO_BANK                GPIOA
#define I2C_THREE_GPIO_SCL_PIN                 8
#define I2C_THREE_SDA_GPIO_BANK                GPIOC
#define I2C_THREE_GPIO_SDA_PIN                 9

/**********************************************************
* SPI1
**********************************************************/
#define SPI1_CLK_GPIO_BANK                     GPIOA
#define SPI1_CLK_GPIO_PIN                      5
#define SPI1_MISO_GPIO_BANK                    GPIOA
#define SPI1_MISO_GPIO_PIN                     6
#define SPI1_MOSI_GPIO_BANK                    GPIOA
#define SPI1_MOSI_GPIO_PIN                     7
#define SPI1_CS_GPIO_BANK                      GPIOC
#define SPI1_CS_GPIO_PIN                       4

/**********************************************************
* SPI2
**********************************************************/
#define SPI2_MOSI_GPIO_BANK 0
#define SPI2_MISO_GPIO_BANK 0
#define SPI2_CLK_GPIO_BANK 0
#define SPI2_MOSI_GPIO_PIN 0
#define SPI2_MISO_GPIO_PIN 0
#define SPI2_CLK_GPIO_PIN 0
#define SPI2_CS_GPIO_BANK 0
#define SPI2_CS_GPIO_PIN 0

/**********************************************************
* SPI3
**********************************************************/
#define SPI3_MOSI_GPIO_BANK 0
#define SPI3_MISO_GPIO_BANK 0
#define SPI3_CLK_GPIO_BANK 0
#define SPI3_MOSI_GPIO_PIN 0
#define SPI3_MISO_GPIO_PIN 0
#define SPI3_CLK_GPIO_PIN  0
#define SPI3_CS_GPIO_BANK 0
#define SPI3_CS_GPIO_PIN 0

/**********************************************************
* External SPI Flash Interface
**********************************************************/
#define SPI_FLASH_BUS_NUM        1
#define SPI_FLASH_BUS            SPI1
#define SPI_FLASH_CS_AF          0
#define SPI_FLASH_SCK_BANK       SPI1_CLK_GPIO_BANK
#define SPI_FLASH_SCK_PIN        SPI1_CLK_GPIO_PIN
#define SPI_FLASH_MISO_BANK      SPI1_MISO_GPIO_BANK
#define SPI_FLASH_MISO_PIN       SPI1_MISO_GPIO_PIN
#define SPI_FLASH_MOSI_BANK      SPI1_MOSI_GPIO_BANK
#define SPI_FLASH_MOSI_PIN       SPI1_MOSI_GPIO_PIN
#define SPI_FLASH_CS_BANK        SPI1_CS_GPIO_BANK
#define SPI_FLASH_CS_PIN         SPI1_CS_GPIO_PIN
#define SPI_FLASH_CS_MODE        GPIO_MODE_OUT_PP
#define SPI_FLASH_WP_L_BANK      GPIOC
#define SPI_FLASH_WP_L_PIN       5

/**********************************************************
* FS USB OTG Interface
**********************************************************/
// output
#define USB_FS_VBUS_GPIO_BANK                  GPIOA
#define USB_FS_VBUS_GPIO_PIN                   9
// input
#define USB_FS_OTG_ID_GPIO_BANK                GPIOA
#define USB_FS_OTG_ID_GPIO_PIN                 10
// bi
#define USB_FS_DM_GPIO_BANK                    GPIOA
#define USB_FS_DM_GPIO_PIN                     11
// bi
#define USB_FS_DP_GPIO_BANK                    GPIOA
#define USB_FS_DP_GPIO_PIN                     12
//output
#define USB_FS_OTG_FS_DRVVBUS_GPIO_BANK        GPIOB
#define USB_FS_OTG_FS_DRVVBUS_GPIO_PIN         6

#endif // EXAMPLE_PIN_MAPPING_H
