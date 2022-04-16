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
#include <skyeye_uart_ops.h>
#define DEBUG
#include <skyeye_log.h>
#include "htey_hdlc.h"
#include "skyeye_system.h"
static exception_t htey_hdlc_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_hdlc_device *dev = opaque->obj;
	switch (offset)
	{
		case HDLC_STACK:
			*(uint32_t *)buf = dev->regs.hdlc_stack;
			break;
		case HDLC_TXLENL:
			*(uint32_t *)buf = dev->regs.hdlc_txlenl;
			break;
		case HDLC_TXLENH:
			*(uint32_t *)buf = dev->regs.hdlc_txlenh;
			break;
		case HDLC_RX_TX_START_STATUS:{
			*(uint32_t *)buf = dev->regs.hdlc_rx_tx_start_status;
			break;
			}
		case HDLC_RXLENL:
			*(uint32_t *)buf = dev->regs.hdlc_rxlenl;
			break;
		case HDLC_RXLENH:
			*(uint32_t *)buf = dev->regs.hdlc_rxlenh;
			break;
		case HDLC_HEAD:
			*(uint32_t *)buf = dev->regs.hdlc_head;
			break;
		case HDLC_IDLE:
			*(uint32_t *)buf = dev->regs.hdlc_idle;
			break;
		case HDLC_BDRL:
			*(uint32_t *)buf = dev->regs.hdlc_bdrl;
			break;
		case HDLC_BDRH:
			*(uint32_t *)buf = dev->regs.hdlc_bdrh;
			break;
		case HDLC_PERIODL:
			*(uint32_t *)buf = dev->regs.hdlc_periodl;
			break;
		case HDLC_PERIODH:
			*(uint32_t *)buf = dev->regs.hdlc_periodh;
			break;
		case HDLC_IF:
			*(uint32_t *)buf = dev->regs.hdlc_if;
			break;
		case HDLC_IER:
			*(uint32_t *)buf = dev->regs.hdlc_ier;
			break;
		case HDLC_TXBASE:
			*(uint32_t *)buf = dev->regs.hdlc_txbase;
			break;
		default:
			if((offset >= HDLC_RXBASE) && (offset < HDLC_RXTAIL))
			{
				uint32_t index = offset - HDLC_RXBASE;
				*(uint32_t *)buf = *((uint32_t *)(dev->rbuf + index));
				//printf("%d:%x\n", index, *(uint32_t *)buf);
				break;
			}

			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in htey_hdlc\n", offset);
			break;
	}

	return 0;
}
static exception_t htey_hdlc_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	htey_hdlc_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	//skyeye_log(Error_log, __FUNCTION__, "obj = %s, offset = %x, value = %x\n", opaque->objname, offset, val);
	switch (offset)
	{
		case HDLC_STACK:
			dev->regs.hdlc_stack = val;
			break;
		case HDLC_TXLENL:
			dev->regs.hdlc_txlenl = val;
			break;
		case HDLC_TXLENH:
			dev->regs.hdlc_txlenh = val;
			break;
		case HDLC_RX_TX_START_STATUS:{
			//dev->regs.hdlc_rx_tx_start_status = val;
			uint32_t len = 0;
			int i;
			char tx_buf[0x400];
			static char out[256];
			
			if(val & 0xaa){
				len = dev->regs.hdlc_txlenl | (dev->regs.hdlc_txlenh << 8);
				for(i = 0; i < len; i++){
					tx_buf[i] = *((uint32_t *)dev->wbuf + i*4);

				}
				int i;
			if(dev->uart_file_iface){
				//printf("%s send: \n", opaque->objname);
				dev->frame_all_num++;
				if ((dev->frame_all_num >= dev->frame_total_num) && (dev->frame_total_num != 0)) {
					dev->execution_status = 0;
				}
				if ((tx_buf[0] == 0x77) && (tx_buf[1] == 0x11)) {		
					dev->frame_num++;
					if (dev->frame_num == 401)
						if (dev->kgl_iface != NULL)
							dev->kgl_iface->kgl_ctrl(dev->kgl, TUOLUO, 0, NULL);
				}
				for(i = 0; i < len; i++){
					//printf("%x ", tx_buf[i]&0xff);
						dev->uart_file_iface->write(dev->uart_file, tx_buf + i, 1);
						//printf("%02x ", tx_buf[i] & 0xff);
						//if ((i + 1) % 16 == 0) printf("\n");
				}
				//printf("\n\n");
				int *ff = &tx_buf[2];
				float *xx = &tx_buf[278];
				float *yy = &tx_buf[282];
				float *zz = &tx_buf[286];
				sprintf(out, "FRAMEN: %d x: %0.14f y: %0.14f z: %0.14f\n", *ff, *xx, *yy, *zz);
			}
				if (dev->term_iface != NULL) {
						dev->term_iface->write(dev->term, &out, strlen(out));
				}
				if(dev->serial_iface){
					
					dev->serial_iface->write_chars(dev->serial, tx_buf, len);
				}
				
			}
			if(val & 0x55)
			{

			}
			break;
		}
		case HDLC_RXLENL:
			dev->regs.hdlc_rxlenl = val;
			break;
		case HDLC_RXLENH:
			dev->regs.hdlc_rxlenh = val;
			break;
		case HDLC_HEAD:
			dev->regs.hdlc_head = val;
			break;
		case HDLC_IDLE:
			dev->regs.hdlc_idle = val;
			break;
		case HDLC_BDRL:
			dev->regs.hdlc_bdrl = val;
			break;
		case HDLC_BDRH:
			dev->regs.hdlc_bdrh = val;
			break;
		case HDLC_PERIODL:
			dev->regs.hdlc_periodl = val;
			break;
		case HDLC_PERIODH:
			dev->regs.hdlc_periodh = val;
			break;
		case HDLC_IF:
			dev->regs.hdlc_if = 0x0;
			break;
		case HDLC_IER:
			dev->regs.hdlc_ier = val;
			break;
		default:
			if((offset >= HDLC_TXBASE) && (offset < HDLC_TXTAIL))
			{
				uint32_t index = offset - HDLC_TXBASE;
				*((uint32_t *)(dev->wbuf + index)) = val;
				break;
			}
			if((offset >= HDLC_RXBASE) && (offset < HDLC_RXTAIL))
			{
				uint32_t index = offset - HDLC_RXBASE;
				*((uint32_t *)(dev->rbuf + index)) = val;
				break;
			}
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in htey_hdlc\n", offset);
			break;
	}
	return 0;
}
static char* htey_hdlc_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t htey_hdlc_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	htey_hdlc_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t htey_hdlc_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t htey_hdlc_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	htey_hdlc_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t htey_hdlc_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_htey_hdlc(char *obj_name)
{
	htey_hdlc_device* dev = skyeye_mm_zero(sizeof(htey_hdlc_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->rbuf = skyeye_mm_zero(0x4000);
	dev->wbuf = skyeye_mm_zero(0x4000);
	dev->execution_status = 1;
	dev->frame_num = 0;
	dev->frame_all_num = 0;
	return dev->obj;
}
static exception_t free_htey_hdlc(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
static exception_t serial_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_hdlc_device *dev = obj->obj;
	dev->serial = obj2;
	dev->serial_iface = (skyeye_serial_device_t*)SKY_get_iface(obj2, SERIAL_DEVICE_INTERFACE);
	if (dev->serial_iface == NULL){
		if (dev->serial == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SERIAL_DEVICE_INTERFACE);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SERIAL_DEVICE_INTERFACE, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t serial_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	htey_hdlc_device *dev = obj->obj;
	*obj2 = dev->serial;
	*port = NULL;
	return No_exp;
}

static exception_t serial_write(conf_object_t *obj, uint32_t value)
{
	htey_hdlc_device *dev = obj->obj;

	return No_exp;
}	

static exception_t serial_write_chars(conf_object_t *obj, void *buf, uint32_t cnt)
{
	htey_hdlc_device *dev = obj->obj;
	char *ch = buf;
	int i;
	//printf("%s receive: ", obj->objname);
	for(i = 0; i < cnt; i++){
			*((uint32_t *)(dev->rbuf + i*16)) = ch[i];
		//	printf("%x ", ch[i]&0xff);
	}
	//printf("\n");

	dev->regs.hdlc_rxlenl = cnt	&0xff;
	dev->regs.hdlc_rxlenh = cnt	>> 8;

	dev->regs.hdlc_if |= 0x2;
	dev->regs.hdlc_rx_tx_start_status = 0;
	return No_exp;
}	


static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_hdlc_device *dev = obj->obj;
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL){
		if (dev->uart_file == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	htey_hdlc_device *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

static exception_t kgl_intf_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_hdlc_device *dev = obj->obj;
	dev->kgl = obj2;
	dev->kgl_iface = (htey_kgl_intf *)SKY_get_iface(dev->kgl, HTEY_KGL_INTF);
	if (dev->kgl_iface == NULL){
		if (dev->kgl == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", HTEY_KGL_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", HTEY_KGL_INTF, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}
 
static exception_t kgl_intf_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	htey_hdlc_device *dev = obj->obj;
	*obj2 = dev->kgl;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* idx){
	htey_hdlc_device *dev = obj->obj;
	attr_value_t frame_total_num = SKY_make_attr_uinteger(dev->frame_total_num);
	return frame_total_num;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	htey_hdlc_device *dev = obj->obj;
	dev->frame_total_num = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_int_status(void* arg, conf_object_t* obj, attr_value_t* idx){
	htey_hdlc_device *dev = obj->obj;
	attr_value_t execution_status = SKY_make_attr_uinteger(dev->execution_status);
	return execution_status;
}
static exception_t set_attr_int_status(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	htey_hdlc_device *dev = obj->obj;
	dev->execution_status = SKY_attr_uinteger(*value);
	return No_exp;
}

static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	htey_hdlc_device* dev = (htey_hdlc_device*)(obj->obj);
	dev->term = obj2;
	dev->term_iface = (memory_space_intf*)SKY_get_iface(dev->term, SKYEYE_UART_INTF);

	if (dev->term_iface == NULL){
		if (dev->term == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	htey_hdlc_device* dev = (htey_hdlc_device*)(obj->obj);
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}

static class_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "frame_total_num", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "frame number");
	SKY_register_attribute(clss, "execution_status", get_attr_int_status, NULL, set_attr_int_status, NULL, SKY_Attr_Optional, "uinteger", "execution status");
}

void init_htey_hdlc(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "htey_hdlc",
		.class_desc = "htey_hdlc",
		.new_instance = new_htey_hdlc,
		.free_instance = free_htey_hdlc
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = htey_hdlc_read,
		.write = htey_hdlc_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = htey_hdlc_get_regvalue_by_id,
		.get_regname_by_id = htey_hdlc_get_regname_by_id,
		.set_regvalue_by_id = htey_hdlc_set_regvalue_by_id,
		.get_regnum = htey_hdlc_get_regnum,
		.get_regid_by_name = htey_hdlc_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static skyeye_serial_device_t serial_device = {
		.write = serial_write,
		.write_chars = serial_write_chars,
		.receive_ready = NULL,
	};
	SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);

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
		(struct InterfaceDescription) {
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = uart_term_set,
			.get = uart_term_get,
		},
		(struct ConnectDescription ){
			.name = SERIAL_DEVICE_INTERFACE,
			.set = serial_set,
			.get = serial_get,
		},
		(struct ConnectDescription ){
			.name = UART_FILE_INTF,
			.set = uart_file_set,
			.get = uart_file_get,
		},
		(struct ConnectDescription ){
			.name = HTEY_KGL_INTF,
			.set = kgl_intf_set,
			.get = kgl_intf_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}
