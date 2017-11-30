#ifndef _ETAPSPI_H_
#define _ETAPSPI_H_

#include "etapcoms.h"


/**
* @DOCETAP
* COMMAND:   spi
*
* ALIAS:     sp
*
* DESCRIP:   SPI bus master test: Send a write/read/test/standby request.
*            Also measure and output ReadTime and WriteTime.
*
* PARAM:    action,     bus,    csPort, csPin,  length or on,   [loopCount | timeoutMs & data]
*
*           action is either 'r' (read), 'w' (write), 't' (test, i.e., both read & write) or
*                            's' ( enter/exit standby mode).
*           bus is the bus number of SPI, decimal 1, 2, or 3;
*           csPort is the ChipSelect GPIO port (or bank), value range: A,B,C,D,E,F,G,H,I,
*               csPort = 0 will use the default chipSelect GPIO port as defined in configsystem.h;
*           csPin is the pin number on the selected GPIO port, value range: 0 - 15;
*               csPin parameter will be ignored if csPort = 0;
*           length is the length of data to transfer if action is not 's', decimal format;
*           on is the flag for standby ON or OFF if action is 's', decimal 0 = OFF, 1 = ON;
*           loopCount is needed in 't' action;
*           timeoutMs is needed in 'w' action, unit is ms; max 65535, ms = 0 for non-blocking;
*           data are needed in 'w' action, data format is hex
*
* REPLY:    When succeed, TAP_OK, data read will be output to logger in 'r' action;
*               Also, average time for read or write in us (micro-second) will be printed;
*               while average times for both read and write will be printed for 't' action.
*           When fail, error will be logged.
*
* EXAMPLE:  sp  r, 1, D, 3, 200 - Read from bus 1, (csPort = D and csPin = 3), 200 bytes
* EXAMPLE:  sp  w, 2, 0, 0, 3, 40, 0x9A, 0x37, 0xD2 - Write to bus 2, 3 bytes, with timeout 40 ms,
*               the 3-byte data are 0x9A, 0x37 and 0xD2, using default chip select (csPort = 0 and csPin = 0)
* EXAMPLE:  sp  t, 2, E, 1, 3, 10 - Test on bus 2, 3 bytes each read/write, repeating 10 loops
* EXAMPLE:  sp  s, 3, 0, 0, 1 - Put bus 3 in standby mode (csPort = 0 and csPin = 0)
*
* @DOCETAPEND
*
* @author dx1007255
*/

#define SPI_HELP_TEXT "* Use this command to read/write/test/standby over SPI bus\n\r\tUsage :    \
sp action, bus, length or on, [loopCount or timeoutMs and data,]"

void TAP_SPICommand(CommandLine_t*);

#define MINIMAL_SPI_NUMBER_ARGUMENTS        5
#define SPI_MAX_BUFFER_LENGTH               256
#define MAX_ETAP_SPI_LOOP_COUNT             1024

#endif //_ETAPSPI_H_
