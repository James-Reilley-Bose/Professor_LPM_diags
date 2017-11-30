#ifndef __MPU_H__
#define __MPU_H__

void configureMPU(void);

// values for RASR size field
enum
{
    MPU_SZ_32 = 4,
    MPU_SZ_64,
    MPU_SZ_128,
    MPU_SZ_256,
    MPU_SZ_512,
    MPU_SZ_1K,
    MPU_SZ_2K,
    MPU_SZ_4K,
    MPU_SZ_8K,
    MPU_SZ_16K,
    MPU_SZ_32K,
    MPU_SZ_64K,
    MPU_SZ_128K,
    MPU_SZ_256K,
    MPU_SZ_512K,
    MPU_SZ_1M,
    MPU_SZ_2M,
    MPU_SZ_4M,
    MPU_SZ_8M,
    MPU_SZ_16M,
    MPU_SZ_32M,
    MPU_SZ_64M,
    MPU_SZ_128M,
    MPU_SZ_256M,
    MPU_SZ_512M,
    MPU_SZ_1G,
    MPU_SZ_2G,
    MPU_SZ_4G,
    MPU_SZ_INVALID = 0,
};

enum
{
    MPU_AP_PRIV_NONE_USER_NONE = 0x0,
    MPU_AP_PRIV_RW_USER_NONE,
    MPU_AP_PRIV_RW_USER_RO,
    MPU_AP_PRIV_RW_USER_RW,
    MPU_AP_RESERVED0,
    MPU_AP_PRIV_RO_USER_NONE,
    MPU_AP_PRIV_RO_USER_RO,
    /* 7 is a duplicate of 6 */
};

typedef struct
{
    uint32_t ADDR;
    unsigned SIZE: 5;
    unsigned REGION: 8;
    unsigned AP: 3;
    unsigned TEX: 3;
    unsigned S: 1;
    unsigned C: 1;
    unsigned B: 1;
    unsigned SRD: 8;
} mpu_entry_t;

extern int mpu_fault_buf_sz;
extern char mpu_fault_buf[32];
void TAP_ExcPrint(char* s);

#define MPU_FaultPrintf(x...) \
  mpu_fault_buf[mpu_fault_buf_sz - 1] = 0; \
  snprintf(mpu_fault_buf, mpu_fault_buf_sz - 1, x); \
  TAP_ExcPrint(mpu_fault_buf);


#endif

