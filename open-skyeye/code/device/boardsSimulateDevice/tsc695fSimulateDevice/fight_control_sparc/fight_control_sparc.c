/* Copyright (C) 
 * 
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
 * @file fight_control_sparc.c
 * @brief The implementation of fight control
 * @author 
 * @version 78.77
 */

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_data_intf.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h> 
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_uart_ops.h>
#include <skyeye_dev_bus_intf.h>
#define DEBUG
#include <skyeye_log.h>

#include "fight_control_sparc.h"
#include "skyeye_system.h"

static int pulse_linker_recv_pulse(conf_object_t *opaque, pulse_msg_t* msg, int io_num, int io_id)
{
    fight_control_sparc_device *dev = opaque->obj;
    return 0;
}

void AttOc_output_data_to_file(fight_control_sparc_device* dev, int channel, int value)
{
    char tmp[32] = {0};

    if (dev->ad_intf)
    {
        sprintf(tmp, "%s", "AttOC Channel :  ");
        dev->ad_intf->write(dev->intf_obj, tmp, strlen(tmp));
        sprintf(tmp, "%d >> Command Width :  ", channel);
        dev->ad_intf->write(dev->intf_obj, tmp, strlen(tmp));
        sprintf(tmp, "%d", value);
        dev->ad_intf->write(dev->intf_obj, tmp, strlen(tmp));
        sprintf(tmp, "\n", strlen(tmp));
        dev->ad_intf->write(dev->intf_obj, tmp, strlen(tmp));
    }
}

void read_obt_width(conf_object_t* opaque)
{
    struct fight_control_sparc_device *dev = opaque->obj;
    fight_control_sparc_reg_t* regs = dev->regs;
    int i;

    for (i = 0; i < OBT_LENGTH;i++)
    {
        if (dev->orbit_width[i].u32 != 0)
        {
            dev->orbit_width[i].u32 -= 1;
        }
    }

    return;
}

void ObtOc_send_pulse(conf_object_t* opaque, uint32_t *buf)
{
    struct fight_control_sparc_device *dev = opaque->obj;
    fight_control_sparc_reg_t* regs = dev->regs;
    pulse_msg_t msg;
    int i, aocs_status = 0;
    if (dev->aocs_iface)
        aocs_status = dev->aocs_iface->get_aocs_status(dev->aocs);

    for (i = 0; i < OBT_LENGTH; i++)
    {
        if(dev->pulse_linker && buf[i] != 0)
        {
            msg.ucCmd = 2;
            msg.ucMode = 0;
            msg.ucChNo = i;
            msg.fPeriod = -1;
            msg.fWidth = (float)buf[i]/1000.0;

            if (dev->pulse_linker_iface == NULL)
            {
                skyeye_log(Error_log, __FUNCTION__, "Can't get 'pulse_linker_iface' interface at Line:%d", __LINE__);
            } else
            {
                if (dev->aocs_iface)
                {
                    if (aocs_status)
                        dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_COMMAND_NUM, IO_4003);
                } else
                {
                    dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_COMMAND_NUM, IO_4003);
                }
            }
        }
    }

    if (dev->timer_handler == -1)
    {
        dev->timer_handler = system_register_timer_handler(opaque, 1000, (time_sched_t)read_obt_width, SCHED_US | SCHED_NORMAL);
    } else
    {
        system_enable_timer(dev->timer_handler);
    }

}

void ObtOc_output_data_to_file(conf_object_t* opaque, int *buf)
{
    struct fight_control_sparc_device *dev = opaque->obj;
    fight_control_sparc_reg_t* regs = dev->regs;
    char tmp[32] = {0};
    int i = 0;

    if (dev->ad_intf)
    {
        sprintf(tmp, "%s", "ObtOC Channel :  ");
        dev->ad_intf->write(dev->intf_obj, tmp, strlen(tmp));
        sprintf(tmp, "%d >> OC Command Width :  ", i);
        dev->ad_intf->write(dev->intf_obj, tmp, strlen(tmp));
        sprintf(tmp, "%d", buf[i]);
        dev->ad_intf->write(dev->intf_obj, tmp, strlen(tmp));
        sprintf(tmp, "\n", strlen(tmp));
        dev->ad_intf->write(dev->intf_obj, tmp, strlen(tmp));
    }
}

static void send_pulse_function(void *opaque)
{
    struct fight_control_sparc_device *dev = ((conf_object_t *)opaque)->obj;
    fight_control_sparc_reg_t* regs = dev->regs;

    dev->delay_time_cnt -= 1;
    if (dev->delay_time_cnt == 0)
    {
        //start send pulse
        ObtOc_send_pulse(opaque, (uint32_t *)dev->orbit_width);
    }
}

static exception_t fight_control_sparc_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct fight_control_sparc_device *dev = opaque->obj;
    fight_control_sparc_reg_t* regs = dev->regs;
    int rmd, div;

    if ((offset >= SLOCK_COMMAND_0) && (offset < ATTITUDE_COMMAND_0))
    {
        rmd = offset % 0xc;
        div = offset / 0xc;

        if (rmd == 0)
        {
            *(uint32_t *)buf = regs->slock_command_X[div];
            return No_exp;
        }

        if (rmd == 4)
        {
            *(uint32_t *)buf = regs->slock_command_T[div];
            return No_exp;
        }

        if (rmd == 8)
        {
            *(uint32_t *)buf = regs->slock_command_A[div];
            return No_exp;
        }
    }

    if ((offset >= ATTITUDE_COMMAND_0) && (offset < ORBIT_COMMAND_0))
    {
        rmd = (offset - ATTITUDE_COMMAND_0) % 0xc;
        div = (offset - ATTITUDE_COMMAND_0) / 0xc;

        if (rmd == 0)
        {
            *(uint32_t *)buf = regs->attitude_command_X[div];
            return No_exp;
        }

        if (rmd == 4)
        {
            *(uint32_t *)buf = regs->attitude_command_T[div];
            return No_exp;
        }

        if (rmd == 8)
        {
            *(uint32_t *)buf = regs->attitude_command_A[div];
            return No_exp;
        }
    }

    if ((offset >= ORBIT_COMMAND_0) && (offset < ORBIT_COMMAND_MODE))
    {
        rmd = (offset - ORBIT_COMMAND_0) % 0x10;
        div = (offset - ORBIT_COMMAND_0) / 0x10;

        if (rmd == 0)
        {
            //*(uint32_t *)buf = regs->orbit_command_X[div];
            *(uint16_t *)buf = dev->orbit_width[div].u16.lsb;
            return No_exp;
        }
        if (rmd == 4)
        {
            //*(uint32_t *)buf = regs->orbit_command_F[div];
            *(uint16_t *)buf = dev->orbit_width[div].u16.msb;
            return No_exp;
        }
        if (rmd == 8)
        {
            *(uint32_t *)buf = regs->orbit_command_T[div];
            return No_exp;
        }
        if (rmd == 12)
        {
            *(uint32_t *)buf = regs->orbit_command_A[div];
            return No_exp;
        }
    }

    if ((offset >= POWER_COMMAND_0) && (offset < POWER_COMMAND_47))
    {
        rmd = (offset - POWER_COMMAND_0) % 0xc;
        div = (offset - POWER_COMMAND_0) / 0xc;

        if (rmd == 0)
        {
            *(uint32_t *)buf = regs->power_command_X[div];
            return No_exp;
        }

        if (rmd == 4)
        {
            *(uint32_t *)buf = regs->power_command_T[div];
            return No_exp;
        }

        if (rmd == 8)
        {
            *(uint32_t *)buf = regs->power_command_A[div];
            return No_exp;
        }
    }

    switch(offset)
    {
        case ORBIT_COMMAND_MODE:
            *(uint32_t *)buf = regs->orbit_command_mode;
            break;
        case ORBIT_TIMER_REG:
            *(uint32_t *)buf = regs->orbit_timer_reg;
            break;
        case ORBIT_MODE_RESET:
            *(uint32_t *)buf = regs->orbit_mode_reset;
            break;
        case UNLOCK_REG:
            *(uint32_t *)buf = regs->unlock_reg;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in fight_control_sparc\n", offset);
            return Invarg_exp;
    }
    return No_exp;
}

static exception_t fight_control_sparc_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct fight_control_sparc_device *dev = opaque->obj;
    fight_control_sparc_reg_t* regs = dev->regs;
    pulse_msg_t msg;
    uint32_t val = *(uint32_t*)buf;
    int rmd, div, aocs_status;

    if ((offset >= SLOCK_COMMAND_0) && (offset < ATTITUDE_COMMAND_0))
    {
        rmd = offset % 0xc;
        div = offset / 0xc;

        if (rmd == 0)
        {
            regs->slock_command_X[div] = val;
            return No_exp;
        }

        if (rmd == 4)
        {
            regs->slock_command_T[div] = val;
            return No_exp;
        }

        if (rmd == 8)
        {
            regs->slock_command_A[div] = val;
            if (regs->slock_command_T[div] == 3 && regs->slock_command_A[div] == 0xa5a5)
            {
                msg.ucCmd = 2;
                msg.ucMode = 0;
                msg.ucChNo = div;
                msg.fPeriod = -1;
                msg.fWidth = (float)(regs->slock_command_X[div])/1000.0;
                if(dev->pulse_linker_iface == NULL)
                {
                    skyeye_log(Error_log, __FUNCTION__, "Can't get 'pulse_linker_iface' interface at Line:%d", __LINE__);
                } else
                {
                    if (dev->aocs_iface)
                    {
                        aocs_status = dev->aocs_iface->get_aocs_status(dev->aocs);
                        if (aocs_status)
                            dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_COMMAND_NUM, IO_4002);
                    } else
                    {
                        dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_COMMAND_NUM, IO_4002);
                    }
                }
            }
            return No_exp;
        }
    }

    if ((offset >= ATTITUDE_COMMAND_0) && (offset < ORBIT_COMMAND_0))
    {
        rmd = (offset - ATTITUDE_COMMAND_0) % 0xc;
        div = (offset - ATTITUDE_COMMAND_0) / 0xc;

        if (rmd == 0)
        {
            regs->attitude_command_X[div] = val;
            //AttOc_output_data_to_file(dev, div, val);
            return No_exp;
        }

        if (rmd == 4)
        {
            regs->attitude_command_T[div] = val;
            return No_exp;
        }

        if (rmd == 8)
        {
            regs->attitude_command_A[div] = val;
            if (regs->attitude_command_T[div] == 3 && regs->attitude_command_A[div] == 0xa5a5)
            {
                msg.ucCmd = 2;
                msg.ucMode = 0;
                msg.ucChNo = div;
                msg.fPeriod = -1;
                msg.fWidth = (float)(regs->attitude_command_X[div]/1000.0);

                if(dev->pulse_linker_iface == NULL)
                {
                    skyeye_log(Error_log, __FUNCTION__, "Can't get 'pulse_linker_iface' interface at Line:%d", __LINE__);
                } else
                {
                    if (dev->aocs_iface)
                    {
                        aocs_status = dev->aocs_iface->get_aocs_status(dev->aocs);
                        if (aocs_status)
                            dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_COMMAND_NUM, IO_4001);
                    } else
                    {
                        dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_COMMAND_NUM, IO_4001);
                    }
                }
            }
            return No_exp;
        }
    }

    if ((offset >= ORBIT_COMMAND_0) && (offset < ORBIT_COMMAND_MODE))
    {
        rmd = (offset - ORBIT_COMMAND_0) % 0x10;
        div = (offset - ORBIT_COMMAND_0) / 0x10;

        if (rmd == 0)
        {
            //regs->orbit_command_X[div] = val;
            dev->orbit_width[div].u16.lsb = val;
            return No_exp;
        }

        if (rmd == 4)
        {
            //regs->orbit_command_F[div] = val;
            dev->orbit_width[div].u16.msb = val & 0xF;
            return No_exp;
        }

        if (rmd == 8)
        {
            regs->orbit_command_T[div] = val;
            return No_exp;
        }

        if (rmd == 12)
        {
            regs->orbit_command_A[div] = val;
            return No_exp;
        }
    }

    if ((offset >= POWER_COMMAND_0) && (offset < POWER_COMMAND_47))
    {
        rmd = (offset - POWER_COMMAND_0) % 0xc;
        div = (offset - POWER_COMMAND_0) / 0xc;

        if (rmd == 0)
        {
            regs->power_command_X[div] = val;
            return No_exp;
        }

        if (rmd == 4)
        {
            regs->power_command_T[div] = val;
            return No_exp;
        }

        if (rmd == 8)
        {
            regs->power_command_A[div] = val;
            if (regs->power_command_T[div] == 3 && regs->power_command_A[div] == 0xa5a5)
            {
                msg.ucCmd = 2;
                msg.ucMode = 0;
                msg.ucChNo = power_id[div];
                msg.fPeriod = -1;
                msg.fWidth = (float)(regs->power_command_X[div]/1000.0);

                if(dev->pulse_linker_iface == NULL)
                {
                    skyeye_log(Error_log, __FUNCTION__, "Can't get 'pulse_linker_iface' interface at Line:%d", __LINE__);
                } else
                {
                    if (dev->aocs_iface)
                    {
                        aocs_status = dev->aocs_iface->get_aocs_status(dev->aocs);
                        if (aocs_status)
                            dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_POWER_NUM, IO_4001);
                    } else
                    {
                        dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_POWER_NUM, IO_4001);
                    }
                }
            }
            return No_exp;
        }
    }

    switch(offset)
    {
        case ORBIT_COMMAND_MODE:
            regs->orbit_command_mode = val;
            if (val == 0xaabb)
            {
                //定时输出
                if (dev->handle == -1)
                    dev->handle = system_register_timer_handler(dev->obj, 1000, (time_sched_t)send_pulse_function, SCHED_US | SCHED_NORMAL);
                else
                    system_enable_timer(dev->handle);

            } else if (val == 0x5544)
            {
                //直接输出
                ObtOc_send_pulse(opaque, (uint32_t *)dev->orbit_width);
            } else
            {
                //do nothing
            }
            break;
        case ORBIT_TIMER_REG:
            regs->orbit_timer_reg = val;
            dev->delay_time_cnt = val;
            break;
        case ORBIT_MODE_RESET:
            regs->orbit_mode_reset = val;
            regs->orbit_command_mode = 0;

            if (dev->handle != -1)
            {
                system_disable_timer(dev->handle);
            }

            if (dev->timer_handler != -1)
            {
                system_disable_timer(dev->timer_handler);
            }

            break;
        case UNLOCK_REG:
            regs->unlock_reg = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in fight_control_sparc\n", offset);
            return Invarg_exp;
    }
    return No_exp;
}

static char* fight_control_sparc_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
    return regs_name[id];
}

static uint32_t fight_control_sparc_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
    fight_control_sparc_device *dev = conf_obj->obj;
    fight_control_sparc_reg_t* regs = dev->regs;
    uint32_t* regs_value = (uint32_t*)regs + id;
    return *regs_value;
}

static exception_t fight_control_sparc_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
    fight_control_sparc_device *dev = conf_obj->obj;
    fight_control_sparc_reg_t* regs = dev->regs;
    *((uint32_t*)regs + id) = value;

    return No_exp;
}

static uint32_t fight_control_sparc_get_regnum(conf_object_t* conf_obj)
{
    fight_control_sparc_device *dev = conf_obj->obj;
    fight_control_sparc_reg_t* regs = dev->regs;
    return  REG_NUM;
}

static uint32_t fight_control_sparc_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
    fight_control_sparc_device *dev = conf_obj->obj;
    fight_control_sparc_reg_t* regs = dev->regs;
    uint32_t regnum;
    regnum = REG_NUM;
    int i;
    for(i = 0; i < regnum; i++)
    {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }
    return regnum;
}

static exception_t output_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    struct fight_control_sparc_device *dev = obj->obj;
    dev->intf_obj = obj2;
    dev->ad_intf = (skyeye_ad_data_intf *)SKY_get_iface(dev->intf_obj, SKYEYE_AD_FILE_DATA_INTF_NAME);
    return No_exp;

}

static exception_t output_file_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    struct fight_control_sparc_device *dev = obj->obj;
    *obj2 = dev->intf_obj;
    *port = NULL;
    return No_exp;
}

static conf_object_t* new_fight_control_sparc(const char* obj_name)
{
    fight_control_sparc_device* dev = skyeye_mm_zero(sizeof(fight_control_sparc_device));
    fight_control_sparc_reg_t* regs =  skyeye_mm_zero(sizeof(fight_control_sparc_reg_t));
    dev->obj = new_conf_object(obj_name, dev);
    /* init fight_control_sparc regs */
    dev->regs = regs;
    dev->timer_scheduler_id = -1;
    dev->delay_time_cnt = 0;
    dev->sched_ms = 2000;

    return dev->obj;
}

static exception_t free_fight_control_sparc(conf_object_t* conf_obj)
{
    fight_control_sparc_device *dev = conf_obj->obj;
    skyeye_free(dev->regs);
    skyeye_free(dev->obj);
    skyeye_free(dev);

    return No_exp;
}

static exception_t pulse_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    fight_control_sparc_device* dev = (fight_control_sparc_device*)(obj->obj);
    dev->pulse_linker = obj2;
    dev->pulse_linker_iface = (pulse_linker_intf*)SKY_get_iface(dev->pulse_linker, PULSE_LINKER_INTF);

    if (dev->pulse_linker_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'pulse_linker_iface' interface from %s fail.", obj2->objname);
    }

    return No_exp;
}

static exception_t pulse_linker_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    fight_control_sparc_device* dev = (fight_control_sparc_device*)(obj->obj);
    *obj2 = dev->pulse_linker;
    *port = NULL;
    return No_exp;
}

static exception_t aocs_intf_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    fight_control_sparc_device* dev = (fight_control_sparc_device*)(obj->obj);
    dev->aocs = obj2;
    dev->aocs_iface = (get_aocs_intf*)SKY_get_iface(dev->aocs, GET_AOCS_INTF);

    if (dev->aocs_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'aocs_iface' interface from %s fail.", obj2->objname);
    }
    return No_exp;

}

static exception_t aocs_intf_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    fight_control_sparc_device* dev = (fight_control_sparc_device*)(obj->obj);
    *obj2 = dev->aocs;
    *port = NULL;
    return No_exp;
}

static fight_control_sparc_register_attribute(conf_class_t* clss)
{
}

void init_fight_control_sparc(){
    static skyeye_class_t class_data =
    {
        .class_name = "sparc_fight_control",
        .class_desc = "sparc_fight_control",
        .new_instance = new_fight_control_sparc,
        .free_instance = free_fight_control_sparc
    };

    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
    static const memory_space_intf io_memory =
    {
        .read = fight_control_sparc_read,
        .write = fight_control_sparc_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = fight_control_sparc_get_regvalue_by_id,
        .get_regname_by_id = fight_control_sparc_get_regname_by_id,
        .set_regvalue_by_id = fight_control_sparc_set_regvalue_by_id,
        .get_regnum = fight_control_sparc_get_regnum,
        .get_regid_by_name = fight_control_sparc_get_regid_by_name,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const pulse_dev_intf pulse_dev_interface =
    {
        .receive_pulse = pulse_linker_recv_pulse,
    };
    SKY_register_iface(clss, PULSE_DEV_INTF, &pulse_dev_interface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory,
        },
        (struct InterfaceDescription)
        {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_intf,
        },
        (struct InterfaceDescription)
        {
            .name = PULSE_DEV_INTF,
            .iface = &pulse_dev_interface,
        }
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = SKYEYE_AD_FILE_DATA_INTF_NAME,
            .set = output_file_set,
            .get = output_file_get,
        },
        (struct ConnectDescription )
        {
            .name = PULSE_LINKER_INTF,
            .set = pulse_linker_set,
            .get = pulse_linker_get,
        },
        (struct ConnectDescription )
        {
            .name = GET_AOCS_INTF,
            .set = aocs_intf_set,
            .get = aocs_intf_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    fight_control_sparc_register_attribute(clss);
}
