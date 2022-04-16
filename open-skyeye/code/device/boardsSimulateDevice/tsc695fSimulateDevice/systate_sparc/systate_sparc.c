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
 * @file systate_sparc.c
 * @brief The implementation of system controller
 * @author Michael.Kang blackfin.kang@gmail.com
 * @version 78.77
 * @date 2011-12-12
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_core.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include <skyeye_dev_bus_intf.h>
#define DEBUG
#include <skyeye_log.h>
#include "skyeye_system.h"

#include "systate_sparc.h"

int get_aocs_status(conf_object_t* opaque)
{
    sparc_systate_device *dev = opaque->obj;
    return dev->aocs_flag;
}

static exception_t power_mach(conf_object_t *opaque, uint32_t power_flag)
{
    sparc_systate_device *dev = opaque->obj;

    if (dev->exec_iface != NULL)
    {
        dev->exec_iface->power(dev->sparc_obj, power_flag);
    }

    return ;
}

static exception_t set_duty(conf_object_t *opaque, uint32_t duty)
{
    sparc_systate_device *dev = opaque->obj;

    dev->iamonduty = duty;
    return ;
}

static exception_t get_signal(conf_object_t *opaque)
{
    sparc_systate_device *dev = opaque->obj;

    return (dev->signal_flag);
}

static exception_t send_ctrl_cmd(conf_object_t *opaque, uint32_t cmd_type, uint32_t cmd)
{
    sparc_systate_device *dev = opaque->obj;
    systate_reg_t* regs = dev->regs;

    if (dev->aocs_ch[0] == 'A')
    {
        if (cmd_type == 1)
        {
            //A机看门狗使能或禁止
            dev->wdg_enable = cmd;
        } else if (cmd_type == 2)
        {
            //A机软件复位
            if (dev->exec_iface != NULL)
                dev->exec_iface->power(dev->sparc_obj, POWER_ON);
        } else
        {
            printf("In %s, line:%d, Not define cmd_type:%d!\n", __func__, __LINE__, cmd_type);
        }
    }

    if (dev->aocs_ch[0] == 'B')
    {
        if (cmd_type == 6)
        {
            //B机看门狗使能或禁止
            dev->wdg_enable = cmd;
        } else if (cmd_type == 7)
        {
            //B机软件复位
            dev->reset_cnt ++;
            if (dev->exec_iface != NULL)
                dev->exec_iface->power(dev->sparc_obj, POWER_ON);
        } else if (cmd_type == 0)
        {
            if (dev->sparc_ctrl_iface)
            {
                dev->sparc_ctrl_iface->send_power_flag(dev->sparc_ctrl_obj, cmd);
            }
        } else
        {
            printf("In %s, line:%d, Not define cmd_type:%d!\n", __func__, __LINE__, cmd_type);
        }
    }
}

static void set_aocc_sw(conf_object_t* opaque, char *buf)
{
    sparc_systate_device *dev = opaque->obj;
    systate_reg_t* regs = dev->regs;
    uint8_t In1 = buf[16];//星箭分离信号
    uint8_t In2 = buf[17];//AOCC开A机B机
    uint8_t In3 = buf[18];//AOCC开A机关B机
    uint8_t In4 = buf[19];//AOCC开B机关A机
    uint8_t In5 = buf[20];//赋AOCCA权
    uint8_t In6 = buf[21];//赋AOCCB权
    uint8_t In7 = buf[22];//准AOCC 自主切换
    uint8_t In8 = buf[23];//禁AOCC 自主切换
    uint8_t In9 = buf[24];//准AOCC 自主加电
    uint8_t In10 = buf[25];//禁AOCC 自主加电
    uint8_t In11 = buf[26];//AOCC 切换线路接通
    uint8_t In12 = buf[27];//AOCC 切换线路断开

    regs->cpu_status = 0;

    if (In2)
    {
        dev->A_Power_flag = 1;
        dev->B_Power_flag = 1;
    }

    if (In3)
    {
        dev->A_Power_flag = 1;
        dev->B_Power_flag = 0;
    }

    if (In4)
    {
        dev->A_Power_flag = 0;
        dev->B_Power_flag = 1;
    }

    if (In7)
    {
        dev->self_duty = 1;
    }else
    {
        dev->self_duty = 0;
    }

    if (In9)
    {
        dev->self_power = 1;
    } else
    {
        dev->self_power = 0;
    }

    if (In11)
    {
        dev->duty_power = 1;
    } else
    {
        dev->duty_power = 0;
    }

    if (dev->aocs_ch[0] == 'A')
    {
        if (In5 != dev->duty_flag)
        {
            if (In5 == 1)
            {
                dev->iamonduty = 1;
            } else
            {
                dev->iamonduty = 0;
            }
        }
        regs->cpu_status = (In7<<DUTY_CHG) | (In9<<DUTY_POWER) | (In1<<BREAK_AWAY) | (dev->iamonduty <<IAMONDUTY) | (1 << JUDGE_AOCC);
    } else
    {
        if (In5 != dev->duty_flag)
        {
            if (In5 == 0)
            {
                dev->iamonduty = 1;
            } else
            {
                dev->iamonduty = 0;
            }
        }
        regs->cpu_status = (In7<<DUTY_CHG) | (In9<<DUTY_POWER) | (In1<<BREAK_AWAY) | (dev->iamonduty <<IAMONDUTY) | (0 << JUDGE_AOCC);
    }

    if (dev->iamonduty)
    {
        dev->aocs_flag = 1;
    } else
    {
        dev->aocs_flag = 0;
    }

    uint8_t io_out[11] = {0};

    io_out[0] = bit_sr(regs->normal_signal_reg, 0); //A机正常信号
    io_out[1] = bit_sr(regs->normal_signal_reg, 0) ? 0 : 1;//B机正常信号
    io_out[2] = bit_sr(regs->cpu_status, 15); //A机开关状态
    io_out[3] = bit_sr(regs->cpu_status, 15) ? 0 : 1; //B机开关状态
    io_out[4] = bit_sr(regs->cpu_status, 4); //准禁自主切换状态
    io_out[5] = bit_sr(regs->cpu_status, 8); //AB机控制权状态

    if (In2 == 1 || In3 == 1)
    {
        io_out[6] = 1;//A机+10V电源
    }

    if (In2 == 1 || In4 == 1)
    {
        io_out[7] = 1;//B机+10V电源
    }

    io_out[8] = bit_sr(regs->cpu_status, 6); //准禁自主加电状态
    io_out[9] = 1;//AOCC切换线路二次电源
    io_out[10] = In11 ^ In12; //AOCC切换线路工作状态

    dev->duty_flag = In5;
    pulse_msg_t msg;

    if(dev->pulse_linker)
    {
        int i;
        msg.ucCmd = 2;
        msg.ucMode = 2;
        msg.fPeriod = -1;
        for(i = 0; i < 11; i++)
        {
            msg.ucChNo = i + 6;
            msg.fWidth = (float)io_out[i];

            if (dev->io_data[i] != io_out[i])
            {
                if (dev->aocs_flag)
                {
                    dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_PULSE_NUM, 1);
                }

                memcpy(dev->io_data, io_out, 11);
            }
        }
    }
}

static int io_linker_recv_data(conf_object_t *opaque, void* buf, int length, int io_id)
{
    sparc_systate_device *dev = opaque->obj;
    char recv_buf[BUFF_SIZE] = {0};
    memcpy(recv_buf, buf, length);
    set_aocc_sw(opaque, recv_buf);

    return length;
}

static void timer_700ms_callback(void *opaque)
{
    sparc_systate_device *dev = ((conf_object_t *)opaque)->obj;
    systate_reg_t* regs = dev->regs;
    dev->wdg_700ms_cnt ++;

    //0.7s signal wdog interrupt
    if (dev->wdg_700ms_cnt == 350)
    {
        if (dev->signal_iface)
        {
            dev->signal_iface->raise_signal(dev->signal, dev->int_number);
        }
    }
}

static void normal_signal_8s_timer(conf_object_t *opaque)
{
    sparc_systate_device *dev = opaque->obj;
    systate_reg_t* regs = dev->regs;

    if (regs->normal_signal_reg == dev->old_normal_signal)
    {
        dev->signal_cnt ++;
    } else
    {
        dev->signal_cnt = 0;
    }

    if (dev->signal_cnt == 4000)
    {
        if (dev->aocs_ch[0] == 'A' && dev->duty_power == 1)
        {
            printf("A机正常信号消失8s，B机上电!\n");
        }
    }
    if (dev->signal_cnt == 7900)
    {
        //A机正常信号消失16s
        if (dev->aocs_ch[0] == 'A')
        {
            dev->signal_flag = 1;
        }
    }

    if (dev->signal_cnt == 8000)
    {
        if (dev->aocs_ch[0] == 'B')
        {
            //B机正常信号消失16s且A机正常信号消失16s，B机断电
            int signal_flag_A = 0;
            if (dev->power_iface)
            {
                signal_flag_A = dev->power_iface->get_signal_linker(dev->power_obj, opaque);
            }

            if (signal_flag_A)
            {
                printf("A机和B机正常信号同时消失16s, B机断电!\n");
                if (dev->sparc_ctrl_iface)
                {
                    dev->sparc_ctrl_iface->send_power_flag(dev->sparc_ctrl_obj, 1);
                }
            }
        }
    }
    dev->old_normal_signal = regs->normal_signal_reg;
}

static void timer_4s_callback(conf_object_t *opaque)
{
    sparc_systate_device *dev = opaque->obj;
    systate_reg_t* regs = dev->regs;

    //4s signal wdog reset
    dev->wdg_4s_cnt ++;
    if (dev->wdg_4s_cnt == 2000)
    {
        if (dev->wdg_enable)
        {
            regs->wdg_count ++;
            if (dev->aocs_ch[0] == 'A' && dev->exec_iface != NULL)
            {
                printf("A机看门狗复位计数器:%d\n", regs->wdg_count);
                dev->exec_iface->power(dev->sparc_obj, POWER_ON);
                if (((regs->wdg_count % 3) == 0) && (dev->self_power == 1))
                {
                    printf("A机看门狗复位3次，B机上电!\n");
                }
            } else if (dev->aocs_ch[0] == 'B' && dev->exec_iface != NULL)
            {
                printf("B机看门狗复位计数器:%d\n", regs->wdg_count);
                dev->exec_iface->power(dev->sparc_obj, POWER_ON);
                if ((regs->wdg_count % 3) == 0 && dev->self_power)
                {
                    if (dev->sparc_ctrl_iface)
                    {
                        dev->sparc_ctrl_iface->send_power_flag(dev->sparc_ctrl_obj, 1);
                    }
                    printf("B机看门狗复位3次，B机断电磁阀!\n");
                }
            }
        }
    }

    if (dev->wdg_4s_cnt == (2000 * 900))
    {
        //1小时 清看门狗计数器
        regs->wdg_count = 0;
    }
}

static exception_t reset_sparc_systate(conf_object_t *obj, const char *parameter)
{
    sparc_systate_device *dev = obj->obj;
    systate_reg_t* regs = dev->regs;
    regs->clr_wdg_count = 0;
    regs->reset_1553B = 0;
    dev->self_power = 0;
    dev->self_duty = 0;
    dev->duty_power = 0;
    regs->cpu_status = 0x80;

    return No_exp;
}

static exception_t sparc_systate_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    sparc_systate_device *dev = opaque->obj;
    systate_reg_t* regs = dev->regs;

    switch(offset)
    {
        case CLR_WDG:
            *(uint32_t*)buf = regs->clr_wdg_reg;
            break;
        case OPEN_WDG:
            *(uint32_t*)buf = regs->open_wdg_reg;
            break;
        case CLOSE_WDG:
            *(uint32_t*)buf = regs->close_wdg_reg;
            break;
        case CLR_WDG_COUNT:
            *(uint32_t*)buf = regs->clr_wdg_count;
            break;
        case RESET_1553B:
            *(uint32_t*)buf = regs->reset_1553B;
            break;
        case START_TIMER:
            *(uint32_t*)buf = regs->start_timer_reg;
            break;
        case STOP_TIMER:
            *(uint32_t*)buf = regs->stop_timer_reg;
            break;
        case SET_DUTY:
            *(uint32_t*)buf = regs->set_duty;
            break;
        case WDG_COUNT:
            *(uint32_t*)buf = regs->wdg_count;
            break;
        case ENABLE_REG:
            *(uint32_t*)buf = regs->enable_reg;
            break;
        case INDEPEND_ON:
            *(uint32_t*)buf = regs->independ_on_reg;
            break;
        case CPU_STATUS:
            *(uint32_t*)buf = regs->cpu_status;
            break;
        case CPU_STATUS_BYTE2:
            *(uint32_t*)buf = regs->cpu_status >> 8;
            break;
        case CPU_STATUS_BYTE3:
            *(uint32_t*)buf = regs->cpu_status >> 16;
            break;
        case CPU_STATUS_BYTE4:
            *(uint32_t*)buf = regs->cpu_status >> 24;
            break;
        case MODIFY_DATE:
            *(uint32_t*)buf = regs->modify_date_reg;
            break;
        case CLR_TIMER:
            *(uint32_t*)buf = regs->clr_timer_reg;
            break;
        case NORMAL_SIGNAL:
            *(uint32_t*)buf = regs->normal_signal_reg;
            break;
        case IO_RESET_SIGNAL:
            *(uint32_t*)buf = regs->io_reset_signal_reg;
            break;
        case EEPROM_RESET_ENABLE:
            *(uint32_t*)buf = regs->eeprom_reset_enable_reg;
            break;
        case EEPROM_RESET_UNENABLE:
            *(uint32_t*)buf = regs->eeprom_reset_unenable_reg;
            break;
        case BOOT_STATUS:
            *(uint32_t*)buf = regs->boot_status_reg;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in systate\n", offset);
            return Invarg_exp;
    }
    return No_exp;
}

static exception_t sparc_systate_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    sparc_systate_device *dev = opaque->obj;

    systate_reg_t* regs = dev->regs;
    DBG("Jiachao ######################### In %s, offset=0x%x\n", __FUNCTION__, offset);
    uint32_t val = *(uint32_t*)buf;

    switch(offset) {
        case CLR_WDG:
            regs->clr_wdg_reg = val;
            if (val == 0x55aa)
            {
                dev->wdg_4s_cnt = 0;
            }
            break;
        case OPEN_WDG:
            regs->open_wdg_reg = val;
            break;
        case CLOSE_WDG:
            regs->close_wdg_reg = val;
            break;
        case CLR_WDG_COUNT:
            regs->clr_wdg_count = val;
            break;
        case RESET_1553B:
            regs->reset_1553B;
            reset_sparc_systate(opaque, NULL);
            break;
        case START_TIMER:
            regs->start_timer_reg = val;
            if (dev->handle == NULL)
            {
                dev->handle = system_register_timer_handler(dev->obj, 2, (time_sched_t)timer_700ms_callback, SCHED_MS | SCHED_NORMAL);
            } else
            {
                system_enable_timer(dev->handle);
            }
            break;
        case STOP_TIMER:
            regs->stop_timer_reg = val;
            if (dev->handle != NULL)
            {
                system_disable_timer(dev->handle);
            }
            break;
        case SET_DUTY:
            regs->set_duty = val;
            if (dev->aocs_ch[0] == 'B' && dev->self_duty)
            {
                dev->iamonduty = 1;
                if (dev->power_iface)
                {
                    dev->power_iface->duty_linker(dev->power_obj, opaque, 0);
                }
            }
            break;
        case WDG_COUNT:
            regs->wdg_count = val;
            break;
        case ENABLE_REG:
            regs->enable_reg = val;
            break;
        case INDEPEND_ON:
            regs->independ_on_reg = val;
            break;
        case CPU_STATUS:
            regs->cpu_status = val;
            break;
        case MODIFY_DATE:
            regs->modify_date_reg = val;
            break;
        case CLR_TIMER:
            regs->clr_timer_reg = val;
            dev->wdg_700ms_cnt = 0;
            break;
        case NORMAL_SIGNAL:
            regs->normal_signal_reg = val;
            if (dev->signal_handle == NULL)
            {
                dev->signal_handle = system_register_timer_handler(dev->obj, 2, (time_sched_t)normal_signal_8s_timer, SCHED_MS | SCHED_NORMAL);
            }
            break;
        case IO_RESET_SIGNAL:
            regs->io_reset_signal_reg = val;
            break;
        case EEPROM_RESET_ENABLE:
            regs->eeprom_reset_enable_reg = val;
            break;
        case EEPROM_RESET_UNENABLE:
            regs->eeprom_reset_unenable_reg = val;
            break;
        case BOOT_STATUS:
            regs->boot_status_reg = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in systate\n", offset);
            return Invarg_exp;
    }
    return No_exp;
}

char* sparc_systate_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
    sparc_systate_device *dev = conf_obj->obj;
    return regs_name[id];
}

uint32_t sparc_systate_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id){
    sparc_systate_device *dev = conf_obj->obj;
    uint32_t* regs_value = (uint32_t*)(dev->regs) + id;
    return *regs_value;
}

static exception_t sparc_systate_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
    sparc_systate_device *dev = conf_obj->obj;
    systate_reg_t *regs = (dev->regs);
    *((uint32_t*)regs + id) = value;

    return No_exp;
}

static uint32_t sparc_systate_get_regnum(conf_object_t* conf_obj)
{
    sparc_systate_device *dev = conf_obj->obj;
    systate_reg_t *regs = (dev->regs);
    return  sizeof(systate_reg_t) / 4;
}

static uint32_t sparc_systate_get_regoffset_by_id(conf_object_t* conf_obj, uint32_t id)
{
    return regs_offset[id];
}

static uint32_t sparc_systate_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
    sparc_systate_device *dev = conf_obj->obj;
    systate_reg_t *regs = (dev->regs);
    uint32_t regnum;
    regnum = sizeof(systate_reg_t)/ 4;

    int i;
    for(i = 0; i < regnum; i++)
    {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    sparc_systate_device *dev = conf_obj->obj;
    attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
    return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    sparc_systate_device *dev = conf_obj->obj;
    dev->int_number = SKY_attr_uinteger(*value);
    return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    sparc_systate_device *dev = obj->obj;
    dev->signal = obj2;
    dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
    return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    sparc_systate_device *dev = obj->obj;
    *obj2 = dev->signal;
    *port = NULL;
    return No_exp;
}

static exception_t pulse_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    sparc_systate_device* dev = obj->obj;
    dev->pulse_linker = obj2;
    dev->pulse_linker_iface = (pulse_linker_intf *)SKY_get_iface(obj2, PULSE_LINKER_INTF);

    if (dev->pulse_linker_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'io_bus_iface' interface from %s fail.", obj2->objname);
    }
    return No_exp;
}

static exception_t pulse_linker_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    sparc_systate_device* dev = obj->obj;
    *obj2 = dev->pulse_linker;
    *port = NULL;
    return No_exp;
}

static exception_t power_interface_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    sparc_systate_device *dev = obj->obj;
    dev->sparc_obj = obj2;
    dev->exec_iface = (core_exec_intf *)SKY_get_iface(dev->sparc_obj, CORE_EXEC_INTF_NAME);
    if (dev->exec_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_EXEC_INTF_NAME, obj2->objname);
    }
    return No_exp;
}

static exception_t power_interface_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    sparc_systate_device *dev = obj->obj;
    *obj2 = dev->sparc_obj;
    *port = NULL;
    return No_exp;
}

static exception_t power_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    sparc_systate_device *dev = obj->obj;
    dev->power_obj = obj2;
    dev->power_iface = (power_linker_intf *)SKY_get_iface(dev->power_obj, POWER_LINKER_INTF);

    if (dev->power_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", POWER_LINKER_INTF, obj2->objname);
    }
    return No_exp;
}

static exception_t power_linker_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    sparc_systate_device *dev = obj->obj;
    *obj2 = dev->power_obj;
    *port = NULL;
    return No_exp;
}

static exception_t sparc_ctrl_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    sparc_systate_device* dev = obj->obj;
    dev->sparc_ctrl_obj = obj2;
    dev->sparc_ctrl_iface =  (sparc_ctrl_intf *)SKY_get_iface(obj2, SPARC_CTRL_INTF);

    if (dev->sparc_ctrl_iface == NULL)
        skyeye_log(Error_log, __FUNCTION__, "Set sparc_ctrl_intf iface error\n");
    return No_exp;
}

static exception_t sparc_ctrl_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    sparc_systate_device* dev = obj->obj;
    *obj2 = NULL;
    *port = NULL;
    return No_exp;
}

exception_t config_sparc_systate(conf_object_t* conf_obj)
{
    sparc_systate_device *dev = conf_obj->obj;
    system_register_timer_handler(dev->obj, 2, (time_sched_t)timer_4s_callback, SCHED_MS | SCHED_NORMAL);

    return No_exp;
}

static conf_object_t* new_sparc_systate(const char* obj_name)
{
    sparc_systate_device* dev = skyeye_mm_zero(sizeof(sparc_systate_device));
    systate_reg_t* regs =  skyeye_mm_zero(sizeof(systate_reg_t));
    dev->aocs_ch = skyeye_mm_zero(STRING_LEN);
    dev->obj = new_conf_object(obj_name, dev);
    /* init systate regs */
    dev->regs = regs;
    dev->wdg_enable = 1;
    dev->self_power = 1;
    dev->self_duty = 1;
    dev->duty_power = 1;
    regs->cpu_status = 0x80;
    reset_sparc_systate(dev->obj, NULL);

    return dev->obj;
}

exception_t free_sparc_systate(conf_object_t* conf_obj)
{
    sparc_systate_device *dev = conf_obj->obj;
    skyeye_free(dev->obj);
    skyeye_free(dev->regs);
    skyeye_free(dev->aocs_ch);
    skyeye_free(dev);

    return;
}

static attr_value_t get_attr_aocs_ch(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    sparc_systate_device* dev = conf_obj->obj;
    attr_value_t aocs_ch  = SKY_make_attr_string(dev->aocs_ch);
    return aocs_ch;
}

static exception_t set_attr_aocs_ch(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    sparc_systate_device* dev = conf_obj->obj;
    dev->aocs_ch = skyeye_strdup(SKY_attr_string(*value));
    if (dev->aocs_ch[0] == 'A')
    {
        dev->iamonduty = 1;
        dev->duty_flag = 1;
    } else if (dev->aocs_ch[0] == 'B')
    {
        dev->iamonduty = 0;
        dev->duty_flag = 1;
    }
    return No_exp;
}

static void class_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "irq_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number ");
    SKY_register_attribute(clss, "aocs_ch", get_attr_aocs_ch, NULL, set_attr_aocs_ch, NULL, SKY_Attr_Optional, "string", "aocs channel");
    return;
}

void init_sparc_systate()
{
    static skyeye_class_t class_data =
    {
        .class_name = "sparc_systate",
        .class_desc = "sparc systate",
        .new_instance = new_sparc_systate,
        .free_instance = free_sparc_systate,
        .reset_instance = reset_sparc_systate,
        .config_instance = config_sparc_systate,
        .get_attr = NULL,
        .set_attr = NULL
    };

    conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);
    static const memory_space_intf io_memory =
    {
        .read = sparc_systate_read,
        .write = sparc_systate_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = sparc_systate_get_regvalue_by_id,
        .get_regname_by_id = sparc_systate_get_regname_by_id,
        .set_regvalue_by_id = sparc_systate_set_regvalue_by_id,
        .get_regnum = sparc_systate_get_regnum,
        .get_regid_by_name = sparc_systate_get_regid_by_name,
        .get_regoffset_by_id = sparc_systate_get_regoffset_by_id,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const io_dev_intf io_dev_interface =
    {
        .receive_data = io_linker_recv_data,
    };
    SKY_register_iface(clss, IO_DEV_INTF, &io_dev_interface);

    static const get_aocs_intf get_aocs_interface =
    {
        .get_aocs_status = get_aocs_status,
    };
    SKY_register_iface(clss, GET_AOCS_INTF, &get_aocs_interface);

    static const power_dev_intf power_dev_iface =
    {
        .power_mach = power_mach,
        .set_duty = set_duty,
        .get_signal = get_signal,
    };
    SKY_register_iface(clss, POWER_DEV_INTF, &power_dev_iface);

    static const sparc_ctrl_intf sparc_ctrl_iface =
    {
        .send_ctrl_cmd = send_ctrl_cmd,
    };
    SKY_register_iface(clss, SPARC_CTRL_INTF, &sparc_ctrl_iface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_intf,
        },
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory,
        },
        (struct InterfaceDescription)
        {
            .name = POWER_DEV_INTF,
            .iface = &power_dev_iface,
        },
        (struct InterfaceDescription)
        {
            .name = GET_AOCS_INTF,
            .iface = &get_aocs_interface,
        },
        (struct InterfaceDescription)
        {
            .name = SPARC_CTRL_INTF,
            .iface = &sparc_ctrl_iface,
        },
        (struct InterfaceDescription)
        {
            .name = IO_DEV_INTF,
            .iface = &io_dev_interface,
        }

    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = general_signal_set,
            .get = general_signal_get,
        },
        (struct ConnectDescription)
        {
            .name = CORE_EXEC_INTF_NAME,
            .set = power_interface_set,
            .get = power_interface_get,
        },
        (struct ConnectDescription)
        {
            .name = POWER_LINKER_INTF,
            .set = power_linker_set,
            .get = power_linker_get,
        },
        (struct ConnectDescription )
        {
            .name = SPARC_CTRL_INTF,
            .set = sparc_ctrl_set,
            .get = sparc_ctrl_get,
        },
        (struct ConnectDescription)
        {
            .name = PULSE_LINKER_INTF,
            .set = pulse_linker_set,
            .get = pulse_linker_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    class_register_attribute(clss);
}
