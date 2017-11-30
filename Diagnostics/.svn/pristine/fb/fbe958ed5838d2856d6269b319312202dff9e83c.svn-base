/*
    File    :   professor_pin_mapping.h
    Author  :   Dillon Johnson
    Created :   1/4/17
    Language:   C
    Copyright:  (C) 2017 Bose Corporation, Framingham, MA

    Description:   All GPIO pin descriptions, clocks, buses, and
                   any other board level definitions.

===============================================================================
*/

#ifndef PRODUCT_PIN_MAPPING_H
#define PRODUCT_PIN_MAPPING_H

#include "gpio_mapping.h"

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
* USART3
**********************************************************/
#define USART3_TX_GPIO_BANK                    I2C_TWO_SCL_GPIO_BANK
#define USART3_TX_GPIO_PIN                     I2C_TWO_GPIO_SCL_PIN
#define USART3_RX_GPIO_BANK                    I2C_TWO_SDA_GPIO_BANK
#define USART3_RX_GPIO_PIN                     I2C_TWO_GPIO_SDA_PIN
#define PSOC_UART                              USART3
#define PSOC_BUART                             BUSART3

/**********************************************************
* I2C3
**********************************************************/
#define I2C_THREE_SCL_GPIO_BANK                GPIOA
#define I2C_THREE_GPIO_SCL_PIN                 8
#define I2C_THREE_SDA_GPIO_BANK                GPIOC
#define I2C_THREE_GPIO_SDA_PIN                 9

/**********************************************************
* PSOC
**********************************************************/
#define PSOC_I2C_BUS                             3
#define PSOC_I2C3_ADDR                           0x10
#define LPM_PD3_CAP_IRQ_BANK                     GPIOD
#define LPM_PD3_CAP_IRQ_PIN                      3
#define I2C_THREE_BUS_CLK                        I2C_MASTER_CLOCK_FAST_SPEED_HZ
#define PSOC_RESET_GPIO_BANK                     GPIOC
#define PSOC_RESET_GPIO_PIN                      1
#define PSOC_INT_GPIO_BANK                       GPIOD
#define PSOC_INT_GPIO_PIN                        3
#define PSOC_I2C_SPEED                           I2C_MASTER_CLOCK_FAST_SPEED_HZ


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
#define SPI2_MOSI_GPIO_BANK                     GPIOC
#define SPI2_MOSI_GPIO_PIN                      3
#define SPI2_MISO_GPIO_BANK                     GPIOC
#define SPI2_MISO_GPIO_PIN                      2
#define SPI2_CLK_GPIO_BANK                      GPIOB
#define SPI2_CLK_GPIO_PIN                       13
#define SPI2_CS_GPIO_BANK                       GPIOB
#define SPI2_CS_GPIO_PIN                        12

/**********************************************************
* SPI3
**********************************************************/
#define SPI3_MOSI_GPIO_BANK                     GPIOB
#define SPI3_MOSI_GPIO_PIN                      5
#define SPI3_MISO_GPIO_BANK                     GPIOC
#define SPI3_MISO_GPIO_PIN                      11
#define SPI3_CLK_GPIO_BANK                      GPIOC
#define SPI3_CLK_GPIO_PIN                       10

/**********************************************************
* LED
**********************************************************/
#define LED1_GPIO_BANK                          GPIOA
#define LED1_GPIO_PIN                           0

/**********************************************************
* Ethernet management IC event
**********************************************************/
#define ENET_PME_IRQ_L_BANK                     GPIOA
#define ENET_PME_IRQ_L_PIN                      2

/**********************************************************
* Power
**********************************************************/
#define ADC_VSYS_SENSE1_BANK                    GPIOA
#define ADC_VSYS_SENSE1_PIN                     4
#define ADC_VSYS_SENSE2_BANK                    GPIOC
#define ADC_VSYS_SENSE2_PIN                     0
#define PFAIL_IRQ_BANK                          GPIOD
#define PFAIL_IRQ_PIN                           4
#define POWER_LP_STANDBY_L_BANK                 GPIOD
#define POWER_LP_STANDBY_L_PIN                  12
#define POWER_NW_STANDBY_L_BANK                 GPIOE
#define POWER_NW_STANDBY_L_PIN                  15
#define POWER_VLED_BANK                         GPIOD
#define POWER_VLED_PIN                          13

/**********************************************************
* Amp
**********************************************************/
#define ADC_THERM1_BANK                         GPIOB
#define ADC_THERM1_PIN                          0
#define ADC_THERM2_BANK                         GPIOB
#define ADC_THERM2_PIN                          1
#define AMP_FAULT_BANK                          GPIOC
#define AMP_FAULT_PIN                           13
// #define AMP_FAULT_AF ??
#define AMP_MUTE_BANK                           GPIOC
#define AMP_MUTE_PIN                            14
#define AMP_OFFSET_FAULT_BANK                   GPIOC
#define AMP_OFFSET_FAULT_PIN                    15
#define AMP_POWER_ENABLE_BANK                   GPIOE
#define AMP_POWER_ENABLE_PIN                    12

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
* Tap
**********************************************************/
#define TAP_UART                               USART1
#define TAP_BUART                              BUSART1
#define TAP_UART_TX_GPIO_BANK                  GPIOA
#define TAP_TX_PIN                             9
#define TAP_IRQ                                USART1_IRQn
#define TAP_UART_RX_GPIO_BANK                  GPIOA
#define TAP_RX_PIN                             10
#define TAP_UART_RCC_PERIPH                    RCC_APB2Periph_USART1
#define TAP_UART_GPIO_AF                       GPIO_AF_USART1
#define TAP_BAUD_RATE                          115200
#define TAP_BAUD                               BR_115200


/**********************************************************
* Buttons
**********************************************************/
#define BUTTON1_GPIO_BANK                       GPIOB
#define BUTTON1_GPIO_PIN                        2
#define BUTTON2_GPIO_BANK                       GPIOB
#define BUTTON2_GPIO_PIN                        9
#define BUTTON3_GPIO_BANK                       GPIOD
#define BUTTON3_GPIO_PIN                        2
#define BUTTON4_GPIO_BANK                       GPIOE
#define BUTTON4_GPIO_PIN                        10

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
* Riviera
**********************************************************/
#define RIVIERA_SPI_BID                         BSPI2
#define RIVIERA_SPI_CLK_BANK                    SPI2_CLK_GPIO_BANK
#define RIVIERA_SPI_CLK_PIN                     SPI2_CLK_GPIO_PIN
#define RIVIERA_SPI_MOSI_BANK                   SPI2_MOSI_GPIO_BANK
#define RIVIERA_SPI_MOSI_PIN                    SPI2_MOSI_GPIO_PIN
#define RIVIERA_SPI_MISO_BANK                   SPI2_MISO_GPIO_BANK
#define RIVIERA_SPI_MISO_PIN                    SPI2_MISO_GPIO_PIN
#define RIVIERA_SPI_CS_BANK                     SPI2_CS_GPIO_BANK
#define RIVIERA_SPI_CS_PIN                      SPI2_CS_GPIO_PIN
#define RIVIERA_SPI_IRQ                         SPI2_IRQn
#define RIVIERA_RTS_GPIO_BANK                   GPIOA
#define RIVIERA_RTS_GPIO_PIN                    11
#define RIVIERA_CTS_GPIO_BANK                   GPIOA
#define RIVIERA_CTS_GPIO_PIN                    1
#define RIVIERA_CTS_IRQ_PORT_SOURCE             EXTI_PortSourceGPIOD
#define RIVIERA_CTS_IRQ_LINE                    EXTI_Line6
#define RIVIERA_CTS_IRQ_LINE_NUM                6
#define RIVIERA_CTS_IRQ_CHANNEL                 EXTI9_5_IRQn
#define RIVIERA_CTS_IRQ_PIN_MASK                GPIO_Pin_6
#define RIVIERA_CTS_IRQ_HANDLER                 EXTI9_5_IRQHandler
#define RIVIERA_CTS_IRQ_ACTIVE_STATE            GPIO_LOW

#define RIVIERA_CBL_POWER_BANK                  GPIOB
#define RIVIERA_CBL_POWER_PIN                   14
#define RIVIERA_KYPD_POWER_BANK                 GPIOB
#define RIVIERA_KYPD_POWER_PIN                  15
#define RIVIERA_KYPD_POWER_ACTIVE_STATE         GPIO_LOW
#define RIVIERA_PON_1_BANK                      GPIOE
#define RIVIERA_PON_1_PIN                       11
#define RIVIERA_RESET_REQ_BANK                  GPIOD
#define RIVIERA_RESET_REQ_PIN                   11
#define RIVIERA_PS_HOLD_BANK                    GPIOD
#define RIVIERA_PS_HOLD_PIN                     15
#define RIVIERA_PS_HOLD_ACTIVE_STATE            GPIO_LOW
#define RIVIERA_FORCE_USB_BOOT_BANK             GPIOE
#define RIVIERA_FORCE_USB_BOOT_PIN              1
#define RIVIERA_FORCE_USB_BOOT_ACTIVE_STATE     GPIO_HIGH
#define RIVIERA_PMIC_RESIN_BANK                 GPIOE
#define RIVIERA_PMIC_RESIN_PIN                  7
#define RIVIERA_PMIC_RESIN_ACTIVE_STATE         GPIO_LOW
#define RIVIERA_PON_RESET_BANK                  GPIOE
#define RIVIERA_PON_RESET_PIN                   8
#define RIVIERA_PON_RESET_ACTIVE_STATE          GPIO_LOW
#define RIVIERA_TRANSLATOR_EN_BANK              GPIOA
#define RIVIERA_TRANSLATOR_EN_PIN               12
#define RIVIERA_TRANSLATOR_ACTIVE_STATE         GPIO_LOW

/**********************************************************
* NFC
**********************************************************/
#define NFC_DET_INT_GPIO_BANK                   GPIOC
#define NFC_DET_INT_GPIO_PIN                    1
#define NFC_DET_INT_GPIO_PIN_MASK               GPIO_Pin_1
#define NFC_I2C_BUS                             3
#define NFC_I2C_SPEED                           I2C_MASTER_CLOCK_STANDARD_SPEED_HZ
#define NFC_SCL_GPIO_BANK                       I2C_THREE_SCL_GPIO_BANK
#define NFC_SCL_GPIO_PIN                        I2C_THREE_GPIO_SCL_PIN
#define NFC_SDA_GPIO_BANK                       I2C_THREE_SDA_GPIO_BANK
#define NFC_SDA_GPIO_PIN                        I2C_THREE_GPIO_SDA_PIN

/**********************************************************
* LCD
**********************************************************/
#define LCD_BACKLIGHT_BANK                      GPIOE
#define LCD_BACKLIGHT_PIN                       13

#define RF_RMT_CS_L_BANK                        GPIOD
#define RF_RMT_CS_L_PIN                         14


/**********************************************************
* LLR Interface
**********************************************************/
#define PROF_LLR_RST_L_BANK                     GPIOD
#define PROF_LLR_RST_L_PIN                      0


#define PROF_I2C_LLR_SCL_BANK                   I2C_TWO_SCL_GPIO_BANK
#define PROF_I2C_LLR_SCL_PIN                    I2C_TWO_GPIO_SCL_PIN
#define PROF_I2C_LLR_SDA_PIN                    I2C_TWO_GPIO_SDA_PIN

/**********************************************************
* ADC
**********************************************************/
#define ADC_VSYS_SENSE1_BANK                  GPIOA  // PA4
#define ADC_VSYS_SENSE1_PIN                   4
#define ADC_THERM1_BANK                       GPIOB  // PB0
#define ADC_THERM1_PIN                        0
#define ADC_THERM2_BANK                       GPIOB  //PB1
#define ADC_THERM2_PIN                        1
#define ADC_VSYS_SENSE2_BANK                  GPIOC  //PC0
#define ADC_VSYS_SENSE2_PIN                   0

/**********************************************************
* CEC Interface /F0 Interface from Ginger
**********************************************************/
#define CEC_SCL_GPIO_BANK       I2C_THREE_SCL_GPIO_BANK
#define CEC_SCL_GPIO_PIN        I2C_THREE_GPIO_SCL_PIN

#define CEC_SDA_GPIO_BANK       I2C_THREE_SDA_GPIO_BANK
#define CEC_SDA_GPIO_PIN        I2C_THREE_GPIO_SDA_PIN

#define CEC_CTRL_INTERRUPT_GPIO_BANK           GPIOC
#define CEC_CTRL_INTERRUPT_GPIO_PIN            8

#define CEC_CTRL_INT_GPIO_BANK                 GPIOC
#define CEC_CTRL_INT_GPIO_PIN                  8
#define CEC_CTRL_INT_GPIO_PIN_MASK             GPIO_Pin_8

#define CEC_CTRL_RST_L_GPIO_BANK               GPIOD
#define CEC_CTRL_RST_L_GPIO_PIN                7
#define CEC_CTRL_RST_L_GPIO_PIN_MASK           GPIO_Pin_7

// delay for GPIO reads after toggle
#define GPIO_READ_DELAY_MS                      5


/**********************************************************
* Just for test of Breakout Board
**********************************************************/
#define SPI_SLAVE_BID               BSPI2
#define SPI_SLAVE_BUS_NUM           1
#define SPI_SLAVE_BUS               SPI2
#define SPI_SLAVE_BUS_RX            SPI2_RX
#define SPI_SLAVE_BUS_TX            SPI2_TX
#define SPI_SLAVE_SCK_BANK          SPI2_CLK_GPIO_BANK
#define SPI_SLAVE_SCK_PIN           SPI2_CLK_GPIO_PIN
#define SPI_SLAVE_MISO_BANK         SPI2_MISO_GPIO_BANK
#define SPI_SLAVE_MISO_PIN          SPI2_MISO_GPIO_PIN
#define SPI_SLAVE_MOSI_BANK         SPI2_MOSI_GPIO_BANK
#define SPI_SLAVE_MOSI_PIN          SPI2_MOSI_GPIO_PIN
#define SPI_SLAVE_CS_BANK           SPI2_CS_GPIO_BANK
#define SPI_SLAVE_CS_PIN            SPI2_CS_GPIO_PIN
#define SPI_SLAVE_GPIO_AF           GPIO_AF_SPI2
#define SPI_SLAVE_IRQ               SPI2_IRQn
#define SPI_SLAVE_EXTI_BANK         GPIOA
#define SPI_SLAVE_EXTI_PIN          11
#define SPI_SLAVE_CTS_GPIO_BANK                   GPIOD
#define SPI_SLAVE_CTS_GPIO_PIN                    6
#define SPI_SLAVE_CTS_IRQ_PORT_SOURCE             EXTI_PortSourceGPIOD
#define SPI_SLAVE_CTS_IRQ_LINE                    EXTI_Line6
#define SPI_SLAVE_CTS_IRQ_LINE_NUM                6
#define SPI_SLAVE_CTS_IRQ_CHANNEL                 EXTI9_5_IRQn
#define SPI_SLAVE_CTS_IRQ_PIN_MASK                GPIO_Pin_6
#define SPI_SLAVE_CTS_IRQ_HANDLER                 EXTI9_5_IRQHandler

/**********************************************************
* Radio Interface
**********************************************************/
#define LLR_I2C_BUS                            2
#define RADIO_SCL_GPIO_BANK                    I2C_TWO_SCL_GPIO_BANK
#define RADIO_SCL_GPIO_PIN                     I2C_TWO_GPIO_SCL_PIN
#define RADIO_SDA_GPIO_BANK                    I2C_TWO_GPIO_SDA_PIN
#define RADIO_SDA_GPIO_PIN                     I2C_TWO_GPIO_SDA_PIN
#define DWAM_RESET_BANK                        GPIOD
#define DWAM_RESET_PIN                         0
#define RADIO_IRQ_GPIO_BANK                    GPIOD
#define RADIO_IRQ_GPIO_PIN                     6
#define RADIO_IRQ_GPIO_PIN_MASK                GPIO_Pin_1
#define DWAM_INTERRUPT_LINE                    EXTI_Line1
#define DWAM_INTERRUPT_PORT_SOURCE             EXTI_PortSourceGPIOD
#define DWAM_INTERRUPT_LINE_NUM                1 /* EXTI_LINE1 */
#define DWAM_INTERRUPT_PIN_MASK                RADIO_IRQ_GPIO_PIN_MASK
#define DWAM_INTERRUPT_CHANNEL                 EXTI1_IRQn
#define DWAM_INTERRUPT_BANK                    RADIO_IRQ_GPIO_BANK
#define DWAM_INTERRUPT_PIN                     RADIO_IRQ_GPIO_PIN
#define DWAM_INTERRUPT_HANDLER                 EXTI1_IRQHandler

/**********************************************************
* DSP Interface
**********************************************************/
#define DSP_UART                               USART6
#define DSP_BUART                              BUSART6
#define DSP_UART_BASE                          USART6_BASE
#define DSP_INTERRUPT_CHANNEL                  USART6_IRQn
#define DSP_TX_GPIO_BANK                       GPIOC
#define DSP_RX_GPIO_BANK                       GPIOC
#define DSP_TX_PIN                             6
#define DSP_RX_PIN                             7
#define DSP_GPIO_AF                            GPIO_AF_USART6
#define DSP_STANDBY_GPIO_BANK
#define DSP_STANDBY_GPIO_PIN
#define DSP_POWER_GPIO_BANK
#define DSP_POWER_GPIO_PIN
#define DSP_RST_L_GPIO_BANK                   GPIOA
#define DSP_RST_L_GPIO_PIN                    3
#define DSP_RST_L_MODE                        GPIO_MODE_OUT_OD_PULLUP
#define DSP_RST_L_AF                          0
#define DSP_LEVEL_SHIFT_L_BANK                GPIOE
#define DSP_LEVEL_SHIFT_L_PIN                 0

/**********************************************************
* F0 Interface
**********************************************************/
#define F0_I2C_BUS                             3
#define F0_I2C_SPEED                           I2C_MASTER_CLOCK_STANDARD_SPEED_HZ
#define F0_I2C_INTERRUPT_GPIO_BANK             GPIOC
#define F0_I2C_INTERRUPT_GPIO_PIN              8
#define F0_I2C_INTERRUPT_EXTI_PORT_SOURCE      EXTI_PortSourceGPIOC
#define F0_I2C_INTERRUPT_LINE                  EXTI_Line8
#define F0_I2C_INTERRUPT_CHANNEL               EXTI9_5_IRQn
#define F0_I2C_INTERRUPT_HANDLER               EXTI9_5_IRQHandler
#define F0_CTRL_RST_L_GPIO_BANK                GPIOD
#define F0_CTRL_RST_L_GPIO_PIN                 7

/**********************************************************
* eARC
**********************************************************/
#define EARC_RESET_L_GPIO_BANK                 GPIOC
#define EARC_RESET_L_GPIO_PIN                  12
#define EARC_IRQ_GPIO_BANK                     GPIOD
#define EARC_IRQ_GPIO_PIN                      13

/**********************************************************
* UEI Blaster Communication
**********************************************************/
#define UEI_UART                                USART3
#define UEI_BUART                               BUSART3
#define UEI_IRQ                                 USART3_IRQn
#define UEI_UART_RCC_PERIPH                     RCC_APB1Periph_USART3
#define UEI_UART_RX_GPIO_BANK                   GPIOD
#define UEI_RX_PIN                              9
#define UEI_UART_TX_GPIO_BANK                   GPIOD
#define UEI_TX_PIN                              8
#define UEI_AF                                  GPIO_AF_USART3
#define QS_RST_L_GPIO_BANK                      GPIOD
#define QS_RST_L_GPIO_PIN                       10

/**********************************************************
* Bassbox
**********************************************************/
#define CONSOLE_DC_TX_GPIO_BANK                 GPIOE
#define CONSOLE_DC_TX_GPIO_PIN                  9
#define CONSOLE_DC_TX_GPIO_PIN_MASK             GPIO_Pin_9
#define BASSBOX_DC_RX_GPIO_BANK                 GPIOE
#define BASSBOX_DC_RX_GPIO_PIN                  10
#define BASSBOX_DC_RX_GPIO_PIN_MASK             GPIO_Pin_10

/**********************************************************
* MIC
**********************************************************/
#define MIC_MUTE_GPIO_BANK                      GPIOB
#define MIC_MUTE_GPIO_PIN                       6

#define MIC_DET_GPIO_BANK                       GPIOD
#define MIC_DET_GPIO_PIN                        5

#endif // PRODUCT_PIN_MAPPING_H
