#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_GPIO 

#include <stdio.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"

#include "cpu/cpu.h"
#include "x86_gpio.h"

bx_gpio_c *gpioDevice = NULL;
#define LOG_THIS gpioDevice->
#define BX_GPIO_THIS this->

static exception_t x86_gpio_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);
static exception_t x86_gpio_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);

int libgpio_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  gpioDevice = new bx_gpio_c();
  bx_devices.plugingpio = gpioDevice;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, gpioDevice, BX_PLUGIN_GPIO);

  return(0); // Success
}

void libgpio_LTX_plugin_fini(void)
{
  delete gpioDevice;
}

struct bx_gpio_s_type {
  Bit16u gpio_registers[8]; /* different gpio value */
} bx_gpio_s;

bx_gpio_c::bx_gpio_c()
{
	put("IODBG");
}

void bx_gpio_c::init(void)
{
	DEV_register_ioread_handler_range(this, read_handler, GPIO_BASE, GPIO_BASE + GPIO_LEN, "BOCHS GPIO", 4);
	DEV_register_iowrite_handler_range(this, write_handler,GPIO_BASE, GPIO_BASE + GPIO_LEN, "BOCHS GPIO", 4);
}

Bit32u bx_gpio_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
	bx_gpio_c *bx_gpio_ptr = (bx_gpio_c *) this_ptr;
	return bx_gpio_ptr->read(addr, io_len);
}

Bit32u bx_gpio_c::read(Bit32u addr, unsigned io_len)
{
	Bit32u data = 0;
	x86_gpio_read((conf_object_t*)gpioDevice->obj, addr - GPIO_BASE, &data, io_len);
	return data;
}

void bx_gpio_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	bx_gpio_c *bx_gpio_ptr = (bx_gpio_c *) this_ptr;
	bx_gpio_ptr->write(addr, dvalue, io_len);
}

void bx_gpio_c::write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	conf_object_t *obj = gpioDevice->obj;
	x86_gpio_write(gpioDevice->obj, addr - GPIO_BASE, &dvalue, io_len);
	
	return;
}

void bx_gpio_c::mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
	return;
}

void bx_gpio_c::mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data){
}

static void gpio_raise_signal(conf_object_t *obj)
{
    x86_gpio_dev *dev = obj->obj;
    //DEV_pic_lower_irq(dev->interrupt_number);
    //DEV_pic_raise_irq(dev->interrupt_number);
}

unsigned int ReadGpioPin_Data(conf_object_t* opaque, int card_id, int pin)
{
	x86_gpio_dev *dev = opaque->obj;
	x86_gpio_reg_t *regs = dev->regs;
	unsigned int status;
	int id;
	id = pin + 16 * card_id;

	if (card_id == 0){
		if (((regs->ctrl_0_data >> pin) & 0x1) == 1) {
			status = regs->pin_data[id] & 1;
		}		
	}
	if (card_id == 1){
		if (((regs->ctrl_1_data >> pin) & 0x1) == 1) {
			status = regs->pin_data[id] & 1;
		}		
	}
	if (card_id == 2){
		if (((regs->ctrl_2_data >> pin) & 0x1) == 1) {
			 status = regs->pin_data[id] & 1;
		}		
	}

	return status;
}

void WriteGpioPin_Data(conf_object_t* opaque, int card_id, int pin, int val)
{
	x86_gpio_dev *dev = opaque->obj;
	x86_gpio_reg_t *regs = dev->regs;
	int id;
	id = pin + 16 * card_id;

	if (card_id == 0){
		if (((regs->ctrl_0_data >> pin) & 0x1) == 0) {
			regs->pin_data[id] = val & 1;
		}		
	}
	if (card_id == 1){
		if (((regs->ctrl_1_data >> pin) & 0x1) == 0) {
			regs->pin_data[id] = val & 1;
		}		
	}
	if (card_id == 2){
		if (((regs->ctrl_2_data >> pin) & 0x1) == 0) {
			regs->pin_data[id] = val & 1;
		}		
	}

	if (dev->gpio_connector_iface) {
		dev->gpio_connector_iface->update(dev->gpio_connector, regs->pin_data[id]);
	}

	if (dev->gpio_linker_intf) {
		dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, id, val & 0x1);
	}
	return;
}


static exception_t x86_gpio_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_gpio_dev *dev = opaque->obj;
	if (offset >= GPIO_PIN_FST && offset <= GPIO_PIN_END){
		int pin = (offset - GPIO_PIN_FST) / 4;	
		if (pin < 16){
			*(uint32_t *)buf = ReadGpioPin_Data(opaque, 0, pin);
		}else if (pin < 32){
			*(uint32_t *)buf = ReadGpioPin_Data(opaque, 1, pin - 16);
		}else if (pin < 48){
			*(uint32_t *)buf = ReadGpioPin_Data(opaque, 2, pin - 32);
		}else {
			printf("In %s, pin = %d is too large!\n", __func__, pin);
			return 0;
		}
		return No_exp;
	}

	switch (offset) {
		case GPIO_CTRL_0:
			*(uint32_t *)buf = dev->regs->ctrl_0_data;
			break;
		case GPIO_CTRL_1:
			*(uint32_t *)buf = dev->regs->ctrl_1_data;
			break;
		case GPIO_CTRL_2:
			*(uint32_t *)buf = dev->regs->ctrl_2_data;
			break;
		default: {
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in x86_gpio\n", offset);
			break;
		}
	}
#if 0
	switch (offset)
	{
		case CTRL_DATA:
			*(uint32_t *)buf = dev->regs->ctrl_data;
			break;
		default: {
			if ((offset >= GPIO_PIN0) && (offset <= GPIO_PIN7)) {
				int index = offset / 4 - 1;
				if (((dev->regs->ctrl_data >> index) & 0x1) == 1) {
					*(uint32_t *)buf = (dev->regs->gpio_pin.all >> index) & 0x1;
				} else {
					skyeye_log(Warning_log, "gpio%d is output mode\n", index);
					return 0;
				}
			} else {
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in gpio_c6748\n", offset);
			}
			break;
		}
	}
#endif
	//printf("%s, %d, offset = %x, buf = %x\n\n", __func__, __LINE__, offset, *(uint32_t *)buf);
	return  No_exp;
}


static exception_t x86_gpio_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	x86_gpio_dev *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	//printf("\n%s, %d, offset = %x, buf = %x\n", __func__, __LINE__, offset, val);
	if (offset >= GPIO_PIN_FST && offset <= GPIO_PIN_END){
		int pin = (offset - GPIO_PIN_FST) / 4;	
		if (pin < 16){
			WriteGpioPin_Data(opaque, 0, pin, val);
		}else if (pin < 32){
			WriteGpioPin_Data(opaque, 1, pin - 16, val);
		}else if (pin < 48){
			WriteGpioPin_Data(opaque, 2, pin - 32, val);
		}else {
			printf("In %s, pin = %d is too large!\n", __func__, pin);
			return 0;
		}
		return No_exp;
	}

	switch (offset) {
		case GPIO_CTRL_0:
			dev->regs->ctrl_0_data = val;
			break;
		case GPIO_CTRL_1:
			dev->regs->ctrl_1_data = val;
			break;
		case GPIO_CTRL_2:
			dev->regs->ctrl_2_data = val;
			break;
		default: {
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in x86_gpio\n", offset);
			break;
		}
	}
#if 0
	switch (offset) {
		case CTRL_DATA:
			dev->regs->ctrl_data = val;
			break;
		default: {
			if ((offset >= GPIO_PIN0) && (offset <= GPIO_PIN7)) {
				int index = offset / 4 - 1;
				if (((dev->regs->ctrl_data >> index) & 0x1) == 0) {
					if (val & 0x1) {
						dev->regs->gpio_pin.all |= (val & 0x1) << index;
					} else {
						dev->regs->gpio_pin.all &= ~((val & 0x1) << index);
					}
					//printf("gpio_pin:%x\n", dev->regs->gpio_pin.all);
					if (dev->gpio_connector_iface) {
						dev->gpio_connector_iface->update(dev->gpio_connector, dev->regs->gpio_pin.all);
					}
					else if (dev->gpio_linker_intf) {
						dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, (dev->regs->gpio_pin.all >> index) & 0x1, val & 0x1);
					}
				} else {
					skyeye_log(Warning_log, "gpio%d is input mode\n", index);
					return 0;
				}
			} else {
				skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in gpio_c6748\n", offset);
			}
			break;
		}
	}
#endif
	return  No_exp;
}

static conf_object_t* new_x86_gpio(char* obj_name)
{
	x86_gpio_dev* dev = skyeye_mm_zero(sizeof(x86_gpio_dev));
	if(dev == NULL)
		printf("alloc memmory for obj in %x fail\n", obj_name);
	x86_gpio_reg_t* regs =  skyeye_mm_zero(sizeof(x86_gpio_reg_t));
	regs->ctrl_0_data = 0;
	regs->ctrl_1_data = 0;
	regs->ctrl_2_data = 0;
	
	int i;
	for (i = 0; i < GPIO_MAX_PIN_NUM; i++)
	{
		regs->pin_data[i] = 0;
	}
	//regs->gpio_pin.all = 0;
	dev->regs = regs;
	dev->obj = new_conf_object(obj_name, dev);
	gpioDevice->obj = dev->obj;

	return dev->obj;
}

static exception_t reset_x86_gpio(conf_object_t* opaque, const char* args)
{
	x86_gpio_dev *dev = opaque->obj;

	return No_exp;        
}

static exception_t free_x86_gpio(conf_object_t* conf_obj)
{
	x86_gpio_dev *dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
	return No_exp;
}

char* x86_gpio_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_gpio_dev *dev = conf_obj->obj;
	return regs_name[id];
}

uint32_t x86_gpio_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	x86_gpio_dev *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
	return *regs_value;
}

static char* x86_gpio_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	x86_gpio_dev *dev = conf_obj->obj;
	x86_gpio_reg_t* regs = dev->regs;
	*((uint32_t*)regs + id) = value;

	return NULL;
}

static uint32_t x86_gpio_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	x86_gpio_dev *dev = conf_obj->obj;
	x86_gpio_reg_t* regs = dev->regs;
	return  17;
}
static attr_value_t get_attr_connect_pin(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	x86_gpio_dev* dev = conf_obj->obj;
	attr_value_t connect_list = SKY_make_attr_string(dev->connect_list);
	return connect_list;
}

static exception_t  set_attr_connect_pin(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	x86_gpio_dev* dev = conf_obj->obj;
	dev->connect_list =SKY_attr_string(*value);

    	int i = 0;
	char tranf[128] = "";
    	char *delim_pin = ".";
    	char *swap;
    	strcpy(tranf,dev->connect_list);
    	dev->connect_pin[i] = atoi(strtok(tranf,delim_pin));
    	
	while(swap = strtok(NULL,delim_pin)){
        	i++;
        	dev->connect_pin[i] = atoi(swap);
    	}
    	dev->connect_quanaity = i+1;
	return No_exp;
}

static exception_t gpio_link_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	x86_gpio_dev *dev = obj->obj;
	dev->gpio_link = obj2;
	dev->gpio_linker_intf = (gpio_linker_pin_intf *)SKY_get_iface(dev->gpio_link, GPIO_LINKER_PIN_INTF);
	if (dev->gpio_linker_intf == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GPIO_LINKER_PIN_INTF, obj2->objname);
	}
	return No_exp;
}

static exception_t gpio_link_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	x86_gpio_dev *dev = obj->obj;
	*obj2 = dev->gpio_link;
	*port = NULL;
	return No_exp;
}

static exception_t gpio_pin_set_data(conf_object_t *obj, uint32_t pin_num, int status)
{
	x86_gpio_dev *dev = obj->obj;
	x86_gpio_reg_t *regs = dev->regs;

	if (pin_num >= GPIO_MAX_PIN_NUM)
	{
		printf("In %s, pin num = %d is larger than GPIO_MAX_PIN_NUM = %d\n", __func__, pin_num, GPIO_MAX_PIN_NUM);
	}
	regs->pin_data[pin_num] = status;
    /*
    	int group = pin_num / 31;
    	int pin = pin_num % 31;
    if(status == PIN_LOW){
        dev->regs->gpio_pin.all &= ~(1 << pin);
    }
    else if(status == PIN_HIGH){
        dev->regs->gpio_pin.all |= (1 << pin);
    }else{
		skyeye_log(Error_log, __FUNCTION__, "Unexpected value:%d", status);
    }
    */
    	return No_exp;
}

void x86_gpio_register_attribute(conf_class_t* clss){
    SKY_register_attribute(clss,"connect_pin",
            get_attr_connect_pin,NULL,
            set_attr_connect_pin,NULL,
            SKY_Attr_Optional,"string",
            "Connected pin, string format: PIN1.PIN2 ... PINn");
}

static uint32_t x86_gpio_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	x86_gpio_dev *dev = conf_obj->obj;
	x86_gpio_reg_t* regs = dev->regs;
	uint32_t regnum;
	regnum = sizeof(x86_gpio_reg_t) / 4;;
	int i;
	for(i =0; i < regnum; i++){
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static exception_t connector_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	x86_gpio_dev *dev = obj->obj;
	dev->gpio_connector = obj2;
	dev->gpio_connector_iface = (skyeye_gpio_connector_intf *)SKY_get_iface(dev->gpio_connector, SKYEYE_GPIO_CONNECTOR);
	if (dev->gpio_connector_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_GPIO_CONNECTOR, obj2->objname);
	}
	return No_exp;
}
 
static exception_t connector_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	x86_gpio_dev *dev = obj->obj;
	*obj2 = dev->gpio_connector;
	*port = NULL;
	return No_exp;
}

exception_t update_gpio(conf_object_t *obj, uint32_t regvalue){
	x86_gpio_dev *dev = obj->obj;
	//dev->regs->gpio_pin.all = regvalue;
	dev->regs->pin_data[0] = regvalue;
	return No_exp;
}

extern "C" void init_x86_gpio();

void init_x86_gpio(){
	static skyeye_class_t class_data = {
		class_name : "x86_simple_gpio",
		class_desc : "x86_simple_gpio",
		new_instance : new_x86_gpio,
		free_instance : free_x86_gpio,
		reset_instance : reset_x86_gpio,
		get_attr : NULL,
		set_attr : NULL
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		conf_obj:NULL,
		read : x86_gpio_read,
		write : x86_gpio_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		conf_obj:NULL,
		get_regvalue_by_id : x86_gpio_get_regvalue_by_id,
		get_regname_by_id : x86_gpio_get_regname_by_id,
		set_regvalue_by_id : x86_gpio_set_regvalue_by_id,
		get_regid_by_name : x86_gpio_get_regid_by_name,
		get_regnum : x86_gpio_get_regnum,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const skyeye_gpio_connector_intf gpio_intf= {
		update : update_gpio,
	};
	SKY_register_iface(clss, SKYEYE_GPIO_CONNECTOR, &gpio_intf);

	static const gpio_pin_set gpio_pin_intf = {
		set_gpio : gpio_pin_set_data,
	};
	SKY_register_iface(clss, GPIO_PIN_SET, &gpio_pin_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			name : SKYEYE_REG_INTF,
			iface : &reg_intf,
		},
		(struct InterfaceDescription) {
			name : MEMORY_SPACE_INTF_NAME, 
			iface : &io_memory,
		},
		(struct InterfaceDescription) {
			name  : SKYEYE_GPIO_CONNECTOR,
			iface : &gpio_intf,
		},
		(struct InterfaceDescription) {
			name  : GPIO_PIN_SET,
			iface : &gpio_pin_intf,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			name : SKYEYE_GPIO_CONNECTOR,
			set  : connector_set,
			get  : connector_get,
		},
		(struct ConnectDescription) {
			name : GPIO_LINKER_PIN_INTF,
			set  : gpio_link_set,
			get  : gpio_link_get,
		},
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	x86_gpio_register_attribute(clss);
}

#endif 

