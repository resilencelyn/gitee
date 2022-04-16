#include "c28_cpu.h"
#include "c28_decode.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "skyeye_types.h"
#include "skyeye_bus.h"
#include "skyeye_uart_ops.h"
#include "skyeye_interface.h"

#define DEBUG
#include <skyeye_log.h>

#ifdef DBG
#undef DBG
#define DBG
#endif

int check_insn_address_mode(c28_core_t * core, uint32_t insn, opcode_fields_t * fields, uint32_t loc32)
{
    int mode_1 = BITS(0, 2);
    int mode_2 = BITS(3, 5);
    int mode_3 = BITS(6, 7);
    int mode_4 = BITS(0, 3);
    int mode_5 = BITS(4, 7);
    int bit6 = BITS(0, 5);
    int loc = 0;

    if (mode_5 == 0xa)
    {
        loc = check_register_mode(core, insn);
        return loc;
    }
    if (mode_3 == 0x0)
    {
        loc = DP;
        core->data_addr = (core->dp << 6 | bit6) & 0x3fffff;
        return loc;
    } else if (mode_3 == 0x1)
    {
        loc = SP;
        core->sp_count = core->sp - bit6;
        return loc;
    } else if (mode_3 == 0x3)
    {
        loc = mode_1;
        SET_AR(core->st1, mode_1);
        core->data_addr = core->xar[mode_1].xar + mode_2;
        return loc;
    } else if (mode_3 == 0x2)
    {
        switch (mode_2)
        {
            case 0:
                loc = check_xar_increment_mode(core, insn, loc32);
                break;
            case 1:
                loc = check_xar_decrement_mode(core, insn, loc32);
                break;
            case 2:
                loc = check_xar_add_ar_mode(core, insn);
                break;
            case 3:
                loc = check_xar_add_ar_mode(core, insn);
                break;
            case 7:
                // *++, *--, *0++, *0--
                if (mode_1 == 5)
                {
                    loc = SP;
                    core->sp_count = core->sp;
                    if (loc32)
                    {
                        core->sp += 2;
                    } else
                    {
                        core->sp += 1;
                    }
                } else if (mode_1 == 6)
                {
                    loc = SP;
                    if (loc32)
                    {
                        core->sp -= 2;
                    } else
                    {
                        core->sp -= 1;
                    }
                    core->sp_count = core->sp;
                } else
                {
                    skyeye_log(Error_log, __func__, "not implement *++ or *--\n");
                }
                break;
            default:
                break;
        }
    }
    return loc;
}

int check_xar_increment_mode(c28_core_t * core, uint32_t insn, uint32_t loc32)
{
    int mode_1 = BITS(0, 2);
    int loc;

    SET_AR(core->st1, mode_1);
    if (loc32)
    {
        core->data_addr = core->xar[mode_1].xar;
        core->xar[mode_1].xar += 2;
    } else
    {
        core->data_addr = core->xar[mode_1].xar;
        core->xar[mode_1].xar += 1;
    }
    loc = mode_1;
    return loc;
}

int check_xar_decrement_mode(c28_core_t * core, uint32_t insn, uint32_t loc32)
{
    int mode_1 = BITS(0, 2);
    int loc;

    SET_AR(core->st1, mode_1);
    if (loc32)
    {
        core->xar[mode_1].xar -= 2;
        core->data_addr = core->xar[mode_1].xar;
    } else
    {
        core->xar[mode_1].xar -= 1;
        core->data_addr = core->xar[mode_1].xar;
    }
    loc = mode_1;
    return loc;
}

int check_xar_add_ar_mode(c28_core_t * core, uint32_t insn)
{
    int mode_1 = BITS(0, 2);
    int mode_2 = BITS(3, 5);
    int loc;

    SET_AR(core->st1, mode_1);
    if (mode_2 == 2)
    {
        core->data_addr = core->xar[mode_1].xar + core->xar[0].ar;
    } else if (mode_2 == 3)
    {
        core->data_addr = core->xar[mode_1].xar + core->xar[1].ar;
    }
    loc = mode_1;

    return loc;
}

int check_register_mode(c28_core_t * core, uint32_t insn)
{
    int mode_4 = BITS(0, 3);
    int loc;

    loc = mode_4;

    return loc;
}

int *get_register(c28_core_t * core, uint32_t insn, uint32_t loc)
{
    int mode_3 = BITS(6, 7);
    int mode_1 = BITS(3, 5);

    switch (loc)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            if ((mode_3 == 0x3) || ((mode_3 == 0x2) && (mode_1 < 0x4)))
            {
                return &core->xar_addr;
            } else
                return &core->xar[loc].xar;
        case 8:
            return &core->ah;
        case 9:
            return &core->al;
        case 10:
            return &core->ph;
        case 11:
            return &core->pl;
        case 12:
            return &core->xt;
        case 13:
            if ((mode_3 == 0x1) || ((mode_3 == 0x2) && (mode_1 == 0x7)))
                return &core->sp_addr;
            else
                return &core->sp;
        case 14:
            if (mode_3 == 0x0)
            {
                return &core->dp_addr;
            } else
                return &core->dp;
        default:
            break;
    }
}

void check_insn_flag_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int cond = BITS(8, 11);
    int offset = BITS(0, 7);
    int c = core->st0 & (0x1 << 3);
    int z = core->st0 & (0x1 << 4);
    int n = core->st0 & (0x1 << 5);
    int v = core->st0 & (0x1 << 6);
    int tc = core->st0 & (0x1 << 2);

    if (offset & (0x1 << 7))
    {
        offset = -(0xff - offset + 1);
    }

    switch (cond)
    {
        case NEQ:
            if (z == 0)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case EQ:
            if (z)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case GT:
            if (z == 0 && n == 0)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case GEQ:
            if (n == 0)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case LT:
            if (n)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case LEQ:
            if (z || n)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case HI:
            if (z == 0 && c)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case HIS_C:
            if (c)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case LO_NC:
            if (c == 0)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case LOS:
            if (z || (c == 0))
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case NOV:
            if (v == 0)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case OV:
            if (v)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case NTC:
            if (tc == 0)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case TC_C:
            if (tc)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case NBIO:
            if (offset == 0)
            {
                core->pc += offset;
                core->insn_length = offset;
            } else
            {
                core->pc += 1;
                core->insn_length = 1;
            }
            break;
        case UNC:
            core->pc += offset;
            core->insn_length = offset;
            break;
        default:
            break;
    }
    return;
}
int check_float_insn_flag_cond(c28_core_t * core, int cond)
{
    int zf = core->stf & ZF;
    int nf = core->stf & NF;
    int tf = core->stf & TEST_FLAG;
    int lu = core->stf & LUF;
    int lv = core->stf & LVF;

    switch (cond)
    {
        case NEQ:
            if (zf == 0)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case EQ:
            if (zf)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case GT:
            if (zf == 0 && nf == 0)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case GEQ:
            if (nf == 0)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case LT:
            if (nf)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case LEQ:
            if (zf && nf)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case TF:
            if (tf)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case NTF:
            if (tf == 0)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case LU:
            if (lu)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case LV:
            if (lv)
            {
                return TRUE;
            } else
            {
                return FALSE;
            }
        case UNCD:
        case UNCF:
            return TRUE;
        default:
            printf("In %s, cond :%x is error.\n", __func__, cond);
            return FALSE;
    }
}
void check_insn_status_cond(c28_core_t * core, uint32_t insn, opcode_fields_t * fields)
{
    int cond = fields->cond0_7;
    int op = fields->op8_15;

    switch (cond)
    {
        case SXM:
            if (op == 0x3b)
            {
                SET(core->st0, SXM_BIT);
            } else if (op == 0x29)
                CLEAR(core->st0, SXM_BIT);
            break;
        case OVM:
            if (op == 0x3b)
            {
                SET(core->st0, OVM_BIT);
            } else if (op == 0x29)
                CLEAR(core->st0, OVM_BIT);
            break;
        case TC:
            if (op == 0x3b)
            {
                SET(core->st0, TC_BIT);
            } else if (op == 0x29)
                CLEAR(core->st0, TC_BIT);
            break;
        case C:
            if (op == 0x3b)
            {
                SET(core->st0, C_BIT);
            } else if (op == 0x29)
                CLEAR(core->st0, C_BIT);
            break;
        case INTM:
            if (op == 0x3b)
            {
                SET(core->st1, INTM_BIT);
            } else if (op == 0x29)
                CLEAR(core->st1, INTM_BIT);
            break;
        case DBGM:
            if (op == 0x3b)
            {
                SET(core->st1, DBGM_BIT);
            } else if (op == 0x29)
                CLEAR(core->st1, DBGM_BIT);
            break;
        case PAGE0:
            if (op == 0x3b)
            {
                SET(core->st1, PAGE0_BIT);
            } else if (op == 0x29)
                CLEAR(core->st1, PAGE0_BIT);
            break;
        case VMAP:
            if (op == 0x3b)
            {
                SET(core->st1, VMAP_BIT);
            } else if (op == 0x29)
                CLEAR(core->st1, VMAP_BIT);
            break;
        case 0x30:
            if (op == 0x3b)
            {
                SET(core->st1, INTM_BIT);
                SET(core->st1, DBGM_BIT);
            } else if (op == 0x29)
                CLEAR(core->st1, PAGE0_BIT);
            break;
        case 0x42:
            if (op == 0x3b)
            {
                SET(core->st1, OVM_BIT);
                SET(core->st1, PAGE0_BIT);
            } else if (op == 0x29)
                CLEAR(core->st1, OVM_BIT);
            CLEAR(core->st1, PAGE0_BIT);
            break;
        default:
            skyeye_log(Error_log, __func__, "decode error instruction for %x at pc = %x\n", insn, core->pc);
    }
    return;
}

void judge_flags_modes(c28_core_t * core, uint32_t insn)
{
    /* Zero conditions */
    if (core->acc == 0)
    {
        SET(core->st0, Z_BIT);
    } else
        CLEAR(core->st0, Z_BIT);

    /* Negative conditions */
    if (core->acc & (0x1 << 31))
    {
        SET(core->st0, N_BIT);
    } else
        CLEAR(core->st0, N_BIT);
    /* Overflow conditions */
    if (core->acc == NEGATIVE_OVERFLOW)
        SET(core->st0, V_BIT);
    /* OVC conditions */
    if (core->st0 & OVM == 0)
    {
        int ovc = core->st0 & 0xfc00;

        /* positive overflow */
        if ((core->st0 & N == 0) && (core->acc == NEGATIVE_OVERFLOW))
        {
            ovc += 1;
            if (ovc == 31)
            {
                ovc = -32;
            }
            core->st0 |= (ovc << 10);
        }
        /* negative overflow */
        if ((core->st0 & N == 1) && (core->acc == POSITIVE_OVERFLOW))
            ovc -= 1;
        if (ovc == -32)
        {
            ovc = 31;
        }
        core->st0 |= (ovc << 10);
    } else
    {
        /* OVM conditions */
        if ((core->st0 & N == 0) && (core->acc & NEGATIVE_OVERFLOW))
        {
            core->acc = POSITIVE_OVERFLOW;
        }
        if ((core->st0 & N == 1) && (core->acc & POSITIVE_OVERFLOW))
        {
            core->acc = NEGATIVE_OVERFLOW;
        }
    }
    return;
}

int fpu_get_single(c28_core_t * core, uint16_t Ra)
{
    uint32_t val = 0;

    val = core->fp_regs[Ra].Ra;

    return val;
}

void fpu_put_single_bit16(c28_core_t * core, uint32_t val, uint16_t Ra)
{

    core->fp_regs[Ra].Ral = val & 0xffff;
}

void fpu_put_single_bit32(c28_core_t * core, uint32_t val, uint16_t Ra)
{

    core->fp_regs[Ra].Ra = val;
}

int32_t sign_extend(int32_t a)
{
    int count;

    count = a & 0x1f;
    if (!count)
        return 0;
    return ((unsigned) 0x80000000 >> count - 1) - 1 ^ 0xffffffff;
}

int c28_memory_read(c28_core_t * core, uint32_t addr)
{
    uint32_t data = 0;
    memory_space_intf *io_memory = core->c28_memory_intf;

    if (io_memory->read(core->obj, addr, &data, 2) != 0)
    {
        skyeye_log(Error_log, __func__, "Read from physical address 0x%x error at pc:%x!\n", addr, core->pc);
    }
    return data & 0xffff;
}

void c28_memory_write(c28_core_t * core, uint32_t addr, uint16_t buffer)
{
    memory_space_intf *io_memory = core->c28_memory_intf;

    if (io_memory->write(core->obj, addr, &buffer, 2) != 0)
    {
        skyeye_log(Error_log, __func__, "Write 0x%x to physical address 0x%x error at pc:%x!\n", buffer, addr,
                   core->pc);
    }
    return;
}

int c28_memory_read_byte_hl(c28_core_t * core, uint32_t addr, int hl_flag)
{
    uint8_t data = 0;
    memory_space_intf *io_memory = core->c28_memory_intf;

    if (io_memory->read_hl(core->obj, addr, &data, 1, hl_flag) != 0)
    {
        skyeye_log(Error_log, __func__, "Read from physical address 0x%x error at pc:%x!\n", addr, core->pc);
    }
    return data & 0xff;
}

void c28_memory_write_byte_hl(c28_core_t * core, uint32_t addr, uint8_t buffer, int hl_flag)
{
    memory_space_intf *io_memory = core->c28_memory_intf;

    if (io_memory->write_hl(core->obj, addr, &buffer, 1, hl_flag) != 0)
    {
        skyeye_log(Error_log, __func__, "Write 0x%x to physical address 0x%x error at pc:%x!\n", buffer, addr,
                   core->pc);
    }
    return;
}
