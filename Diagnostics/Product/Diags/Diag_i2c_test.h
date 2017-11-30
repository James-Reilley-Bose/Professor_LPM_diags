//
// Diag_i2c_test.h
//

#ifndef DIAG_I2C_TEST_H
#define DIAG_I2C_TEST_H

#include "DiagData.h"

//REGISTER        ADDRESS 15  14  13  12  11   10   9   8   7   6   5   4   3   2   1   0
//Result          00h     E3  E2  E1  E0  R11  R10  R9  R8  R7  R6  R5  R4  R3  R2  R1  R0
//                        This register contains the result of the most recent light to digital conversion.
//                        This 16-bit register has two fields: a4-bit exponent and a 12-bit mantissa.

//Configuration   01h     RN3 RN2 RN1 RN0 CT   M1   M0  OVF CRF FH  FL  L   POL ME  FC1 FC0   reset = C810h
//Low Limit       02h     LE3 LE2 LE1 LE0 TL11 TL10 TL9 TL8 TL7 TL6 TL5 TL4 TL3 TL2 TL1 TL0   reset = C0000h
//High Limit      03h     HE3 HE2 HE1 HE0 TH11 TH10 TH9 TH8 TH7 TH6 TH5 TH4 TH3 TH2 TH1 TH0   reset = BFFFh
//Manufacturer ID 7Eh     reset = 5449h = "TI"
//Device ID       7Fh     reset = 3001h

#define OPT_HELP "dt opt,<arg1>,<arg2>,<arg3>\r\n<arg1> = 0,1,2(init,wr,rd)\r\n<arg2> = (reg)\r\n<arg3> = (val)\r\n"
#define OPT_I2C3_ADDR       0x88


#define OPT3001_RESULT_READY_SHORT  150
#define OPT3001_RESULT_READY_LONG   1000

#define OPT_RES_REGADDR     0
#define OPT_CFG_REGADDR     1
#define OPT_CFG_MODE        0xce10  // continuous conversion mode [10:9]=3
#define OPT_LL_REGADDR      2
#define OPT_HL_REGADDR      3
#define OPT_MFG_ID_REGADDR  0x7e
#define OPT_DEV_ID_REGADDR  0x7f

#define OPT3001_REG_EXPONENT(n)     ((n) >> 12)
#define OPT3001_REG_MANTISSA(n)     ((n) & 0xfff)
#define LUX_FULL_SCALE     8386560                   // exp=b, mantissa =fff
#define ONE_PCNT_LUX_FULL_SCALE  LUX_FULL_SCALE/10000  //1% of full scale
typedef struct
{
    uint16_t  fc: 2; //Fault count field (read or write)
    uint16_t  me: 1; //Mask exponent field (read or write)
    uint16_t pol: 1; //Polarity field (read or write)
    uint16_t   l: 1; //Latch field (read or write)
    uint16_t  fl: 1; //Flag low field (read-only)
    uint16_t  fh: 1; //Flag high field (read-only)
    uint16_t crf: 1; //Conversion ready field (read-only)
    uint16_t ovf: 1; //Overflow flag field (read-only)
    uint16_t   m: 2; //Conversion mode
    uint16_t  ct: 1; //Conversion time field (read or write)
    uint16_t  rn: 4; //Range number field (read or write)


} OPT3001_CFG_BITS;

typedef union
{
    OPT3001_CFG_BITS b;
    uint8_t cb[2];
} OPT3001_CONFIG_REG;

typedef enum
{
    OPT3001_INIT,
    OPT3001_WR,
    OPT3001_RD,
    OPT3001_LUX_RD,
    OPT3001_I2C_BUS
} OPT3001_COMMANDS;

void opt_cmd(uint32_t* cmd);
void opt3001_to_iio_ret(uint16_t regval, int* val, int* val2);
int opt3001_rd_lux(uint32_t* lux);

#endif // DIAG_I2C_TEST_H
