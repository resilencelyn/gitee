#ifndef __RTC_AM335X
#define __RTC_AM335X


typedef struct RTCState
{
	conf_object_t *obj;
	struct registers
	{	
		uint32 rtc_second; //r/w
		uint32 rtc_minute; //r/w
		uint32 rtc_hour;   //r/w
		uint32 rtc_day;    //r/w
		uint32 rtc_month;  //r/w
		uint32 rtc_year;   //r/w
		uint32 rtc_week;   //r/w
		uint32 rtc_alarm_second; //r/w
		uint32 rtc_alarm_minute; //r/w
		uint32 rtc_alarm_hour;   //r/w
		uint32 rtc_alarm_day;    //r/w 
		uint32 rtc_alarm_month;  //r/w
		uint32 rtc_alarm_year;   //r/w
                uint32 rtc_ctrl;         //r/w          
                uint32 rtc_status;        //0 ~ 5 is r, 6 ~ 7 is w      
                uint32 rtc_interrupt;     //r/w       
                uint32 rtc_comp_lsb;      //r/w  
                uint32 rtc_comp_msb;     //r/w        
                uint32 rtc_osc;           //r/w       
                uint32 rtc_scratch0;  //r/w       
                uint32 rtc_scratch1;      //r/w       
                uint32 rtc_scratch2;      //r/w       
                uint32 rtc_kick0;         //w       
                uint32 rtc_kick1;         //w       
		uint32 rtc_revision;      //r
                uint32 rtc_sysconfig;     //r/w 
                uint32 rtc_irqwakeen;     //r/w 
                uint32 rtc_alarm2_seconds;//r/w 
                uint32 rtc_alarm2_minutes;//r/w 
                uint32 rtc_alarm2_hours;  //r/w 
                uint32 rtc_alarm2_days;  //r/w 
                uint32 rtc_alarm2_months; //r/w 
                uint32 rtc_alarm2_years;  //r/w 
                uint32 rtc_pmic;          //r/w 
                uint32 rtc_debounce;      //r/w 
	}regs;
	conf_object_t* signal_obj;
	general_signal_intf *signal;
	uint32_t interrupt_number;
	uint32_t rtc_clk_count;
}am335x_rtc_dev;

/*************************************************************************\
 * Registers Definition
\*************************************************************************/
#define RTC_SECOND               (0x0)
#define RTC_MINUTE               (0x4)                                                              
#define RTC_HOUR                 (0x8)
#define RTC_DAY                  (0xC)
#define RTC_MONTH                (0x10)
#define RTC_YEAR                 (0x14)
#define RTC_WEEK                 (0x18)
#define RTC_ALARMSECOND          (0x20)
#define RTC_ALARMMINUTE          (0x24)
#define RTC_ALARMHOUR            (0x28)
#define RTC_ALARMDAY             (0x2C)
#define RTC_ALARMMONTH           (0x30)
#define RTC_ALARMYEAR            (0x34)
#define RTC_CTRL                 (0x40)
#define RTC_STATUS               (0x44)
#define RTC_INTERRUPT            (0x48)
#define RTC_COMPLSB              (0x4C)
#define RTC_COMPMSB              (0x50)
#define RTC_OSC                  (0x54)
#define RTC_SCRATCH0             (0x60)
#define RTC_SCRATCH1             (0x64)
#define RTC_SCRATCH2             (0x68)
#define RTC_KICK0                (0x6C)
#define RTC_KICK1                (0x70)

/*------Definitions specific to RTC IP in AM335x -------- */
#define RTC_REVISION             (0x74)
#define RTC_SYSCONFIG            (0x78)
#define RTC_IRQWAKEEN            (0x7C)
#define RTC_ALARM2_SECONDS       (0x80)
#define RTC_ALARM2_MINUTES       (0x84)
#define RTC_ALARM2_HOURS         (0x88)
#define RTC_ALARM2_DAYS          (0x8C)
#define RTC_ALARM2_MONTHS        (0x90)
#define RTC_ALARM2_YEARS         (0x94)
#define RTC_PMIC                 (0x98)
#define RTC_DEBOUNCE             (0x9C)



#define RTC_CTRL_RUN            (0x00000001)
#define RTC_INTERRUPTS_TIMER    (0x00000004)



#endif
