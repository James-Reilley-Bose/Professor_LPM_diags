//
// etapFlash.h
//

#ifndef _ETAP_FLASH_H_
#define _ETAP_FLASH_H_

#include "etapcoms.h"
#include "etap.h"

#include "nvram.h"

/**
 * @DOCETAP
 * COMMAND:   flash
 *
 * ALIAS:     sf
 *
 * DESCRIP:   Erases, writes, and reads spi flash
 *
 * PARAM:     address: starting address
 *            length: number of bytes to write
 *            watermark: pattern to write to flash
 *
 * REPLY:     empty
 *
 * EXAMPLE:   sf 0,100000,AC
 *
 * @DOCETAPEND
 */

#define FLASH_HELP_TEXT "* Write a watermark to spi flash\r\n\tUsage: sf <address>,<length>,<watermark>"

TAPCommand(TAP_Flash);

/**
 * @DOCETAP
 * COMMAND:   flashdump
 *
 * ALIAS:     sfd
 *
 * DESCRIP:   Dumps spi flash contents to TAP
 *
 * PARAM:     address: starting address
 *            length: number of bytes to read
 *            pretty: 0/1 (nice outout)
 *
 * REPLY:     empty
 *
 * EXAMPLE:   sfd 0,100
 *
 * @DOCETAPEND
 */

#define FLASH_DUMP_HELP_TEXT "* Dump contents of spi flash to tap\r\n\tUsage: sfd <address>,<length>,[0/1]"

TAPCommand(TAP_FlashDump);

/**
 * @DOCETAP
 * COMMAND:   flasherase
 *
 * ALIAS:     sfe
 *
 * DESCRIP:   Erase the entire flash chip
 *
 * PARAM:     address: starting address
 *            length: number of bytes or erase
 *            or
 *            "all": erase the entire chip
 *
 * REPLY:     n/a
 *
 * EXAMPLE:   sfe all: erase the entire chip
 * EXAMPLE:   sfe 0, 1000: erase 0x0000 -> 0x1000
 *
 * @DOCETAPEND
 */
TAPCommand(TAP_FlashErase);
#define FLASH_ERASE_HELP_TEXT "* Erase the flash chip\n* Usage: sfe address, length | sfe all"

#endif // _ETAP_FLASH_H_
