/*************************************************************************
  > File Name    : bm3803_intc.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/12/24 13:51:00
 ************************************************************************/
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>
#include "bm3803_system_control.h"

void signal_interrupt_to_cpu(conf_object_t *opaque)
{
	bm3803_system_control_device *dev = opaque->obj;
	int irq_num = 0;

	if (CheckIntcReadNum(dev->fifo) == 0)
		return;

	RW_RDLOCK(dev->lock);
	ReadFIFO(dev->fifo, &irq_num, 1);
	//printf("***********read irq_num: %d intNum = %d****************\n", irq_num, CheckIntcReadNum(dev->fifo));
	RW_UNLOCK(dev->lock);

	interrupt_signal_t interrupt_signal;
	interrupt_signal.sparc_signal.irq_no = irq_num;
	interrupt_signal.sparc_signal.level = High_level;

	if (!IFORCE(irq_num) || !IPEND(irq_num))
	{
		CLR_PEND(irq_num);
		CLR_FORCE(irq_num);
	}

	if (dev->signal.intf)
		dev->signal.intf->signal(dev->signal.obj, &interrupt_signal);
}

void select_send_interrupt(conf_object_t *opaque)
{
	bm3803_system_control_device *dev = opaque->obj;
	int i = 15;

	for (; i >= 0; i--)
	{
		if (IPEND(i) || IFORCE(i))
		{
			if (dev->regs.intr_reg.itmp.ilevel != 0)
			{
				if (ILEVEL(i))
				{
					//insert high level interrupt number to fifo
					WriteFIFO(dev->fifo, &i, 1);
					continue;
				}
			} 
			else
			{
				//insert lower level interrupt number to fifo
				WriteFIFO(dev->fifo, &i, 1);
				continue;
			}
		}
	}
}

void interrupt_request_handle(conf_object_t *opaque, int irq_num)
{
	bm3803_system_control_device *dev = opaque->obj;
	
	if (IMASK(irq_num))
	{
		SET_PEND(irq_num);
		SET_FORCE(irq_num);
		//printf("*****************write irq_num: %d\n",irq_num);
		select_send_interrupt(opaque);
	}

}

exception_t intc_memory_write(conf_object_t *opaque, generic_address_t offset, uint32_t data, size_t count)
{
	bm3803_system_control_device *dev = opaque->obj;

	switch (offset)
	{
		case ITMP:
			dev->regs.intr_reg.itmp.all = data;
			break;
		case ITP:
			dev->regs.intr_reg.itp.all = data;
			break;
		case ITF:
			dev->regs.intr_reg.itf.all = data;
			break;
		case ITC:
			dev->regs.intr_reg.itc.all = data;
			dev->regs.intr_reg.itp.ipend &= ~data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in intr_memory_write\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}

exception_t intc_memory_read(conf_object_t *opaque, generic_address_t offset, uint32_t *val, size_t count)
{
	bm3803_system_control_device *dev = opaque->obj;

	switch (offset)
	{
		case ITMP:
			*val = dev->regs.intr_reg.itmp.all;
			break;
		case ITP:
			*val = dev->regs.intr_reg.itp.all;
			break;
		case ITF:
			*val = dev->regs.intr_reg.itf.all;
			break;
		case ITC:
			*val = dev->regs.intr_reg.itc.all;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in intr_memory_read\n", offset);
			return Not_found_exp;
	}

	return No_exp;
}
