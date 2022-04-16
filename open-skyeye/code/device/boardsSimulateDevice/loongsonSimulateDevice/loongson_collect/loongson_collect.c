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
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_system.h>
#define DEBUG
#include "loongson_collect.h"

void update_nonbyPassData(conf_object_t *opaque)
{
	loongson_collect_device *dev = opaque->obj;

	dev->regs.valve_current_state_base = dev->nonbyPassStartVal;

	if(dev->nonbyPassStartVal < dev->nonbyPassEndVal)
		dev->nonbyPassStartVal ++;

	else
		system_disable_timer(dev->nonbyPassThread);

	return;
}

void update_byPassData(conf_object_t *opaque)
{
	loongson_collect_device *dev = opaque->obj;

	dev->regs.valve_current_state_base = dev->byPassStartVal << 13;

	if(dev->byPassStartVal < dev->byPassEndVal)
		dev->byPassStartVal ++;

	else
		system_disable_timer(dev->byPassThread);

	return;
}

void write_to_channel(loongson_collect_device *dev, uint32_t channel,uint32_t val)
{
	if(channel & 0x1)
		dev->regs.freaq_ch[4] = val;
	if(channel & 0x2)
		dev->regs.freaq_ch[5] = val;
	if(channel & 0x4)
		dev->regs.freaq_ch[0] = val;
	if(channel & 0x8)
		dev->regs.freaq_ch[1] = val;
	if(channel & 0x10)
		dev->regs.freaq_ch[2] = val;
	if(channel & 0x20)
		dev->regs.freaq_ch[3] = val;
}

void cac_h_thresholod(loongson_collect_device *dev)
{
	uint8_t ThreshlodTmpVal;
	if(dev->H_RecvThreshlod[0] > dev->H_RecvThreshlod[1])
	{
		ThreshlodTmpVal = dev->H_RecvThreshlod[0] - dev->H_RecvThreshlod[1];
	}
	else
	{
		ThreshlodTmpVal = dev->H_RecvThreshlod[1] - dev->H_RecvThreshlod[0];
	}
	dev->H_DecreThreVal = 5000 * ThreshlodTmpVal / 100; 
}

void cac_l_thresholod(loongson_collect_device *dev)
{
	uint8_t ThreshlodTmpVal;
	if(dev->L_RecvThreshlod[0] > dev->L_RecvThreshlod[1])
	{
		ThreshlodTmpVal = dev->L_RecvThreshlod[0] - dev->L_RecvThreshlod[1];
	}
	else
	{
		ThreshlodTmpVal = dev->L_RecvThreshlod[1] - dev->L_RecvThreshlod[0];
	}
	dev->L_DecreThreVal = 5000 * ThreshlodTmpVal / 100; 
}

void cac_uh_thresholod(loongson_collect_device *dev)
{
	uint8_t ThreshlodTmpVal;
	if(dev->UH_RecvThreshlod[0] > dev->UH_RecvThreshlod[1])
	{
		ThreshlodTmpVal = dev->UH_RecvThreshlod[0] - dev->UH_RecvThreshlod[1];
	}
	else
	{
		ThreshlodTmpVal = dev->UH_RecvThreshlod[1] - dev->UH_RecvThreshlod[0];
	}
	dev->UH_DecreThreVal = 5000 * ThreshlodTmpVal / 100; 
}

void cac_ul_thresholod(loongson_collect_device *dev)
{
	uint8_t ThreshlodTmpVal;
	if(dev->UL_RecvThreshlod[0] > dev->UL_RecvThreshlod[1])
	{
		ThreshlodTmpVal = dev->UL_RecvThreshlod[0] - dev->UL_RecvThreshlod[1];
	}
	else
	{
		ThreshlodTmpVal = dev->UL_RecvThreshlod[1] - dev->UL_RecvThreshlod[0];
	}
	dev->UL_DecreThreVal = 5000 * ThreshlodTmpVal / 100; 
}

int ad_receive_data(conf_object_t *ad_device, void *buf, int length)
{
	loongson_collect_device *dev = (loongson_collect_device *)ad_device->obj;

	int i;
	uint8_t TmpBuf[256];
	uint32_t RecvBuf[256];
	memcpy(TmpBuf, buf, length);

	dev->dataType = TmpBuf[0];

	if(TmpBuf[0] < TYPE_MIN)
	{
		return -1;
	}

	for(i = 0;i < length - 1;i++)
	{
		RecvBuf[i] = TmpBuf[i + 1];
	}

	switch(dev->dataType)
	{
		case HOLFREQ_GET_TYPE:
		{
			uint32_t freq = 1000000;
			uint32_t dNum = ((RecvBuf[6] << 8) | RecvBuf[7]);
			
			uint32_t cacNum = freq  / (dNum * 2);
			write_to_channel(dev, RecvBuf[1], (cacNum + ((freq % dNum) ? 1 : 0)));

			break;
		}
		case ELCTCTLR_GET_TYPE:
		{
			dev->regs.valve_current_state_base |= FILING_MID_BIT(RecvBuf[0],RecvBuf[1]);
			break;
		}
		case ELECIFACE_OUT_TYPE:
		{
			uint32_t tmp = 0;
			//支路全关 bit0,, 对应配置项中bit31
			if (RecvBuf[0] & 0x1)
				tmp |= 1 << 31;

			//旁路全关 bit2,, 对应配置项中bit30
			if (RecvBuf[0] & 0x4)
				tmp |= 1 << 30;

			//支路全开 bit1,, 对应配置项中bit29
			if (RecvBuf[0] & 0x2)
				tmp |= 1 << 29;

			//旁路全开 bit3, 对应配置项中bit28
			if (RecvBuf[0] & 0x8)
				tmp |= 1 << 28;

			//清28-31bit位
			dev->regs.valve_current_state_base &= 0xfffffff;
			dev->regs.valve_current_state_base |= tmp;
			break;
		}
		case B1553_DATA_TYPE:
		{
			/*下总线接收数据标志位
			*RecvBuf[0] : 0x88 / 0x99
			*RecvBuf[1] : 0x22 / 0x33
			*0x8822 : 写入valve_current_state_base[31 - 19]
			*0x9933 : 写入valve_current_state_base[18 - 6]
			*RecvBuf[2] : 用于计算阈值的数据*/
			if((RecvBuf[0] == 0x88) && (RecvBuf[1] == 0x22))
			{
				/*Threshlod[0] : 首次获取到的阈值
				 *Threshlod[1] : 二次获取到的阈值
				 *两者大小未知
				 */
				if(dev->H_WR_Bit == 0)
				{
					dev->H_RecvThreshlod[0] = RecvBuf[2];
					dev->H_WR_Bit ++;
				}
				else
				{
					dev->H_RecvThreshlod[1] = RecvBuf[2];
					dev->H_WR_Bit = 0;
					dev->H_Cur_WR_Data = 0;
					cac_h_thresholod(dev);
					dev->H_WR_flag = 1;
				}
			}
			
			if((RecvBuf[0] == 0x99) && (RecvBuf[1] == 0x33))
			{
				/*Threshlod[0] : 首次获取到的阈值
				 *Threshlod[1] : 二次获取到的阈值
				 *两者大小未知
				 */
				if(dev->L_WR_Bit == 0)
				{
					dev->L_RecvThreshlod[0] = RecvBuf[2];
					dev->L_WR_Bit ++;
				}
				else
				{
					dev->L_RecvThreshlod[1] = RecvBuf[2];
					dev->L_WR_Bit = 0;
					dev->L_Cur_WR_Data = 0;
					cac_l_thresholod(dev);
					dev->L_WR_flag = 1;
				}
			}
			/*上总线接收数据标志位
			*RecvBuf[0] : 0x94 / 0x94
			*RecvBuf[1] : 0xb9 / 0xa8
			*0x8822 : 写入valve_current_state_base[31 - 19]
			*0x9933 : 写入valve_current_state_base[18 - 6]
			*RecvBuf[2] : 用于计算阈值的数据*/
			if((RecvBuf[0] == 0x94) && (RecvBuf[1] == 0xb9))
			{
				/*Threshlod[0] : 首次获取到的阈值
				 *Threshlod[1] : 二次获取到的阈值
				 *两者大小未知
				 */
				if(dev->UH_WR_Bit == 0)
				{
					dev->UH_RecvThreshlod[0] = RecvBuf[2];
					dev->UH_WR_Bit ++;
				}
				else
				{
					dev->UH_RecvThreshlod[1] = RecvBuf[2];
					dev->UH_WR_Bit = 0;
					dev->UH_Cur_WR_Data = 0;
					cac_uh_thresholod(dev);
					dev->UH_WR_flag = 1;
				}
			}

			if((RecvBuf[0] == 0x94) && (RecvBuf[1] == 0xa8))
			{
				/*Threshlod[0] : 首次获取到的阈值
				 *Threshlod[1] : 二次获取到的阈值
				 *两者大小未知
				 */
				if(dev->UL_WR_Bit == 0)
				{
					dev->UL_RecvThreshlod[0] = RecvBuf[2];
					dev->UL_WR_Bit ++;
				}
				else
				{
					dev->UL_RecvThreshlod[1] = RecvBuf[2];
					dev->UL_WR_Bit = 0;
					dev->UL_Cur_WR_Data = 0;
					cac_ul_thresholod(dev);
					dev->UL_WR_flag = 1;
				}
			}
			break;
		}
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot recognize datatype : %d\n", dev->dataType);
			break;
	}
	return 0;
}

static exception_t loongson_freq_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;

	if((offset % 4) != 0)
	{
		uint8_t chanId = (offset - FREQ_BASE) / 4;
		*(uint32_t*)buf = dev->regs.freaq_ch[chanId] >> ((offset % 4) * 8);
		return No_exp;
	}

	if (offset >= FREQ_BASE && offset <= MAX_CH * 4)
	{
		*(uint32_t*)buf = dev->regs.freaq_ch[(offset - FREQ_BASE) / 4];
	} else
	{
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_collect\n", offset);
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_freq_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if((offset % 4) != 0)
	{
		uint8_t chanId = (offset - FREQ_BASE) / 4;
		dev->regs.freaq_ch[chanId] &= clear_bits(dev->regs.freaq_ch[chanId],((offset % 4) + 1) * 8 - 1 , (offset % 4) * 8 );
		dev->regs.freaq_ch[chanId] |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	if (offset >= FREQ_BASE && offset <= MAX_CH * 4)
	{
		dev->regs.freaq_ch[(offset - FREQ_BASE) / 4] = val;
	} else
	{
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_collect\n", offset);
	}

	return No_exp;
}

static exception_t loongson_freq_flag_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;

	if((offset < (READOVER_FLAG + 0x4)) && (offset > READOVER_FLAG))
	{
		*(uint32_t*)buf = dev->regs.readover_flag >> (offset * 8);
		return No_exp;
	}

	switch(offset)
	{
		case READOVER_FLAG:
			*(uint32_t*)buf = dev->regs.readover_flag;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_freq_flag_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	
	if(offset > 0x0 && offset < 0x4)
	{
		dev->regs.readover_flag &= clear_bits(dev->regs.readover_flag,(offset + 1) * 8 - 1 , offset * 8 );
		dev->regs.readover_flag |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		case READOVER_FLAG:
			dev->regs.readover_flag = val;
			break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_collect\n", offset);
		break;
	}
	return No_exp;
}

static exception_t loongson_oc_ctrl_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;

	if((offset < (OC_CTRL_BASE + 0x4)) && (offset > OC_CTRL_BASE))
	{
		*(uint32_t*)buf = dev->regs.oc_ctrl_base >> (offset * 8);
		return No_exp;
	}

	switch(offset)
	{
		case OC_CTRL_BASE:
			*(uint32_t*)buf = dev->regs.oc_ctrl_base;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_oc_ctrl_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	//
	if(offset > 0x0 && offset < 0x4)
	{
		dev->regs.oc_ctrl_base &= clear_bits(dev->regs.oc_ctrl_base,(offset + 1) * 8 - 1 , offset * 8 );
		dev->regs.oc_ctrl_base |= (val & 0xff) << (8 * offset);
		return No_exp;
	}
	
	switch(offset)
	{
		case OC_CTRL_BASE:
			dev->regs.oc_ctrl_base = val;
			/*if (val == 0x5ac7)
			{
				if(dev->flash.obj)
					dev->flash.intf->write(dev->flash.obj, 0x2, VALVE_STATE_TRUN_ON, 1);
			}
			else if (val == 0xa57e)
			{
				if (dev->flash.obj)
					dev->flash.intf->write(dev->flash.obj, 0x2, VALVE_STATE_TRUN_OFF, 1);
			}*/
			break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_collect\n", offset);
		break;
	}
	return No_exp;
}

static exception_t loongson_check_set_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;

	if((offset < (SIDE_VALVE_CHECK_ENABLE_CONFIG + 0x4)) && (offset > SIDE_VALVE_CHECK_ENABLE_CONFIG))
	{
		*(uint32_t*)buf = dev->regs.side_valve_check_enable_config >> (offset * 8);
		return No_exp;
	}
	
	if((offset < (BYPASS_VALVE_CHECK_ENABLE_CONFIG + 0x4)) && (offset > BYPASS_VALVE_CHECK_ENABLE_CONFIG))
	{
		*(uint32_t*)buf = dev->regs.bypass_valve_check_enable_config >> (offset * 8);
		return No_exp;
	}

	switch(offset)
	{
		case SIDE_VALVE_CHECK_ENABLE_CONFIG:
			*(uint32_t*)buf = dev->regs.side_valve_check_enable_config;
			break;
		case BYPASS_VALVE_CHECK_ENABLE_CONFIG:
			*(uint32_t*)buf = dev->regs.bypass_valve_check_enable_config;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_check_set_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if(offset > 0x0 && offset < 0x4)
	{
		dev->regs.side_valve_check_enable_config &= clear_bits(dev->regs.side_valve_check_enable_config,(offset + 1) * 8 - 1 , offset * 8 );
		dev->regs.side_valve_check_enable_config |= (val & 0xff) << (8 * offset);
		return No_exp;
	}
	
	if(offset > 0x4 && offset < 0x8)
	{
		dev->regs.bypass_valve_check_enable_config &= clear_bits(dev->regs.bypass_valve_check_enable_config,(offset + 1) * 8 - 1 , offset * 8 );
		dev->regs.bypass_valve_check_enable_config |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		case SIDE_VALVE_CHECK_ENABLE_CONFIG:
			dev->regs.side_valve_check_enable_config = val;
			break;
		case BYPASS_VALVE_CHECK_ENABLE_CONFIG:
			dev->regs.bypass_valve_check_enable_config = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_current_state_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;

	if((offset < (VALVE_CURRENT_STATE_BASE + 0x4)) && (offset > VALVE_CURRENT_STATE_BASE))
	{
		*(uint32_t*)buf = dev->regs.valve_current_state_base >> (offset * 8);
		return No_exp;
	}

	switch(offset)
	{
		case VALVE_CURRENT_STATE_BASE:
			*(uint32_t*)buf = dev->regs.valve_current_state_base;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_collect\n", offset);
			break;
	}

	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_current_state_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if(offset > 0x0 && offset < 0x4)
	{
		dev->regs.valve_current_state_base &= clear_bits(dev->regs.valve_current_state_base,(offset + 1) * 8 - 1 , offset * 8 );
		dev->regs.valve_current_state_base |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		case VALVE_CURRENT_STATE_BASE:
			dev->regs.valve_current_state_base = val;
			break;
		
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_set_steps_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;

		if((offset < (SIDE_VALVE_SET_STEPS + 4)) && (offset > SIDE_VALVE_SET_STEPS))
	{
		*(uint32_t*)buf = dev->regs.side_valve_set_steps >> (offset * 8);
		return No_exp;
	}

	switch(offset)
	{
		case SIDE_VALVE_SET_STEPS:
			*(uint32_t*)buf = dev->regs.side_valve_set_steps;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_set_steps_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	uint16_t targetStepVal = (val >> 16) & 0xffff;
	uint16_t curStepVal = (val >>  0) & 0xffff;

	if(offset > 0x0 && offset < 0x4)
	{
		if(targetStepVal != curStepVal)
		{
			dev->nonbyPassStartVal = (targetStepVal < curStepVal ? targetStepVal : curStepVal);
			dev->nonbyPassEndVal = (targetStepVal > curStepVal ? targetStepVal : curStepVal);
			system_enable_timer(dev->nonbyPassThread);
		}

		dev->regs.side_valve_set_steps &= clear_bits(dev->regs.side_valve_set_steps,(offset + 1) * 8 - 1 , offset * 8 );
		dev->regs.side_valve_set_steps |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		case SIDE_VALVE_SET_STEPS:
			dev->regs.side_valve_set_steps = val;

			if(targetStepVal != curStepVal)
			{
				dev->nonbyPassStartVal = (targetStepVal < curStepVal ? targetStepVal : curStepVal);
				dev->nonbyPassEndVal = (targetStepVal > curStepVal ? targetStepVal : curStepVal);
				system_enable_timer(dev->nonbyPassThread);
			}
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_bypass_set_steps_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;

	if((offset < (BYPASS_VALVE_SET_STEPS + 4)) && (offset > BYPASS_VALVE_SET_STEPS))
	{
		*(uint32_t*)buf = dev->regs.bypass_valve_set_steps >> (offset * 8);
		return No_exp;
	}

	switch(offset)
	{
		case BYPASS_VALVE_SET_STEPS:
			*(uint32_t*)buf = dev->regs.bypass_valve_set_steps;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_bypass_set_steps_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if(offset > 0x0 && offset < 0x4)
	{
		uint16_t targetStepVal = (val >> 16) & 0xffff;
		uint16_t curStepVal = (val >>  0) & 0xffff;

		if(targetStepVal != curStepVal)
		{
			dev->byPassStartVal = (targetStepVal < curStepVal ? targetStepVal : curStepVal);
			dev->byPassEndVal = (targetStepVal < curStepVal ? targetStepVal : curStepVal);
			system_enable_timer(dev->byPassThread);
		}

		dev->regs.bypass_valve_set_steps &= clear_bits(dev->regs.bypass_valve_set_steps,(offset + 1) * 8 - 1 , offset * 8 );
		dev->regs.bypass_valve_set_steps |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		case BYPASS_VALVE_SET_STEPS:
			dev->regs.bypass_valve_set_steps = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_pump_host_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;

	if((offset < (PUMP_HOST_CTRL + 4)) && (offset > PUMP_HOST_CTRL))
	{
		*(uint32_t*)buf = dev->regs.pump_host_ctrl >> (offset * 8);
		return No_exp;
	}

	switch(offset)
	{
		case PUMP_HOST_CTRL:
			*(uint32_t*)buf = dev->regs.pump_host_ctrl;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_pump_host_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	if(offset > PUMP_HOST_CTRL && offset < (PUMP_HOST_CTRL + 0x4))
	{
		dev->regs.pump_host_ctrl &= clear_bits(dev->regs.pump_host_ctrl,(offset + 1) * 8 - 1 , offset * 8 );
		dev->regs.pump_host_ctrl |= (val & 0xff) << (8 * offset);
		return No_exp;
	}

	switch(offset)
	{
		case PUMP_HOST_CTRL:
			dev->regs.pump_host_ctrl = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

static exception_t loongson_pump_backup_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;

	if((offset < 0x4) && (offset > 0))
	{
		*(uint32_t*)buf = dev->regs.pump_backup_ctrl >> (offset * 8);
		return No_exp;
	}

	switch(offset)
	{
		case PUMP_BACKUP_CTRL:
			*(uint32_t*)buf = dev->regs.pump_backup_ctrl;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t loongson_pump_backup_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;

	switch(offset)
	{
		case PUMP_BACKUP_CTRL:
			dev->regs.pump_backup_ctrl = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in loongson_collect\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* loongson_collect_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t loongson_collect_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t loongson_collect_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t loongson_collect_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t loongson_collect_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_loongson_collect(const char *obj_name)
{
	loongson_collect_device* dev = skyeye_mm_zero(sizeof(loongson_collect_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here

	dev->regs.valve_current_state_base = 0x3FFFFFF;
	// Register initialization
	struct registers *regs = &(dev->regs);

	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_loongson_collect(conf_object_t* conf_obj)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	// TODO: place the code here

	dev->nonbyPassThread = system_register_timer_handler(conf_obj, 40, (time_sched_t)update_nonbyPassData, SCHED_MS|SCHED_NORMAL);
	dev->byPassThread = system_register_timer_handler(conf_obj, 40, (time_sched_t)update_byPassData, SCHED_MS|SCHED_NORMAL);

	system_disable_timer(dev->nonbyPassThread);
	system_disable_timer(dev->byPassThread);

	No_exp;
}

// Free the device object to do some memory free
static exception_t free_loongson_collect(conf_object_t* conf_obj)
{
	loongson_collect_device *dev = (loongson_collect_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

static exception_t ad_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	loongson_collect_device* dev = (loongson_collect_device*)(obj->obj);
	dev->ad_linker = obj2;

	dev->ad_linker_iface = (ad_linker_intf*)SKY_get_iface(obj2, AD_LINKER_INTF);
	if (dev->ad_linker_iface == NULL)
	{
		if (obj2 == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Get 'ad_linker_iface' interface from NULL object fail.", obj2);
		} else
		{
			skyeye_log(Error_log, __FUNCTION__, "Get 'ad_linker_iface' interface from %s fail.", obj2->objname);
		}

		return Not_found_exp;
	}

	return No_exp;
}

static exception_t ad_linker_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
	loongson_collect_device* dev = (loongson_collect_device*)(obj->obj);

	*obj2 = dev->ad_linker;
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_loongson_collect(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "loongson_collect",
		.class_desc      = "",
		.new_instance    = new_loongson_collect,
		.free_instance   = free_loongson_collect,
		.config_instance = config_loongson_collect,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);

	// sets the register address read-write method
	static memory_space_intf io_memory_interface[] =
	{
		//0:0x52002000
		{
			.read  = loongson_freq_read,
			.write = loongson_freq_write
		},
		//1:0x52001000
		{
			.read  = loongson_freq_flag_read,
			.write = loongson_freq_flag_write
		},
		//2:0x51005000
		{
			.read  = loongson_oc_ctrl_read,
			.write = loongson_oc_ctrl_write
		},
		//3:0x57005000
		{
			.read  = loongson_check_set_read,
			.write = loongson_check_set_write
		},
		//4:0x57002000
		{
			.read  = loongson_current_state_read,
			.write = loongson_current_state_write
		},
		//5:0x57004000
		{
			.read  = loongson_set_steps_read,
			.write = loongson_set_steps_write
		},
		//6:0x57003000
		{
			.read  = loongson_bypass_set_steps_read,
			.write = loongson_bypass_set_steps_write
		},
		//7:0x55001000
		{
			.read  = loongson_pump_host_read,
			.write = loongson_pump_host_write
		},
		//8:0x56004000
		{
			.read  = loongson_pump_backup_read,
			.write = loongson_pump_backup_write
		}
	};

	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);

	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = loongson_collect_get_regvalue_by_id,
		.get_regname_by_id  = loongson_collect_get_regname_by_id,
		.set_regvalue_by_id = loongson_collect_set_regvalue_by_id,
		.get_regnum         = loongson_collect_get_regnum,
		.get_regid_by_name  = loongson_collect_get_regid_by_name
	};
	SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);

	static const ad_dev_intf ad_dev_iface =
	{
		.receive_data = ad_receive_data
	};
	SKY_register_iface(dev_class, AD_DEV_INTF, &ad_dev_iface);

	// list of interface methods
	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name  = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory_interface
		},
		(struct InterfaceDescription)
		{
			.name  = SKYEYE_REG_INTF,
			.iface = &reg_interface
		},
		(struct InterfaceDescription)
		{
			.name  = AD_DEV_INTF,
			.iface = &ad_dev_iface
		},
	};

	// list of interface connection methods
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = AD_LINKER_INTF,
			.set = ad_linker_set,
			.get = ad_linker_get,
		},
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}
