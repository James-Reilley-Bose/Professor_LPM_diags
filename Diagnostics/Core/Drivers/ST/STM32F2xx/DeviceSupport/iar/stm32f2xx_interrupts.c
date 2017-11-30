/******************** (C) COPYRIGHT 2008, 2009 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V1.02
* Date               : 03/03/2008
* Description        : Main Interrupt Service Routines.
*                      This file can be used to describe all the exceptions
*                      subroutines that may occur within user application.
*                      When an interrupt happens, the software will branch
*                      automatically to the corresponding routine.
*                      The following routines are all empty, user can write code
*                      for exceptions handlers and peripherals IRQ interrupts.
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

/* NOTE - this is the stm32f10x_it file converted to support the 205 */
/* Includes ------------------------------------------------------------------*/
#include "project.h"
#include "stm32f2xx_conf.h"

void __iar_program_start( void );
void SystemInit (void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Reset
* Description    : This function handles The reset vector.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetVectorHandler(void)
{
    SystemInit ();
    __iar_program_start();
}

/*******************************************************************************
* Function Name  : NMIException
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMIException(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFaultException(uint32_t from)
{
  TAP_AssertPrint("Hardfault", from);
}

/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManageException(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  debug_assert(0);
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFaultException(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  debug_assert(0);
}

/*******************************************************************************
* Function Name  : UsageFaultException
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFaultException(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  debug_assert(0);
}

/*******************************************************************************
* Function Name  : DebugMonitor
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMonitor(void)
{
}

/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVCHandler(void)
{
}

/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSVC(void)
{
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTickHandler(void)
{
}

#ifndef NEW_INTERRUPT_SCHEME
/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ WWDG_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ PVD_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TAMPER_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ RTC_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ FLASH_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ RCC_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ EXTI0_IRQHandler(void)
{
   debug_assert(0);
   EXTI->PR = EXTI_Line0;
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ EXTI1_IRQHandler(void)
{
   debug_assert(0);
   EXTI->PR = EXTI_Line1;
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ EXTI2_IRQHandler(void)
{
   debug_assert(0);
   EXTI->PR = EXTI_Line2;
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ EXTI3_IRQHandler(void)
{
   debug_assert(0);
   EXTI->PR = EXTI_Line3;
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ EXTI4_IRQHandler(void)
{
   debug_assert(0);
   EXTI->PR = EXTI_Line4;
}

/*******************************************************************************
* Function Name  : DMA1_Stream0_IRQHandler
* Description    : This function handles DMA1 Stream 0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA1_Stream0_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : DMA1_Stream1_IRQHandler
* Description    : This function handles DMA1 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA1_Stream1_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : DMA1_Stream2_IRQHandler
* Description    : This function handles DMA1 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA1_Stream2_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : DMA1_Stream3_IRQHandler
* Description    : This function handles DMA1 Channel 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA1_Stream3_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : DMA1_Stream4_IRQHandler
* Description    : This function handles DMA1 Channel 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA1_Stream4_IRQHandler(void)
{
   debug_assert(0);
}


/*******************************************************************************
* Function Name  : DMA1_Stream5_IRQHandler
* Description    : This function handles DMA1 Channel 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA1_Stream5_IRQHandler(void)
{
    debug_assert(0);
}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 Channel 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA1_Stream6_IRQHandler(void)
{
    debug_assert(0);
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ ADC1_2_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ USB_HP_CAN_TX_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ USB_LP_CAN_RX0_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ CAN_RX1_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ CAN_SCE_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ EXTI9_5_IRQHandler(void)
{
    debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM1_BRK_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM1_UP_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM1_TRG_COM_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM1_CC_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM2_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM3_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM4_IRQHandler(void)
{
    debug_assert(0);
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ I2C1_EV_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ I2C1_ER_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ I2C2_EV_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ I2C2_ER_IRQHandler(void)
{
   debug_assert(0);
}

// SPI ISRs are handled in SpiDriver.c

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ EXTI15_10_IRQHandler(void)
{

}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ RTCAlarm_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ USBWakeUp_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : This function handles TIM8 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM8_BRK_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : This function handles TIM8 overflow and update interrupt
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM8_UP_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM8_TRG_COM_IRQHandler
* Description    : This function handles TIM8 Trigger and commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM8_TRG_COM_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM8_CC_IRQHandler
* Description    : This function handles TIM8 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM8_CC_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : ADC3_IRQHandler
* Description    : This function handles ADC3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ ADC3_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : FSMC_IRQHandler
* Description    : This function handles FSMC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ FSMC_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ SDIO_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM5_IRQHandler(void)
{
    debug_assert(0);
}

/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles SPI3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ SPI3_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM6_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ TIM7_IRQHandler(void)
{
   //IrTask_HandleTimerInterrupt();
}

/*******************************************************************************
* Function Name  : DMA2_Channel1_IRQHandler
* Description    : This function handles DMA2 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA2_Channel1_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : DMA2_Channel2_IRQHandler
* Description    : This function handles DMA2 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA2_Channel2_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : DMA2_Channel3_IRQHandler
* Description    : This function handles DMA2 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA2_Channel3_IRQHandler(void)
{
   debug_assert(0);
}

/*******************************************************************************
* Function Name  : DMA2_Channel4_5_IRQHandler
* Description    : This function handles DMA2 Channel 4 and DMA2 Channel 5
*                  interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void __arm_irq__ DMA2_Channel4_5_IRQHandler(void)
{
   debug_assert(0);
}

void __arm_irq__ USART1_IRQHandler(void)
{
    BUSART_IRQ(BUSART1);
}

void __arm_irq__ USART2_IRQHandler(void)
{
    BUSART_IRQ(BUSART2);
}

void __arm_irq__ USART3_IRQHandler(void)
{
    BUSART_IRQ(BUSART3);
}

void __arm_irq__ UART4_IRQHandler(void)
{
    BUSART_IRQ(BUSART4);
}

void __arm_irq__ UART5_IRQHandler(void)
{
    BUSART_IRQ(BUSART5);
}

void __arm_irq__ Unhandled_IRQ(void)
{
    debug_assert(0);
}
#endif //  NEW_INTERRUPT_SCHEME

#ifndef IAR_COMPILER
#include <sys/types.h>

caddr_t _sbrk ( int incr )
{
   return 0;
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
