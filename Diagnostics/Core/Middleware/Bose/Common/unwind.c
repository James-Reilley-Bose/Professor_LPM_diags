#include <stdio.h>
#include "project.h"
#include "etap.h"
#include "TapParserAPI.h"
#include "BoseUSART_API.h"
#include "versionlib.h"
#include "unwind.h"
#include "serialTapListener.h"

#define IS_THUMB_ADD_SP(x) (((x) & 0xff80) == 0xb000)
#define IS_THUMB_SUB_SP(x) (((x) & 0xff80) == 0xb080)
#define IS_THUMB_PUSHM(x) (((x) & 0xfe00) == 0xb400)
#define IS_THUMB_POPM(x) (((x) & 0xfe00) == 0xbc00)
#define IS_THUMB_BX_LR(x) ((x) == 0x4770)
#define IS_THUMB_POP_PC(x) (((x) & 0xff00) == 0xbd00)

#define IS_ARM_PUSHM(x) (((x) & 0xffff0000) == 0xe92d0000)

static char unwind_print_buf[64];
#define unwind_printf(x...) \
  snprintf(unwind_print_buf, sizeof(unwind_print_buf), x); \
  Listener_PutString(unwind_print_buf);


// dumb heuristic unwind; we assume that push marks the start of a function
void unwind_stack(cpu_state_t* cpu, int search_limit, int depth_limit)
{
    uint16_t* insn16_p, insn16;
    uint32_t insn32;
    int insn_len;
    int i, j;
    int insns = 0;
    int depth = 0;
    uint32_t func;

    unwind_printf("Unwinding stack\r\n");
    unwind_printf("%s\r\n", VersionGetVersion());

    while ( 1 )
    {
        insn16_p = (uint16_t*)cpu->pc;
        insn16 = *insn16_p;

        // 16 or 32 bit instruction?
        switch ( insn16 >> 11 )
        {
            case 0x1d:
            case 0x1e:
            case 0x1f:
                insn_len = 2;
                break;
            default:
                insn_len = 1;
                break;

        }

        if ( insn_len == 2)
        {
            insn32 = (insn16_p[0] << 16) | insn16_p[1];

            if (IS_ARM_PUSHM(insn32))
            {
                insns = 0;
                for (i = 12, cpu->valid_regs = 0; i >= 0; i--)
                {
                    if (insn32 & (1 << i))
                    {
                        cpu->r.reglist[i] = *(uint32_t*)(cpu->r.regs.sp);
                        cpu->valid_regs |= (1 << i);
                        cpu->r.regs.sp += 4;
                    }
                }
                if (insn32 & (1 << 14))
                {
                    func = cpu->pc;
                    cpu->pc = ((*(uint32_t*)(cpu->r.regs.sp)) & ~0x1) - 2;
                    cpu->r.regs.sp += 4;
                    unwind_printf(
                        "%d function 0x%08x, return to 0x%08x\r\n    ",
                        depth, func, cpu->pc);
                    for (i = 0, j = 0; i <= 12; i++)
                    {
                        if (cpu->valid_regs & (1 << i))
                        {
                            unwind_printf("r%d: 0x%08x ", i, cpu->r.reglist[i]);
                            if ((++j % 4) == 0)
                            {
                                unwind_printf("\r\n    ");
                            }
                        }
                    }
                    unwind_printf("lr: 0x%08x ", cpu->pc);
                    depth++;
                }
                else
                {
                    cpu->pc -= 4;
                }
            }
            else
            {
                insns++;
                cpu->pc -= 4;
            }
        }
        else
        {
            // push usually marks the start of a function
            // the function may not have a stack frame though
            // in this case, we may end up walking back to the tail end
            // of a previous function, in which case we should see something
            // like "bx lr"
            // this isn't foolproof either, since there may be a literal
            // pool between functions
            if (IS_THUMB_BX_LR(insn16) || IS_THUMB_POP_PC(insn16))
            {
                unwind_printf("%d function 0x%08x, return to 0x%08x\r\n",
                              depth++, cpu->pc, cpu->r.regs.lr);
                insns = 0;
                cpu->pc = (cpu->r.regs.lr & ~0x1) - 2;
            }
            else if (IS_THUMB_SUB_SP(insn16))
            {
                insns = 0;
                cpu->pc -= 2;
                cpu->r.regs.sp += ((insn16 & 0x7f) << 2);
            }
            else if (IS_THUMB_PUSHM(insn16))
            {
                insns = 0;
                for (i = 7, cpu->valid_regs = 0; i >= 0; i--)
                {
                    if (insn16 & (1 << i))
                    {
                        cpu->r.reglist[i] = *(uint32_t*)(cpu->r.regs.sp);
                        cpu->valid_regs |= (1 << i);
                        cpu->r.regs.sp += 4;
                    }
                }
                if (insn16 & (1 << 8))
                {
                    func = cpu->pc;
                    cpu->pc = ((*(uint32_t*)(cpu->r.regs.sp)) & ~0x1) - 2;
                    cpu->r.regs.sp += 4;
                    unwind_printf(
                        "%d function 0x%08x, return to 0x%08x\r\n    ",
                        depth, func, cpu->pc);
                    for (i = 0, j = 0; i <= 7; i++)
                    {
                        if (cpu->valid_regs & (1 << i))
                        {
                            unwind_printf("r%d: 0x%08x ", i, cpu->r.reglist[i]);
                            if ((++j % 4) == 0)
                            {
                                unwind_printf("\r\n    ");
                            }
                        }
                    }
                    unwind_printf("lr: 0x%08x\r\n", cpu->pc);
                    depth++;
                }
                else
                {
                    cpu->pc -= 2;
                }
            }
            // the rest aren't really needed, just here in case we decide
            // to expand unwinding in the future
            else if (IS_THUMB_ADD_SP(insn16))
            {
                insns = 0;
                cpu->pc -= 2;
                cpu->r.regs.sp -= ((insn16 & 0x7f) << 2);
            }
            else if (IS_THUMB_POPM(insn16))
            {
                insns = 0;
                cpu->pc -= 2;
                for (i = 0; i < 8; i++)
                {
                    if (insn16 & (1 << i))
                    {
                        cpu->r.regs.sp -= 4;
                    }
                }
                if (insn16 & (1 << 8))
                {
                    cpu->r.regs.sp -= 4;
                }
            }
            else
            {
                insns++;
                cpu->pc -= 2;
            }
        }

        if (insns == search_limit)
        {
            unwind_printf("hit search limit!\r\n");
            break;
        }
        if (depth == depth_limit)
        {
            unwind_printf("hit depth limit!\r\n");
            break;
        }
    }

    unwind_printf("done!\r\n");
    debug_assert ( 0 );
}

