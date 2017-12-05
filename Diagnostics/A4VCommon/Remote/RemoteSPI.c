//
// RemoteSPI.c
//
#include "SpiBus_API.h"
#include "RemoteSPI.h"
#include "RemoteUtility.h"
#include "RemoteTask.h"
#include "RemoteRx.h"
#include "RemoteTx.h"
#include "RemoteDefs.h"

#include "etap.h" //temporary, remove when TAP_Printf is not needed

#define REMOTE_SPI_WAIT_MS 100


static void InitIRQ(void);

SCRIBE_DECL(remote);
SPI_BUS_HANDLE_TYPE RemoteSpiHandle = NULL;
Remote_SPIPacket_t pkt;
//SPI_Bus_Config_t runningSpiConfig;
//SPI_Bus_Config_t updatingSpiConfig;

// does NOT call SPIBus_Initialize
void Remote_SPI_Init(void)
{
    /*
    //REMOTE_LOG("starting RemoteSPI");
    Remote_SPI_EnableEXTI();
    SPI_API_ERROR_CODE SpiError = SPI_API_NO_ERROR;
    SPI_Bus_Config_t SpiConfig = {0};

    // Beginning of SPI Bus driver info
    memcpy( &SpiConfig,
            &spiDefaultConfig[REMOTE_SPI_BUS_NUM - 1],
            sizeof(SpiConfig) );

    SpiConfig.busNumber = REMOTE_SPI_BUS_NUM;
    SpiConfig.clockPolarity = 1;
    // use default SPI bus clocking.
    SpiConfig.baudRateExponent = 3;

    // gpio bank must be NULL when the Chip Select pin is under user control.
    // This tells the SPI bus driver that we will be driving chip select pin.
    SpiConfig.csGpioBank        = REMOTE_CHIP_SELECT_BANK;
    SpiConfig.spiPinCS          = REMOTE_CHIP_SELECT_PIN;
    SpiConfig.chipSelectControl = CHIP_SELECT_CONTROL_API_DRIVER;

    RemoteSpiHandle = SPIBus_Initialize(&SpiConfig, &SpiError);
    */
    
    
    
    /*
    // Set up SPI configs
    memcpy( &runningSpiConfig,
            &spiDefaultConfig[REMOTE_SPI_BUS_NUM - 1],
            sizeof(SPI_Bus_Config_t) );
    runningSpiConfig.csGpioBank = REMOTE_CHIP_SELECT_BANK;
    runningSpiConfig.spiPinCS = REMOTE_CHIP_SELECT_PIN;
    runningSpiConfig.chipSelectControl = CHIP_SELECT_CONTROL_API_DRIVER;
    runningSpiConfig.busNumber = REMOTE_SPI_BUS_NUM;
    runningSpiConfig.clockPolarity = 1;
    runningSpiConfig.baudRateExponent = 3;
    
    // spi config for Updating state uses mostly same values as running, so copy it
    memcpy( &updatingSpiConfig,
            &runningSpiConfig,
            sizeof(SPI_Bus_Config_t) );
    runningSpiConfig.clockPhase = 1;
    */
    
    SPI_Bus_Config_t spiConfig = {0};
    memcpy( &spiConfig,
            &spiDefaultConfig[REMOTE_SPI_BUS_NUM - 1],
            sizeof(SPI_Bus_Config_t) );
    spiConfig.csGpioBank = REMOTE_CHIP_SELECT_BANK;
    spiConfig.spiPinCS = REMOTE_CHIP_SELECT_PIN;
    spiConfig.chipSelectControl = CHIP_SELECT_CONTROL_API_DRIVER;
    spiConfig.busNumber = REMOTE_SPI_BUS_NUM;
    spiConfig.clockPolarity = 1;
    spiConfig.baudRateExponent = 3;
    spiConfig.clockPhase = 1;
    
    SPI_API_ERROR_CODE SpiError = SPI_API_NO_ERROR;
    RemoteSpiHandle = SPIBus_Initialize(&spiConfig, &SpiError);
    // debug_assert(SpiError == SPI_API_NO_ERROR);
    InitIRQ();
}


BOOL Remote_SPI_Write(const void* data, unsigned int length)
{
    SPI_API_ERROR_CODE SpiError = SPI_API_NO_ERROR;

    unsigned int spi_bytes_ret = SPIBus_Communicate( RemoteSpiHandle,
                                                     length,          /* page write len */
                                                     (uint8_t*) data, /* write buffer */
                                                     NULL,           /* no read */
                                                     NULL,           /* blocking */
                                                     REMOTE_SPI_WAIT_MS, /* blocking timeout*/
                                                     &SpiError );
    //REMOTE_LOG("Wrote SPI data: %d bytes", spi_bytes_ret);
    return ((SpiError == SPI_API_NO_ERROR) && (spi_bytes_ret == length));
}


BOOL Remote_SPI_Read(void* data, unsigned int length)
{
    SPI_API_ERROR_CODE SpiError = SPI_API_NO_ERROR;

    unsigned int spi_bytes_ret = SPIBus_Communicate( RemoteSpiHandle,
                                                     length,          /* page write len */
                                                     NULL,           /* no write */
                                                     data,           /* read */
                                                     NULL,           /* blocking */
                                                     REMOTE_SPI_WAIT_MS, /* blocking timeout*/
                                                     &SpiError );
    //REMOTE_LOG("SPI_Read: Read %d bytes", spi_bytes_ret);
    return ((SpiError == SPI_API_NO_ERROR) && (spi_bytes_ret == length)); 
}

extern void diag_rf_remote(uint8_t Remote_numPacketsReceived, Remote_SPIPacket_t *pkt);

// Reads data from SPI until error or there is no data left. Packets are
//  immedietly parsed
void Remote_SPI_ReadPackets(void)
{
    // Make sure packet is zeroed
    Remote_ZeroSPIPacket(&pkt);
    BOOL validRead = Remote_SPI_Read(&pkt, sizeof(Remote_SPIPacket_t));
    if(!validRead)
    {
        LOG(remote, ROTTEN_LOGLEVEL_NORMAL, "SPI read failed");
        return;
    }
    // The first read should be all zeros, as that is what CC2640
    //  fills register with. With those read we can get the good stuff
    if(AllZero(&pkt, sizeof(Remote_SPIPacket_t)))
    {
        // the good stuff
        validRead = Remote_SPI_Read(&pkt, sizeof(Remote_SPIPacket_t));
    }
    do // Loop until zero packet or SPI error
    {
        if(!validRead)
        {
            LOG(remote, ROTTEN_LOGLEVEL_NORMAL, "SPI read failed");
            break;
        }
        if(AllZero(&pkt, sizeof(Remote_SPIPacket_t)))
        {
            break;
        }
        // check xor
        uint32_t xor = Remote_SPI_ComputeXOR(&pkt);
        if(xor == pkt.xor)
        {
            // parse packet
            Remote_Rx_ParsePacket(&pkt);
            Remote_numPacketsReceived += 1;
            diag_rf_remote(Remote_numPacketsReceived, &pkt);
        }
        else
        {
            LOG(remote, ROTTEN_LOGLEVEL_NORMAL, "SPI: Invalid packet XOR");
        }
        Remote_ZeroSPIPacket(&pkt);
        BOOL validRead = Remote_SPI_Read(&pkt, sizeof(Remote_SPIPacket_t));
    }  while(TRUE);
}


// Compute the XOR over the message packet
// requires <pkt> to be aligned
uint32_t Remote_SPI_ComputeXOR(Remote_SPIPacket_t *pkt)
{
    uint32_t *pWd = (uint32_t *)pkt;
    uint8_t wdCnt = sizeof(Remote_SPIPacket_t) / sizeof(uint32_t) - 1; /* One less for xor word */
    uint8_t i;

    // start with the 1st word (opcode, etc.)
    uint32_t xorVal = *pWd++;
    
    // XOR over the data payload
    for (i = 1; (i < wdCnt); i++) {
        xorVal ^= *pWd++;
    }
    return xorVal;
}


static void InitIRQ(void){
    // Select the input source pin for the EXTI line using SYSCFG_EXTILineConfig()
    SYSCFG_EXTILineConfig(REMOTE_INTERRUPT_PORT_SOURCE, REMOTE_INTERRUPT_LINE_NUM);

    // Configure NVIC IRQ channel mapped to the EXTI line using NVIC_Init()
    Interrupt_RegisterISR(REMOTE_INTERRUPT_CHANNEL, REMOTE_INTERRUPT_HANDLER);
    
    Interrupt_Enable(REMOTE_INTERRUPT_CHANNEL, configLIBRARY_KERNEL_INTERRUPT_PRIORITY, 0);
}

void Remote_SPI_EnableIRQ(BOOL enable)
{
    EXTI_InitTypeDef EXTI_InitStructure = {
        .EXTI_Mode = EXTI_Mode_Interrupt,
        .EXTI_Line = REMOTE_INTERRUPT_PIN_MASK,
        .EXTI_Trigger = EXTI_Trigger_Falling,
    };
    EXTI_InitStructure.EXTI_LineCmd = enable ? ENABLE : DISABLE;
    EXTI_Init(&EXTI_InitStructure);
}
