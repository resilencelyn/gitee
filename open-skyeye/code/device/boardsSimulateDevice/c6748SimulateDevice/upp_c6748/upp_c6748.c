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
#include <skyeye_system.h>
#include "upp_c6748.h"
#include "upp_fifo.h"

#define RECEIVE_MODE 0
#define TRANSMIT_MODE 1 
#define DUPLEX_0_MODE 2
#define DUPLEX_1_MODE 3

#define SIGNAL_CHANNEL_MODE 0
#define DUAL_CHANNEL_MODE 1

#define CHANNEL_1 0
#define CHANNEL_2 1

static void transfer_data_A(conf_object_t *opaque)
{
	upp_c6748_device *dev = opaque->obj;
	if(dev->regs.upctl.bit.mode == TRANSMIT_MODE){
		char data[4];
		if(CheckCanReadNum(dev->channel_fifo[0]) >= 4){
			ReadFIFO(dev->channel_fifo[0], data,4);
			if(dev->upp_linker_intf){
				while(dev->upp_linker_intf->upp_linker_data_send(dev->upp_linker,opaque,0,data,4));
			}
		}
	}else if(dev->regs.upctl.bit.mode == DUPLEX_1_MODE){
		char data[4];
		if(CheckCanReadNum(dev->channel_fifo[0]) >= 4){
			ReadFIFO(dev->channel_fifo[0], data,4);
			if(dev->upp_linker_intf){
				while(dev->upp_linker_intf->upp_linker_data_send(dev->upp_linker,opaque,0,data,4));
			}
		}
	}

}
static void transfer_data_B(conf_object_t *opaque)
{
	upp_c6748_device *dev = opaque->obj;
	if(dev->regs.upctl.bit.mode == TRANSMIT_MODE){
		char data[4];
		if(CheckCanReadNum(dev->channel_fifo[1]) >= 4){
			ReadFIFO(dev->channel_fifo[1], data,4);
			if(dev->upp_linker_intf){
				while(dev->upp_linker_intf->upp_linker_data_send(dev->upp_linker,opaque,1,data,4));
			}
		}
	}else if(dev->regs.upctl.bit.mode == DUPLEX_0_MODE){
		char data[4];
		if(CheckCanReadNum(dev->channel_fifo[1]) >= 4){
			ReadFIFO(dev->channel_fifo[1], data,4);
			if(dev->upp_linker_intf){
				while(dev->upp_linker_intf->upp_linker_data_send(dev->upp_linker,opaque,1,data,4));
			}
		}
	}
}
static void write_memory(conf_object_t *opaque,uint32_t src_addr,uint32_t bcnt,uint32_t lcnt,uint32_t offset,uint32_t channel)
{
	upp_c6748_device *dev = opaque->obj;
	char data[4]; 
	if(dev->channel_fifo[channel]){ 
		if(CheckCanReadNum(dev->channel_fifo[channel]) >= 4){
			ReadFIFO(dev->channel_fifo[channel], data, 4);
		}else{
			return;
		}
		if(dev->image_space != NULL){
			if(dev->size[channel] >= bcnt){
				dev->line[channel]++;
				dev->size[channel] = 0;
			}
			dev->image_space->write(dev->image,src_addr + dev->size[channel] + dev->line[channel] * offset,data,4); 
			dev->size[channel] += 4;
		}
	}
	if(dev->line[channel] >= lcnt && dev->size[channel] >= bcnt){
		dev->size[channel] = 0;
		dev->line[channel] = 0;
		if(channel == CHANNEL_1){
			dev->regs.upid0.all = 0; 
			dev->regs.upid1.all =0; 
			dev->regs.upid2.all = 0;
			dev->regs.upier.bit.eowi = 1;
		}else{
			dev->regs.upqd0.all = 0; 
			dev->regs.upqd1.all =0; 
			dev->regs.upqd2.all = 0;
			dev->regs.upier.bit.eowq = 1;
		}
		dev->isr_status++;
		if(dev->isr_status >= 2){
			if(dev->signal_iface != NULL){
				dev->signal_iface->raise_signal(dev->signal, UPP_INT);
			}
			dev->isr_status = 0;
		}
	}
}
static void read_memory(conf_object_t *opaque,uint32_t src_addr,uint32_t bcnt,uint32_t lcnt,uint32_t offset,uint32_t channel)
{
	upp_c6748_device *dev = opaque->obj;
	char data[4]; 
	if(dev->channel_fifo[channel]){ 
		if(CheckCanWriteNum(dev->channel_fifo[channel]) <= 4){
			return;
		}else{
			if(dev->size[channel] >= bcnt){
				dev->line[channel]++;
				dev->size[channel] = 0;
			}
			if(dev->image_space != NULL){
				dev->image_space->read(dev->image,src_addr + dev->size[channel] + dev->line[channel] * offset,data,4); 
				WriteFIFO(dev->channel_fifo[channel], data, 4);
				dev->size[channel] += 4;
			}
		}
	}
	if(dev->line[channel] >= lcnt && dev->size[channel] >= bcnt){
		dev->size[channel] = 0;
		dev->line[channel] = 0;
		if(channel == CHANNEL_1){
			dev->regs.upid0.all = 0; 
			dev->regs.upid1.all =0; 
			dev->regs.upid2.all = 0;
			dev->regs.upier.bit.eowi = 1;
		}else{
			dev->regs.upqd0.all = 0; 
			dev->regs.upqd1.all =0; 
			dev->regs.upqd2.all = 0;
			dev->regs.upier.bit.eowq = 1;
		}
		dev->isr_status++;
		if(dev->isr_status >= 2){
			if(dev->signal_iface != NULL){
				dev->signal_iface->raise_signal(dev->signal, UPP_INT);
			}
			dev->isr_status = 0;
		}
	}
}
/*FIX ME Data Interleave Mode Not realized*/
static void dma_i_carry(conf_object_t *opaque)
{
	upp_c6748_device *dev = opaque->obj;

	/*check device is enabled*/
	if (dev->dev_enable == 0)
		return;
	if(dev->regs.upctl.bit.mode == RECEIVE_MODE || dev->regs.upctl.bit.mode == DUPLEX_0_MODE){
		if(dev->regs.upid0.all != 0 && dev->regs.upid1.all !=0 && dev->regs.upid2.all != 0){
			write_memory(opaque,dev->regs.upid0.all,dev->regs.upid1.bit.bcnth << 1,dev->regs.upid1.bit.lncnt - 1,dev->regs.upid2.bit.lnoffseth << 3,CHANNEL_1);
		}
	}else{
		if(dev->regs.upid0.all != 0 && dev->regs.upid1.all !=0 && dev->regs.upid2.all != 0){
			read_memory(opaque,dev->regs.upid0.all,dev->regs.upid1.bit.bcnth << 1,dev->regs.upid1.bit.lncnt - 1,dev->regs.upid2.bit.lnoffseth << 3,CHANNEL_1);
		}
	}
}

/*FIX ME Data Interleave Mode Not realized*/
static void dma_q_carry(conf_object_t *opaque)
{
	upp_c6748_device *dev = opaque->obj;
	if (dev->dev_enable == 0)
		return;

	if(dev->regs.upctl.bit.mode == RECEIVE_MODE || dev->regs.upctl.bit.mode == DUPLEX_0_MODE){
		if(dev->regs.upqd0.all != 0 && dev->regs.upqd1.all !=0 && dev->regs.upqd2.all != 0){
			write_memory(opaque,dev->regs.upqd0.all,dev->regs.upqd1.bit.bcnth << 1,dev->regs.upqd1.bit.lncnt - 1,dev->regs.upqd2.bit.lnoffseth << 3,CHANNEL_2);
		}
	}else{
		if(dev->regs.upqd0.all != 0 && dev->regs.upqd1.all !=0 && dev->regs.upqd2.all != 0){
			read_memory(opaque,dev->regs.upqd0.all,dev->regs.upqd1.bit.bcnth << 1,dev->regs.upqd1.bit.lncnt - 1,dev->regs.upqd2.bit.lnoffseth << 3,CHANNEL_2);
		}
	}
}

static exception_t upp_c6748_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	upp_c6748_device *dev = opaque->obj;
	switch (offset)
	{
		case UPPID:
			*(uint32_t *)buf = dev->regs.uppid;
			break;
		case UPPCR:
			*(uint32_t *)buf = dev->regs.uppcr.all;
			break;
		case UPDLB:
			*(uint32_t *)buf = dev->regs.updlb.all;
			break;
		case UPCTL:
			*(uint32_t *)buf = dev->regs.upctl.all;
			break;
		case UPICR:
			*(uint32_t *)buf = dev->regs.upicr.all;
			break;
		case UPIVR:
			*(uint32_t *)buf = dev->regs.upivr.all;
			break;
		case UPTCR:
			*(uint32_t *)buf = dev->regs.uptcr.all;
			break;
		case UPISR:
			*(uint32_t *)buf = dev->regs.upisr.all;
			break;
		case UPIER:
			*(uint32_t *)buf = dev->regs.upier.all;
			break;
		case UPIES:
			*(uint32_t *)buf = dev->regs.upies.all;
			break;
		case UPIEC:
			*(uint32_t *)buf = dev->regs.upiec.all;
			break;
		case UPEOI:
			*(uint32_t *)buf = dev->regs.upeoi;
			break;
		case UPID0:
			*(uint32_t *)buf = dev->regs.upid0.all;
			break;
		case UPID1:
			*(uint32_t *)buf = dev->regs.upid1.all;
			break;
		case UPID2:
			*(uint32_t *)buf = dev->regs.upid2.all;
			break;
		case UPIS0:
			*(uint32_t *)buf = dev->regs.upis0;
			break;
		case UPIS1:
			*(uint32_t *)buf = dev->regs.upis1.all;
			break;
		case UPIS2:
			*(uint32_t *)buf = dev->regs.upis2.all;
			break;
		case UPQD0:
			*(uint32_t *)buf = dev->regs.upqd0.all;
			break;
		case UPQD1:
			*(uint32_t *)buf = dev->regs.upqd1.all;
			break;
		case UPQD2:
			*(uint32_t *)buf = dev->regs.upqd2.all;
			break;
		case UPQS0:
			*(uint32_t *)buf = dev->regs.upqs0;
			break;
		case UPQS1:
			*(uint32_t *)buf = dev->regs.upqs1.all;
			break;
		case UPQS2:
			*(uint32_t *)buf = dev->regs.upqs2.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in upp_c6748\n", offset);
			break;
	}
	return No_exp;
}
static exception_t upp_c6748_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	upp_c6748_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case UPPID:
			//dev->regs.uppid = val;
			break;
		case UPPCR:
			dev->regs.uppcr.all = val;
			if(dev->regs.uppcr.bit.swrst == 1){ //software reset
				dev->regs.uppcr.bit.en = 0;
				dev->regs.uppcr.bit.db = 0;
			}
			if(dev->regs.uppcr.bit.en == 1){
				if(dev->sche_id[0] == NULL){
					dev->sche_id[0] = system_register_timer_handler(dev->obj, 400, (time_sched_t)transfer_data_A, SCHED_NS|SCHED_NORMAL);
				}
				if(dev->sche_id[1] == NULL){
					dev->sche_id[1] = system_register_timer_handler(dev->obj, 400, (time_sched_t)transfer_data_B, SCHED_NS|SCHED_NORMAL);
				}
			}else{
				if(dev->sche_id[0] != NULL){
					system_disable_timer(dev->sche_id[0]);
					dev->sche_id[0] = NULL;
				}
				if(dev->sche_id[1] != NULL){
					system_disable_timer(dev->sche_id[1]);
					dev->sche_id[1] = NULL;
				}
			}
			break;
		case UPDLB:
			dev->regs.updlb.all = val;
			break;
		case UPCTL:
			dev->regs.upctl.all = val;
			break;
		case UPICR:
			dev->regs.upicr.all = val;
			break;
		case UPIVR:
			dev->regs.upivr.all = val;
			break;
		case UPTCR:
			dev->regs.uptcr.all = val;
			break;
		case UPISR:
			dev->regs.upisr.all = val;
			break;
		case UPIER:
			dev->regs.upier.all &= ~val;
			break;
		case UPIES:
			dev->regs.upies.all |= val;
			break;
		case UPIEC:
			dev->regs.upiec.all = val;
			break;
		case UPEOI:
			dev->regs.upeoi= val;
			break;
		case UPID0:
			dev->regs.upid0.all = (val & 0xfffffff8);
			break;
		case UPID1:
			dev->regs.upid1.all = (val & 0xfffffffe);
			break;
		case UPID2:
			dev->regs.upid2.all = (val & 0xfffffff8);
			break;
		case UPIS0:
			dev->regs.upis0= val;
			break;
		case UPIS1:
			dev->regs.upis1.all = val;
			break;
		case UPIS2:
			dev->regs.upis2.all = val;
			break;
		case UPQD0:
			dev->regs.upqd0.all = val;
			break;
		case UPQD1:
			dev->regs.upqd1.all = val;
			break;
		case UPQD2:
			dev->regs.upqd2.all = val;
			break;
		case UPQS0:
			dev->regs.upqs0= val;
			break;
		case UPQS1:
			dev->regs.upqs1.all = val;
			break;
		case UPQS2:
			dev->regs.upqs2.all = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in upp_c6748\n", offset);
			break;
	}
	return 0;
}
static char* upp_c6748_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t upp_c6748_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	upp_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t upp_c6748_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t upp_c6748_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	upp_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t upp_c6748_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_upp_c6748(char *obj_name)
{
	upp_c6748_device* dev = skyeye_mm_zero(sizeof(upp_c6748_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	regs->uppid = 0x44231100;
	dev->size[0] = 0;
	dev->size[1] = 0;
	dev->line[0] = 0;
	dev->line[1] = 0;
	dev->sche_id[0] = NULL;
	dev->sche_id[1] = NULL;
	dev->dma_id[0] = NULL;
	dev->dma_id[1] = NULL;
	dev->channel_fifo[0] = CreateFIFO(512);
	dev->channel_fifo[1] = CreateFIFO(512);
	dev->isr_status = 0;
	dev->dma_id[0] = system_register_timer_handler(dev->obj, 400, (time_sched_t)dma_i_carry, SCHED_NS|SCHED_NORMAL);
	dev->dma_id[1] = system_register_timer_handler(dev->obj, 400, (time_sched_t)dma_q_carry, SCHED_NS|SCHED_NORMAL);

	dev->dev_enable = 1;
	return dev->obj;
}
static exception_t free_upp_c6748(conf_object_t* opaque)
{
	upp_c6748_device *dev = opaque->obj;
	int i = 0;
	for(i = 0;i < 2; i++){
		if(dev->channel_fifo[i])
			FreeFIFO(dev->channel_fifo[i]);
	}
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	upp_c6748_device *dev = obj->obj;
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

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	upp_c6748_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	upp_c6748_device *dev = obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf*)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	if (dev->image == NULL)
		dev->dev_enable = 0;
	else
		dev->dev_enable = 1;
	return No_exp;
}
static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	upp_c6748_device *dev = obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

static int upp_recive_data(conf_object_t *obj,uint32_t channel, char *data,uint32_t datalength)
{
	upp_c6748_device *dev = obj->obj;
	if(dev->channel_fifo[channel] == NULL){ 
		return 0;
	}
	if(CheckCanWriteNum(dev->channel_fifo[0]) <= 8){
		return 1;
	}else{
		WriteFIFO(dev->channel_fifo[channel], data, datalength);
	}
	return 0;
}

static exception_t upp_linker_data_tranf_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	upp_c6748_device *dev = obj->obj;
	dev->upp_linker = obj2;
	dev->upp_linker_intf = (memory_space_intf*)SKY_get_iface(dev->upp_linker, UPP_LINKER_DATA_TRANF_INTF);

	if (dev->upp_linker == NULL)
		dev->dev_enable = 0;
	else
		dev->dev_enable = 1;
	return No_exp;
}
static exception_t upp_linker_data_tranf_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	upp_c6748_device *dev = obj->obj;
	*obj2 = dev->upp_linker;
	*port = NULL;
	return No_exp;
}

void init_upp_c6748(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6748_upp",
		.class_desc = "c6748_upp",
		.new_instance = new_upp_c6748,
		.free_instance = free_upp_c6748,
		.module_type = SKYML_ONCHIP(SC_c6748),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = upp_c6748_read,
		.write = upp_c6748_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = upp_c6748_get_regvalue_by_id,
		.get_regname_by_id = upp_c6748_get_regname_by_id,
		.set_regvalue_by_id = upp_c6748_set_regvalue_by_id,
		.get_regnum = upp_c6748_get_regnum,
		.get_regid_by_name = upp_c6748_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
	static upp_data_signal_intf data_signal =
	{
		.send_data = upp_recive_data,
	};
	SKY_register_iface(clss, UPP_DATA_SIGNAL_INTF, &data_signal);
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
			.name = UPP_DATA_SIGNAL_INTF,
			.iface = &data_signal,

		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		},
		(struct ConnectDescription){
			.name = UPP_LINKER_DATA_TRANF_INTF,
			.set = upp_linker_data_tranf_set,
			.get = upp_linker_data_tranf_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
