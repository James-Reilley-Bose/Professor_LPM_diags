/*
    File    :   CC2500_driver.c
    Title   :
    Author  :   jc45540
    Created :   02/12/2016
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   CC2500 driver Implementation for Bardeen

===============================================================================
*/
// System
#include "project.h"
#include "BufferManager.h"
#include "LinkedList.h"
#include "nv_mfg.h"
#include "nv_system.h"

// Drivers
#include "regs_chipcon.h"
#include "cc2500_platform.h"
#include "cc2500_driver.h"

// Application
#include "RFProtocolStateMachine.h"

// For logging chipcon events
SCRIBE_DECL( chipcon );
// Global variables
CC2500_Dictionary_t CC2500States[] =
{
    { .key = CC2500_STATUS_CHIP_IDLE,       .value = "idle state" },
    { .key = CC2500_STATUS_CHIP_RX_MODE,    .value = "receive mode" },
    { .key = CC2500_STATUS_CHIP_TX_MODE,    .value = "transmit mode" },
    { .key = CC2500_STATUS_CHIP_FSTXON,     .value = "frequency synthesizer on" },
    { .key = CC2500_STATUS_CHIP_CALIBRATE,  .value = "frequency synthesizer calibrating" },
    { .key = CC2500_STATUS_CHIP_SETTLING,   .value = "PLL settling" },
    { .key = CC2500_STATUS_CHIP_RX_OVER,    .value = "receive FIFO overflow" },
    { .key = CC2500_STATUS_CHIP_TX_UNDER,   .value = "transmit FIFO underflow" },
};
const uint32_t Num_CC2500States = sizeof( CC2500States ) / sizeof( CC2500_Dictionary_t );

// Local variables
const cc2500_regs_t defaultRFSettings =
{
    .spi_addr_burst = 0x40,
    .IOCFG2 = 0x06,                      // Assert GDO2 (Interrupt line) packet is transmitted/received, deassert at end of packet
    .IOCFG1 = 0x2e,                      // GDO1 not used
    .IOCFG0 = GDOx_SET_HIGH,             // GDO0 set high (0x6f)
    .FIFOTHR = 0x0f,                     // Tx FIFO threshold = 1 byte, Rx FIFO threshold = 64 bytes
    .SYNC1 = 0xd3,                       // Sync word high byte
    .SYNC0 = 0x91,                       // Sync word low byte
    .PKTLEN = 0x3e,                      // Maxmimum length packet = 62 bytes
    .PKTCTRL1 = 0x08,                    // Auto flush Rx FIFO on bad CRC
    .PKTCTRL0 = 0x45,                    // Data whitening on, CRC enabled, variable packet length mode
    .ADDR = 0x00,                        // Not used
    .CHANNR = 0x00,                      // Channel number initialized to zero
    .FSCTRL1 = 0x0a,                     // IF frequency for receiver = 254 kHz
    .FSCTRL0 = 0x00,                     // Frequency offset initialized to zero
    .FREQ2 = 0x5c,                       // Carrier frequency base = 2.400 GHz
    .FREQ1 = 0x4f,
    .FREQ0 = 0xc0,
    .MDMCFG4 = 0x2d,                     // Channel bandwidth = 542 kHz
    .MDMCFG3 = 0x3b,                     // Symbol rate = 250 kBaud
    .MDMCFG2 = 0x73,                     // Enable DC blocking before demodulator, MSK modulation, 30/32 sync word bits detection
    .MDMCFG1 = 0x23,                     // FEC disabled, 4 preamble bytes
    .MDMCFG0 = 0xa4,                     // Channel spacing = 333 kHz
    .DEVIATN = 0x00,                     // MSK deviation = 1.6 kHz
    .MCSM2 = 0x07,                       // Rx timeout set to end of packet
    .MCSM1 = 0x03,                       // Go to idle after packet has been received, go to receive after packet has been transmitted
    .MCSM0 = 0x08,                       // Auto calibrate disabled, power on wait = 149 - 155 us
    .FOCCFG = 0x1d,                      // Frequency compensation loop gain = 4K, frequency offset compensation = wide band mode (BW/2)
    .BSCFG = 0x1c,                       // Bit synchronization configuration
    .AGCCTRL2 = 0xc7,                    // AGC configuration
    .AGCCTRL1 = 0x00,
    .AGCCTRL0 = 0xb2,
    .WOREVT1 = 0x87,                     // Wake on radio configuration
    .WOREVT0 = 0x6b,
    .WORCTRL = 0xf8,
    .FREND1 = 0xb6,                      // Front-end rx/tx configuration
    .FREND0 = 0x10,
    .FSCAL3 = 0xca,                      // Frequency synthesizer calibration configuration
    .FSCAL2 = 0x0a,
    .FSCAL1 = 0x00,
    .FSCAL0 = 0x11,
    .RCCCTRL1 = 0x41,                    // RC oscillator configuration
    .RCCCTRL0 = 0x00,
};

const uint8_t cw_pktformat = 0x32;      // Asynchronous serial input from GDO0
const uint8_t cw_modformat = 0x33;      // Modulation format OOK

const uint8_t RxFIFOCommand[] =
{
    CCxxx0_RXFIFO | CHIPCON_READ_BURST,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static cc2500_regs_t currRFSettings;
static uint8_t currRFPower;
const uint8_t defaultRFPower = DEFAULT_PATABLE_POWER;
static BOOL transmitting = TRUE;
static uint8_t txcounter = 0;
static TimerHandle_t PacketTimer = NULL;

static void CC2500Config_EnableChargePumpCalibration( BOOL a_enable );
static void CC2500Config_Reset( void );

void CC2500_ResetPeripherals( void )
{
    CC2500_ConfigureGDO2AsInput();

    /* Clear any pending interrupts. After a software reset, the NVIC remains unchanged so the
     *  moment an interrupt is enabled, it will fire. These function calls are in the core_cm3.h
     *  file under STM32f205/DevPlatform/IAR. They are in-line.
     */
    NVIC_DisableIRQ( CHIPCON_MSG_INTERRUPT_CHANNEL );
    NVIC_ClearPendingIRQ( CHIPCON_MSG_INTERRUPT_CHANNEL );
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief Initialize the chipcon part to default settings.
/// This sets up the basics of the driver, gpio, spi, and some
///  initial chipcon registers
//////////////////////////////////////////////////////////////////////////////////
void CC2500_Init( void )
{
    uint8_t spitxrx[3];

    // Reset all the registers in the ram shadow of the cc2500 settings
    CC2500Config_Reset();

    // Configure MSG Interrupt Pin (formerly called GDO2)
    CC2500_ConfigureGDO2IntFallingEdge();

    // Reset CC2500
    spitxrx[0] = CCxxx0_SRES;
    ChipconSpi_Communicate_Blocking( 1, spitxrx, NULL );

    // Set configuration of CC2500
    CC2500_Command( CC2500_CONFIG, sizeof( currRFSettings ), &currRFSettings );

    // Initialize CC2500 power amplifier table
    CC2500_Command( CC2500_SET_POWERTABLE, sizeof( currRFPower ), &currRFPower );

    // Initial calibration
    // turn on charge pump calibration
    CC2500Config_EnableChargePumpCalibration( TRUE );

    spitxrx[0] = CCxxx0_FSCAL3;
    spitxrx[1] = currRFSettings.FSCAL3;
    spitxrx[2] = CCxxx0_SCAL;
    ChipconSpi_Communicate_Blocking( 3, spitxrx, NULL );

    // turn off charge pump calibration
    CC2500Config_EnableChargePumpCalibration( FALSE );
    spitxrx[0] = CCxxx0_FSCAL3;
    spitxrx[1] = currRFSettings.FSCAL3;
    ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );

    // Idle
    CC2500_Command( CC2500_NOP, 0, NULL );
    CC2500_Command( CC2500_NOP, 0, NULL );

    // Release CC2500 Tx semaphore
    xSemaphoreGive( CC2500_TxSemaphore );

    // Get packet timer handle
    PacketTimer = RFTask_GetTimerHandle( RFPROTOCOLSTATEMACHINE_MESSAGE_ID_PacketTimerDone );
}

void CC2500_Command( const CC2500_DRIVER_COMMAND cmd, size_t size, void* data )
{
    uint8_t spitxrx[4];  // a small buffer to transmit to the chipcon chip
    PacketBuffer_t* pPkt;

    switch ( cmd )
    {
        case CC2500_SET_CHANNEL:
            spitxrx[0] = CCxxx0_SIDLE;
            spitxrx[1] = CCxxx0_SFRX;
            spitxrx[2] = CCxxx0_CHANNR;
            spitxrx[3] = ( ( uint8_t* )data )[0];
            ChipconSpi_Communicate_Blocking( 4, spitxrx, NULL );
            break;

        case CC2500_CAL:
            spitxrx[0] = CCxxx0_SCAL;
            ChipconSpi_Communicate_Blocking( 1, spitxrx, spitxrx );
            do
            {
                spitxrx[0] = CCxxx0_SNOP;
                ChipconSpi_Communicate_Blocking( 1, spitxrx, spitxrx );
            }
            while ( ( ( spitxrx[0] >> 4 ) & 0x07 ) == CC2500_STATUS_CHIP_CALIBRATE );
            break;

        case CC2500_CONFIG:
            // Load configuration
            ChipconSpi_Communicate_Blocking( size, data, NULL );
            break;

        case CC2500_SET_POWERTABLE:
            // Set power table
            spitxrx[0] = CCxxx0_PATABLE;
            spitxrx[1] = ( ( uint8_t* )data )[0];
            ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );
            break;

        case CC2500_SET_TXOFFMODE:
            spitxrx[0] = CCxxx0_MCSM1;
            spitxrx[1] = ( ( uint8_t* )data )[0];
            ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );
            break;

        case CC2500_NOP:
            spitxrx[0] = CCxxx0_SNOP | CHIPCON_CHECK_STATUS_RX_FIFO;
            ChipconSpi_Communicate_Blocking( 1, spitxrx, NULL );
            break;

        case CC2500_IDLE:
            spitxrx[0] = CCxxx0_SIDLE | CHIPCON_CHECK_STATUS_RX_FIFO;
            ChipconSpi_Communicate_Blocking( 1, spitxrx, NULL );
            break;

        case CC2500_TX_PKT:
            // Check pointer not null
            pPkt = ( PacketBuffer_t* )data;

            if ( ( pPkt != NULL ) )
            {
                if ( xSemaphoreTake( CC2500_TxSemaphore, 0 ) )
                {
                    // Set transmitting flag
                    transmitting = TRUE;
                    txcounter = 1 + ( pPkt->packet.header.flags & RFPACKET_FLAGS_ACK_REQ ? 1 : 0 );

                    // Idle and Flush Tx FIFO
                    spitxrx[0] = CCxxx0_SIDLE;
                    spitxrx[1] = CCxxx0_SFTX;
                    ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );

                    // Fill Tx FIFO
                    pPkt->packet.llheader = CCxxx0_TXFIFO | CHIPCON_WRITE_BURST;
                    ChipconSpi_Communicate_Blocking( pPkt->packet.header.length + 2, ( const uint8_t* )&pPkt->packet, NULL );

                    // Transmit
                    spitxrx[0] = currRFPower > 0 ? CCxxx0_STX : CCxxx0_SRX;
                    ChipconSpi_Communicate_Blocking( 1, spitxrx, NULL );
                }
            }
            break;

        case CC2500_RX_PKT:
            // Flush Rx FIFO and put into Rx mode
            spitxrx[0] = CCxxx0_SFRX;
            spitxrx[1] = CCxxx0_SRX;
            ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );
            break;

        default:
            break;
    }
}

uint32_t CC2500_Ping( void )
{
    uint8_t spitxrx[2];
    uint8_t partNumber, version;
    uint32_t status;

    // Init locals
    partNumber = 0;
    version = 0;
    status = 0;

    // Get part number
    spitxrx[0] = CHIPCON_READ_STATE_3X | CCxxx0_PARTNUM;
    spitxrx[1] = 0x00;
    if ( ChipconSpi_Communicate_Blocking( 2, spitxrx, spitxrx ) == 2 )
    {
        status = spitxrx[0];
        partNumber = spitxrx[1];

    }

    // Get version
    spitxrx[0] = CHIPCON_READ_STATE_3X | CCxxx0_VERSION;
    if ( ChipconSpi_Communicate_Blocking( 2, spitxrx, spitxrx ) == 2 )
    {
        version = spitxrx[1];
    }

    // Fill part number, version, status
    status |= ( ( version << 16 ) | (partNumber << 8 ) );

    return status;
}

//////////////////////////////////////////////////////////////////////////////////
/// @brief ISR to process the GDO line goin low
//////////////////////////////////////////////////////////////////////////////////
void CC2500_HandleGDO2Interrupt()
{
    uint8_t spitxrx[2];
    uint8_t size;
    ChannelState_t channel = GetChannelState();
    int16_t sample;

    // Kick the watchdog
    RFTask_KickWatchdog();

    // Packet received?
    if ( !transmitting )
    {
        // Retrieve number of bytes received from Rx FIFO
        spitxrx[0] = CCxxx0_RXBYTES | CHIPCON_READ_BURST;
        spitxrx[1] = 0;
        ChipconSpi_Communicate_Blocking( 2, spitxrx, spitxrx );
        size = spitxrx[1];

        // Is it a valid sized packet?
        if ( size > 0 && size <= MAX_PACKET_SIZE )
        {
            // Create buffer for packet to be received
            PacketBuffer_t* pPkt = BufferManagerGetBuffer( sizeof( PacketBuffer_t ) + size );
            if ( pPkt != NULL )
            {
                pPkt->size = sizeof( PacketBuffer_t ) + size;
                pPkt->timestamp = xTaskGetTickCount();
                pPkt->doneCallback = NULL;

                // Retrieve packet from the Rx FIFO
                ChipconSpi_Communicate_Blocking( size + 1, RxFIFOCommand, ( uint8_t* )&pPkt->packet );

                // Is this packet for me?
                if ( ( pPkt->packet.header.flags & RFPACKET_FLAGS_BROADCAST ) == 0 && pPkt->packet.header.address == NV_GetChipconAddress() )
                {
                    // Was this an ACK to a previously transmitted packet?
                    if ( ( pPkt->packet.header.flags & RFPACKET_FLAGS_TYPE_MASK ) == RFPACKET_FLAGS_ACK )
                    {
                        ReceiveAck( pPkt->packet.ack.RXedSequenceNumber );
                        BufferManagerFreeBuffer( pPkt );
                    }
                    // Post packet to upper layer - upper layer will free the buffer eventually
                    else
                    {
                        RFPostMsg( RFPROTOCOLSTATEMACHINE_MESSAGE_ID_Rx, ( uint32_t )pPkt, channel );
                    }

                    // Decrement semaphore counter
                    if ( txcounter > 0 )
                    {
                        txcounter--;
                    }

                    // Statistics
                    spitxrx[0] = CCxxx0_RSSI | CHIPCON_READ_BURST;
                    spitxrx[1] = 0;
                    ChipconSpi_Communicate_Blocking( 2, spitxrx, spitxrx );
                    sample = spitxrx[1] < 128 ? ( ( int16_t )spitxrx[1] >> 1 ) - RSSI_OFFSET : ( ( ( int16_t )spitxrx[1] - 256 ) >> 1 ) - RSSI_OFFSET;
                    RFProtocolStateMachine_GetStatistics()->RSSI = ( 7 * RFProtocolStateMachine_GetStatistics()->RSSI + sample ) >> 3;
                    RFProtocolStateMachine_GetStatistics()->lastpacketreceivedtime = xTaskGetTickCount();
                    RFTask_SetLinkStatus( TRUE );
                }
                // Not mine, release packet buffer
                else
                {
                    BufferManagerFreeBuffer( pPkt );
                }
            }
        }
        else if (size > MAX_PACKET_SIZE )
        {
            RFPostMsg( RFPROTOCOLSTATEMACHINE_MESSAGE_ID_Init, 0, 0 );

            //NEED to do this so that in OOB the LPM is kept in pairing state
            if (GetPairingState() == PAIRING_ENABLED)
            {
                RFPostMsg( RFPROTOCOLSTATEMACHINE_MESSAGE_ID_EnablePairing, 0, 0 );
            }
        }

        // Restart Rx mode
        spitxrx[0] = CCxxx0_SFRX;
        spitxrx[1] = CCxxx0_SRX;
        ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );
    }
    else
    {
        // Start retransmission timer
        xTimerStart( PacketTimer, 0 );

        // Decrement semaphore counter
        if ( txcounter > 0 )
        {
            txcounter--;
        }
    }

    transmitting = FALSE;
    if ( txcounter == 0 )
    {
        // Release transmit semaphore
        xSemaphoreGive( CC2500_TxSemaphore );

        // Stop packet timer (if it was running)
        xTimerStop( PacketTimer, 0 );
    }
}

void CC2500_ClearTxSemaphore( void )
{
    txcounter = 0;
    xSemaphoreGive( CC2500_TxSemaphore );
}

void CC2500Config_SetTxOffMode( CC2500CONFIG_TXOFF mode )
{
    currRFSettings.MCSM1 &= ~CC2500CONFIG_TXOFF_MASK;
    currRFSettings.MCSM1 |= mode;
    CC2500_Command( CC2500_SET_TXOFFMODE, 1, &currRFSettings.MCSM1 );
}

uint8_t CC2500_GetRFPowerSetting( void )
{
    return currRFPower;
}

void CC2500_SetRFPowerSetting( uint8_t RFPower )
{
    currRFPower = RFPower;
    CC2500_Command( CC2500_SET_POWERTABLE, sizeof( currRFPower ), &currRFPower );
}

void CC2500_CW( BOOL flag )
{
    uint8_t spitxrx[2];

    if ( flag )
    {
        // Set input to GDO0 (always high)
        spitxrx[0] = CCxxx0_PKTCTRL0;
        spitxrx[1] = cw_pktformat;
        ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );

        // Set modulation format to OOK
        spitxrx[0] = CCxxx0_MDMCFG2;
        spitxrx[1] = cw_modformat;
        ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );

        // Start transmission
        spitxrx[0] = currRFPower > 0 ? CCxxx0_STX : CCxxx0_SRX;
        ChipconSpi_Communicate_Blocking( 1, spitxrx, NULL );
    }
    else
    {
        // Stop transmission
        CC2500_Command( CC2500_IDLE, 0, NULL );

        // Set modulation format to default
        spitxrx[0] = CCxxx0_MDMCFG2;
        spitxrx[1] = defaultRFSettings.MDMCFG2;
        ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );

        // Set input to default
        spitxrx[0] = CCxxx0_PKTCTRL0;
        spitxrx[1] = defaultRFSettings.PKTCTRL0;
        ChipconSpi_Communicate_Blocking( 2, spitxrx, NULL );
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief next time we calibrate, do we want to calibrate the charge pump?
///
/// @param a_enable should charge pump calibration be enabled
////////////////////////////////////////////////////////////////////////////////
static void CC2500Config_EnableChargePumpCalibration( BOOL a_enable )
{
    if ( a_enable )
    {
        currRFSettings.FSCAL3 |= 0x20;
    }
    else
    {
        currRFSettings.FSCAL3 &= ~0x20;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief reset all the registers in the ram shadow of the cc2500 settings
////////////////////////////////////////////////////////////////////////////////
static void CC2500Config_Reset( void )
{
    memcpy(&currRFSettings, &defaultRFSettings, sizeof( currRFSettings ) );
    currRFPower = defaultRFPower;
}


