#ifndef _BX_GPIO_H
#define _BX_GPIO_H

#if BX_SUPPORT_GPIO

#define GPIO_BASE 0x9300
#define GPIO_LEN  0xF0

#define GPIO_CARD 	2
#define GPIO_REG_NUM 	3
#define GPIO_PIN_NUM    8

#define GPIO_MAX_PIN_NUM (GPIO_CARD * GPIO_REG_NUM * GPIO_PIN_NUM)

class bx_gpio_c : public bx_gpio_stub_c {
public:
  bx_gpio_c();
  virtual ~bx_gpio_c() {}
  virtual void init(void);
  virtual void reset (unsigned type) {}
  virtual void mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data);
  virtual void mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data);
  void *obj;
private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u addr, unsigned io_len);
  void write(Bit32u addr, Bit32u dvalue, unsigned io_len);
};

typedef struct {
	uint32_t ctrl_0_data;
	uint32_t ctrl_1_data;
	uint32_t ctrl_2_data;

	uint32_t pin_data[GPIO_MAX_PIN_NUM];
#if 0	
	union {
		uint32_t all;
		struct {
			uint32_t pin0 :1;
			uint32_t pin1 :1;
			uint32_t pin2 :1;
			uint32_t pin3 :1;
			uint32_t pin4 :1;
			uint32_t pin5 :1;
			uint32_t pin6 :1;
			uint32_t pin7 :1;
		};
	}gpio_pin;
#endif
} x86_gpio_reg_t;

typedef struct x86_gpio_device{
	conf_object_t 	*obj;                
	x86_gpio_reg_t* regs;

	conf_object_t	*can_linker;
	can_linker_intf *can_linker_iface;

	skyeye_gpio_connector_intf *gpio_connector_iface;
	conf_object_t* gpio_connector;

	conf_object_t* gpio_link;
    	gpio_linker_pin_intf* gpio_linker_intf;

    	char *connect_list;
    	int connect_quanaity;
    	int connect_pin[128];
} x86_gpio_dev;

static char* regs_name[] = {
	"ctrl_0_data",
	"ctrl_1_data",
	"ctrl_2_data",
	"gpio_pin0",
	"gpio_pin1",
	"gpio_pin2",
	"gpio_pin3",
	"gpio_pin4",
	"gpio_pin5",
	"gpio_pin6",
	"gpio_pin7",
	"gpio_pin8",
	"gpio_pin9",
	"gpio_pin10",
	"gpio_pin11",
	"gpio_pin12",
	"gpio_pin13",
	"gpio_pin14",
	"gpio_pin15",
	"gpio_pin16",
	"gpio_pin17",
	"gpio_pin18",
	"gpio_pin19",
	"gpio_pin20",
	"gpio_pin21",
	"gpio_pin22",
	"gpio_pin23",
	"gpio_pin24",
	"gpio_pin25",
	"gpio_pin26",
	"gpio_pin27",
	"gpio_pin28",
	"gpio_pin29",
	"gpio_pin30",
	"gpio_pin31",
	"gpio_pin32",
	"gpio_pin33",
	"gpio_pin34",
	"gpio_pin35",
	"gpio_pin36",
	"gpio_pin37",
	"gpio_pin38",
	"gpio_pin39",
	"gpio_pin40",
	"gpio_pin41",
	"gpio_pin42",
	"gpio_pin43",
	"gpio_pin44",
	"gpio_pin45",
	"gpio_pin46",
	"gpio_pin47",
	NULL
};

#if 0
#define CTRL_DATA  0x0
#define GPIO_PIN0 0x4
#define GPIO_PIN1 0x8
#define GPIO_PIN2 0xC
#define GPIO_PIN3 0x10
#define GPIO_PIN4 0x14
#define GPIO_PIN5 0x18
#define GPIO_PIN6 0x1c
#define GPIO_PIN7 0x20
#endif

#define GPIO_CTRL_0 	   	0
#define GPIO_CTRL_1 	   	4
#define GPIO_CTRL_2 	   	8
#define GPIO_PIN_FST 		12
#define GPIO_PIN_END		(12 + 4 * GPIO_MAX_PIN_NUM)
#endif

#endif
