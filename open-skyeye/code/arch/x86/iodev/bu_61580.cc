/* Copyright (C) 
* 2015 - Michael.Kang blackfin.kang@gmail.com
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
* @file bu_61580.cc
* @brief 
* @author zyuming zyumingfit@163.com
* @version 7849
* @date 2015-07-23
*/

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_BU_61580 

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
#include "skyeye_system.h"
#include "cpu/cpu.h"
#include "bu_61580.h"

#ifdef __MINGW32__
#include <windows.h>
#else
#endif



bx_bu_61580_c *theBu_61580Device = NULL;
#define LOG_THIS theBu_61580Device->
#define BX_BU_61580_THIS this->

static exception_t bu_61580_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count);
static exception_t bu_61580_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count);

BUFIFO* BUCreateFIFO(unsigned int  FIFOLength)
{
    BUFIFO *fifo = malloc(sizeof(BUFIFO));
    if(fifo == NULL)
	    return NULL;

    network_mesg_t *pfifo;
    pfifo = malloc(FIFOLength*sizeof(network_mesg_t));
    memset(pfifo, 0, FIFOLength*sizeof(network_mesg_t));

    if(pfifo == NULL)
	    return NULL;

    fifo->pFirst = pfifo;
    fifo->pLast = pfifo + FIFOLength-1;
    fifo->Length = FIFOLength;
    fifo->pIn     = fifo->pFirst;
    fifo->pOut    = fifo->pFirst;
    fifo->Enteres = 0;  
    RWLOCK_INIT(fifo->lock);

    return fifo;
}

unsigned int  BUWriteFIFO(BUFIFO *fifo, network_mesg_t *pSource,unsigned int  WriteLength)
{
    unsigned int  i;
//	RW_WRLOCK(fifo->lock);
	for (i = 0; i < WriteLength; i++){
		if (fifo->Enteres >= fifo->Length){
			return i;
		}
		memcpy(fifo->pIn, pSource, sizeof(network_mesg_t));
		pSource++;
		if (fifo->pIn == fifo->pLast)
		{
			fifo->pIn = fifo->pFirst;
		}
		else
		{
			fifo->pIn++;
		}
		//fifo->Enteres ++;
        __sync_fetch_and_add(&(fifo->Enteres),1);
	}
//	RW_UNLOCK(fifo->lock);
    return i;
}



unsigned int  BUReadFIFO(BUFIFO *fifo, network_mesg_t *pAim,unsigned int  ReadLength)
{
    unsigned int  i;
	//RW_WRLOCK(fifo->lock);
	for (i = 0; i < ReadLength; i++){
		if (fifo->Enteres <= 0){
			return i; 
		}
		memcpy(pAim, fifo->pOut, sizeof(network_mesg_t));
		pAim++;
		if (fifo->pOut == fifo->pLast)
		{
			fifo->pOut = fifo->pFirst;
		}
		else
		{
			fifo->pOut++;
		}
		//fifo->Enteres -- ;
        	__sync_fetch_and_sub(&(fifo->Enteres),1);
	}
	//RW_UNLOCK(fifo->lock);
	
	return i;
}




unsigned int  BUCheckFIFOLength(BUFIFO *fifo)
{
    return fifo->Length;
}


network_mesg_t *BUCheckCurrentWritePoint(BUFIFO *fifo)
{
    return (fifo->pIn);
}

network_mesg_t* BUCheckCurrentReadPoint(BUFIFO *fifo)
{
    return (fifo->pOut);
}


void BUFreeFIFO(BUFIFO *fifo)
{
    free(fifo->pFirst);
    free(fifo);
}

unsigned int  BUCheckCanWriteNum(BUFIFO *fifo)
{
    return (fifo->Length - fifo->Enteres);
}

unsigned int  BUCheckCanReadNum(BUFIFO *fifo)
{
    return fifo->Enteres;
}



int libbu_61580_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theBu_61580Device = new bx_bu_61580_c();
  theBu_61580Device->obj = NULL;
  bx_devices.pluginBu_61580 = theBu_61580Device;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theBu_61580Device, BX_PLUGIN_BU_61580);

  return(0); // Success
}

void libbu_61580_LTX_plugin_fini(void)
{
  //delete theBu_61580Device;
}



bx_bu_61580_c::bx_bu_61580_c()
{
  put("IODBG");
}

void bx_bu_61580_c::init(void)
{
  DEV_register_ioread_handler_range(this, read_handler, PointBase, PointBase+ 0xfff, "BOCHS BU_61580", 2);
  DEV_register_iowrite_handler_range(this, write_handler, PointBase, PointBase+ 0xfff, "BOCHS BU_61580", 2);

  DEV_register_ioread_handler_range(this, read_ram_handler, RAM_PointBase, RAM_PointBase+ 0x1000, "BOCHS BU_61580_RAM", 2);
  DEV_register_iowrite_handler_range(this, write_ram_handler, RAM_PointBase, RAM_PointBase+ 0x1000, "BOCHS BU_61580_RAM", 2);
}

Bit32u bx_bu_61580_c::read_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_bu_61580_c *bx_bu_61580_ptr = (bx_bu_61580_c *) this_ptr;
  return bx_bu_61580_ptr->read(addr, io_len);
}

Bit32u bx_bu_61580_c::read_ram_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  bx_bu_61580_c *bx_bu_61580_ptr = (bx_bu_61580_c *) this_ptr;
  return bx_bu_61580_ptr->mem_read(addr, io_len);
}

Bit32u bx_bu_61580_c::read(Bit32u addr, unsigned io_len)
{
	Bit32u data;
	if(theBu_61580Device->obj)
		bu_61580_read((conf_object_t*)theBu_61580Device->obj, addr - PointBase, &data,  io_len);
	return data;
}

Bit32u bx_bu_61580_c::mem_read(Bit32u addr, unsigned io_len)
{
	Bit32u data;
	if(theBu_61580Device->obj)
		bu_61580_ram_read((conf_object_t*)theBu_61580Device->mem_obj, addr - RAM_PointBase, &data,  io_len);
	return data;
}

void bx_bu_61580_c::write_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_bu_61580_c *bx_bu_61580_ptr = (bx_bu_61580_c *) this_ptr;
  bx_bu_61580_ptr->write(addr, dvalue, io_len);
}

void bx_bu_61580_c::write_ram_handler(void *this_ptr, Bit32u addr, Bit32u dvalue, unsigned io_len)
{
  bx_bu_61580_c *bx_bu_61580_ptr = (bx_bu_61580_c *) this_ptr;
  bx_bu_61580_ptr->mem_write(addr, dvalue, io_len);
}

void bx_bu_61580_c::write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	if(theBu_61580Device->obj)
	bu_61580_write(theBu_61580Device->obj, addr - PointBase, &dvalue,  io_len);
	
	return;
}

void bx_bu_61580_c::mem_write(Bit32u addr, Bit32u dvalue, unsigned io_len)
{
	if(theBu_61580Device->obj)
	bu_61580_ram_write(theBu_61580Device->mem_obj, addr - RAM_PointBase, &dvalue,  io_len);
	
	return;
}

static conf_object_t* new_x86_bu61580(char* obj_name)
{
	bu_61580_dev* bu_61580 = skyeye_mm_zero(sizeof(bu_61580_dev));
	bu_61580->obj = new_conf_object(obj_name, bu_61580);
	bu_61580->ram_ptr = skyeye_mm_zero(DEFAULT_RAM_SIZE);
	bu_61580->wt_ptr = skyeye_mm_zero(DEFAULT_RAM_SIZE);
	memset(bu_61580->subaddr_op, 0, SUBADDR_CNT);
	bu_61580->core_signal = 0;
	bu_61580->service_request = 0;
	bu_61580->regs.cr1 = 0x200;
	bu_61580->recv_fifo = BUCreateFIFO(4096);
	RWLOCK_INIT(bu_61580->lock);
	RWLOCK_INIT(bu_61580->signal_lock);
	int i;
	for(i=0;i<VECTOR_ACE_NUM;i++)
		bu_61580->vector_table[i].ram_buffer = skyeye_mm_zero(DEFAULT_RAM_SIZE);

	for(i=0;i<TX_SUBADDR_CNT;i++)
		bu_61580->subaddr_sendData[i].buffer = skyeye_mm_zero(DEFAULT_RAM_SIZE);

  	theBu_61580Device->obj = bu_61580->obj;
	return bu_61580->obj;
}

static exception_t reset_bu_61580(conf_object_t* opaque, const char* args)
{
	bu_61580_dev *dev = opaque->obj;
	memset(dev->ram_ptr, 0, DEFAULT_RAM_SIZE);
	memset(dev->subaddr_op, 0, SUBADDR_CNT);
	return No_exp;

}

static exception_t free_x86_bu61580(conf_object_t* opaque, const char* args)
{
	bu_61580_dev* dev = opaque->obj;
	int i;
	skyeye_free(opaque->objname);
	skyeye_free(dev->ram_ptr);
	for(i=0;i<VECTOR_ACE_NUM;i++)
		skyeye_free(dev->vector_table[i].ram_buffer);

	for(i=0;i<TX_SUBADDR_CNT;i++)
		skyeye_free(dev->subaddr_sendData[i].buffer);
	skyeye_free(dev->obj);
	skyeye_free(dev->recv_fifo);
	skyeye_free(dev);

	return No_exp;        
}


static void bu61580_raise_signal(conf_object_t *obj)
{
	bu_61580_dev *dev = obj->obj;
	RW_WRLOCK(dev->signal_lock);
	DEV_pic_lower_irq(dev->irq_number);
    	DEV_pic_raise_irq(dev->irq_number);
	RW_UNLOCK(dev->signal_lock);
}

int receive_mesg(conf_object_t *opaque, network_mesg_t *message){
	bu_61580_dev* dev = (bu_61580_dev*)(opaque->obj);
	int i, data_block_addr, length, sub_addr, rx_bx_flag, tx_rx_flag;
	length = message->data_length;
	sub_addr = message->sub_addr;
	rx_bx_flag = message->rx_bx_flag;
	tx_rx_flag = message->tx_rx_flag;
	
	if (dev->core_signal == SPARC_CORE){
		dev->ram_ptr[RT_SP + SP_OFFSET_3_LOW] = 0; // receive command 
        dev->ram_ptr[RT_SP + SP_OFFSET_3_HIGH] = (tx_rx_flag << 10) | (sub_addr << 5); // receive command
	}else {
		dev->ram_ptr[RT_SP + SP_OFFSET_3_LOW] = (tx_rx_flag << 10) | (sub_addr << 5); // receive command 
        dev->ram_ptr[RT_SP + SP_OFFSET_3_HIGH] = 0; // receive command
	}
    
    if (tx_rx_flag == 0){
        if (dev->core_signal == SPARC_CORE){
		    int data;
		    //Big endian
		    for(i = 0; i < length/2; i++){
			    data = *(uint16_t *)(message->buffer + i); 
			    *(uint16_t *)(message->buffer + i) = BSWAP_HALF(data);	
		    }
	    }

	    if (rx_bx_flag)
		    data_block_addr = dev->bx_subaddr_table[sub_addr].data_block_pointer;
	    else 
		    data_block_addr = dev->rx_subaddr_table[sub_addr].data_block_pointer;

	    memcpy(&dev->ram_ptr[data_block_addr], message->buffer, length);
    }

    if(dev->regs.imr & RT_MODE_CODE){
        bu61580_raise_signal(opaque);
    }
}

void netmsg_recv_callback(conf_object_t *obj){
    bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);
    network_mesg_t msg;

    if(BUCheckCanReadNum(dev->recv_fifo) == 0){
	    return;
    }

    BUReadFIFO(dev->recv_fifo, &msg, 1);
    receive_mesg(obj, &msg);
}

static exception_t config_bu_61580(conf_object_t* conf_obj){
	bu_61580_dev* dev = conf_obj->obj;
	int id = -1;

    	system_register_timer_handler(conf_obj, 80, (time_sched_t)netmsg_recv_callback, SCHED_MS|SCHED_NORMAL);
}

static exception_t bu_61580_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	bu_61580_dev *dev = opaque->obj;
	uint16_t val ;

	if (dev->core_signal == C6K_CORE)
		offset = offset / 2; 
	else if (dev->core_signal == SPARC_CORE)
		offset = offset / 4; 

	switch(offset){
		case IMR:
			val = dev->regs.imr;
			break;
		case CR1:
			val = dev->regs.cr1;
			break;
		case CR2:
			val = dev->regs.cr2;
			break;
		case SR_SP:
			val = dev->regs.sr_sp;
			break;
		case BCW_RSCW:       
			val = dev->regs.bcw_rscw;
			break;
		case TTR:             
			val = dev->regs.ttr ;
			break;
		case ISR:             
			val = dev->regs.isr;
			dev->regs.isr &= ~END_OF_MESSAGE;
			break;
		case CR3:             
			val = dev->regs.cr3 ;
			break;
		case CR4   :             
			val = dev->regs.cr4 ;
			break;
		case CR5   :             
			val = dev->regs.cr5 ;

			break;
		case DSAR:            
			val = dev->regs.dsar;
			break;
		case FTRR:            
			val = dev->regs.ftrr;
			break;
		case MTRR:            
			val = dev->regs.mtrr;
			break;
		case FT_LS_TR:        
			val = dev->regs.ft_ls_tr;
			break;
		case SWR:             
			val = dev->regs.swr;
			break;
		case BITWR:          
			val = dev->regs.bitwr;
			break;
		case TMR0:            
			val = dev->regs.tmr0;
			break;
		case TMR1:            
			val = dev->regs.tmr1;
			break;
		case TMR2:            
			val = dev->regs.tmr2;
			break;
		case TMR3:            
			val = dev->regs.tmr3;
			break;
		case TMR4:            
			val = dev->regs.tmr4;
			break;
		case TMR5:            
			val = dev->regs.tmr5;
			break;
		case TMR6:            
			val = dev->regs.tmr6;
			break;
		case TMR7:            
			val = dev->regs.tmr7;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in %s\n", offset, __func__);
			return Invarg_exp;


	}
	*(uint16_t *)buf = val;
	return No_exp;
}
int bc_start(conf_object_t *opaque)
{
	bu_61580_dev *dev = opaque->obj;
	int msg_cnt, word_cnt, sub_addr, i, j, word_index;
	uint16_t control_word, command_word, second_command_word;
	std_1553b_mesg_t mesg;
	bc_msg_des_block_t *msg_des;
	msg_cnt = (~(dev->ram_ptr[BC_MESG_CNT])) & 0xffff;
		
	for(i = 0; i < msg_cnt; i++)
	{
		msg_des = (bc_msg_des_block_t *)(dev->ram_ptr + BC_SP);
		word_index = 0;
		control_word = *(dev->ram_ptr + msg_des->msg_block_pointer);
		command_word = *(dev->ram_ptr + msg_des->msg_block_pointer +1);
		word_cnt = command_word & 0x1f;
		sub_addr = (command_word >> 5) & 0x1f;
		if((control_word & RT_RT_FORMAT) && (control_word & BROADCAST_FORMAT)){//RT ->RT && BROADCASE

			printf("%s, %d, %d: rt->rt&broadcast\n", __func__, __LINE__, i);
		}else if((control_word & RT_RT_FORMAT) && ((control_word & BROADCAST_FORMAT) == 0)){//RT->RT
			second_command_word = *(dev->ram_ptr + msg_des->msg_block_pointer +2);
			mesg.type = RT_TO_RT; 
			mesg.words[word_index++].value = command_word; //receive command
			mesg.words[word_index++].value = second_command_word; //transmit command
			mesg.words[word_index++].value = 0; //transmit command looped back init
			mesg.words[word_index++].value = 0; //Tx RT status word 
			for(j = 0; j < word_cnt; j++){
				mesg.words[word_index].value  = *(dev->ram_ptr + msg_des->msg_block_pointer + 1 + word_index);
				word_index++;
			}
			mesg.words[word_index++].value = 0; //Rx RT status word init

		}else{
			if(COMMAND_T_R(command_word) == RT_RECEIVE){ //BC->RT MODE CODE
				if(control_word & MODE_CODE_FORMAT){
					if ((sub_addr == 0) || (sub_addr == 0x1F)){//mode code no data
						mesg.type = MODE_CODE_NO_DATA;
						mesg.words[word_index++].value = command_word;
						mesg.words[word_index++].value = 0; //mode command word looped back init
						mesg.words[word_index++].value = 0; //status received init
						
					}else {
						mesg.type = RX_MODE_CODE_WITH_DATA;
						mesg.words[word_index++].value = command_word;
						mesg.words[word_index].value  = *(dev->ram_ptr + msg_des->msg_block_pointer + 1 + word_index);
						word_index++;
						mesg.words[word_index++].value = 0; //data word looped back init
						mesg.words[word_index++].value = 0; //status received init
					}
				}else{//BC->RT
					mesg.type = BC_TO_RT;
					mesg.words[word_index++].value = command_word;
					for(j = 0; j < word_cnt; j++){
						mesg.words[word_index].value  = *(dev->ram_ptr + msg_des->msg_block_pointer + 1 + word_index);
						word_index++ ;

					}
					mesg.words[word_index++].value = 0; //last data word looped back init
					mesg.words[word_index++].value = 0; //status received init

				}
			}else{ 
				if(control_word & MODE_CODE_FORMAT){ 
					if ((sub_addr == 0) || (sub_addr == 0x1F)){//mode code no data
						mesg.type = MODE_CODE_NO_DATA;
						mesg.words[word_index++].value = command_word;
						mesg.words[word_index++].value = 0; //mode command word looped back init
						mesg.words[word_index++].value = 0; //status received init
						
					}else {
						mesg.type = TX_MODE_CODE_WITH_DATA;
						mesg.words[word_index++].value = command_word;
						mesg.words[word_index++].value = 0; //data word looped back init
						mesg.words[word_index++].value = 0; //status received init
						mesg.words[word_index].value  = *(dev->ram_ptr + msg_des->msg_block_pointer + 1 + word_index);
					}
				//	printf("%s, %d, %d:rt->bc  moding code\n", __func__, __LINE__, i);
				}else{//RT->BC 
					printf("%s, %d, %d:rt->bc\n", __func__, __LINE__, i);
				}


			}
		}
		mesg.bc_mark = BC_SP;
		INIT_MARK_OBJECT(mesg.bc_mark, mesg.type, word_cnt);
		INCREASE_BC_SP4;
		mesg.word_cnt = word_index;
    	        if (dev->std_1553b_linker_iface)
		      dev->std_1553b_linker_iface->bc_send_message(dev->std_1553b_linker, &mesg);

	}
	return 0;
}


static exception_t bu_61580_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	bu_61580_dev *dev = opaque->obj;
	uint16_t val = *(uint16_t *)buf;

	if (dev->core_signal == C6K_CORE)
		offset = offset / 2; 
	else if (dev->core_signal == SPARC_CORE)
		offset = offset / 4; 

	switch(offset){
		case IMR:
			dev->regs.imr = val;
			break;
		case CR1:
			dev->regs.cr1 = val;
    	    if (dev->std_1553b_linker_iface)
				dev->std_1553b_linker_iface->set_mode(dev->std_1553b_linker, opaque, GET_MODE_FROM_VALUE(val));
			break;
		case CR2:
			dev->regs.cr2 = val;
			break;
		case SR_SP:
			dev->regs.sr_sp = val;
			if(val & BC_MT_START){
				if(BU61580_MODE == BC_MODE){
					bc_start(opaque);
				}else if(BU61580_MODE == MT_MODE){
				}
            }
			break;
		case BCW_RSCW:       
			dev->regs.bcw_rscw = val;
			break;
		case TTR:             
			dev->regs.ttr = val;
			break;
		case ISR:             
			dev->regs.isr = val;
			break;
		case CR3:             
			dev->regs.cr3 = val;
			break;
		case CR4:             
			dev->regs.cr4 = val;
			break;
		case CR5:{             
			 dev->regs.cr5 = val;
    	    //	 if (dev->std_1553b_linker_iface)
			//	 	dev->std_1553b_linker_iface->set_address(dev->std_1553b_linker, opaque, GET_ADDRESS_FROM_VALUE(val));
			 }
			 break;
		case DSAR:            
			 dev->regs.dsar = val;
			 break;
		case FTRR:            
			 dev->regs.ftrr = val;
			 break;
		case MTRR:            
			 dev->regs.mtrr = val;
			 break;
		case FT_LS_TR:        
			 dev->regs.ft_ls_tr = val;
			 break;
		case SWR:             
			 dev->regs.swr = val;
			 break;
		case BITWR:          
			 dev->regs.bitwr = val;
			 break;
		case TMR0:            
			 dev->regs.tmr0 = val;
			 break;
		case TMR1:            
			 dev->regs.tmr1 = val;
			 break;
		case TMR2:            
			 dev->regs.tmr2 = val;
			 break;
		case TMR3:            
			 dev->regs.tmr3 = val;
			 break;
		case TMR4:            
			 dev->regs.tmr4 = val;
			 break;
		case TMR5:            
			 dev->regs.tmr5 = val;
			 break;
		case TMR6:            
			 dev->regs.tmr6 = val;
			 break;
		case TMR7:            
			 dev->regs.tmr7 = val;
			 break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in %s\n", offset, __func__);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t bu61580_ram_intf_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	bu_61580_ram_dev* dev = (bu_61580_ram_dev*)(obj->obj);
	dev->bu61580 = obj2;
	dev->bu61580_ram_iface = (bu61580_ram_intf *)SKY_get_iface(dev->bu61580, BU61580_RAM_INTF);
	if(dev->bu61580_ram_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get %s interface from %s fail.", BU61580_RAM_INTF, obj2->objname);
	}

	return No_exp;
}

static exception_t bu61580_ram_intf_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	bu_61580_ram_dev* dev = (bu_61580_ram_dev*)(obj->obj);
	*obj2 = dev->bu61580;
	*port = NULL;
	return No_exp;
}


static attr_value_t get_attr_irq_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	bu_61580_dev* dev = (bu_61580_dev*)(conf_obj->obj);
	attr_value_t irq_number = SKY_make_attr_uinteger(dev->irq_number);
	return irq_number;
}

static exception_t set_attr_irq_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	bu_61580_dev* dev = (bu_61580_dev*)(conf_obj->obj);
	dev->irq_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_rt_addr(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	bu_61580_dev* dev = (bu_61580_dev*)(conf_obj->obj);
	attr_value_t rt_addr = SKY_make_attr_uinteger(dev->rt_addr);
	return rt_addr;
}

static exception_t set_attr_rt_addr(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	bu_61580_dev* dev = (bu_61580_dev*)(conf_obj->obj);
	dev->rt_addr = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_connect_rt_addr(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	bu_61580_dev* dev = (bu_61580_dev*)(conf_obj->obj);
	attr_value_t connect_rt_addr = SKY_make_attr_uinteger(dev->connect_rt_addr);
	return connect_rt_addr;
}

static exception_t set_attr_connect_rt_addr(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	bu_61580_dev* dev = (bu_61580_dev*)(conf_obj->obj);
	dev->connect_rt_addr = SKY_attr_uinteger(*value);
	return No_exp;
}

static void bu_61580_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss, "irq_number", get_attr_irq_number, NULL, set_attr_irq_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of bu61580");
	SKY_register_attribute(clss, "rt_addr", get_attr_rt_addr, NULL, set_attr_rt_addr, NULL, SKY_Attr_Optional, "uinteger", "rt_addr of bu61580");
	SKY_register_attribute(clss, "connect_rt_addr", get_attr_connect_rt_addr, NULL, set_attr_connect_rt_addr, NULL, SKY_Attr_Optional, "uinteger", "connect_rt_addr of bu61580");
	return;
}

exception_t ram_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{

	bu_61580_dev *dev = opaque->obj;
	uint32_t i, dbp_offset, sa, rt_ram_subaddr_ptr, cnt;
	std_1553b_word_t word;
	generic_address_t offset_16;
	uint16_t val = *(uint16_t *)buf;

	//printf("In %s, offset:%x, buf:%x\n", __func__, offset, val);
	if (dev->core_signal == C6K_CORE){
		offset_16 = offset / 2; 
    	}else if(dev->core_signal == C28_CORE){  
		offset_16 = offset; 
	    	rt_ram_subaddr_ptr = offset_16;
    	}else if (dev->core_signal == SPARC_CORE){
		offset_16 = offset; 
	    	rt_ram_subaddr_ptr = offset_16 >> 2;
    	}else {
		offset_16 = offset;
	    	rt_ram_subaddr_ptr = offset_16;
	}

	if (rt_ram_subaddr_ptr == ACE_VECTOR_ADDR){
		dev->ace_vector = val;//矢量字	
		if (dev->ace_vector > 0){
			for (i = 0; i < 16;i++){
				if ((dev->ace_vector >> i) & 0x1){
					switch(i){
						case V_WKMDCHG:
							dev->vector_table[i].sub_addr = 21;	
							dev->vector_table[i].tx_rx_flag = 1;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 2;	
							break;
						case V_CTURM:
							dev->vector_table[i].sub_addr = 3;	
							dev->vector_table[i].tx_rx_flag = 1;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 1024;	
							break;
						case V_PKGRM:
							dev->vector_table[i].sub_addr = 4;	
							dev->vector_table[i].tx_rx_flag = 1;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 256;	
							break;
						case V_FASTRM:
							dev->vector_table[i].sub_addr = 16;	
							dev->vector_table[i].tx_rx_flag = 1;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 64;	
							break;
						case V_BACKUP:
							dev->vector_table[i].sub_addr = 14;	
							dev->vector_table[i].tx_rx_flag = 1;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 256;	
							break;
						case V_RESTORE:
							dev->vector_table[i].sub_addr = 14;	
							dev->vector_table[i].tx_rx_flag = 0;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 256;	
							break;
						case V_KERCTRL:
							dev->vector_table[i].sub_addr = 11;	
							dev->vector_table[i].tx_rx_flag = 0;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 64;	
							break;
						case V_KERDATA:
							dev->vector_table[i].sub_addr = 10;	
							dev->vector_table[i].tx_rx_flag = 1;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 64;	
							break;
						case V_EPHEM:
							dev->vector_table[i].sub_addr = 8;	
							dev->vector_table[i].tx_rx_flag = 1;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 64;	
							break;
						case V_ANSDEVINFO:
							dev->vector_table[i].sub_addr = 19;	
							dev->vector_table[i].tx_rx_flag = 0;	
							dev->vector_table[i].bx_flag = 1;	
							dev->vector_table[i].data_length = 64;	
							break;
						case V_RELAPOS:
							dev->vector_table[i].sub_addr = 6;	
							dev->vector_table[i].tx_rx_flag = 0;	
							dev->vector_table[i].bx_flag = 1;	
							dev->vector_table[i].data_length = 64;	
							break;
						case V_LOCALPOS:
							dev->vector_table[i].sub_addr = 7;	
							dev->vector_table[i].tx_rx_flag = 0;	
							dev->vector_table[i].bx_flag = 1;	
							dev->vector_table[i].data_length = 64;	
							break;
						case V_MEMDUMP:
							dev->vector_table[i].sub_addr = 18;	
							dev->vector_table[i].tx_rx_flag = 1;	
							dev->vector_table[i].bx_flag = 0;	
							dev->vector_table[i].data_length = 768;	
							break;
						default:
							printf("%s %d Not set ace vector subaddr %d\n", __func__, __LINE__, i);
							break;
					}
				}
			}
		}
	}else if (rt_ram_subaddr_ptr >= RT_SUBADDR_R_A_BASE
			&& rt_ram_subaddr_ptr < RT_SUBADDR_W_A_BASE){
		sa = rt_ram_subaddr_ptr - RT_SUBADDR_R_A_BASE;//RT接收表子地址
		dev->rx_subaddr_table[sa].data_block_pointer = val;//数据块地址偏移	
	}else if(rt_ram_subaddr_ptr >= RT_SUBADDR_B_A_BASE
		       	&& rt_ram_subaddr_ptr < RT_SUBADDR_C_A_BASE){ 
		sa = rt_ram_subaddr_ptr - RT_SUBADDR_B_A_BASE;//RT广播接收表子地址
		dev->bx_subaddr_table[sa].data_block_pointer = val;//数据块地址偏移	

	}else if (rt_ram_subaddr_ptr >= RT_SUBADDR_W_A_BASE
			&& rt_ram_subaddr_ptr < RT_SUBADDR_B_A_BASE){
		sa = rt_ram_subaddr_ptr - RT_SUBADDR_W_A_BASE;//RT发送表子地址
		dev->tx_subaddr_table[sa].data_block_pointer = val;//数据块地址偏移	

	}else if (rt_ram_subaddr_ptr >= RT_SUBADDR_C_A_BASE
			&& rt_ram_subaddr_ptr < RT_SUBADDR_R_B_BASE){
		sa = rt_ram_subaddr_ptr - RT_SUBADDR_C_A_BASE;//RT子地址控制字子地址
		dev->subaddr_ctrlwd_table[sa].intr_mode = val;//中断模式	

	}else if (rt_ram_subaddr_ptr >= RT_DATA_BLOCK_START
			&& rt_ram_subaddr_ptr <= RT_DATA_BLOCK_END){
		*(dev->ram_ptr + rt_ram_subaddr_ptr) = *(uint16_t *)buf;	
	}else {
		*(dev->ram_ptr + offset_16) = *(uint16_t *)buf;
	}

	if (rt_ram_subaddr_ptr >= RT_SUBADDR_W_START_MEMBASE 
			&& rt_ram_subaddr_ptr < RT_SUBADDR_W_END_MEMBASE){
		sa = (rt_ram_subaddr_ptr - RT_SUBADDR_W_START_MEMBASE) / SUBADDR_MAX_DATA_NUM; //发送子地址
		cnt = dev->subaddr_sendData[sa].cnt;
		dev->subaddr_sendData[sa].buffer[cnt] = val;
		dev->subaddr_sendData[sa].cnt ++;
		if (dev->subaddr_sendData[sa].cnt == SUBADDR_MAX_DATA_NUM){
			uint16_t data;
			network_mesg_t network_mesg;

			network_mesg.sub_addr = sa;
			network_mesg.rt_addr = dev->connect_rt_addr;
		        network_mesg.rx_bx_flag = 0; //接收类型 0 为接收 1 为广播  
			network_mesg.tx_rx_flag = 1; //发送类型 1: RT为接收端  
		        network_mesg.data_length = SUBADDR_MAX_DATA_NUM; //接收数据长度
			for(i = 0; i < SUBADDR_MAX_DATA_NUM; i++){
				data = *(uint16_t *)(dev->subaddr_sendData[sa].buffer + i); 
				*(uint16_t *)(dev->subaddr_sendData[sa].buffer + i) = BSWAP_HALF(data);	
			}
			memcpy(network_mesg.buffer, dev->subaddr_sendData[sa].buffer, 2 * SUBADDR_MAX_DATA_NUM);
			//发送给1553B数据激励模块和其他RT设备
			if (dev->bridge_iface){
				//1553B数据帧头不为0时候发送
				if (dev->subaddr_sendData[sa].buffer[0] != 0){
					dev->bridge_iface->receive_msg(dev->bridge, &network_mesg, 2);
				}
			}	
				
			dev->subaddr_sendData[sa].cnt = 0;
		}
	}
	
	for(i = 0; i < SUBADDR_CNT; i++){
		if(dev->subaddr_op[i] == 0)
			continue;
		dbp_offset =  offset_16 - dev->subaddr_op_data[i].data_block_pointer;
		if((dbp_offset >= 0) && (dbp_offset <= 0x1f)){
			word.mesg_type = dev->subaddr_op_data[i].mesg_type;
			word.value = *(uint16_t *)buf;
			dev->subaddr_op_data[i].counter +=1;
			if(dev->subaddr_op_data[i].counter == dev->subaddr_op_data[i].data_cnt){
				dev->subaddr_op[i] = 0;
				word.type = LAST_DATA_WORD;
			}else{
				word.type = DATA_WORD;
			}
    	       		if (dev->std_1553b_linker_iface)
				dev->std_1553b_linker_iface->reponse_word(dev->std_1553b_linker, opaque, &word, i);
		}
	}
	
	return No_exp;
}
exception_t ram_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	bu_61580_dev *dev = opaque->obj;
	generic_address_t offset_16 ;
	int rt_ram_subaddr_ptr, sa;

	if (dev->core_signal == C6K_CORE)
		offset_16 = offset / 2; 
	else if (dev->core_signal == C28_CORE){ 
		offset_16 = offset; 
	    	rt_ram_subaddr_ptr = offset_16;
    	}else if (dev->core_signal == SPARC_CORE){
		offset_16 = offset; 
	    	rt_ram_subaddr_ptr = offset_16 >> 2;
    	}else {
		offset_16 = offset;
	    	rt_ram_subaddr_ptr = offset_16;
	}

	if (rt_ram_subaddr_ptr == ACE_VECTOR_ADDR){
		*(uint16_t *)buf = dev->ace_vector;
	}else if (rt_ram_subaddr_ptr >= RT_DATA_BLOCK_START
			&& rt_ram_subaddr_ptr <= RT_DATA_BLOCK_END){
		*(uint16_t *)buf = *(uint16_t *)(dev->ram_ptr + rt_ram_subaddr_ptr);
	}else if (rt_ram_subaddr_ptr >= RT_SUBADDR_R_A_BASE
			&& rt_ram_subaddr_ptr < RT_SUBADDR_W_A_BASE){
		sa = rt_ram_subaddr_ptr - RT_SUBADDR_R_A_BASE;//RT接收表子地址
		*(uint16_t *)buf = dev->rx_subaddr_table[sa].data_block_pointer;

	}else if(rt_ram_subaddr_ptr >= RT_SUBADDR_B_A_BASE
		       	&& rt_ram_subaddr_ptr < RT_SUBADDR_C_A_BASE){ 
		sa = rt_ram_subaddr_ptr - RT_SUBADDR_B_A_BASE;//RT广播接收表子地址
		*(uint16_t *)buf = dev->bx_subaddr_table[sa].data_block_pointer;

	}else if (rt_ram_subaddr_ptr >= RT_SUBADDR_W_A_BASE
			&& rt_ram_subaddr_ptr < RT_SUBADDR_B_A_BASE){
		sa = rt_ram_subaddr_ptr - RT_SUBADDR_W_A_BASE;//RT发送表子地址
		*(uint16_t *)buf = dev->tx_subaddr_table[sa].data_block_pointer;//数据块地址偏移	
	}else if (rt_ram_subaddr_ptr >= RT_SUBADDR_C_A_BASE
			&& rt_ram_subaddr_ptr < RT_SUBADDR_R_B_BASE){
		sa = rt_ram_subaddr_ptr - RT_SUBADDR_C_A_BASE;//RT子地址控制字子地址
		*(uint16_t *)buf = dev->subaddr_ctrlwd_table[sa].intr_mode;//中断模式	
	}else {
		*(uint16_t *)buf = *(uint16_t *)(dev->ram_ptr + offset_16);
	}

	return No_exp;
}

static int receive_network_mesg(conf_object_t *obj, network_mesg_t *message){
	bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);

	RW_WRLOCK(dev->lock);
	BUWriteFIFO(dev->recv_fifo, message, 1);
	RW_UNLOCK(dev->lock);
     
	return 0;
}

void send_data_to_network(conf_object_t *obj, int sub_addr, int length, uint16_t* send_buffer){
	bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);
	dev->network_mesg.sub_addr = sub_addr;
	dev->network_mesg.data_length = length;

	if (dev->core_signal == SPARC_CORE){
		int i, data;
		//Big endian
		for(i = 0; i < length/2; i++){
			data = *(uint16_t *)(send_buffer + i); 
			*(uint16_t *)(send_buffer + i) = BSWAP_HALF(data);	
		}
	}
	memcpy(dev->network_mesg.buffer, send_buffer, length);

	if (dev->network_iface){
		dev->network_iface->send_network_mesg(dev->network, &dev->network_mesg);
	}
}


void check_ace_vector(conf_object_t *obj, std_1553b_mesg_t *mesg){
	bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);
    	network_mesg_t network_mesg;
    	vector_table_t vector_mesg; 
	int sub_addr, tx_rx_flag, data_length, data_block_addr;
	int i, j, aocs_status;

	for (i = 0; i < 16;i++){
        	if ((dev->ace_vector >> i) & 0x1){
			//判断子地址和子地址数据块
	      		sub_addr = dev->vector_table[i].sub_addr;
	        	tx_rx_flag = dev->vector_table[i].tx_rx_flag;
		    	data_length = dev->vector_table[i].data_length;
            
    			if (tx_rx_flag){ 
	     	  		data_block_addr = dev->tx_subaddr_table[sub_addr].data_block_pointer; //发送数据块地址
		    		memcpy(dev->vector_table[i].ram_buffer, &dev->ram_ptr[data_block_addr], data_length);
		    	//	output_data_to_file(obj, dev->vector_table[i].ram_buffer, data_length, i);
                		if (V_WKMDCHG == i){
                        		network_mesg.sub_addr = sub_addr;
		                	network_mesg.rx_bx_flag = 0; //接收类型 0 为接收 1 为广播  
		                	network_mesg.data_length = data_length; //接收数据长度
                        		for(j=0;j<data_length/2;j++){
                           	 		network_mesg.buffer[j] = NEG_SWAP_HALF_WORD(dev->vector_table[i].ram_buffer[j]);
                        		}
                        		receive_network_mesg(obj, &network_mesg);
					if (dev->bridge_iface){
                            			memcpy(network_mesg.buffer, dev->vector_table[i].ram_buffer, data_length);
                            			dev->bridge_iface->receive_msg(dev->bridge, &network_mesg, 1);
                        		}
				}
                	}else {
                    		if (dev->aocs_iface){
                    			aocs_status = dev->aocs_iface->get_aocs_status(dev->aocs);  
                        		if (aocs_status)
		    	            		send_data_to_network(obj, sub_addr, data_length, dev->vector_table[i].ram_buffer);
                    		}else{ 
		    	        	send_data_to_network(obj, sub_addr, data_length, dev->vector_table[i].ram_buffer);
                    		}

                    		/* 发送给转发软件: 内存下卸 和 快遥数据 以及 备份参数*/
                    		if (V_MEMDUMP == i || V_FASTRM == i || V_BACKUP == i){ 
                        		if (dev->bridge_iface){
                            			network_mesg.sub_addr = sub_addr;
		            			network_mesg.data_length = data_length; //接收数据长度
                            			memcpy(network_mesg.buffer, dev->vector_table[i].ram_buffer, data_length);
                            			dev->bridge_iface->receive_msg(dev->bridge, &network_mesg, 1);
                        		}
                    		}
               	 	}
                	network_mesg.sub_addr = sub_addr;
                	network_mesg.tx_rx_flag = tx_rx_flag;
			network_mesg.data_length = data_length; //接收数据长度
               		BUWriteFIFO(dev->recv_fifo, &network_mesg, 1);
   	    	}
	}
}


int rt_receive_message(conf_object_t *obj, std_1553b_mesg_t *mesg)
{
	bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);
	uint32_t sub_addr, data_block_addr, data_cnt, sp, mode_code_type;
	std_1553b_word_t word;
	int i, data_length, bx_flag = 0, tx_rx_flag = 0;
	dev->regs.swr |=0x8; //set rt to busy
	switch(mesg->type){
		case BC_TO_RT:{
	        sub_addr = COMMAND_SUB_ADDR((mesg->words[0].value));
   	        data_block_addr = dev->ram_ptr[RT_SUBADDR_R_BASE + sub_addr];
 	        data_cnt = COMMAND_DATA_CNT(mesg->words[0].value);
    	    for(i = 0; i < data_cnt; i++){
		        dev->ram_ptr[data_block_addr + i] = mesg->words[i + 1].value;
		    }
		    dev->ram_ptr[RT_SP] = 0; // block status word
		    INCREASE_RT_SP;
		    dev->ram_ptr[RT_SP] = 0; // time tag word
		    INCREASE_RT_SP;
		    dev->ram_ptr[RT_SP] = data_block_addr; // message block addr
		    INCREASE_RT_SP;
		    dev->ram_ptr[RT_SP] = mesg->words[0].value; // receive command word
		    INCREASE_RT_SP;

		    //reponse last data word looped back
		    word.type = LAST_DATA_WORD_LOOPED_BACK;
		    word.value = mesg->words[data_cnt].value;
		    word.mesg_type = BC_TO_RT;
		    if (dev->std_1553b_linker_iface)
		 	    dev->std_1553b_linker_iface->reponse_word(dev->std_1553b_linker, obj, &word, sub_addr);
			//reponse status word
			    word.type = STATUS_WORD;
			    word.value = GET_STATUS_WORD(RT_ADDRESS, 0,0,0,0,0,0,1,0);
			    if (dev->std_1553b_linker_iface)
			        dev->std_1553b_linker_iface->reponse_word(dev->std_1553b_linker, obj, &word,sub_addr);
			    if(dev->regs.imr & RT_SUBADDRESS_CONTROL_WORD_EOM){
				    dev->regs.isr |= MASK_INTERRUPT | RT_SUBADDRESS_CONTROL_WORD_EOM | END_OF_MESSAGE;
				    bu61580_raise_signal(obj);
				}
	    }
	    break;
		case RT_TO_RT:{

	        if(mesg->rtr_status == RT_TO_RT_TRANSMIT){
		        sub_addr = COMMAND_SUB_ADDR((mesg->words[1].value));
			    data_block_addr = dev->ram_ptr[RT_SUBADDR_W_BASE + sub_addr];
			    dev->ram_ptr[RT_SP] = 0; // block status word
			    INCREASE_RT_SP;
			    dev->ram_ptr[RT_SP] = 0; // time tag word
			    INCREASE_RT_SP;
			    dev->ram_ptr[RT_SP] = data_block_addr; // message block addr
			    INCREASE_RT_SP;
			    dev->ram_ptr[RT_SP] = mesg->words[1].value; // receive command word
			    INCREASE_RT_SP;

		        //reponse transmit command looped back
			    word.type = TRANSMIT_COMMAND_WORD_LOOPED_BACK;
			    word.value = mesg->words[1].value;
			    word.mesg_type = RT_TO_RT;
			    if (dev->std_1553b_linker_iface)
			        dev->std_1553b_linker_iface->reponse_word(dev->std_1553b_linker, obj, &word, sub_addr);
				//reponse tx rt status word 
			    word.type = TX_RT_STATUS_WORD;
			    word.value = GET_STATUS_WORD(RT_ADDRESS, 0,0,0,0,0,0,1,0);

			    if (dev->std_1553b_linker_iface)
			        dev->std_1553b_linker_iface->reponse_word(dev->std_1553b_linker, obj, &word,sub_addr);

			    dev->subaddr_op[sub_addr] = 1;
				dev->subaddr_op_data[sub_addr].data_block_pointer = data_block_addr;
				dev->subaddr_op_data[sub_addr].mesg_type = RT_TO_RT;
				dev->subaddr_op_data[sub_addr].data_cnt = COMMAND_DATA_CNT(mesg->words[1].value);
				dev->subaddr_op_data[sub_addr].counter = 0;

				if(dev->regs.imr & RT_SUBADDRESS_CONTROL_WORD_EOM){
				    dev->regs.isr |= MASK_INTERRUPT | RT_SUBADDRESS_CONTROL_WORD_EOM | END_OF_MESSAGE;
				    bu61580_raise_signal(obj);
				}
			}else{
				sub_addr = COMMAND_SUB_ADDR((mesg->words[0].value));

				data_block_addr = dev->ram_ptr[RT_SUBADDR_R_BASE + sub_addr];
				data_cnt = COMMAND_DATA_CNT(mesg->words[0].value);
				for(i = 0; i < data_cnt; i++){
				    dev->ram_ptr[data_block_addr + i] = mesg->words[i + 4].value;
				}
				dev->ram_ptr[RT_SP] = 0; // block status word
				INCREASE_RT_SP;
				dev->ram_ptr[RT_SP] = 0; // time tag word
				INCREASE_RT_SP;
				dev->ram_ptr[RT_SP] = data_block_addr; // message block addr
				INCREASE_RT_SP;
				dev->ram_ptr[RT_SP] = mesg->words[0].value; // receive command word
				INCREASE_RT_SP;

				word.type = RX_RT_STATUS_WORD;
				word.mesg_type = RT_TO_RT;
				word.value = GET_STATUS_WORD(RT_ADDRESS, 0,0,0,0,0,0,1,0);
				if (dev->std_1553b_linker_iface)
				    dev->std_1553b_linker_iface->reponse_word(dev->std_1553b_linker, obj, &word,sub_addr);
			    if(dev->regs.imr & RT_SUBADDRESS_CONTROL_WORD_EOM){
			        dev->regs.isr |= MASK_INTERRUPT | RT_SUBADDRESS_CONTROL_WORD_EOM | END_OF_MESSAGE;
				    bu61580_raise_signal(obj);
				}
		    }
		}
	    break;
		case RT_TO_BC:
	        break;
		case RT_TO_RT_BROADCAST:
		    break;
		case BROADCAST:
		    break;
		case MODE_CODE_NO_DATA:
		    mode_code_type = mesg->words[0].value & 0x1F;
                  
		    if (mode_code_type == TRANSMIT_STATUS_WORD){ //发送状态命令字
		        sub_addr = COMMAND_SUB_ADDR((mesg->words[0].value));
			    dev->ram_ptr[RT_SP + SP_OFFSET_0_LOW] = 0; // block status word
			    dev->ram_ptr[RT_SP + SP_OFFSET_0_HIGH] = 0; // block status word
			    dev->ram_ptr[RT_SP + SP_OFFSET_1_LOW] = 0; // time tag word
			    dev->ram_ptr[RT_SP + SP_OFFSET_1_HIGH] = 0; // time tag word
			    dev->ram_ptr[RT_SP + SP_OFFSET_2_LOW] = 0; // message block addr
			    dev->ram_ptr[RT_SP + SP_OFFSET_2_HIGH] = 0; // message block addr
			    if (dev->core_signal == SPARC_CORE){
				    dev->ram_ptr[RT_SP + SP_OFFSET_3_LOW] = 0; // receive command 
			        dev->ram_ptr[RT_SP + SP_OFFSET_3_HIGH] = mesg->words[0].value; // receive command
				}else {
			        dev->ram_ptr[RT_SP + SP_OFFSET_3_LOW] = mesg->words[0].value; // receive command 
			        dev->ram_ptr[RT_SP + SP_OFFSET_3_HIGH] = 0; // receive command
				}
				if(dev->regs.imr & RT_MODE_CODE){
			        dev->regs.isr |= MASK_INTERRUPT | RT_MODE_CODE | END_OF_MESSAGE;
			        bu61580_raise_signal(obj);
			    }
				//reponse last command word looped back
				word.type = MODE_COMMAND_WORD_LOOPED_BACK;
			    word.value = mesg->words[0].value;
			    word.mesg_type = MODE_CODE_NO_DATA;
				if (dev->std_1553b_linker_iface)
				    dev->std_1553b_linker_iface->reponse_word(dev->std_1553b_linker, obj, &word, sub_addr);

				if ((dev->regs.cr1 >> 9 & 0x1) == 0){
				    dev->service_request = 1;
				}

			    //reponse status word
			    word.type = STATUS_WORD;
			    word.value = GET_STATUS_WORD(RT_ADDRESS, 0,0,dev->service_request,0,0,0,1,0); //服务请求位
			    word.mesg_type = MODE_CODE_NO_DATA;
			    if (dev->std_1553b_linker_iface)
			        dev->std_1553b_linker_iface->reponse_word(dev->std_1553b_linker, obj, &word, sub_addr);

			    }else if (mode_code_type == TRANSMIT_VECTOR_WORD){ //发送矢量字
				    if (dev->ace_vector > 0){
				        check_ace_vector(obj, mesg);
				    }
		  	      	dev->service_request = 0;
			    }else {
			        skyeye_log(Error_log, __func__, "Not implement Mode code type = %x\n", mode_code_type);
			    }
                  
			break;
		case TX_MODE_CODE_WITH_DATA:
			break;
		case RX_MODE_CODE_WITH_DATA:
			break;
		case BROADCAST_MODE_CODE_NO_DATA:
			break;
		case BROADCAST_MODE_CODE_WITH_DATA:
			break;
		default:
		    skyeye_log(Info_log, __FUNCTION__, "unknow 1553b message type\n", obj->objname);
	}

	dev->regs.swr &= ~0x8; //set rt to idle
	return 0;
}

position_t bc_receive_word(conf_object_t *obj, std_1553b_word_t *word, uint32_t bc_mark)
{
	mark_object_t *mark_object;
	bu_61580_dev *dev = obj->obj;
	int32_t wtp; 
	uint32_t pos;
	bc_msg_des_block_t *msg_des;
	calc_func_t calc_func;
	wtp = word_type_pos_table[word->mesg_type][word->type]; 
	calc_func =calc_func_table[word->mesg_type][word->type];
	if(wtp == NP){
		//do nothing
	}else if(wtp == NC){
		mark_object = GET_MARK_OBJECT(bc_mark);
		pos = calc_func(mark_object);
		if((word->type == DATA_WORD)||(word->type == LAST_DATA_WORD)){
			if(mark_object->data_counter < mark_object->data_cnt)
				mark_object->data_counter++;
		}
	}else{
		pos = wtp;
	}
	msg_des = (bc_msg_des_block_t *)dev->ram_ptr + bc_mark / 4;
	(dev->ram_ptr)[msg_des->msg_block_pointer + pos] = word->value; 

	return pos;
}

char* bu_61580_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	bu_61580_dev *dev = conf_obj->obj;

	return NULL;
}


uint32_t bu_61580_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	bu_61580_dev *dev = conf_obj->obj;
	return 0;
}

static exception_t bu_61580_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	bu_61580_dev *dev = conf_obj->obj;

	return No_exp;
}

static uint32_t bu_61580_get_regnum(conf_object_t* conf_obj)
{
	bu_61580_dev *dev = conf_obj->obj;
	return  0;
}

static uint32_t bu_61580_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	bu_61580_dev *dev = conf_obj->obj;
	return 0;
}

static void receive_bc_message(conf_object_t *obj, network_mesg_t *message){
    	bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);
    	int i, data_block_addr, length, sub_addr, rt_addr, rx_bx_flag, tx_rx_flag, stack_a;
	uint16_t data;

	length = message->data_length;
	sub_addr = message->sub_addr;
    	rt_addr = message->rt_addr;
	rx_bx_flag = message->rx_bx_flag;
	tx_rx_flag = message->tx_rx_flag;

    	if (AREA == 0){
	    	stack_a = STACK_A + 4 * dev->ram_ptr[STACKPOINT_A]; 
        	dev->ram_ptr[stack_a] = 0x8000; // block status word
		dev->ram_ptr[stack_a + 1] = 0; // time tag word
		dev->ram_ptr[stack_a + 2] = (tx_rx_flag << 10); // 有问题，但是8部驱动中是这样实现
        	dev->ram_ptr[stack_a + 3] = (sub_addr << 5); // receive command
	    	dev->ram_ptr[STACKPOINT_A] += 1; //消息计数 

        	if (rx_bx_flag)
			data_block_addr = dev->bx_subaddr_table[sub_addr].data_block_pointer;
		else 
			data_block_addr = dev->rx_subaddr_table[sub_addr].data_block_pointer;

   		for(i = 0; i < length; i++){
			data = *(uint16_t *)(message->buffer + i); 
			*(uint16_t *)(message->buffer + i) = BSWAP_HALF(data);	
		}
    		memcpy(&dev->ram_ptr[data_block_addr], message->buffer, length*2);
    	}

    	if(dev->regs.imr & END_OF_MESSAGE){
        	dev->regs.isr |= END_OF_MESSAGE;
        	bu61580_raise_signal(obj);
    	}
}
static int recv_msg_from_bridge(conf_object_t *obj, network_mesg_t *msg, int count){
	bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);
	if (dev->rt_addr == msg->rt_addr)
     		receive_bc_message(obj, msg);

     	return 0;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
	if (dev->uart_file_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'uart_file_intf' interface from %s fail.", obj2->objname);
		return Not_found_exp;
	}
	return No_exp;
}
 
static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

static exception_t std_1553b_linker_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	bu_61580_dev *dev = (bu_61580_dev*)(obj->obj);
	*obj2 = dev->std_1553b_linker;
	*port = NULL;

	return No_exp;
}

static exception_t std_1553b_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	bu_61580_dev* dev = (bu_61580_dev*)(obj->obj);
	dev->std_1553b_linker = obj2;
	dev->std_1553b_linker_iface = (std_1553b_intf *)SKY_get_iface(dev->std_1553b_linker, STD_1553B_INTF);
	if (dev->std_1553b_linker_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);
	}
	return No_exp;
}

static exception_t set_attr_network(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	bu_61580_dev *dev = obj->obj;
	dev->network = obj2;
	dev->network_iface = (network_bus_intf *)SKY_get_iface(obj2, NETWORK_BUS_INTF);

	return No_exp;
}
 
static exception_t get_attr_network(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	bu_61580_dev *dev = obj->obj;
	*obj2 = dev->network;
	*port = NULL;
	return No_exp;
}

static exception_t set_attr_bu61580_bridge(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	bu_61580_dev *dev = obj->obj;
	dev->bridge = obj2;
	dev->bridge_iface = (bu61580_bridge_intf *)SKY_get_iface(obj2, BU61580_BRIDGE_INTF);

	return No_exp;
}
 
static exception_t get_attr_bu61580_bridge(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	bu_61580_dev *dev = obj->obj;
	*obj2 = dev->bridge;
	*port = NULL;
	return No_exp;
}

static exception_t aocs_intf_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	bu_61580_dev* dev = (obj->obj);
	dev->aocs = obj2;
	dev->aocs_iface = (get_aocs_intf*)SKY_get_iface(dev->aocs, GET_AOCS_INTF);

	if (dev->aocs_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'aocs_iface' interface from %s fail.", obj2->objname);
	}
	return No_exp;

}
 
static exception_t aocs_intf_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	bu_61580_dev* dev = (obj->obj);
	*obj2 = dev->aocs;
	*port = NULL;
	return No_exp;
}


extern "C" void init_x86_bu61580();

void init_x86_bu61580(){
	static skyeye_class_t class_data = {
		class_name : "x86_bu61580",
		class_desc : "x86_bu61580",
		new_instance : new_x86_bu61580,
		free_instance : free_x86_bu61580,
		reset_instance : reset_bu_61580,
		get_attr : NULL,
		set_attr : NULL,
		config_instance: config_bu_61580
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		conf_obj:NULL,
		read :bu_61580_read,
		write : bu_61580_write,
		get_page:NULL,
		read_hl:NULL,
		write_hl:NULL
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const bu61580_ram_intf bu61580_ram_iface = {
		read :ram_read,
		write : ram_write,
	};
	SKY_register_iface(clss, BU61580_RAM_INTF, &bu61580_ram_iface);

	static const std_1553b_intf std_1553b_dev_iface = {
		bc_send_message:NULL,
		rt_receive_message:rt_receive_message,
		reponse_word:NULL,
		bc_receive_word : NULL,
		register_address:NULL,
		set_mode:NULL,
		set_address:NULL
	};
	SKY_register_iface(clss, STD_1553B_INTF, &std_1553b_dev_iface);

	static const skyeye_reg_intf reg_intf = {
		conf_obj:NULL,
		get_regvalue_by_id : bu_61580_get_regvalue_by_id,
		get_regname_by_id : bu_61580_get_regname_by_id,
		set_regvalue_by_id : bu_61580_set_regvalue_by_id,
		get_regid_by_name : bu_61580_get_regid_by_name,
		get_regnum : bu_61580_get_regnum,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const network_bus_intf network_intf = {
		receive_network_mesg : receive_network_mesg 
	};
	SKY_register_iface(clss, NETWORK_BUS_INTF, &network_intf);

	 static const bu61580_bridge_intf bu61580_bridge_interface = {
		receive_msg : recv_msg_from_bridge,
	};
	SKY_register_iface(clss, BU61580_BRIDGE_INTF, &bu61580_bridge_interface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			name :SKYEYE_REG_INTF,
			iface: &reg_intf,
		},
		(struct InterfaceDescription) {
			name : MEMORY_SPACE_INTF_NAME, 
			iface : &io_memory,
		},
		(struct InterfaceDescription) {
			name : BU61580_RAM_INTF,
			iface : &bu61580_ram_iface,
		},
		(struct InterfaceDescription) {
			name : STD_1553B_INTF,
			iface : &std_1553b_dev_iface,
		},
		(struct InterfaceDescription) {
			name : NETWORK_BUS_INTF, 
			iface : &network_intf,
		},
        	(struct InterfaceDescription) {
			name : BU61580_BRIDGE_INTF, 
			iface : &bu61580_bridge_interface,
		}
	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			name : UART_FILE_INTF,
			set : uart_file_set,
			get : uart_file_get,
		},
		(struct ConnectDescription) {
			name :STD_1553B_INTF,
			set : std_1553b_linker_set,
			get : std_1553b_linker_get,
		},
		(struct ConnectDescription) {
			name : NETWORK_BUS_INTF,
			set : set_attr_network,
			get : get_attr_network,
		},
        (struct ConnectDescription) {
			name :BU61580_BRIDGE_INTF,
			set : set_attr_bu61580_bridge,
			get : get_attr_bu61580_bridge,
		},
        (struct ConnectDescription ){
			name : GET_AOCS_INTF,
			set : aocs_intf_set,
			get : aocs_intf_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	bu_61580_register_attribute(clss);

}

static conf_object_t* new_bu_61580_ram(const char* obj_name)
{
	bu_61580_ram_dev* bu_61580_ram = skyeye_mm_zero(sizeof(bu_61580_ram_dev));
	bu_61580_ram->obj = new_conf_object(obj_name, bu_61580_ram);

  	theBu_61580Device->mem_obj = bu_61580_ram->obj;

	return bu_61580_ram->obj;
}

static exception_t reset_bu_61580_ram(conf_object_t* opaque, const char* args)
{
	bu_61580_ram_dev *dev = opaque->obj;
	return No_exp;
}

static exception_t free_bu_61580_ram(conf_object_t* conf_obj)
{
	bu_61580_ram_dev* dev = conf_obj->obj;
	skyeye_free(dev->obj);
	skyeye_free(dev);
}

static exception_t bu_61580_ram_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	bu_61580_ram_dev *dev = opaque->obj;
	dev->bu61580_ram_iface->write(dev->bu61580, offset, buf, count);
	return No_exp;
}

static exception_t bu_61580_ram_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	bu_61580_ram_dev *dev = opaque->obj;
	dev->bu61580_ram_iface->read(dev->bu61580, offset, buf, count);
	return No_exp;
}

extern "C" void init_bu_61580_ram();
void init_bu_61580_ram(){
	static skyeye_class_t class_data = {
		class_name : "x86_bu_61580_ram",
		class_desc : "x86_bu_61580_ram",
		new_instance : new_bu_61580_ram,
		free_instance : free_bu_61580_ram,
		reset_instance : reset_bu_61580_ram,
		get_attr : NULL,
		set_attr : NULL,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		conf_obj:NULL,
		read : bu_61580_ram_read,
		write : bu_61580_ram_write,
		get_page:NULL,
		read_hl:NULL,
		write_hl:NULL
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			name : MEMORY_SPACE_INTF_NAME, 
			iface : &io_memory,
		}
	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription) {
			name:BU61580_RAM_INTF,
			set : bu61580_ram_intf_set,
			get : bu61580_ram_intf_get,
		}
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	return;
}
#endif 

