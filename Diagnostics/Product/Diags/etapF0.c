/*
  File  : etapF0.c
  Title :
  Author  : Frank Mazzarella
  Created : 01/27/2016
  Language: C
  Copyright:  (C) 2016 Bose Corporation, Framingham, MA

  Description:
                i2c F0 TAP communications
===============================================================================
*/
#include "project.h"
#include "etap.h"
#include "TapListenerAPI.h"
#include "gpio_mapping.h"
#if defined(BARDEEN)
#include "bardeen_pin_mapping.h"
#endif
#include "diag_cec.h"
#include "instrument.h"
#include "i2cMaster_API.h"
#include "BoseUSART_API.h"
#include "f0_cmd_interface.h"
#include "etapF0.h"
#include <stdlib.h>

#include "IpcProtocolLpm.h"  //JFR

//JFR hacks
#define GPIO_MODER_INPUT    (uint8_t)(0x00)
#define GPIO_MODER_OUTPUT   (uint8_t)(0x01)
#define GPIO_READ_DELAY_MS  5
#define HIGH         (ENABLE)
#define LOW          (DISABLE)

extern IpcF0HealthStatusPayload_t F0HS;

SCRIBE_DECL(diag);


/* packet buffer */
static uint8_t g_f0_i2c_packet[ I2C_TX_BUFFERSIZE ];

/* F0 GPIO pin strings */
#if defined(BARDEEN)
//const char* F0_AP_BOOT_SEL_name        = "F0_AP_BOOT_SEL        (PF1) ";
#endif
//const char* F0_PWR_EN_AP_name          = "F0_PWR_EN_AP          (PA8) ";
//const char* F0_AP_RST_L_name           = "F0_AP_RST_L           (PB1) ";
/* F0 HDMI pins */
const char* F0_PWR_HDMI_EN_name        = "F0_PWR_HDMI_EN        (PA1) ";
const char* F0_PWR_HDMI_5V_EN_name     = "F0_PWR_HDMI_5V_EN     (PA12)";
#if defined(BARDEEN)
//const char* F0_HDMI1_RST_L_name        = "F0_HDMI1_RST_L        (PA15)";
#else
//const char* F0_HDMI1_RST_L_name        = "F0_HDMI_RST_L         (PA15)";
#endif
//const char* F0_HDMI2_RST_L_name        = "F0_HDMI2_RST_L        (PB0) ";
/* F0 SM2 pins */
const char* F0_PWR_RIVIERA_EN_name     = "F0_PWR_RIVIERA_EN     (PA11)";
//const char* F0_PWR_SM2_EN_name         = "F0_PWR_SM2_EN         (PA7) ";
//const char* F0_SM2_USB0_VBUS_EN_H_name = "F0_SM2_USB0_VBUS_EN_H (PF0) ";

/* F0 Mfg error string for a non-even, hex nibble count */
const char* mfg_hex_char_error_msg =
    "\nError: Each hex char must be 2-nibble hex! ex. 01==1, 3e==3e, etc.\n";

//-------------------------------------------------------------------
// Function:  TAPCommand(TAP_F0)
// Description: Supports reading\writing the F0 GPIO pins.
//--------------------------------------------------------------------
TAPCommand(TAP_F0)
{


    if ( !strcmp("hdmipwr", CommandLine->args[0]) )
    {
        if (CommandLine->numArgs < 2)
        {
            TAP_Printf( F0_MISSING_ARGS);
        }
        else
        {
            if ( !strcmp("0", CommandLine->args[1]) )
            {
                f0_gpio_pin_tap(  "0", F0_PWR_HDMI_EN_name, CMD_PWR_HDMI_EN_WRITE, CMD_PWR_HDMI_EN_READ );
            }
            else if ( !strcmp("1", CommandLine->args[1]) )
            {
                f0_gpio_pin_tap(  "1", F0_PWR_HDMI_EN_name, CMD_PWR_HDMI_EN_WRITE, CMD_PWR_HDMI_EN_READ );
            }
            else if ( !strcmp("st", CommandLine->args[1]) )
            {
                f0_gpio_pin_tap(  "st", F0_PWR_HDMI_EN_name, CMD_PWR_HDMI_EN_WRITE, CMD_PWR_HDMI_EN_READ );
            }
            else
            {
                TAP_Printf( F0_INVALID_ARGS);
            }
        }

    }
    else if ( !strcmp("hdmi5v", CommandLine->args[0]) )
    {
        if (CommandLine->numArgs < 2)
        {
            TAP_Printf( F0_MISSING_ARGS);
        }
        else
        {
            if ( !strcmp("0", CommandLine->args[1]) )
            {
                f0_gpio_pin_tap(  "0", F0_PWR_HDMI_5V_EN_name, CMD_HDMI_5V_EN_WRITE, CMD_HDMI_5V_EN_READ );
            }
            else if ( !strcmp("1", CommandLine->args[1]) )
            {
                f0_gpio_pin_tap(  "1", F0_PWR_HDMI_5V_EN_name, CMD_HDMI_5V_EN_WRITE, CMD_HDMI_5V_EN_READ );
            }
            else if ( !strcmp("st", CommandLine->args[1]) )
            {
                f0_gpio_pin_tap(  "st", F0_PWR_HDMI_5V_EN_name, CMD_HDMI_5V_EN_WRITE, CMD_HDMI_5V_EN_READ );
            }
            else
            {
                TAP_Printf( F0_INVALID_ARGS);
            }
        }

    }
    else if ( !strcmp("pwr", CommandLine->args[0]) )
    {
        if (CommandLine->numArgs < 2)
        {
            TAP_Printf( F0_MISSING_ARGS);
        }
        else
        {
            f0_pwr_tap( CommandLine->args[1] );
        }

    }
    else if ( !strcmp("vr", CommandLine->args[0]) )
    {
        f0_variant_cmd(F0_VERSION);
    }
    else if ( !strcmp("reset", CommandLine->args[0]) )
    {
        TAP_Printf( "\r\n\tResetting the F0.\r\n");
        f0_reset();
    }
    else if ( !strcmp("diags", CommandLine->args[0]) )
    {
        f0_diags(1);
    }
    else if ( !strcmp("help", CommandLine->args[0]) )
    {
        TAP_Printf( F0_HELP_TEXT_4_OF_4);
    }
    else
    {

        TAP_Printf( F0_ERROR_ARGS);
        //TAP_Printf( F0_HELP_TEXT_1_OF_4);
        //TAP_Printf( F0_HELP_TEXT_2_OF_4);
        //TAP_Printf( F0_HELP_TEXT_3_OF_4);
        TAP_Printf( F0_HELP_TEXT_4_OF_4);
    }
}
//-------------------------------------------------------------------
// Function:  void f0_diags(uint32_t loop_count)
// Description: Run all F0 commands for max loop_count.
// Parameters:  int loop_count - max loops
//--------------------------------------------------------------------
void f0_diags(uint32_t loop_count)
{
    /* for the f0 mfg tap command. */
    CommandLine_t cmd = {.command = "f0", .numArgs = 2, .args[0] = "mfg", .args[1] = "all"};

    TAP_Printf( "\r\n\trunning LPM <-i2c-> F0 diagnotics.\r\n");
    for ( uint32_t i = 0; i < loop_count; i++)
    {
        TAP_Printf( "\r\n\t ************ Loop [%d] ****************\r\n", i + 1);
        f0_gpio_pin_tap(  "0", F0_PWR_HDMI_EN_name, CMD_PWR_HDMI_EN_WRITE, CMD_PWR_HDMI_EN_READ );
        f0_gpio_pin_tap( "st", F0_PWR_HDMI_EN_name, CMD_PWR_HDMI_EN_WRITE, CMD_PWR_HDMI_EN_READ );
        f0_gpio_pin_tap(  "1", F0_PWR_HDMI_EN_name, CMD_PWR_HDMI_EN_WRITE, CMD_PWR_HDMI_EN_READ );
        f0_gpio_pin_tap( "st", F0_PWR_HDMI_EN_name, CMD_PWR_HDMI_EN_WRITE, CMD_PWR_HDMI_EN_READ );

        TAP_Printf( "\r\n");

        f0_gpio_pin_tap(  "0", F0_PWR_HDMI_5V_EN_name, CMD_HDMI_5V_EN_WRITE, CMD_HDMI_5V_EN_READ );
        f0_gpio_pin_tap( "st", F0_PWR_HDMI_5V_EN_name, CMD_HDMI_5V_EN_WRITE, CMD_HDMI_5V_EN_READ );
        f0_gpio_pin_tap(  "1", F0_PWR_HDMI_5V_EN_name, CMD_HDMI_5V_EN_WRITE, CMD_HDMI_5V_EN_READ );
        f0_gpio_pin_tap( "st", F0_PWR_HDMI_5V_EN_name, CMD_HDMI_5V_EN_WRITE, CMD_HDMI_5V_EN_READ );

        TAP_Printf( "\r\n");

#if 0
        TAP_Printf( "\r\n");
        f0_gpio_pin_tap(  "0", F0_PWR_SM2_3V8_EN_name, CMD_PWR_SM2_3V8_EN_WRITE, CMD_PWR_SM2_3V8_EN_READ );
        f0_gpio_pin_tap( "st", F0_PWR_SM2_3V8_EN_name, CMD_PWR_SM2_3V8_EN_WRITE, CMD_PWR_SM2_3V8_EN_READ );
        f0_gpio_pin_tap(  "1", F0_PWR_SM2_3V8_EN_name, CMD_PWR_SM2_3V8_EN_WRITE, CMD_PWR_SM2_3V8_EN_READ );
        f0_gpio_pin_tap( "st", F0_PWR_SM2_3V8_EN_name, CMD_PWR_SM2_3V8_EN_WRITE, CMD_PWR_SM2_3V8_EN_READ );

        TAP_Printf( "\r\n");

        f0_gpio_pin_tap(  "0", F0_PWR_SM2_EN_name, CMD_PWR_SM2_EN_WRITE, CMD_PWR_SM2_EN_READ );
        f0_gpio_pin_tap( "st", F0_PWR_SM2_EN_name, CMD_PWR_SM2_EN_WRITE, CMD_PWR_SM2_EN_READ );
        f0_gpio_pin_tap(  "1", F0_PWR_SM2_EN_name, CMD_PWR_SM2_EN_WRITE, CMD_PWR_SM2_EN_READ );
        f0_gpio_pin_tap( "st", F0_PWR_SM2_EN_name, CMD_PWR_SM2_EN_WRITE, CMD_PWR_SM2_EN_READ );

        TAP_Printf( "\r\n");

        f0_gpio_pin_tap(  "0", F0_SM2_USB0_VBUS_EN_H_name, CMD_SM2_USB0_VBUS_EN_H_WRITE, CMD_SM2_USB0_VBUS_EN_H_READ );
        f0_gpio_pin_tap( "st", F0_SM2_USB0_VBUS_EN_H_name, CMD_SM2_USB0_VBUS_EN_H_WRITE, CMD_SM2_USB0_VBUS_EN_H_READ );
        f0_gpio_pin_tap(  "1", F0_SM2_USB0_VBUS_EN_H_name, CMD_SM2_USB0_VBUS_EN_H_WRITE, CMD_SM2_USB0_VBUS_EN_H_READ );
        f0_gpio_pin_tap( "st", F0_SM2_USB0_VBUS_EN_H_name, CMD_SM2_USB0_VBUS_EN_H_WRITE, CMD_SM2_USB0_VBUS_EN_H_READ );

        TAP_Printf( "\r\n");
#endif
        TAP_Printf( "\r\n");
        f0_pwr_tap( "dis" );
        TAP_Printf( "\r\n");
        f0_pwr_tap( "st" );
        TAP_Printf( "\r\n");
        f0_pwr_tap( "en" );
        TAP_Printf( "\r\n");
        f0_pwr_tap( "st" );
        TAP_Printf( "\r\n");
        f0_mfg_params_tap(&cmd);
        TAP_Printf( "\r\n");

        f0_variant_cmd(F0_VERSION);

        TAP_Printf( "\r\n");
    }
}
//-------------------------------------------------------------------
// Function:  void f0_pwr_tap(char *arg)
// Description: Tap wrapper function for all power initialization
//              operations.
// Parameters:  char *arg - 2nd Tap cmd arg.
//--------------------------------------------------------------------
void f0_pwr_tap( char* arg )
{
    if ( !strcmp("en", arg ))
    {
        /* enable power initialization sequence */
        f0_pwr_seq_cmd( F0_PWR_INIT_SEQ_EN );

    }
    else if ( !strcmp("dis", arg) )
    {
        /* disable power initialization sequence */
        f0_pwr_seq_cmd( F0_PWR_INIT_SEQ_DIS );
    }
    else if ( !strcmp("st", arg) )
    {
        /* get the state of all gpio pins involved
           in the power initialization sequence */

        TAP_Printf( "\r\n\tHDMI Gpio Pins");
        TAP_Printf( "\r\n\t----------------------------------------------");
        f0_gpio_pin_tap(  "st", F0_PWR_HDMI_EN_name, CMD_PWR_HDMI_EN_WRITE, CMD_PWR_HDMI_EN_READ );
        Delay_ms(100);
        f0_gpio_pin_tap(  "st", F0_PWR_HDMI_5V_EN_name, CMD_HDMI_5V_EN_WRITE, CMD_HDMI_5V_EN_READ );

#if 0
        f0_gpio_pin_tap( arg,
                         F0_PWR_HDMI_EN_name,
                         CMD_PWR_HDMI_EN_WRITE,
                         CMD_PWR_HDMI_EN_READ );
        f0_gpio_pin_tap( arg,
                         F0_PWR_HDMI_5V_EN_name,
                         CMD_HDMI_5V_EN_WRITE,
                         CMD_HDMI_5V_EN_READ );
        f0_gpio_pin_tap( arg,
                         F0_HDMI1_RST_L_name,
                         CMD_HDMI1_RST_L_WRITE,
                         CMD_HDMI1_RST_L_READ );
        TAP_Printf( "\r\n\r\n\tSM2 Gpio Pins");
        TAP_Printf( "\r\n\t----------------------------------------------");
        f0_gpio_pin_tap( arg,
                         F0_PWR_SM2_3V8_EN_name,
                         CMD_PWR_SM2_3V8_EN_WRITE,
                         CMD_PWR_SM2_3V8_EN_READ );
        f0_gpio_pin_tap( arg,
                         F0_PWR_SM2_EN_name,
                         CMD_PWR_SM2_EN_WRITE,
                         CMD_PWR_SM2_EN_READ );
        f0_gpio_pin_tap( arg,
                         F0_SM2_USB0_VBUS_EN_H_name,
                         CMD_SM2_USB0_VBUS_EN_H_WRITE,
                         CMD_SM2_USB0_VBUS_EN_H_READ );
        TAP_Printf( "\r\n");
#endif
    }
    else
    {
        TAP_Printf( F0_ERROR_ARGS);
        //TAP_Printf( F0_HELP_TEXT_1_OF_4);
        //TAP_Printf( F0_HELP_TEXT_2_OF_4);
        //TAP_Printf( F0_HELP_TEXT_3_OF_4);
        TAP_Printf( F0_HELP_TEXT_4_OF_4);

    }
}
//-------------------------------------------------------------------
// Function:  void f0_gpio_pin_tap()
// Description: Tap wrapper function for all GPIO pin operations
// Parameters:  char *arg             - 2nd Tap arg
//              const char *pinName   - GPIO Pin string name
//              uint8_t cmd_write     - GPIO Pin write cmd
//              uint8_t cmd_read      - GPIO Pin read cmd
//--------------------------------------------------------------------
void f0_gpio_pin_tap( char* arg,
                      const char* pinName,
                      uint8_t cmd_write,
                      uint8_t cmd_read )
{
    uint8_t gpio_pin_value;

    if ( !strcmp("0", arg ))
    {
        /* write low */
        F0_GPIO_Pin_cmd( cmd_write,
                         pinName,
                         F0_GPIO_SET_LOW,
                         &gpio_pin_value );
    }
    else if ( !strcmp("1", arg) )
    {
        /* write high */
        F0_GPIO_Pin_cmd( cmd_write,
                         pinName,
                         F0_GPIO_SET_HIGH,
                         &gpio_pin_value );
    }
    else if ( !strcmp("st", arg) )
    {
        /* get the state */
        F0_GPIO_Pin_cmd( cmd_read,
                         pinName,
                         0,
                         &gpio_pin_value );
    }
    else
    {
        TAP_Printf( F0_ERROR_ARGS);
        //TAP_Printf( F0_HELP_TEXT_1_OF_4);
        //TAP_Printf( F0_HELP_TEXT_2_OF_4);
        //TAP_Printf( F0_HELP_TEXT_3_OF_4);
        TAP_Printf( F0_HELP_TEXT_4_OF_4);
    }
}
//-------------------------------------------------------------------
// Function: BOOL send_cmd_and_get_response( uint8_t *pkt,
//                                           uint16_t pktSize)
// Description: Main function for sending out the F0 i2c cmd pkt,
//              waiting for a response pkt (via the CEC INT),
//              and checking the response pkt for errors.
// Parameters:  uint8_t *pkt      - F0 i2c cmd pkt
//              uint16_t pktSize  - pkt size in bytes
// Returns: TRUE==success FALSE==failure
//--------------------------------------------------------------------
BOOL send_cmd_and_get_response(uint8_t* pkt, uint16_t pktSize)
{
    uint16_t ret;
    uint8_t cec_int;
    uint8_t checksum_test;
    /* i2c driver variables */
    I2C_API_ERROR_CODE i2cError   = I2C_API_DMA_CONFIG_ERROR; //I2C_API_NO_ERROR;
    I2C_BUS_HANDLE_TYPE I2cHandle = NULL;


    /* initialize I2C bus 3 */
    I2cHandle = init_i2c_bus_3(&i2cError);

    if (I2cHandle == NULL)
    {
        TAP_Printf( "\n\tError: I2C handle == NULL after init\n");
        return (FALSE);
    }

    ret = i2cMaster_WriteNoRegister( I2cHandle,
                                     F0_I2C_ADDR,
                                     pkt,
                                     pktSize ); /* actual size sent out */
    if (ret != pktSize)
    {
        TAP_Printf( "\r\n\tSend pkt Failure: cmd i2c pktSize exp[%d] act[%d] . exiting...\r\n",
            pktSize, ret);
        return (FALSE);
    }



    /* wait for the F0 to set the CECCtrl int line before proceeding. */
    cec_int = check_for_CECCtrl_Int();

    if (!cec_int)
    {
        TAP_Printf( "\r\n\t Error: No CEC INT detected from F0!\r\n");
        return (FALSE);
    }
    /* clear the original packet in preparation for the response pkt from the F0. */
    memset( (void*)(pkt), 0, I2C_TX_BUFFERSIZE);

    /* request the response packet. */
    ret = i2cMaster_ReadNoRegister( I2cHandle,
                                    F0_I2C_ADDR,
                                    pkt,
                                    pktSize );
    if (ret != pktSize)
    {
        TAP_Printf( "\r\n\tFailure: Did not get back "
            "correct i2c F0 response pktSize. act[%d] exp[%d]. exiting...\r\n",
            ret, pktSize);
        return (FALSE);
    }

    /* did the F0 see any transmission errors or unknown commands? */
    if (has_cmd_pkt_errors(pkt, pktSize) == TRUE)
    {
        return (FALSE);
    }


#if 1 //JFR
    /* verify checksum. */
    checksum_test = calc_pkt_checksum(pkt, pktSize);
    checksum_test += pkt[ HDR_PKT_CHECKSUM ];

    if (checksum_test != 0)
    {
        TAP_Printf( "\r\n\tError: checksum [0x%02x] test failed for "
            "F0 status packet!\r\n", checksum_test);
        //               return(FALSE);
    }
    //        TAP_Printf( "\r\n\tF0 sent:%s", &(pkt[4]));
    return (TRUE);

#else

    uint32_t chksum = 0;
    chksum = IpcCalculateXor(pkt, pktSize);
    if (((chksum >> 24) & 0xff) == pkt[124])
    {
        if (((chksum >> 16) & 0xff) == pkt[125])
        {
            if (((chksum >> 8) & 0xff) == pkt[126])
            {
                if (((chksum >> 0) & 0xff) == pkt[127])
                {
                    return (TRUE);
                }
            }
        }
    }

    TAP_Printf( "\r\n\tError: checksum [0x%02x] test failed for "
        "F0 status packet!\r\n", chksum);
    return (FALSE);

#endif
}
//-------------------------------------------------------------------
// Function:  BOOL F0_GPIO_Pin_cmd( uint8_t F0GpioPinCmd,
//                                  const char *F0GpioPinName,
//                                  uint8_t setToState,
//                                  uint8_t *readState)
//
// Description: Main function for reading or writing and F0 GPIO pin.
//              The function creates a checksum'd i2c packet for
//              transmission, and then waits for the CECCtrl int
//              to be set from the F0. Once CECCtrl int is detected,
//              a response request is sent to the F0, which will return
//              the requested information.
//              error checking is performed on the following:
//                   - checksum on both transmit and response i2c packets.
//                   - transmission errors are checked
//                     on the transmit packet.
//                   - boundary error checks on the returned data
//
// Parameters:   uint8_t F0GpioPinCmd       - read/write
//               const char *F0GpioPinName  - string name
//               uint8_t setToState         - if write, set\clear
//               uint8_t *readState         - high\low
// Returns: TRUE==success FALSE==failure
//--------------------------------------------------------------------
BOOL F0_GPIO_Pin_cmd( uint8_t F0GpioPinCmd,
                      const char* F0GpioPinName,
                      uint8_t setToState,
                      uint8_t* readState )
{
    if ( (F0GpioPinName == NULL) || (readState == NULL) )
    {
        return (FALSE);
    }

    F0_GPIO_PIN_struct gpioPin = {0};
    F0_GPIO_PIN_struct gpioPin_rx;
    uint16_t pktSize = (HDR_SIZE + F0_GPIO_PIN_struct_size);

    /* set GPIO pin set\clear for write operation.
       Ignored for reads. */
    gpioPin.setToState = setToState;

    build_f0_packet( g_f0_i2c_packet,             /* i2c packet */
                     I2C_TX_BUFFERSIZE,           /* i2c packet max size (for clearing) */
                     F0GpioPinCmd,                /* F0 Gpio pin operation */
                     (uint8_t*)&gpioPin,          /* data struct for F0 Gpio operation */
                     F0_GPIO_PIN_struct_size );   /* data struct size */

    if (send_cmd_and_get_response(g_f0_i2c_packet, pktSize) == FALSE)
    {
        return (FALSE);
    }

    /* get the gpio data structure */
    memcpy( (void*)(&gpioPin_rx),
            (const void*)(&g_f0_i2c_packet[ HDR_DATA ]),
            F0_GPIO_PIN_struct_size );

    display_F0_gpio_pin_config(F0GpioPinName, &gpioPin_rx);

    return (TRUE);
}
//-------------------------------------------------------------------
// Function:  BOOL f0_pwr_seq_cmd( uint8_t operation )
//
// Description: power sequence command.
//
// Parameters:   uint8_t operation - en\dis\state
// Returns: TRUE==success FALSE==failure
//--------------------------------------------------------------------
BOOL f0_pwr_seq_cmd( uint8_t operation )
{
    /* cheating a little by using the gpio pin struct for pwr init sequence */
    F0_GPIO_PIN_struct pwr = {0};
    F0_GPIO_PIN_struct pwr_rx;
    uint16_t pktSize = (HDR_SIZE + F0_GPIO_PIN_struct_size);

    /* use the GPIO pin setToState field to specify whether en\dis\state operation. */
    pwr.setToState = operation;

    build_f0_packet( g_f0_i2c_packet,                /* i2c packet */
                     I2C_TX_BUFFERSIZE,              /* i2c packet max size (for clearing) */
                     CMD_PWR_INIT_SEQ,               /* pwr init command */
                     (uint8_t*)&pwr,                 /* squatting on F0 Gpio struct */
                     F0_GPIO_PIN_struct_size );      /* pwr data struct size */

    if (send_cmd_and_get_response(g_f0_i2c_packet, pktSize) == FALSE)
    {
        return (FALSE);
    }

    /* get the gpio data structure */
    memcpy( (void*)(&pwr_rx),
            (const void*)(&g_f0_i2c_packet[ HDR_DATA ]),
            F0_GPIO_PIN_struct_size );

    /* display the results */
    if (pwr_rx.readState == F0_PWR_INIT_SEQ_EN)
    {
        TAP_Printf( "\r\n\tPower Initialization Sequence ENABLED.");

    }
    else if (pwr_rx.readState == F0_PWR_INIT_SEQ_DIS)
    {
        TAP_Printf( "\r\n\tPower Initialization Sequence DISABLED.");
    }
    else
    {
        TAP_Printf( "\r\n\tError: Power Initialization Sequence request[0x%02x] unknown!",
            pwr_rx.readState);
    }

    return (TRUE);
}
//-------------------------------------------------------------------
// Function:  BOOL f0_variant_cmd( void )
// Description: gets F0 variant and versioning information.
// Returns: TRUE==success FALSE==failure
//--------------------------------------------------------------------
BOOL f0_variant_cmd( PROFESSOR_F0_CMDS cmd)
{
    #if 0
    F0_VARIANT_struct f0 = {0};
    uint16_t pktSize = (HDR_SIZE + F0_VARIANT_struct_size);

    for (char i = 0; i < 50; i++)
    {
        f0.variant[i] = i;
    }


    /* cmd, etc. loaded in the pkt here */
    build_f0_packet( g_f0_i2c_packet,
                     I2C_TX_BUFFERSIZE,
                     (uint8_t)cmd,
                     (uint8_t*)&f0,
                     F0_VARIANT_struct_size );

    if (send_cmd_and_get_response(g_f0_i2c_packet, pktSize) == FALSE)
    {
        return (FALSE);
    }

    /* get our variant strings (strncpy done on the F0 side) */
    memcpy( (void*)(&f0),
            (const void*)(&g_f0_i2c_packet[ HDR_DATA ]),
            F0_VARIANT_struct_size );

    /* display the results */
    TAP_Printf( "\r\n\tF0 %s Diags Image", f0.variant);
    /* display Bose version strings */

    if (cmd == F0_VERSION)
    {
        TAP_Printf( "\r\n\t%s: %s", f0.version, f0.buildTime );
    }
    //        TAP_Printf( "\r\n\t%s\r\n", f0.copyright);

    return (TRUE);
    #else
    TAP_Printf("F0 Bootloader: %s\n", F0HS.swBootloaderVersion);
    TAP_Printf("F0: %s\n", F0HS.swLongVersion);
    return (TRUE);
    #endif
}
//-------------------------------------------------------------------
// Function: I2C_BUS_HANDLE_TYPE init_i2c_bus_3(I2C_API_ERROR_CODE *i2cError)
// Description: initializes i2c bus 3.
// Parameters:   I2C_API_ERROR_CODE *i2cError - error storage
// Returns: I2C_BUS_HANDLE_TYPE!=NULL--> success
//--------------------------------------------------------------------
I2C_BUS_HANDLE_TYPE init_i2c_bus_3(I2C_API_ERROR_CODE* i2cError)
{
    /* i2c driver variables */
    I2C_Master_Config_t I2cConfig;
    /* bus 3 at 100KHz */
    I2cConfig.busNumber = 3;
    I2cConfig.masterClockFrequency = I2C_MASTER_CLOCK_STANDARD_SPEED_HZ;
    I2cConfig.disableDMA = 1; //disable

    /* initialize I2C bus 3 */
    return ( i2cMaster_Init(&I2cConfig, i2cError) );
}
//-------------------------------------------------------------------
// Function: uint8_t calc_pkt_checksum(uint8_t *pkt, uint16_t size)
// Description: calculates a byte checksum on all bytes in the i2c
//              packet, except for the HDR_PKT_CHECKSUM field.
// Parameters:  uint8_t *buf             - i2c packet
//              uint16_t size            - packet size.
// Returns: byte checksum
//--------------------------------------------------------------------
uint8_t calc_pkt_checksum(uint8_t* pkt, uint16_t size)
{
    uint8_t checksum = 0;

    for (int i = 0; i < size; i++)
    {
        /* don't include the actual checksum value
           (which is really the 2's compl of the checksum). */
        if (i == HDR_PKT_CHECKSUM)
        {
            continue;
        }
        checksum += pkt[i];
    }
    return (checksum);
}

//-------------------------------------------------------------------
// Function: uint8_t check_for_CECCtrl_Int(void)
// Description: The F0 will set CECCtrl int high when it is ready
//              to receive a response request from the LPM. This
//              routine will poll for up to 5 seconds (pwr en takes
//              a long time..) for this transition.
// Returns: CECCtrl int value, 0 or 1.
//--------------------------------------------------------------------
uint8_t check_for_CECCtrl_Int(void)
{
    uint8_t cec_int;
    int wait_chunk = 5000; /* 5000 * 1ms */

    /* now we wait for a status packet to come back from the F0
       (up to 5sec). The F0 will set the shared CEC EXT INT
       pin when it's ready to receive the request for status
       from the LPM. */
    do
    {
        cec_int = read_CECCtrl_INT_GpioPin();
        if (cec_int)
        {
            break;
        }

        vTaskDelay(TIMER_MSEC_TO_TICKS(1));

    }
    while ( wait_chunk-- );

    return (cec_int);
}
//-------------------------------------------------------------------
// Function: BOOL has_cmd_pkt_errors(uint8_t *respPkt, uint16_t size)
// Description: checks the response packet sent from the F0 to determine
//              if there were any errors.
// Returns: TRUE==detected errors or bad args FALSE==no errors detected.
//--------------------------------------------------------------------
BOOL has_cmd_pkt_errors(uint8_t* respPkt, uint16_t size)
{
    uint8_t cmd;
    BOOL status = FALSE;
    uint32_t isr;

    if (respPkt == NULL)
    {
        return (TRUE);
    }

    cmd = respPkt[ HDR_CMD ];

    switch (cmd)
    {
        case CMD_NACK_BAD_CKSUM:
            TAP_Printf( "\r\n\tError: F0 checksum test for cmd pkt failed "
                "orig[0x%02x] failed(should be zero)[0x%02x]!\r\n",
                respPkt[ HDR_DATA + 1], respPkt[ HDR_DATA ]);
            status = TRUE;
            break;
        case CMD_NACK_ISR:
            isr = get_isr_status( respPkt );
            TAP_Printf( "\r\n\tError: F0 isr errors! [0x%08x]\r\n", isr  );
            check_for_f0_isr_errors(isr);
            status = TRUE;
            break;
        case CMD_NACK_UNKNOWN:
            TAP_Printf( "\r\n\tError: F0 cmd pkt has unknown command[0x%02x]!\r\n",
                respPkt[ HDR_DATA ]);
            status = TRUE;
            break;
    }

    return (status);
}
//-------------------------------------------------------------------
// Function: BOOL display_F0_gpio_pin_config( const char *F0GpioPinName,
//                                            F0_GPIO_PIN_struct *gpio )
// Description: Display the GPIO pin status from the F0.
// Parameters: const char *F0GpioPinName - GPIO pin string name
//             F0_GPIO_PIN_struct *gpio  - GPIO pin status struct.
// Returns: TRUE==No errors FALSE==bad args, unknown status.
//--------------------------------------------------------------------
BOOL display_F0_gpio_pin_config(const char* F0GpioPinName, F0_GPIO_PIN_struct* gpio)
{
    if ( (F0GpioPinName == NULL) || (gpio == NULL) )
    {
        return (FALSE);
    }

    /* display the F0 gpio pin settings */
    if (gpio->pol == GPIO_MODER_INPUT)
    {
        TAP_Printf( "\r\n\t%s  Input <%s>",
            F0GpioPinName, (gpio->readState) ? "High" : "Low");
    }
    else if (gpio->pol == GPIO_MODER_OUTPUT)
    {
        TAP_Printf( "\r\n\t%s  Output %s <%s>",
            F0GpioPinName, (gpio->OType) ? "OD" : "PP",
            (gpio->readState) ? "High" : "Low" );
    }
    else
    {
        TAP_Printf( "\r\n\t Error: %s has unknown configuration!",
            F0GpioPinName);
        return (FALSE);
    }

    return (TRUE);
}


//-------------------------------------------------------------------
// Function:  void f0_reset(void)
// Description: resets the F0.
//--------------------------------------------------------------------
void f0_reset(void)
{
    /* First drive as an output. */
    CECCtrl_RST_L_GpioInitAsOutput();
    Delay_ms(GPIO_READ_DELAY_MS);

    /* perform a low to high transition */
    CECCtrl_RST_L_SetState( LOW );
    Delay_ms(GPIO_READ_DELAY_MS);
    CECCtrl_RST_L_SetState( HIGH );

    /* wait */
    Delay_ms(100);

    /* drive the F0 reset as an input to avoid
       blocking the JTAG and local f0 reset. */
    CECCtrl_RST_L_GpioInitAsInput();
}
//-------------------------------------------------------------------
// Function:    f0_mfg_params_check_args
// Description: checks and loads the f0 mfg args into the
//              F0_MFG_PARAMS_struct.
// Parameters:  char *arg1 - mfg param string
//              char *arg2 - if a write op, this is the write value.
//              F0_MFG_PARAMS_struct *F0_mfg - ptr to storage.
//              uint8_t action               - read/write op
// Returns: SUCCESS/ERROR
//--------------------------------------------------------------------
ErrorStatus f0_mfg_params_check_args(char* arg1, char* arg2, F0_MFG_PARAMS_struct* F0_mfg, uint8_t action)
{
    /* we need a valid arg2 when doing a write operation. */
    if ( (arg1 == NULL) || ((action == F0_MFG_PARAMS_WRITE) && (arg2 == NULL)) )
    {
        TapListener_PrintString("\r\nError: f0_mfg_params_check_args() has incorrect args!\r\n");
        return (ERROR);
    }

    ErrorStatus status = SUCCESS;
    F0_mfg->action = action;
    uint8_t len;
    uint8_t ssFieldSize = (F0_MFG_PARAMS_SS_MAX_LEN - 1);

    if (!strcmp("ss", arg1 )) /* serial number param */
    {
        F0_mfg->param = F0_MFG_PARAMS_SS;
        if (action == F0_MFG_PARAMS_WRITE)
        {
            len = strlen(arg2);
            if (len != ssFieldSize)
            {
                TAP_Printf( "\r\nError: Incorrect length of %d bytes!"
                    " Should be %d bytes. Please try again.\r\n",
                    len, ssFieldSize);
                status = ERROR;
            }

            strncpy((char*)F0_mfg->p.serial_number, arg2, F0_MFG_PARAMS_SS_MAX_LEN);
            TAP_StringToUpper((char*)F0_mfg->p.serial_number);
        }
    }
    else if (!strcmp("tr", arg1 )) /* test result param */
    {
        F0_mfg->param = F0_MFG_PARAMS_TR;
        if (action == F0_MFG_PARAMS_WRITE)
        {
            if (TAP_HexStringToUInt8Array(arg2, &F0_mfg->p.test_result, 1) == FALSE)
            {
                TapListener_PrintString(mfg_hex_char_error_msg);
                status = ERROR;
            }
        }
    }
    else if (!strcmp("all", arg1 )) /* all param (read-only) */
    {
        F0_mfg->param = F0_MFG_PARAMS_ALL;
        if (action != F0_MFG_PARAMS_READ)
        {
            TapListener_PrintString("\r\nError:\"all\" option is read-only!\r\n");
            status = ERROR;
        }
    }
    else /* not supported */
    {
        TAP_Printf( "\r\n\"%s\" does not exist! See help below.\r\n", arg1 );
        print_mfg_params_help();
        status = ERROR;
    }
    return (status);
}
//-------------------------------------------------------------------
// Function:     f0_mfg_params_send
// Precondition: a call to f0_mfg_params_check_args().
// Description:  loads the F0_MFG_PARAMS_struct into an
//               i2c packet and sends to the F0. The F0 response packet
//               is detected and the results are displayed to the user.
// Parameters:   F0_MFG_PARAMS_struct *F0_mfg - ptr to storage.
// Returns: SUCCESS/ERROR
//--------------------------------------------------------------------
ErrorStatus f0_mfg_params_send(F0_MFG_PARAMS_struct* F0_mfg)
{
    if (F0_mfg == NULL)
    {
        return (ERROR);
    }

    ErrorStatus status = SUCCESS;

    /* packet size that actually gets sent to the F0. */
    uint16_t pktSize = (HDR_SIZE + F0_MFG_PARAMS_struct_size);

    const char* param = "undefined";

    if (F0_mfg->param == F0_MFG_PARAMS_SS)
    {
        param = "serial number";
    }
    else if (F0_mfg->param == F0_MFG_PARAMS_TR)
    {
        param = "test result";
    }
    else if (F0_mfg->param == F0_MFG_PARAMS_ALL)
    {
        param = "all";
    }

    /* create the proper i2c packet with our F0_MFG_PARAMS_struct info. */
    build_f0_packet(g_f0_i2c_packet, I2C_TX_BUFFERSIZE,
                    CMD_MFG_PARAMS, (uint8_t*)F0_mfg,
                    F0_MFG_PARAMS_struct_size);

    do      /* send to the F0 and wait for a response */
    {
        if (send_cmd_and_get_response(g_f0_i2c_packet, pktSize) == FALSE)
        {
            status = ERROR;
            break;
        }

        /* clear and load our original F0_MFG_PARAMS_struct with
           the F0 response data. */
        memset((void*)(F0_mfg), 0, F0_MFG_PARAMS_struct_size);
        memcpy( (void*)(F0_mfg),
                (const void*)(&g_f0_i2c_packet[ HDR_DATA ]),
                F0_MFG_PARAMS_struct_size );

        /* display what we got back. */
        if (F0_mfg->action == F0_MFG_PARAMS_PASS)
        {
            if (F0_mfg->param == F0_MFG_PARAMS_SS)
            {
                TAP_Printf( "\r\nF0 flash %s: %s\r\n", param, F0_mfg->p.serial_number);
            }
            else if (F0_mfg->param == F0_MFG_PARAMS_TR)
            {
                TAP_Printf( "\r\nF0 mfg %s:\r\n%02x", param, F0_mfg->p.test_result);
            }
            else if (F0_mfg->param == F0_MFG_PARAMS_ALL)
            {
                TAP_Printf( "\nss (%02d) %s", sizeof(F0_mfg->p.serial_number), F0_mfg->p.serial_number);
                TAP_Printf( "\r\ntr (%02d) %02x", sizeof(F0_mfg->p.test_result), F0_mfg->p.test_result);
            }
        }
        else if (F0_mfg->action == F0_MFG_PARAMS_FAIL)
        {
            TAP_Printf( "\r\n\tF0 mfg %s returned failed!\r\n", param);
            status = ERROR;
            break;
        }
        else if (F0_mfg->action == F0_MFG_PARAMS_BAD_ARGS)
        {
            TAP_Printf( "\r\n\tF0 mfg %s returned bad args!\r\n", param);
            status = ERROR;
            break;
        }
        else
        {
            TAP_Printf( "\r\n\tF0 mfg %s action is undefined!\r\n", param);
            status = ERROR;
            break;
        }
    }
    while (0);

    return (status);
}
/********************************************************************
* Function:    print_mfg_params_help
* Description: help string for mfg params
**********************************************************************/
void print_mfg_params_help(void)
{
    /* store help strings in flash, not SRAM */
    const char* mfg_help = "\r\n\tGet/Set f0 manufacturing data."
                           "\r\n\tUsage: f0 mfg,param       - gets a value"
                           "\r\n\t       f0 mfg,param,value - writes a value (hex string)"
                           "\r\n\tA commit to flash is automatically performed after writing."
                           "\r\n\tParams are:"
                           "\r\n\tall (read-only, will also show the sizes of the fields)"
                           "\r\n\tss  - sysSerial  (up to 24 hex bytes)."
                           "\r\n\ttr  - testResult (1 hex byte)."
                           "\r\n\t  ex."
                           "\r\n\t    f0 mfg,all   - display all F0 mfg params."
                           "\r\n\t    f0 mfg,ss    - display F0 mfg serial number."
                           "\r\n\t    f0 mfg,tr,21 - set f0 mfg test result param to 21.";

    TapListener_PrintString((char*)mfg_help);
}
//-------------------------------------------------------------------
// Function:     f0_mfg_params_tap
// Description:  TAP command for accessing F0 mfg params.
// Parameters:   CommandLine_t* CommandLine - user CLI strings.
// Returns: TRUE/FALSE
//--------------------------------------------------------------------
BOOL f0_mfg_params_tap(CommandLine_t* CommandLine)
{
    if (CommandLine == NULL)
    {
        TAP_Printf( "\r\nf0_mfg_params_cmd() has NULL arg!\r\n");
        return (FALSE);
    }

    BOOL result = FALSE;

    F0_MFG_PARAMS_struct F0_mfg = {0};

    switch (CommandLine->numArgs)
    {
        case 0:
        case 1: /* help */
            print_mfg_params_help();
            break;
        case 2: /* read param value from F0 */
            if (f0_mfg_params_check_args(CommandLine->args[1], NULL, &F0_mfg, F0_MFG_PARAMS_READ) == SUCCESS)
            {
                result = f0_mfg_params_send(&F0_mfg);
            }
            break;
        case 3: /* write param value to F0 */
            if (f0_mfg_params_check_args(CommandLine->args[1], CommandLine->args[2], &F0_mfg, F0_MFG_PARAMS_WRITE) == SUCCESS)
            {
                result = f0_mfg_params_send(&F0_mfg);
            }
            break;
        default:
            TAP_Printf( "\r\nToo many args!\r\n" );
            result = FALSE;
            print_mfg_params_help();
            break;
    }

    return (result);
}
//-------------------------------------------------------------------
// Function:     build_f0_packet
// Description:  generic, i2c packet builder.
// Parameters:   uint8_t *pkt - i2c packet
//               uint16_t pktSize   - i2c max packet size (used
//                                    only for clearing).
//               uint8_t cmd        - F0 command.
//               uint8_t *data      - F0 command data.
//               uint32_t dataSize  - F0 command data size (in bytes).
//
// Returns: TRUE/FALSE
//--------------------------------------------------------------------
void build_f0_packet( uint8_t* pkt,
                      uint16_t pktSize,
                      uint8_t cmd,
                      uint8_t* data,
                      uint16_t dataSize  )
{
    uint8_t checksum;

    if ((pkt == NULL) || (data == NULL))
    {
        return;
    }

    /* first clear the i2c packet buffer */
    memset((void*)(pkt), 0, pktSize);

    /* load header command and data size into the packet. */
    load_hdr_cmd_datasize(pkt, cmd, dataSize);

    /* load the data */
    load_pkt_data(pkt, data, dataSize);

    /* checksum on entire packet (except the checksum field itself) */
    checksum = calc_pkt_checksum(pkt, pktSize);

    /* add two's complement of the final checksum to the header */
    pkt[ HDR_PKT_CHECKSUM ] = TWOS_COMPL(checksum);
}

