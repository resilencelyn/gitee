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
#include <skyeye_dev_intf.h>
#include <skyeye_dev_bus_intf.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include "rm48x_iic.h"

static FIFO* CreateFIFO(uint32 FIFOLength)
{
	FIFO *fifo = malloc(sizeof(FIFO));
	if(fifo == NULL)
		return NULL;
	unsigned char  *pfifo;
	pfifo = (unsigned char  *)malloc(FIFOLength);
	if(pfifo == NULL)
		return NULL;
	fifo->pFirst = pfifo;
	fifo->pLast = pfifo + FIFOLength-1;
	fifo->Length = FIFOLength;
	fifo->pIn     = fifo->pFirst;
	fifo->pOut    = fifo->pFirst;
	fifo->Enteres = 0;  

	return fifo;
}

static uint32 WriteFIFO(FIFO *fifo, uint8 *pSource,uint32 WriteLength)
{
	uint32 i;

	for (i = 0; i < WriteLength; i++){
		if (fifo->Enteres >= fifo->Length){
			return i;
		}
		*(fifo->pIn) = *(pSource ++);
		if (fifo->pIn == fifo->pLast)
		{
			fifo->pIn = fifo->pFirst;
		}
		else
		{
			fifo->pIn ++;
		}
		fifo->Enteres ++;
	}
	return i;
}


static uint32 ReadFIFO(FIFO *fifo, uint8 *pAim,uint32 ReadLength)
{
	uint32 i;
	for (i = 0; i < ReadLength; i++){
		if (fifo->Enteres <= 0){
			return i;
		}
		*(pAim ++) = *(fifo->pOut);
		if (fifo->pOut == fifo->pLast)
		{
			fifo->pOut = fifo->pFirst;
		}
		else
		{
			fifo->pOut ++;
		}
		fifo->Enteres -- ;
	} 
	return i;
}


static uint32 CheckFIFOLength(FIFO *fifo)
{
	return fifo->Length;
}


static uint8 *CheckCurrentWritePoint(FIFO *fifo)
{
	return (fifo->pIn);
}

static uint8 *CheckCurrentReadPoint(FIFO *fifo)
{
	return (fifo->pOut);
}


static void FreeFIFO(FIFO *fifo)
{
	free(fifo->pFirst);
}

static void ResetFIFO(FIFO *fifo)
{
	fifo->pIn  = fifo->pFirst;
	fifo->pOut = fifo->pFirst;
	fifo->Enteres = 0;  
}

static uint32 CheckCanWriteNum(FIFO *fifo)
{
	return (fifo->Length - fifo->Enteres);
}

static uint32 CheckCanReadNum(FIFO *fifo)
{
	return fifo->Enteres;
}

static int rm48x_i2c_set_state(conf_object_t* opaque, i2c_device_state_t state, uint8_t address)
{
	rm48x_iic_device *dev=  (rm48x_iic_device *)opaque->obj;
	return 0;
}

static uint8_t rm48x_i2c_read_data(conf_object_t *opaque){
	rm48x_iic_device *dev=  (rm48x_iic_device *)opaque->obj;
	char ch;

	core_signal_intf* signal_iface = dev->signal_iface;

	dev->data_count -= 1;
	ReadFIFO(dev->i2cfifo, &ch, 1);
	if (dev->data_count == 0){
		if (ICMDR_STP) {
			//send STOP condition
			if (dev->i2c_bus_iface != NULL)
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			dev->regs.i2cstr &= ~(0x1 << 12); //Bus is free 
			dev->regs.i2civr = I2C_INTCODE_STOP;
			dev->regs.i2cmdr &= ~(0x1 << 11); //clear stop bit
			dev->regs.i2cstr |= (0x1 << 5); //set stop condition 
			dev->regs.i2cstr &= ~(0x1 << 3);//clear ICRRDY

			if (SCD){
				if (signal_iface != NULL){
					signal_iface->signal(dev->core, dev->interrupt_number);
				}
			}
		}
	}

	return ch;
}


static uint8_t rm48x_i2c_write_data(conf_object_t *opaque, uint8_t value){
	rm48x_iic_device *dev=  (rm48x_iic_device *)opaque->obj;

	core_signal_intf* signal_iface = dev->signal_iface;

	dev->data_count -= 1;
	WriteFIFO(dev->i2cfifo, &value, 1);

	if (dev->data_count == 0){
		if (ICMDR_STP) {
			//send STOP condition
			if (dev->i2c_bus_iface != NULL)
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			dev->regs.i2cstr &= ~(0x1 << 12); //Bus is free 
			dev->regs.i2civr = I2C_INTCODE_STOP;
			dev->regs.i2cmdr &= ~(0x1 << 11); //clear stop bit
			dev->regs.i2cstr |= (0x1 << 5); //set stop condition 
			dev->regs.i2cstr &= ~(0x1 << 4);//clear ICXRDY

			if (SCD){
				if (signal_iface != NULL){
					signal_iface->signal(dev->core, dev->interrupt_number);
				}
			}
		}
	} 

	return 0;
}


static uint8_t rm48x_i2c_get_address(conf_object_t *opaque)
{
	rm48x_iic_device *dev=  (rm48x_iic_device *)opaque->obj;
	return 0;
}


static void check_transmit_stop_condition(rm48x_iic_device *dev){

	core_signal_intf* signal_iface = dev->signal_iface;

	if (dev->send_count == dev->regs.i2ccnt){
		dev->send_count = 0;
		if (ICMDR_STP) {
			//send STOP condition
			if (dev->i2c_bus_iface != NULL)
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			dev->regs.i2cstr &= ~(0x1 << 12); //Bus is free 
			dev->regs.i2civr = I2C_INTCODE_STOP;
			dev->regs.i2cmdr &= ~(0x1 << 11); //clear stop bit
			dev->regs.i2cstr |= (0x1 << 5); //set stop condition 
			dev->regs.i2cstr &= ~(0x1 << 4);//clear ICXRDY

			if (SCD){
				if (signal_iface != NULL){
					signal_iface->signal(dev->core, dev->interrupt_number);
				}
			}
		}
	}else {
		if (ICXRDY || ICDMAC_TX){
			dev->regs.i2civr = I2C_INTCODE_ICXRDY;
			if (signal_iface != NULL){
				signal_iface->signal(dev->core, dev->interrupt_number);
			}
		}
	}
}



static void check_receive_stop_condition(rm48x_iic_device *dev)
{

	core_signal_intf* signal_iface = dev->signal_iface;

	dev->data_count -= 1;

	if (dev->data_count == 0){
		if (ICMDR_STP) {
			//send STOP condition
			if (dev->i2c_bus_iface != NULL)
				dev->i2c_bus_iface->stop(dev->i2c_bus_obj);
			dev->regs.i2cstr &= ~(0x1 << 12); //Bus is free 
			dev->regs.i2civr = I2C_INTCODE_STOP;
			dev->regs.i2cmdr &= ~(0x1 << 11); //clear stop bit
			dev->regs.i2cstr |= (0x1 << 5); //set stop condition 
			dev->regs.i2cstr &= ~(0x1 << 3);//clear ICRRDY

			if (SCD){
				if (signal_iface != NULL){
					signal_iface->signal(dev->core, dev->interrupt_number);
				}
			}
		}
	}
}




static exception_t rm48x_iic_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_iic_device *dev = opaque->obj;
	switch (offset)
	{
		case I2COAR:
			*(uint32_t *)buf = dev->regs.i2coar;
			break;
		case I2CIMR:
			*(uint32_t *)buf = dev->regs.i2cimr;
			break;
		case I2CSTR:
			*(uint32_t *)buf = dev->regs.i2cstr;
			break;
		case I2CCKL:
			*(uint32_t *)buf = dev->regs.i2cckl;
			break;
		case I2CCKH:
			*(uint32_t *)buf = dev->regs.i2cckh;
			break;
		case I2CCNT:
			*(uint32_t *)buf = dev->regs.i2ccnt;
			break;
		case I2CDRR:
			if(dev->i2c_bus_iface != NULL)
				*(uint32_t *)buf = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
			check_receive_stop_condition(dev);
			break;
		case I2CSAR:
			*(uint32_t *)buf = dev->regs.i2csar;
			break;
		case I2CDXR:
			*(uint32_t *)buf = dev->regs.i2cdxr;
			break;
		case I2CMDR:
			*(uint32_t *)buf = dev->regs.i2cmdr;
			break;
		case I2CIVR:
			*(uint32_t *)buf = dev->regs.i2civr;
			break;
		case I2CEMDR:
			*(uint32_t *)buf = dev->regs.i2cemdr;
			break;
		case I2CPSC:
			*(uint32_t *)buf = dev->regs.i2cpsc;
			break;
		case I2CPID1:
			*(uint32_t *)buf = dev->regs.i2cpid1;
			break;
		case I2CPID2:
			*(uint32_t *)buf = dev->regs.i2cpid2;
			break;
		case I2CDMACR:
			*(uint32_t *)buf = dev->regs.i2cdmacr;
			break;
		case I2CPFNC:
			*(uint32_t *)buf = dev->regs.i2cpfnc;
			break;
		case I2CPDIR:
			*(uint32_t *)buf = dev->regs.i2cpdir;
			break;
		case I2CDIN:
			*(uint32_t *)buf = dev->regs.i2cdin;
			break;
		case I2CDOUT:
			*(uint32_t *)buf = dev->regs.i2cdout;
			break;
		case I2CDSET:
			*(uint32_t *)buf = dev->regs.i2cdset;
			break;
		case I2CDCLR:
			*(uint32_t *)buf = dev->regs.i2cdclr;
			break;
		case I2CPDR:
			*(uint32_t *)buf = dev->regs.i2cpdr;
			break;
		case I2CPDIS:
			*(uint32_t *)buf = dev->regs.i2cpdis;
			break;
		case I2CPSEL:
			*(uint32_t *)buf = dev->regs.i2cpsel;
			break;
		case I2CSRS:
			*(uint32_t *)buf = dev->regs.i2csrs;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_iic\n", offset);
			break;
	}
	return 0;
}
static exception_t rm48x_iic_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_iic_device *dev = opaque->obj;

	core_signal_intf* signal_iface = dev->signal_iface;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case I2COAR:
			dev->regs.i2coar = val;
			if (ICMDR_XA == 0)
				dev->own_slave_addr = val & 0x7f;
			else 
				dev->own_slave_addr = val & 0x3ff;
			break;
		case I2CIMR:
			dev->regs.i2cimr = val;
			if ((val & 0x5f) == 0)
				dev->regs.i2civr = 0;
			break;
		case I2CSTR:
			dev->regs.i2cstr = val;
			break;
		case I2CCKL:
			dev->regs.i2cckl = val;
			break;
		case I2CCKH:
			dev->regs.i2cckh = val;
			break;
		case I2CCNT:
			dev->regs.i2ccnt = val;
			if(ICMDR_RM == 0)
				dev->data_count = (val & 0xffff);
			break;
		case I2CDRR:
			dev->regs.i2cdrr = val;
			break;
		case I2CSAR:
			dev->regs.i2csar = val;
			if(ICMDR_XA == 0)
				dev->slave_addr = val & 0x7f;
			else
				dev->slave_addr = val & 0x3ff;
			break;
		case I2CDXR:
			dev->regs.i2cdxr = val;

			dev->send_count += 1;
			if(dev->i2c_bus_iface != NULL)
				dev->i2c_bus_iface->write_data(dev->i2c_bus_obj, val & 0xFF);
			check_transmit_stop_condition(dev);
			break;
		case I2CMDR:
			dev->regs.i2cmdr = val;
			if(ICMDR_FDF)
				dev->regs.i2cstr |= (0x1 << 9);

			if (ICMDR_IRS){
				if (ICMDR_MST){
					//Master mode
					if (ICMDR_STT){ //general a start condition
						if (dev->i2c_bus_iface != NULL)
							dev->i2c_bus_iface->start(dev->i2c_bus_obj, dev->slave_addr & (~0x1));
						if (ICMDR_TRX){
							//transmit data 
							dev->regs.i2cstr |= (0x1 << 4);//set ICXRDY
							check_transmit_stop_condition(dev);
						}else{
							//receive data 
							dev->regs.i2cstr |= (0x1 << 3);//set ICRRDY
							if (ICRRDY || ICDMAC_RX){
								dev->regs.i2civr = I2C_INTCODE_ICRRDY;
								if (signal_iface != NULL){
									signal_iface->signal(dev->core,dev->interrupt_number);
								}
							}
						} 

						dev->regs.i2cmdr &= ~(0x1 << 13); //clear start bit 
						dev->regs.i2cstr |= (0x1 << 12); //Bus is busy
					}
				}else {
					//Slave mode
					if (ICMDR_STT){ //transmit/receive data from master
						if (dev->i2c_bus_iface != NULL)
							dev->regs.i2cdrr = dev->i2c_bus_iface->read_data(dev->i2c_bus_obj);
						if (signal_iface != NULL){
							signal_iface->signal(dev->core, dev->interrupt_number);
						}
						dev->read_irq_count++;
						dev->regs.i2cstr |= (0x1 << 12); //Bus is busy
					}
				}
			}

			break;
		case I2CIVR:
			dev->regs.i2civr = val;
			break;
		case I2CEMDR:
			dev->regs.i2cemdr = val;
			break;
		case I2CPSC:
			dev->regs.i2cpsc = val;
			break;
		case I2CPID1:
			dev->regs.i2cpid1 = val;
			break;
		case I2CPID2:
			dev->regs.i2cpid2 = val;
			break;
		case I2CDMACR:
			dev->regs.i2cdmacr = val;
			break;
		case I2CPFNC:
			dev->regs.i2cpfnc = val;
			break;
		case I2CPDIR:
			dev->regs.i2cpdir = val;
			break;
		case I2CDIN:
			dev->regs.i2cdin = val;
			break;
		case I2CDOUT:
			dev->regs.i2cdout = val;
			break;
		case I2CDSET:
			dev->regs.i2cdset = val;
			break;
		case I2CDCLR:
			dev->regs.i2cdclr = val;
			break;
		case I2CPDR:
			dev->regs.i2cpdr = val;
			break;
		case I2CPDIS:
			dev->regs.i2cpdis = val;
			break;
		case I2CPSEL:
			dev->regs.i2cpsel = val;
			break;
		case I2CSRS:
			dev->regs.i2csrs = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_iic\n", offset);
			break;
	}
	return 0;
}
static char* rm48x_iic_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t rm48x_iic_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_iic_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t rm48x_iic_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t rm48x_iic_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_iic_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t rm48x_iic_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_rm48x_iic(const char *obj_name)
{
	rm48x_iic_device* dev = skyeye_mm_zero(sizeof(rm48x_iic_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	dev->regs.i2cstr = 0x0410;
	dev->regs.i2csar = 0x3ff;
	dev->regs.i2cemdr = 0x1;
	dev->regs.i2cpid1 = 0x146;
	dev->regs.i2cpid2 = 0x5;
	dev->regs.i2cdmacr = 0x3;
	dev->regs.i2cpdis = 0x3;
	dev->i2cfifo = CreateFIFO(1024);

	return dev->obj;
}
static exception_t reset_rm48x_iic(conf_object_t* opaque, const char* args)
{
	rm48x_iic_device *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
	return No_exp;        
}




static exception_t free_rm48x_iic(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}


static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_iic_device *dev = obj->obj;
	dev->core = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->core, CORE_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->core == NULL)
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
	rm48x_iic_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;

}
static exception_t get_attr_i2c_master(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	rm48x_iic_device *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port = NULL;
	return No_exp;
}
static exception_t set_attr_i2c_master(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	rm48x_iic_device *dev = obj->obj;
	dev->i2c_bus_obj = obj2;
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);

	if (dev->i2c_bus_iface == NULL){
		if (dev->i2c_bus_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", I2C_BUS_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", I2C_BUS_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}


static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rm48x_iic_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rm48x_iic_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void omapl138_iic_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

}


void init_rm48x_iic(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_iic",
		.class_desc = "rm48x_iic",
		.new_instance = new_rm48x_iic,
		.reset_instance = reset_rm48x_iic,
		.free_instance = free_rm48x_iic
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = rm48x_iic_read,
		.write = rm48x_iic_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const i2c_device_intf i2c_dev_iface = {
		.set_state = rm48x_i2c_set_state,
		.write_data = rm48x_i2c_write_data,
		.read_data = rm48x_i2c_read_data,
		.get_address = rm48x_i2c_get_address,
	};
	SKY_register_iface(clss, I2C_DEVICE_INTF_NAME, &i2c_dev_iface);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_iic_get_regvalue_by_id,
		.get_regname_by_id = rm48x_iic_get_regname_by_id,
		.set_regvalue_by_id = rm48x_iic_set_regvalue_by_id,
		.get_regnum = rm48x_iic_get_regnum,
		.get_regid_by_name = rm48x_iic_get_regid_by_name
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
		(struct InterfaceDescription)
		{
			.name = I2C_DEVICE_INTF_NAME,
			.iface = &i2c_dev_iface
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_master,
			.get = get_attr_i2c_master,
		},
		(struct ConnectDescription){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	omapl138_iic_register_attribute(clss);
}
