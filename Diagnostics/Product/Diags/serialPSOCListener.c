#include "serialPSOCListener.h"
#include "DiagTask.h"
#include "product_config.h"

SCRIBE_DECL(tap_events);
SCRIBE_DECL(system_events);
SCRIBE_DECL(diag);

extern I2C_BUS_HANDLE_TYPE PSoCTask_ConfigureI2C(void);

void Listener_PsocPrintChar(const char c);
void Listener_PsocPrintString(const char *s);
ManagedTask* ManagedPsocParserTask = NULL;
#define PSOC_MUTEX_WAIT TIMER_MSEC_TO_TICKS(100)

I2C_BUS_HANDLE_TYPE psocI2cHandle = NULL;

uint8_t psoc_cmd[PSOC_CMD_BUFFER_SZ];
uint8_t psoc_rcv[PSOC_READ_SZ];


void psoc_read_result(void)
{
  psoc_i2c_init();
  get_psoc_listener_buffer(psoc_rcv);
  
  LOG(diag, ROTTEN_LOGLEVEL_VERBOSE, "psoc resp = %02x %02x %02x %02x %02x %02x  cnt[%d]\n\r",
             psoc_rcv[0],psoc_rcv[1],psoc_rcv[2],psoc_rcv[3],psoc_rcv[4],psoc_rcv[5],gbl_psoc_int_det);

  if(psoc_cmd[1] == 0){     //GetVersion_cmd
    if(psoc_rcv[0] == 1){
      TAP_Printf("\nPSoC SW Version (major.minor): %d.%d \n",psoc_rcv[1], psoc_rcv[2]);
    }
  }
}

//Global buffer to hold returned string
void get_psoc_listener_buffer(uint8_t *rcvBuff)
{   
  if(!i2cMaster_ReadNoRegister (psocI2cHandle,
                                    PSOC_I2C3_ADDR, 
                                    rcvBuff,
                                    PSOC_READ_SZ))
  {
    LOG(diag,ROTTEN_LOGLEVEL_NORMAL,"%s read error!", __FUNCTION__);
  }
}

int psoc_i2c_init(void){
  if(psocI2cHandle == NULL)
  {
    psocI2cHandle = PSoCTask_ConfigureI2C();
  }
  return (psocI2cHandle == NULL)?ERROR:SUCCESS;
}

/*******************************************************************************
Function:	Listener_PsocPrintString()
Returns:	N/A
Description: Outputs a string to PSOC (no mutex) 
Globals:
Locals:
Calls:
Called by:any routine that wishes to print a string to the serial port.
*******************************************************************************/
void Listener_Psoc_PutString(uint8_t *s)
{
 debug_assert(s); 
 debug_assert(psocI2cHandle); 

 if(!i2cMaster_WriteNoRegister(psocI2cHandle, PSOC_I2C3_ADDR, &(s[1]), s[0])){
    LOG(diag,ROTTEN_LOGLEVEL_NORMAL,"%s failed!",__FUNCTION__);
    return;
 }
}

// Bootloader stuff
int psoc_open_connection(void)
{
    return 0;
}

int psoc_close_connection(void)
{
    return 0;
}

int psoc_write(unsigned char* wrData, int byteCnt)
{
    uint16_t ret = 0;

    if (!(ret = i2cMaster_WriteNoRegister(psocI2cHandle, PSOC_I2C3_ADDR, (const uint8_t*)wrData, byteCnt)))
    {
        LOG(tap_events, ROTTEN_LOGLEVEL_NORMAL, "psoc_write failed! %x", ret);
        return 1;
    }

    Delay_ms(25);

    LOG(tap_events, ROTTEN_LOGLEVEL_NORMAL, "psoc_write len=%d", ret);

    return 0;
}

int psoc_read(unsigned char* rdData, int byteCnt)
{
//    uint16_t lenrd = 0;

//    lenrd = 
      i2cMaster_ReadNoRegister (psocI2cHandle,
                                      PSOC_I2C3_ADDR,
                                      (uint8_t*)rdData,
                                      byteCnt);

    //Delay_ms(25);

//    LOG(diag, ROTTEN_LOGLEVEL_NORMAL, "psoc_read %d bytes", lenrd);
    return 0;
}
