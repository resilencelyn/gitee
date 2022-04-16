#ifndef __MC146818_RTC_H__
#define __MC146818_RTC_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char *regs_name[] =
    {
        "RTC_PORT_MC146818",
        "RTC_SECONDS",
        "RTC_SECONDS_ALARM",
        "RTC_MINUTES",
        "RTC_MINUTES_ALARM",
        "RTC_HOURS",
        "RTC_HOURS_ALARM",
        "RTC_DAY_OF_WEEK",
        "RTC_DATE_OF_MONTH",
        "RTC_MONTH",
        "RTC_YEAR",
        "RTC_CONFIG_A",
        "RTC_CONFIG_B",
        "RTC_CONFIG_C",
        "RTC_CONFIG_D",
        "RTC_REG_SIZE",
        NULL};

// 寄存器偏移地址
#define _RTC_SECONDS       0x00
#define _RTC_SECONDS_ALARM 0x01
#define _RTC_MINUTES       0x02
#define _RTC_MINUTES_ALARM 0x03
#define _RTC_HOURS         0x04
#define _RTC_HOURS_ALARM   0x05
#define _RTC_DAY_OF_WEEK   0x06
#define _RTC_DATE_OF_MONTH 0x07
#define _RTC_MONTH         0x08
#define _RTC_YEAR          0x09
#define _RTC_CONFIG_A      0x0A
#define _RTC_CONFIG_B      0x0B
#define _RTC_CONFIG_C      0x0C
#define _RTC_CONFIG_D      0x0D

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
    {
        _RTC_SECONDS,
        _RTC_SECONDS_ALARM,
        _RTC_MINUTES,
        _RTC_MINUTES_ALARM,
        _RTC_HOURS,
        _RTC_HOURS_ALARM,
        _RTC_DAY_OF_WEEK,
        _RTC_DATE_OF_MONTH,
        _RTC_MONTH,
        _RTC_YEAR,
        _RTC_CONFIG_A,
        _RTC_CONFIG_B,
        _RTC_CONFIG_C,
        _RTC_CONFIG_D,

};

// 设备寄存器结构体
struct registers
{
    uint8_t RTC_SECONDS;
    uint8_t RTC_SECONDS_ALARM;
    uint8_t RTC_MINUTES;
    uint8_t RTC_MINUTES_ALARM;
    uint8_t RTC_HOURS;
    uint8_t RTC_HOURS_ALARM;
    uint8_t RTC_DAY_OF_WEEK;
    uint8_t RTC_DATE_OF_MONTH;
    uint8_t RTC_MONTH;
    uint8_t RTC_YEAR;
    uint8_t RTC_CONFIG_A;
    uint8_t RTC_CONFIG_B;
    uint8_t RTC_CONFIG_C;
    uint8_t RTC_CONFIG_D;
};

// 设备对象结构
struct mc146818_rtc_device_t
{
    conf_object_t *obj;
    // 接口对象和方法;
    struct registers regs;
};

typedef struct mc146818_rtc_device_t mc146818_rtc_device;

#endif
