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
* @file ds3231.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-05
*/

#include <stdio.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_dev_intf.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include "ds3231.h"

static exception_t ds3231_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);
static exception_t ds3231_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count);
uint32_t threshold[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


static int ds3231_set_state(conf_object_t* opaque, i2c_device_state_t state, uint8_t address)
{
	ds3231_dev *dev=  (ds3231_dev *)opaque->obj;
	dev->op_mod = state;
	dev->addr_count = 1;
	return 0;
}

static uint8_t ds3231_read_data(conf_object_t *opaque)
{
	uint8_t ch;
	ds3231_dev *dev=  (ds3231_dev *)opaque->obj;
	ds3231_read(opaque, dev->offset, &ch,  1);
	dev->offset++;
	return ch;
}

static void  ds3231_write_data(conf_object_t *opaque, uint8_t value)
{
	ds3231_dev *dev=  (ds3231_dev *)opaque->obj;
	if(dev->op_mod == -1)                                     /*i2c bus idle*/
		return;
	else{
		if(dev->addr_count == 0 && dev->op_mod == 0){    /*slave receive mod*/
			ds3231_write(opaque, dev->offset, &value,  1);
			dev->offset++;
		}else{
			(dev->addr_count)--;
			if(dev->addr_count == 0){
				dev->offset = value;
			}
		}
	}
	return;
}

static uint8_t ds3231_get_address(conf_object_t *opaque)
{
	ds3231_dev *dev=  (ds3231_dev *)opaque->obj;
	return dev->address;
}
 
static void week_update(void *dev)
{
	ds3231_dev* dsdev  = dev;
}

static void year_update(void *dev)
{
	ds3231_dev* dsdev  = dev;
	uint8_t year0, year1, year = 0, dec_year;
	year0 = (dsdev->regs.year & 0xf);
	year1 = (dsdev->regs.year >> 4) & 0xf;
	dec_year = year1 * 10 + year0;
	dec_year += 1;
	year |= (dec_year / 10) << 4;
	year |= (dec_year % 10);
	dsdev->regs.year = year;
}

static void month_update(void *dev)
{
	ds3231_dev* dsdev  = dev;
	uint8_t month0, month1, month = 0, dec_month;
	int YearUpdateFlag = 0;
        month0 = (dsdev->regs.month & 0xf);
	month1 = (dsdev->regs.month >> 4) & 0x1;
	dec_month = month1 *10 + month0;
	if(dec_month >= 12){
		YearUpdateFlag = 1;
		dec_month = 1;
	}else{
		dec_month += 1;
	}
	month |= (dec_month / 10) << 4;
	month |= (dec_month % 10);
	dsdev->regs.month = month;
	if(YearUpdateFlag == 1)
		year_update(dev);
}

static void day_update(void *dev)
{
	ds3231_dev* dsdev  = dev;
	uint8_t day0, day1, day = 0, dec_day, current_month;
	int MonthUpdateFlag = 0;
        day0 = (dsdev->regs.date & 0xf);
	day1 = (dsdev->regs.date >> 4) & 0x3;
	current_month = (dsdev->regs.month) & 0xf + ((dsdev->regs.month >> 4) & 0x3)*10;
	dec_day = day1 * 10 + day0;
	if(dec_day >=  threshold[current_month]){
		dec_day = 1;
		MonthUpdateFlag = 1;
	}else{
		dec_day += 1;
	}
	day |= (dec_day / 10) << 4;
	day |= (dec_day % 10);
	dsdev->regs.date = day;
	if(MonthUpdateFlag == 1)
		month_update(dev);
}

static void hour_update(void *dev)
{
	ds3231_dev* dsdev  = dev;
	uint8_t hour0, hour1, hour = 0, dec_hour;
	static int newday = 0;
	int DayUpdateFlag = 0, APFlag;
	APFlag = (dsdev->regs.hours >> 6) & 1;
	hour0 = (dsdev->regs.hours & 0xf);
	if(APFlag == 1)
		hour1 = (dsdev->regs.hours >> 4) & 0x1;
	else
		hour1 = (dsdev->regs.hours >> 4) & 0x3;
	dec_hour = hour1 * 10 + hour0;
	if(APFlag == 1){
		if(dec_hour >= 12){
			dec_hour = 0;
			dsdev->regs.hours |= 0x20;    //Change to PM
			newday++;
			if(newday == 2){
				DayUpdateFlag = 1;
				dsdev->regs.hours &= ~0x20;    //Change to AM
				newday = 0;
			}
		}else{
			dec_hour += 1;
		}
	}else{
		if(dec_hour >= 24){
			dec_hour = 0;
			DayUpdateFlag = 1;
		}else{
			dec_hour += 1;
		}
	}

	hour |= (dec_hour / 10) << 4;
	hour |= (dec_hour % 10);
	dsdev->regs.hours  = hour;
	if(DayUpdateFlag == 1){
		day_update(dev);
	}
}


static void min_update(void *dev)
{
	ds3231_dev* dsdev  = dev;
	uint8_t min0, min1, min = 0, dec_min;
	int HourUpdateFlag = 0;
	min0 = (dsdev->regs.minutes & 0xf);
	min1 = (dsdev->regs.minutes >> 4) & 0x7;
	dec_min = min1 * 10 + min0;
	if(dec_min >= 59){
		dec_min = 0;
		HourUpdateFlag = 1;
	}else{
		dec_min += 1;
	}
	min |= (dec_min / 10) << 4;
	min |= (dec_min % 10);
	dsdev->regs.minutes = min;
	if(HourUpdateFlag == 1)
		hour_update(dev);
}

static void sec_update(void *dev)
{
	ds3231_dev* dsdev  = dev;
	uint8_t sec0, sec1, sec = 0, dec_sec;
	int MinUpdateFlag = 0;
	sec0 = (dsdev->regs.seconds & 0xf);
	sec1 = (dsdev->regs.seconds >> 4) & 0x7;
	dec_sec = sec1 * 10 + sec0;
	if(dec_sec >= 59){
		dec_sec  = 0;
		MinUpdateFlag = 1;
	}else{
		dec_sec += 1;
	}
	sec |= (dec_sec / 10) << 4;
	sec |= dec_sec % 10;
	dsdev->regs.seconds = sec;
	if(MinUpdateFlag == 1)
		min_update(dev);
}

static void time_update(void *dev)
{
	ds3231_dev* dsdev  = dev;
	sec_update(dev);
}

void showtime(void *dev){
	ds3231_dev* dsdev  = dev;
	uint8_t l;
	uint8_t h;
	l =  dsdev->regs.seconds & 0xf;
	h =  ((dsdev->regs.seconds >> 4) & 0x7);
	int sec = h * 10 + l;
	l =  dsdev->regs.minutes & 0xf;
	h = ((dsdev->regs.minutes >> 4) & 0x7);
	int min = h * 10 + l;
	l = dsdev->regs.hours & 0xf;
	h = ((dsdev->regs.hours >> 4) & 0x3);
	int hour = h * 10 + l;
	l = dsdev->regs.date & 0xf;
	h = ((dsdev->regs.date >> 4) & 0x3);
	int day = h * 10 + l;
	l = dsdev->regs.month & 0xf;
	h = ((dsdev->regs.month >> 4) & 0x1);
	int mouth = h * 10 + l;
	l = dsdev->regs.year & 0xf;
	h =  ((dsdev->regs.year >> 4) & 0xf);
	int year = h * 10 + l;
	printf("%d年%d月%d日%d时%d分  %d妙\n", year, mouth, day, hour, min, sec);
}
void settime(uint32_t year, uint32_t mouth, uint32_t day, uint32_t hour, uint32_t min, uint32_t sec)
{


}
static void *hardware_trans(void *dev)
{
	ds3231_dev* dsdev  = dev;
	time_update(dsdev);
	//showtime(dsdev);

	return NULL;
}


static exception_t ds3231_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	ds3231_dev  *dev = opaque->obj;


	return No_exp;
}

static exception_t ds3231_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	ds3231_dev *dev = opaque->obj;
	switch(offset)
	{
		case SECONDS:          
			*(uint8 *)buf = dev->regs.seconds;
			break;
		case MINUTES:      
			*(uint8 *)buf = dev->regs.minutes;
			break;
		case HOURS:         
			*(uint8 *)buf = dev->regs.hours;
			break;
		case DAY:           
			*(uint8 *)buf = dev->regs.day;
			break;
		case DATE:          
			*(uint8 *)buf = dev->regs.date;
			break;
		case MONTH:        
			*(uint8 *)buf = dev->regs.month;
			break;
		case YEAR:          
			*(uint8 *)buf = dev->regs.year;
			break;
		case ALARM1_SECONDS:
			*(uint8 *)buf = dev->regs.alarm1_seconds;
			break;
		case ALARM1_MINUTES:
			*(uint8 *)buf = dev->regs.alarm1_minutes;
			break;
		case ALARM1_HOURS:  
			*(uint8 *)buf = dev->regs.alarm1_hours;
			break;
		case ALARM1_DATE:   
			*(uint8 *)buf = dev->regs.alarm1_day_date;
			break;
		case ALARM2_MINUTES:
			*(uint8 *)buf = dev->regs.alarm2_minutes;
			break;
		case ALARM2_HOURS:  
			*(uint8 *)buf = dev->regs.alarm2_hours;
			break;
		case ALARM2_DAY_DATE:   
			*(uint8 *)buf = dev->regs.alarm2_day_date;
			break;
		case CONTROL:       
			*(uint8 *)buf = dev->regs.control;
			break;
		case STATUS:        
			*(uint8 *)buf = dev->regs.status;
			break;
		case OFFSET:        
			*(uint8 *)buf = dev->regs.offset;
			break;
		case MSB:           
			*(uint8 *)buf = dev->regs.msb;
			break;
		case LSB:          
			*(uint8 *)buf = dev->regs.lsb;
			break;
	}       
	return  No_exp;
}

static exception_t ds3231_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	ds3231_dev *dev = opaque->obj;
	switch(offset)
	{
		case SECONDS:          
			dev->regs.seconds = *(uint8 *)buf;
			break;
		case MINUTES:      
			dev->regs.minutes = *(uint8 *)buf;
			break;
		case HOURS:         
			dev->regs.hours = *(uint8 *)buf;
			break;
		case DAY:           
			dev->regs.day = *(uint8 *)buf;
			break;
		case DATE:          
			dev->regs.date = *(uint8 *)buf;
			break;
		case MONTH:        
			dev->regs.month = *(uint8 *)buf;
			break;
		case YEAR:          
			dev->regs.year = *(uint8 *)buf;
			break;
		case ALARM1_SECONDS:
			dev->regs.alarm1_seconds = *(uint8 *)buf;
			break;
		case ALARM1_MINUTES:
			dev->regs.alarm1_minutes = *(uint8 *)buf;
			break;
		case ALARM1_HOURS:  
			dev->regs.alarm1_hours = *(uint8 *)buf;
			break;
		case ALARM1_DATE:   
			dev->regs.alarm1_day_date = *(uint8 *)buf;
			break;
		case ALARM2_MINUTES:
			dev->regs.alarm2_minutes = *(uint8 *)buf;
			break;
		case ALARM2_HOURS:  
			dev->regs.alarm2_hours = *(uint8 *)buf;
			break;
		case ALARM2_DAY_DATE:   
			dev->regs.alarm2_day_date = *(uint8 *)buf;
			break;
		case CONTROL:       
			dev->regs.control = *(uint8 *)buf;
			break;
		case STATUS:        
			dev->regs.status = *(uint8 *)buf;
			break;
		case OFFSET:        
			dev->regs.offset = *(uint8 *)buf;
			break;
		case MSB:           
			dev->regs.msb = *(uint8 *)buf;
			break;
		case LSB:          
			dev->regs.lsb = *(uint8 *)buf;
			break;
	}
	return  No_exp;
}

static conf_object_t* new_ds3231(char* obj_name)
{
	int  pthread_id;
	ds3231_dev* ds3231 = skyeye_mm_zero(sizeof(ds3231_dev));
	ds3231->obj = new_conf_object(obj_name, ds3231);

#if 0
	i2c_device_intf *i2c_dev_iface = skyeye_mm_zero(sizeof(i2c_device_intf));
	i2c_dev_iface->set_state = ds3231_set_state;
	i2c_dev_iface->write_data = ds3231_write_data;
	i2c_dev_iface->read_data = ds3231_read_data;
	i2c_dev_iface->get_address = ds3231_get_address;
	SKY_register_interface((void*)i2c_dev_iface, obj_name, I2C_DEVICE_INTF_NAME);
#endif

	ds3231->regs.month = 0x1;
	ds3231->address = 0x68;
	create_thread_scheduler(1000000, Periodic_sched, hardware_trans, ds3231, &pthread_id);
	
	return ds3231->obj;
}

static exception_t reset_ds3231(conf_object_t* opaque, const char* args)
{
	ds3231_dev *dev = opaque->obj;
	memset(&(dev->regs), 0, sizeof(dev->regs));
	return No_exp;        
}


static exception_t free_ds3231(conf_object_t* conf_obj)
{
	return No_exp;
}
static void ds3231_register_interface(conf_class_t* class){
	static const i2c_device_intf i2c_dev_iface = {
		.set_state = ds3231_set_state,
		.write_data = ds3231_write_data,
		.read_data = ds3231_read_data,
		.get_address = ds3231_get_address,
	};
	SKY_register_iface(class, I2C_DEVICE_INTF_NAME, &i2c_dev_iface);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = I2C_DEVICE_INTF_NAME,
			.iface = &i2c_dev_iface,
		}
 	};
	class_register_ifaces(class, ifaces);

	return;
}
void init_ds3231(){
	static skyeye_class_t class_data = {
		.class_name = "ds3231",
		.class_desc = "ds3231",
		.new_instance = new_ds3231,
		.reset_instance = reset_ds3231,
		.free_instance = free_ds3231,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	ds3231_register_interface(class);
}
