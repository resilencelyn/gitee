#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_uart.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_system.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>
#include "uart_mpc5121.h"

void raise_signal(conf_object_t *obj){
	uart_mpc5121_device *dev = obj->obj;
	dev->signal_iface->raise_signal(dev->signal, dev->irq_num);
}
static exception_t uart_mpc5121_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	uart_mpc5121_device *dev = opaque->obj;
	switch (offset)
	{
		case MR1_MR2:
			*(uint32_t *)buf = dev->regs.mr1_mr2;
			break;
		case SR_CSR:
			*(uint32_t *)buf = dev->regs.sr_csr;
			break;
		case CR:
			*(uint32_t *)buf = dev->regs.cr;
			break;
		case RB_TB:
			*(uint32_t *)buf = dev->regs.rb_tb;
			break;
		case IPCR_ACR:
			*(uint32_t *)buf = dev->regs.ipcr_acr;
			break;
		case ISR_IMR:
			*(uint32_t *)buf = dev->regs.isr_imr;
			break;
		case CTUR:
			*(uint32_t *)buf = dev->regs.ctur;
			break;
		case CTLR:
			*(uint32_t *)buf = dev->regs.ctlr;
			break;
		case CCR:
			*(uint32_t *)buf = dev->regs.ccr;
			break;
		case AC97SLOTS:
			*(uint32_t *)buf = dev->regs.ac97slots;
			break;
		case AC97CMD:
			*(uint32_t *)buf = dev->regs.ac97cmd;
			break;
		case AC97DATA:
			*(uint32_t *)buf = dev->regs.ac97data;
			break;
		case IP:
			*(uint32_t *)buf = dev->regs.ip;
			break;
		case OP1:
			*(uint32_t *)buf = dev->regs.op1;
			break;
		case OP0:
			*(uint32_t *)buf = dev->regs.op0;
			break;
		case SICR:
			*(uint32_t *)buf = dev->regs.sicr;
			break;
		case PSCN_TX_CMD:
			*(uint32_t *)buf = dev->regs.pscn_tx_cmd;
			break;
		case PSCN_TX_ALARM:
			*(uint32_t *)buf = dev->regs.pscn_tx_alarm;
			break;
		case PSCN_TX_SR:
			*(uint32_t *)buf = dev->regs.pscn_tx_sr;
			break;
		case PSCN_TX_ISR:
			*(uint32_t *)buf = dev->regs.pscn_tx_isr;
			break;
		case PSCN_TX_IMR:
			*(uint32_t *)buf = dev->regs.pscn_tx_imr;
			break;
		case PSCN_TX_COUNT:
			*(uint32_t *)buf = dev->regs.pscn_tx_count;
			break;
		case PSCN_TX_POINTER:
			*(uint32_t *)buf = dev->regs.pscn_tx_pointer;
			break;
		case PSCN_TX_SIZE:
			*(uint32_t *)buf = dev->regs.pscn_tx_size;
			break;
		case PSCN_TX_DATA:
			*(uint32_t *)buf = dev->regs.pscn_tx_data;
			break;
		case PSCN_RX_CMD:
			*(uint32_t *)buf = dev->regs.pscn_rx_cmd;
			break;
		case PSCN_RX_ALARM:
			*(uint32_t *)buf = dev->regs.pscn_rx_alarm;
			break;
		case PSCN_RX_STAT:
			*(uint32_t *)buf = dev->regs.pscn_rx_stat;
			break;
		case PSCN_RX_ISR:
			*(uint32_t *)buf = dev->regs.pscn_rx_isr;
			break;
		case PSCN_RX_IMR:
			*(uint32_t *)buf = dev->regs.pscn_rx_imr;
			break;
		case PSCN_RX_COUNT:
			*(uint32_t *)buf = dev->regs.pscn_rx_count;
			break;
		case PSCN_RX_POINTER:
			*(uint32_t *)buf = dev->regs.pscn_rx_pointer;
			break;
		case PSCN_RX_SIZE:
			*(uint32_t *)buf = dev->regs.pscn_rx_size;
			break;
		case PSCN_RX_DATA:
			*(uint32_t *)buf = dev->regs.pscn_rx_data;
			dev->regs.pscn_rx_stat &= ~ALARM;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in uart_mpc5121\n", offset);
			break;
	}

	return 0;
}
static exception_t uart_mpc5121_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	uart_mpc5121_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case MR1_MR2:
			dev->regs.mr1_mr2 = val;
			break;
		case SR_CSR:
			dev->regs.sr_csr = val;
			break;
		case CR:
			dev->regs.cr = val;
			break;
		case RB_TB:
			dev->regs.rb_tb = val;
			break;
		case IPCR_ACR:
			dev->regs.ipcr_acr = val;
			break;
		case ISR_IMR:
			dev->regs.isr_imr = val;
			
			break;
		case CTUR:
			dev->regs.ctur = val;
			break;
		case CTLR:
			dev->regs.ctlr = val;
			break;
		case CCR:
			dev->regs.ccr = val;
			break;
		case AC97SLOTS:
			dev->regs.ac97slots = val;
			break;
		case AC97CMD:
			dev->regs.ac97cmd = val;
			break;
		case AC97DATA:
			dev->regs.ac97data = val;
			break;
		case IP:
			dev->regs.ip = val;
			break;
		case OP1:
			dev->regs.op1 = val;
			break;
		case OP0:
			dev->regs.op0 = val;
			break;
		case SICR:
			dev->regs.sicr = val;
			break;
		case PSCN_TX_CMD:
			dev->regs.pscn_tx_cmd = val;
			break;
		case PSCN_TX_ALARM:
			dev->regs.pscn_tx_alarm = val;
			break;
		case PSCN_TX_SR:
			dev->regs.pscn_tx_sr = val;
			break;
		case PSCN_TX_ISR:
			dev->regs.pscn_tx_isr &= ~val;
			break;
		case PSCN_TX_IMR:
			dev->regs.pscn_tx_imr = val;
			break;
		case PSCN_TX_COUNT:
			dev->regs.pscn_tx_count = val;
			break;
		case PSCN_TX_POINTER:
			dev->regs.pscn_tx_pointer = val;
			break;
		case PSCN_TX_SIZE:
			dev->regs.pscn_tx_size = val;
			break;
		case PSCN_TX_DATA:{
				dev->regs.pscn_tx_data = val;
				dev->tx_fifo_empty = F_FALSE;
				if(dev->term){
					dev->term_iface->write(dev->term, &val, 1);
				}
				if (dev->uart_file)
					dev->uart_file_iface->write(dev->uart_file, &val, 1);
				dev->tx_fifo_empty = F_TRUE;
			}
			break;
		case PSCN_RX_CMD:
			dev->regs.pscn_rx_cmd = val;
			break;
		case PSCN_RX_ALARM:
			dev->regs.pscn_rx_alarm = val;
			break;
		case PSCN_RX_STAT:
			dev->regs.pscn_rx_stat = val;
			break;
		case PSCN_RX_ISR:
			dev->regs.pscn_rx_isr &= ~val;
			break;
		case PSCN_RX_IMR:
			dev->regs.pscn_rx_imr = val;
			break;
		case PSCN_RX_COUNT:
			dev->regs.pscn_rx_count = val;
			break;
		case PSCN_RX_POINTER:
			dev->regs.pscn_rx_pointer = val;
			break;
		case PSCN_RX_SIZE:
			dev->regs.pscn_rx_size = val;
			break;
		case PSCN_RX_DATA:
			dev->regs.pscn_rx_data = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in uart_mpc5121\n", offset);
			break;
	}
	return 0;
}
static char* uart_mpc5121_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t uart_mpc5121_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	uart_mpc5121_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t uart_mpc5121_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t uart_mpc5121_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	uart_mpc5121_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t uart_mpc5121_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

void fifo_check(conf_object_t  *obj){
	uart_mpc5121_device *dev = obj->obj;
	uint32_t fifoc_int;

	if (dev->dev_enable == 0)
		return;
	//check TX interrupt
	if((dev->regs.pscn_tx_imr & (ALARM|EMPTY)) && (dev->tx_fifo_empty == F_TRUE)){
		dev->sfifoc_iface->read_register(dev->sfifoc, FIFOC_INT, &fifoc_int, L_OP);
		fifoc_int |= (1 << dev->id);
		dev->sfifoc_iface->write_register(dev->sfifoc, FIFOC_INT, fifoc_int, L_OP);
		dev->regs.pscn_tx_isr |= dev->regs.pscn_tx_imr & (ALARM|EMPTY);
		dev->regs.pscn_tx_sr |= dev->regs.pscn_tx_imr & (ALARM|EMPTY);
	}

	//check RX interrupt
	char ch;
	int ret;
	if(dev->term){
		ret = dev->term_iface->read(dev->term, &ch, 1);
		if(ret){
			dev->regs.pscn_rx_data = ch;
			if((dev->regs.pscn_rx_imr & (ALARM))){
				dev->sfifoc_iface->read_register(dev->sfifoc, FIFOC_INT, &fifoc_int, L_OP);
				fifoc_int |= (1 << (dev->id + 16));
				dev->sfifoc_iface->write_register(dev->sfifoc, FIFOC_INT, fifoc_int, L_OP);
				dev->regs.pscn_rx_isr |= ALARM;
				dev->regs.pscn_rx_stat |= ALARM;
			} 
		}
	}
		
	//add some other interrupt check
	//
	if(dev->regs.pscn_tx_isr || dev->regs.pscn_rx_isr){
		raise_signal(obj);
	}
	
}

static conf_object_t* new_uart_mpc5121(char *obj_name)
{
	uart_mpc5121_device* dev = skyeye_mm_zero(sizeof(uart_mpc5121_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	dev->tx_fifo_empty = F_TRUE;
	dev->rx_fifo_full = F_FALSE;
	int id;
	system_register_timer_handler(dev->obj, 10, fifo_check, SCHED_NORMAL|SCHED_US);
	dev->dev_enable = 1;
	return dev->obj;
}

static exception_t free_uart_mpc5121(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t uart_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_mpc5121_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;

	return No_exp;
}

static exception_t uart_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_mpc5121_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* idx){
	uart_mpc5121_device *dev = obj->obj;
	attr_value_t irq_num = SKY_make_attr_uinteger(dev->irq_num);
	return irq_num;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	uart_mpc5121_device *dev = obj->obj;
	dev->irq_num = SKY_attr_uinteger(*value);
	return No_exp;
}


static attr_value_t get_attr_id(void* arg, conf_object_t* obj, attr_value_t* idx){
	uart_mpc5121_device *dev = obj->obj;
	return SKY_make_attr_uinteger(dev->irq_num);
}
static exception_t set_attr_id(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	uart_mpc5121_device *dev = obj->obj;
	dev->id = SKY_attr_uinteger(*value);
	return No_exp;
}


static void class_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "irq_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number");
	SKY_register_attribute(clss, "id", get_attr_id, NULL, set_attr_id, NULL, SKY_Attr_Optional, "uinteger", "psc id number, 0 ~ 11");
	return;
}


static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_mpc5121_device *dev = obj->obj;
	dev->term = obj2;
	dev->term_iface = (skyeye_uart_intf*)SKY_get_iface(obj2, SKYEYE_UART_INTF);
	if (dev->term_iface == NULL){
		if (dev->term == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}

	dev->dev_enable = 1;
	return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_mpc5121_device *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}
static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_mpc5121_device *dev = obj->obj;
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL){
		if (dev->uart_file == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;
	return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_mpc5121_device *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

static exception_t sfifoc_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	uart_mpc5121_device *dev = obj->obj;
	dev->sfifoc = obj2;
	dev->sfifoc_iface = (mpc5121_sfifoc_intf*)SKY_get_iface(obj2, MPC5121_SFIFOC_INTF);
	if (dev->sfifoc == NULL){
		if (dev->sfifoc == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Get %s interface from NULL object fail.", MPC5121_SFIFOC_INTF);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.", MPC5121_SFIFOC_INTF, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;
	return No_exp;
}

static exception_t sfifoc_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	uart_mpc5121_device *dev = obj->obj;
	*obj2 = dev->sfifoc;
	*port = NULL;
	return No_exp;
}


void init_uart_mpc5121(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "mpc5121_uart",
		.class_desc = "mpc5121_uart",
		.new_instance = new_uart_mpc5121,
		.free_instance = free_uart_mpc5121,
		.module_type = SKYML_ONCHIP(SC_mpc5121),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = uart_mpc5121_read,
		.write = uart_mpc5121_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = uart_mpc5121_get_regvalue_by_id,
		.get_regname_by_id = uart_mpc5121_get_regname_by_id,
		.set_regvalue_by_id = uart_mpc5121_set_regvalue_by_id,
		.get_regnum = uart_mpc5121_get_regnum,
		.get_regid_by_name = uart_mpc5121_get_regid_by_name
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
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = uart_signal_set,
			.get = uart_signal_get,
		},
		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = uart_term_set,
			.get = uart_term_get,
		},
		(struct ConnectDescription ){
			.name = UART_FILE_INTF,
			.set = uart_file_set,
			.get = uart_file_get,
		},
		(struct ConnectDescription ){
			.name = MPC5121_SFIFOC_INTF,
			.set = sfifoc_set,
			.get = sfifoc_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	class_register_attribute(clss);
}
