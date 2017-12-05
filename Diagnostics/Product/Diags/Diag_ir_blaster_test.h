/**
  ******************************************************************************
  * @file    Diag_ir_blaster_test.h
  * @brief   Diag test interface.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the IR blaster:
  *           + Diag test interface for IR blaster functionality  
 ===============================================================================
 **/
#ifndef DIAG_IR_BLASTER_H
#define DIAG_IR_BLASTER_H

/*
 * @DOCETAP
 * COMMAND:   ueiblaster
 *
 * ALIAS:     ut
 *
 * DESCRIP:   Forward commands to UEI MAX610LP chip. 
 *
 * PARAM:     action, timeout, codeset_byte_1, codeset_byte_2, key_code
 *            action:  'v'  get sw (version) from MAX610LP, 'd' (download) code set to MAX610LP, 'l' (list) code set on MAX610LP, 
 *                     'r' remove codeset from MAX610LP,'s' (send) key from code set, 'k' get (keymap) from code set
 *            timeout: Amount of time for time out in ms
 *            codeset_byte_1:  first byte of code set
 *            codeset_byte_2:  second byte of code set
 *            key_code: key code to send to device under control, refer to Table 5.2 in UEI OEM QS IR Blaster (LP) Product Specification
 *
 * REPLY:    When succeed, software version will be displayed for 'v', key map will be displayed for 'k', code set will be 
 *           displayed for 'l' and response from MAX610LP will be printed out for 'd','r','s'
 *           When fail, error will be logged.
 *
 * EXAMPLE: ut s, 1000, T2051, 0x4
 *          |  |     |    |     |    
 *          |  |     |    |     |    
 *          |  |     |    |     |--------------------- key code
 *          |  |     |    +--------------------------- code set byte 1
 *          |  |     +-------------------------------- time out (1000 ms)
 *          |  | 
 *          |  +-------------------------------------- Command type
 *          +----------------------------------------- tap command
 * EXAMPLE: ut v,1000                   - Get software version for MAX610LP
 * EXAMPLE: ut d,1000, T2051            - Download code set T2051
 * EXAMPLE: ut l,1000                   - List code set in MAX610LP
 * EXAMPLE: ut k,1000, T2051            - Print key map for code set T2051
 * EXAMPLE: ut s,1000, T2051, 0x04      - Send key 0x4 from code set T2051
 *
 * @DOCETAPEND
 * 
 */
   
typedef enum{
  DIAG_IR_BLASTER_VERSION=0x1,
  DIAG_IR_BLASTER_CODESET,
  DIAG_IR_BLASTER_DOWNLOAD_CODESET,
  DIAG_IR_BLASTER_SEND_KEY,
  DIAG_IR_BLASTER_SEND_KEY_END
}DIAG_IR_BLASTER_COMMANDS;

void DIAG_UEIBlasterCommand_Info(DIAG_IR_BLASTER_COMMANDS commandType, uint32_t param);

#endif
