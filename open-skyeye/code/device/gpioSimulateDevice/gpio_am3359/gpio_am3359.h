#ifndef __GPIO_AM3359_H
#define __GPIO_AM3359_H
#include <stdint.h>

/*******************************************************/
/*      Base addresses of GPIO memory mapped registers */
/*******************************************************/
#define SOC_GPIO_0_REGS			(0x44E07000)
#define SOC_GPIO_1_REGS			(0x4804C000)
#define SOC_GPIO_2_REGS			(0x481AC000)
#define SOC_GPIO_3_REGS			(0x481AE000)

/********************************************************/
/*       GPIO REGISTERS OFFSET                          */
/********************************************************/

#define GPIO_REVISION			(0x0)
#define GPIO_SYSCONFIG			(0x10)
#define GPIO_EOI			(0x20)
#define	GPIO_IRQSTATUS_RAW_0		(0x24)
#define	GPIO_IRQSTATUS_RAW_1		(0x28)
#define	GPIO_IRQSTATUS_0		(0x2C)
#define	GPIO_IRQSTATUS_1		(0x30)
#define	GPIO_IRQSTATUS_SET_0		(0x34)
#define	GPIO_IRQSTATUS_SET_1		(0x38)
#define	GPIO_IRQSTATUS_CLR_0		(0x3C)
#define	GPIO_IRQSTATUS_CLR_1		(0x40)
#define	GPIO_IRQWAKEN_0			(0x44)
#define	GPIO_IRQWAKEN_1			(0x48)
#define	GPIO_SYSSTATUS			(0x114)
#define	GPIO_CTRL			(0x130)
#define	GPIO_OE				(0x134)
#define	GPIO_DATAIN			(0x138)
#define	GPIO_DATAOUT			(0x13C)
#define	GPIO_LEVELDETECT0		(0x140)
#define	GPIO_LEVELDETECT1		(0x144)
#define	GPIO_RISINGDETECT		(0x148)
#define	GPIO_FALLINGDETECT		(0x14C)
#define	GPIO_DEBOUNCENABLE		(0x150)
#define	GPIO_DEBOUNCINGTIME		(0x154)
#define	GPIO_CLEARDATAOUT		(0x190)
#define	GPIO_SETDATAOUT			(0x194)

typedef struct GPIOState
{
	conf_object_t *obj;
	struct registers
	{
		uint32_t revision;
		uint32_t sysconfig;
		uint32_t eoi;
		uint32_t irqstatus_raw_0;
		uint32_t irqstatus_raw_1;
		uint32_t irqstatus_0;
		uint32_t irqstatus_1;
		uint32_t irqstatus_set_0;
		uint32_t irqstatus_set_1;
		uint32_t irqstatus_clr_0;
		uint32_t irqstatus_clr_1;
		uint32_t irqwaken_0;
		uint32_t irqwaken_1;
		uint32_t sysstatus;
		uint32_t ctrl;
		uint32_t oe;
		uint32_t datain;
		uint32_t dataout;
		uint32_t levedetect0;
		uint32_t levedetect1;
		uint32_t risingdetect;
		uint32_t fallingdetect;
		uint32_t debouncenable;
		uint32_t debouncingtime;
		uint32_t cleardataout;
		uint32_t setdataout;

		uint32_t irq_set_status0;
		uint32_t irq_set_status1;
	}regs;
	char **regs_name;
	general_signal_intf *signal;
	uint32_t interrupt_number;
}am3359_gpio_dev;

static char *regs_name[] = {
		"revision",
		"sysconfig",
		"eoi",
		"irqstatus_raw_0",
		"irqstatus_raw_1",
		"irqstatus_0",
		"irqstatus_1",
		"irqstatus_set_0",
		"irqstatus_set_1",
		"irqstatus_clr_0",
		"irqstatus_clr_1",
		"irqwaken_0",
		"irqwaken_1",
		"sysstatus",
		"ctrl",
		"oe",
		"datain",
		"dataout",
		"levedetect0",
		"levedetect1",
		"risingdetect",
		"fallingdetect",
		"debouncenable",
		"debouncingtime",
		"cleardataout",
		"setdataout",

		"irq_set_status0",
		"irq_set_status1",
		NULL,
};
#endif
