////////////////////////////////////////////////////////////////////////////////
/// @file            SystemMessages.h
/// @brief           Identifies all the system level messages that FreeRTOS will
///                  use
/// @author          Richard Jackson
/// @date            Creation Date: Mon Dec 5 2011
///                  Modified Date: Tues Mar 18 2013 (for Triode project)
///
/// Copyright 2011 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_SYSTEM_MESSAGES_H
#define INCLUDE_SYSTEM_MESSAGES_H


/* Message ID Ranges for the tasks in the system.
*
*  This ensures no overlap in messages for different Tasks, meaning we can
*  trap messages sent to the wrong Task and track unique system messages in
*  the system.
*/
#define SYS_MSG_BASE_SIZE           0x00001000UL
#define SYS_MSG_BASE_START          0x00000001UL

typedef unsigned long systemMessageID_t;

/* 0x00000000 - 0x00001000 */
#define SYS_MSG_UI_TASK_MODULE_START        SYS_MSG_BASE_START
#define SYS_MSG_UI_TASK_MODULE_END          (SYS_MSG_UI_TASK_MODULE_START      + SYS_MSG_BASE_SIZE)

/* 0x00001000 - 0x00002000 */
#define SYS_MSG_IR_TASK_MODULE_START        SYS_MSG_UI_TASK_MODULE_END
#define SYS_MSG_IR_TASK_MODULE_END          (SYS_MSG_IR_TASK_MODULE_START      + SYS_MSG_BASE_SIZE)

/* 0x00002000 - 0x00003000 */
#define SYS_MSG_KEY_TASK_MODULE_START       SYS_MSG_IR_TASK_MODULE_END
#define SYS_MSG_KEY_TASK_MODULE_END         (SYS_MSG_KEY_TASK_MODULE_START     + SYS_MSG_BASE_SIZE)

/* 0x00003000 - 0x00004000 */
#define SYS_MSG_KEY_TASK_START              SYS_MSG_KEY_TASK_MODULE_END
#define SYS_MSG_KEY_TASK_END                (SYS_MSG_KEY_TASK_START            + SYS_MSG_BASE_SIZE)

/* 0x00004000 - 0x00005000 */
#define SYS_MSG_ETAP_TASK_START             SYS_MSG_KEY_TASK_END
#define SYS_MSG_ETAP_TASK_END               (SYS_MSG_ETAP_TASK_START           + SYS_MSG_BASE_SIZE)

/* 0x00005000 - 0x00006000 */
#define SYS_MSG_DISPLAY_TASK_MODULE_START   SYS_MSG_ETAP_TASK_END
#define SYS_MSG_DISPLAY_TASK_MODULE_END     (SYS_MSG_DISPLAY_TASK_MODULE_START + SYS_MSG_BASE_SIZE)

/* 0x00006000 - 0x00007000 */
#define SYS_MSG_NV_TASK_MODULE_START        SYS_MSG_DISPLAY_TASK_MODULE_END
#define SYS_MSG_NV_TASK_MODULE_END          (SYS_MSG_NV_TASK_MODULE_START      + SYS_MSG_BASE_SIZE)

/* 0x00007000 - 0x00008000 */
#define SYS_MSG_POWER_TASK_START            SYS_MSG_NV_TASK_MODULE_END
#define SYS_MSG_POWER_TASK_END              (SYS_MSG_POWER_TASK_START          + SYS_MSG_BASE_SIZE)

/* 0x00008000 - 0x00009000 */
#define SYS_MSG_ROOMMGR_TASK_START          SYS_MSG_POWER_TASK_END
#define SYS_MSG_ROOMMGR_TASK_END            (SYS_MSG_ROOMMGR_TASK_START        + SYS_MSG_BASE_SIZE)

/* 0x00009000 - 0x0000A000 */
#define SYS_MSG_BT_TASK_START               SYS_MSG_ROOMMGR_TASK_END
#define SYS_MSG_BT_TASK_END                 (SYS_MSG_BT_TASK_START             + SYS_MSG_BASE_SIZE)

/* 0x0000A000 - 0x0000B000 */
#define SYS_MSG_HDMI_TASK_START             SYS_MSG_BT_TASK_END
#define SYS_MSG_HDMI_TASK_END               (SYS_MSG_HDMI_TASK_START           + SYS_MSG_BASE_SIZE)

/* 0x0000B000 - 0x0000C000 */
#define SYS_MSG_SS_TASK_MODULE_START        SYS_MSG_HDMI_TASK_END
#define SYS_MSG_SS_TASK_MODULE_END          (SYS_MSG_SS_TASK_MODULE_START      + SYS_MSG_BASE_SIZE)

/* 0x0000C000 - 0x0000D000 */
#define SYS_MSG_LISA_TASK_MODULE_START      SYS_MSG_SS_TASK_MODULE_END
#define SYS_MSG_LISA_TASK_MODULE_END        (SYS_MSG_LISA_TASK_MODULE_START    + SYS_MSG_BASE_SIZE)

/* 0x0000D000 - 0x0000E000 */
#define SYS_MSG_HEAT_TASK_MODULE_START      SYS_MSG_LISA_TASK_MODULE_END
#define SYS_MSG_HEAT_TASK_MODULE_END        (SYS_MSG_HEAT_TASK_MODULE_START    + SYS_MSG_BASE_SIZE)

/* 0x0000E000 - 0x0000F000 */
#define SYS_MSG_FWUPDATE_TASK_MODULE_START      SYS_MSG_HEAT_TASK_MODULE_END
#define SYS_MSG_FWUPDATE_TASK_MODULE_END        (SYS_MSG_FWUPDATE_TASK_MODULE_START    + SYS_MSG_BASE_SIZE)


#define SYS_MSG_END_OF_LIST                 SYS_MSG_HEAT_TASK_MODULE_END

#endif // INCLUDE_SYSTEM_MESSAGES_H
