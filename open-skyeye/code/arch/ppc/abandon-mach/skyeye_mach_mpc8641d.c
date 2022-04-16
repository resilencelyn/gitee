/*
 * skyeye_mach_mpc8641d.c - mpc8641d machine simulation implementation
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
 * 01/04/2007   Michael.Kang  <blackfin.kang@gmail.com>
 */

#include <stdint.h>
#include "skyeye_config.h"
#include "skyeye_cell.h"
#include "skyeye_pref.h"

#include <ppc_cpu.h>
#include <ppc_exc.h>

#ifdef __CYGWIN__
#include <sys/time.h>
#endif

#ifdef __MINGW32__
#include <sys/time.h>
#endif

#include "skyeye_uart.h"
#define UART_IRQ 26
#include "skyeye_lcd_intf.h"
#include "skyeye_lcd_surface.h"
#include "skyeye_class.h"
#include "skyeye_iface.h"
#include "skyeye_mm.h"
#include <skyeye_sched.h>
#include "skyeye_thread.h"

typedef struct ccsr_reg_s
{
    //uint32_t ccsr; /* Configuration,control and status registers base address register */
    uint32_t altcbar;                   /* allternate configuration base address register */
    uint32_t altcar;                    /* alternate configuration attribute register */
    //uint32_t bptr; /* Boot page translation register */
} ccsr_reg_t;

/* Local bus controll register */
typedef struct lb_ctrl_s
{
    uint32 br[8];
    //uint32 or[8];
    uint32 lcrr;                        /* Clock ratio register */
    //uint32 lbcr; /* Configuration register */
} lb_ctrl_t;

typedef struct law_reg_s
{
    uint32_t lawbar[8];
    uint32_t lawar[8];
} law_reg_t;

typedef struct por_conf_s
{
    uint32_t porpllsr;
    uint32_t porbmsr;
    uint32_t porimpscr;
    uint32_t pordevsr;
    uint32_t pordbgmsr;
    uint32_t gpporcr;
} por_conf_t;

typedef struct i2c_reg_s
{
    uint32 i2cadr;
    uint32 i2ccr;
    uint32 i2csr;
    uint32 i2cdr;
    uint32 i2cfdr;
    uint32 i2cdfsrr;
} i2c_reg_t;

typedef struct debug_ctrl_s
{
    uint32 clkocr;
    uint32 ddrdllcr;
    uint32 lbdrrcr;
} debug_ctrl_t;

typedef struct ppc_dma_s
{
    uint32 satr0;
    uint32 datr0;
} mpc_dma_t;

typedef struct ddr_ctrl_s
{
    uint32 err_disable;
} ddr_ctrl_t;

typedef struct l2_reg_s
{
    uint32 l2ctl;
} l2_reg_t;

typedef struct pci_cfg_s
{
    uint32 cfg_addr;
    uint32 cfg_data;
    uint32 int_ack;
} pci_cfg_t;

typedef struct pci_atmu_s
{
    uint32 potar1;
    uint32 potear1;
    uint32 powbar1;
    uint32 reserv1;
    uint32 powar1;
} pci_atmu_t;

typedef struct ecm_s
{
    uint32 eebacr;
    //uint32 eebpcr;
} ecm_t;

/* data structure related to diu */

/* IMMRBAR is base address
 * DIU base:           IMMRBAR + 0x2100
 * Clock Control base: IMMRBAR + 0x0f00
 * IO Control base:    IMMRBAR + 0xa000
 */
#define IMMRBAR 0xA0000

typedef struct mpc8641d_io_s
{
    uint32 core_num;
    //ppc_cpm_t cpm_reg; /* Communication processor */
    ccsr_reg_t ccsr;
    law_reg_t law;                      /* Local access window */
    lb_ctrl_t lb_ctrl;                  /* Local bus controll register */
    uint32 sccr;                        /* System clock control register */
    por_conf_t por_conf;
    i2c_reg_t i2c_reg;
    debug_ctrl_t debug_ctrl;
    ddr_ctrl_t ddr_ctrl;
    mpc_dma_t dma;
    l2_reg_t l2_reg;
    pci_cfg_t pci_cfg;
    pci_atmu_t pci_atmu;

    ecm_t ecm;

    conf_object_t *uart1_obj;
    memory_space_intf *uart_mem_intf;
    int uart_signal;

    conf_object_t *diu_obj;
    memory_space_intf *diu_mem_intf;

    conf_object_t *uart2_obj;
    memory_space_intf *uart_mem_intf2;
#if 0
    diu_t diu;
#endif
} mpc8641d_io_t;

static mpc8641d_io_t mpc8641d_io;
static conf_object_t *mpc8641d_io_conf_obj;

uint32_t get_cpu_register(conf_object_t * cpu, char *name)
{
    if (!SKY_has_attribute(cpu, name))
    {
        attr_value_t attr = SKY_get_attribute(cpu, name);

        return SKY_attr_uinteger(attr);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s not has attribute %s\n", cpu->objname, name);
        return 0;
    }
}

void set_cpu_register(conf_object_t * cpu, char *name, uint32_t value)
{
    attr_value_t attr;

    attr = SKY_make_attr_uinteger(value);
    if (!SKY_has_attribute(cpu, name))
    {
        SKY_set_attribute(cpu, name, &attr);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s not has attribute %s\n", cpu->objname, name);
    }
}

static void mpc8641d_io_do_cycle(void *state)
{
    mpc8641d_io_t *io = &mpc8641d_io;

    return 0;
    /* fix me
     * if((io->pic_global.gtbcra0 >> 31) == 0){
     * if((io->pic_global.gtccra0 & 0x7FFFFFFF) > 0)
     * io->pic_global.gtccra0--;
     * else{
     * if((io->pic_global.gtccra0 & 0x7FFFFFFF) == 0){
     * // toggle 
     * io->pic_global.gtccra0 = (io->pic_global.gtccra0) ? 0x0 : 0x80000000;
     * 
     * }
     * //reload counter 
     * io->pic_global.gtccra0 |= (io->pic_global.gtbcra0 & 0x7FFFFFFF);
     * ; //generate an interrupt 
     * 
     * }
     * }
     */
}
static void *io_event(void *not_used)
{
    mpc8641d_io_do_cycle(NULL);
    return NULL;
}

extern void mpc8641d_boot_linux();
extern void mpc8641d_boot_application();

extern void mpc8641d_boot_vxworks();

static void mpc8641d_io_reset(void *state)
{
    mpc8641d_io_t *io = &mpc8641d_io;

    /* Just for convience of boot linux */
    //io->conf.ccsrbar = 0x000E0000;
    io->por_conf.porpllsr = 0x40004;
    io->lb_ctrl.lcrr = 0x80000008;
    io->i2c_reg.i2csr = 0x81;
    io->i2c_reg.i2cdfsrr = 0x10;

    /* Match os to select boot setting */
    skyeye_config_t *config = get_current_config();

    if (!strcmp(config->os->os_name, "linux"))
    {
        sky_pref_t *pref = get_skyeye_pref();

        if (pref->user_mode_sim)
            mpc8641d_boot_application();
        else
            mpc8641d_boot_linux();
    } else if (!strcmp(config->os->os_name, "vxworks"))
    {
        mpc8641d_boot_vxworks();
    }

}
static uint32_t mpc8641d_io_read_byte(void *state, uint32_t offset)
{
    e500_core_t *core = get_current_core();

    mpc8641d_io_t *io = &mpc8641d_io;

    if (offset >= 0x919C0 && offset <= 0x919E0)
    {
        switch (offset)
        {
            default:
                fprintf(stderr, "in %s, error when read CCSR.addr=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                skyeye_exit(-1);
        }
    }

    if (offset >= 0xE0000 && offset <= 0xE0020)
    {
        switch (offset)
        {
            case 0xE0000:
                return io->por_conf.porpllsr;
            default:
                {
                    e500_core_t *core = get_current_core();

                    fprintf(stderr, "in %s, error when read CCSR.addr=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                    skyeye_exit(-1);
                }
        }
    }
    PPC_CPU_State *cpu = get_current_cpu();

    switch (offset)
    {
        case 0x0:
            return cpu->ccsr;
        case 0x90C80:
            return io->sccr;
        case 0x300C:
            return io->i2c_reg.i2csr;
        case 0x8006:
            return io->pci_cfg.cfg_data;
        default:
            {
                e500_core_t *core = get_current_core();

                return Not_found_exp;
                //skyeye_exit(-1);
            }
    }

    return 0;
}
static uint32_t mpc8641d_io_read_halfword(void *state, uint32_t offset)
{
    e500_core_t *core = get_current_core();

    mpc8641d_io_t *io = &mpc8641d_io;

    //int offset = p - GET_CCSR_BASE (io->ccsr.ccsr);
    if (offset >= 0x919C0 && offset <= 0x919E0)
    {
        switch (offset)
        {
            default:
                fprintf(stderr, "in %s, error when read CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                skyeye_exit(-1);
        }
    }

    if (offset >= 0xE0000 && offset <= 0xE0020)
    {
        switch (offset)
        {
            case 0xE0000:
                return io->por_conf.porpllsr;
            default:
                fprintf(stderr, "in %s, error when read CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                skyeye_exit(-1);
        }

    }

    PPC_CPU_State *cpu = get_current_cpu();

    switch (offset)
    {
        case 0x0:
            return cpu->ccsr;
        case 0x90C80:
            return io->sccr;
        case 0x8004:
            return io->pci_cfg.cfg_data;
        case 0x8006:
            return io->pci_cfg.cfg_data;
        default:
            fprintf(stderr, "in %s, error when read CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
            //skyeye_exit(-1);
    }

    return 0;
}
static uint32_t mpc8641d_io_read_word(void *state, uint32_t offset)
{
    e500_core_t *core = get_current_core();
    mpc8641d_io_t *io = &mpc8641d_io;

    if (offset >= 0x919C0 && offset <= 0x919E0)
    {
        switch (offset)
        {
            default:
                fprintf(stderr, "in %s, error when read CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                skyeye_exit(-1);
        }
    }
    if (offset >= 0x2000 && offset <= 0x2E58)
    {
        switch (offset)
        {
            case 0x2E44:
                return io->ddr_ctrl.err_disable;
            default:
                fprintf(stderr, "in %s, error when read CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                skyeye_exit(-1);
        }
    }

    /* DMA */
    /*
     * if (offset >= 0x21100 && offset <= 0x21300) {
     * printf("%s, %d\n", __func__, __LINE__);
     * switch (offset) {
     * case 0x21110:
     * // source attribute register for DMA0 
     * return io->dma.satr0;
     * case 0x21118:
     * return io->dma.satr0;
     * default:
     * fprintf (stderr, "in %s, error when read dma.offset=0x%x,pc=0x%x\n",
     * __FUNCTION__, offset, core->pc);
     * return 0;
     * 
     * }
     * }
     */
    /* Input/Output port */
    if (offset >= 0x90D00 && offset <= 0x90D70)
    {
        switch (offset)
        {

            default:
                fprintf(stderr, "in %s, error when read IO port.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                return 0;
                //skyeye_exit(-1);
        }
    }

    if (offset >= 0xE0000 && offset <= 0xE0020)
    {
        switch (offset)
        {
            case 0xE0000:
                return io->por_conf.porpllsr;
            case 0xE000C:
                return io->por_conf.pordevsr;
            default:
                fprintf(stderr, "in %s, error when read CCSR.addr=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                skyeye_exit(-1);
        }
    }

    if (offset >= IMMRBAR && offset <= (IMMRBAR + 0xffff))
    {
        uint32 tmpoff = offset - IMMRBAR;

        /* freescale diu */
        if (tmpoff >= 0x2100 && tmpoff <= 0x21ff)
        {
            tmpoff -= 0x2100;
            uint32 value;

            io->diu_mem_intf->read(io->diu_obj, tmpoff, &value, 4);
            return value;
        }
        /* not implement */
        fprintf(stderr, "%s: ADS5121e registers candy (On 0x%x PC 0x%x)\n", __func__, offset, core->pc);
        return 0;
    }

    PPC_CPU_State *cpu = get_current_cpu();

    switch (offset)
    {
        case 0x0:
            return cpu->ccsr;
        case 0x20:
            return cpu->bptr;
        case 0x1010:
            return cpu->eebpcr;
        case 0xC28:
            return io->law.lawbar[1];
        case 0xC30:
            return io->law.lawar[1];
        case 0x90C80:
            return io->sccr;
        case 0xe0e10:
            return io->debug_ctrl.ddrdllcr;
        case 0x50D4:
            return io->lb_ctrl.lcrr;
        case 0x20000:
            return io->l2_reg.l2ctl;
        case 0x8004:
            return io->pci_cfg.cfg_data;
        default:
            fprintf(stderr, "in %s, error when read CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
            return Not_found_exp;
    }

    return 0;
}
static exception_t mpc8641d_io_write_byte(void *state, uint32_t offset, uint32_t data)
{
    PPC_CPU_State *cpu = get_current_cpu();
    e500_core_t *core = get_current_core();

    mpc8641d_io_t *io = &mpc8641d_io;

    if (offset >= 0xC08 && offset <= 0xCF0)
    {
        if (offset & 0x8)
        {
            io->law.lawbar[(offset - 0xC08) / 0x20] = data;
        } else
        {
            io->law.lawar[(offset - 0xC10) / 0x20] = data;
        }
        return No_exp;
    }
    switch (offset)
    {
        case 0x0:
            cpu->ccsr = data;
            break;
        case 0x3000:
            io->i2c_reg.i2cadr = data;
            break;
        case 0x3004:
            io->i2c_reg.i2cfdr = data;
            break;
        case 0x3008:
            io->i2c_reg.i2ccr = data;
            break;
        case 0x300C:
            io->i2c_reg.i2csr = data;
            break;
        case 0x3010:
            io->i2c_reg.i2cdr = data;
            /* set bit of MIF */
            io->i2c_reg.i2csr |= 0x02;
            break;
        case 0x3014:
            io->i2c_reg.i2cdfsrr = data;
            break;
        case 0x8004:
            io->pci_cfg.cfg_data = data;
            break;
        case 0x8005:
            io->pci_cfg.cfg_data = data;
            break;
        default:
            return Not_found_exp;
            //skyeye_exit(-1);
    }
    return No_exp;
}

static exception_t mpc8641d_io_write_halfword(void *state, uint32_t offset, uint32_t data)
{
    PPC_CPU_State *cpu = get_current_cpu();
    e500_core_t *core = get_current_core();

    mpc8641d_io_t *io = &mpc8641d_io;

    if (offset >= 0xC08 && offset <= 0xCF0)
    {
        if (offset & 0x8)
        {
            io->law.lawbar[(offset - 0xC08) / 0x20] = data;
        } else
        {
            io->law.lawar[(offset - 0xC10) / 0x20] = data;
        }
        return No_exp;
    }
    if (offset >= 0x5000 && offset <= 0x5038)
    {
        io->lb_ctrl.br[(offset - 0x5000) / 0x8] = data;
        return No_exp;
    }
    if (offset >= 0x91A00 && offset <= 0x91A3F)
    {
        int i = (0x20 & offset) >> 5;

        offset = 0x1f & offset;
        switch (offset)
        {
            default:
                fprintf(stderr, "in %s, error when read CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                return Not_found_exp;
        }
    }
    if (offset >= 0x919C0 && offset <= 0x919E0)
    {
        switch (offset)
        {
            default:
                fprintf(stderr,
                        "in %s, error when write to CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                //skyeye_exit(-1);
        }
    }
    if (offset >= 0x80000 && offset < 0x8C000)
    {
        //printf("DIU_DBG_CPM:in %s,offset=0x%x,data=0x%x,pc=0x%x\n",__FUNCTION__, offset, data, core->pc);
        return No_exp;
    }

    switch (offset)
    {
        case 0x0:
            cpu->ccsr = data;
            break;
        case 0x90C80:
            io->sccr = data;
            break;
        case 0x8004:
            io->pci_cfg.cfg_data = data;
            break;
        case 0x8006:
            io->pci_cfg.cfg_data = data;
            break;
        default:
            fprintf(stderr, "in %s, error when write to CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
            return Not_found_exp;
            //skyeye_exit(-1);
    }
    return No_exp;
}

static exception_t mpc8641d_io_write_word(void *state, uint32_t offset, uint32_t data)
{
    PPC_CPU_State *cpu = get_current_cpu();
    e500_core_t *core = get_current_core();

    mpc8641d_io_t *io = &mpc8641d_io;

    if (offset >= 0xC08 && offset <= 0xCF0)
    {
        if (offset & 0x8)
        {
            io->law.lawbar[(offset - 0xC08) / 0x20] = data;
        } else
        {
            io->law.lawar[(offset - 0xC10) / 0x20] = data;
        }
    }

    if (offset >= 0x2000 && offset <= 0x2E58)
    {
        switch (offset)
        {
            case 0x2E44:
                io->ddr_ctrl.err_disable = data;
                break;
            default:
                fprintf(stderr,
                        "in %s, error when write ddr_ctrl,offset=0x%x,pc=0x%x\n", __FUNCTION__, offset, core->pc);
                skyeye_exit(-1);
        }
    }

    if (offset >= 0x5000 && offset <= 0x50D4)
    {
        if (offset >= 0x5000 && offset <= 0x5038)
        {
            io->lb_ctrl.br[(offset - 0x5000) / 0x8] = data;
        }
#if 0
        switch (offset)
        {
            case 0x50D0:
                core->lb_ctrl.lbcr = data;
                return r;
            default:
                fprintf(stderr, "in %s, error when read CCSR.addr=0x%x, \
                                pc=0x%x\n", __FUNCTION__, addr, core->pc);

                skyeye_exit(-1);

        }
#endif

        return Not_found_exp;
    }

    /* DMA */
    /*
     * if (offset >= 0x21100 && offset <= 0x21300) {
     * printf("%s, %d\n", __func__, __LINE__);
     * switch (offset) {
     * case 0x21110:
     * source attribute register for DMA0 
     * io->dma.satr0 = data;
     * break;
     * case 0x21118:
     * io->dma.satr0 = data;
     * break;
     * default:
     * fprintf (stderr, "in %s, error when write dma.addr=0x%x,pc=0x%x\n",
     * __FUNCTION__, offset, core->pc);
     * return Not_found_exp;
     * }
     * }
     */

    /* ETH0 */
    /*
     * if (offset >= 0x24000 && offset < 0x25000) {
     * printf("%s, %d\n", __func__, __LINE__);
     * switch (offset) {
     * case 0x24014:
     * source attribute register for DMA0 
     * io->dma.satr0 = data;
     * break;
     * case 0x21118:
     * io->dma.satr0 = data;
     * break;
     * default:
     * fprintf (stderr, "in %s, error when write dma.addr=0x%x,pc=0x%x\n",
     * __FUNCTION__, offset, core->pc);
     * return Not_found_exp;
     * }
     * }
     */

        /**
	 * Interrupt Controller
	 */

    if (offset >= 0x90C00 && offset <= 0x90C7F)
    {
        switch (offset)
        {
            default:
                fprintf(stderr,
                        "in %s, error when write interrupt controller,offset=0x%x,pc=0x%x\n",
                        __FUNCTION__, offset, core->pc);
                return Not_found_exp;
        }
    }

    if (offset >= 0x919C0 && offset <= 0x919E0)
    {
        switch (offset)
        {
            default:
                return Not_found_exp;
        }
    }
    if (offset >= 0x91A00 && offset <= 0x91A3F)
    {
        int i = (0x20 & offset) >> 5;

        offset = 0x1f & offset;
        switch (offset)
        {
            default:
                return Not_found_exp;
        }
    }

    /* CPM MUX I/O */
    if (offset >= 0x91B00 && offset <= 0x91B1F)
    {
        switch (offset)
        {
            default:
                //fprintf (stderr, "in %s, error when read CCSR.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset,
                // core->pc);
                return Not_found_exp;
        }
    }
    /* Input/Output port */
    if (offset >= 0x90D00 && offset <= 0x90D70)
    {
        switch (offset)
        {
            default:
                //fprintf (stderr, "in %s, error when write io port.offset=0x%x,pc=0x%x\n", __FUNCTION__, offset,
                //       core->pc);
                return Not_found_exp;
        }
    }

    if (offset >= 0x8C00 && offset <= 0x8DFC)
    {
        switch (offset)
        {
            case 0x8C20:
                io->pci_atmu.potar1 = data;
                return No_exp;
            case 0x8C24:
                io->pci_atmu.potear1 = data;
                return No_exp;
            case 0x8C28:
                io->pci_atmu.powbar1 = data;
                return No_exp;
            case 0x8C2C:
                io->pci_atmu.reserv1 = data;
                return No_exp;
            case 0x8C30:
                io->pci_atmu.powar1 = data;
                return No_exp;
            default:
                //fprintf (stderr,
                //       "in %s, error when write to PCI_ATMU.offset=0x%x,pc=0x%x\n",
                //       __FUNCTION__, offset, core->pc);
                //skyeye_exit(-1);
                return Not_found_exp;
        }
    }
    if (offset >= IMMRBAR && offset <= (IMMRBAR + 0xffff))
    {
        uint32 tmpoff = offset - IMMRBAR;

        /* freescale diu */
        if (tmpoff >= 0x2100 && tmpoff <= 0x21ff)
        {
            tmpoff -= 0x2100;
            io->diu_mem_intf->write(io->diu_obj, tmpoff, &data, 4);
        }
#if 0
        /* clock module */
        if (tmpoff >= 0xf00 && tmpoff <= 0xfff)
        {
            tmpoff -= 0xf00;
            switch (tmpoff)
            {
                case 0x08:
                    io->diu.candy.sccr2 = data;
                    return No_exp;
                case 0x0c:
                    io->diu.candy.scfr1 = data;
                    return No_exp;
                default:
                    fprintf(stderr, "%s: ADS5121e registers candy (On 0x%x PC 0x%x)\n", __func__, offset, core->pc);
                    return Not_found_exp;
            }
        }
        /* io control */
        if (tmpoff >= 0xa000 && tmpoff <= 0xafff)
        {
            io->diu.candy.ioctrl[tmpoff - 0xa000] = data;
            return No_exp;
        }
#endif
        /* not implement */
        fprintf(stderr, "%s: ADS5121e registers candy (On 0x%x PC 0x%x)\n", __func__, offset, core->pc);
        return Not_found_exp;
    }

    switch (offset)
    {
        case 0x0:
            cpu->ccsr = data;
            break;
        case 0x20:
            //io->ccsr.bptr = data;
            cpu->bptr = data;
            break;
        case 0x1010:
            //io->ecm.eebpcr = data;
            cpu->eebpcr = data;
            if (data & 0x2000000)       /* enable CPU1 */
                /* The address of e600 reset vector */
                cpu->core[1].pc = PPC_EXC_SYS_RESET;
            break;
        case 0x90C80:
            io->sccr = data;
            break;
        case 0x50D4:
            io->lb_ctrl.lcrr = data;
            break;
        case 0x3008:
            io->i2c_reg.i2ccr = data;
            break;
        case 0xe0e10:
            io->debug_ctrl.ddrdllcr = data;
            break;
        case 0x8000:
            io->pci_cfg.cfg_addr = data;
            break;
        case 0x8004:
            io->pci_cfg.cfg_data = data;
            break;
        default:
            return Not_found_exp;
    }

    return No_exp;
}
static void mpc8641d_update_int(void *state)
{
}

static void mpc8641d_set_intr(uint32_t irq)
{

}

int uart_raise_signal(conf_object_t * obj, int line)
{
    mpc8641d_io.uart_signal = 1;
    return 0;
}
int uart_lower_signal(conf_object_t * obj, int line)
{
    mpc8641d_io.uart_signal = 0;
    return 0;
}
void mpc8641d_mach_init(void *arch_instance, machine_config_t * this_mach)
{
    //PPC_CPU_State *cpu = (PPC_CPU_State *) state;
#if 1
    this_mach->mach_io_do_cycle = mpc8641d_io_do_cycle;
    this_mach->mach_io_reset = mpc8641d_io_reset;
    this_mach->mach_io_read_byte = mpc8641d_io_read_byte;
    this_mach->mach_io_write_byte = mpc8641d_io_write_byte;
    this_mach->mach_io_read_halfword = mpc8641d_io_read_halfword;
    this_mach->mach_io_write_halfword = mpc8641d_io_write_halfword;
    this_mach->mach_io_read_word = mpc8641d_io_read_word;
    this_mach->mach_io_write_word = mpc8641d_io_write_word;
    this_mach->mach_update_int = mpc8641d_update_int;
    this_mach->mach_set_intr = mpc8641d_set_intr;
#endif
    memset(&mpc8641d_io, 0, sizeof (mpc8641d_io_t));

    //conf_object_t *diu = pre_conf_obj("diu_ads5121e_0", "diu_ads5121e");
    //mpc8641d_io.diu_mem_intf = (memory_space_intf*)SKY_get_iface(diu, MEMORY_SPACE_INTF_NAME);

    attr_value_t *attr;

    //int  pthread_id;
    //create_thread_scheduler(1, Periodic_sched, io_event, NULL, &pthread_id);

    /* some initialization copyied from reset */
    mpc8641d_io_t *io = &mpc8641d_io;

    io->por_conf.porpllsr = 0x40004;
    io->lb_ctrl.lcrr = 0x80000008;
    io->i2c_reg.i2csr = 0x81;
    io->i2c_reg.i2cdfsrr = 0x10;

}
typedef struct skyeye_mach_mpc8641d_device
{
    conf_object_t *obj;
    common_machine_attr_t attr;
} skyeye_mach_mpc8641d_device;

static conf_object_t *skyeye_mach_mpc8641d_new(char *obj_name)
{
    skyeye_mach_mpc8641d_device *mach = skyeye_mm_zero(sizeof (skyeye_mach_mpc8641d_device));

    mach->obj = new_conf_object(obj_name, mach);
    mach->attr.core_num = 0;

#if 1                                   /* Deprecated */
    mpc8641d_io_conf_obj = mach->obj;
    skyeye_config_t *config = get_current_config();
    machine_config_t *mach_config = skyeye_mm(sizeof (machine_config_t));

    config->mach = mach_config;

    mpc8641d_mach_init(NULL, config->mach);

#endif

    return mach->obj;
}

int skyeye_mach_mpc8641d_update_boot_info(conf_object_t * obj)
{
    int i;
    skyeye_mach_mpc8641d_device *mach = obj->obj;

    /* Match os to select boot setting */
    mpc8641d_boot_vxworks();

    //mpc8641d_boot_linux(); 
    //mpc8641d_boot_application();
    return 0;
}

static void mpc8641d_register_interface(conf_class_t * clss)
{
    static const skyeye_machine_intf mach_intf = {
        .update_boot_info = skyeye_mach_mpc8641d_update_boot_info,
    };
    SKY_register_iface(clss, MACHINE_INTF_NAME, &mach_intf);

    static general_signal_intf signal_intf = {
        .raise_signal = uart_raise_signal,
        .lower_signal = uart_lower_signal,
    };
    SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

    return;
}

static exception_t skyeye_mach_mpc8641d_free(conf_object_t * dev)
{
    skyeye_mach_mpc8641d_device *mach = dev->obj;

    skyeye_free(mach->obj);
    skyeye_free(mach);
    skyeye_config_t *config = get_current_config();

    skyeye_free(config->mach);

    return No_exp;
}

static exception_t skyeye_mach_mpc8641d_reset(conf_object_t * obj)
{
    return No_exp;
}

void init_skyeye_mach_mpc8641d()
{
    static skyeye_class_t class_data = {
        .class_name = "mpc8641d_mach",
        .class_desc = "skyeye_mach_mpc8641d",
        .new_instance = skyeye_mach_mpc8641d_new,
        .free_instance = skyeye_mach_mpc8641d_free,
        .reset_instance = skyeye_mach_mpc8641d_reset,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *clss = SKY_register_soc_class(class_data.class_name, &class_data);

    mpc8641d_register_interface(clss);
}
