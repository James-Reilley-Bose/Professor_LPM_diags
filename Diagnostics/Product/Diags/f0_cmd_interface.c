#include "project.h"
#include "versionlib.h"
#include "etap.h"
#include "f0_cmd_interface.h"

/* convenient size defs */
uint32_t F0_GPIO_PIN_struct_size   = sizeof(F0_GPIO_PIN_struct);
uint32_t F0_VARIANT_struct_size    = sizeof(F0_VARIANT_struct);
uint32_t F0_MFG_PARAMS_struct_size = sizeof(F0_MFG_PARAMS_struct);

/* F0 i2c ISR error table */
static F0_ISR_ERR_MASK_struct f0_isr[] =
{

    { "F0_I2C_ISR_TXIS",     F0_I2C_ISR_TXIS    },
    { "F0_I2C_ISR_ADDR",     F0_I2C_ISR_ADDR    },
    { "F0_I2C_ISR_NACKF",    F0_I2C_ISR_NACKF   },
    { "F0_I2C_ISR_STOPF",    F0_I2C_ISR_STOPF   },
    { "F0_I2C_ISR_TC",       F0_I2C_ISR_TC      },
    { "F0_I2C_ISR_TCR",      F0_I2C_ISR_TCR     },
    { "F0_I2C_ISR_BERR",     F0_I2C_ISR_BERR    },
    { "F0_I2C_ISR_ARLO",     F0_I2C_ISR_ARLO    },
    { "F0_I2C_ISR_OVR",      F0_I2C_ISR_OVR     },
    { "F0_I2C_ISR_PECERR",   F0_I2C_ISR_PECERR  },
    { "F0_I2C_ISR_TIMEOUT",  F0_I2C_ISR_TIMEOUT },
    { "F0_I2C_ISR_ALERT",    F0_I2C_ISR_ALERT   },
    { "F0_I2C_ISR_BUSY",     F0_I2C_ISR_BUSY    },
};

static uint32_t f0_isr_tbl_size = ( sizeof(f0_isr) / sizeof(f0_isr[0]) );

//----------------------------------------------------------------------
// Function:  BOOL check_for_f0_isr_errors(uint32_t isrValue)
// Description: Checks for ISR errors via a lookup table of
//              error bit masks.
// Parameters:  uint32_t isrValue - ISR register contents
// Returns: TRUE\error(s) FALSE\no errors
//----------------------------------------------------------------------
BOOL check_for_f0_isr_errors(uint32_t isrValue)
{
    BOOL status = TRUE;

    for (int i = 0; i < f0_isr_tbl_size; i++)
    {
        /* display all ISR errors */
        if ( isrValue & f0_isr[i].mask)
        {
            TAP_Printf("\r\n\t%s error occured!\r\n",
                       f0_isr[i].name);
            status = FALSE;
        }
    }

    return (status);
}
//----------------------------------------------------------------------
// Function:  uint8_t sum32(uint32_t val)
// Description: Helper function. Creates a byte checksum
//              from a unsigned long value.
// Parameters:  uint32_t val - unsigned long
// Returns: unsigned char checksum
//----------------------------------------------------------------------
uint8_t sum32(uint32_t val)
{
    return (uint8_t)((val >> 24) & 0xff)
           + (uint8_t)((val >> 16) & 0xff)
           + (uint8_t)((val >>  8) & 0xff)
           + (uint8_t)(val & 0xff);
}
//----------------------------------------------------------------------
// Function:  uint8_t sum16(uint32_t val)
// Description: Helper function. Creates a byte checksum
//              from a unsigned short value.
// Parameters:  uint16_t val - unsigned short
// Returns: unsigned char checksum
//----------------------------------------------------------------------
uint8_t sum16(uint16_t val)
{
    return (uint8_t)((val >>  8) & 0xff)
           + (uint8_t)(val & 0xff);
}
//----------------------------------------------------------------------
// Function: void load_hdr_cmd_datasize( uint8_t *buff,
//                                       uint8_t cmd,
//                                       uint16_t data_size )
// Description: Loads the HDR_CMD and HDR_DATAx fields into the i2c pkt.
// Parameters:
//              uint8_t *buff      - i2c packet
//              uint8_t cmd        - LPM TAP command
//              uint16_t data_size - total number of data bytes.
//----------------------------------------------------------------------
void load_hdr_cmd_datasize( uint8_t* buff, uint8_t cmd, uint16_t data_size )
{
    /* load the command */
    buff[ HDR_CMD ] = cmd;

    /* load the data size and add it to the checksum */
    buff[ HDR_DATA_SIZE_H ] = ( uint8_t )( ( data_size >> 8 ) & 0xff );
    buff[ HDR_DATA_SIZE_L ] = ( uint8_t )( data_size & 0xff );
}
//----------------------------------------------------------------------
// Function: void load_pkt_data
// Description: loads buffer into the data field of the packet.
//
// Parameters:
//
//----------------------------------------------------------------------
void load_pkt_data( uint8_t* buff, uint8_t* dataBuff, uint16_t dataSize)
{
    memcpy((void*)(&buff[ HDR_DATA ]), (const void*)dataBuff, dataSize);
}
//----------------------------------------------------------------------
// Function: uint32_t get_isr_status( uint8_t *buff )
// Description: gets the ISR value from the packet header.
//
// Parameters:  uint8_t *buff - i2c pkt.
// Returns: 32 bit ISR value
//----------------------------------------------------------------------
uint32_t get_isr_status( uint8_t* buff )
{
    return ( (buff[ HDR_ISR_3 ] << 24) +
             (buff[ HDR_ISR_2 ] << 16) +
             (buff[ HDR_ISR_1 ] << 8)  +
             buff[ HDR_ISR_0 ] );
}

