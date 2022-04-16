#ifndef __SPI_C6678_H__
#define __SPI_C6678_H__

/* the Field MaKe (Raw) macro */
#define CSL_FMKR(msb, lsb, val)                                             \
    (((val) & ((1 << ((msb) - (lsb) + 1)) - 1)) << (lsb))

/* the Field EXTract (Raw) macro */
#define CSL_FEXTR(reg, msb, lsb)                                            \
    (((reg) >> (lsb)) & ((1 << ((msb) - (lsb) + 1)) - 1))

/* the Field INSert (Raw) macro */
#define CSL_FINSR(reg, msb, lsb, val)                                       \
    ((reg) = ((reg) &~ (((1 << ((msb) - (lsb) + 1)) - 1) << (lsb)))         \
    | CSL_FMKR(msb, lsb, val))

struct registers {
	uint32_t spigcr0;		// 0x0    控制软重置位
	uint32_t spigcr1;		// 0x4    控制基本配置
	uint32_t spiint0;		// 0x8    使能中断, error, DMA和其他功能
	uint32_t spilvl;		// 0xC    设置SPI中断等级
	uint32_t spiflg;		// 0x10   显示在操作期间的几个事件的状态

	uint32_t spipc0;		// 0x14   确定pins是否作为通用I/O或SPI功能别针
	// Bit 11, Slave out, master in pin function.
	#define SPISOMI_RESERVED           0
	#define SPISOMI_SPI_FUNCTIONAL_PIN 1

	// Bit 10, Slave in, master out pin function. 
	#define SPISIMO_RESERVED           0
	#define SPISIMO_SPI_FUNCTIONAL_PIN 1

	// Bit 9, SPI clock pin function.
	#define SPICLK_RESERVED            0
	#define SPICLK_SPI_FUNCTIONAL_PIN  1

	#define CS_NUM 4 // 片选信号引脚的数量
	// Bit i = n ~ 0; n等于片选信号引脚的数量
	#define SPISCS_i_RESERVED            0
	#define SPISCS_i_SPI_FUNCTIONAL_PIN  1

	uint32_t spidat0;		// 0x38   传输数据寄存器
	uint32_t spidat1;		// 0x3C   传输数据格式选择寄存器
	union {
		uint32_t all;
		struct {
			uint32_t RXDATA		: 16;  //15-0
			uint32_t Reserved	: 12;  //27-16
			uint32_t BITERR		: 1;   //27-16
			uint32_t TXFULL		: 1;   //27-16
			uint32_t RXOVR		: 1;   //27-16, 缓存接收溢出位
			uint32_t RXEMPTY	: 1;   //27-16
		};
	} spibuf;				// 0x40   持有接收到数据, 一个字
	uint32_t spiemu;		// 0x44   SPIBUF的镜像, 读取没有清除标志寄存器
	uint32_t spidelay;		// 0x48   设置 SPISCS[n] 模式, SPISCS[n] pre-transfer / post-transfer 延迟时间
	uint32_t spidef;		// 0x4C   仅仅在SPISCS[n]解码模式中, sets high low/active SPISCS[n] signal. 
	uint32_t spifmt0;		// 0x50   Configuration of data word format 0. // 忽略其它位, 4-0位的 CHARLEN 需要关注, 表示SPI data word length (value = 0-1Fh)
	uint32_t spifmt1;		// 0x54   Configuration of data word format 1.
	uint32_t spifmt2;		// 0x58   Configuration of data word format 2.
	uint32_t spifmt3;		// 0x5C   Configuration of data word format 3.
	uint32_t intvec0;		// 0x60   Interrupt vector for line INT0.
	uint32_t intvec1;		// 0x64   Interrupt vector for line INT1.
};

struct c6678_device {
	conf_object_t* obj;

	conf_object_t* signal;
	core_signal_intf* signal_iface;

	conf_object_t *serial[CS_NUM];
	skyeye_serial_device_t *serial_iface[CS_NUM];

	#define MASTER 1
	#define SLAVE  0
	uint32_t spi_mode;		// master or slave mode

	FIFO * ReceiveBuff;		// 事件通道队列, 每个队列长度为16个


	struct registers regs;
};

typedef struct c6678_device spi_c6678_device;

static char* regs_name[] = {
	"spigcr0",
	"spigcr1",
	"spiint0",
	"spilvl",
	"spiflg",
	"spipc0",
	"spidat0",
	"spidat1",
	"spibuf",
	"spiemu",
	"spidelay",
	"spidef",
	"spifmt0",
	"spifmt1",
	"spifmt2",
	"spifmt3",
	"intvec0",
	"intvec1",
	NULL
};

#define SPIGCR0     0x0
#define SPIGCR1     0x4
#define SPIINT0     0x8
#define SPILVL      0xC
#define SPIFLG      0x10
#define SPIPC0      0x14
#define SPIDAT0     0x38
#define SPIDAT1     0x3C
#define SPIBUF      0x40
#define SPIEMU      0x44
#define SPIDELAY    0x48
#define SPIDEF      0x4C
#define SPIFMT0     0x50
#define SPIFMT1     0x54
#define SPIFMT2     0x58
#define SPIFMT3     0x5C
#define INTVEC0     0x60
#define INTVEC1     0x64

#endif
