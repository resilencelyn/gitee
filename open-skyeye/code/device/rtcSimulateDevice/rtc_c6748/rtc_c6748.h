#ifndef __RTC_C6748_H__ 
#define __RTC_C6748_H__ 
struct registers 
{
	
	uint32_t second;
	
	uint32_t minute;
	
	uint32_t hour;
	
	uint32_t day;
	
	uint32_t month;
	
	uint32_t year;
	
	uint32_t dotw;
	
	uint32_t alarmsecond;
	
	uint32_t alarmmibute;
	
	uint32_t alarmhour;
	
	uint32_t alarmday;

	uint32_t alarmmonth;
	
	uint32_t alarmyear;
	
	uint32_t ctrl;
	
	uint32_t status;
	
	uint32_t interrupt;
	
	uint32_t complsb;
	
	uint32_t compmsb;
	
	uint32_t osc;
	
	uint32_t scratch0;
	
	uint32_t scratch1;
	
	uint32_t scratch2;
	
	uint32_t kick0r;
	
	uint32_t kick1r;
	
}rtc_reg_t;


struct c6748_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	struct registers regs;
	uint32_t rtc_clk_count;
	int interrupt_number;
	int pthread_id;
	
};
typedef struct c6748_device rtc_c6748_device;
static char* regs_name[] = 
{
	"second",
	"minute",
	"hour",
	"day",
	"month",
	"year",
	"dotw",
	"alarmsecond",
	"alarmmibute",
	"alarmhour",
	"alarmday",
	"alarmmonth",
	"alarmyear",
	"ctrl",
	"status",
	"interrupt",
	"complsb",
	"compmsb",
	"osc",
	"scratch0",
	"scratch1",
	"scratch2",
	"kick0r",
	"kick1r",
	NULL
};
#define SECOND 0x0
#define MINUTE 0x4
#define HOUR 0x8
#define DAY 0xc
#define MONTH 0x10
#define YEAR 0x14
#define DOTW 0x18
#define ALARMSECOND 0x20
#define ALARMMIBUTE 0x24
#define ALARMHOUR 0x28
#define ALARMDAY 0x2c
#define ALARMMONTH 0x30
#define ALARMYEAR 0x34
#define CTRL 0x40
#define STATUS 0x44
#define INTERRUPT 0x48
#define COMPLSB 0x4c
#define COMPMSB 0x50
#define OSC 0x54
#define SCRATCH0 0x60
#define SCRATCH1 0x64
#define SCRATCH2 0x68
#define KICK0R 0x6c
#define KICK1R 0x70

#define RTC_CTRL_RUN 0x00000001
#define RTC_INTERRUPT_TIMER 0x00000004
#endif  
