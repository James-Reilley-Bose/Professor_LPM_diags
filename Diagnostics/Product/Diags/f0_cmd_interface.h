#ifndef __F0_CMD_INTERFACE_H__
#define __F0_CMD_INTERFACE_H__

#include "project.h"

extern uint32_t F0_VARIANT_struct_size;   /* sizeof(F0_VARIANT_struct) */
extern uint32_t F0_GPIO_PIN_struct_size;  /* sizeof(F0_GPIO_PIN_struct) */
extern uint32_t F0_MFG_PARAMS_struct_size;/* sizeof(F0_MFG_PARAMS_struct) */

/* max data sizes for our F0 i2c packets. */
#define I2C_RX_DATA_SIZE_BYTES  (128)
#define I2C_TX_DATA_SIZE_BYTES  (128)

/* max sizes for our F0 i2c packets */
#define I2C_RX_BUFFERSIZE (HDR_SIZE + I2C_RX_DATA_SIZE_BYTES)
#define I2C_TX_BUFFERSIZE (HDR_SIZE + I2C_TX_DATA_SIZE_BYTES)

/*
   I2C packet header layout (8 bytes).
   ------------------------------------------------------------------------------------------------------------------------
   | command | isr_3 | isr_2 | isr_1 | isr_0 | data_size_h | data_size_l | checksum* | data_0 | .... | data_n |
   ------------------------------------------------------------------------------------------------------------------------
 *checksum is 2's complement
*/

typedef  enum PROFESSOR_F0_CMDS
{
    CMD_PWR_HDMI_EN_WRITE = 0x86,
    CMD_PWR_HDMI_EN_READ,
    CMD_HDMI_5V_EN_WRITE,
    CMD_HDMI_5V_EN_READ,
    CMD_HDMI1_RST_L_WRITE,
    CMD_HDMI1_RST_L_READ,
    F0_VERSION = 0x9a,
    F0_HDMIPWR_ON,
    F0_HDMIPWR_OFF,
    F0_HDMIPWR_ST,
    F0_HDMI5V_TV_ON,
    F0_HDMI5V_TV_OFF,
    F0_HDMI5V_TV_ST
} PROFESSOR_F0_CMDS;

#define CMD_PWR_HDMI_EN_WRITE    (uint8_t)(0x86)
#define CMD_PWR_HDMI_EN_READ     (uint8_t)(0x87)
#define CMD_HDMI_5V_EN_WRITE     (uint8_t)(0x88)
#define CMD_HDMI_5V_EN_READ      (uint8_t)(0x89)
#define CMD_HDMI1_RST_L_WRITE    (uint8_t)(0x90)
#define CMD_HDMI1_RST_L_READ     (uint8_t)(0x91)


/* offsets into buffer */
#define HDR_CMD          0
#define HDR_ISR_3        1
#define HDR_ISR_2        2
#define HDR_ISR_1        3
#define HDR_ISR_0        4
#define HDR_DATA_SIZE_H  5
#define HDR_DATA_SIZE_L  6
#define HDR_PKT_CHECKSUM 7 /* includes HDR + DATA (all packet bytes) */
#define HDR_DATA         8
#define HDR_SIZE        HDR_DATA

/* F0 i2c commands */
/* AP GPIO pins */
#if defined(BARDEEN)
#define CMD_AP_BOOT_SEL_WRITE    (uint8_t)(0x80)
#define CMD_AP_BOOT_SEL_READ     (uint8_t)(0x81)
#define CMD_AP_PWR_EN_WRITE      (uint8_t)(0x82)
#define CMD_AP_PWR_EN_READ       (uint8_t)(0x83)
#define CMD_AP_RST_L_WRITE       (uint8_t)(0x84)
#define CMD_AP_RST_L_READ        (uint8_t)(0x85)
#endif
/* HDMI pins */
#define CMD_PWR_HDMI_EN_WRITE    (uint8_t)(0x86)
#define CMD_PWR_HDMI_EN_READ     (uint8_t)(0x87)
#define CMD_HDMI_5V_EN_WRITE     (uint8_t)(0x88)
#define CMD_HDMI_5V_EN_READ      (uint8_t)(0x89)
#define CMD_HDMI1_RST_L_WRITE    (uint8_t)(0x90)
#define CMD_HDMI1_RST_L_READ     (uint8_t)(0x91)
#if defined(BARDEEN)
#define CMD_HDMI2_RST_L_WRITE    (uint8_t)(0x92)
#define CMD_HDMI2_RST_L_READ     (uint8_t)(0x93)
#endif
/* SM2 pins */
#define CMD_PWR_SM2_3V8_EN_WRITE (uint8_t)(0x94)
#define CMD_PWR_SM2_3V8_EN_READ  (uint8_t)(0x95)
#define CMD_PWR_SM2_EN_WRITE     (uint8_t)(0x96)
#define CMD_PWR_SM2_EN_READ      (uint8_t)(0x97)
#define CMD_SM2_USB0_VBUS_EN_H_WRITE (uint8_t)(0x98)
#define CMD_SM2_USB0_VBUS_EN_H_READ  (uint8_t)(0x99)
/* variant command */
#define CMD_VARIANT              (uint8_t)(0x9A)
/* power initialization sequence cmd */
#define CMD_PWR_INIT_SEQ         (uint8_t)(0x9B)
/* mfg params command */
#define CMD_MFG_PARAMS           (uint8_t)(0x9C)
/* return status commands (cmd values are historical) */
#define CMD_ACK                  (uint8_t)(0x44)
#define CMD_NACK                 (uint8_t)(0x33)
#define CMD_NACK_BAD_CKSUM       (uint8_t)(0x35)
#define CMD_NACK_ISR             (uint8_t)(0x36)
#define CMD_NACK_UNKNOWN         (uint8_t)(0x39)

/* F0 I2C error flag struct */
typedef struct
{
    const char* name;
    uint32_t mask;
} F0_ISR_ERR_MASK_struct;

/* F0 I2C error flags */
#define  F0_I2C_ISR_TXE       ((uint32_t)0x00000001)   /*!< Transmit data register empty */
#define  F0_I2C_ISR_TXIS      ((uint32_t)0x00000002)   /*!< Transmit interrupt status */
#define  F0_I2C_ISR_RXNE      ((uint32_t)0x00000004)   /*!< Receive data register not empty */
#define  F0_I2C_ISR_ADDR      ((uint32_t)0x00000008)   /*!< Address matched (slave mode)*/
#define  F0_I2C_ISR_NACKF     ((uint32_t)0x00000010)   /*!< NACK received flag */
#define  F0_I2C_ISR_STOPF     ((uint32_t)0x00000020)   /*!< STOP detection flag */
#define  F0_I2C_ISR_TC        ((uint32_t)0x00000040)   /*!< Transfer complete (master mode) */
#define  F0_I2C_ISR_TCR       ((uint32_t)0x00000080)   /*!< Transfer complete reload */
#define  F0_I2C_ISR_BERR      ((uint32_t)0x00000100)   /*!< Bus error */
#define  F0_I2C_ISR_ARLO      ((uint32_t)0x00000200)   /*!< Arbitration lost */
#define  F0_I2C_ISR_OVR       ((uint32_t)0x00000400)   /*!< Overrun/Underrun */
#define  F0_I2C_ISR_PECERR    ((uint32_t)0x00000800)   /*!< PEC error in reception */
#define  F0_I2C_ISR_TIMEOUT   ((uint32_t)0x00001000)   /*!< Timeout or Tlow detection flag */
#define  F0_I2C_ISR_ALERT     ((uint32_t)0x00002000)   /*!< SMBus alert */
#define  F0_I2C_ISR_BUSY      ((uint32_t)0x00008000)   /*!< Bus busy */
#define  F0_I2C_ISR_DIR       ((uint32_t)0x00010000)   /*!< Transfer direction (slave mode) */
#define  F0_I2C_ISR_ADDCODE   ((uint32_t)0x00FE0000)   /*!< Address match code (slave mode) */

/* ISR read mask for possible I2C errors */
#define F0_I2C_ISR_ERRORS  ( F0_I2C_ISR_TXE     |\
                             F0_I2C_ISR_TXIS    |\
                             F0_I2C_ISR_ADDR    |\
                             F0_I2C_ISR_NACKF   |\
                             F0_I2C_ISR_STOPF   |\
                             F0_I2C_ISR_TC      |\
                             F0_I2C_ISR_TCR     |\
                             F0_I2C_ISR_BERR    |\
                             F0_I2C_ISR_ARLO    |\
                             F0_I2C_ISR_OVR     |\
                             F0_I2C_ISR_PECERR  |\
                             F0_I2C_ISR_TIMEOUT |\
                             F0_I2C_ISR_ALERT   |\
                             F0_I2C_ISR_BUSY    |\
                             F0_I2C_ISR_DIR )

#define F0_I2C_ADDR    (0xA4)  /* (0x52 << 1)  //F0 diags (same as app) i2c slave addr */

#define TWOS_COMPL(x) (~(x) + 1)

/* power initialization sequence cmd operations */
#define F0_PWR_INIT_SEQ_EN       (uint8_t)(0x11)
#define F0_PWR_INIT_SEQ_DIS      (uint8_t)(0x22)

/* GPIO pin cmd operations */
#define F0_GPIO_SET_HIGH         (uint8_t)(0xAA)
#define F0_GPIO_SET_LOW          (uint8_t)(0x55)

/* variant cmd macros */
#define VARIANT_SIZE   (10)
#define VERSION_SIZE   (40)
#define BUILDTIME_SIZE (30)
#define COPYRIGHT_SIZE (40)

/* mfg params cmd macros */
#define F0_MFG_PARAMS_READ       (uint8_t)(0xaa)
#define F0_MFG_PARAMS_WRITE      (uint8_t)(0xbb)
#define F0_MFG_PARAMS_SS         (uint8_t)(0x11)
#define F0_MFG_PARAMS_TR         (uint8_t)(0x22)
#define F0_MFG_PARAMS_ALL        (uint8_t)(0x33)
#define F0_MFG_PARAMS_PASS       (uint8_t)(0x44)
#define F0_MFG_PARAMS_FAIL       (uint8_t)(0x55)
#define F0_MFG_PARAMS_BAD_ARGS   (uint8_t)(0x66)
#define F0_MFG_PARAMS_SS_MAX_LEN (24)

/* mfg params cmd struct */
/* manufacturing parameters structure */
#pragma pack(1)
typedef struct
{
    uint8_t serial_number[ F0_MFG_PARAMS_SS_MAX_LEN ];
    uint8_t test_result;
} F0_MFG_PARAMS_s;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint8_t param;
    uint8_t action;
    F0_MFG_PARAMS_s p;
} F0_MFG_PARAMS_struct;
#pragma pack()

/* variant cmd struct */
#pragma pack(1)
typedef struct
{
    char  variant[ VARIANT_SIZE ];
    char  version[ VERSION_SIZE ];
    char  buildTime[ BUILDTIME_SIZE ];
    char  copyright[ COPYRIGHT_SIZE ];

} F0_VARIANT_struct;
#pragma pack()

/* gpio pin cmd struct */
#pragma pack(1)
typedef struct
{
    uint8_t  receivedCmd;
    uint8_t  setToState;
    uint8_t  readState;
    uint8_t  pol;
    uint8_t  OType;

} F0_GPIO_PIN_struct;
#pragma pack()


/* function prototypes */
uint8_t sum32(uint32_t val);
uint8_t sum16(uint16_t val);
void load_hdr_cmd_datasize( uint8_t* buff, uint8_t cmd, uint16_t data_size );
BOOL check_for_f0_isr_errors(uint32_t isrValue);
uint32_t get_isr_status( uint8_t* buff );
void load_pkt_data( uint8_t* buff, uint8_t* dataBuff, uint16_t dataSize);

#endif /* __F0_CMD_INTERFACE_H__ */
