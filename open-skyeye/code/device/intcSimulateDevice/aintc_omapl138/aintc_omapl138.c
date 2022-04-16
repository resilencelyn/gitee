/* Copyright (C)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundatdev->; either version 2
* of the License, or (at your optdev->) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundatdev->, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/
/**
* @file aintc_omapl138.c
* @brief The implementatdev-> of system controller
* @author
* @versdev-> 78.77
*/

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

#include "aintc_omapl138.h"

static int get_channel_index(aintc_omapl138_device *dev, int line)
{
	int i, j, mask = 0xff;

	i = line / 4;
	j = line % 4;

	return (dev->regs.aintc_cmr[i] >> 8 * j) & mask;
}
static void raise_irq(aintc_omapl138_device *dev)
{
	interrupt_signal_t interrupt_signal;

	interrupt_signal.arm_signal.firq = Prev_level;
	interrupt_signal.arm_signal.irq = Low_level;
	interrupt_signal.arm_signal.reset = Prev_level;

	if(dev->signal_iface)
		dev->signal_iface->signal(dev->signal, &interrupt_signal);
}
static void raise_fiq(aintc_omapl138_device *dev)
{
	interrupt_signal_t interrupt_signal;

	interrupt_signal.arm_signal.firq = Low_level;
	interrupt_signal.arm_signal.irq = Prev_level;
	interrupt_signal.arm_signal.reset = Prev_level;

	if(dev->signal_iface)
		dev->signal_iface->signal(dev->signal, &interrupt_signal);
}
static void update_gnlr(aintc_omapl138_device *dev, int channel)
{
	if (dev->regs.aintc_gnlr & (1 << 31) == 0) {
		dev->regs.aintc_gnlr &= ~0x1ff;
		dev->regs.aintc_gnlr |= channel;
	}
}
static void update_hinlr(aintc_omapl138_device *dev, int channel, int is_irq)
{
	if (is_irq) {
		if (dev->regs.aintc_hinlr2 & (1 << 31) == 0) {
			dev->regs.aintc_hinlr2 &= ~0x1ff;
			dev->regs.aintc_hinlr2 |= channel;
		}
	} else {
		if (dev->regs.aintc_hinlr1 & (1 << 31) == 0) {
			dev->regs.aintc_hinlr1 &= ~0x1ff;
			dev->regs.aintc_hinlr1 |= channel;
		}
	}
}
static int aintc_omapl138_raise_signal(conf_object_t *opaque, int line){
	aintc_omapl138_device *dev = opaque->obj;
	uint32_t index, num, res, addr;
	int channel, is_irq;

	index = line / 32;
	num = line % 32;
	res = 1 << num;

	if (dev->regs.aintc_ger & 0x1 == 0)
		return No_exp;

	channel = get_channel_index(dev, line);

	if (channel < 0)
		return No_exp;
	if (channel >= 0 && channel <= 1) {
		/* FIQ */
		if (dev->regs.aintc_hier & (1 << 0) == 0)
			return No_exp;
		is_irq = 0;
	} else {
		/* IRQ */
		if (dev->regs.aintc_hier & (1 << 1) == 0)
			return No_exp;
		is_irq = 1;
	}

	addr = dev->regs.aintc_vbr + line * (((dev->regs.aintc_vsr & 0xff) + 1) * 4);

	if (index == 0) {
		dev->regs.aintc_srsr1 |= res;

		if (dev->aintc_sysinter1 & res) {
			dev->regs.aintc_secr1 |= res;
			update_gnlr(dev, channel);
			update_hinlr(dev, channel, is_irq);
			if (is_irq) {
				dev->regs.aintc_hipvr2 = addr;
				dev->regs.aintc_hipir2 = line;

				dev->regs.aintc_gpvr = addr;
				dev->regs.aintc_gpir = line;
				raise_irq(dev);
			} else {
				dev->regs.aintc_hipvr1 = addr;
				dev->regs.aintc_hipir1 = line;

				dev->regs.aintc_gpvr = addr;
				dev->regs.aintc_gpir = line;
				raise_fiq(dev);
			}
		}
	} else if (index == 1) {
		dev->regs.aintc_srsr2 |= res;

		if (dev->aintc_sysinter2 & res) {
			dev->regs.aintc_secr2 |= res;
			update_gnlr(dev, channel);
			update_hinlr(dev, channel, is_irq);
			if (is_irq) {
				dev->regs.aintc_hipvr2 = addr;
				dev->regs.aintc_hipir2 = line;

				dev->regs.aintc_gpvr = addr;
				dev->regs.aintc_gpir = line;
				raise_irq(dev);
			} else {
				dev->regs.aintc_hipvr1 = addr;
				dev->regs.aintc_hipir1 = line;

				dev->regs.aintc_gpvr = addr;
				dev->regs.aintc_gpir = line;
				raise_fiq(dev);
			}
		}
	} else if (index == 2) {
		dev->regs.aintc_srsr3 |= res;

		if (dev->aintc_sysinter3 & res) {
			dev->regs.aintc_secr3 |= res;
			update_gnlr(dev, channel);
			update_hinlr(dev, channel, is_irq);
			if (is_irq) {
				dev->regs.aintc_hipvr2 = addr;
				dev->regs.aintc_hipir2 = line;

				dev->regs.aintc_gpvr = addr;
				dev->regs.aintc_gpir = line;
				raise_irq(dev);
			} else {
				dev->regs.aintc_hipvr1 = addr;
				dev->regs.aintc_hipir1 = line;

				dev->regs.aintc_gpvr = addr;
				dev->regs.aintc_gpir = line;
				raise_fiq(dev);
			}
		}
	} else if (index == 3) {
		dev->regs.aintc_srsr4 |= res;

		if (dev->aintc_sysinter4 & res) {
			dev->regs.aintc_secr4 |= res;
			update_gnlr(dev, channel);
			update_hinlr(dev, channel, is_irq);
			if (is_irq) {
				dev->regs.aintc_hipvr2 = addr;
				dev->regs.aintc_hipir2 = line;

				dev->regs.aintc_gpvr = addr;
				dev->regs.aintc_gpir = line;
				raise_irq(dev);
			} else {
				dev->regs.aintc_hipvr1 = addr;
				dev->regs.aintc_hipir1 = line;

				dev->regs.aintc_gpvr = addr;
				dev->regs.aintc_gpir = line;
				raise_fiq(dev);
			}
		}
	}

	return No_exp;
}

static int aintc_omapl138_lower_signal(conf_object_t *opaque, int line)
{
	struct aintc_omapl138_device *dev = opaque->obj;
	return No_exp;
}

static exception_t aintc_omapl138_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	aintc_omapl138_device *dev = opaque->obj;
	uint32_t index, irqno, mod;

	if (offset >= AINTC_CMR && offset < AINTC_HIPIR1) {
		/* support byte access to register */
		if (count == 1) {
			char *cmrp = (char *)(dev->regs.aintc_cmr);
			uint32_t off = offset - AINTC_CMR;

			*(uint32_t *)buf = *(cmrp + off);
		} else {
			index = offset - AINTC_CMR >> 2;
			*(uint32_t *)buf = dev->regs.aintc_cmr[index];
		}

		return No_exp;
	}

	switch (offset) {
		case AINTC_REVID:
			*(uint32_t *)buf = dev->regs.aintc_revid;
			break;
		case AINTC_CR:
			*(uint32_t *)buf = dev->regs.aintc_cr;
			break;
		case AINTC_GER:
			*(uint32_t *)buf = dev->regs.aintc_ger;
			break;
		case AINTC_GNLR:
			*(uint32_t *)buf = dev->regs.aintc_gnlr & ~(1 << 31);
			break;
		case AINTC_SISR:
			*(uint32_t *)buf = 0;
			break;
		case AINTC_SICR:
			*(uint32_t *)buf = 0;
			break;
		case AINTC_EISR:
			*(uint32_t *)buf = 0;
			break;
		case AINTC_EICR:
			*(uint32_t *)buf = 0;
			break;
		case AINTC_HIEISR:
			*(uint32_t *)buf = 0;
			break;
		case AINTC_HIEICR:
			*(uint32_t *)buf = 0;
			break;
		case AINTC_VBR:
			*(uint32_t *)buf = dev->regs.aintc_vbr;
			break;
		case AINTC_VSR:
			*(uint32_t *)buf = dev->regs.aintc_vsr;
			break;
		case AINTC_VNR:
			*(uint32_t *)buf = dev->regs.aintc_vnr;
			break;
		case AINTC_SRSR1:
			*(uint32_t *)buf = dev->regs.aintc_srsr1;
			break;
		case AINTC_SRSR2:
			*(uint32_t *)buf = dev->regs.aintc_srsr2;
			break;
		case AINTC_SRSR3:
			*(uint32_t *)buf = dev->regs.aintc_srsr3;
			break;
		case AINTC_SRSR4:
			*(uint32_t *)buf = dev->regs.aintc_srsr4;
			break;
		case AINTC_SECR1:
			*(uint32_t *)buf = dev->regs.aintc_secr1;
			break;
		case AINTC_SECR2:
			*(uint32_t *)buf = dev->regs.aintc_secr2;
			break;
		case AINTC_SECR3:
			*(uint32_t *)buf = dev->regs.aintc_secr3;
			break;
		case AINTC_SECR4:
			*(uint32_t *)buf = dev->regs.aintc_secr4;
			break;
		case AINTC_GPIR:
			*(uint32_t *)buf = dev->regs.aintc_gpir;
			break;
		case AINTC_GPVR:
			*(uint32_t *)buf = dev->regs.aintc_gpvr;
			break;
		case AINTC_HIPIR1:
			*(uint32_t *)buf = dev->regs.aintc_hipir1;
			break;
		case AINTC_HIPIR2:
			*(uint32_t *)buf = dev->regs.aintc_hipir2;
			break;
		case AINTC_HINLR1:
			*(uint32_t *)buf = dev->regs.aintc_hinlr1;
			break;
		case AINTC_HINLR2:
			*(uint32_t *)buf = dev->regs.aintc_hinlr2;
			break;
		case AINTC_HIER:
			*(uint32_t *)buf = dev->regs.aintc_hier;
			break;
		case AINTC_HIPVR1:
			*(uint32_t *)buf = dev->regs.aintc_hipvr1;
			break;
		case AINTC_HIPVR2:
			*(uint32_t *)buf = dev->regs.aintc_hipvr2;
			break;
		default:
			/* support byte access to register */
			if (offset >= AINTC_ESR1 && offset <= AINTC_ESR4 && count == 1) {
				char *esrp = &dev->regs.aintc_esr1;
				uint32_t off = offset - AINTC_ESR1;

				*(uint32_t *)buf = *(esrp + off);
			} else {
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in aintc_omapl138\n", offset);
			}
			break;
	}
	return No_exp;
}

static exception_t aintc_omapl138_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	aintc_omapl138_device *dev = opaque->obj;
	uint32_t val, index, num;

	val = *(uint32_t *)buf;

	if (offset >= AINTC_CMR && offset < AINTC_HIPIR1) {
		/* support byte access to register */
		if (count == 1) {
			char *cmrp = (char *)(dev->regs.aintc_cmr);
			uint32_t off = offset - AINTC_CMR;

			*(cmrp + off) = val;
		} else {
			index = offset - AINTC_CMR >> 2;
			dev->regs.aintc_cmr[index] = val;
		}

		return No_exp;
	}
	/* support byte access to register */
	if (offset >= AINTC_ESR1 && offset <= (AINTC_ESR4 + 4) && count == 1) {
		unsigned char *esrp = &dev->regs.aintc_esr1;
		unsigned char *sysp = &dev->aintc_sysinter1;
		uint32_t off = offset - AINTC_ESR1;

		*(esrp + off) = val;
		*(sysp + off) |= val;
		return No_exp;
	} else if (offset >= AINTC_SECR1 && offset <= (AINTC_SECR4 + 4) && count == 1) {
		unsigned char *secrp = &dev->regs.aintc_secr1;
		unsigned char *srsrp = &dev->regs.aintc_srsr1;
		uint32_t off = offset - AINTC_SECR1;

		*(secrp + off) = val;
		*(srsrp + off) &= ~val;
		return No_exp;
	} else if (offset >= AINTC_ECR1 && offset <= (AINTC_ECR4 + 4) && count == 1) {
		unsigned char *ecrp = &dev->regs.aintc_ecr1;
		unsigned char *psys = &dev->aintc_sysinter1;
		uint32_t off = offset - AINTC_ECR1;

		*(ecrp + off) = val;
		*(psys + off) &= ~val;
		return No_exp;
	}

	switch (offset) {
		case AINTC_CR:
			dev->regs.aintc_cr = val;
			break;
		case AINTC_GER:
			dev->regs.aintc_ger = val;
			break;
		case AINTC_GNLR:
			dev->regs.aintc_gnlr = val;
			break;
		case AINTC_SISR:
			dev->regs.aintc_sisr = val;
			index = (val & 0x7f) / 32;
			num = (val & 0x7f) % 32;

			if (index == 0) {
				dev->regs.aintc_srsr1 |= 1 << num;
				dev->regs.aintc_secr1 |= (1 << num);
				return No_exp;
			}
			if (index == 1) {
				dev->regs.aintc_srsr2 |= 1 << num;
				dev->regs.aintc_secr2 |= (1 << num);
				return No_exp;
			}
			if (index == 2) {
				dev->regs.aintc_srsr3 |= 1 << num;
				dev->regs.aintc_secr3 |= (1 << num);
				return No_exp;
			}
			if (index == 3) {
				dev->regs.aintc_srsr4 |= 1 << num;
				dev->regs.aintc_secr4 |= (1 << num);
				return No_exp;
			}
			break;
		case AINTC_SICR:
			dev->regs.aintc_sicr = val;
			index = (val & 0x7f) / 32;
			num = (val & 0x7f) % 32;

			if (index == 0) {
				dev->regs.aintc_srsr1 &= ~(1 << num);
				dev->regs.aintc_secr1 &= ~(1 << num);
				return No_exp;
			}

			if (index == 1) {
				dev->regs.aintc_srsr2 &= ~(1 << num);
				dev->regs.aintc_secr2 &= ~(1 << num);
				return No_exp;
			}
			if (index == 2) {
				dev->regs.aintc_srsr3 &= ~(1 << num);
				dev->regs.aintc_secr3 &= ~(1 << num);
				return No_exp;
			}
			if (index == 3) {
				dev->regs.aintc_srsr4 &= ~(1 << num);
				dev->regs.aintc_secr4 &= ~(1 << num);
				return No_exp;
			}
			break;
		case AINTC_EISR:
			dev->regs.aintc_eisr = val;
			index = (val & 0x7f) / 32;
			num = (val & 0x7f) % 32;
			if (index == 0) {
				dev->aintc_sysinter1 |= (1 << num);
				return No_exp;
			}
			if (index == 1) {
				dev->aintc_sysinter2 |= (1 << num);
				return No_exp;
			}
			if (index == 2) {
				dev->aintc_sysinter3 |= (1 << num);
				return No_exp;
			}
			if (index == 3) {
				dev->aintc_sysinter4 |= (1 << num);
				return No_exp;
			}
			break;
		case AINTC_EICR:
			dev->regs.aintc_eicr = val;
			index = (val & 0x7f) / 32;
			num = (val & 0x7f) % 32;
			if (index == 0) {
				dev->aintc_sysinter1 &= ~(1 << num);
				return No_exp;
			}
			if (index == 1) {
				dev->aintc_sysinter2 &= ~(1 << num);
				return No_exp;
			}
			if (index == 2) {
				dev->aintc_sysinter3 &= ~(1 << num);
				return No_exp;
			}
			if (index == 3) {
				dev->aintc_sysinter4 &= ~(1 << num);
				return No_exp;
			}
			break;
		case AINTC_HIEISR:
			dev->regs.aintc_hieisr = val;
			if (val & 1) {
				dev->regs.aintc_hier |= 1 << 1;
			} else {
				dev->regs.aintc_hier |= 1 << 0;
			}
			break;
		case AINTC_HIEICR:
			dev->regs.aintc_hieicr = val;
			if (val & 1) {
				dev->regs.aintc_hier &= ~(1 << 1);
			} else {
				dev->regs.aintc_hier &= ~(1 << 0);
			}
			break;
		case AINTC_VBR:
			dev->regs.aintc_vbr = val;
			break;
		case AINTC_VSR:
			dev->regs.aintc_vsr = val;
			break;
		case AINTC_VNR:
			dev->regs.aintc_vnr = val;
			break;
		case AINTC_SRSR1:
			dev->regs.aintc_srsr1 |= val;
			break;
		case AINTC_SRSR2:
			dev->regs.aintc_srsr2 |= val;
			break;
		case AINTC_SRSR3:
			dev->regs.aintc_srsr3 |= val;
			break;
		case AINTC_SRSR4:
			dev->regs.aintc_srsr4 |= val;
			break;
		case AINTC_SECR1:
			dev->regs.aintc_secr1 &= ~val;
			dev->regs.aintc_srsr1 &= ~val;
			break;
		case AINTC_SECR2:
			dev->regs.aintc_secr2 &= ~val;
			dev->regs.aintc_srsr2 &= ~val;
			break;
		case AINTC_SECR3:
			dev->regs.aintc_secr3 &= ~val;
			dev->regs.aintc_srsr3 &= ~val;
			break;
		case AINTC_SECR4:
			dev->regs.aintc_secr4 &= ~val;
			dev->regs.aintc_srsr4 &= ~val;
			break;
		case AINTC_ESR1:
			dev->regs.aintc_esr1 |= val;
			dev->aintc_sysinter1 |= val;
			break;
		case AINTC_ESR2:
			dev->regs.aintc_esr2 |= val;
			dev->aintc_sysinter2 |= val;
			break;
		case AINTC_ESR3:
			dev->regs.aintc_esr3 |= val;
			dev->aintc_sysinter3 |= val;
			break;
		case AINTC_ESR4:
			dev->regs.aintc_esr4 |= val;
			dev->aintc_sysinter4 |= val;
			break;
		case AINTC_ECR1:
			dev->regs.aintc_ecr1 |= val;
			dev->aintc_sysinter1 &= ~val;
			break;
		case AINTC_ECR2:
			dev->regs.aintc_ecr2 |= val;
			dev->aintc_sysinter2 &= ~val;
			break;
		case AINTC_ECR3:
			dev->regs.aintc_ecr3 |= val;
			dev->aintc_sysinter3 &= ~val;
			break;
		case AINTC_ECR4:
			dev->regs.aintc_ecr4 |= val;
			dev->aintc_sysinter4 &= ~val;
			break;
		case AINTC_HINLR1:
			dev->regs.aintc_hinlr1 = val;
			break;
		case AINTC_HINLR2:
			dev->regs.aintc_hinlr2 = val;
			break;
		case AINTC_HIER:
			dev->regs.aintc_hier = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in aintc_omapl138\n", offset);
			break;
	}

	return No_exp;
}

static char* aintc_omapl138_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t aintc_omapl138_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	aintc_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t aintc_omapl138_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t aintc_omapl138_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers)/ 4;
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static uint32_t aintc_omapl138_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id) {
	aintc_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static exception_t aintc_omapl138_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	aintc_omapl138_device *dev = opaque->obj;
}

static conf_object_t* new_aintc_omapl138(char* obj_name){
	aintc_omapl138_device* dev = skyeye_mm_zero(sizeof(aintc_omapl138_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	memset(regs, 0, sizeof(struct registers));

	regs->aintc_revid	= 0x4e82a900;
	regs->aintc_gnlr	= 0x00000100;
	regs->aintc_hinlr1	= 0x00000100;
	regs->aintc_hinlr2	= 0x00000100;
	return dev->obj;
}
static exception_t free_aintc_omapl138(conf_object_t* opaque) {
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	aintc_omapl138_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(0);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	aintc_omapl138_device *dev = conf_obj->obj;
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	aintc_omapl138_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);
	if(dev->signal_iface == NULL) {
		if (dev->signal == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);
	}
	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	aintc_omapl138_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
void init_aintc_omapl138(){
	static skyeye_class_t class_data = {
		.class_name = "omapl138_aintc",
		.class_desc = "omapl138_aintc",
		.new_instance = new_aintc_omapl138,
		.free_instance = free_aintc_omapl138,
		.get_attr = NULL,
		.set_attr = aintc_omapl138_set_attr,
		.module_type = SKYML_ONCHIP(SC_omapl138),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static const memory_space_intf io_memory = {
		.read = aintc_omapl138_read,
		.write = aintc_omapl138_write,
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = aintc_omapl138_get_regvalue_by_id,
		.get_regname_by_id = aintc_omapl138_get_regname_by_id,
		.set_regvalue_by_id = aintc_omapl138_set_regvalue_by_id,
		.get_regnum = aintc_omapl138_get_regnum,
		.get_regid_by_name = aintc_omapl138_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const general_signal_intf signal_intf = {
		.raise_signal = aintc_omapl138_raise_signal,
		.lower_signal = aintc_omapl138_lower_signal,
	};
	SKY_register_iface(clss, GENERAL_SIGNAL_INTF_NAME, &signal_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.iface = &signal_intf,
		},
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
 	};

	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
