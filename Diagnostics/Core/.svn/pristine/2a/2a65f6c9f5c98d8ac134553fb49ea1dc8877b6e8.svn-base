/*
    File    :   CC2500_driver.h
    Title   :
    Author  :   jc45540
    Created :   02/11/2015
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:   CC2500 driver header for Bardeen

===============================================================================
*/

#ifndef _CC2500_DRIVER_H_
#define _CC2500_DRIVER_H_

#include "LinkedList.h"

#define CHIPCON_READ_STATE_3X   0xC0
#define CHIPCON_PART_NUMBER     0x80
#define CHIPCON_CHIP_VERSION    0x03

// Status Bits as from CC2500 datasheet
#define CC2500_STATUS_CHIP_READYn       0x80
#define CC2500_STATUS_CHIP_IDLE         0
#define CC2500_STATUS_CHIP_RX_MODE      1
#define CC2500_STATUS_CHIP_TX_MODE      2
#define CC2500_STATUS_CHIP_FSTXON       3
#define CC2500_STATUS_CHIP_CALIBRATE    4
#define CC2500_STATUS_CHIP_SETTLING     5
#define CC2500_STATUS_CHIP_RX_OVER      6
#define CC2500_STATUS_CHIP_TX_UNDER     7

/********** Data Types **********/
typedef enum
{
    CC2500_SET_CHANNEL,
    CC2500_TX_PKT,
    CC2500_RX_PKT,
    CC2500_CAL,
    CC2500_SLEEP,
    CC2500_CONFIG,
    CC2500_SET_POWERTABLE,
    CC2500_MEASURE_RSSI,
    CC2500_WAKE,
    CC2500_SET_GDOZERO,
    CC2500_NOP,
    CC2500_IDLE,
    CC2500_SET_TXOFFMODE,
    NUM_CC2500_DRIVER_COMMANDS
} CC2500_DRIVER_COMMAND;

typedef enum
{
    CC2500_CONFIG_RESET_ALL,
    CC2500_CONFIG_PREAMBLE_LENGTH,
    CC2500_CONFIG_SYNC_WORD,
    CC2500_CONFIG_DATA_WHITENING,
    CC2500_CONFIG_BASE_FREQUENCY,
    CC2500_CONFIG_CHANNEL_SPACING,
    CC2500_CONFIG_OUTPUT_POWER,
    CC2500_CONFIG_SYNC_WORD_MODE,
    CC2500_CONFIG_MODULATION_FORMAT,
    CC2500_CONFIG_MANCHESTER_ENCODING,
    CC2500_CONFIG_DATA_RATE,
    CC2500_CONFIG_CHANNEL_BANDWIDTH,
    CC2500_CONFIG_SET_CHANNEL,
    CC2500_NUM_CONFIG_OPTIONS
} CC2500_CONFIG_OPTION;

// allowable preable lengths
typedef enum
{
    CC2500_2_BYTE_PREAMBLE = 0x0,
    CC2500_3_BYTE_PREAMBLE = 0x1,
    CC2500_4_BYTE_PREAMBLE = 0x2,
    CC2500_6_BYTE_PREAMBLE = 0x3,
    CC2500_8_BYTE_PREAMBLE = 0x4,
    CC2500_12_BYTE_PREAMBLE = 0x5,
    CC2500_16_BYTE_PREAMBLE = 0x6,
    CC2500_24_BYTE_PREAMBLE = 0x7,
    CC2500_PREAMBLE_LENGTH_MASK = 0x70,
    CC2500_PREAMBLE_LENGTH_SHIFT = 4
} CC2500_PREAMBLE_LENGTH;


// sync word modes
typedef enum
{
    CC2500_DISABLE_PREAMBLE_AND_SYNC,
    CC2500_15_OF_16_SYNC_BITS,
    CC2500_16_OF_16_SYNC_BITS,
    CC2500_30_OF_32_SYNC_BITS,
    CC2500_NO_PREAMBLE_OR_SYNC_JUST_CARRIER_SENSE,
    CC2500_15_OF_16_SYNC_BITS_AND_CARRIER_SENSE,
    CC2500_16_OF_16_SYNC_BITS_AND_CARRIER_SENSE,
    CC2500_30_OF_32_SYNC_BITS_AND_CARRIER_SENSE,
    CC2500_SYNC_WORD_QUALIFIER_MASK = 0x07
} CC2500_SYNC_WORD_QUALIFIER_MODE;

// data whitening
#define CC2500_DATA_WHITENING_ENBALE_MASK 0x40

// channel spacing
#define CC2500_CHANNEL_SPACING_EXPONNET_MASK 0x03

// modulation formats
typedef enum
{
    CC2500_2_FSK_MODULATION = 0x00,
    CC2500_GFSK_MODULATION = 0x01,
    CC2500_OOK_MODULATION = 0x03,
    CC2500_MSK_MODULATION = 0x70,
    CC2500_MODULATION_SHIFT = 4
} CC2500_MODULATION_FORMATS;

// Manchester Encoding
#define CC2500_MANCHESTER_ENCODING_MASK 0x08

// Data Rate
#define CC2500_DATA_RATE_EXPONENT_MASK 0x0f

// Channel Bandwidth
#define CC2500_CHANNEL_BANDWIDTH_MASK 0xf0
#define CC2500_CHANNEL_BANDWIDTH_EXPONENT_MASK 0x03
#define CC2500_CHANNEL_BANDWIDTH_MANTISSA_MASK 0x03
#define CC2500_CHANNEL_BANDWIDTH_EXPONENT_SHIFT 6
#define CC2500_CHANNEL_BANDWIDTH_MANTISSA_SHIFT 4

// packet formats
typedef enum
{
    CC2500_PKT_FORMAT_NORMAL = 0x00,
    CC2500_PKT_FORMAT_SYNCHRONOUS_SERIAL = 0x10,
    CC2500_PKT_FORMAT_RANDOM_TX = 0x20,
    CC2500_PKT_FORMAT_ASYNCHRONOUS_SERIAL = 0x30,
    CC2500_PKT_FORMAT_MASK = 0x30
} CC2500_PKT_FORMAT;

// packet length configuration
typedef enum
{
    CC2500_PKT_LENGTH_CONFIG_FIXED = 0x00,
    CC2500_PKT_LENGTH_CONFIG_VARIABLE = 0x01,
    CC2500_PKT_LENGTH_CONFIG_INFINITE = 0x02,
    CC2500_PKT_LENGTH_CONFIG_NORMAL = CC2500_PKT_LENGTH_CONFIG_VARIABLE,
    CC2500_PKT_LENGTH_CONFIG_MASK = 0x03
} CC2500_PKT_LENGTH_CONFIG;

// Data whitening
typedef enum
{
    CC2500_PKT_DATA_WHITENING_ON = 0x40,
    CC2500_PKT_DATA_WHITENING_OFF = 0x00,
    CC2500_PKT_DATA_WHITENING_NORMAL = CC2500_PKT_DATA_WHITENING_ON,
    CC2500_PKT_DATA_WHITENING_MASK = 0x40
} CC2500_PKT_DATA_WHITENING;

// Tx offmode
typedef enum
{
    CC2500CONFIG_TXOFF_IDLE = 0x00,
    CC2500CONFIG_TXOFF_FSTXON = 0x01,
    CC2500CONFIG_TXOFF_TX = 0x02,
    CC2500CONFIG_TXOFF_RX = 0x03,
    CC2500CONFIG_TXOFF_MASK = 0x03
} CC2500CONFIG_TXOFF;

// pass a pointer to this as the argument to the set channel command
typedef struct
{
    const uint8_t* TxBuffer; // initial buffer to send on channel(optional)
    uint8_t Channel;
} CC2500_SetChannelArg_t;

// pass a pointer to the appropriate one of the following
//   as the argument to the config command
typedef struct
{
    CC2500_CONFIG_OPTION ConfigOption;
} CC2500_Configuration_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    CC2500_PREAMBLE_LENGTH Length;
} CC2500_SetPreambleLength_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    uint16_t SyncWord;
} CC2500_SetSyncWord_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    BOOL Enabled;
} CC2500_SetDataWhitening_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    uint32_t FrequencyIncrement; // see page 66 of chipcon spec(FREQ2)
} CC2500_SetBaseFrequency_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    uint8_t Mantissa;
    uint8_t Exponent;
} CC2500_SetChannelSpacing_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    uint8_t PowerSetting;
} CC2500_SetOutputPower_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    CC2500_SYNC_WORD_QUALIFIER_MODE SyncWordMode;
} CC2500_SetSyncWordMode_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    CC2500_MODULATION_FORMATS ModulationFormat;
} CC2500_SetModulationFormat_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    BOOL Enabled;
} CC2500_SetManchesterEncoding_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    uint8_t Mantissa;
    uint8_t Exponent;
} CC2500_SetDataRate_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    uint8_t Mantissa;
    uint8_t Exponent;
} CC2500_SetChannelBandwidth_Parameter_t;

typedef struct
{
    CC2500_Configuration_Parameter_t Base;
    uint8_t Channel;
} CC2500_SetChannel_Parameter_t;

// This is used as the argument for the measure RSSI Command
typedef struct
{
    // the function to call from ISR when complete
    void (*NewRSSICallBack)(uint8_t a_channel, uint8_t a_rssiValue);
    uint8_t* Value; // a pointer to where to store the measured value
} CC2500_MeasureRSSI_Parameter_t;

typedef struct
{
    uint8_t key;
    const char* value;
} CC2500_Dictionary_t;

// CC2500 Registers
#define GDOx_SET_LOW  0x2F
#define GDOx_SET_HIGH 0x6F
/// transmit power
#define DEFAULT_PATABLE_POWER 0xff

#pragma pack(1)
// a data structure which contains all relevant CCxxx0 registers
typedef struct
{
    uint8_t spi_addr_burst;      // first byte of message specifies the register to write to
    uint8_t IOCFG2;              // GDO2 output pin configuration
    uint8_t IOCFG1;              // GDO1 output pin configuration
    uint8_t IOCFG0;              // GDO0 output pin configuration
    uint8_t FIFOTHR;             // fifo thresholds
    uint8_t SYNC1;               // sync high byte
    uint8_t SYNC0;               // sync low byte
    uint8_t PKTLEN;              // Packet length.
    uint8_t PKTCTRL1;            // Packet automation control.
    uint8_t PKTCTRL0;            // Packet automation control.
    uint8_t ADDR;                // Device address.
    uint8_t CHANNR;              // Channel number.
    uint8_t FSCTRL1;             // Frequency synthesizer control.
    uint8_t FSCTRL0;             // Frequency synthesizer control.
    uint8_t FREQ2;               // Frequency control word, high byte.
    uint8_t FREQ1;               // Frequency control word, middle byte.
    uint8_t FREQ0;               // Frequency control word, low byte.
    uint8_t MDMCFG4;             // Modem configuration.
    uint8_t MDMCFG3;             // Modem configuration.
    uint8_t MDMCFG2;             // Modem configuration.
    uint8_t MDMCFG1;             // Modem configuration.
    uint8_t MDMCFG0;             // Modem configuration.
    uint8_t DEVIATN;             // Modem deviation setting (when FSK modulation is enabled).
    uint8_t MCSM2;               // Main Radio Control State Machine configuration.
    uint8_t MCSM1;               // Main Radio Control State Machine configuration.
    uint8_t MCSM0;               // Main Radio Control State Machine configuration.
    uint8_t FOCCFG;              // Frequency Offset Compensation Configuration.
    uint8_t BSCFG;               // Bit synchronization Configuration.
    uint8_t AGCCTRL2;            // AGC control.
    uint8_t AGCCTRL1;            // AGC control.
    uint8_t AGCCTRL0;            // AGC control.
    uint8_t WOREVT1;             // high byte event0 timeout
    uint8_t WOREVT0;             // low byte event0 timeout
    uint8_t WORCTRL;             // wake on radio control
    uint8_t FREND1;              // Front end RX configuration.
    uint8_t FREND0;              // Front end RX configuration.
    uint8_t FSCAL3;              // Frequency synthesizer calibration.
    uint8_t FSCAL2;              // Frequency synthesizer calibration.
    uint8_t FSCAL1;              // Frequency synthesizer calibration.
    uint8_t FSCAL0;              // Frequency synthesizer calibration.
    uint8_t RCCCTRL1;            // rc oscillator control.
    uint8_t RCCCTRL0;            // rc oscillator control.
} cc2500_regs_t;
#pragma pack()

// Public API
extern CC2500_Dictionary_t CC2500States[];
extern const uint32_t Num_CC2500States;

void CC2500_ResetPeripherals( void );
void CC2500_Command( const CC2500_DRIVER_COMMAND cmd, size_t size, void* data );
void CC2500_Init( void );
void CC2500_ClearTxSemaphore( void );
uint32_t CC2500_Ping( void );
void CC2500_HandleGDO2Interrupt( void );
uint8_t CC2500_GetRFPowerSetting( void );
void CC2500_SetRFPowerSetting( uint8_t RFPower );
void CC2500_CW( BOOL flag );
void CC2500Config_SetTxOffMode( CC2500CONFIG_TXOFF mode );

#endif // _CC2500_DRIVER_H_
