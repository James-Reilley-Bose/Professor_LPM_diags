/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_it.h
* Author             : MCD Application Team
* Version            : V1.02
* Date               : 03/03/2008
* Description        : This file contains the headers of the interrupt handlers.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2xx_INTERRUPTS_H
#define __STM32F2xx_INTERRUPTS_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void ResetVectorHandler(void);
void NMIException(void);
void HardFaultException(void);
void MemManageException(void);
void BusFaultException(void);
void UsageFaultException(void);
void DebugMonitor(void);
void SVCHandler(void);
void PendSVC(void);
void SysTickHandler(void) __arm_irq__;
void WWDG_IRQHandler(void) __arm_irq__;
void PVD_IRQHandler(void) __arm_irq__;
void TAMPER_IRQHandler(void) __arm_irq__;
void RTC_IRQHandler(void) __arm_irq__;
void FLASH_IRQHandler(void) __arm_irq__;
void RCC_IRQHandler(void) __arm_irq__;
void EXTI0_IRQHandler(void) __arm_irq__;
void EXTI1_IRQHandler(void) __arm_irq__;
void EXTI2_IRQHandler(void) __arm_irq__;
void EXTI3_IRQHandler(void) __arm_irq__;
void EXTI4_IRQHandler(void) __arm_irq__;
void DMA1_Stream0_IRQHandler(void) __arm_irq__;
void DMA1_Stream1_IRQHandler(void) __arm_irq__;
void DMA1_Stream2_IRQHandler(void) __arm_irq__;
void DMA1_Stream3_IRQHandler(void) __arm_irq__;
void DMA1_Stream4_IRQHandler(void) __arm_irq__;
void DMA1_Stream5_IRQHandler(void) __arm_irq__;
void DMA1_Stream6_IRQHandler(void) __arm_irq__;
void ADC1_2_IRQHandler(void) __arm_irq__;
void USB_HP_CAN_TX_IRQHandler(void) __arm_irq__;
void USB_LP_CAN_RX0_IRQHandler(void) __arm_irq__;
void CAN_RX1_IRQHandler(void) __arm_irq__;
void CAN_SCE_IRQHandler(void) __arm_irq__;
void EXTI9_5_IRQHandler(void) __arm_irq__;
void TIM1_BRK_IRQHandler(void) __arm_irq__;
void TIM1_UP_IRQHandler(void) __arm_irq__;
void TIM1_TRG_COM_IRQHandler(void) __arm_irq__;
void TIM1_CC_IRQHandler(void) __arm_irq__;
void TIM2_IRQHandler(void) __arm_irq__;
void TIM3_IRQHandler(void) __arm_irq__;
void TIM4_IRQHandler(void) __arm_irq__;
void I2C1_EV_IRQHandler(void) __arm_irq__;
void I2C1_ER_IRQHandler(void) __arm_irq__;
void I2C2_EV_IRQHandler(void) __arm_irq__;
void I2C2_ER_IRQHandler(void) __arm_irq__;
void SPI1_IRQHandler(void) __arm_irq__;
void SPI2_IRQHandler(void) __arm_irq__;
void USART1_IRQHandler(void) __arm_irq__;
void USART2_IRQHandler(void) __arm_irq__;
void USART3_IRQHandler(void) __arm_irq__;
void EXTI15_10_IRQHandler(void) __arm_irq__;
void RTCAlarm_IRQHandler(void) __arm_irq__;
void USBWakeUp_IRQHandler(void) __arm_irq__;
void TIM8_BRK_IRQHandler(void) __arm_irq__;
void TIM8_UP_IRQHandler(void) __arm_irq__;
void TIM8_TRG_COM_IRQHandler(void) __arm_irq__;
void TIM8_CC_IRQHandler(void) __arm_irq__;
void ADC3_IRQHandler(void) __arm_irq__;
void FSMC_IRQHandler(void) __arm_irq__;
void SDIO_IRQHandler(void) __arm_irq__;
void TIM5_IRQHandler(void) __arm_irq__;
void SPI3_IRQHandler(void) __arm_irq__;
void UART4_IRQHandler(void) __arm_irq__;
void UART5_IRQHandler(void) __arm_irq__;
void TIM6_IRQHandler(void) __arm_irq__;
void TIM7_IRQHandler(void) __arm_irq__;
void DMA2_Channel1_IRQHandler(void) __arm_irq__;
void DMA2_Channel2_IRQHandler(void) __arm_irq__;
void DMA2_Channel3_IRQHandler(void) __arm_irq__;
void DMA2_Channel4_5_IRQHandler(void) __arm_irq__;
void Unhandled_IRQ(void) __arm_irq__;

#endif /* __STM32F2xx_INTERRUPTS_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
