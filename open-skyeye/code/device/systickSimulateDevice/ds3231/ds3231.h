#ifndef __DS3231_H
#define __DS3231_H

#define         SECONDS              0x0
#define         MINUTES              0x1
#define         HOURS                0x2
#define         DAY                  0x3
#define         DATE                 0x4
#define         MONTH                0x5
#define         YEAR                 0x6
#define         ALARM1_SECONDS       0x7
#define         ALARM1_MINUTES       0x8
#define         ALARM1_HOURS         0x9
#define         ALARM1_DATE          0xa
#define         ALARM2_MINUTES       0xb
#define         ALARM2_HOURS         0xc
#define         ALARM2_DAY_DATE      0xd
#define         CONTROL              0xe
#define         STATUS               0xf
#define         OFFSET               0x10
#define         MSB                  0x11
#define         LSB                  0x12



typedef struct RTCState
{
	conf_object_t *obj;
	struct registers
	{	
		uint8_t seconds;
		uint8_t minutes;
		uint8_t hours;
		uint8_t day;
		uint8_t date;
		uint8_t month;
		uint8_t year;
		uint8_t alarm1_seconds;
		uint8_t alarm1_minutes;
		uint8_t alarm1_hours;
		uint8_t alarm1_day_date;
		uint8_t alarm2_minutes;
		uint8_t alarm2_hours;
		uint8_t alarm2_day_date;
		uint8_t control;
		uint8_t status;
		uint8_t offset;
		uint8_t msb;
		uint8_t lsb;
	}regs;
	uint32_t offset;                                 /*location of the last operation */
	uint8_t address;                                 /*i2c slave dev address*/
	uint32_t addr_count;
	int32_t op_mod;
}ds3231_dev;
#endif
