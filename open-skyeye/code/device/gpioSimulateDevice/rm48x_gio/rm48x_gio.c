#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>
#include "rm48x_gio.h"

#define JUDGE_PIN_STATUS(val, pin_num, dir) (((val >> pin_num) & 1) && ((~dir) >> pin_num) & 1)
#define GIOA_INT 9
#define GIOB_INT 23 

static void gio_raise_signal(conf_object_t *opaque, int pin){
	rm48x_gio_device *dev = opaque->obj;
	int int_number = 0;
	if (pin > 0x0 && pin <= 0x8)
		int_number = GIOA_INT;
	else if (pin >=0x9 && pin >= 0x10)
		int_number = GIOB_INT;

	if (dev->signal_iface != NULL)
		dev->signal_iface->signal(dev->signal, int_number);
}

static void	rising_edge_signal_mode(conf_object_t *opaque, int val){
	rm48x_gio_device *dev = opaque->obj;
	struct registers regs = dev->regs;
	int pin = dev->regs.giooff1 & 0x31;
	if (pin!= 0)
	{
		gio_raise_signal(opaque, pin);
	}
}

static void	falling_edge_signal_mode(conf_object_t *opaque, int val){
	rm48x_gio_device *dev = opaque->obj;
	struct registers regs = dev->regs;
	int pin = dev->regs.giooff2 & 0x31;
	if (pin!= 0)
	{
		gio_raise_signal(opaque, pin);
	}
}

static exception_t rm48x_gio_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_gio_device *dev = opaque->obj;
	switch (offset)
	{
		case GIOGCR0:
			*(uint32_t *)buf = dev->regs.giogcr0;
			break;
		case GIOINTDET:
			*(uint32_t *)buf = dev->regs.giointdet;
			break;
		case GIOPOL:
			*(uint32_t *)buf = dev->regs.giopol;
			break;
		case GIOENASET:
			*(uint32_t *)buf = dev->regs.gioenaset;
			break;
		case GIOENACLR:
			*(uint32_t *)buf = dev->regs.gioenaclr;
			break;
		case GIOLVLSET:
			*(uint32_t *)buf = dev->regs.giolvlset;
			break;
		case GIOLVLCLR:
			*(uint32_t *)buf = dev->regs.giolvlclr;
			break;
		case GIOFLG:
			*(uint32_t *)buf = dev->regs.gioflg;
			break;
		case GIOOFF1:
			*(uint32_t *)buf = dev->regs.giooff1;
			break;
		case GIOOFF2:
			*(uint32_t *)buf = dev->regs.giooff2;
			break;
		case GIOEMU1:
			*(uint32_t *)buf = dev->regs.gioemu1;
			break;
		case GIOEMU2:
			*(uint32_t *)buf = dev->regs.gioemu2;
			break;
		case GIODIRA:
			*(uint32_t *)buf = dev->regs.giodira;
			break;
		case GIODINA:
			*(uint32_t *)buf = dev->regs.giodina;
			break;
		case GIODOUTA:
			*(uint32_t *)buf = dev->regs.giodouta;
			break;
		case GIODSETA:
			*(uint32_t *)buf = dev->regs.giodseta;
			break;
		case GIODCLRA:
			*(uint32_t *)buf = dev->regs.giodclra;
			break;
		case GIOPDRA:
			*(uint32_t *)buf = dev->regs.giopdra;
			break;
		case GIOPULDISA:
			*(uint32_t *)buf = dev->regs.giopuldisa;
			break;
		case GIOPSLA:
			*(uint32_t *)buf = dev->regs.giopsla;
			break;
		case GIODIRB:
			*(uint32_t *)buf = dev->regs.giodirb;
			break;
		case GIODINB:
			*(uint32_t *)buf = dev->regs.giodinb;
			break;
		case GIODOUTB:
			*(uint32_t *)buf = dev->regs.giodoutb;
			break;
		case GIODSETB:
			*(uint32_t *)buf = dev->regs.giodsetb;
			break;
		case GIODCLRB:
			*(uint32_t *)buf = dev->regs.giodclrb;
			break;
		case GIOPDRB:
			*(uint32_t *)buf = dev->regs.giopdrb;
			break;
		case GIOPULDISB:
			*(uint32_t *)buf = dev->regs.giopuldisb;
			break;
		case GIOPSLB:
			*(uint32_t *)buf = dev->regs.giopslb;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_gio\n", offset);
			break;
	}
	//printf(" %s\t offset = 0x%x\t value = 0x%x\n",__FUNCTION__,offset,*(uint32_t*)buf);
	return 0;
}

static exception_t rm48x_gio_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_gio_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//printf(" %s offset = 0x%x value = 0x%x\n",__FUNCTION__,offset,val);
	switch (offset)
	{
		case GIOGCR0:
			dev->regs.giogcr0 = val;
			break;
		case GIOINTDET:
			dev->regs.giointdet = val;
			break;
		case GIOPOL:
			dev->regs.giopol = val;
			break;
		case GIOENASET:
			dev->regs.gioenaset = val;
			break;
		case GIOENACLR:
			dev->regs.gioenaclr = val;
			dev->regs.gioenaset &= ~val;
			break;
		case GIOLVLSET:
			dev->regs.giolvlset = val;
			break;
		case GIOLVLCLR:
			dev->regs.giolvlclr = val;
			dev->regs.giolvlset &= ~val;
			break;
		case GIOFLG:
			dev->regs.gioflg &= ~val;
			break;
		case GIOOFF1:
			dev->regs.giooff1 = val;
			break;
		case GIOOFF2:
			dev->regs.giooff2 = val;
			break;
		case GIOEMU1:
			dev->regs.gioemu1 = val;
			break;
		case GIOEMU2:
			dev->regs.gioemu2 = val;
			break;
		case GIODIRA:
			dev->regs.giodira = val;
			break;
		case GIODINA:
			dev->regs.giodina = val;
			break;
		case GIODOUTA:
			dev->regs.giodouta = val;
			break;
		case GIODSETA:
			{
				dev->regs.giodina = val;
				dev->regs.giodouta = val & ~(dev->regs.giodira);
				if (val != 0)
					rising_edge_signal_mode(opaque, val);

				int i = 0;
				for(i = 0; i <= dev->connect_quanaity; i++){
					if(JUDGE_PIN_STATUS(val, dev->connect_pin[i] % 7, dev->regs.giodira)){
						if(dev->gpio_linker_intf != NULL){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, dev->connect_pin[i], PIN_HIGH); 
						}
					}
				}

			}
			break;
		case GIODCLRA:
			{
				dev->regs.giodclra = val;
				dev->regs.giodouta &= ~(val & (~dev->regs.giodira));
				if (val != 0)
					falling_edge_signal_mode(opaque, val);
				int i = 0;
				for(i = 0; i <= dev->connect_quanaity; i++){
					if(JUDGE_PIN_STATUS(val, dev->connect_pin[i] % 7, dev->regs.giodira)){
						if(dev->gpio_linker_intf != NULL){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, dev->connect_pin[i], PIN_LOW); 
						}
					}
				}
				
			}
			break;
		case GIOPDRA:
			dev->regs.giopdra = val;
			break;
		case GIOPULDISA:
			dev->regs.giopuldisa = val;
			break;
		case GIOPSLA:
			dev->regs.giopsla = val;
			break;
		case GIODIRB:
			dev->regs.giodirb = val;
			break;
		case GIODINB:
			dev->regs.giodinb = val;
			break;
		case GIODOUTB:
			dev->regs.giodoutb = val;
			break;
		case GIODSETB:
			{
				dev->regs.giodinb = val;
				dev->regs.giodoutb = val & ~(dev->regs.giodirb);
				if (val != 0)
					rising_edge_signal_mode(opaque, val);

				int i = 0;
				for(i = 0; i <= dev->connect_quanaity; i++){
					if(JUDGE_PIN_STATUS(val, dev->connect_pin[i] % 7, dev->regs.giodirb)){
						if(dev->gpio_linker_intf != NULL){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, dev->connect_pin[i], PIN_HIGH); 
						}
					}
				}

			}
			break;
		case GIODCLRB:
			{
				dev->regs.giodclrb = val;
				dev->regs.giodoutb &= ~(val & (~dev->regs.giodirb));
				if (val != 0)
					falling_edge_signal_mode(opaque, val);
				int i = 0;
				for(i = 0; i <= dev->connect_quanaity; i++){
					if(JUDGE_PIN_STATUS(val, dev->connect_pin[i] % 7, dev->regs.giodirb)){
						if(dev->gpio_linker_intf != NULL){
							dev->gpio_linker_intf->linker_pin_change_state(dev->gpio_link, opaque, dev->connect_pin[i], PIN_LOW); 
						}
					}
				}
				
			}
			break;
		case GIOPDRB:
			dev->regs.giopdrb = val;
			break;
		case GIOPULDISB:
			dev->regs.giopuldisb = val;
			break;
		case GIOPSLB:
			dev->regs.giopslb = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_gio\n", offset);
			break;
	}
	return 0;
}

static char* rm48x_gio_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t rm48x_gio_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_gio_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t rm48x_gio_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t rm48x_gio_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_gio_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t rm48x_gio_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_rm48x_gio(const char *obj_name)
{
	rm48x_gio_device* dev = skyeye_mm_zero(sizeof(rm48x_gio_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->pin_num  = 16;
	return dev->obj;
}

static exception_t free_rm48x_gio(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t set_attr_connect_pin(void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	rm48x_gio_device* dev = conf_obj->obj;
	dev->connect_list =SKY_attr_string(*value);
	char tranf[128] = "";
	char *delim_pin = ".";
	char *swap;
	strcpy(tranf, dev->connect_list);
	int i = 0;
	dev->connect_pin[i] = atoi(strtok(tranf, delim_pin));
	while (swap = strtok(NULL, delim_pin)) {
		i++;
		dev->connect_pin[i] = atoi(swap);
	}
	dev->connect_quanaity = i + 1;
	return No_exp;
}

static attr_value_t get_attr_connect_pin(void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	rm48x_gio_device* dev = conf_obj->obj;
	attr_value_t connect_list = SKY_make_attr_string(dev->connect_list);
	return connect_list;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_gio_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rm48x_gio_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t gpio_link_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_gio_device *dev = obj->obj;
	dev->gpio_link = obj2;
	dev->gpio_linker_intf = (gpio_linker_pin_intf **)SKY_get_iface(dev->gpio_link, GPIO_LINKER_PIN_INTF);

	if (dev->gpio_linker_intf == NULL){
		if (dev->gpio_link == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GPIO_LINKER_PIN_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GPIO_LINKER_PIN_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t gpio_link_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rm48x_gio_device *dev = obj->obj;
	*obj2 = dev->gpio_link;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_circuit_display (void *arg, conf_object_t *conf_obj, attr_value_t *idx)
{
	rm48x_gio_device* dev = conf_obj->obj;
	attr_value_t circuit_display = SKY_make_attr_string(dev->circuit_display);
	return circuit_display;
}

static exception_t set_attr_circuit_display (void *arg, conf_object_t *conf_obj, attr_value_t *value, attr_value_t *idx)
{
	rm48x_gio_device* dev = conf_obj->obj;
	char *circuit_string = SKY_attr_string(*value);
	dev->circuit_display = skyeye_mm_zero(strlen(circuit_string) + 1);
	strcpy(dev->circuit_display, circuit_string);
	return No_exp;
}

void rm48x_gio_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss,"connect_pin",
			get_attr_connect_pin, NULL,
			set_attr_connect_pin, NULL,
			SKY_Attr_Optional,"string",
			"Connected pin, string format: PIN1.PIN2 ... PINn");
	SKY_register_attribute(clss, "circuit_display", get_attr_circuit_display, NULL, set_attr_circuit_display, NULL, SKY_Attr_Optional, "string", "if or not display in MultiSim");
}

void init_rm48x_gio(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_gio",
		.class_desc = "rm48x_gio",
		.new_instance = new_rm48x_gio,
		.free_instance = free_rm48x_gio
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = rm48x_gio_read,
		.write = rm48x_gio_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_gio_get_regvalue_by_id,
		.get_regname_by_id = rm48x_gio_get_regname_by_id,
		.set_regvalue_by_id = rm48x_gio_set_regvalue_by_id,
		.get_regnum = rm48x_gio_get_regnum,
		.get_regid_by_name = rm48x_gio_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = 
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
		 (struct ConnectDescription) {
			.name = GPIO_LINKER_PIN_INTF,
			.set = gpio_link_set,
			.get = gpio_link_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	
	rm48x_gio_register_attribute(clss);
}
