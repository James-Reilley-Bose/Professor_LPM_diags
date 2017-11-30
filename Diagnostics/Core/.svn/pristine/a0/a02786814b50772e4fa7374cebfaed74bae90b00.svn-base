#ifndef _ETAPIIC_H_
#define _ETAPIIC_H_

#include "etapcoms.h"
#include "i2cMaster_API.h"

/**
* @DOCETAP
* COMMAND:   i2c
*
* ALIAS:     ic
*
* DESCRIP:   Send a write or read or test (for both read/write) request based on
*               bus, address, register, register_length, data_length, loopCount or data.
*            Also measure and output ReadTime and WriteTime.
*
* PARAM:    action,     bus,     address,    register,   register_length,  data_length,   loopcount or [data,]
*
*           action is either  'p' (ping), 'r' (read), 'w' (write) or 't' (test, i.e., both read & write).
*           bus is the bus number of I2C, decimal 1, 2, or 3;
*           address is the slave address on the bus, hex format, the MSB 7-bit is the 7-bit slave address,
*           and LSB bit 0 is set to 0. For example, slave address is 0x67, the input value for "address"
*           byte is 0xCE (obtained from left-shifting 1 of 0x67);
*           register address, hex format, up to 4 bytes, 0xFF means no register;
*           register_length, length of register, range 0 - 4
*           data_length is the length of data to transfer, decimal format, max value 256
*           loopCount is needed in 't' action;
*           data are needed in 'w' action, data format is hex
*
* REPLY:    When succeed, TAP_OK, data read will be output to logger in 'r' action;
*               Also, average time for read or write in us (micro-second) will be printed;
*               while average times for both read and write will be printed for test action.
*           When fail, error will be logged.
*
* EXAMPLE:  ic  p, 3, A4 - Ping slave device on bus 3, address 0x52
* EXAMPLE:  ic  r, 1, CE, C1, 1, 200 - Read from bus 1, address 0x67, register 0xC1, register_length 1, 200 bytes
* EXAMPLE:  ic  w, 2, CE, C13F, 2, 3, 9A, 37, CD - Write to bus 2, address 0x67, register 0xC13F, register_length 2,
*               the 3-byte data are 0x9A, 0x37 and 0xCD
* EXAMPLE:  ic  t, 2, 0xCE, 0xC1, 1, 2, n - Test on bus 2, address 0x67, register 0xC1, regiser_length 1,
*               2 bytes each read/write, repeating n loops
*
* @DOCETAPEND
*
*/

#define I2C_HELP_TEXT "* Use this command to read/write/test over i2c bus\n\r\tUsage :    \
ic action, bus, address, register, register_length, data_length, [loopCount or data,]"


void TAP_IICCommand(CommandLine_t*);

#define MINIMAL_I2C_PING_NUMBER_ARGUMENTS   3
#define MINIMAL_I2C_NUMBER_ARGUMENTS        6
#define I2C_SLAVE_NO_REGISTER               0xFF
#define MAX_ETAP_I2C_LOOP_COUNT             1024

#endif //_ETAPIIC_H_
