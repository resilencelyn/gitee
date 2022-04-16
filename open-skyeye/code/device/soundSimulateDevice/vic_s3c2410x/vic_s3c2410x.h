#ifndef __VIC_S3C2410X_H__
#define __VIC_S3C2410X_H__

#define SRCPND		(0x0)
#define INTMOD		(0x4)
#define INTMSK		(0x8)
#define PRIORITY	(0xc)
#define	INTPND		(0x10)
#define INTOFFSET	(0x14)
#define SUBSRCPND	(0x18)
#define INTSUBMSK	(0x1c)

typedef struct s3c2410x_device {
	conf_object_t *obj;
	struct registers{
		uint32_t srcpnd;
		uint32_t intmod;
		uint32_t intmsk;
		uint32_t priority;
		uint32_t intpnd;
		uint32_t intoffset;
		uint32_t subsrcpnd;
		uint32_t intsubmsk;
	}regs;
	core_signal_intf *signal_iface;
	conf_object_t *signal;
}vic_s3c2410x_device;

static char* regs_name[] = {
	"srcpnd",
	"intmod",
	"intmsk",
	"priority",
	"intpnd",
	"intoffset",
	"subsrcpnd",
	"intsubmsk",
	NULL,
};

#define INT_ADC		(1 << 31)
#define INT_RTC		(1 << 30)
#define INT_SPI1	(1 << 29)
#define INT_UART0	(1 << 28)
#define INT_IIC		(1 << 27)
#define INT_USBH	(1 << 26)
#define INT_USBD	(1 << 25)
#define INT_UART1	(1 << 23)
#define INT_SPI0	(1 << 22)
#define INT_SDI		(1 << 21)
#define INT_DMA3	(1 << 20)
#define INT_DMA2	(1 << 19)
#define INT_DMA1	(1 << 18)
#define INT_DMA0	(1 << 17)
#define INT_LCD		(1 << 16)
#define INT_UART2	(1 << 15)
#define INT_TIMER4	(1 << 14)
#define INT_TIMER3 	(1 << 13)
#define INT_TIMER2	(1 << 12)
#define INT_TIMER1	(1 << 11)
#define INT_TIMER0	(1 << 10)
#define INT_WDT		(1 << 9)
#define INT_TICK	(1 << 8)
#define nBATT_FLT	(1 << 7)
#define EINT8_23	(1 << 5)
#define EINT4_7		(1 << 4)
#define EINT3		(1 << 3)
#define EINT2		(1 << 2)
#define EINT1		(1 << 1)
#define EINT0		(1 << 0)

#define SUB_INT_RXD0	(1 << 0)
#define SUB_INT_TXD0	(1 << 1)
#define SUB_INT_ERR0	(1 << 2)
#define SUB_INT_RXD1	(1 << 3)
#define SUB_INT_TXD1	(1 << 4)
#define SUB_INT_ERR1	(1 << 5)
#define SUB_INT_RXD2	(1 << 6)
#define SUB_INT_TXD2	(1 << 7)
#define SUB_INT_ERR2	(1 << 8)
#define SUB_INT_TC	(1 << 9)
#define SUB_INT_ADC	(1 << 10)

#endif
