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
#include <skyeye_uart_ops.h>
#include <fifo_queue.h>
#include "spi_c6678.h"

static exception_t spi_c6678_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	spi_c6678_device *dev = opaque->obj;

	switch (offset) {
	case SPIGCR0:
		*(uint32_t *)buf = dev->regs.spigcr0;
		break;
	case SPIGCR1:
		*(uint32_t *)buf = dev->regs.spigcr1;
		break;
	case SPIINT0:
		*(uint32_t *)buf = dev->regs.spiint0;
		break;
	case SPILVL:
		*(uint32_t *)buf = dev->regs.spilvl;
		break;
	case SPIFLG:
		*(uint32_t *)buf = dev->regs.spiflg;
		break;
	case SPIPC0:
		*(uint32_t *)buf = dev->regs.spipc0;
		break;
	case SPIDAT0:
		*(uint32_t *)buf = dev->regs.spidat0;
		break;
	case SPIDAT1:
		*(uint32_t *)buf = dev->regs.spidat1;
		break;
	case SPIBUF: {
		*(uint32_t *)buf = dev->regs.spibuf.all;
		//printf("%s  offset: 0x%02x val: 0x%08x\n", __func__, offset, *(uint32_t *)buf);
/*
		uint32_t data = 0;
		if (CheckCanReadNum(dev->ReceiveBuff) > 0) {
			ReadFIFO(dev->ReceiveBuff, &data, 1);
		}
		printf("%s  read data: 0x%x\n\n", __func__, data);
		*(uint32_t *)buf = data;
*/
		break;
	}
	case SPIEMU:
		*(uint32_t *)buf = dev->regs.spiemu;
		//printf("%s  offset: 0x%02x val: 0x%08x\n", __func__, offset, *(uint32_t *)buf);
		break;
	case SPIDELAY:
		*(uint32_t *)buf = dev->regs.spidelay;
		break;
	case SPIDEF:
		*(uint32_t *)buf = dev->regs.spidef;
		break;
	case SPIFMT0:
		*(uint32_t *)buf = dev->regs.spifmt0;
		break;
	case SPIFMT1:
		*(uint32_t *)buf = dev->regs.spifmt1;
		break;
	case SPIFMT2:
		*(uint32_t *)buf = dev->regs.spifmt2;
		break;
	case SPIFMT3:
		*(uint32_t *)buf = dev->regs.spifmt3;
		break;
	case INTVEC0:
		*(uint32_t *)buf = dev->regs.intvec0;
		break;
	case INTVEC1:
		*(uint32_t *)buf = dev->regs.intvec1;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in spi_c6678\n", offset);
		break;
	}

	//printf("%s  offset: 0x%02x val: 0x%08x\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t spi_c6678_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	spi_c6678_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	//printf("%s offset: 0x%02x val: 0x%08x\n", __func__, offset, *(uint32_t *)buf);
	switch (offset) {
	case SPIGCR0:
		dev->regs.spigcr0 = val;
		break;
	case SPIGCR1:
		dev->regs.spigcr1 = val;
		// 判断CLKMOD MASTER
		if (CSL_FEXTR(dev->regs.spigcr1, 1, 0) == 0x3) {
			dev->spi_mode = MASTER;
		} else {
			dev->spi_mode = SLAVE;
		}

		// ENABLE 位
		if (CSL_FEXTR(dev->regs.spigcr1, 24, 24) == 0x0) {
			// Writing a 0 to the SPIGCR1.ENABLE forces the TXDATA[15:0] field to 0.
			CSL_FINSR(dev->regs.spidat0, 15, 0, 0x0);
			CSL_FINSR(dev->regs.spidat1, 15, 0, 0x0);
		}
		break;
	case SPIINT0:
		dev->regs.spiint0 = val;
		break;
	case SPILVL:
		dev->regs.spilvl = val;
		break;
	case SPIFLG:
		dev->regs.spiflg = val;
		break;
	case SPIPC0: {
		dev->regs.spipc0 = val;
		// 使能SPISOMI引脚
		if (CSL_FEXTR(dev->regs.spipc0, 11, 11) == SPISOMI_SPI_FUNCTIONAL_PIN) {
			// Slave out, master in pin function.
			if (dev->spi_mode == MASTER) {
				;
			} else if (dev->spi_mode == SLAVE) {
				;
			}
		}
		// 使能SPISIMO引脚
		if (CSL_FEXTR(dev->regs.spipc0, 10, 10) == SPISIMO_SPI_FUNCTIONAL_PIN) {
			// Slave in, master out pin function.
			if (dev->spi_mode == MASTER) {
				;
			} else if (dev->spi_mode == SLAVE) {
				;
			}
		}
		// 使能SPICLK引脚
		if (CSL_FEXTR(dev->regs.spipc0, 9, 9) == SPICLK_SPI_FUNCTIONAL_PIN) {
			// SSPI clock pin function. 
			
		} else {
			
		}

		// 使能 SPISCS[i] 引脚, 片选信号引脚
		int i;
		for (i = 0; i < CS_NUM; i++) {
			if (CSL_FEXTR(dev->regs.spipc0, i, i) == SPISCS_i_SPI_FUNCTIONAL_PIN) {
				;
			}
		}
		break;
	}
	case SPIDAT0: {
		//printf("%s offset: 0x%02x val: 0x%08x\n", __func__, offset, *(uint32_t *)buf);
		dev->regs.spidat0 = val;
		// Transfer data (value = 0-FFFFh)
		uint16_t TXDATA;
		TXDATA = CSL_FEXTR(dev->regs.spidat1, 15, 0);
		break;
	}
	case SPIDAT1: {
		//printf("%s offset: 0x%02x val: 0x%08x\n", __func__, offset, *(uint32_t *)buf);

		// 
		uint16_t CSHOLD;
		CSHOLD = CSL_FEXTR(val, 28, 28);

		// Transfer data (value = 0-FFFFh)
		uint16_t TXDATA;
		TXDATA = CSL_FEXTR(val, 15, 0);

		dev->regs.spidat1 = val;
		// 片选信号引脚号码
		int CSNR = CSL_FEXTR(dev->regs.spidat1, 23, 16);

		//printf("TXDATA: 0x%X  CSNR: 0x%X  CSHOLD: 0x%X\n", TXDATA, CSNR, CSHOLD);

		// 判断是否使能SPI
		if (CSL_FEXTR(dev->regs.spigcr1, 24, 24) == 0x1) {
			// 发送数据
			#if 1
			int i;
			for (i = 0; i < CS_NUM; i++) {
				if (CSL_FEXTR(CSNR, i, i) == 0x1) {
					if (dev->serial[i]) {
						dev->serial_iface[i]->write(dev->serial[i], TXDATA & 0xffff);
						//dev->serial_iface[i]->write_chars(dev->serial[i], buf, count);
					}
				}
			}
			#endif
			#if 0
			int i;
			for (i = 0; i < CS_NUM; i++) {
				if (CSL_FEXTR(dev->regs.spidef, i, i) == 0x1) {
					if (dev->serial[i]) {
						printf("i: %d\n", i);
						dev->serial_iface[i]->write(dev->serial[i], TXDATA & 0xffff);
						//dev->serial_iface[i]->write_chars(dev->serial[i], buf, count);
					}
				}
			}
			#endif
		}
		break;
	}
	case SPIBUF:
		dev->regs.spibuf.all = val;
		break;
	case SPIEMU:
		dev->regs.spiemu = val;
		break;
	case SPIDELAY:
		dev->regs.spidelay = val;
		break;
	case SPIDEF:
		dev->regs.spidef = val;
		break;
	case SPIFMT0:
		dev->regs.spifmt0 = val;
		break;
	case SPIFMT1:
		dev->regs.spifmt1 = val;
		break;
	case SPIFMT2:
		dev->regs.spifmt2 = val;
		break;
	case SPIFMT3:
		dev->regs.spifmt3 = val;
		break;
	case INTVEC0:
		dev->regs.intvec0 = val;
		break;
	case INTVEC1:
		dev->regs.intvec1 = val;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in spi_c6678\n", offset);
		break;
	}

	return 0;
}

static exception_t serial_receive_data_word(conf_object_t *obj, uint32_t data)
{
	spi_c6678_device *dev = obj->obj;

	//printf("spi: %s data: 0x%x \n\n", __func__, data);

	dev->regs.spibuf.RXDATA = data & 0xFFFF;
	dev->regs.spibuf.RXEMPTY = 0;			// 接收到数据

/*
	if (CheckCanReadNum(dev->ReceiveBuff) <= 32) {
		WriteFIFO(dev->ReceiveBuff, &data, 1);
	}
*/

	return No_exp;
}

static char* spi_c6678_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t spi_c6678_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	spi_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t spi_c6678_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t spi_c6678_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	spi_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t spi_c6678_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_spi_c6678(char *obj_name)
{
	spi_c6678_device* dev = skyeye_mm_zero(sizeof(spi_c6678_device));
	dev->obj = new_conf_object(obj_name, dev);

	struct registers *regs = &(dev->regs);

	//dev->regs.spibuf.RXEMPTY = 1;			// 接收数据缓存为空

	dev->ReceiveBuff =  CreateFIFO(32);

	return dev->obj;
}

static exception_t free_spi_c6678(conf_object_t* opaque)
{
	spi_c6678_device* dev = skyeye_mm_zero(sizeof(spi_c6678_device));

	FreeFIFO(dev->ReceiveBuff);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t serial_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	spi_c6678_device *dev = obj->obj;

	if ((index >= 0) && (index < CS_NUM)) {
		dev->serial[index] = obj2;
		dev->serial_iface[index] = (skyeye_serial_device_t*)SKY_get_iface(obj2, SERIAL_DEVICE_INTERFACE);
		if (dev->serial_iface[index] == NULL){
			skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.", SERIAL_DEVICE_INTERFACE, obj2->objname);
			return Not_found_exp;
		}
	} else {
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail. index out of bounds", SERIAL_DEVICE_INTERFACE, obj2->objname);
	}
	return No_exp;
}
 
static exception_t serial_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	spi_c6678_device *dev = obj->obj;
	*obj2 = dev->serial[index];
	*port = NULL;
	return No_exp;
}

void init_spi_c6678(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6678_spi",
		.class_desc = "c6678_spi",
		.new_instance = new_spi_c6678,
		.free_instance = free_spi_c6678
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = spi_c6678_read,
		.write = spi_c6678_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = spi_c6678_get_regvalue_by_id,
		.get_regname_by_id = spi_c6678_get_regname_by_id,
		.set_regvalue_by_id = spi_c6678_set_regvalue_by_id,
		.get_regnum = spi_c6678_get_regnum,
		.get_regid_by_name = spi_c6678_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static skyeye_serial_device_t serial_device = {
		.write = serial_receive_data_word,
		.write_chars = NULL,
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
		(struct ConnectDescription ){
			.name = SERIAL_DEVICE_INTERFACE,
			.set = serial_set,
			.get = serial_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
