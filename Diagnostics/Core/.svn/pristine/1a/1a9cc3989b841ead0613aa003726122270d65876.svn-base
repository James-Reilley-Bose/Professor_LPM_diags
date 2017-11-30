#ifndef __UNWIND_H__
#define __UNWIND_H__

typedef struct gp_regs_s
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t sp;
    uint32_t lr;
} gp_regs_t;

// we don't really track most of this right now, it's just here
// in case we decide to expand unwinding in the future
typedef struct cpu_state_s
{
    union
    {
        uint32_t reglist[15];
        gp_regs_t regs;
    } r;
    uint32_t pc;
    uint32_t valid_regs;
} cpu_state_t;

void unwind_stack(cpu_state_t* cpu, int search_limit, int depth_limit);

#endif // __UNWIND_H__

