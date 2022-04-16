#ifndef __LOONGSON_UART_H__ 
#define __LOONGSON_UART_H__ 

union DAT_REG
{
	uint8_t DAT_T;
	uint8_t all;
};

typedef struct
{
	uint8_t IRE:1;
	uint8_t ITE:1;
	uint8_t ILE:1;
	uint8_t IME:1;
	uint8_t reserved:4;
}IER_REG_BITS;

union IER_REG
{
	IER_REG_BITS bits;
	uint8_t all;
};
	
typedef struct
{
	uint8_t INTPn:1;
	uint8_t H:3;
	uint8_t reserved:4;
}HR_REG_BIT;


typedef struct
{
	uint8_t reserved1:1;
	uint8_t rxreset:1;
	uint8_t txreset:1;
	uint8_t reserved2:3;
	uint8_t trigger:2;
}FCR_REG_BIT;

union HR_REG
{
	HR_REG_BIT bits;
	uint8_t all;
};


typedef struct
{
	uint8_t bec:2;
	uint8_t sb:1;
	uint8_t pe:1;
	uint8_t eps:1;
	uint8_t spd:1;
	uint8_t bcb:1;
	uint8_t dlab:1;
}LCR_REG_BIT;

union LCR_REG
{
	LCR_REG_BIT bits;
	uint8_t all;
};

typedef struct
{
	uint8_t reserved5:5;
	uint8_t infrared_pol:1;
	uint8_t rx_pol:1;
	uint8_t infrared:1;
}MCR_REG_BIT;

union MCR_REG
{	
	MCR_REG_BIT bits;
	uint8_t all;
};

typedef struct
{
	uint8_t DR:1;
	uint8_t OE:1;
	uint8_t PE:1;
	uint8_t FE:1;
	uint8_t BI:1;
	uint8_t TFE:1;
	uint8_t TE:1;
	uint8_t error:1;
}LSR_REG_BIT;

union LSR_REG
{
	LSR_REG_BIT bits;
	uint8_t all;
};

union MSR_REG
{
	uint8_t all;
	uint8_t reserved:8;
};

typedef struct
{
	uint8_t reserved1:1;
	uint8_t rxreset:1;
	uint8_t txreset:1;
	uint8_t reserved2:3;
	uint8_t trigger:2;
}FCR_REG_T;
	
union FCR_REG
{
	uint8_t all;
	FCR_REG_T bits;
};

typedef struct
{
	uint8_t DL_d;
	uint8_t DL_h;
	uint8_t DL_l;
}DL_REG;


typedef struct
{
	union DAT_REG DAT_T;
	union IER_REG IER_T;
	union HR_REG HR_T;
	union LCR_REG LCR_T;
	union MCR_REG MCR_T;
	union LSR_REG LSR_T;
	union MSR_REG MSR_T;
}loongson_uart_reg_t;

struct loongson_uart_device_t
{
	conf_object_t* obj;
	loongson_uart_reg_t *regs;
	skyeye_uart_intf *term_iface;
	conf_object_t *term;
	conf_object_t *uart_file;
	uart_file_intf *uart_file_iface;
	uint8_t recv_data_num;
	union FCR_REG FCR_T;
	DL_REG DL;
};
typedef struct loongson_uart_device_t loongson_uart_device;
static char* regs_name[] =
{
	"DAT",
	"DL_L",
	"IER",
	"DL_H",
	"HR",
	"FCR",
	"DL_D",
	"LCR",
	"MCR",
	"LSR",
	"MSR",
	NULL
};

static uint32_t regs_offset[] =
{
	0x0000,
	0x0001,
	0x0002,
	0x0003,
	0x0004,
	0x0005,
	0x0006
};

#define  DAT			0x0
#define  DL_L			0x0
#define  IER			0x1
#define  DL_H			0x1
#define  HR				0x2
#define  FCR			0x2
#define  DL_D			0x2
#define  LCR			0x3
#define  MCR			0x4
#define  LSR			0x5
#define  MSR			0x6
#endif
