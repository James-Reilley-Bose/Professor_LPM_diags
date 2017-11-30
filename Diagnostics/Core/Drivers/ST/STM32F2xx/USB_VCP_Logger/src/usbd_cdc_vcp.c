/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Generic media access Layer.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED 
#pragma     data_alignment = 4 
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"
#include "usb_conf.h"
#include "usb_bsp.h"
#include "instrument.h"

/* Added by STM to detect connectivity to the serial terminal */
extern __IO uint32_t wValue;
__IO uint32_t USB_Set_Control_Line_State = 0;


extern USB_OTG_CORE_HANDLE USB_OTG_dev;
extern uint8_t  USB_Tx_State;

/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */



/* Private variables ---------------------------------------------------------*/
LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };




/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
static uint16_t VCP_DataTx   (uint8_t* Buf);
static uint16_t VCP_DataRx   (uint8_t* Buf, uint32_t Len);


/* application file operations function structure. */
CDC_IF_Prop_TypeDef VCP_fops = 
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  VCP_DataTx,
  VCP_DataRx
};

/** -----------------------------------------------------------
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  -------------------------------------------------------------*/
static uint16_t VCP_Init(void)
{
  return USBD_OK;
}
/** --------------------------------------------------------
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  ----------------------------------------------------------*/
static uint16_t VCP_DeInit(void)
{
  return USBD_OK;
}
/** ------------------------------------------------------------------
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  ---------------------------------------------------------------------*/
static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:
    linecoding.bitrate    = (uint32_t)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
    linecoding.format     = Buf[4];
    linecoding.paritytype = Buf[5];
    linecoding.datatype   = Buf[6];
    break;

  case GET_LINE_CODING:
    Buf[0] = (uint8_t)(linecoding.bitrate);
    Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
    Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
    Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
    Buf[4] = linecoding.format;
    Buf[5] = linecoding.paritytype;
    Buf[6] = linecoding.datatype; 
    break;

  case SET_CONTROL_LINE_STATE:
    /* Added by STM to detect connectivity to the serial terminal */
    if (wValue & 0x1)  
    {
        USB_Set_Control_Line_State = 1;
    }
    else
    {
        USB_Set_Control_Line_State = 0;
    }
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;    
    
  default:
    break;
  }

  return USBD_OK;
}
//----------------------------------------------------------------------
// Function:  USB_PrintChar(const char ch)
//
// Description: Prints a character to the connected serial terminal. 
//              It does this by filling APP_Rx_Buffer[] and incrementing
//              APP_Rx_ptr_in. When the hardware (AHB and USB) is 
//              ready to send characters, it will set a transfer 
//              complete interrupt (OTG_FS_DIEPINTx<0> XFRC -- 
//              see RM0033,rev7,pp 1021). The interrupt handler will
//              check if any new characters have been added to APP_Rx_Buffer[]
//              via the APP_Rx_ptr_in value since the last transfer. Any
//              new characters are sent to the serial terminal.
//
//
// Parameters:  const char ch - character to print.
//
//----------------------------------------------------------------------
void USB_PrintChar(const char ch)
{
       /* make sure we are enumerated (configured) AND 
          have a connection to a serial terminal before 
          transmitting. */
        if( (USB_OTG_dev.dev.device_status     == USB_OTG_CONFIGURED) &&
            (USB_OTG_dev.dev.connection_status == 1)                  &&
            (USB_Set_Control_Line_State        == 1) )
        {
                
                APP_Rx_Buffer[APP_Rx_ptr_in] = ch;

                /* We don't want the writes to this buffer to
                   outrace the USB hardware Tx task. */
                USB_OTG_BSP_uDelay (20);

                APP_Rx_ptr_in++; 

                /* To avoid buffer overflow */
                if(APP_Rx_ptr_in >= APP_RX_DATA_SIZE)
                {
                        APP_Rx_ptr_in = 0;
                }  
        }
}

extern USB_OTG_CORE_HANDLE  USB_OTG_dev; // defined in main.c

void USB_FlushTxBuffers(void)
{
    /*Send zero-length packet so that lower level USB driver clears its buffer */
    DCD_EP_Tx (&USB_OTG_dev, CDC_IN_EP, 0, 0);
}

/** ---------------------------------------------------------------
  * @brief  VCP_DataTx
  *         PrintString-like functionality for the USB driver.
  * @param  Buf: Buffer of data to be sent
  * @retval Result of the operation: USBD_OK 
  --------------------------------------------------------------- */
static uint16_t VCP_DataTx (uint8_t* s)
{
        while(*s) {
          USB_PrintChar(*s++);
        }
        return USBD_OK;
}
/** -------------------------------------------------------------------------------
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         until exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  ------------------------------------------------------------------------------- */
static uint16_t VCP_DataRx (uint8_t* Buf, uint32_t Len)
{
  uint32_t i;
  
  for (i = 0; i < Len; i++)
  {
    //USART_SendData(EVAL_COM1, *(Buf + i) );
    //while(USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TXE) == RESET); 
  } 
 
  return USBD_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
