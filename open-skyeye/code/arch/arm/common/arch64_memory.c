
/*************************************************************************
	> File Name    : armv8_memory.c
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/9/22 13:40:41
************************************************************************/
#include "armdefs.h"
#include "arch64_memory.h"
#include "arch64_exception.h"
#include "arch64_helper.h"

#define WRITE_MEM_FAULT	printf("In %s.Line:%d Write address = %llx Error!Current PC = %llx\n", __func__, __LINE__, address, state->A64_PC);
#define READ_MEM_FAULT	printf("In %s.Line:%d Read address = %llx Error!Current PC = %llx\n", __func__, __LINE__, address, state->A64_PC);

void check_sp_alignment(ARMul_State * state)
{

}

inline bool_t checkAlignment(ARMdword address, int size)
{
    size = 1 << size;
    return address == ((address / size) * size);
}

void clear_vec_high(ARMul_State * state, bool_t is_q, int tt, int size)
{
    int i;

    if (!is_q)
    {
        switch (size)
        {
            case 0:
                for (i = 0; i < 8; i++)
                    state->V[tt].byte.b[8 + i] = 0;
                break;
            case 1:
                for (i = 0; i < 4; i++)
                    state->V[tt].hword.h[4 + i] = 0;
                break;
            case 2:
                for (i = 0; i < 2; i++)
                    state->V[tt].word.s[2 + i] = 0;
                break;
            case 3:
                state->V[tt].dword.d[1] = 0;
                break;
        }
    }
}

void do_vec_st(ARMul_State * state, int tt, int e, int address, int size)
{
    assert(tt < 32);
    switch (size)
    {
        case 0:
            {
                assert(e < 16);
                ARMbyte data = state->V[tt].byte.b[e];

                if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data, 1))      //mem[address, 1] = data
                    WRITE_MEM_FAULT;
            }
            break;
        case 1:
            {
                assert(e < 8);
                ARMhword data = state->V[tt].hword.h[e];

                if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data, 2))      //mem[address, 2] = data
                    WRITE_MEM_FAULT;
            }
            break;
        case 2:
            {
                assert(e < 4);
                ARMword data = state->V[tt].word.s[e];

                if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data, 4))      //mem[address, 4] = data
                    WRITE_MEM_FAULT;
            }
            break;
        case 3:
            {
                assert(e < 2);
                ARMdword data = state->V[tt].dword.d[e];

                if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data, 8))      //mem[address, 8] = data
                    WRITE_MEM_FAULT;
            }
            break;
    }
}

void do_vec_ld(ARMul_State * state, int tt, int e, int address, int size)
{
    assert(tt < 32);
    switch (size)
    {
        case 0:
            {
                assert(e < 16);
                ARMword data = 0;

                if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, &data, NULL, 1))        //data = mem[address, 1]
                    READ_MEM_FAULT;
                state->V[tt].byte.b[e] = data;
            }
            break;
        case 1:
            {
                assert(e < 8);
                ARMword data = 0;

                if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, &data, NULL, 2))        //data = mem[address, 2]
                    READ_MEM_FAULT;
                state->V[tt].hword.h[e] = data;
            }
            break;
        case 2:
            {
                assert(e < 4);
                ARMword data = 0;

                if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, &data, NULL, 4))        //data = mem[address, 4]
                    READ_MEM_FAULT;
                state->V[tt].word.s[e] = data;
            }
            break;
        case 3:
            {
                assert(e < 2);
                ARMdword data = 0;
                ARMword data_lo32, data_hi32;

                if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, &data_lo32, &data_hi32, 8))     //data = mem[address, 8]
                    READ_MEM_FAULT;

                data = data_lo32 | ((ARMdword) data_hi32 << 32);
                state->V[tt].dword.d[e] = data;
            }
            break;
    }
}

void mem_st_i64(ARMul_State * state, ARMdword address, ARMdword data, ARMword size)
{
    switch (size)
    {
        case 0:
            if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data, 1))  //mem[address, 1] = data
                WRITE_MEM_FAULT;
            break;
        case 1:
            if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data, 2))  //mem[address, 2] = data
                WRITE_MEM_FAULT;
            break;
        case 2:
            if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data, 4))  //mem[address, 4] = data
                WRITE_MEM_FAULT;
            break;
        case 3:
            if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data, 8))  //mem[address, 8] = data
                WRITE_MEM_FAULT;
            break;
        default:
            break;
    }
}

void mem_ld_i64(ARMul_State * state, ARMdword address, ARMdword * data, ARMword size)
{
    ARMword data_lo32, data_hi32;

    switch (size)
    {
        case 0:
            if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, (ARMword *) data, NULL, 1)) //data = mem[address, 1]
            {
                *data = 0;
                READ_MEM_FAULT;
            } else
                *data &= 0xff;
            break;
        case 1:
            if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, (ARMword *) data, NULL, 2)) //data = mem[address, 2]
            {
                *data = 0;
                READ_MEM_FAULT;
            } else
                *data &= 0xffff;
            break;
        case 2:
            if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, (ARMword *) data, NULL, 4)) //data = mem[address, 4]
            {
                *data = 0;
                READ_MEM_FAULT;
            } else
                *data &= 0xffffffff;
            break;
        case 3:
            if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, &data_lo32, &data_hi32, 8)) //data = mem[address, 8]
            {
                *data = 0;
                READ_MEM_FAULT;
            } else
                *data = data_lo32 | ((ARMdword) data_hi32 << 32);
            break;
        default:
            break;
    }
}

void do_fp_st_mem(ARMul_State * state, ARMdword address, ARMword Rt, ARMword size)
{
    bool_t aligned = False;

    aligned = checkAlignment(address, size);
    bit128 data;

    memset(&data, 0, sizeof (bit128));

    /*
     * if ((int64_t)address < 0)
     * {
     * address = -address;
     * address += state->stack_address;
     * }
     */

    if (aligned)
    {
        GET_VFP_DATA(size, Rt, data.lo64, data.hi64);
        if (size == 4)
        {
            if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data.lo64, 8))     //mem[address, 8] = data.lo
                WRITE_MEM_FAULT;
            address += 8;
            if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data.hi64, 8))     //mem[address, 8] = data.hi
                WRITE_MEM_FAULT;
        } else if (size == 3)
        {
            if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, data.lo64, 8))     //mem[address, 8] = data.lo
                WRITE_MEM_FAULT;
        } else if (size == 2)
        {
            if (NO_FAULT != arch64_mem_write(state, address, AccType_NORMAL, (uint32_t) data.lo64, 4))  //mem[address, 4] = data.lo
                WRITE_MEM_FAULT;
        } else
        {
            printf("In %s. Line:%d, address = 0x%llx size = %d Error!!!\n", __func__, __LINE__, address, size);
        }
    } else
    {
        printf("In %s. Line:%d, address = 0x%llx size = %d Not Aligned!!!\n", __func__, __LINE__, address, size);
        return;
    }
}

bit128 do_fp_ld_mem(ARMul_State * state, ARMdword address, ARMword size)
{
    bool_t aligned = False;

    aligned = checkAlignment(address, size);
    bit128 data;
    ARMword data_lo32, data_hi32;

    memset(&data, 0, sizeof (bit128));

    /*
     * if ((int64_t)address < 0)
     * {
     * address = -address;
     * address += state->stack_address;
     * }
     */

    if (aligned)
    {
        if (size == 4)
        {
            if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, &data_lo32, &data_hi32, 8)) //data.lo64 = mem[address, 8]
            {
                READ_MEM_FAULT;
            } else
            {
                data.lo64 = data_lo32 | ((ARMdword) data_hi32 << 32);
                address += 8;
                if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, &data_lo32, &data_hi32, 8))     //data.hi64 = mem[address, 8]
                {
                    READ_MEM_FAULT;
                } else
                    data.hi64 = data_lo32 | ((ARMdword) data_hi32 << 32);
            }
        } else if (size == 3)
        {
            if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, &data_lo32, &data_hi32, 8)) //data.lo64 = mem[address, 8]
            {
                READ_MEM_FAULT;
            } else
                data.lo64 = data_lo32 | ((ARMdword) data_hi32 << 32);
        } else if (size == 2)
        {
            if (NO_FAULT != arch64_mem_read(state, address, AccType_NORMAL, (uint32_t *) & data.lo64, NULL, 4)) //data.lo64 = mem[address, 4]
            {
                data.lo64 = 0;
                READ_MEM_FAULT;
            }
        } else
        {
            printf("In %s. Line:%d, address = 0x%llx size = %d Error!!!\n", __func__, __LINE__, address, size);
            return data;
        }
    } else
    {
        printf("In %s. Line:%d, address = 0x%llx size = %d Not Aligned!!!\n", __func__, __LINE__, address, size);
        return data;
    }

    return data;
}

void do_st_mem(ARMul_State * state, ARMdword address, ARMdword data, ARMword size, bool_t iss_valid, ARMword iss_srt,
               ARMword iss_sf, ARMword iss_ar)
{
    bool_t aligned = False;

    aligned = checkAlignment(address, size);
    /*
     * if ((int64_t)address < 0)
     * {
     * address = -address;
     * address += state->stack_address;
     * }
     */

    if (aligned)
        mem_st_i64(state, address, data, size);
    else
    {
        printf("In %s. Line:%d, address = 0x%llx size = %d Not Aligned!!!\n", __func__, __LINE__, address, size);
        return;
    }

    if (iss_valid)
    {
        ARMword syn;

        syn = syn_data_abort_with_iss(0, size, False, iss_srt, iss_sf, iss_ar, 0, 0, 0, 0, 0, False);

        set_insn_syndrome(state, syn);
    }
}

ARMdword do_ld_mem(ARMul_State * state, ARMdword address, ARMword size, bool_t is_signed, bool_t is_extended,
                   bool_t iss_valid, ARMword iss_srt, ARMword iss_sf, ARMword iss_ar)
{
    bool_t aligned = False;
    ARMdword result;

    aligned = checkAlignment(address, size);

    /*
     * if ((int64_t)address < 0 && (address > 0xffffffff))
     * {
     * address = -address;
     * address += state->stack_address;
     * }
     */

    if (aligned)
        mem_ld_i64(state, address, &result, size);
    else
    {
        printf("In %s. Line:%d, address = 0x%llx size = %d Not Aligned!!!\n", __func__, __LINE__, address, size);
        return;
    }

    if (is_extended && is_signed)
    {
        switch (size)
        {
            case 0:
                result = SignExtend64(result, 8);
                break;
            case 1:
                result = SignExtend64(result, 16);
                break;
            case 2:
                result = SignExtend64(result, 32);
                break;
            default:
                break;
        }
    }

    if (iss_valid)
    {
        ARMword syn;

        syn = syn_data_abort_with_iss(0, size, False, iss_srt, iss_sf, iss_ar, 0, 0, 0, 0, 0, False);

        set_insn_syndrome(state, syn);
    }

    return result;
}

bool_t ldst_compute_iss_sf(int size, bool_t is_signed, int opc)
{
    int op = opc & 1;
    int regsize;

    if (is_signed)
    {
        regsize = op ? 32 : 64;
    } else
    {
        regsize = size == 3 ? 64 : 32;
    }

    return regsize == 64;
}

fault_t mem_read_dword(ARMul_State * state, ARMdword address, ARMword * data_lo32, ARMword * data_hi32)
{
    fault_t ret_fault;

    ret_fault = state->memory_space_iface->read_addr64(state->memory_space, (uint32_t) address, data_lo32, 4);
    if (ret_fault != NO_FAULT)
        return ret_fault;

    ret_fault = state->memory_space_iface->read_addr64(state->memory_space, (uint32_t) address + 4, data_hi32, 4);
    if (ret_fault != NO_FAULT)
        return ret_fault;

    return ret_fault;
}

fault_t mem_read_word(ARMul_State * state, ARMdword address, ARMword * data)
{
    fault_t ret_fault;

    ret_fault = state->memory_space_iface->read_addr64(state->memory_space, (uint32_t) address, data, 4);

    return ret_fault;
}

fault_t mem_read_half(ARMul_State * state, ARMdword address, ARMword * data)
{
    fault_t ret_fault;

    ret_fault = state->memory_space_iface->read_addr64(state->memory_space, (uint32_t) address, data, 2);

    return ret_fault;
}

fault_t mem_read_byte(ARMul_State * state, ARMdword address, ARMword * data)
{
    fault_t ret_fault;

    ret_fault = state->memory_space_iface->read_addr64(state->memory_space, (uint32_t) address, data, 1);

    return ret_fault;
}

fault_t mem_write_dword(ARMul_State * state, ARMdword address, ARMdword data)
{
    fault_t ret_fault;
    uint32_t lo32_data, hi32_data;

    lo32_data = data & 0xFFFFFFFF;
    hi32_data = data >> 32;
    ret_fault = state->memory_space_iface->write_addr64(state->memory_space, (uint32_t) address, &lo32_data, 4);
    if (ret_fault != NO_FAULT)
        return ret_fault;
    ret_fault = state->memory_space_iface->write_addr64(state->memory_space, (uint32_t) address + 4, &hi32_data, 4);

    return ret_fault;
}

fault_t mem_write_word(ARMul_State * state, ARMdword address, ARMword data)
{
    fault_t ret_fault;

    ret_fault = state->memory_space_iface->write_addr64(state->memory_space, (uint32_t) address, &data, 4);

    return ret_fault;
}

fault_t mem_write_half(ARMul_State * state, ARMdword address, ARMhword data)
{
    fault_t ret_fault;

    ret_fault = state->memory_space_iface->write_addr64(state->memory_space, (uint32_t) address, &data, 2);

    return ret_fault;
}

fault_t mem_write_byte(ARMul_State * state, ARMdword address, ARMbyte data)
{
    fault_t ret_fault;

    ret_fault = state->memory_space_iface->write_addr64(state->memory_space, (uint32_t) address, &data, 1);

    return ret_fault;
}

fault_t arch64_mem_write(ARMul_State * state, ARMdword address, AccType accType, ARMdword data, int count)
{
    fault_t ret_fault;

    if (state->gic_iface)
    {
        if (address >= state->gic_interface_base_addr
            && address <= (state->gic_interface_base_addr + state->gic_interface_length))
        {
            uint32_t offset = address - state->gic_interface_base_addr; //gic offset

            ret_fault = state->gic_iface->write_gic_interface(state->gic, offset, &data, count, state->cpuID);
            return ret_fault;
        }
    }
    //MMU lookup
    //AddressDescriptor addrDesc = TranslateAddress(state, address, AccType_ATOMICRW, count);
    switch (count)
    {
        case 1:
            ret_fault = mem_write_byte(state, address, (ARMbyte) data);
            break;
        case 2:
            ret_fault = mem_write_half(state, address, (ARMhword) data);
            break;
        case 4:
            ret_fault = mem_write_word(state, address, (ARMword) data);
            break;
        case 8:
            ret_fault = mem_write_dword(state, address, data);
            break;
        default:
            break;
    }

    return ret_fault;
}

fault_t arch64_mem_read(ARMul_State * state, ARMdword address, AccType accType, ARMword * data_lo32,
                        ARMword * data_hi32, int count)
{
    fault_t ret_fault;

    if (state->gic_iface)
    {
        if (address >= state->gic_interface_base_addr
            && address <= (state->gic_interface_base_addr + state->gic_interface_length))
        {
            uint32_t offset = address - state->gic_interface_base_addr;

            ret_fault = state->gic_iface->read_gic_interface(state->gic, offset, data_lo32, count, state->cpuID);
            return ret_fault;
        }
    }

    switch (count)
    {
        case 1:
            ret_fault = mem_read_byte(state, address, data_lo32);
            break;
        case 2:
            ret_fault = mem_read_half(state, address, data_lo32);
            break;
        case 4:
            ret_fault = mem_read_word(state, address, data_lo32);
            break;
        case 8:
            ret_fault = mem_read_dword(state, address, data_lo32, data_hi32);
            break;
        default:
            break;
    }

    return ret_fault;
}
