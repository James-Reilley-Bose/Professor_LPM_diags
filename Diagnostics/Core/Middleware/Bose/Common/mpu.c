#include <stdio.h>
#include "project.h"
#include "etap.h"
#include "TapParserAPI.h"
#include "BoseUSART_API.h"
#include "mpu.h"
#include "unwind.h"

/*
 * for now we just define a negative region and allow access to everything
 * else via the background region; in the future the background region may
 * go away and we may define all the regions where access is allowed
 */
static mpu_entry_t mpu_table[] =
{
    {
        .ADDR = 0x00000000, .SIZE = MPU_SZ_128M, .AP = MPU_AP_PRIV_NONE_USER_NONE,
        .REGION = 0, .TEX = 0, .S = 0, .C = 0, .B = 0, .SRD = 0
    },

    {
        .SIZE = MPU_SZ_INVALID,
    },
};

/*
 * Enable MPU regions according to the mpu_regions table.
 */
void configureMPU(void)
{
    mpu_entry_t* e;

    TAP_Printf("Cortex MPU\r\n");
    TAP_Printf("IREGION: %d DREGION: %d \r\n",
               (MPU->TYPE & MPU_TYPE_IREGION_Msk) >> MPU_TYPE_IREGION_Pos,
               (MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_IREGION_Pos);

    for (e = mpu_table; e->SIZE != MPU_SZ_INVALID; e++)
    {
        MPU->RNR = e->REGION;
        MPU->RBAR = e->ADDR;
        MPU->RASR = (e->AP << MPU_RASR_AP_Pos) | (e->SIZE << MPU_RASR_SIZE_Pos) |
                    (1 << MPU_RASR_ENA_Pos);
    }

    // enable mpu and default background region
    MPU->CTRL = (1 << MPU_CTRL_PRIVDEFENA_Pos) | (1 << MPU_CTRL_ENABLE_Pos);

    // enable memfault exceptions
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

    // sync
    asm("dsb");
    asm("isb");

#if TEST_MPU
    volatile uint32_t* test_ptr = NULL;
    *test_ptr = 14;
#endif
}

/*
 * Memory manager fault
 * TODO: we should dump out regs for all fault types
 */
char mpu_fault_buf[32];
int mpu_fault_buf_sz = sizeof(mpu_fault_buf);
static volatile uint32_t fault_regs[8];

/*
 * Blocking print out the tap port that doesn't care if we're in an
 * interrupt
 */
void TAP_ExcPrint(char* s)
{
    while (*s)
    {
        while ( !BUSART_TRANSMIT_COMPLETE(TAP_BUART) )
        {
        }
        BUSART_SendData(TAP_BUART, *s++);
    }
}

void dump_stack(void)
{
    cpu_state_t cpu_state;

    asm volatile("mrs %0, psp" : "=r" (cpu_state.r.regs.sp));
    cpu_state.r.regs.lr = fault_regs[5];
    cpu_state.pc = fault_regs[6];
    cpu_state.r.regs.sp += 32;
    unwind_stack(&cpu_state, 256, 32);
}

/*
 * Dump registers from the fault
 */
void __CrashHandler(
    uint32_t* faultStack, uint32_t exceptionStatus)
{
    static int faulted = 0;

    if (faulted)
    {
        NVIC_SystemReset();
    }
    ++faulted;

    fault_regs[0] = faultStack[0];
    fault_regs[1] = faultStack[1];
    fault_regs[2] = faultStack[2];
    fault_regs[3] = faultStack[3];
    fault_regs[4] = faultStack[4];
    fault_regs[5] = faultStack[5];
    fault_regs[6] = faultStack[6];
    fault_regs[7] = faultStack[7];

    MPU_FaultPrintf("Fault\r\n");
    MPU_FaultPrintf("ipsr:\t0x%08x\r\n", exceptionStatus);
    MPU_FaultPrintf("r0:\t0x%08x\r\n", fault_regs[0]);
    MPU_FaultPrintf("r1:\t0x%08x\r\n", fault_regs[1]);
    MPU_FaultPrintf("r2:\t0x%08x\r\n", fault_regs[2]);
    MPU_FaultPrintf("r3:\t0x%08x\r\n", fault_regs[3]);
    MPU_FaultPrintf("r12:\t0x%08x\r\n", fault_regs[4]);
    MPU_FaultPrintf("lr:\t0x%08x\r\n", fault_regs[5]);
    MPU_FaultPrintf("pc:\t0x%08x\r\n", fault_regs[6]);
    MPU_FaultPrintf("psr:\t0x%08x\r\n", fault_regs[7]);

    dump_stack();

    // pretend to double-fault (this will make us reboot or hang)
    __CrashHandler(NULL, 0);

}

