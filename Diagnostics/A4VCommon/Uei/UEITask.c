/**
  ******************************************************************************
  * @file    UEITask.c
  * @author  cc1014948
  * @version V0.0.1
  * @date    04-May-2015
  * @brief   UEI Task.
  *          This file provides firmware functions to manage the following
  *          functionalities of the UEI:
  *           + Task communications
  *           + Init & Deinit of drivers
  *           + Resource allocations
 @verbatim
 ===============================================================================
 **/
#include "UEITask.h"
#include "project.h"
#include "TaskManager.h"
#include "TaskDefs.h"
#include "UEIServices.h"
#include "UEISerial.h"
#include "BoseUSART_API.h"
#include "buffermanager.h"



SCRIBE(uei_serial,ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(uei_service,ROTTEN_LOGLEVEL_NORMAL);
SCRIBE(uei_task,ROTTEN_LOGLEVEL_DISABLED);

/* Local functions */
static void InitUEIUart(void);

ManagedTask* ManagedUEIBlasterTask;

/* Configurations for the UART 610LP chip */
static const BU_InitTypeDef buartInit =
{
    BR_19200,   // 9,600
    WL_8BITS,  // 8 bit
    SB_1,      // 1 stop bit
    P_NONE,    // No parity
    M_RX_TX,   // RX and TX
    FC_NONE    // No Flow Control
};


static bool g_bBlasterOperation = FALSE;


////////////////////////
//Basic Task functions//
////////////////////////
/*
===============================================================================
@fn Function: UEIBlasterModule_Init()
@brief Initialize routine, init IOs, semaphore
===============================================================================
*/
void UEIBlasterModule_Init(void* p)
{
  InitUEIUart();
  UEI_InitSerial();

  ManagedUEIBlasterTask = GetManagedTaskPointer("UEIBlasterTask");
}

/*
===============================================================================
@fn Function: UEIBlasterModule_PowerInit()
@brief Power informing that supply for chip is ready and perform a reset
===============================================================================
*/
void UEIBlasterModule_PowerInit(void)
{
  g_bBlasterOperation = TRUE;

  UEIBlasterPostMsg(UEIBLASTER_MESSAGE_ID_PowerUp, NOP_CALLBACK, NULL);

}

/*
===============================================================================
@fn Function: InitUEIUart()
@brief Initialize routine, init IOs, semaphore
===============================================================================
*/
static void InitUEIUart(void)
{
  // Setup interupt on the UART
  BUSART_PlatformBindInterruptHandler(UEI_BUART, UEI_IRQHandler, TRUE);

  // GPIO Init
  ConfigureGpioPin (UEI_UART_RX_GPIO_BANK, UEI_RX_PIN, GPIO_MODE_AF_OUT_PP, UEI_AF);
  ConfigureGpioPin (UEI_UART_TX_GPIO_BANK, UEI_TX_PIN, GPIO_MODE_AF_OUT_PP, UEI_AF);
  ConfigureGpioPin (QS_RST_L_GPIO_BANK, QS_RST_L_GPIO_PIN, GPIO_MODE_OUT_PP, UEI_AF);

  // pull the RESET_L pin high
  GPIO_SetBits(QS_RST_L_GPIO_BANK, 1 << QS_RST_L_GPIO_PIN);

  // Configure the UART for UEI
  BUSART_DeInit(UEI_BUART);
  BUSART_Init(UEI_BUART, &buartInit);

  BUSART_ITConfig(UEI_BUART, IRQ_RXNE, PS_ENABLE);
  BUSART_ITConfig(UEI_BUART, IRQ_TXE, PS_DISABLE);

  BUSART_Cmd(UEI_BUART, PS_ENABLE);
}

/*
===============================================================================
@fn Function: UEIBlasterModule_DeInit()
@brief De-initilization routine
===============================================================================
*/
void UEIBlasterModule_DeInit(void)
{
  g_bBlasterOperation = FALSE;
}


/*
===============================================================================
@fn Function: UEIBlasterModule()
@brief Task routine for UEI serial driver, called by operating system
===============================================================================
*/
void UEIBlasterModule (void *pvParamaters)
{

    debug_assert(ManagedUEIBlasterTask != NULL);

    for(;;)
    {
        TaskManagerPollQueue(ManagedUEIBlasterTask);
    }
}

/*
===============================================================================
@fn Function: UEIBlasterModule_HandleMessage()
@brief Task handler function for UEI serial driver
===============================================================================
*/
void UEIBlasterModule_HandleMessage(GENERIC_MSG_t* msg)
{
    static BOOL m_inFlightFromTAP = FALSE;

    if (g_bBlasterOperation == FALSE)
    {
        LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t isn't ready for  %s",GetEventString(msg->msgID));
        return;
    }

    switch (msg->msgID)
    {
        case UEIBLASTER_MESSAGE_ID_PowerUp:
            // Give the blaster module a chance to stabilize - PowerOnReset time from spec is 500 msec
            vTaskDelay(TIMER_MSEC_TO_TICKS(500));

            break;

        case UEIBLASTER_MESSAGE_ID_SendFromIPC:
            m_inFlightFromTAP = FALSE;
            UEI_BlasterSendMsg((uint8_t *)msg->params[0], (uint16_t)msg->params[1]);
            break;

        case UEIBLASTER_MESSAGE_ID_SendFromTAP:
            m_inFlightFromTAP = TRUE;
            UEI_BlasterSendMsg((uint8_t *)msg->params[0], (uint16_t)msg->params[1]);
            break;    

        case UEIBLASTER_MESSAGE_ID_KeyRelease:
            UEI_WakeUp();
            break; 

        case UEIBLASTER_MESSAGE_ID_ReceiveMsg:
            UEI_BlasterGetResp(m_inFlightFromTAP);
            m_inFlightFromTAP = FALSE;
            break;

        default:
            LOG(uei_task,ROTTEN_LOGLEVEL_NORMAL,"\tUEIBlaster\t%s was invalid message",GetEventString(msg->msgID));
            break;
    }
}


