
/**
 * @DOCETAP
 * COMMAND:   ipc
 *
 * ALIAS:     ip
 *
 * DESCRIP:   Send IPC to a specific device
 *
 * PARAM:     clear: set to "clear" to reset the list
 *
 * REPLY:     empty
 *
 * EXAMPLE:   ip
 *
 * @DOCETAPEND
 */

#define IPC_HELP_TEXT "* This command sends an ipc message " \
                      "\r\n* \tUsage: ipc <send/sendraw/rtw/bulk/bulkabort/hammer/buffer/setstatus/log/stats>"
#define IPC_SEND_HELP_TEXT "This command sends an ipc message to a specific device" \
                      "\r\n\tUsage: ipc send, <destination>, <opcode>, <ascii hex data...>"
#define IPC_SENDRAW_HELP_TEXT "* This command sends a raw ipc message" \
                      "\r\n\tUsage: ipc sendraw, <ascii hex data...>, [xor]" \
                      "\r\n\tData will be padded with 0x00" \
                      "\r\n\tIf you specific xor, the xor word will be calcuated for you"
#define IPC_RTW_HELP_TEXT "* This command sends a rtw ipc message to a specific device" \
                      "\r\n\tUsage: ipc rtw, <destintation>, <0/1/Number>" \
                      "\r\n\t0: off" \
                      "\r\n\t1: on" \
                      "\r\n\tNumber: time to live"
#define IPC_BULK_HELP_TEXT "* This command sends a bulk test pattern to a specific device" \
                      "\r\n\tUsage: ipc bulk,<destination>,<length>,<fileID>,<filename>,<valid>"
#define IPC_BULK_ABORT_HELP_TEXT "* This command aborts an ongoing bulk transfer" \
                      ":\r\n\tUsage: ipc bulkabort,<transferID>"
#define IPC_HAMMER_HELP_TEXT "* This command continuously sends packets with an incrementing counter" \
                      "\r\n\tUsage: ipc hammer,<destination>,<start/stop>"
#define IPC_SETSTATUS_HELP_TEXT "This command sets the LPM status for all future H&S reports" \
                      "\r\n\tUsage: ipc setstatus, 01"
#define IPC_LOG_HELP_TEXT "This command sets up the list of opcodes you ALWAYS want to log (up to 10)" \
                      "\r\n\tUsage: ipc log, 1405\r\n\tor ipc log,00"


void TAP_IpcCommand(CommandLine_t*);

BOOL stringToDestination(char* str, uint8_t* destination);
