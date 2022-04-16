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
#include "rtc_c6748.h"

uint32_t threshold[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 ,31};

static void week_update(void *rtc_dev)
{
	if (NULL == rtc_dev)
	{
		
	}
	rtc_c6748_device *dev = rtc_dev;
}

static void year_update(void *rtc_dev)
{
	if (NULL == rtc_dev)
	{
		
	}
	rtc_c6748_device *dev = rtc_dev;
	uint32_t year0, year1, year = 0, dec_year;
	year0 = (dev->regs.year & 0xf);
	year1 = (dev->regs.year >>4 ) & 0xf;
	dec_year = year1 * 10 + year0;
	dec_year += 1;
	year |= (dec_year / 10) << 4;
	year |= (dec_year % 10);
	dev->regs.year = year;
}

static void month_update(void *rtc_dev)
{
	if (NULL == rtc_dev)
	{
		
	}
	rtc_c6748_device *dev = rtc_dev;
	uint32_t month0, month1, month = 0, dec_month;
	int YearUpdateFlag = 0;
	month0 = (dev->regs.month & 0xf);
	month1 = (dev->regs.month >> 4) & 0x1;
	dec_month = month1 * 10 + month0;
	if(dec_month >= 12)
	{
		dec_month =1;
	}
	else
	{
		dec_month += 1;
		YearUpdateFlag =1;
	}
		month |= (dec_month / 10) << 4;
		month |= (dec_month % 10);
		dev->regs.month = month;
		if(1 == YearUpdateFlag)
			year_update(rtc_dev);
}

static void day_update(void *rtc_dev)
{
	if (NULL == rtc_dev)
	{
		
	}
	rtc_c6748_device *dev = rtc_dev;
	uint32_t day0, day1, day = 0, dec_day, current_month;
	int MonthUpdateFlag = 0;
	day0 = (dev->regs.day & 0xf);
	day1 = (dev->regs.day >> 4) & 0x3;
	current_month = (dev->regs.month) & 0xf + ((dev->regs.month >> 4) & 0x3) * 10;
	dec_day = day1 * 10 + day0;
	if(dec_day >= threshold[current_month])
	{
		dec_day = 1;
		MonthUpdateFlag = 1;
	}
	else
	{
		dec_day += 1;
	}
	day |= (dec_day / 10) << 4;
	day |= (dec_day % 10);
	dev->regs.day = day;
	if(1 == MonthUpdateFlag)
		month_update(rtc_dev);
}

static void hour_update(void *rtc_dev)
{
	if (NULL == rtc_dev)
	{
		
	}
	rtc_c6748_device *dev = rtc_dev;
	uint32_t hour0, hour1, hour = 0, dec_hour;
	static newday = 0;
	int DayUpdateFlag = 0, APFlag;
	APFlag = (dev->regs.hour >> 7) & 0x1;
	hour0 = (dev->regs.hour & 0xf);
	hour1 = (dev->regs.hour >> 4) & 0x3;
	dec_hour = hour1 * 10 + hour0;
	if(0 == APFlag)
	{
		if(dec_hour >= 12)
		{
			dec_hour = 0;
			newday++;
			if(2 == newday)
			{
				DayUpdateFlag = 1;
				newday = 0;
			}
		}
		else
		{
			dec_hour += 1;
		}
	}
	else
	{
		if(dec_hour >= 24)
		{
			dec_hour = 0;
			DayUpdateFlag = 1;
		}
		else
		{
			dec_hour += 1;
		}
	}
	hour |= (dec_hour /10) << 4;
	hour |= (dec_hour %10);
	dev->regs.hour = hour;
	if(1 == DayUpdateFlag)
		day_update(rtc_dev);
}

static void min_update(void *rtc_dev)
{
	if (NULL == rtc_dev)
	{
		
	}
	rtc_c6748_device *dev = rtc_dev;
	uint32_t min0, min1, min = 0, dec_min;
	int HourUpdateFlag = 0;
	min0 = (dev->regs.minute & 0xf);
	min1 = (dev->regs.minute >>4) & 0x7;
	dec_min = min1 * 10 + min0;
	if(dec_min >= 59)
	{
		dec_min = 0;
		HourUpdateFlag = 1;
	}
	else
	{
		dec_min += 1; 
	}
	min |= (dec_min / 10) << 4;
	min |= (dec_min % 10);
	dev->regs.minute = min;
	if(1 == HourUpdateFlag)
		hour_update(rtc_dev);
}

static void sec_update(void *rtc_dev)
{
	if (NULL == rtc_dev)
	{
		
	}
	rtc_c6748_device *dev = rtc_dev;
	uint32_t sec0, sec1, sec = 0, dec_sec;
	int MinUpdateFlag = 0;
	sec0 = (dev->regs.second & 0xf);
	sec1 = (dev->regs.second >> 4) & 0x7;
	dec_sec = sec1 * 10 +sec0;
	if(dec_sec >= 59)
	{
		dec_sec = 0;
		MinUpdateFlag = 1;
	}
else
	{
		dec_sec += 1;
	}
	sec |= (dec_sec / 10) << 4;
	sec |= dec_sec % 10;
	dev->regs.second = sec;
	if(1 == MinUpdateFlag)
		min_update(rtc_dev);
}

static void time_update(void *rtc_dev)
{
	rtc_c6748_device *dev = rtc_dev;
	sec_update(rtc_dev);
}

static void signal_raise(conf_object_t *obj)
{
	rtc_c6748_device *dev = obj->obj;
	uint32_t hour0, hour1, hour = 0, dec_hour;
	hour0 = (dev->regs.hour & 0xf);
	hour1 = (dev->regs.hour >> 4) & 0x3;
	dec_hour = hour1 * 10 + hour0;
	if(dec_hour >= 12)
	{
		if(0 == ((dev->regs.hour >> 8) & 0x1))
		{
			dec_hour -= 12;
			hour |= (dec_hour / 10) << 4;
			hour |= (dec_hour % 10);
			dev->regs.hour = hour;
		}
	}
	/*触发中断处理*/
	if(dev->regs.ctrl & RTC_CTRL_RUN)
	{
		time_update(obj);
		if(dev->regs.interrupt & RTC_INTERRUPT_TIMER)
		{
			dev->signal_iface->signal(dev->signal, dev->interrupt_number);

		}
	}

	return ;
}

static exception_t rtc_c6748_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rtc_c6748_device *dev = opaque->obj;

	switch (offset)
	{
		case SECOND:
			*(uint32_t *)buf = dev->regs.second;
			break;
		case MINUTE:
			*(uint32_t *)buf = dev->regs.minute;
			break;
		case HOUR:
			*(uint32_t *)buf = dev->regs.hour;
			break;
		case DAY:
			*(uint32_t *)buf = dev->regs.day;
			break;
		case MONTH:
			*(uint32_t *)buf = dev->regs.month;
			break;
		case YEAR:
			*(uint32_t *)buf = dev->regs.year;
			break;
		case DOTW:
			*(uint32_t *)buf = dev->regs.dotw;
			break;
		case ALARMSECOND:
			*(uint32_t *)buf = dev->regs.alarmsecond;
			break;
		case ALARMMIBUTE:
			*(uint32_t *)buf = dev->regs.alarmmibute;
			break;
		case ALARMHOUR:
			*(uint32_t *)buf = dev->regs.alarmhour;
			break;
		case ALARMDAY:
			*(uint32_t *)buf = dev->regs.alarmday;
			break;
		case ALARMMONTH:
			*(uint32_t *)buf = dev->regs.alarmmonth;
			break;
		case ALARMYEAR:
			*(uint32_t *)buf = dev->regs.alarmyear;
			break;
		case CTRL:
			*(uint32_t *)buf = dev->regs.ctrl;
			break;
		case STATUS:
			*(uint32_t *)buf = dev->regs.status;
			break;
		case INTERRUPT:
			*(uint32_t *)buf = dev->regs.interrupt;
			break;
		case COMPLSB:
			*(uint32_t *)buf = dev->regs.complsb;
			break;
		case COMPMSB:
			*(uint32_t *)buf = dev->regs.compmsb;
			break;
		case OSC:
			*(uint32_t *)buf = dev->regs.osc;
			break;
		case SCRATCH0:
			*(uint32_t *)buf = dev->regs.scratch0;
			break;
		case SCRATCH1:
			*(uint32_t *)buf = dev->regs.scratch1;
			break;
		case SCRATCH2:
			*(uint32_t *)buf = dev->regs.scratch2;
			break;
		case KICK0R:
			*(uint32_t *)buf = dev->regs.kick0r;
			break;
		case KICK1R:
			*(uint32_t *)buf = dev->regs.kick1r;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rtc_c6748\n", offset);
			break; 
	}

	return 0;
}
static exception_t rtc_c6748_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rtc_c6748_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	switch (offset)
	{
		case SECOND:
			dev->regs.second = val;
			break;
		case MINUTE:
			dev->regs.minute = val;
			break;
		case HOUR:
			dev->regs.hour = val;
			break;
		case DAY:
			dev->regs.day = val;
			break;
		case MONTH:
			dev->regs.month = val;
			break;
		case YEAR:
			dev->regs.year = val;
			break;
		case DOTW:
			dev->regs.dotw = val;
			break;
		case ALARMSECOND:
			dev->regs.alarmsecond = val;
			break;
		case ALARMMIBUTE:
			dev->regs.alarmmibute = val;
			break;
		case ALARMHOUR:
			dev->regs.alarmhour = val;
			break;
		case ALARMDAY:
			dev->regs.alarmday = val;
			break;
		case ALARMMONTH:
			dev->regs.alarmmonth = val;
			break;
		case ALARMYEAR:
			dev->regs.alarmyear = val;
			break;
		case CTRL:
			dev->regs.ctrl = val;
			break;
		case STATUS:
			dev->regs.status = val;
			break;
		case INTERRUPT:
			dev->regs.interrupt = val;
			break;
		case COMPLSB:
			dev->regs.complsb = val;
			break;
		case COMPMSB:
			dev->regs.compmsb = val;
			break;
		case OSC:
			dev->regs.osc = val;
			break;
		case SCRATCH0:
			dev->regs.scratch0 = val;
			break;
		case SCRATCH1:
			dev->regs.scratch1 = val;
			break;
		case SCRATCH2:
			dev->regs.scratch2 = val;
			break;
		case KICK0R:
			dev->regs.kick0r = val;
			break;
		case KICK1R:
			dev->regs.kick1r = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rtc_c6748\n", offset);
			break;
	}
	return 0;
}

static char* rtc_c6748_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{

	return regs_name[id];

}
static exception_t rtc_c6748_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rtc_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;

	return No_exp;

}

static uint32_t rtc_c6748_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	
	return  sizeof(struct registers) / 4;

}

static uint32_t rtc_c6748_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rtc_c6748_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;

	return *regs_value;

}

static uint32_t rtc_c6748_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) 
	{
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}

	return regnum;

}

static conf_object_t* new_rtc_c6748(char *obj_name)
{
	rtc_c6748_device* dev = skyeye_mm_zero(sizeof(rtc_c6748_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	dev->rtc_clk_count = 1000000;

	dev->pthread_id = -1;
	create_thread_scheduler(dev->rtc_clk_count, Periodic_sched, signal_raise, dev->obj, &dev->pthread_id);

	return dev->obj;
	
}

static exception_t free_rtc_c6748(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
	
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	rtc_c6748_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);

	return int_number;

}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	rtc_c6748_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);

	return No_exp;

}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rtc_c6748_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;

	return No_exp;

}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rtc_c6748_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf **)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}

	return No_exp;

}

static void class_register_attribute(conf_class_t* clss)
{
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

	return;

}	

void init_rtc_c6748(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6748_rtc",
		.class_desc = "c6748_rtc",
		.new_instance = new_rtc_c6748,
		.free_instance = free_rtc_c6748,
		.module_type = SKYML_ONCHIP(SC_c6748),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = rtc_c6748_read,
		.write = rtc_c6748_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rtc_c6748_get_regvalue_by_id,
		.get_regname_by_id = rtc_c6748_get_regname_by_id,
		.set_regvalue_by_id = rtc_c6748_set_regvalue_by_id,
		.get_regnum = rtc_c6748_get_regnum,
		.get_regid_by_name = rtc_c6748_get_regid_by_name
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
	};
	static const struct ConnectDescription connects[] = 
	{
		(struct ConnectDescription)
		{
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		}
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}
