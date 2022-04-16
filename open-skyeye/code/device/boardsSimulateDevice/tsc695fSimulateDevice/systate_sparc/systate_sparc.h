/* Copyright (C) 
 * 2011 - Michael.Kang blackfin.kang@gmail.com
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
 * @file systate_sparc.h
 * @brief The definition of system controller for sparc
 * @author Michael.Kang blackfin.kang@gmail.com
 * @version 78.77
 * @date 2011-12-12
 */

#ifndef __SYSTATE_SPARC_H__
#define __SYSTATE_SPARC_H__

#include <skyeye_dev_intf.h>
#include "skyeye_system.h"

typedef struct reg_systate
{
    uint16_t rsvd1:4; //0:3
    uint16_t Alow:1; //4
    uint16_t insite:1; //5
    uint16_t rsvd2:1; //6
    uint16_t lost:1; //7
    uint16_t state:1; //8
    uint16_t rsvd3:4; //9:12
    uint16_t mode:2; //13:14
    uint16_t ABFlag:1; //15
}status_t;

typedef struct systate_reg
{
    uint32_t clr_wdg_reg;      //清看门狗寄存器
    uint32_t open_wdg_reg;     //开看门狗寄存器
    uint32_t close_wdg_reg;    //关看门狗寄存器
    uint32_t clr_wdg_count;    //清看门狗次数寄存器
    uint32_t reset_1553B;      //1553B复位寄存器
    uint32_t start_timer_reg;  //启动0.7s定时器寄存器
    uint32_t stop_timer_reg;   //停止0.7s定时器寄存器
    uint32_t set_duty;        //内部赋权寄存器
    uint32_t wdg_count;        //读取看门狗次数寄存器 /1ms脉冲是能寄存器
    uint32_t enable_reg;       //写：锁存1us使能寄存器      读：读5M计数器 低16bit
    uint32_t independ_on_reg;  //写：准禁自主开机寄存器     读: 读5M计数器 高7bit
    uint32_t cpu_status;        //读CPU状态寄存器
    uint32_t modify_date_reg;  //修改日期寄存器
    uint32_t clr_timer_reg;    //清0.7s定时器寄存器
    uint32_t normal_signal_reg;          //正常信号寄存器
    uint32_t io_reset_signal_reg;        //IO复位信号寄存器
    uint32_t eeprom_reset_enable_reg;    //EEPROM复位使能寄存器
    uint32_t eeprom_reset_unenable_reg;  //EEPROM复位禁止寄存器
    uint32_t boot_status_reg;            //BOOT及准禁自主加电状态寄存器 D0为准禁自主状态，D1为boot状态
}systate_reg_t;

static char *regs_name[] =
{
    "clr_wdg_reg",
    "open_wdg_reg",
    "close_wdg_reg",
    "clr_wdg_count",
    "reset_1553B",
    "start_timer_reg",
    "stop_timer_reg",
    "set_duty",
    "wdg_count",
    "enable_reg",
    "independ_on_reg",
    "cpu_status",
    "modify_date_reg",
    "clr_timer_reg",
    "normal_signal_reg",
    "io_reset_signal_reg",
    "eeprom_reset_enable_reg",
    "eeprom_reset_unenable_reg",
    "boot_status_reg",
    NULL
};

static uint32_t regs_offset[] =
{
    0x000,
    0x004,
    0x008,
    0x00c,
    0x010,
    0x014,
    0x018,
    0x01c,
    0x020,
    0x024,
    0x028,
    0x030,
    0x038,
    0x03c,
    0x044,
    0x048,
    0x04c,
    0x050,
    0x07c,
};

typedef struct
{
    conf_object_t* obj;
    systate_reg_t* regs;

    conf_object_t *signal;
    general_signal_intf *signal_iface;

    conf_object_t *pulse_linker;
    pulse_linker_intf *pulse_linker_iface;

    core_exec_intf *exec_iface;
    conf_object_t *sparc_obj;

    power_linker_intf *power_iface;
    conf_object_t *power_obj;

    sparc_ctrl_intf *sparc_ctrl_iface;
    conf_object_t *sparc_ctrl_obj;
    time_handle_t *handle;
    time_handle_t *signal_handle;
    char *aocs_ch;

    int int_number;
    int timer_cnt;
    int aocs_flag;
    int A_Power_flag;
    int B_Power_flag;
    int wdg_700ms_cnt;
    int wdg_4s_cnt;
    int signal_cnt;
    int wdg_cnt;
    int old_normal_signal;
    int iamonduty;
    int wdg_enable;
    int signal_flag;
    uint8_t io_data[11];

    int self_power;
    int self_duty;
    int duty_power;
    int reset_cnt;
    int duty_flag;
}sparc_systate_device;

#define IO_PULSE_NUM 2

#define DUTY_CHG   4 //自主切换
#define DUTY_POWER 6 //自主加电
#define BREAK_AWAY 7 //星箭分离
#define IAMONDUTY  8 //权状态
#define JUDGE_AOCC 15 //A机B机

#define bit_sr(a, b) ((a >> b) & 0x1)
#define BUFF_SIZE 64
#define STRING_LEN 16

#define CLR_WDG                 0x0
#define OPEN_WDG                0x4
#define CLOSE_WDG               0x8
#define CLR_WDG_COUNT           0xc
#define RESET_1553B             0x10
#define START_TIMER             0x14
#define STOP_TIMER              0x18
#define SET_DUTY                0x1c
#define WDG_COUNT               0x20
#define ENABLE_REG              0x24
#define INDEPEND_ON             0x28
#define CPU_STATUS              0x30
#define CPU_STATUS_BYTE2        0x31
#define CPU_STATUS_BYTE3        0x32
#define CPU_STATUS_BYTE4        0x33
#define MODIFY_DATE             0x38
#define CLR_TIMER               0x3c
#define NORMAL_SIGNAL           0x44
#define IO_RESET_SIGNAL         0x48
#define EEPROM_RESET_ENABLE     0x4c
#define EEPROM_RESET_UNENABLE   0x50
#define BOOT_STATUS             0x7c


#define POWER_ON  1
#define POWER_OFF 0
#endif
