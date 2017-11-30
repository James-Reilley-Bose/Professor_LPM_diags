#ifndef INCLUDE_SERIALPSOCLISTENER_H
#define INCLUDE_SERIALPSOCLISTENER_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "project.h"
#include "etap.h"
#include "TapParserAPI.h"
#include "TapListenerAPI.h"
#include "SystemAnalysis.h"
#include "ascii.h"
#include "i2cMaster_API.h"
#include "rottenlog.h"


#define PSOC_CMD_BUFFER_SZ 64
#define PSOC_READ_SZ 6


extern uint8_t psoc_cmd[PSOC_CMD_BUFFER_SZ];
extern uint8_t psoc_rcv[PSOC_READ_SZ];
extern I2C_BUS_HANDLE_TYPE psocI2cHandle;
extern int gbl_psoc_int_det;

void psoc_get_str(uint8_t *str);

void Listener_PSOC_PrintString(uint8_t *s);
void Listener_Psoc_PutString(uint8_t *s);
int psoc_i2c_init(void);
void get_psoc_listener_buffer(uint8_t *str);

// Bootloader stuff
int psoc_open_connection(void);
int psoc_close_connection(void);
int psoc_write(unsigned char* wrData, int byteCnt);
int psoc_read(unsigned char* rdData, int byteCnt);
void psoc_read_result(void);

#endif // INCLUDE_SERIALPSOCLISTENER_H
