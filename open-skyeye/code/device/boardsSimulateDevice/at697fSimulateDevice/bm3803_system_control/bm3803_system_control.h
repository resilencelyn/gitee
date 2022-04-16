#ifndef __BM3803_SYSTEM_CONTROL_H__
#define __BM3803_SYSTEM_CONTROL_H__

#include <skyeye_uart_ops.h>
#include "intc_fifo.h"
#include "skyeye_lock.h"

/*memory interface registers*/
typedef union
{
	uint32_t all;
	struct
	{
		uint32_t prrws    :4; //PROM read waitstates
		uint32_t prwws    :4; //PROM write waitstates
		uint32_t prwdh    :2; //PROM width
		uint32_t res0     :1;
		uint32_t prwen    :1; //PROM write enable
		uint32_t res1     :7;
		uint32_t ioen     :1; //I/O area enable
		uint32_t iows     :4; //I/O waitstates
		uint32_t res2     :1;
		uint32_t bexc     :1; //Bus error enable for RAM, PROM and IO access
		uint32_t iobrdy   :1; //IO area bus ready enable
		uint32_t iowdh    :2; //I/O bus width
		uint32_t abrdy    :1; //Asynchronous bus ready
		uint32_t pbrdy    :1; //PROM area bus-ready enable
	};
}mcfg1_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t ramrws   :2; //SRAM read waitstates
		uint32_t ramwws   :2; //SRAM write waitstates
		uint32_t ramwdh   :2; //SRAM bus width
		uint32_t ramrmw   :1; //Read-modify-write on the SRAM
		uint32_t rambrdy  :1; //SRAM area bus ready enable
		uint32_t res0     :1;
		uint32_t rambs    :4; //SRAM bank size
		uint32_t si       :1; //SRAM disable
		uint32_t se       :1; //SDRAM enable
		uint32_t res1     :4;
		uint32_t sdrcmd   :2; //SDRAM command
		uint32_t sdrcls   :2; //SDRAM column size
		uint32_t sdrbs    :3; //SDRAM banks size
		uint32_t sdrcas   :1; //SDRAM CAS delay
		uint32_t trfc     :3; //SDRAM tRFC timing
		uint32_t trp      :1; //SDRAM tRP timing
		uint32_t sdrref   :1; //SDRAM refresh
	};
}mcfg2_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t tcb     :8; //Test checkbits
		uint32_t pe      :1; //PROM EDAC enable
		uint32_t re      :1; //RAM EDAC enable
		uint32_t rb      :1; //EDAC diagnostic read
		uint32_t wb      :1; //EDAC diagnostic write bypass
		uint32_t srcrv   :15;//SDRAM refresh counter reload value
		uint32_t me      :1; //Memory EDAC
		uint32_t res0    :2;
		uint32_t rfc     :2; //Register file check bits
	};
}mcfg3_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t mask    :15; //Address Mask
		uint32_t tag     :15; //Address Tag
		uint32_t bp      :1;  //Block Protect
		uint32_t en      :1;  //Enable
	};
}wpr_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t rev0    :1;
		uint32_t bp      :1;  //Block protect
		uint32_t start   :28; //Start Address
	};
}wpsta_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t su      :1;  //Supervisor Mode
		uint32_t us      :1;  //User Mode
		uint32_t end     :28; //End Address
	};
}wpsto_reg_t;

typedef struct
{
	mcfg1_reg_t mcfg1;   //Memory Configuration Register 1
	mcfg2_reg_t mcfg2;   //Memory Configuration Register 2
	mcfg3_reg_t mcfg3;   //Memory Configuration Register 3
	wpr_reg_t   wpr1;    //Write Protection Register 1
	wpr_reg_t   wpr2;    //Write Protection Register 2
}memory_reg_t;

/*system registers*/
typedef union
{
	uint32_t all;
	struct
	{
		uint32_t wprt     :2; //Write Protection
		uint32_t pci      :2; //PCI Core Type
		uint32_t fpu      :2; //FPU Type
		uint32_t memstat  :1; //Memory Status and Address Failing Register
		uint32_t wdog     :1; //Watchdog
		uint32_t mulinst  :1; //UMUL/SMUL instructions
		uint32_t divinst  :1; //UDIV/SDIV instructions
		uint32_t dlsz     :2; //Data Cache Line Size
		uint32_t dcsz     :3; //Data Cache Set Size
		uint32_t ilsz     :2; //Instruction Cache Line Size
		uint32_t icsz     :3; //Instruction Cache Set Size
		uint32_t nwin     :5; //IU Register File Windows
		uint32_t imac     :1; //UMAC/SMAC instructions
		uint32_t wtpnt    :3; //IU Watchpoints
		uint32_t sdrctrl  :1; //SDRAM Controller
		uint32_t dsu      :1; //Debug Support Unit
		uint32_t mmu      :1; //Memory Management Unit
	};
}pcr_reg_t;

typedef uint32_t failar_reg_t;
typedef union
{
	uint32_t all;
	struct
	{
		uint32_t hsize      :3; //Transmission Size
		uint32_t hmast      :4; //Record access failed HMASER
		uint32_t rw         :1; //R/W access
		uint32_t ne         :1; //Error status
	};
}failsr_reg_t;

typedef struct
{
	pcr_reg_t      pcr;  //Product Configuration Register
	failar_reg_t   failar;  //Fail Address Register
	failsr_reg_t   failsr;  //Fail Status Register
}system_reg_t;

typedef struct
{
	uint32_t all;
	struct
	{
		uint32_t ics      :2; //Instruction Cache State
		uint32_t dcs      :2; //Data Cache State
		uint32_t ifs      :1; //Instruction Cache Freeze on Interrupt
		uint32_t df       :1; //Data Cache Freeze on Interrupt
		uint32_t res0     :8;
		uint32_t dp       :1; //Data cache flush pending
		uint32_t ip       :1; //Instruction cache flush pending
		uint32_t ib       :1; //Instruction burst fetch
		uint32_t res1     :4;
		uint32_t fi       :1; //Flush Instruction cache
		uint32_t fd       :1; //Flush data cache
		uint32_t ds       :1; //Data cache snoop enable
		uint32_t dsets    :2; //Data cache associativity
		uint32_t isets    :2; //Instruction cache associativity
		uint32_t irepl    :2; //Instruction cache replacement policy
		uint32_t drepl    :2; //Data cache replacement policy
	};
}cache_reg_t;

typedef uint32_t idle_reg_t;

/*Timer Registers*/
typedef union
{
	uint32_t all;
	struct
	{
		uint32_t en    :1; //Enable counter
		uint32_t rl    :1; //Reload counter
		uint32_t ld    :1; //Load counter
	};
}timctr_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t cnt    :10; //Prescaler counter value
	};
}scac_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t rv    :10; //Prescaler reload value
	};
}scar_reg_t;

typedef struct
{
	uint32_t     timc1;   //Timer 1 Counter Register
	uint32_t     timr1;   //Timer 1 Reload Register
	timctr_reg_t timctr1; //Timer 1 Control Register
	uint32_t     wdg;     //Watchdog Register
	uint32_t     timc2;   //Timer 2 Counter Register
	uint32_t     timr2;   //Timer 2 Reload Register
	timctr_reg_t timctr2; //Timer 2 Control Register
	scac_reg_t   scac;    //Prescaler Counter Register
	scar_reg_t   scar;    //Prescaler Reload Register
}timer_reg_t;

/*UART Registers*/

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t rtd    :8; //Received/Transmit Data on the UART
	};
}data_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t dr    :1; //Data ready
		uint32_t ts    :1; //Transmitter shift register empty
		uint32_t th    :1; //Transmitter hold register empty
		uint32_t br    :1; //Break received
		uint32_t ov    :1; //Overrun
		uint32_t pe    :1; //Parity error
		uint32_t fe    :1; //Framing error
	};
}status_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t re    :1; //Receiver enable
		uint32_t te    :1; //Transmitter enable
		uint32_t ri    :1; //Receiver interrupt enable
		uint32_t ti    :1; //Transmitter interrupt enable
		uint32_t ps    :1; //Parity select
		uint32_t pe    :1; //Parity enable
		uint32_t fl    :1; //Flow control
		uint32_t lb    :1; //Loop back
		uint32_t ec    :1; //External Clock
	};
}control_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t rv    :12; //UART scaler reload value
	};
}scaler_reg_t;

typedef struct
{
	data_reg_t    uad1;      //UART 1 Data Register
	status_reg_t  uas1;      //UART 1 Status Register
	control_reg_t uac1;      //UART 1 Control Register
	scaler_reg_t  uasca1;    //UART 1 Scaler Register
	data_reg_t    uad2;      //UART 2 Data Register
	status_reg_t  uas2;      //UART 2 Status Register
	control_reg_t uac2;      //UART 2 Control Register
	scaler_reg_t  uasca2;    //UART 2 Scaler Register
}uart_reg_t;

/*Interrupt Registers*/
typedef union
{
	uint32_t all;
	struct
	{
		uint32_t imask    :16; //Interrupt Mask
		uint32_t ilevel   :16; //Interrupt Level
	};
}itmp_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t ipend    :16; //Interrupt Pending
	};
}itp_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t iforce   :16; //Interrupt Force
	};
}itf_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t iclear   :16; //Interrupt Clear
	};
}itc_reg_t;

typedef struct
{
	itmp_reg_t itmp; //Interrupt Mask and Priority Register
	itp_reg_t  itp;  //Interrupt Pending Register
	itf_reg_t  itf;  //Interrupt Force Register
	itc_reg_t  itc;  //Interrupt Clear Register
}intr_reg_t;

/*GPIO Registers*/
typedef union
{
	uint32_t all;
	struct
	{
		uint32_t piodat   :16; //PIO[15:0] port value
		uint32_t lowdat   :8;  //D[0:7] bus value
		uint32_t meddat   :8;  //D[15:8] bus value
	};
}iodat_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t piodir   :16; //PIO[15:0] port direction 1=output 0=input
		uint32_t lowdir   :8;  //D[7..0] port direction 1=output 0=input
		uint32_t meddir   :8;  //D[15:8] port direction 1=output 0=input
	};
}iodir_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t isel0   :5; //I/O port select The value of this field defines which I/O port (0 - 31) should generate interrupt 0
		uint32_t pl0     :1; //Polarity
		uint32_t le0     :1; //Level/edge triggered.
		uint32_t en0     :1; //Enable
		uint32_t isel1   :5; //I/O port select The value of this field defines which I/O port (0 - 31) should generate interrupt 1
		uint32_t pl1     :1;
		uint32_t le1     :1;
		uint32_t en1     :1;
		uint32_t isel2   :5; //I/O port select The value of this field defines which I/O port (0 - 31) should generate interrupt 2
		uint32_t pl2     :1;
		uint32_t le2     :1;
		uint32_t en2     :1;
		uint32_t isel3   :5; //I/O port select The value of this field defines which I/O port (0 - 31) should generate interrupt 3
		uint32_t pl3     :1;
		uint32_t le3     :1;
		uint32_t en3     :1;
	};
}ioit1_reg_t;

typedef union
{
	uint32_t all;
	struct
	{
		uint32_t isel4   :5; //I/O port select The value of this field defines which I/O port (0 - 31) should generate interrupt 4
		uint32_t pl4     :1; //Polarity
		uint32_t le4     :1; //Level/edge triggered.
		uint32_t en4     :1; //Enable
		uint32_t isel5   :5; //I/O port select The value of this field defines which I/O port (0 - 31) should generate interrupt 5
		uint32_t pl5     :1;
		uint32_t le5     :1;
		uint32_t en5     :1;
		uint32_t isel6   :5; //I/O port select The value of this field defines which I/O port (0 - 31) should generate interrupt 6
		uint32_t pl6     :1;
		uint32_t le6     :1;
		uint32_t en6     :1;
		uint32_t isel7   :5; //I/O port select The value of this field defines which I/O port (0 - 31) should generate interrupt 7
		uint32_t pl7     :1;
		uint32_t le7     :1;
		uint32_t en7     :1;
	};
}ioit2_reg_t;

typedef struct
{
	iodat_reg_t iodat; //I/O Port Data Register
	iodir_reg_t iodir; //I/O Port Direction Register
	ioit1_reg_t ioit1; //I/O Port Interrupt Register
	ioit2_reg_t ioit2; //I/O Port Interrupt Register
}gpio_reg_t;

struct registers
{
	memory_reg_t memory_reg;
	system_reg_t system_reg;
	cache_reg_t  cache_reg;
	idle_reg_t   idle_reg;
	timer_reg_t  timer_reg;
	uart_reg_t   uart_reg;
	intr_reg_t   intr_reg;
	gpio_reg_t   gpio_reg;
};

struct bm3803_system_control_device_t
{
	conf_object_t* obj;
	struct registers regs;

	struct {
		conf_object_t          *obj;
		skyeye_uart_intf       *intf;
	}term1;

	struct {
		conf_object_t          *obj;
		skyeye_uart_intf       *intf;
	}term2;

	struct {
		conf_object_t          *obj;
		uart_file_intf         *intf;
	}uart_file;

	struct {
		int                    num;
		conf_object_t          *obj;
		core_signal_intf       *intf;
		general_signal_intf    *iface;
	}signal;

	int cpu_freq;
	int timer;
	int baudrate;
	int uart1_scaler;
	int uart2_scaler;
	FIFO *fifo;
	RWLOCK_T lock;
};

typedef struct bm3803_system_control_device_t bm3803_system_control_device;

/*System Control Base Address--0x80000000*/

/*Memory Interface Offset*/
#define MCFG1    0x0
#define MCFG2    0x4
#define MCFG3    0x8
#define WPR1     0x1C
#define WPR2     0x20

/*System Registers Offset*/
#define PCR      0x24
#define FAILAR   0x0C
#define FAILSR   0x10

/*Cache Register Offset*/
#define CCR      0x14

/*Idle Register Offset*/
#define IDLE     0x18

/*Timer Registers Offset*/
#define TIMC1    0x40
#define TIMR1    0x44
#define TIMCTR1  0x48
#define WDG      0x4C
#define TIMC2    0x50
#define TIMR2    0x54
#define TIMCTR2  0x58
#define SCAC     0x60
#define SCAR     0x64

/*UART Registers Offset*/
#define UAD1     0x70
#define UAS1     0x74
#define UAC1     0x78
#define UASCA1   0x7C
#define UAD2     0x80
#define UAS2     0x84
#define UAC2     0x88
#define UASCA2   0x8C

/*Interrupt Registers Offset*/
#define ITMP     0x90
#define ITP      0x94
#define ITF      0x98
#define ITC      0x9C

/*GPIO Registers Offset*/
#define IODAT    0xA0
#define IODIR    0xA4
#define IOIT1    0xA8
#define IOIT2    0xAC

static char* regs_name[] =
{
	"mcfg1",
	"mcfg2",
	"mcfg3",
	"wpr1",
	"wpr2",

	"pcr",
	"failar",
	"failsr",

	"ccr",

	"idle",

	"timc1",
	"timr1",
	"timctr1",
	"wdg",
	"timc2",
	"timr2",
	"timctr2",
	"scac",
	"scar",

	"uad1",
	"uas1",
	"uac1",
	"uasca1",
	"uad2",
	"uas2",
	"uac2",
	"uasca2",

	"itmp",
	"itp",
	"itf",
	"itc",

	"iodat",
	"iodir",
	"ioit1",
	"ioit2",
	NULL
};

enum Interrupt_Number
{
	HardWare_Error = 1,
	UART2,
	UART1,
	IO0,
	IO1,
	IO2,
	IO3,
	Timer1,
	Timer2,
	IO4,
	DSU_TRACE,
	IO5,
	IO6,
	PCI,
	IO7
};

#define IT1EN_CHECK(n)    (dev->regs.gpio_reg.ioit1.all >> (8 * (n + 1) - 1))
#define IT2EN_CHECK(n)    (dev->regs.gpio_reg.ioit2.all >> (8 * (n + 1) - 1))

#define IT1LE_CHECK(n)    (dev->regs.gpio_reg.ioit1.all >> (8 * (n + 1) - 2))
#define IT2LE_CHECK(n)    (dev->regs.gpio_reg.ioit2.all >> (8 * (n + 1) - 2))

#define IT1PL_CHECK(n)    (dev->regs.gpio_reg.ioit1.all >> (8 * (n + 1) - 3))
#define IT2PL_CHECK(n)    (dev->regs.gpio_reg.ioit2.all >> (8 * (n + 1) - 3))

#define IT1ISEL_CHECK(n)  (dev->regs.gpio_reg.ioit1.all >> (8 * n) & 0x1f)
#define IT2ISEL_CHECK(n)  (dev->regs.gpio_reg.ioit2.all >> (8 * n) & 0x1f)

#define IPEND(n)          ((dev->regs.intr_reg.itp.ipend >> n) & 0x1)
#define IMASK(n)          ((dev->regs.intr_reg.itmp.imask >> n) & 0x1)
#define IFORCE(n)         ((dev->regs.intr_reg.itf.iforce >> n) & 0x1)
#define ILEVEL(n)         ((dev->regs.intr_reg.itmp.ilevel >> n) & 0x1)

#define SET_PEND(n)       (dev->regs.intr_reg.itp.ipend |= (1 << n))
#define CLR_PEND(n)       (dev->regs.intr_reg.itp.ipend &= ~(1 << n))
#define SET_FORCE(n)       (dev->regs.intr_reg.itf.iforce |= (1 << n))
#define CLR_FORCE(n)       (dev->regs.intr_reg.itf.iforce &= ~(1 << n))
void uart_read_handler(conf_object_t *opaque);
void uart_tick_handler(conf_object_t *opaque);
void timer_count_handler(conf_object_t *opaque);
void timer_interrupt_handler(conf_object_t *opaque);
void uart_interrupt_handler(conf_object_t *opaque);
void signal_interrupt_to_cpu(conf_object_t *opaque);

void interrupt_request_handle(conf_object_t *opaque, int irq_num);

static int regs_offset[] = {
	MCFG1,
	MCFG2,
	MCFG3,
	WPR1,
	WPR2,
	PCR,
	FAILAR,
	FAILSR,
	CCR,
	IDLE,
	TIMC1,
	TIMR1,
	TIMCTR1,
	WDG,
	TIMC2,
	TIMR2,
	TIMCTR2,
	SCAC,
	SCAR,
	UAD1,
	UAS1,
	UAC1,
	UASCA1,
	UAD2,
	UAS2,
	UAC2,
	UASCA2,
	ITMP,
	ITP,
	ITF,
	ITC,
	IODAT,
	IODIR,
	IOIT1,
	IOIT2
};

#define MAX_FIFO_NUM  0x40000

exception_t uart_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count);
exception_t uart_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count);

exception_t gpio_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count);
exception_t gpio_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count);

exception_t intc_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count);
exception_t intc_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count);

exception_t timer_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count);
exception_t timer_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count);
#endif
