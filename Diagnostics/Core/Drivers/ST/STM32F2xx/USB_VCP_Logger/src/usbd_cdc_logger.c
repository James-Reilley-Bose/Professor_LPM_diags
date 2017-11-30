/**
	File	:	usbd_cdc_logger.c
	Title	:	
	Author	:	dx1007255
	Created	:	12/29/2015
	Language:	C
	Copyright:	(C) 2015 Bose Corporation, Framingham, MA

	Description:   Implementating interface to USB CDC logger and
                       providing user callback functions.
===============================================================================
*/ 

#include "project.h"
#include "usb_conf.h"
#include "usbd_conf.h"
#include "usbd_cdc_core.h"
#include "usbd_desc.h"
#include "string.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */

/* Private function prototypes -----------------------------------------------*/
static uint16_t UsbLogger_Init     (void);
static uint16_t UsbLogger_DeInit   (void);
static uint16_t UsbLogger_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
static uint16_t UsbLogger_DataTx   ();
static uint16_t UsbLogger_DataRx (uint8_t* Buf, uint32_t Len);

void     USBD_USR_Init(void);
void     USBD_USR_DeviceReset (uint8_t speed);
void     USBD_USR_DeviceConfigured (void);
void     USBD_USR_DeviceSuspended(void);
void     USBD_USR_DeviceResumed(void);
void     USBD_USR_DeviceConnected(void);
void     USBD_USR_DeviceDisconnected(void); 

// File operations
CDC_IF_Prop_TypeDef VCP_fops = 
{
  UsbLogger_Init,
  UsbLogger_DeInit,
  UsbLogger_Ctrl,
  UsbLogger_DataTx,
  UsbLogger_DataRx
};

// User callbacks
USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
    
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};

/* --------------------------------------------------
 * @func USB_LoggerTaskInit (void)
 *
 * @brief Initialize the USB port
 *
 --------------------------------------------------- */
void USB_LoggerTaskInit (void)
{  
    portENTER_CRITICAL();
    /* Initialize our USB device  */
    USBD_Init(&USB_OTG_dev,
  #ifdef USE_USB_OTG_HS 
              USB_OTG_HS_CORE_ID,
  #else            
              USB_OTG_FS_CORE_ID,
  #endif  
              &USR_desc, 
              &USBD_CDC_cb, 
              &USR_cb);
    portEXIT_CRITICAL();
}

void USB_LoggerEnterStandby(uint8_t enterStby)
{
    USB_OTG_CORE_HANDLE *pdev = &USB_OTG_dev;
    USB_OTG_GCCFG_TypeDef pwrdwn;
    pwrdwn.d32 = USB_OTG_READ_REG32(&pdev->regs.GREGS->GCCFG);
    
    if(enterStby)
    {
        pwrdwn.b.pwdn = 0; // name makes it seem it should be opposite but it is really 0 power down
        pwrdwn.b.vbussensingA = 0;
        pwrdwn.b.vbussensingB = 0;
        USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GCCFG, pwrdwn.d32);

        USB_OTG_PCGCCTL_TypeDef power;
        /*  switch-off the clocks */
        power.d32 = 0;
        power.b.stoppclk = 1;
        USB_OTG_MODIFY_REG32(pdev->regs.PCGCCTL, 0, power.d32);

        power.b.gatehclk = 1;
        USB_OTG_MODIFY_REG32(pdev->regs.PCGCCTL, 0, power.d32);
    }
    else
    {
        if(pwrdwn.b.pwdn == 0) // If zero, it's ok to initialize. This is for coldbooting, where it was not previously shutdown.
        {
            USB_LoggerTaskInit();
        }
    }
}

/* --------------------------------------------------
 * @func USB_LoggerPrintString
 *
 * @param Buf - Pointer to a string
 *
 * @brief Output a string to the logger
 *
 --------------------------------------------------- */
void USB_LoggerPrintString(const char *Buf)
{
    uint32_t Len = strlen(Buf);
   
    uint32_t tempIndex = APP_Rx_ptr_in + Len;
    
    if(tempIndex < APP_RX_DATA_SIZE)
    {
        memcpy(&APP_Rx_Buffer[APP_Rx_ptr_in], Buf, Len);
    }
    else
    { 
        // To avoid buffer overflow
        tempIndex -= APP_RX_DATA_SIZE;
        
        uint32_t residual = APP_RX_DATA_SIZE - APP_Rx_ptr_in;
        
        if(residual)
            memcpy(&APP_Rx_Buffer[APP_Rx_ptr_in], Buf, residual);
        
        // Copy the remaining part
        memcpy(APP_Rx_Buffer, &Buf[residual], Len - residual);
    }
        
    APP_Rx_ptr_in = tempIndex;      
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  UsbLogger_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t UsbLogger_Init(void)
{
  return USBD_OK;
}

/**
  * @brief  UsbLogger_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t UsbLogger_DeInit(void)
{
   return USBD_OK;
}


/**
  * @brief  UsbLogger_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t UsbLogger_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
  case GET_ENCAPSULATED_RESPONSE:
  case SET_COMM_FEATURE:
  case GET_COMM_FEATURE:
  case CLEAR_COMM_FEATURE:
  case SET_LINE_CODING:
  case GET_LINE_CODING:
  case SET_CONTROL_LINE_STATE:
  case SEND_BREAK:
    /* Not  needed for this driver */
    break;    
    
  default:
    break;
  }

  return USBD_OK;
}

/**
  * @brief  UsbLogger_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in 
  *         this function.
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t UsbLogger_DataTx (void)
{
    return USBD_OK;
}

/**
  * @brief  UsbLogger_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t UsbLogger_DataRx (uint8_t* Buf, uint32_t Len)
{
  return USBD_OK;
}

/**
* @brief  USBD_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{  
}

/**
* @brief  USBD_USR_DeviceReset 
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed )
{
 switch (speed)
 {
   case USB_OTG_SPEED_HIGH: 
     break;

  case USB_OTG_SPEED_FULL: 
     break;
  
  default:
     break;
 }
}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConfigured (void)
{
}

/**
* @brief  USBD_USR_DeviceSuspended 
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
}


/**
* @brief  USBD_USR_DeviceResumed 
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConnected (void)
{
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceDisconnected (void)
{
}
/**
* @}
*/ 

/**
* @}
*/ 
