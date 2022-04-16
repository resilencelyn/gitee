#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_dev_intf.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_emif.h>
#define DEBUG
#include <skyeye_log.h>
#include "nand_c6748.h"

void write_dma_data_to_space(conf_object_t *opaque, int val){
	nand_c6748_device *dev = opaque->obj;
	WriteFIFO(dev->read_fifo, &val, 1);
}
void read_dma_data_from_space(conf_object_t *opaque, void* buf){
	nand_c6748_device *dev = opaque->obj;
	ReadFIFO(dev->read_fifo, buf, 1);
}
void start_dma_transfer(conf_object_t *opaque){
	nand_c6748_device *dev = opaque->obj;
	dev->transfer_type = DMA_TRANSFER;
}
void transmit_complete(conf_object_t *opaque, int length){
	nand_c6748_device *dev = opaque->obj;
	dev->transfer_type = DEFAULE_TRANSFER;
}
void set_rb_ready(nand_c6748_device* dev){
	if (dev->emif_iface != NULL)
		dev->emif_iface->device_ready(dev->emif);
}
/* function do nandflash reset*/
static exception_t nandflash_reset(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	memset(dev->blocks, NULL, dev->pro.blocknum);
	set_rb_ready(dev);

	return 0;
}

/* function do nandflash erase accurately*/
static exception_t nandflash_erase(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	uint8 pgaddr;
	uint16 bladdr, claddr; 
	bladdr = BLOCKADDRESSMASK & (dev->address >> BLOCKADDRESSOFFSET);
	pgaddr = PAGEADDRESSMASK & (dev->address >> PAGEADDRESSOFFSET);
	claddr = COLUMADDRESSMASK & (dev->address);

	if( (NULL == dev->blocks[bladdr]) || (NULL == dev->blocks[bladdr][pgaddr]) || (BLANK == dev->blocks[bladdr][pgaddr][claddr]) )
	{
		dev->regs.data = BLANK;
		return 0;
	}
	else
	{
		dev->blocks[bladdr][pgaddr][claddr] = BLANK;
		dev->regs.data = BLANK;
	}
	return 0;

}

/* function do nandflash'block erase*/
static exception_t nandflash_eraseblock(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	int i = 0;
	uint8 pgaddr;
	uint16 bladdr, claddr; 
	bladdr = BLOCKADDRESSMASK & (dev->address >> BLOCKADDRESSOFFSET);
	pgaddr = PAGEADDRESSMASK & (dev->address >> PAGEADDRESSOFFSET);
	claddr = COLUMADDRESSMASK & (dev->address);
	if( NULL != dev->blocks[bladdr] )
	{
		for( i = 0 ; i < dev->pro.pagenum; i++)
		{
			if(NULL != dev->blocks[bladdr][i])
			{
				memset(dev->blocks[bladdr][i], NULL, dev->pro.pagesize);
				free(dev->blocks[bladdr][i]);
			}
		}
		memset(dev->blocks[bladdr], NULL, dev->pro.pagenum);
		free(dev->blocks[bladdr]);
	}

	return 0;
}

/* function do nandflsh write accurately */
static exception_t nandflash_datawrite(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	uint8 pgaddr;
	uint16 bladdr, claddr; 
	bladdr = BLOCKADDRESSMASK & (dev->address >> BLOCKADDRESSOFFSET);
	pgaddr = PAGEADDRESSMASK & (dev->address >> PAGEADDRESSOFFSET);
	claddr = COLUMADDRESSMASK & (dev->address);

	if( NULL == dev->blocks[bladdr] )
	{
		dev->blocks[bladdr] = (uint32 *)malloc( (dev->pro.pagenum) * sizeof(uint32 *) );
		memset( dev->blocks[bladdr], NULL, dev->pro.pagenum );
		dev->blocks[bladdr][pgaddr] = (uint32 *)malloc( dev->pro.pagesize );
		memset( dev->blocks[bladdr][pgaddr], BLANK, dev->pro.pagesize);
		dev->blocks[bladdr][pgaddr][claddr] = dev->regs.data;
	}
	else if(NULL == dev->blocks[bladdr][pgaddr])
	{
		dev->blocks[bladdr][pgaddr] = (uint32 *)malloc(dev->pro.pagesize);
		memset(dev->blocks[bladdr][pgaddr], BLANK, dev->pro.pagesize);
		dev->blocks[bladdr][pgaddr][claddr] = dev->regs.data;
	}
	else
	{
		dev->blocks[bladdr][pgaddr][claddr] = dev->regs.data;
	}
	
	return 0;

}

/* function do read the data accurately */
static exception_t nandflash_dataread(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	uint8 pgaddr;
	uint16 bladdr, claddr; 
	bladdr = BLOCKADDRESSMASK & (dev->address >> BLOCKADDRESSOFFSET);
	pgaddr = PAGEADDRESSMASK & (dev->address >> PAGEADDRESSOFFSET);
	claddr = COLUMADDRESSMASK & (dev->address);

	if( (NULL == dev->blocks[bladdr]) || (NULL == dev->blocks[bladdr][pgaddr]) || (BLANK == dev->blocks[bladdr][pgaddr][claddr]) )
	{
		dev->regs.data = BLANK;
	}
	else
	{
		dev->regs.data = dev->blocks[bladdr][pgaddr][claddr]; 
	}

	return 0;
			
}

/* function do read the whole pages'data */
static exception_t nandflash_pageread(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	uint8 pgaddr;
	uint16 bladdr, claddr;
	bladdr = BLOCKADDRESSMASK & (dev->address >> BLOCKADDRESSOFFSET);
	pgaddr = PAGEADDRESSMASK & (dev->address >> PAGEADDRESSOFFSET);
	claddr = dev->pro.cursor;

	dev->regs.data = dev->blocks[bladdr][pgaddr][claddr];
	dev->pro.cursor += 1;

	if(2048 == dev->pro.cursor)
	{
		dev->pro.cursor = 0;
	}

	printf("-----------------\n");

	return 0;

}

/* funtion do write the date to whole page */
static exception_t nandflash_pagewrite(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	uint8 pgaddr;
	uint16 bladdr, claddr;
	bladdr = BLOCKADDRESSMASK & (dev->address >> BLOCKADDRESSOFFSET);
	pgaddr = PAGEADDRESSMASK & (dev->address >> PAGEADDRESSOFFSET);
	claddr = dev->pro.cursor;
	if( NULL == dev->blocks[bladdr] )
	{
		dev->blocks[bladdr] = (uint32 *)malloc( (dev->pro.pagenum) * sizeof(uint32 *) );
		memset( dev->blocks[bladdr], NULL, dev->pro.pagenum );
		dev->blocks[bladdr][pgaddr] = (uint32 *)malloc( dev->pro.pagesize );
		memset( dev->blocks[bladdr][pgaddr], BLANK, dev->pro.pagesize);
		dev->blocks[bladdr][pgaddr][claddr] = dev->regs.data;
		dev->pro.cursor += 1;
	}
	else if( NULL == dev->blocks[bladdr][pgaddr] )
	{
		dev->blocks[bladdr][pgaddr] = (uint32 *)malloc(dev->pro.pagesize);
		memset( dev->blocks[bladdr][pgaddr], BLANK, dev->pro.pagesize);
		dev->blocks[bladdr][pgaddr][claddr] = dev->regs.data;
		dev->pro.cursor += 1;
		getchar();
	}
	else
	{
		dev->blocks[bladdr][pgaddr][claddr] = dev->regs.data;
		dev->pro.cursor += 1;
	}
	if(2048 == dev->pro.cursor)
	{
		dev->pro.cursor = 0;
		dev->regs.data = dev->blocks[bladdr][pgaddr][0];
	}

	//printf("dev->regs.data = %x\n",dev->blocks[bladdr][pgaddr][claddr]);

	return 0;
}

/* function do read the nandflash status regisiter*/
static exception_t nandflash_readstatus(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	dev->regs.data = 0xa0;
	//printf("######%d\n",dev->regs.data);
	
	return 0;

}

/* function do read the nandflash's manID & devID */
static exception_t nandflash_readid(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	switch(dev->readid_st)
	{
		case NF_readID_1st:
			dev->regs.data = dev->ID[0];
			dev->readid_st = NF_readID_2nd;
			break;
		case NF_readID_2nd:
			dev->regs.data = dev->ID[1];
			dev->readid_st = NF_readID_3rd;
			break;
		case NF_readID_3rd:
			dev->regs.data = dev->ID[2];
			dev->readid_st = NF_readID_4th;
			break;
		case NF_readID_4th:
			dev->regs.data = dev->ID[3];
			dev->readid_st = NF_readID_5th;
			break;
		case NF_readID_5th:
			dev->regs.data = dev->ID[4];
			dev->readid_st = NF_readID_finish;
			break;
		default:
			break;
	}
	
	return 0;
}

/* function do read the address which you want to write/read */
static exception_t nandflash_read(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	switch(dev->read_st)
	{
		case NF_addr_1st:
			dev->address = dev->regs.addr;
			dev->read_st = NF_addr_2nd;
			break;
		case NF_addr_2nd:
			dev->address = ( (uint64)(dev->regs.addr)<<ADDRESSOFFSET8BIT | dev->address );
			dev->read_st = NF_addr_3rd;
			break;
		case NF_addr_3rd:
			dev->address = ( ((uint64)dev->regs.addr)<<ADDRESSOFFSET16BIT | dev->address );
			dev->read_st = NF_addr_4th;
			break;
		case NF_addr_4th:
			dev->address = ( ((uint64)dev->regs.addr)<<ADDRESSOFFSET24BIT | dev->address );
			dev->read_st = NF_addr_5th;
			break;
		case NF_addr_5th:
			dev->address = ( ((uint64)dev->regs.addr)<<ADDRESSOFFSET32BIT | dev->address );
			dev->read_st = NF_addr_finish;
			if(NF_PROGRAM == dev->cmd_t)
			{
				dev->cmd_t = NF_PAGEWRITESTART;
			}
			break;
		default:
			break;
	}

	return 0;

}

/* function do commond by the commond type */
static exception_t nandflash_docmd(conf_object_t *opaque)
{
	nand_c6748_device *dev = opaque->obj;
	switch(dev->cmd_t)
	{
		case NF_DATAREADID:
			nandflash_readid(opaque);
			break;
		case NF_DATAREADSTART:
			nandflash_read(opaque);
			break;
		case NF_ERASE:
			nandflash_erase(opaque);
			break;
		case NF_BLOCKERASES:
			nandflash_eraseblock(opaque);
			break;
		case NF_RESET:
			nandflash_reset(opaque);
			break;
		case NF_DATAREAD:
			nandflash_dataread(opaque);
			break;
		case NF_READSTATUS:
			nandflash_readstatus(opaque);
			break;
		case NF_DATAWRITE:
			nandflash_datawrite(opaque);
			break;
		case NF_PAGEWRITESTART:
			//nandflash_pagewrite(opaque);
			break;
		case NF_PROGRAM:
			nandflash_read(opaque);
			break;
		case NF_PROGRAM2:
			nandflash_pageread(opaque);
			break;
		default:
			break;
	}

	return 0;
}

static exception_t nand_c6748_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	nand_c6748_device *dev = opaque->obj;
	switch (offset)
	{
		case DATA:
			if (dev->transfer_type == DMA_TRANSFER)
				read_dma_data_from_space(opaque, buf);
			else 
				*(uint32_t *)buf = dev->regs.data;
			if(NF_DATAREADID == dev->cmd_t)
			{
				nandflash_docmd(opaque);
			}
			else
			{
				dev->cmd_t = NF_DATAREAD;
				nandflash_docmd(opaque);
			}
			
			break;
		case ADDR:
			*(uint32_t *)buf = dev->regs.addr;
			break;
		case CMD:
		{		
			switch(dev->regs.cmd)
			{
				case NAND_CMD_READ:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_READ_CYC2:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_READID:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_RESET:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_PROGRAM:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_PROGRAM_CYC2:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_ERASE:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_ERASE_CYC2:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_READ_STATUS:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_RANDOM:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				case NAND_CMD_RANDOM_CYC2:
					*(uint32_t *)buf = dev->regs.cmd;
					break;
				default:
					break;
				}
			break;
		}
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in nand_c6748\n", offset);
			break;
	}

	return 0;

}
static exception_t nand_c6748_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	nand_c6748_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		case DATA:
			dev->regs.data = val;
			if (dev->transfer_type == DMA_TRANSFER){
				write_dma_data_to_space(opaque, val);
			}
		/*	if(NF_PAGEWRITESTART == dev->cmd_t)
			{
				nandflash_docmd(opaque);
			}
			else
			{
				dev->cmd_t = NF_DATAWRITE;
				nandflash_docmd(opaque);
			}
		*/
			break;
		case ADDR:
			dev->regs.addr = val;
			if (NF_DATAREADID == dev->cmd_t)
			{
				nandflash_docmd(opaque);	
			}
			else if(NF_PROGRAM == dev->cmd_t)
			{
				nandflash_docmd(opaque);
			}
			else
			{
				nandflash_docmd(opaque);
			}
			break;
		case CMD:
		{
			switch(val)
			{
				case NAND_CMD_READ:
					dev->cmd_t = NF_DATAREADSTART;
					dev->read_st = NF_addr_1st;
					nandflash_docmd(opaque);
					dev->regs.cmd = NAND_CMD_READ;
					break;
				case NAND_CMD_READ_CYC2:
					if( NF_DATAREADSTART == dev->cmd_t)
					{
						dev->cmd_t = NF_DATAREAD;
						nandflash_docmd(opaque);
					}
					else
					{
						dev->cmd_t = NF_DATAREADDOWN;
					}
					dev->regs.cmd = NAND_CMD_READ_CYC2;
					set_rb_ready(dev);
					break;
				case NAND_CMD_READID:
					dev->cmd_t = NF_DATAREADID;
					dev->regs.cmd = NAND_CMD_READID;
					dev->readid_st = NF_readID_1st;
					break;
				case NAND_CMD_RESET:
					dev->cmd_t = NF_RESET;
					dev->regs.cmd = NAND_CMD_RESET;
					nandflash_docmd(opaque);
					break;
				case NAND_CMD_PROGRAM:
					dev->cmd_t = NF_PROGRAM;
					dev->read_st = NF_addr_1st;
					dev->regs.cmd = NAND_CMD_PROGRAM;
					break;
				case NAND_CMD_PROGRAM_CYC2:
					dev->regs.cmd = NAND_CMD_PROGRAM_CYC2;
					set_rb_ready(dev);
					break;
				case NAND_CMD_ERASE:
					dev->cmd_t = NF_BLOCKERASESTART;
					dev->read_st = NF_addr_3rd;
					dev->regs.cmd = NAND_CMD_ERASE;
					break;
				case NAND_CMD_ERASE_CYC2:
					dev->regs.cmd = NAND_CMD_ERASE_CYC2;
					dev->cmd_t = NF_BLOCKERASES;
					set_rb_ready(dev);
					break;
				case NAND_CMD_READ_STATUS:
					dev->regs.cmd = NAND_CMD_READ_STATUS;
					dev->cmd_t = NF_READSTATUS;
					nandflash_docmd(opaque);
					break;
				case NAND_CMD_RANDOM:
					dev->regs.cmd = NAND_CMD_RANDOM;
					break;
				case NAND_CMD_RANDOM_CYC2:
					dev->regs.cmd = NAND_CMD_RANDOM_CYC2;
					set_rb_ready(dev);
					break;
				default:
					break;
			}
			break;
		}
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in nand_c6748\n", offset);
			break;
	}

	return 0;

}

static char* nand_c6748_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t nand_c6748_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	nand_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;

}

static uint32_t nand_c6748_get_regnum(conf_object_t *conf_obj, uint32_t id)
{

	return  sizeof(struct registers) / 4;

}

static uint32_t nand_c6748_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	nand_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;

	return *regs_value;

}

static uint32_t nand_c6748_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;

}

static conf_object_t* new_nand_c6748(char *obj_name)
{
	nand_c6748_device* dev = skyeye_mm_zero(sizeof(nand_c6748_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	dev->address = 0;
	dev->readid_st = NF_NOSTATUS;
	dev->read_st = NF_noread;
	dev->cmd_t = NF_NONE;

	dev->pro.pagesize = NAND_PAGESIZE;
	dev->pro.pagenum = NAND_PAGE_OF_BLK;
	dev->pro.blocknum = NAND_BLOCK_OF_DEVICE;
	dev->pro.cursor = 0;
	dev->transfer_type = DEFAULE_TRANSFER; 

	dev->blocks = (uint32 ***)malloc( (dev->pro.blocknum) * sizeof(uint32 **) );
	if(NULL == dev->blocks)
	{
		skyeye_log(Error_log, __FUNCTION__,"Nandflash malloc error!\n");
	}	

	dev->ID[0] = 0x2c;
	dev->ID[1] = 0xdc;
	dev->ID[2] = 0x90;
	dev->ID[3] = 0x95;
	dev->ID[4] = 0x56;

	dev->read_fifo = CreateFIFO(FIFO_LENGTH);
	if(dev->read_fifo == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Create Read Fifo Fail.");

	return dev->obj;

}

static exception_t free_nand_c6748(conf_object_t* opaque)
{
	nand_c6748_device *dev = opaque->obj;
	if(dev->read_fifo)
		FreeFIFO(dev->read_fifo);

	skyeye_free(dev->blocks);
	skyeye_free(opaque->obj);
	skyeye_free(opaque->objname);
	skyeye_free(opaque);

	return No_exp;

}

static exception_t emif_iface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	nand_c6748_device *dev = obj->obj;
	dev->emif = obj2;
	dev->emif_iface = (skyeye_emif_intf *)SKY_get_iface(dev->emif, SKYEYE_EMIF_INTF);
	if (dev->emif_iface == NULL){
		if (dev->emif == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_EMIF_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_EMIF_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t emif_iface_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	nand_c6748_device *dev = obj->obj;
	*obj2 = dev->emif;
	*port = NULL;
	return No_exp;
}
static attr_value_t get_attr_device_id(void* arg, conf_object_t* obj, attr_value_t* idx){
	nand_c6748_device *dev = obj->obj;
	attr_value_t device_id = SKY_make_attr_uinteger(dev->ID[1]);
	return device_id;
}
static exception_t set_attr_device_id(void* arg, conf_object_t* obj, attr_value_t* value, attr_value_t* idx){
	nand_c6748_device *dev = obj->obj;
	dev->ID[1] = SKY_attr_uinteger(*value);
	return No_exp;
}
static nand_c6748_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "device_id", get_attr_device_id, NULL, set_attr_device_id, NULL, SKY_Attr_Optional, "uinteger", "nand_c6748 device ID");
}
void init_nand_c6748(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6748_nand",
		.class_desc = "c6748_nand",
		.new_instance = new_nand_c6748,
		.free_instance = free_nand_c6748,
		.module_type = SKYML_ONCHIP(SC_c6748),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = nand_c6748_read,
		.write = nand_c6748_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static edma_dev_intf edma_iface =
	{
		.start_dma_transfer = start_dma_transfer,
		.transmit_complete = transmit_complete
	};
	SKY_register_iface(clss, EDMA_DEV_INTF, &edma_iface);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = nand_c6748_get_regvalue_by_id,
		.get_regname_by_id = nand_c6748_get_regname_by_id,
		.set_regvalue_by_id = nand_c6748_set_regvalue_by_id,
		.get_regnum = nand_c6748_get_regnum,
		.get_regid_by_name = nand_c6748_get_regid_by_name
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
			.name = EDMA_DEV_INTF,
			.iface = &edma_iface
		},
	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			.name = SKYEYE_EMIF_INTF,
			.set = emif_iface_set,
			.get = emif_iface_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	nand_c6748_register_attribute(clss);
}
