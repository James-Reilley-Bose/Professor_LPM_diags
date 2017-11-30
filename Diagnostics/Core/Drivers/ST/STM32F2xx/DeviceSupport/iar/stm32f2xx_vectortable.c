/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_vector.c
* Author             : MCD Application Team
* Version            : V1.02
* Date               : 03/03/2008
* Description        : This file contains the vector table for STM32F10x.
*                      After Reset the Cortex-M3 processor is in Thread mode,
*                      priority is Privileged, and the Stack is set to Main.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "project.h"
#include "stm32f2xx_interrupts.h"
#include "exception_handle.h"
#include "mpu.h"

#ifdef NEW_INTERRUPT_SCHEME
extern void Interrupt_Handler(void) __arm_irq__;
#else
extern void HwTimer_IRQHandler(void);
extern void SPI2_HandleDMA_TxComplete(void);
extern void SPI2_HandleDMA_RxComplete(void);
extern void i2cOne_Event_IRQHandler(void);
extern void i2cOne_Error_IRQHandler(void);
extern void i2cTwo_Event_IRQHandler(void);
extern void i2cTwo_Error_IRQHandler(void);
extern void IR_RX_IRQHandler(void);
extern void IR_RX_Overflow_ISR(void);
extern void Unhandled_IRQ(void);
#endif

// FreeRTOS handlers
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
extern void vPortSVCHandler( void );

/* Private typedef -----------------------------------------------------------*/
typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#pragma language=extended
#pragma segment="CSTACK"

#pragma location = "INTVEC"
/* STM32F2xx Vector Table entries */
const intvec_elem __vector_table[] =
{
    { .__ptr = __sfe( "CSTACK" ) },       // Exception #0
  ResetVectorHandler,
  NMIException,
  __CallHardFault,
  __CallHardFault,
  BusFaultException,
  UsageFaultException,
  0, 0, 0, 0,            /* Reserved */
  vPortSVCHandler,
  DebugMonitor,
  0,                      /* Reserved */
  xPortPendSVHandler,
  xPortSysTickHandler,                  // Exception #15
#ifdef NEW_INTERRUPT_SCHEME
  Interrupt_Handler,            // 0 - WWDG_IRQn
  Interrupt_Handler,            // 1 - PVD_IRQn
  Interrupt_Handler,            // 2 - TAMP_STAMP_IRQn
  Interrupt_Handler,            // 3 - RTC_WKUP_IRQn
  Interrupt_Handler,            // 4 - FLASH_IRQn
  Interrupt_Handler,            // 5 - RCC_IRQn
  Interrupt_Handler,            // 6 - EXTI0_IRQn
  Interrupt_Handler,            // 7 - EXTI1_IRQn
  Interrupt_Handler,            // 8 - EXTI2_IRQn
  Interrupt_Handler,            // 9 - EXTI3_IRQn
  Interrupt_Handler,            // 10- EXTI4_IRQn
  Interrupt_Handler,            // 11- DMA1_Stream0_IRQn
  Interrupt_Handler,            // 12- DMA1_Stream1_IRQn
  Interrupt_Handler,            // 13- DMA1_Stream2_IRQn
  Interrupt_Handler,            // 14- DMA1_Stream3_IRQn
  Interrupt_Handler,            // 15- DMA1_Stream4_IRQn
  Interrupt_Handler,            // 16- DMA1_Stream5_IRQn
  Interrupt_Handler,            // 17- DMA1_Stream6_IRQn
  Interrupt_Handler,            // 18- ADC_IRQn 
  Interrupt_Handler,            // 19- CAN1_TX_IRQn
  Interrupt_Handler,            // 20- CAN1_RX0_IRQn 
  Interrupt_Handler,            // 21- CAN1_RX1_IRQn
  Interrupt_Handler,            // 22- CAN1_SCE_IRQn
  Interrupt_Handler,            // 23- EXTI9_5_IRQn
  Interrupt_Handler,            // 24- TIM1_BRK_TIM9_IRQn
  Interrupt_Handler,            // 25- TIM1_UP_TIM10_IRQn
  Interrupt_Handler,            // 26- TIM1_TRG_COM_TIM11_IRQn
  Interrupt_Handler,            // 27- TIM1_CC_IRQn 
  Interrupt_Handler,            // 28- TIM2_IRQn
  Interrupt_Handler,            // 29- TIM3_IRQn
  Interrupt_Handler,            // 30- TIM4_IRQn
  Interrupt_Handler,            // 31- I2C1_EV_IRQn 
  Interrupt_Handler,            // 32- I2C1_ER_IRQn
  Interrupt_Handler,            // 33- I2C2_EV_IRQn
  Interrupt_Handler,            // 34- I2C2_ER_IRQn
  Interrupt_Handler,            // 35- SPI1_IRQn
  Interrupt_Handler,            // 36- SPI2_IRQn 
  Interrupt_Handler,            // 37- USART1_IRQn
  Interrupt_Handler,            // 38- USART2_IRQn
  Interrupt_Handler,            // 39- USART3_IRQn
  Interrupt_Handler,            // 40- EXTI15_10_IRQn
  Interrupt_Handler,            // 41- RTC_Alarm_IRQn
  Interrupt_Handler,            // 42- OTG_FS_WKUP_IRQn 
  Interrupt_Handler,            // 43- TIM8_BRK_TIM12_IRQn
  Interrupt_Handler,            // 44- TIM8_UP_TIM13_IRQn
  Interrupt_Handler,            // 45- TIM8_TRG_COM_TIM14_IRQn 
  Interrupt_Handler,            // 46- TIM8_CC_IRQn 
  Interrupt_Handler,            // 47- DMA1_Stream7_IRQn
  Interrupt_Handler,            // 48- FSMC_IRQn
  Interrupt_Handler,            // 49- SDIO_IRQn
  Interrupt_Handler,            // 50- TIM5_IRQn 
  Interrupt_Handler,            // 51- SPI3_IRQn
  Interrupt_Handler,            // 52- UART4_IRQn
  Interrupt_Handler,            // 53- UART5_IRQn
  Interrupt_Handler,            // 54- TIM6_DAC_IRQn
  Interrupt_Handler,            // 55- TIM7_IRQn
  Interrupt_Handler,            // 56- DMA2_Stream0_IRQn 
  Interrupt_Handler,            // 57- DMA2_Stream1_IRQn
  Interrupt_Handler,            // 58- DMA2_Stream2_IRQn
  Interrupt_Handler,            // 59- DMA2_Stream3_IRQn
  Interrupt_Handler,            // 60- DMA2_Stream4_IRQn
  Interrupt_Handler,            // 61- ETH_IRQn
  Interrupt_Handler,            // 62- ETH_WKUP_IRQn
  Interrupt_Handler,            // 63- CAN2_TX_IRQn
  Interrupt_Handler,            // 64- CAN2_RX0_IRQn 
  Interrupt_Handler,            // 65- CAN2_RX1_IRQn
  Interrupt_Handler,            // 66- CAN2_SCE_IRQn
  Interrupt_Handler,            // 67- OTG_FS_IRQn
  Interrupt_Handler,            // 68- DMA2_Stream5_IRQn
  Interrupt_Handler,            // 69- DMA2_Stream6_IRQn
  Interrupt_Handler,            // 70- DMA2_Stream7_IRQn
  Interrupt_Handler,            // 71- USART6_IRQn
  Interrupt_Handler,            // 72- I2C3_EV_IRQn
  Interrupt_Handler,            // 73- I2C3_ER_IRQn
  Interrupt_Handler,            // 74- OTG_HS_EP1_OUT_IRQn 
  Interrupt_Handler,            // 75- OTG_HS_EP1_IN_IRQn
  Interrupt_Handler,            // 76- OTG_HS_WKUP_IRQn
  Interrupt_Handler,            // 77- OTG_HS_IRQn
  Interrupt_Handler,            // 78- DCMI_IRQn
  Interrupt_Handler,            // 79- CRYP_IRQn
  Interrupt_Handler             // 80- HASH_RNG_IRQn
 
#else
  WWDG_IRQHandler,
  PVD_IRQHandler,
  TAMPER_IRQHandler,
  RTC_IRQHandler,                       // Exception #19
  FLASH_IRQHandler,
  RCC_IRQHandler,
  EXTI0_IRQHandler,
  EXTI1_IRQHandler,
  EXTI2_IRQHandler,
  EXTI3_IRQHandler,
  EXTI4_IRQHandler,
  DMA1_Stream0_IRQHandler, // DMA Stream0
  Unhandled_IRQ,//DMA1_Stream1_IRQHandler, DMA Stream1
  Unhandled_IRQ,//DMA1_Stream2_IRQHandler, DMA Stream2
  Unhandled_IRQ, //SPI2_HandleDMA_RxComplete,was DMA Stream3 // spitwo rx
  Unhandled_IRQ, //SPI2_HandleDMA_TxComplete, was DMA Stream4 // spitwo tx
  DMA1_Stream5_IRQHandler, //DMA Stream5  // uart2 dma rx boselink
  DMA1_Stream6_IRQHandler, //DMA Stream6  // uart2 dma tx boselink
  ADC1_2_IRQHandler,
  USB_HP_CAN_TX_IRQHandler,
  USB_LP_CAN_RX0_IRQHandler,
  CAN_RX1_IRQHandler,
  CAN_SCE_IRQHandler,
  EXTI9_5_IRQHandler,   // IPC ack, alert and 5V OVER voltage
  TIM1_BRK_IRQHandler,
  TIM1_UP_IRQHandler,//IR_RX_Overflow_ISR
  TIM1_TRG_COM_IRQHandler,
  TIM1_CC_IRQHandler,
  Unhandled_IRQ, // TIM2
  TIM3_IRQHandler,//PWM - calls debug assert
  TIM4_IRQHandler, // PLL - calls debug assert
  Unhandled_IRQ,
  Unhandled_IRQ,
  Unhandled_IRQ,
  Unhandled_IRQ,
  Unhandled_IRQ, // SPI1_IRQHandler,
  Unhandled_IRQ, // SPI2_IRQHandler
  USART1_IRQHandler,
  USART2_IRQHandler,
  USART3_IRQHandler,
  EXTI15_10_IRQHandler,
  RTCAlarm_IRQHandler,
  USBWakeUp_IRQHandler,
  TIM8_BRK_IRQHandler,
  TIM8_UP_IRQHandler,
  TIM8_TRG_COM_IRQHandler,
  TIM8_CC_IRQHandler,
  ADC3_IRQHandler,
  FSMC_IRQHandler,
  SDIO_IRQHandler,
  TIM5_IRQHandler,
  SPI3_IRQHandler,
  UART4_IRQHandler,
  UART5_IRQHandler,
  TIM6_IRQHandler,
  TIM7_IRQHandler,
  Unhandled_IRQ,        /*!< DMA2 Stream 0 global Interrupt               */
  Unhandled_IRQ,        /*!< DMA2 Stream 1 global Interrupt               */
  Unhandled_IRQ,        /*!< DMA2 Stream 2 global Interrupt               */
  Unhandled_IRQ         /*!< DMA2 Stream 3 global Interrupt               */
#endif
};

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

