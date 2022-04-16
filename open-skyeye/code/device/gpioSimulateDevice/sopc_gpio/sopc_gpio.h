#ifndef __SOPC_GPIO_H__
#define __SOPC_GPIO_H__

#define IIC_ENABLE_MASK		0x3C0000

struct registers
{
	uint32_t gpio_porta_dr;    //A Port Data register
	uint32_t gpio_porta_ddr;   //A Port Data Direction register
	uint32_t gpio_portb_dr;    //B Port Data register
	uint32_t gpio_portb_ddr;   //B Port Data Direction register
	uint32_t gpio_portc_dr;    //C Port Data register
	uint32_t gpio_portc_ddr;   //C Port Data Direction register
	uint32_t gpio_inten;       //interrupt enable register
	uint32_t gpio_intmask;     //interrupt mask register
	uint32_t gpio_inttype_level;  //interrupt level register
	uint32_t gpio_int_polarity;   //interrupt polarity register
	uint32_t gpio_intstatus;      //interrupt Status register
	uint32_t gpio_raw_intstatus;  //Raw Interrupt Status register
	uint32_t gpio_debounce;       //Debounce enable register
	uint32_t gpio_porta_eoi;      //Clear Interrupt register
	uint32_t gpio_ext_porta;      //External Port A register
	uint32_t gpio_ext_portb;      //External Port B register
	uint32_t gpio_ext_portc;      //External Port C register
	uint32_t gpio_ls_sync;        //Sync level register
};

struct sopc_gpio_device_t
{
	conf_object_t* obj;
	struct {
		int                     num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;

	fpga_gpio_connect_intf *iic_iface;
	conf_object_t *iic;
	struct registers regs;
};

typedef struct sopc_gpio_device_t sopc_gpio_device;

/*gpio Registers Base--0x68600000 Offset*/
#define GPIO_PORTA_DR       0x00
#define GPIO_PORTA_DDR      0x04
#define GPIO_PORTB_DR       0x0c
#define GPIO_PORTB_DDR      0x10
#define GPIO_PORTC_DR       0x18
#define GPIO_PORTC_DDR      0x1c

#define GPIO_INTEN          0x30
#define GPIO_INTMASK        0x34
#define GPIO_INTTYPE_LEVEL  0x38
#define GPIO_INT_POLARITY   0x3c
#define GPIO_INT_STATUS     0x40
#define GPIO_RAW_INT_STATUS 0x44
#define GPIO_DEBOUNCE       0x48
#define GPIO_PORTA_EOI      0x4c
#define GPIO_EXT_PORTA      0x50
#define GPIO_EXT_PORTB      0x54
#define GPIO_EXT_PORTC      0x58
#define GPIO_LS_SYNC        0x60

static char* regs_name[] =
{
	"gpio_porta_dr",
	"gpio_porta_ddr",
	"gpio_portb_dr",
	"gpio_portb_ddr",
	"gpio_portc_dr",
	"gpio_portc_ddr",
	"gpio_inten",
	"gpio_intmask",
	"gpio_inttype_level",
	"gpio_int_polarity",
	"gpio_intstatus",
	"gpio_raw_intstatus",
	"gpio_debounce",
	"gpio_porta_eoi",
	"gpio_ext_porta",
	"gpio_ext_portb",
	"gpio_ext_portc",
	"gpio_ls_sync",
	NULL
};

static int regs_offset[] =
{
	GPIO_PORTA_DR,
	GPIO_PORTA_DDR,
	GPIO_PORTB_DR,
	GPIO_PORTB_DDR,
	GPIO_PORTC_DR,
	GPIO_PORTC_DDR,
	GPIO_INTEN,
	GPIO_INTMASK,
	GPIO_INTTYPE_LEVEL,
	GPIO_INT_POLARITY,
	GPIO_INT_STATUS,
	GPIO_RAW_INT_STATUS,
	GPIO_DEBOUNCE,
	GPIO_PORTA_EOI,
	GPIO_EXT_PORTA,
	GPIO_EXT_PORTB,
	GPIO_EXT_PORTC,
	GPIO_LS_SYNC,
};

#define GET_PORTA_DDR(i)    ((regs->gpio_porta_ddr >> i) & 0x1)
#define SET_PORTA(i)    (regs->gpio_porta_dr |= (1 << i))
#define GET_PORTB_DDR(i)    ((regs->gpio_portb_ddr >> i) & 0x1)
#define SET_PORTB(i)    (regs->gpio_portb_dr |= (1 << i))
#define GET_PORTC_DDR(i)    ((regs->gpio_portc_ddr >> i) & 0x1)
#define SET_PORTC(i)    (regs->gpio_portc_dr |= (1 << i))
#endif
