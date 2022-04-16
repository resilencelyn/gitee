/*************************************************************************
	> TriCore simulation for SkyEye: main translation routines
	> File Name    : cpu.c
	> Author       : jiachao
	> Mail         : jiachao@digiproto.com
	> Created Time : 2021/10/26 09:42:09
************************************************************************/
#include "cpu.h"
#include "helper.h"

void tricore_cpu_dump_state(conf_object_t* opaque, FILE *f)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    uint32_t psw;
    int i;

    psw = psw_read(core);

    fprintf(f, "PC: %08x" , core->PC);
    fprintf(f, " OPCODE: %08x" , core->opcode);
    fprintf(f, " PSW: %08x", psw);
    fprintf(f, " ICR: %08x", core->ICR);
    fprintf(f, "\nPCXI: %08x", core->PCXI);
    fprintf(f, " SYSCON: %08x", core->SYSCON);
    fprintf(f, " FCX: %08x", core->FCX);
    fprintf(f, " LCX: %08x", core->LCX);

    for (i = 0; i < 16; i++)
    {
        if ((i & 3) == 0)
        {
            fprintf(f, "\nGPR A%02d:", i);
        }
        fprintf(f, " %08x", core->gpr_a[i]);
    }
    for (i = 0; i < 16; i++)
    {
        if ((i & 3) == 0)
        {
            fprintf(f, "\nGPR D%02d:", i);
        }
        fprintf(f, " %08x", core->gpr_d[i]);
    }
    fprintf(f, "\n\n");
}

uint32_t tricore_step_once(conf_object_t* opaque)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    uint32_t cycle_num = core->insn_num;
    uint32_t addr = core->PC;
    uint32_t opcode = 0;

    if (core->memory_space.iface != NULL)
    {
        exception_t ret = core->memory_space.iface->read(core->memory_space.obj, addr, &opcode, 4);
        core->opcode = opcode;
        if (ret != No_exp)
        {
            skyeye_log(Error_log, __FUNCTION__, "Read Instruction Failed From PC: 0x%08x!\n", addr);
            SIM_stop(NULL);
        }
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "Get memory_space_intf Failed!Please Check the iface is Connects?\n");
        SIM_stop(NULL);
    }

    /* dump cpu registers to file */
    if (core->dbgEnable)
    {
        core->dump_state(opaque, core->dbg_fp);
    }

    decode_opc_and_exec(opaque, opcode);
    core->PC = core->next_pc;
    core->insn_num ++;

    check_breakpoint(&(core->bp_manager), core->PC, core->insn_num);

    if (core->stop_step != 0 && core->insn_num != 0
            && core->stop_step == core->insn_num)
    {
        SIM_stop(NULL);
        core->stop_step = 0;
    }

    return (core->insn_num - cycle_num);
}
