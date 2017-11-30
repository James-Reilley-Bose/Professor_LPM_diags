/******************** (C) COPYRIGHT 2006, 2009, 2010 STMicroelectronics ********
* File Name          : main.c
* Author             : Richard jackson from Dylan from MDC
* Date First Issued  : 03/10/2006 : V0.1
* Description        : Main program body
********************************************************************************/

/*
*   About those clocks...
*
*  The function SystemInit in the file system_stm32f2xx.c will
*  do all the clock set up initially - it defaults to HCLK at 16MHz. We
*  change that using clock values determined using the xls
*  macro file 'STM32F2xx_Clock_Configuration_V1.1.0.xls' saved at the project
*  root level. That tool auto-creates system_stm32f2xx.c. Put it in
*  \STMLibraries\CMSIS\CM3\DeviceSupport\ST\STM32F2xx.
*
*  PLL_M is the divisor that takes the external cystal and drops it to
*  1-2MHZ as the clock input to the VCO.
*
*  For the 8MHz crystal, must be between 1-2MHz so it can 4 or 8 (whole
*  numbers only). 4 is better as it reduces jitter (according to docs).
*
*  Therefore, VCO input = 2MHz
*
*  PLL_N is the VCO multiplication factor. This gives the VCO output freq.
*
*  Result must be between 63 and 432 MHz. Thus for 8Mhz, the VCO input is
*  2MHz so with PLL_P = 2, PLL_N is 240.
*
*  SYSCLK = PLL_VCO / PLL_P. This is the main system clock which cannot exceed
*  120MHz. PLL_P must be 2,4,6,8
*
*  Finally, PLL_Q drives USB OTG FS, SDIO and RNG Clock which are not used but
*  the logic still needs a value.
*/

/*
*  About the RTOS
*
*  Message Queues are created when the tasks are created.
*
*  We automatically get the idle task and because CONFIG_UseTimers is
*  enabled in the FreeRTOSConfig.h file, we get a timer task also.
*
*  STACKS - the main stack, called CSTACK, is only used during initialization
*           and then the RTOS takes over with seperate stacks so CSTACK need
*           only be big enough for the initialization sequence.
*
*  HEAP   - same as CSTACK, the RTOS has it's own. Use pvPortMalloc
*
* RTOS RAM Consumption : 236 bytes for scheduler + (76 bytes for each Q + length
*                        of Q) + 64 bytes for the task + tasks stack size.
*/

#include <stdio.h>
#include "project.h"
#include "etap.h"
#include "watchdog.h"
#include "SystemAnalysis.h"
#include "TaskDefs.h"
#include "PowerTask.h"
#include "nv_mfg.h"
#include "i2cMaster_API.h"
#include "internalFlashAPI.h"
#include "serialTapListener.h"
#include "mpu.h"

SCRIBE_DECL(system);
SCRIBE_DECL(event);

volatile system_time_t g_SystemUpTime_ms;

/***************************************************************************//**
* @brief    Called from the RTOS idle task when there are spare cycles.
* @param    void
* @return   void
*******************************************************************************/
void vApplicationIdleHook(void)
{
    // TODO: add system analysis to keep track of how often the idle task is run

    PAT_WATCHDOG();

    /* Wait for Interrupt */
    __WFI();
}

/***************************************************************************//**
* @brief    Called from the RTOS every OS tick - so it better be short.
* @param    void
* @return   void
*******************************************************************************/
void vApplicationTickHook (void)
{
    static uint8_t tickCount = 0;
    if (++tickCount == (1000 / configRTOS_TICK_INTERVAL_US))
    {
        tickCount = 0;
        g_SystemUpTime_ms++;
    }
}

/***************************************************************************//**
* @brief    Called from RTOS when a stack has overflowed and the CHECK_STACK
*           option is set in FreeRTOSConfig.h
*
*           NOTE : This doesn't always work as a blown stack rarely leaves the
*                  system stable enough to get the message out ETAP but it should
*                  manage to get it written to EE.
*
* @param    pxTask - pointer to the task handle
* @param    pcTaskName - pointer to the tasks name.
* @return   void
*******************************************************************************/
void vApplicationStackOverflowHook( xTaskHandle* pxTask, signed char* pcTaskName )
{
    uint32_t msp, psp;
    tskTCB* t = (tskTCB*)pxTask;

    asm volatile("mrs %0, msp" : "=r" (msp));
    asm volatile("mrs %0, psp" : "=r" (psp));
    MPU_FaultPrintf("!!!!! stack overflow !!!!!\r\n");
    MPU_FaultPrintf("task: %s\r\n", t->pcTaskName);
    MPU_FaultPrintf("stack pointer: 0x%08x\r\n", t->pxTopOfStack);
    MPU_FaultPrintf("stack base: 0x%08x\r\n", t->pxStack);
    MPU_FaultPrintf("psp: 0x%08x\r\n", psp);
    MPU_FaultPrintf("msp: 0x%08x\r\n", msp);

    // do whatever it is we used to do
    TAP_AssertPrint("done, asserting!\r\n", 0);
}

/***************************************************************************//**
* @brief    Turns on the clocks to the shared peripherals we need running.
*
*           All other peripherals are enabled by their owning subsystems.
*
* @param    void
* @return   void
*******************************************************************************/
static void Main_EnablePeripheralClocks (void)
{
    uint32_t periphs;

    /*
    ** The functional characteristics of peripheral in each bus
    ** are board specific and therefore added to the <board>config.h file.
    */
    periphs = BUS_AHB1_PERIPHERAL_SET;

    RCC_AHB1PeriphResetCmd(periphs, ENABLE);
    RCC_AHB1PeriphClockCmd(periphs, ENABLE);
    RCC_AHB1PeriphResetCmd(periphs, DISABLE);

    periphs = BUS_APB1_PERIPHERAL_SET;

    RCC_APB1PeriphClockCmd(periphs, ENABLE);
    periphs = BUS_APB2_PERIPHERAL_SET;

    RCC_APB2PeriphClockCmd (periphs, ENABLE);

}

/***************************************************************************
 * @brief    Calculate pllN based on the defines in the device config files. This calculation
 *           makes the assumption that the input freq is always 1Mhz
 *
 * @param    ptr to pllN, it will be updated here
 * @param    ptr to pllP, it will be updated here
 * @return   return TRUE is N and P exist for the desired freq
 *******************************************************************************/
static BOOL CalculatePLLNandP(uint16* pllN, uint8_t* pllP)
{
    // To satisfy our needs (N/Q == 48Mhz), pllN can only be 192, 240, 288, 336, 384, 432
    for (int N = 192; N <= 432; N += 48)
    {
        // Check if it divides evenly, if not move on
        if (N % DESIRED_SYS_CLK_MHZ != 0)
        {
            continue;
        }
        // it divides evenly but we only have 4 possible P divide choices, 2,4,6,8
        *pllP = N / DESIRED_SYS_CLK_MHZ;

        //If odd or greater than 8, then it's invalid, move on
        if ((*pllP & 0x1) || *pllP > 8)
        {
            continue;
        }
        // we made it here so N and P must be a valid values
        *pllN = N;
        return TRUE;
    }
    return FALSE;
}

#define MAX_SYS_CLK_MHZ  (120)
#define MAX_PLL_SRC_DIV  (63)
/***************************************************************************
 * @brief    System Clock Configuration
 *
 *           Will Calculate the PLL settings based on defines in the device config headers
 *           DESIRED_SYS_CLK_MHZ is define in the IDE or the default is set in FreeRTOSConfig.h
 *           Also see table in system_stm32f2xx.c
 *
 * @param    void
 * @return   None
 *******************************************************************************/
void SystemClock_Config(void)
{
    uint32_t timeoutCounter = HSE_STARTUP_TIMEOUT;
    int8_t pllM;
    uint16_t pllN;
    uint8_t pllP;
    uint8_t pllQ;

    debug_assert(DESIRED_SYS_CLK_MHZ <= MAX_SYS_CLK_MHZ);
    debug_assert((CLK_SRC / 1000000) < MAX_PLL_SRC_DIV);

    // Defines are in the device <device>_config.h files
    // Keep pllM set to input Freq to produce 1 MHz(needed to keep PLLI2S params constant)
    pllM = CLK_SRC / 1000000;

    BOOL found = CalculatePLLNandP(&pllN, &pllP);

    debug_assert(found);
    debug_assert((pllN % 48) == 0);

    pllQ = pllN / 48;  // pllQ must be set so that the result is 48Mhz

    ENABLE_CLK_SOURCE();

    // wait for clk source to be ready
    do
    {
        timeoutCounter--;
    }
    while (RCC_GetFlagStatus(CLK_RDY_FLAG) != SET && timeoutCounter);

    if (timeoutCounter != 0)
    {
        RCC_PLLConfig(PLL_SRC, pllM, pllN, pllP, pllQ);

        // Do not change these settings without changing the device files
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK1Config(APB1_DIVIDER);
        RCC_PCLK2Config(APB2_DIVIDER);

        //enable the PLL
        RCC_PLLCmd(ENABLE);

        //wait for PLL to be ready
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET)
        {
            // TODO: timeout?
        }

        /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | DESIRED_FLASH_ACR_LATENCY;

        // Set sys clk to pll src.
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        //Wait for sys clk to be the PLL
        while (RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL)
        {
            //TODO: timeout? What would we do anyway?
        }
    }
    else
    {
        // TODO: clk source failed to start, need to handle error, report error???
    }

    SystemCoreClockUpdate(); // update SystemCoreClock variable
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern const void* __vector_table;
void NVIC_Configuration(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, (uint32_t)&__vector_table - INTERNAL_FLASH_START_ADDR);

    /* Configure the NVIC Preemption Priority Bits
    *
    *  group 4 means all bits are assigned as preemption priority
    *  bits. This is under advisement of the FreeRTOS notes.
    */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* The interrupts are at the same level of the kernel. If we want them to run
    *  outside the Kernel space, just change preemptionPriority (but DO NOT CALL any FreeRTOS
    *  functions for interrupts higher than the Kernel priority).
    */
}

/*******************************************************************************
* Function Name  : verify_minimum_heap
* Description    : Verify that the heap is at least configTOTAL_HEAP_SIZE bytes
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void verify_minimum_heap(void)
{
#pragma section = "HEAP"
    uint8_t* ucHeap = __section_begin("HEAP");
    extern uint8_t __heap_end__;
    size_t xTotalHeapSize = &__heap_end__ - ucHeap;

    debug_assert(xTotalHeapSize >= configTOTAL_HEAP_SIZE);
}

/*
*   WARNING  :  Until the Scheduler starts (at the end of main), all code executes using the
*               main 'C' stack. This stack is then used by the RTOS. The RTOS provides stacks
*               for each task. As such, the CSTACK is as small as it needs to be. If you
*               modify any code in main or any of the functions they call then you may need
*               to increase this number. It can be found under:
*
*               Project | Options | Linker | Config tab | Edit... | Stack/Heap Sizes.
*
*               The value is the number of 32-bit ints. You can analyze stack usage with
*               the debugger.
*/
int main(void)
{
    SystemClock_Config();
    Interrupt_Init();
    Main_EnablePeripheralClocks();

    EXTI_DeInit();

    RCC_ClearFlag();

    Instrument_Init();

    SystemAnalysis_Init();

    /* Nested Vector Interrupt Controller */
    NVIC_Configuration();

    /* Configure HCLK clock as SysTick clock source. (For FreeRTOS) */
    SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

    Init_Listener_UART();
    InternalFlashAPI_Init();
    i2cMaster_SetInitialConditions();

    PowerVariant_TransitionColdBootToLowPower();

    VariantInit();

    //verify_minimum_heap();

    /* Task creation order is important. Some tasks post messages
    * to TAP when they start so the TAP queue has to exist first.
    */
    CreateAllTasks();

    configureMPU();

    // Start Scheduler
    vTaskStartScheduler();

    return 0; // this makes Klocwork happy
}

void prvGetRegistersFromStack( uint32_t* pulFaultStackAddress )
{
    /* These are volatile to try and prevent the compiler/linker optimising them
    away as the variables never actually get used.  If the debugger won't show the
    values of the variables, make them global my moving their declaration outside
    of this function. */
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr; /* Link register. */
    volatile uint32_t pc; /* Program counter. */
    volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
    TAP_AssertPrint("Hard Fault", 0);
}
