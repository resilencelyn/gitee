/*
 * ppc_io.c - necessary arm definition for skyeye debugger
 * Copyright (C) 2003-2007 Skyeye Develop Group
 * for help please send mail to <skyeye-developer@lists.sf.linuxforum.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
/*
 * 04/26/2006   Michael.Kang  <blackfin.kang@gmail.com>
 */

#include <stdint.h>

//#include "skyeye_config.h"

#include "types.h"
#include "ppc_mmu.h"
#include "ppc_io.h"
#include "ppc_boot.h"
#include "ppc_exc.h"
#include "ppc_cpu.h"
#include "bank_defs.h"
#define UART_IRQ 26

/* For e500 */
//#define DEFAULT_CCSR_MEM 0xFF700000
//#define CCSR_MEM_SIZE 0x100000
//#define GET_CCSR_BASE(reg)(((reg >> 8)&0xFFF) << 20)

/* For e600 */
//#define DEFAULT_CCSR_MEM 0xF8000000
//#define CCSR_MEM_SIZE 0x100000
//#define GET_CCSR_BASE(reg)(((reg >> 8)&0xFFFF) << 16)

bool_t in_ccsr_range(e500_core_t *current_core, uint32_t p)
{
    PPC_CPU_State *cpu = (PPC_CPU_State *) current_core->cpu;

    return (p >= current_core->get_ccsr_base(cpu->ccsr))
        && (p < (current_core->get_ccsr_base(cpu->ccsr) + current_core->ccsr_size));
}

int FASTCALL ppc_read_effective_word(e500_core_t *current_core, uint32 addr, uint32 * result)
{
    PPC_CPU_State *cpu = current_core->cpu;

    uint32 p;
    int r;
    uint32_t r_data = 0;
    e500_core_t *core = cpu->core;

    r = PPC_MMU_OK;
    p = addr;

    r = ppc_effective_to_physical(current_core, addr, PPC_MMU_READ, &p);
    if (r)
        printf("%s, %d:mmu translate fail! Effective addr:%x .\n", __func__, __LINE__, addr);

    if (in_ccsr_range(current_core, p))
    {
        /*
         * skyeye_config_t *config = get_current_config();
         * if (config->mach == NULL || config->mach->mach_io_read_word == NULL) {
         * *result = Not_found_exp;
         * } else {
         * *result =
         * config->mach->mach_io_read_word(cpu,
         * (p -
         * current_core->
         * get_ccsr_base(cpu->
         * ccsr)));
         * }
         * 
         * if(*result == Not_found_exp)
         * {
         * core->memory_ccsr_iface->read(core->memory_ccsr, (p - current_core->get_ccsr_base(cpu->ccsr)), &r_data, 4);
         * *result = r_data;
         * }
         */

        core->memory_ccsr_iface->read(core->memory_ccsr, (p - current_core->get_ccsr_base(cpu->ccsr)), &r_data, 4);
        *result = r_data;
    } else
    {
        core->core_memory_iface->read(core->obj, p, &r_data, 4);
        *result = r_data;
    }

    return r;
}

int FASTCALL ppc_read_effective_half(e500_core_t *current_core, uint32 addr, uint16 * result)
{
    PPC_CPU_State *cpu = current_core->cpu;
    uint32 p;
    int r;
    uint16_t r_data;
    e500_core_t *core = current_core;

    r = PPC_MMU_OK;
    p = addr;

    r = ppc_effective_to_physical(current_core, addr, PPC_MMU_READ, &p);
    if (r)
        printf("%s, %d:mmu translate fail! Effective addr:%x .\n", __func__, __LINE__, addr);

    if (in_ccsr_range(current_core, p))
    {
        core->memory_ccsr_iface->read(core->memory_ccsr, (p - current_core->get_ccsr_base(cpu->ccsr)), result, 2);

    } else
    {
        core->core_memory_iface->read(core->obj, p, &r_data, 2);
        *(uint32 *) result = r_data;
    }

    return 0;
}

int ppc_read_effective_byte(e500_core_t *current_core, uint32 addr, uint8 * result)
{
    PPC_CPU_State *cpu = current_core->cpu;
    e500_core_t *core = current_core;
    uint32 p;
    int r;
    uint8_t r_data;

    r = PPC_MMU_OK;
    p = addr;
    r = ppc_effective_to_physical(current_core, addr, PPC_MMU_READ, &p);
    if (r)
        printf("%s, %d:mmu translate fail! Effective addr:%x .\n", __func__, __LINE__, addr);

    if (in_ccsr_range(current_core, p))
    {
        core->memory_ccsr_iface->read(core->memory_ccsr, (p - current_core->get_ccsr_base(cpu->ccsr)), result, 1);
    } else
    {
        core->core_memory_iface->read(core->obj, p, &r_data, 1);
        *(uint32_t *) result = r_data;
    }

    return r;
}

int FASTCALL ppc_write_effective_word(e500_core_t *current_core, uint32 addr, uint32 data)
{
    PPC_CPU_State *cpu = current_core->cpu;
    uint32 p;
    int r;
    e500_core_t *core = current_core;
    uint32_t w_data;
    int ret;

    if (addr & 0x3)
    {
        //printf("In %s, addr : 0x%x not align\n", __func__, addr);
        //ppc_exception for unalign
    }
    r = PPC_MMU_OK;
    p = addr;
    //printf("in %s, p=0x%x, *result=0x%x, ccsr=0x%x\n", __FUNCTION__, p, data, cpu->ccsr);
    r = ppc_effective_to_physical(current_core, addr, PPC_MMU_WRITE, &p);
    if (r)
        printf("%s, %d:mmu translate fail! Effective addr:%x .\n", __func__, __LINE__, addr);

    if (in_ccsr_range(current_core, p))
    {
        core->memory_ccsr_iface->write(core->memory_ccsr, (p - current_core->get_ccsr_base(cpu->ccsr)), &data, 4);
    } else
    {
        w_data = data;
        core->core_memory_iface->write(core->obj, p, &w_data, 4);
    }

    return r;
}

int FASTCALL ppc_write_effective_half(e500_core_t *core, uint32 addr, uint16 data)
{
    uint32 w_data = 0, p;
    int ret, r;

    r = PPC_MMU_OK;
    p = addr;

    r = ppc_effective_to_physical(core, addr, PPC_MMU_WRITE, &p);
    if (r)
        printf("%s, %d:mmu translate fail! Effective addr:%x .\n", __func__, __LINE__, addr);
    if (in_ccsr_range(core, p))
    {
        w_data = data;
        core->memory_ccsr_iface->write(core->memory_ccsr, (p - core->get_ccsr_base(core->cpu->ccsr)), &w_data, 2);

    } else
    {
        w_data = data;
        core->core_memory_iface->write(core->obj, p, &w_data, 2);
    }

    return r;
}

int FASTCALL ppc_write_effective_byte(e500_core_t *core, uint32 addr, uint8 data)
{
    uint32 p;
    int r;
    uint32_t w_data = data;
    int ret;

    r = PPC_MMU_OK;
    p = addr;

    r = ppc_effective_to_physical(core, addr, PPC_MMU_WRITE, &p);
    if (r)
        printf("%s, %d:mmu translate fail! Effective addr:%x .\n", __func__, __LINE__, addr);

    if (in_ccsr_range(core, p))
    {

        core->memory_ccsr_iface->write(core->memory_ccsr, (p - core->get_ccsr_base(core->cpu->ccsr)), &w_data, 1);
        //printf("DBG:write to CCSR,value=0x%x,offset=0x%x,pc=0x%x\n", data, offset,current_core->pc);
    } else
    {
        core->core_memory_iface->write(core->obj, p, &w_data, 1);
        //skyeye_exit(-1);
    }
    return r;
}
