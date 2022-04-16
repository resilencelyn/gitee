/* Copyright (C) 
* 2014 - Michael.Kang blackfin.kang@gmail.com
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/
/**
* @file sopc_i2c.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-04
*/

#include <stdio.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h> 
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include <skyeye_dev_bus_intf.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include "sopc_i2c.h"
#ifdef DEBUG
#define debug_printf printf
#else 
#define debug_printf
#endif

static int abandon_addr = 0;
static int out_offset_num = 0;
static int in_offset_num = 0;

static int recv_data(conf_object_t *conf_obj, void *buf, int length)
{
	sopc_i2c_dev *dev = conf_obj->obj;

	switch(MACH_GET(*(uint32_t *)buf))
	{
		case MACH_OUT_ENABLE: 
#if 0
			dev->w_out_count ++;
			if(dev->w_out_count > 3)
			{
				dev->w_out_count = 0;
			}
#endif
			dev->w_out_mach_flag = 1;
			debug_printf("IIC ENABLE OUT EEPROM\n");
			break;
		case MACH_IN_ENABLE: 
			dev->w_in_mach_flag = 1;
#if 0
			dev->w_in_count ++;
			if(dev->w_in_count > 3)
			{
				dev->w_in_count = 0;
			}
#endif
			debug_printf("IIC ENABLE IN EEPROM\n");
			break;
		case MACH_OUT_DISABLE:
			dev->w_out_mach_flag = 0;
			debug_printf("IIC DISABLE OUT EEPROM\n");
			break;
		case MACH_IN_DISABLE:
			dev->w_in_mach_flag = 0;
			debug_printf("IIC DISABLE IN EEPROM\n");
			break;
		default:
			break;
	}
	return 1;
}

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

int file_size(char *filename)
{
	FILE *fp = fopen(filename,"r");
	if(!fp)
		return -1;
	fseek(fp,0L,SEEK_END);
	int size = ftell(fp);
	fclose(fp);

	return size;
}

static void read_operation(conf_object_t *opaque)
{
	sopc_i2c_dev *dev = opaque->obj;

	int i = 0;
	unsigned char value;

	if((dev->out_file_exist || dev->in_file_exist) && \
			(dev->write_flag != 1))
	{
		FILE *EF_fp;

		if(dev->w_out_mach_flag)
		{
			EF_fp = fopen("out_eeprom_file.txt","rb");
			fseek(EF_fp, out_offset_num * dev->page_size, SEEK_SET);
			out_offset_num ++;
			for (i = 0; i < dev->page_num; i++)
			{
				value = fgetc(EF_fp);
				WriteFIFO(dev->i2c_refifo,&value,1);
			}
			fclose(EF_fp);
		}
		else
		{
			EF_fp = fopen("in_eeprom_file.txt","rb");
			fseek(EF_fp, in_offset_num * dev->page_size, SEEK_SET);
			in_offset_num ++;
			for (i = 0; i < dev->page_num; i++)
			{
				value = fgetc(EF_fp);
				WriteFIFO(dev->i2c_refifo,&value,1);
			}
			fclose(EF_fp);
		}	
	}
	else
	{
		FILE *eeprom_file = fopen(dev->eeprom_filename, "rb");
		if (eeprom_file == NULL)
		{
			dev->read_count = 0;
			skyeye_log(Error_log, __FUNCTION__, "Can not read file: %s\n", dev->eeprom_filename);
		}
		else
		{
			for (i = 0; i < dev->page_size; i++)
			{
				value = fgetc(eeprom_file);
				WriteFIFO(dev->i2c_refifo,&value,1);
			}
			fclose(eeprom_file);
		}
	}

}

static void write_complete_file(conf_object_t *opaque,uint8_t *value)
{
	sopc_i2c_dev *dev = opaque->obj;

	/*当前程序如果读取/写入失败，没有失能对应板子的eeprom,使用if else解决该问题*/
	if(dev->w_out_mach_flag)
		fwrite(value,1,M_PAGE_BYTE_NUM,dev->out_fp);
	//if(dev->w_in_mach_flag)
	else
		fwrite(value,1,M_PAGE_BYTE_NUM,dev->in_fp);
}

static void write_operation(conf_object_t *opaque)
{
	sopc_i2c_dev *dev = opaque->obj;

	uint8_t *tmp = (uint8_t *)malloc(sizeof(uint8_t) * M_PAGE_BYTE_NUM);
	dev->write_flag = 1;
	FILE *eeprom_file = fopen(dev->eeprom_filename,"wb");
	debug_printf("current write file : %s\n",dev->eeprom_filename);
	if(eeprom_file == NULL)
	{
		skyeye_log(Error_log, __FUNCTION__, "Can not write file: %s\n", dev->eeprom_filename);
	}

	else
	{
		int i;
		ReadFIFO(dev->i2c_fifo, tmp, M_PAGE_BYTE_NUM);
		fwrite(tmp, 1, M_PAGE_BYTE_NUM, eeprom_file);
		write_complete_file(opaque,tmp);
		fclose(eeprom_file);
		free(tmp);
	}
}

static exception_t sopc_i2c_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	sopc_i2c_dev *dev=  (sopc_i2c_dev *)opaque->obj;
	
	switch(offset)
	{
		case I2C_CON:          
			*(uint32_t*)buf = dev->regs.i2c_con;
			break;
		case I2C_TAR :           
			*(uint32_t*)buf = dev->regs.i2c_tar;            
			break;
		case I2C_SAR :               
			*(uint32_t*)buf = dev->regs.i2c_sar;                
			break;
		case I2C_HS_MADDR :      
			*(uint32_t*)buf = dev->regs.i2c_hs_maddr;       
			break;
		case I2C_DATA_CMD:
			if(dev->read_count > 0)
			{
				ReadFIFO(dev->i2c_refifo,&(dev->regs.i2c_data_cmd),1);
				dev->regs.i2c_rxflr --;
				/*读取64个字节(一页)的数据结束*/
				//if((dev->regs.i2c_txflr % M_PAGE_BYTE_NUM) == 0)
				if(dev->regs.i2c_rxflr == 0)
				{
					dev->read_count = 0;
				}
				*(uint32_t*)buf = dev->regs.i2c_data_cmd;
			}
			break;
		case I2C_SS_SCL_HCNT :      
			*(uint32_t*)buf = dev->regs.i2c_ss_scl_hcnt;       
			break;
		case I2C_SS_SCL_LCNT :      
			*(uint32_t*)buf = dev->regs.i2c_ss_scl_lcnt;
			break;
		case I2C_FS_SCL_HCNT :           
			*(uint32_t*)buf = dev->regs.i2c_fs_scl_hcnt;      
			break;
		case I2C_FS_SCL_LCNT :    
			*(uint32_t*)buf = dev->regs.i2c_fs_scl_lcnt;    
			break;
		case I2C_HS_SCL_HCNT  :   
			*(uint32_t*)buf = dev->regs.i2c_hs_scl_hcnt;    
			break;
		case I2C_HS_SCL_LCNT  :   
			*(uint32_t*)buf = dev->regs.i2c_hs_scl_lcnt;    
			break;
		case I2C_INTR_STAT :    
			*(uint32_t*)buf = dev->regs.i2c_intr_stat;    
			break;
		case I2C_INTR_MASK  :    
			*(uint32_t*)buf = dev->regs.i2c_intr_mask;    
			break;
		case I2C_RAW_INTR_STAT  : 
		 	dev->regs.i2c_raw_intr_stat |= I2C_RAW_INTR_ACTIVITY;	
			*(uint32_t*)buf = dev->regs.i2c_raw_intr_stat;    
			break;
		case I2C_RX_TL  :    
			*(uint32_t*)buf = dev->regs.i2c_rx_tl;    
			break;
		case I2C_TX_TL:    
			*(uint32_t*)buf = dev->regs.i2c_tx_tl;    
			break;
		case I2C_CLR_INTR :    
			*(uint32_t*)buf = dev->regs.i2c_clr_intr;    
			break;
		case I2C_CLR_RX_UNDER  :    
			*(uint32_t*)buf = dev->regs.i2c_clr_rx_under;    
			break;
		case I2C_CLR_RX_OVER   :    
			*(uint32_t*)buf = dev->regs.i2c_clr_rx_over;    
			break;
		case I2C_CLR_TX_OVER :
			*(uint32_t*)buf = dev->regs.i2c_clr_tx_over;    
			break;
		case I2C_CLR_RD_REQ :
			*(uint32_t*)buf = dev->regs.i2c_clr_rd_req;		 
			break;
		case I2C_CLR_TX_ABRT :
			*(uint32_t*)buf = dev->regs.i2c_clr_tx_abrt;		 
			break;
		case I2C_CLR_RX_DONE : 
			*(uint32_t*)buf = dev->regs.i2c_clr_rx_done;
			break;
		case I2C_CLR_ACTIVITY : 
			*(uint32_t*)buf = dev->regs.i2c_clr_activity;
			break;
		case I2C_CLR_STOP_DET : 
			*(uint32_t*)buf = dev->regs.i2c_clr_stop_det;
			break;
		case I2C_CLR_START_DET : 
			*(uint32_t*)buf = dev->regs.i2c_clr_start_def;
			break;
		case I2C_ENABLE : 
			*(uint32_t*)buf = dev->regs.i2c_enable;
		 	break;
		case I2C_STATUS : 
			*(uint32_t*)buf = dev->regs.i2c_status;
			break;
		case I2C_TXFLR : 
			*(uint32_t*)buf = dev->regs.i2c_txflr;
			break;
		case I2C_RXFLR : 
			if(dev->out_file_exist || dev->in_file_exist == 1)
				dev->regs.i2c_rxflr = 64;
			*(uint32_t*)buf = dev->regs.i2c_rxflr;
			break;
		case I2C_TX_ABRT_SOURCE : 
			*(uint32_t*)buf = dev->regs.i2c_tx_abrt_source;
			break;
		case I2C_SLV_DATA_NACK_ONLY : 
			*(uint32_t*)buf = dev->regs.i2c_slv_data_nack_only;
			break;
		case I2C_DMA_CR : 
			*(uint32_t*)buf = dev->regs.i2c_dma_cr;
			break;
		case I2C_DMA_TDLR : 
			*(uint32_t*)buf = dev->regs.i2c_dma_tdlr;
			break;
		case I2C_DMA_RDLR : 
			*(uint32_t*)buf = dev->regs.i2c_dma_rdlr;
			break;
		case I2C_SDA_SETUP : 
			*(uint32_t*)buf = dev->regs.i2c_sda_setup;
			break;
		case I2C_ACK_GENERAL_CALL : 
			*(uint32_t*)buf = dev->regs.i2c_ack_general_call;
			break;
		case I2C_ENABLE_STATUS : 
			*(uint32_t*)buf = dev->regs.i2c_enable_status;
			break;
		case I2C_COMP_PARAM_1 : 
			*(uint32_t*)buf = dev->regs.i2c_comp_param_1;
			break;
		case I2C_COMP_VERSION : 
			*(uint32_t*)buf = dev->regs.i2c_comp_version;
			break;
		case I2c_COMP_TYPE : 
			*(uint32_t*)buf = dev->regs.i2c_comp_type;
			break;
		default:
			break;
	}       
	return  No_exp;
}

static exception_t sopc_i2c_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{

	sopc_i2c_dev *dev=  (sopc_i2c_dev *)opaque->obj;
	uint32_t val = *(uint32_t*)buf;
	//printf("In %s, offset:%x, val:%x\n", __func__, offset, val);
	switch(offset)
	{
		case I2C_CON:          
			dev->regs.i2c_con = val;
			break;
		case I2C_TAR :           
			dev->regs.i2c_tar = val;
			break;
		case I2C_SAR :               
			dev->regs.i2c_sar = val;                
			break;
		case I2C_HS_MADDR :      
			dev->regs.i2c_hs_maddr = val;       
			break;
		case I2C_DATA_CMD:
			dev->regs.i2c_data_cmd = val;
			if(val & I2C_DATA_READ_CMD)
			{
				dev->read_count++;
				if(dev->read_count == 1)
				{
					read_operation(opaque);
					abandon_addr = 0;
				}
			}
			else
			{
				/*should set iic_write_enable before write data to FIFO*/
				if(dev->w_out_mach_flag || dev->w_in_mach_flag)
				{
					uint8_t tmp = (uint8_t)WRITE_VAL(val);
					/*放弃前两位地址位*/
					if(abandon_addr < 2)
					{
						abandon_addr ++;
						break;
					}
					else
					{
						WriteFIFO(dev->i2c_fifo,&tmp,1);
						dev->regs.i2c_rxflr ++;
						if(dev->w_out_mach_flag)
						{
							dev->eeprom_filename = dev->out_eeprom_filename;
						}
						else
						{
							dev->eeprom_filename = dev->in_eeprom_filename;
						}
					}
				}
			}
			break;
		case I2C_SS_SCL_HCNT :      
			dev->regs.i2c_ss_scl_hcnt = val;
			break;
		case I2C_SS_SCL_LCNT :      
			dev->regs.i2c_ss_scl_lcnt = val;      
			break;
		case I2C_FS_SCL_HCNT :           
			dev->regs.i2c_fs_scl_hcnt = val;
			break;
		case I2C_FS_SCL_LCNT :    
			dev->regs.i2c_fs_scl_lcnt = val;
			break;
		case I2C_HS_SCL_HCNT  :   
			dev->regs.i2c_hs_scl_hcnt = val;   
			break;
		case I2C_HS_SCL_LCNT  :   
			dev->regs.i2c_hs_scl_lcnt = val;    
			break;
		case I2C_INTR_STAT :    
			dev->regs.i2c_intr_stat = val;    
			break;
		case I2C_INTR_MASK  :    
			dev->regs.i2c_intr_mask = val;    
			break;
		case I2C_RAW_INTR_STAT  :    
			dev->regs.i2c_raw_intr_stat = val;    
			break;
		case I2C_RX_TL  :    
			dev->regs.i2c_rx_tl = val;    
			break;
		case I2C_TX_TL:    
			dev->regs.i2c_tx_tl = val;    
			break;
		case I2C_CLR_INTR :    
			dev->regs.i2c_clr_intr = val;    
			break;
		case I2C_CLR_RX_UNDER  :    
			dev->regs.i2c_clr_rx_under = val;    
			break;
		case I2C_CLR_RX_OVER   :    
			dev->regs.i2c_clr_rx_over = val;    
			break;
		case I2C_CLR_TX_OVER :
			dev->regs.i2c_clr_tx_over = val;    
			break;
		case I2C_CLR_RD_REQ :
			dev->regs.i2c_clr_rd_req = val;		 
			break;
		case I2C_CLR_TX_ABRT :
			dev->regs.i2c_clr_tx_abrt = val;		 
			break;
		case I2C_CLR_RX_DONE :
			dev->regs.i2c_clr_rx_done = val;		 
			break;
		case I2C_CLR_ACTIVITY :
			dev->regs.i2c_clr_activity = val;		 
			break;	
		case I2C_CLR_STOP_DET :
			dev->regs.i2c_clr_stop_det = val;		 
			break;
		case I2C_CLR_START_DET :
			dev->regs.i2c_clr_start_def = val;		 
			break;
		case I2C_ENABLE :
			dev->regs.i2c_enable = val;		 
			break;
		case I2C_STATUS :
			dev->regs.i2c_status = val;		 
			break;
		case I2C_TXFLR :
			dev->regs.i2c_txflr = val;		 
			break;
		case I2C_RXFLR :
			dev->regs.i2c_rxflr = val;		 
			break;
		case I2C_TX_ABRT_SOURCE :
			dev->regs.i2c_tx_abrt_source = val;		 
			break;	
		case I2C_SLV_DATA_NACK_ONLY :
			dev->regs.i2c_slv_data_nack_only = val;		 
			break;
		case I2C_DMA_CR :
			dev->regs.i2c_dma_cr = val;		 
			/*set I2C_DMA_CR 0 means DMA transfer over,time to write data to file*/
			if(I2C_DMA_STATUS(val) == I2C_DMA_ENABLE)
			{
			}

			if(I2C_DMA_STATUS(val) == I2C_DMA_DISABLE)
			{
				write_operation(opaque);
				abandon_addr = 0;
			}
			break;
		case I2C_DMA_TDLR :
			dev->regs.i2c_dma_tdlr = val;		 
			break;	
		case I2C_DMA_RDLR :
			dev->regs.i2c_dma_rdlr = val;		 
			break;	
		case I2C_SDA_SETUP :
			dev->regs.i2c_sda_setup = val;		 
			break;	
		case I2C_ACK_GENERAL_CALL :
			dev->regs.i2c_ack_general_call = val;		 
			break;	
		case I2C_ENABLE_STATUS :
			dev->regs.i2c_enable_status = val;		 
			break;	
		case I2C_COMP_PARAM_1 :
			dev->regs.i2c_comp_param_1 = val;		 
			break;	
		case I2C_COMP_VERSION :
			dev->regs.i2c_comp_version = val;		 
			break;
		case I2c_COMP_TYPE :
			dev->regs.i2c_comp_type = val;		 
			break;		
		default:
			break;
	}
	return  No_exp;
}

static char* iic_omapl138_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t iic_omapl138_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	sopc_i2c_dev *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t iic_omapl138_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t iic_omapl138_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	sopc_i2c_dev *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t iic_omapl138_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_sopc_i2c(char* obj_name)
{
	sopc_i2c_dev* sopc_i2c = skyeye_mm_zero(sizeof(sopc_i2c_dev));
	sopc_i2c->obj = new_conf_object(obj_name, sopc_i2c);
	
	int pthread_id;
	int size;
	sopc_i2c->read_count = 0;
	sopc_i2c->write_flag = 0;

	sopc_i2c->w_out_mach_flag = 0;
	sopc_i2c->w_in_mach_flag = 0;
#if 0
	sopc_i2c->regs.i2c_icstr = 0x410;
	sopc_i2c->regs.i2c_revid1 = 0x4415;
	sopc_i2c->regs.i2c_revid2 = 0x6;
	sopc_i2c->regs.i2c_icdmac = 0x3;
	create_thread_scheduler(10000, Periodic_sched, Detect_GPIO_Pin, sopc_i2c->obj, &pthread_id);
#endif

	if(file_size("out_eeprom_file.txt") == 5056)
	{
		sopc_i2c->out_file_exist = 1;
	}
	else
		sopc_i2c->out_fp = fopen("out_eeprom_file.txt","wab");

	if(file_size("in_eeprom_file.txt") == 1088)
	{
		sopc_i2c->in_file_exist = 1;
	}
	else
		sopc_i2c->in_fp = fopen("in_eeprom_file.txt","wab");

	sopc_i2c->i2c_fifo = CreateFIFO(1024);
	sopc_i2c->i2c_refifo = CreateFIFO(1024);
	return sopc_i2c->obj;
}

static exception_t reset_sopc_i2c(conf_object_t* opaque, const char* args)
{
	sopc_i2c_dev *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
	return No_exp;        
}


static exception_t free_sopc_i2c(conf_object_t* opaque)
{
	sopc_i2c_dev *dev = opaque->obj;
	if(dev->out_fp)
		fclose(dev->out_fp);
	if(dev->in_fp)
		fclose(dev->in_fp);
	FreeFIFO(dev->i2c_fifo);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static exception_t memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_i2c_dev *dev =  obj->obj;
	dev->space = obj2;
	dev->space_iface = (memory_space_intf *)SKY_get_iface(dev->space, MEMORY_SPACE_INTF_NAME);

	if (dev->space_iface == NULL){
		if (dev->space == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t memory_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sopc_i2c_dev *dev =  obj->obj;
	*obj2 = dev->space;
	*port = NULL;
	return No_exp;
}

static attr_value_t get_attr_in_eeprom_filename(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	sopc_i2c_dev *dev = conf_obj->obj;
	attr_value_t in_eeprom_filename = SKY_make_attr_string(dev->in_eeprom_filename);
	return in_eeprom_filename;
}

static exception_t set_in_eeprom_filename(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	sopc_i2c_dev *dev = conf_obj->obj;
	dev->in_eeprom_filename = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static attr_value_t get_attr_out_eeprom_filename(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	sopc_i2c_dev *dev = conf_obj->obj;
	attr_value_t out_eeprom_filename = SKY_make_attr_string(dev->out_eeprom_filename);
	return out_eeprom_filename;
}

static exception_t set_out_eeprom_filename(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	sopc_i2c_dev *dev = conf_obj->obj;
	dev->out_eeprom_filename = skyeye_strdup(SKY_attr_string(*value));
	return No_exp;
}

static attr_value_t get_attr_page_num(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	sopc_i2c_dev *dev = conf_obj->obj;
	attr_value_t page_num = SKY_make_attr_uinteger(dev->page_num);
	return page_num;
}

static exception_t set_attr_page_num(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	sopc_i2c_dev *dev = conf_obj->obj;
	dev->page_num = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_page_size(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	sopc_i2c_dev *dev = conf_obj->obj;
	attr_value_t page_size = SKY_make_attr_uinteger(dev->page_size);
	return page_size;
}

static exception_t set_attr_page_size(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	sopc_i2c_dev *dev = conf_obj->obj;
	dev->page_size = SKY_attr_uinteger(*value);
	return No_exp;
}

static class_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "in_eeprom_filename", get_attr_in_eeprom_filename, NULL, set_in_eeprom_filename, NULL, SKY_Attr_Optional, "string", "set in_eeprom_filename");
	SKY_register_attribute(clss, "out_eeprom_filename", get_attr_out_eeprom_filename, NULL, set_out_eeprom_filename, NULL, SKY_Attr_Optional, "string", "set out_eeprom_filename");
	SKY_register_attribute(clss, "page_num",        get_attr_page_num,        NULL, set_attr_page_num,   NULL, SKY_Attr_Optional, "uinteger" , "set page_num");
	SKY_register_attribute(clss, "page_size",       get_attr_page_size,       NULL, set_attr_page_size,  NULL, SKY_Attr_Optional, "uinteger" , "set page_size");
}


static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	sopc_i2c_dev *dev = obj->obj;
	dev->signal_obj = obj2;
	dev->signal = (general_signal_intf *)SKY_get_iface(dev->signal_obj, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal == NULL){
		if (dev->signal_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	sopc_i2c_dev *dev = obj->obj;
	*obj2 = dev->signal_obj;
	*port = NULL;
	return No_exp;
}

static exception_t get_attr_i2c_master(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	sopc_i2c_dev *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port = NULL;
	return No_exp;
}

static exception_t set_attr_i2c_master(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	sopc_i2c_dev *dev = obj->obj;
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
	sopc_i2c_dev *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	sopc_i2c_dev *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void sopc_i2c_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

}

void init_sopc_i2c(){
	static skyeye_class_t class_data = {
		.class_name = "sopc_i2c",
		.class_desc = "sopc_i2c",
		.new_instance = new_sopc_i2c,
		.reset_instance = reset_sopc_i2c,
		.free_instance = free_sopc_i2c,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_omapl138 SC_c6748),
	};
	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = sopc_i2c_read,
		.write = sopc_i2c_write,
	};
	SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);
#if 0 
	static const i2c_device_intf i2c_dev_iface = {
		.set_state = sopc_i2c_set_state,
		.write_data = sopc_i2c_write_data,
		.read_data = sopc_i2c_read_data,
		.get_address = sopc_i2c_get_address,
	};
	SKY_register_iface(class, I2C_DEVICE_INTF_NAME, &i2c_dev_iface);
#endif

	static fpga_gpio_connect_intf fpga_intf =
	{
		.send_data = recv_data
	};
	SKY_register_iface(class, FPGA_GPIO_CONNECT, &fpga_intf);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = iic_omapl138_get_regvalue_by_id,
		.get_regname_by_id = iic_omapl138_get_regname_by_id,
		.set_regvalue_by_id = iic_omapl138_set_regvalue_by_id,
		.get_regnum = iic_omapl138_get_regnum,
		.get_regid_by_name = iic_omapl138_get_regid_by_name
	};
	SKY_register_iface(class, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = FPGA_GPIO_CONNECT,
			.iface = &fpga_intf
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
#if 0		
		(struct InterfaceDescription)
		{
			.name = I2C_DEVICE_INTF_NAME,
			.iface = &i2c_dev_iface
		},
#endif		
	};
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_master,
			.get = get_attr_i2c_master,
		},
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_space_set,
			.get = memory_space_get,
		}
	};
	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);
	sopc_i2c_register_attribute(class);

	class_register_attribute(class);
}


