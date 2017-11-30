/**
  ******************************************************************************
  * @file    UEIDefinitions.h
  * @author  cc1014948
  * @version V0.0.1
  * @date    04-May-2015
  * @brief   UEI global definitions.
  *          This file provides definitions for the UEI drivers
 @verbatim
 ===============================================================================
 **/
#ifndef _UEIDEFINITIONS_H
#define _UEIDEFINITIONS_H

#define UEI_YES 0x1
#define UEI_NO  0x2

/* According per: BoseQS_3457_WW_Global_Key_Chart_1 5.xlsx, the keymap could be upto
  244 bytes. This is the largest received data for blaster for the moment.
*/
/* Per UEI OEM QS IR Blaster Product Specification Rev 2.0, GetKeyMap could return 2 header bytes
  and (1-255) bytes of key map, so planned for the worst. */
#define UEI_RECEIVE_BUFFER_SIZE                 260
#define UEI_RECEIVE_BUFFER_DATA_LENGTH_POS      0

/* GetVersion, SendKey, GetKeyMap, GetNextDevice, GetPreviousDevice, */
#define UEI_RECEIVE_BUFFER_RESPONSE_LEN_POS     0
#define UEI_RECEIVE_BUFFER_RESPONSE_CODE_POS    1
#define UEI_RECEIVE_BUFFER_FIRST_DATA_POS       2

/* ListAllUpgradeCode */
#define UEI_RECEIVE_BUFFER_NUM_OF_IDS_POS       2
/* The number of codeset within the MAX610 FDRA, assuming each codeset size of
 * 100, 4k of FDRA will allow 40 codesets within the FDRA
 */
#define UEI_MAX_NUMBER_OF_CODESET_IN_FDRA       40

/* As per the document: BoseQS_3457_WW_Global_Key_Chart_1.5.xlsx */
#define UEI_MAX_NUMBER_OF_KEYS_IN_KEYMAP        245

/* Per UEI OEM QS IR Blaster Product Specification Rev 2.0, max response time
 * is 13 ms However, it can be observed that the IR Blaster sends out status
 * to an endKey much later than this (100ms). Likely whenever the current
 * key in transmission ends. Furthermore, it appears any command that
 * writes/erases internal NV storage of the UEI chip requires additional time.
 *
 * The following commands have been measured
 * DownLoadDeviceToFDRA = 172ms to response
 * MasterReset = 345 ms to response
 * DeleteDownloadCode = 58 ms to response
 * GetVersion = 235 us to response
 * ListAllUpgradeCode = 300us
 * GetKeyMap = 2ms to response + data length @ baudrate
 *
 * For GetKeyMap, we will assume worst case receive size (260)
 * ,worst case response delay, and min tolerance @ 19200(-2.5%)
 * 1 start bit + 8 data bits + 1 stop bit = 10 bits/byte
 * 260 Bytes * 10 bits/byte = 2600 bits
 * Baudrate = 19200 *.975 = 18720
 * 2600/18720 = 138.8ms
 * Default = 139ms + 13ms = 152ms
 */
#define UEI_DEFAULT_TIME_MS 500
#define UEI_KEY_TIME_MS 25
#define UEI_END_KEY_TIME_MS 120
#define UEI_LOAD_CODESET_TIME_MS  2500


#define UEI_NO_RESPONSE 0xFF

#define UEI_SEND_KEY_ID             0x01
#define UEI_GET_KEY_MAP_ID          0x02
#define UEI_MASTER_RESET_ID         0x09
#define UEI_SW_VER_ID               0x0B
#define UEI_DEL_CODE_SET_ID         0x0C
#define UEI_LIST_ALL_CODE_ID        0x11
#define UEI_DWNLD_CODE_SET_ID       0x22

/*
===============================================================================
@ UEI command definitions
===============================================================================
*/
#define UEI_MESSAGE_FLAG_INDEX 6
#define UEI_SEND_QUICK_KEY_FLAG 0x80

#define UEI_610LP_SYSTEM_ID {0x40, 0x41, 0x42, 0x43}

#define UEI_WAKE_UP_COMMAND {00}
#define UEI_WAKE_UP_COMMAND_LENGTH 1

#define UEI_GET_VERSION_COMMAND {0x00, 0x01, 0x0b}
#define UEI_GET_VERSION_COMMAND_LENGTH 3

#define UEI_SEND_KEY_COMMAND {0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define UEI_SEND_KEY_COMMAND_LENGTH 9
#define UEI_SEND_KEY_CODE_SET_POS_0 3
#define UEI_SEND_KEY_CODE_SET_POS_1 4
#define UEI_SEND_KEY_KEY_CODE_POS   5

/* RcdLength(2), 22h, DeviceTypeNum(2), DownloadData(n) */
#define UEI_DOWNLOAD_CODE_SET_COMMAND {0x00, 0x00, 0x22, 0x00, 0x00}
#define UEI_DOWNLOAD_CODE_SET_COMMAND_LENGTH 5
#define UEI_DOWNLOAD_COMMAND_LENGTH_POS_0 0
#define UEI_DOWNLOAD_COMMAND_LENGTH_POS_1 1
#define UEI_DOWNLOAD_CODE_SET_POS_0 3
#define UEI_DOWNLOAD_CODE_SET_POS_1 4

/* RcdLength(2), 11h */
#define UEI_LIST_ALL_UPGRADE_CODE_COMMAND {0x00, 0x00, 0x11}
#define UEI_LIST_ALL_UPGRADE_CODE_COMMAND_LENGTH 3
#define UEI_LIST_ALL_UPGRADE_CODE_COMMAND_LENGTH_POS_0 0
#define UEI_LIST_ALL_UPGRADE_CODE_COMMAND_LENGTH_POS_1 1


#define UEI_MASTER_RESET_COMMAND { 0x00, 0x02, 0x09, 0x00 }
#define UEI_MASTER_RESET_COMMAND_LENGTH 4
#define UEI_MASTER_RESET_RESET_LEVEL    3


#define UEI_REMOVE_SINGLE_CODESET_COMMAND {0x00, 0x03, 0x0C, 0xEE, 0xEE}
#define UEI_REMOVE_SINGLE_CODESET_POS_0              3
#define UEI_REMOVE_SINGLE_CODESET_POS_1              4
#define UEI_REMOVE_SINGLE_CODESET_COMMAND_LENGTH     5

#define UEI_GET_KEY_MAP_COMMAND {0x00, 0x03, 0x02, 0xEE, 0xEE}
#define UEI_GET_KEY_MAP_COMMAND_LENGTH_POS_0    3
#define UEI_GET_KEY_MAP_COMMAND_LENGTH_POS_1    4
#define UEI_GET_KEY_MAP_COMMAND_LENGTH          5

/*
===============================================================================
@ Response code from UEI
===============================================================================
*/
#define UEI_RESP_SUCCESS                                        0x0
#define UEI_RESP_INVALID_DEVICE_CODE                            0x1
#define UEI_RESP_INVALID_DEVICE_TYPE                            0x2
#define UEI_RESP_INVALID_KEY_CODE_NO_IR_DATA_PICKED             0x3
#define UEI_RESP_BAD_FDRA_BAD_MEMORY                            0x4
#define UEI_RESP_OUT_OF_MEMORY                                  0x5
#define UEI_RESP_LEARNING_ERROR_TIME_OUT                        0x6
#define UEI_RESP_DATA_PACKET_FORMAT_ERROR                       0x7
#define UEI_RESP_DOWNLOADED_ID_ALREADY_EXIST                    0x8
#define UEI_RESP_LOW_VOLTAGE_FDRA_ACCESS_ERROR                  0x9
#define UEI_RESP_INVALID_LEARNCODEID                            0xa
#define UEI_RESP_INCORRECT_SEQUENCE_NUMBER                      0xb
#define UEI_RESP_INVALID_IMAGE_NO_APPLICATION_AVAILABLE         0xc
#define UEI_RESP_DECRYPT_FAILED                                 0xd
#define UEI_RESP_INVALID_COMMAND                                0xe
#define UEI_RESP_UNDEFINED                                      0xf

/*
===============================================================================
@ UEI Service semaphore wait delay
===============================================================================
*/
#define UEI_BLASTER_SERVICE_RETRY_DEFAULT       3
#define UEI_BLASTER_PEND_DEFAULT                1

/*
===============================================================================
@ UEI Service command type
===============================================================================
*/
#define UEI_SERVICE_GET_SOFTWARE_VERSION                0x1
#define UEI_SERVICE_SEND_KEY                            0x2
#define UEI_SERVICE_MASTER_RESET                        0x3
#define UEI_SERVICE_DOWNLOAD_DEVICE_TO_FDRA             0x4
#define UEI_SERVICE_DELETE_DOWNLOAD_CODE                0x5

#define UEI_MASTER_RESET_CLEAR_ALL             0x01
#define UEI_MASTER_RESET_CLEAR_LEARNED_CODE    0x02
#define UEI_MASTER_RESET_CLEAR_CODESET         0x03

#endif /* _UEIDEFINITIONS_H_ */
