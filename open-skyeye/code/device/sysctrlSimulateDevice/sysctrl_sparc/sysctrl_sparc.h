/* Copyright (C) 
 * 2011 - Michael.Kang blackfin.kang@gmail.com
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
 * @file sysctrl_sparc.h
 * @brief The definition of system controller for sparc
 * @author jiachao jiachao@digiproto.com
 * @version 78.77
 * @date 2011-12-12
 */

#ifndef __SYSCTRL_SPARC_H__
#define __SYSCTRL_SPARC_H__
#include "skyeye_system.h"
#include "sysctrl_fifo.h"

typedef struct reg_sysctrl
{
    uint32_t prd :1;
    uint32_t swr :1;
    uint32_t bto :1;
    uint32_t bp :1;
    uint32_t wdcs :1;
    uint32_t iuemmsk :1;
    uint32_t rhiuem :1;
    uint32_t iuemsk :1;
    uint32_t rhiuhe :1;
    uint32_t reserved1 :4;
    uint32_t syshemsk :1;
    uint32_t rhsyshe :1;
    uint32_t iwde :1;
    uint32_t dmae :1;
    uint32_t dpe :1;
    uint32_t dst :1;
    uint32_t ubr :1;
    uint32_t upe :1;
    uint32_t up :1;
    uint32_t usb :1;
    uint32_t ucs :1;
    uint32_t us :8;
}sysctrl_t;

typedef struct reg_testctr
{
    uint32_t cb :7;
    uint32_t reserved0 :10;
    uint32_t et :1;
    uint32_t pt :1;
    uint32_t it :1;
    uint32_t ewe :1;
    uint32_t reserved1 :11;
}testctr_t;

typedef struct reg_mcnfr
{
    uint32_t rbcs :2;
    uint32_t rbs0 :1;
    uint32_t rbr0 :3;
    uint32_t rbs1 :1;
    uint32_t rbr1 :3;
    uint32_t rsiz :3;
    uint32_t rpa :1;
    uint32_t rec :1;
    uint32_t reserved0 :1;
    uint32_t pwr :1;
    uint32_t p8 :1;
    uint32_t psiz :3;
    uint32_t reserved1 :3;
    uint32_t esiz :3;
    uint32_t epa :1;
    uint32_t eec :1;
    uint32_t eex :1;
    uint32_t reserved2 :2;
}mcnfr_t;

typedef struct reg_iocnfr
{
    uint32_t size0 :4;
    uint32_t io0 :1;
    uint32_t pa0 :1;
    uint32_t reserved0 :2;
    uint32_t size1 :4;
    uint32_t io1 :1;
    uint32_t pa1 :1;
    uint32_t reserved1 :2;
    uint32_t size2 :4;
    uint32_t io2 :1;
    uint32_t pa2 :1;
    uint32_t reserved2 :2;
    uint32_t size3 :4;
    uint32_t io3 :1;
    uint32_t pa3 :1;
    uint32_t reserved3 :2;
}iocnfr_t;

typedef struct reg_intshr
{
    uint32_t edge :5;
    uint32_t ack :3;
    uint32_t pol :5;
    uint32_t reserved :19;
}intshr_t;

typedef struct reg_intpdr
{
    uint32_t reserved0 :1;
    uint32_t hw_err :1;
    uint32_t extint0 :1;
    uint32_t extint1 :1;
    uint32_t uarta :1;
    uint32_t uartb :1;
    uint32_t me_merr :1;
    uint32_t uart_err :1;
    uint32_t dma_access_err :1;
    uint32_t dma_timeout :1;
    uint32_t extint2 :1;
    uint32_t extint3 :1;
    uint32_t gpt :1;
    uint32_t rtct :1;
    uint32_t extint4 :1;
    uint32_t wd_timeout :1;
    uint32_t reserved1 :16;
}intpdr_t;

typedef struct reg_timctr
{
    uint32_t gptr :1;
    uint32_t gptcl :1;
    uint32_t gpte :1;
    uint32_t gptsl :1;
    uint32_t phlt :1;
    uint32_t ahlt :1;
    uint32_t bhlt :1;
    uint32_t reserved0 :1;
    uint32_t rtcr :1;
    uint32_t rtccl :1;
    uint32_t rtce :1;
    uint32_t rtcsl :1;
    uint32_t reserved1 :20;
}timctr_t;

typedef struct sysctrl_reg
{

    union
    {
        sysctrl_t bit;
        uint32_t all;
    }sysctr;

    //uint32_t sysctr;
    uint32_t swrst;
    uint32_t pdown;
    //uint32_t mcnfr;
    //uint32_t iocnfr;
    union
    {
        mcnfr_t bit;
        uint32 all;
    }mcnfr;
    union
    {
        iocnfr_t bit;
        uint32 all;
    }iocnfr;

    uint32_t wscnfr;
    uint32_t aps1br;
    uint32_t aps1er;
    uint32_t aps2br;
    uint32_t aps2er;
    union
    {
        intshr_t bit;
        uint32 all;
    }intshr;
    union
    {
        intpdr_t bit;
        uint32 all;
    }intpdr;

    //uint32_t intshr;
    //uint32_t intpdr;
    uint32_t intmkr;
    uint32_t intclr;
    uint32_t intfcr;
    uint32_t wdogtr;
    uint32_t wdogst;
    int rtccr;
    uint32_t rtcsr;
    int gptcr;
    uint32_t gptsr;
    union
    {
        timctr_t bit;
        uint32 all;
    }timctr;

    //uint32_t timctr;
    uint32_t sysfsr;
    uint32_t failar;
    uint32_t errrsr;
    union
    {
        testctr_t bit;
        uint32 all;
    }testctr;

    //uint32_t testctr;
    uint32_t gpicnfr;
    uint32_t gpidatr;
}sysctrl_reg_t; 

static char *regs_name[] =
{
    "sysctr",
    "swrst",
    "pdown",
    "mcnfr",
    "iocnfr",
    "wscnfr",
    "aps1br",
    "aps1er",
    "aps2br",
    "aps2er",
    "intshr",
    "intpdr",
    "intmkr",
    "intclr",
    "intfcr",
    "wdogtr",
    "wdogst",
    "rtccr",
    "rtcsr",
    "gptcr",
    "gptsr",
    "timctr",
    "sysfsr",
    "failar",
    "errrsr",
    "testctr",
    "gpicnfr",
    "gpidatr",
    NULL
};

static uint32_t regs_offset[] =
{
    0x000,
    0x004,
    0x008,
    0x010,
    0x014,
    0x018,
    0x020,
    0x024,
    0x028,
    0x02c,
    0x044,
    0x048,
    0x04c,
    0x050,
    0x054,
    0x060,
    0x064,
    0x080,
    0x084,
    0x088,
    0x08c,
    0x098,
    0x0a0,
    0x0a4,
    0x0b0,
    0x0d0,
    0x0a8,
    0x0ac,
};

typedef struct sparc_sysctrl_device
{
    conf_object_t* obj;
    sysctrl_reg_t* regs;
    conf_object_t* signal;
    core_signal_intf* signal_iface;
    int timer_sched_id;
    time_handle_t *handle;
    int timer_cnt;
    RWLOCK_T lock;
    FIFO* recv_fifo;
    int gptcr;
}sparc_sysctrl_device;

#define O_SYSCTR     0x000      /* System Control Register */
#define O_SWRST      0x004      /* Software Reset */
#define O_PDOWN      0x008      /* Power Down */
#define O_SYSFSR     0x0a0      /* System Fault Status Register */
#define O_FAILAR     0x0a4      /* Fail Address Register */
#define O_ERRRSR     0x0b0      /* Error and Reset Status Register */
#define O_TESTCTR    0x0d0      /* Test Control Register */
#define O_MCNFR      0x010      /* Memory Configuration Register */
#define O_IOCNFR     0x014      /* I/O Configuration Register */
#define O_WSCNFR     0x018      /* Waitstate Configuration Register */
#define O_APS1BR     0x020      /* Access Protection Segment 1 Base Register */
#define O_APS1ER     0x024      /* Access Protection Segment 1 End Register */
#define O_APS2BR     0x028      /* Access Protection Segment 1 Base Register */
#define O_APS2ER     0x02c      /* Access Protection Segment 2 End Register */
#define O_INTSHR     0x044      /* Interrupt Shape Register */
#define O_INTPDR     0x048      /* Interrupt Pending Register */
#define O_INTMKR     0x04c      /* Interrupt Mask Register */
#define O_INTCLR     0x050      /* Interrupt Clear Register */
#define O_INTFCR     0x054      /* Interrupt ForceRegister */
#define O_WDOGTR     0x060      /* Watchdog Timer Register */
#define O_WDOGST     0x064      /* Watchdog Trap Door Set */
#define O_RTCCR      0x080      /* Real-time Clock Timer <Counter> Register */
#define O_RTCSR      0x084      /* Real-time Clock Timer <Scalar> Register */
#define O_GPTCR      0x088      /* General-Purpose Timer <Counter> Register */
#define O_GPTSR      0x08c      /* General-Purpose Timer <Scaler> Register */
#define O_TIMCTR     0x098      /* Timer Control Register */
#define O_GPICNFR    0x0a8      /* GPI Configuration Register */
#define O_GPIDATR    0x0ac      /* GPI Data Register */
#define O_UARTAR     0x0e0      /* UART 'A' Rx and Tx Register */
#define O_UARTBR     0x0e4      /* UART 'B' Rx and Tx Register */
#define O_UARTSR     0x0e8      /* UART Status Register */

#define SHRREG (dev->regs->intshr.all)
#define MASKREG (dev->regs->intmkr)
#define PENDREG (dev->regs->intpdr.all)
#define FORCEREG (dev->regs->intfcr)
#define CLEARREG (dev->regs->intclr)

#define RTCE (dev->regs->timctr.bit.rtce)
#define GPTE (dev->regs->timctr.bit.gpte)
#define GPTCL (dev->regs->timctr.bit.gptcl)
#define GPTSL (dev->regs->timctr.bit.gptsl)

#define CPU_INT0 2
#define CPU_INT1 3
#define CPU_INT2 10
#define CPU_INT3 11
#define CPU_INT4 14
#define GPT_INT 12
#define RTC_INT 13
#endif
