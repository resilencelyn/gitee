/* Copyright (C) 
 * 2013 - Michael.Kang blackfin.kang@gmail.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/**
* @file c28_arch_interface.c
* @brief The definition of c28 arch
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifdef __MINGW32__
#include <windows.h>
#endif

#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "skyeye_internal.h"
#include "skyeye_types.h"
#include "skyeye_config.h"
#include "skyeye_options.h"
#include "skyeye_exec.h"
#include "skyeye_cell.h"
#include "skyeye_core.h"
#include "skyeye_mm.h"
#include "skyeye_obj.h"
#include "skyeye_checkpoint.h"
#include "c28_cpu.h"
#include "c28_decode.h"

//before Isr handler Saved registers to the stack
void register_pairs_save(c28_core_t * core, int ret_pc)
{
    c28_memory_write(core, core->sp, core->st0);
    c28_memory_write(core, core->sp + 1, core->th);

    core->sp += 2;
    c28_memory_write(core, core->sp, core->al);
    c28_memory_write(core, core->sp + 1, core->ah);

    core->sp += 2;
    c28_memory_write(core, core->sp, core->pl);
    c28_memory_write(core, core->sp + 1, core->ph);

    core->sp += 2;
    c28_memory_write(core, core->sp, core->xar[0].ar);
    c28_memory_write(core, core->sp + 1, core->xar[1].ar);

    core->sp += 2;
    c28_memory_write(core, core->sp, core->st1);
    c28_memory_write(core, core->sp + 1, core->dp);

    core->sp += 2;
    c28_memory_write(core, core->sp, core->ier);
    c28_memory_write(core, core->sp + 1, core->dbgstat);

    core->sp += 2;
    c28_memory_write(core, core->sp, ret_pc & 0xffff);
    c28_memory_write(core, core->sp + 1, (ret_pc & 0x3f0000) >> 16);

    core->sp += 2;
    return;
}

void set_status_register(c28_core_t * core)
{
    core->st1 |= 0x3;                   //Set INTM DBGM disturb Isr.
    core->st1 &= ~(0x1 << 5);           //clear LOOP
    core->st1 &= ~(0x1 << 6);           //clear EALLOW
    core->st1 &= ~(0x1 << 7);           //clear IDLESTAT

    return;
}

void check_non_periph_intr(c28_core_t * core, int cpu_irq_num)
{
    if (core->ifr != 0)
    {
        int i = cpu_irq_num;
        int intm = core->st1 & 0x1;
        int ier = core->ier & (1 << i);
        int ret_pc = 0;
        int isr_addr = 0;
        int isr_base_addr = PIE_VECTOR_BASE_ADDR;       //PieVectTable Base Address.

        if (i > 11 && i < 16)
        {
            if (ier)
            {
                core->ifr &= ~(1 << i);
                ret_pc = core->pc;
                core->sp += 1;

                register_pairs_save(core, ret_pc);
                core->ier &= ~(1 << i); //Clear ier 
                set_status_register(core);
                core->rptc_ret = core->rptc;
                isr_addr = isr_base_addr + (i + 1) * 2; //interrupt ISR handler address offset from Pie Vector Table base address (3fffc0).
                core->pc = c28_memory_read(core, isr_addr) | (c28_memory_read(core, isr_addr + 1) << 16);
                core->signal_type = 0;
            }
        }
    }
}

void check_periph_intr(c28_core_t * core, int irq_group_num, int irq_num)
{
    if (core->ifr != 0)
    {
        int i = irq_group_num;
        int j = irq_num;
        int intm = core->st1 & 0x1;
        int ier = core->ier & (1 << i);
        int ret_pc = 0;
        int isr_addr = 0;
        int isr_base_addr = PIE_VECTOR_GROUP_BASE_ADDR; //PieVectTable Group1 Base Address.

        if (i < 13)
        {
            if (ier)
            {
                core->ifr &= ~(1 << i);
                ret_pc = core->pc;
                core->sp += 1;

                register_pairs_save(core, ret_pc);
                core->rptc_ret = core->rptc;
                core->ier &= ~(1 << i); //Clear ier 
                set_status_register(core);
                isr_addr = isr_base_addr + (i * 8 + j) * 2;     //interrupt ISR handler address offset from Pie Vector Table base address (3fffc0).
                core->pc = c28_memory_read(core, isr_addr) | (c28_memory_read(core, isr_addr + 1) << 16);
                core->pie_intr_num[i][j] = 0;
                core->signal_type = 0;
            }
        }
    }
}
