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
* @file rtc_am335x.c
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
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"

#include "rtc_am335x.h"

uint32_t threshold[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static void week_update(void *rtc_dev)
{
	am335x_rtc_dev * dev = rtc_dev;

}

static void year_update(void *rtc_dev)
{
	am335x_rtc_dev * dev = rtc_dev;
	uint32_t year0, year1, year = 0, dec_year;
	year0 = (dev->regs.rtc_year & 0xf);
	year1 = (dev->regs.rtc_year >> 4) & 0xf;
	dec_year = year1 * 10 + year0;
	dec_year += 1;
	year |= (dec_year / 10) << 4;
	year |= (dec_year % 10);
	dev->regs.rtc_year = year;
}

static void month_update(void *rtc_dev)
{
	am335x_rtc_dev * dev = rtc_dev;
	uint32_t month0, month1, month = 0, dec_month;
	int YearUpdateFlag = 0;
        month0 = (dev->regs.rtc_month & 0xf);
	month1 = (dev->regs.rtc_month >> 4) & 0x1;
	dec_month = month1 *10 + month0;
	if(dec_month >= 12){
		dec_month = 1;
	}else{
		dec_month += 1;
		YearUpdateFlag = 1;
	}
	month |= (dec_month / 10) << 4;
	month |= (dec_month % 10);
	dev->regs.rtc_month = month;
	if(YearUpdateFlag == 1)
		year_update(rtc_dev);
}

static void day_update(void *rtc_dev)
{
	am335x_rtc_dev * dev = rtc_dev;
	uint32_t day0, day1, day = 0, dec_day, current_month;
	int MonthUpdateFlag = 0;
        day0 = (dev->regs.rtc_day& 0xf);
	day1 = (dev->regs.rtc_day >> 4) & 0x3;
	current_month = (dev->regs.rtc_month) & 0xf + ((dev->regs.rtc_month >> 4) & 0x3)*10;
	dec_day = day1 * 10 + day0;
	if(dec_day >=  threshold[current_month]){
		dec_day = 1;
		MonthUpdateFlag = 1;
	}else{
		dec_day += 1;
	}
	day |= (dec_day / 10) << 4;
	day |= (dec_day % 10);
	dev->regs.rtc_day = day;
	if(MonthUpdateFlag == 1)
		month_update(rtc_dev);
}

static void hour_update(void *rtc_dev)
{
	am335x_rtc_dev * dev = rtc_dev;
	uint32_t hour0, hour1, hour = 0, dec_hour;
	static newday = 0;
	int DayUpdateFlag = 0, APFlag;
	APFlag = (dev->regs.rtc_hour >> 7) & 1;
	hour0 = (dev->regs.rtc_hour& 0xf);
	hour1 = (dev->regs.rtc_hour >> 4) & 0x3;
	dec_hour = hour1 * 10 + hour0;
	if(APFlag == 0){
		if(dec_hour >= 12){
			dec_hour = 0;
			newday++;
			if(newday == 2){
				DayUpdateFlag = 1;
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
	dev->regs.rtc_hour  = hour;
	if(DayUpdateFlag == 1){
		day_update(rtc_dev);
	}
}

static void min_update(void *rtc_dev)
{
	am335x_rtc_dev * dev = rtc_dev;
	uint32_t min0, min1, min = 0, dec_min;
	int HourUpdateFlag = 0;
	min0 = (dev->regs.rtc_minute & 0xf);
	min1 = (dev->regs.rtc_minute >> 4) & 0x7;
	dec_min = min1 * 10 + min0;
	if(dec_min >= 59){
		dec_min = 0;
		HourUpdateFlag = 1;
	}else{
		dec_min += 1;
	}
	min |= (dec_min / 10) << 4;
	min |= (dec_min % 10);
	dev->regs.rtc_minute = min;
	if(HourUpdateFlag == 1)
		hour_update(rtc_dev);
}
static void sec_update(void *rtc_dev)
{
	am335x_rtc_dev * dev = rtc_dev;
	uint32_t sec0, sec1, sec = 0, dec_sec;
	int MinUpdateFlag = 0;
	sec0 = (dev->regs.rtc_second & 0xf);
	sec1 = (dev->regs.rtc_second >> 4) & 0x7;
	dec_sec = sec1 * 10 + sec0;
	if(dec_sec >= 59){
		dec_sec  = 0;
		MinUpdateFlag = 1;
	}else{
		dec_sec += 1;
	}
	sec |= (dec_sec / 10) << 4;
	sec |= dec_sec % 10;
	dev->regs.rtc_second = sec;
	if(MinUpdateFlag == 1)
		min_update(rtc_dev);
}

static void time_update(void *rtc_dev)
{
	am335x_rtc_dev * dev = rtc_dev;
	sec_update(rtc_dev);

}
static void *hardware_trans(void *rtc_dev)
{
	am335x_rtc_dev * dev = rtc_dev;
	uint32_t hour0, hour1, hour = 0, dec_hour;
	hour0 = (dev->regs.rtc_hour& 0xf);
	hour1 = (dev->regs.rtc_hour >> 4) & 0x3;
	dec_hour = hour1 * 10 + hour0;
	if(dec_hour >= 12){
		if(((dev->regs.rtc_hour >> 8) & 0x1) == 0){
			dec_hour -= 12;
			hour |= (dec_hour / 10) << 4;
			hour |= (dec_hour % 10);
			dev->regs.rtc_hour  = hour;
		}
	}

	if(dev->regs.rtc_ctrl & RTC_CTRL_RUN ){
		time_update(rtc_dev);
		//printf("second: %x\n", dev->regs.rtc_second);
		if(dev->regs.rtc_interrupt & RTC_INTERRUPTS_TIMER)
			dev->signal->raise_signal(dev->signal_obj, dev->interrupt_number);/*触发接收中断*/
	}
	return NULL;
}
#if 0
static exception_t am335x_rtc_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	am335x_rtc_dev *dev = opaque->obj;
	if(!strncmp(attr_name, "signal", strlen("signal"))){
		dev->signal = (general_signal_intf *)SKY_get_iface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
	}else if(!strncmp(attr_name, "int_number", strlen("int_number"))){
		dev->interrupt_number = value->u.uinteger;
	}else{
		printf("parameter error\n");
		return Invarg_exp;
	}
	return No_exp;
}
#endif
static exception_t am335x_rtc_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	am335x_rtc_dev *dev = opaque->obj;
	switch(offset)
	{
		case RTC_SECOND:  
			*(uint32 *)buf = dev->regs.rtc_second; //r/w             
			break;                                                  
	        case RTC_MINUTE:                                                
			*(uint32 *)buf = dev->regs.rtc_minute; //r/w
			break;                                                  
                case RTC_HOUR:                                                  
			*(uint32 *)buf = dev->regs.rtc_hour;   //r/w
			break;                                                  
                case RTC_DAY:                                                   
			*(uint32 *)buf = dev->regs.rtc_day;    //r/w
			break;                                                  
                case RTC_MONTH:                                                 
			*(uint32 *)buf = dev->regs.rtc_month;  //r/w
			break;                                                  
                case RTC_YEAR:                                                  
			*(uint32 *)buf = dev->regs.rtc_year;   //r/w
			break;                                                  
                case RTC_WEEK:                                                  
			*(uint32 *)buf = dev->regs.rtc_week;   //r/w
			break;                                                  
                case RTC_ALARMSECOND:                                           
			*(uint32 *)buf = dev->regs.rtc_alarm_second; //r/w
			break;                                                  
                case RTC_ALARMMINUTE:                                           
			*(uint32 *)buf = dev->regs.rtc_alarm_minute; //r/w
			break;                                                  
                case RTC_ALARMHOUR:                                             
			*(uint32 *)buf = dev->regs.rtc_alarm_hour;   //r/w
			break;                                                  
                case RTC_ALARMDAY:                                              
			*(uint32 *)buf = dev->regs.rtc_alarm_day;    //r/w 
			break;                                                  
                case RTC_ALARMMONTH:                                            
			*(uint32 *)buf = dev->regs.rtc_alarm_month;  //r/w
			break;                                                  
                case RTC_ALARMYEAR:                                             
			*(uint32 *)buf = dev->regs.rtc_alarm_year;   //r/w
			break;                                                  
                case RTC_CTRL:                                                  
			*(uint32 *)buf = dev->regs.rtc_ctrl;         //r/w      
			break;                                                  
                case RTC_STATUS:                                                
			*(uint32 *)buf = dev->regs.rtc_status;        //0 ~ 5 is 
			break;                                                  
                case RTC_INTERRUPT:                                             
			*(uint32 *)buf = dev->regs.rtc_interrupt ;    //r/w      
			break;                                                  
                case RTC_COMPLSB:                                               
			*(uint32 *)buf = dev->regs.rtc_comp_lsb ;     //r/w  
			break;                                                  
                case RTC_COMPMSB:                                               
			*(uint32 *)buf = dev->regs.rtc_comp_msb ;     //r/w      
			break;                                                  
                case RTC_OSC:                                                   
			*(uint32 *)buf = dev->regs.rtc_osc;           //r/w      
			break;                                                  
                case RTC_SCRATCH0:                                              
			*(uint32 *)buf = dev->regs.rtc_scratch0;      //r/w      
			break;                                                  
                case RTC_SCRATCH1:                                              
			*(uint32 *)buf = dev->regs.rtc_scratch1 ;     //r/w      
			break;                                                  
                case RTC_SCRATCH2:                                              
			*(uint32 *)buf = dev->regs.rtc_scratch2;      //r/w      
			break;                                                  
                case RTC_KICK0:                                                 
			*(uint32 *)buf = dev->regs.rtc_kick0;         //w       
			break;                                                  
                case RTC_KICK1:                                                 
			*(uint32 *)buf = dev->regs.rtc_kick1;         //w       
			break;
                case RTC_REVISION:  
			*(uint32 *)buf = dev->regs.rtc_revision;      //r
			break;                                              
                case RTC_SYSCONFIG:                                         
			*(uint32 *)buf = dev->regs.rtc_sysconfig;     //r/w 
			break;                                              
                case RTC_IRQWAKEEN:                                         
			*(uint32 *)buf = dev->regs.rtc_irqwakeen;     //r/w 
			break;                                              
                case RTC_ALARM2_SECONDS:                                    
			*(uint32 *)buf = dev->regs.rtc_alarm2_seconds;//r/w 
			break;                                              
                case RTC_ALARM2_MINUTES:                                    
			*(uint32 *)buf = dev->regs.rtc_alarm2_minutes;//r/w 
			break;                                              
                case RTC_ALARM2_HOURS:                                      
			*(uint32 *)buf = dev->regs.rtc_alarm2_hours;  //r/w 
			break;                                              
                case RTC_ALARM2_DAYS:                                       
			*(uint32 *)buf = dev->regs.rtc_alarm2_days;   //r/w 
			break;                                              
                case RTC_ALARM2_MONTHS:                                     
			*(uint32 *)buf = dev->regs.rtc_alarm2_months; //r/w 
			break;                                              
                case RTC_ALARM2_YEARS:                                      
			*(uint32 *)buf = dev->regs.rtc_alarm2_years ; //r/w 
			break;                                              
                case RTC_PMIC:                                              
			*(uint32 *)buf = dev->regs.rtc_pmic;          //r/w 
			break;                                              
                case RTC_DEBOUNCE:                                          
			*(uint32 *)buf = dev->regs.rtc_debounce ;     //r/w 
			break;
                default:
			printf("read %s error offset %d : 0x%x\n",dev->obj->objname, offset, *(uint16*)buf);
			break;
	}       

	return  No_exp;
}

static exception_t am335x_rtc_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	am335x_rtc_dev *dev = opaque->obj;
	switch(offset)
	{
		case RTC_SECOND:  
			dev->regs.rtc_second = *(uint32 *)buf; //r/w             
			break;                                                  
	        case RTC_MINUTE:                                                
			dev->regs.rtc_minute = *(uint32 *)buf; //r/w
			break;                                                  
                case RTC_HOUR:                                                  
			dev->regs.rtc_hour = *(uint32 *)buf;   //r/w
			break;                                                  
                case RTC_DAY:                                                   
			dev->regs.rtc_day = *(uint32 *)buf;    //r/w
			break;                                                  
                case RTC_MONTH:                                                 
			dev->regs.rtc_month = *(uint32 *)buf;  //r/w
			break;                                                  
                case RTC_YEAR:                                                  
			dev->regs.rtc_year = *(uint32 *)buf;   //r/w
			break;                                                  
                case RTC_WEEK:                                                  
			dev->regs.rtc_week = *(uint32 *)buf;   //r/w
			break;                                                  
                case RTC_ALARMSECOND:                                           
			dev->regs.rtc_alarm_second = *(uint32 *)buf; //r/w
			break;                                                  
                case RTC_ALARMMINUTE:                                           
			dev->regs.rtc_alarm_minute = *(uint32 *)buf; //r/w
			break;                                                  
                case RTC_ALARMHOUR:                                             
			dev->regs.rtc_alarm_hour = *(uint32 *)buf;   //r/w
			break;                                                  
                case RTC_ALARMDAY:                                              
			dev->regs.rtc_alarm_day = *(uint32 *)buf;    //r/w 
			break;                                                  
                case RTC_ALARMMONTH:                                            
			dev->regs.rtc_alarm_month = *(uint32 *)buf;  //r/w
			break;                                                  
                case RTC_ALARMYEAR:                                             
			dev->regs.rtc_alarm_year = *(uint32 *)buf;   //r/w
			break;                                                  
                case RTC_CTRL:                                                  
			dev->regs.rtc_ctrl = *(uint32 *)buf;          //r/w      
			break;                                                  
                case RTC_STATUS:                                                
			dev->regs.rtc_status = *(uint32 *)buf;       //0 ~ 5 is 
			break;                                                  
                case RTC_INTERRUPT:                                             
			dev->regs.rtc_interrupt = *(uint32 *)buf;    //r/w      
			break;                                                  
                case RTC_COMPLSB:                                               
			dev->regs.rtc_comp_lsb = *(uint32 *)buf;      //r/w  
			break;                                                  
                case RTC_COMPMSB:                                               
			dev->regs.rtc_comp_msb = *(uint32 *)buf;      //r/w      
			break;                                                  
                case RTC_OSC:                                                   
			dev->regs.rtc_osc  = *(uint32 *)buf;          //r/w      
			break;                                                  
                case RTC_SCRATCH0:                                              
			dev->regs.rtc_scratch0  = *(uint32 *)buf;     //r/w      
			break;                                                  
                case RTC_SCRATCH1:                                              
			dev->regs.rtc_scratch1  = *(uint32 *)buf;     //r/w      
			break;                                                  
                case RTC_SCRATCH2:                                              
			dev->regs.rtc_scratch2  = *(uint32 *)buf;     //r/w      
			break;                                                  
                case RTC_KICK0:                                                 
			dev->regs.rtc_kick0 = *(uint32 *)buf;         //w       
			break;                                                  
                case RTC_KICK1:                                                 
			dev->regs.rtc_kick1 = *(uint32 *)buf;         //w       
			break;
                case RTC_REVISION:  
			dev->regs.rtc_revision  = *(uint32 *)buf;     //r
			break;                                              
                case RTC_SYSCONFIG:                                         
			dev->regs.rtc_sysconfig = *(uint32 *)buf;     //r/w 
			break;                                              
                case RTC_IRQWAKEEN:                                         
			dev->regs.rtc_irqwakeen = *(uint32 *)buf ;    //r/w 
			break;                                              
                case RTC_ALARM2_SECONDS:                                    
			dev->regs.rtc_alarm2_seconds = *(uint32 *)buf;//r/w 
			break;                                              
                case RTC_ALARM2_MINUTES:                                    
			dev->regs.rtc_alarm2_minutes = *(uint32 *)buf;//r/w 
			break;                                              
                case RTC_ALARM2_HOURS:                                      
			dev->regs.rtc_alarm2_hours = *(uint32 *)buf ; //r/w 
			break;                                              
                case RTC_ALARM2_DAYS:                                       
			dev->regs.rtc_alarm2_days = *(uint32 *)buf;   //r/w 
			break;                                              
                case RTC_ALARM2_MONTHS:                                     
			dev->regs.rtc_alarm2_months = *(uint32 *)buf; //r/w 
			break;                                              
                case RTC_ALARM2_YEARS:                                      
			dev->regs.rtc_alarm2_years = *(uint32 *)buf; //r/w 
			break;                                              
                case RTC_PMIC:                                              
			dev->regs.rtc_pmic = *(uint32 *)buf;         //r/w 
			break;                                              
                case RTC_DEBOUNCE:                                          
			dev->regs.rtc_debounce = *(uint32 *)buf;      //r/w 
			break;

		default:
			printf("write %s error offset %d : 0x%x\n",dev->obj->objname, offset, *(uint16*)buf);
			break;
	}
	return  No_exp;
}

static conf_object_t* new_am335x_rtc(char* obj_name)
{
	am335x_rtc_dev* am335x_rtc = skyeye_mm_zero(sizeof(am335x_rtc_dev));

	am335x_rtc->obj = new_conf_object(obj_name, am335x_rtc);
#if 0
	memory_space_intf* io_memory = skyeye_mm_zero(sizeof(memory_space_intf));
	io_memory->conf_obj = am335x_rtc->obj;
	io_memory->read = am335x_rtc_read;
	io_memory->write = am335x_rtc_write;

	SKY_register_interface((void*)io_memory, obj_name, MEMORY_SPACE_INTF_NAME);
#endif
	skyeye_config_t* config = get_current_config();
	if(config->cpu_rate != 0)
		am335x_rtc->rtc_clk_count  = 1000000 * (500 / ((double)config->cpu_rate));
	else
		am335x_rtc->rtc_clk_count = 1000000;

	int  pthread_id;
	create_thread_scheduler(am335x_rtc->rtc_clk_count, Periodic_sched, hardware_trans, am335x_rtc, &pthread_id);
	return am335x_rtc->obj;
}

static exception_t reset_am335x_rtc(conf_object_t* opaque, const char* args)
{
	am335x_rtc_dev *dev = opaque->obj;
	return No_exp;        
}


static exception_t free_am335x_rtc(conf_object_t* conf_obj)
{
	am335x_rtc_dev* dev = conf_obj->obj;

	return No_exp;
}

static attr_value_t get_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	am335x_rtc_dev *dev = conf_obj->obj;
	attr_value_t signal_attr = SKY_make_attr_object(dev->signal_obj);
	return signal_attr;
}
static exception_t set_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	am335x_rtc_dev *dev = conf_obj->obj;
	dev->signal_obj = SKY_attr_object(*value);
	dev->signal = (general_signal_intf *)SKY_get_iface(dev->signal_obj, GENERAL_SIGNAL_INTF_NAME);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	am335x_rtc_dev*dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	am335x_rtc_dev *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void am335x_rtc_register_attribute(conf_class_t* class){
	SKY_register_attribute(class, "signal", get_attr_signal, NULL, set_attr_signal, NULL, SKY_Attr_Optional, "object", " interrupt controler  connected");
	SKY_register_attribute(class, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of timer");

}
static void am335x_rtc_register_interface(conf_class_t* class){
	static const memory_space_intf io_memory = {
		.read = am335x_rtc_read,
		.write = am335x_rtc_write,
	};
	SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		}
 	};
	class_register_ifaces(class, ifaces);
	return;
}
void init_am335x_rtc(){
	static skyeye_class_t class_data = {
		.class_name = "am335x_rtc",
		.class_desc = "am335x_rtc",
		.new_instance = new_am335x_rtc,
		.reset_instance = reset_am335x_rtc,
		.free_instance = free_am335x_rtc,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_ONCHIP(SC_am335x SC_am3359),
	};

	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	am335x_rtc_register_interface(class);
	am335x_rtc_register_attribute(class);
}
