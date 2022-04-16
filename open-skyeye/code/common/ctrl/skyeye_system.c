/* Copyright (C)
 * 2015 - zyuming zyumingfit@163.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.  *
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
* @file skyeye_time.c
* @brief the skyeye time implementation
* @author zyuming zyumingfit@163.com
* @version
* @date 2015-09-02
*/

#include "skyeye_mm.h"
#include "skyeye_sched.h"
#include "skyeye_queue.h"
#include "skyeye_attribute.h"
#include "skyeye_obj.h"
#include "skyeye_log.h"
#include <stdio.h>
#include "skyeye_lock.h"
#include "skyeye_system.h"
#include "skyeye_iface.h"
#include "skyeye_types.h"
#ifdef __MINGW32__
#include <windows.h>
#endif

skyeye_system_t *vir_system = NULL;

#define DEFAULT_CPU_FREQ 200000000      //20MHZ

#define DEFAULT_IO_CYCLE 1000000

#define MIN_TIME_ACCURACY(freq) ((1000000000 / freq) > 1 ? (1000000000 / freq) : 1)

void system_init()
{
    int i;

    vir_system = skyeye_mm_zero(sizeof (skyeye_system_t));
    vir_system->timer_cnt = 0;
    vir_system->soc_cnt = 0;
    vir_system->linker_cnt = 0;
    vir_system->dispatch_ready = False;
    vir_system->syn_enable = NO_SYNCHRONIZATION;
    vir_system->syn_runtime = 0;
    for (i = 0; i < MAX_SOC; i++)
    {
        sys_soc_t *soc = &(vir_system->socs[i]);

        soc->dev_cnt = 0;
        soc->cpu_cnt = 0;
        soc->timer_cnt = 0;
        LIST_INIT(&(soc->handler_list));
    }
    RWLOCK_INIT(vir_system->lock);
}

void system_clear()
{
    int i, j;
    struct event *tmp;
    struct event *q = NULL;

    for (i = 0; i < vir_system->soc_cnt; i++)
    {
        sys_soc_t *sys_soc = &(vir_system->socs[vir_system->soc_cnt]);

        LIST_FOREACH(tmp, &(sys_soc->handler_list), list_entry)
        {
            q = tmp;
            LIST_REMOVE(tmp, list_entry);
            skyeye_free(q);
        }
        LIST_EMPTY(&sys_soc->handler_list);
    }
    skyeye_free(vir_system);
    vir_system = NULL;
}

void *system_get_struct(conf_object_t * obj)
{
    return obj->sys_struct;
}

sys_soc_t *system_register_soc(conf_object_t * soc)
{
    sys_soc_t *sys_soc = &(vir_system->socs[vir_system->soc_cnt]);

    vir_system->soc_cnt++;
    sys_soc->soc = soc;
    soc->sys_struct = (void *) sys_soc;
    return sys_soc;
}

sys_linker_t *system_register_linker(conf_object_t * linker)
{
    sys_linker_t *sys_linker = &(vir_system->linkers[vir_system->linker_cnt]);

    vir_system->linker_cnt++;
    linker->sys_struct = sys_linker;
    sys_linker->linker = linker;
    return sys_linker;
}

sys_dev_t *system_register_dev(conf_object_t * dev, conf_object_t * soc)
{
    sys_soc_t *sys_soc = (sys_soc_t *) system_get_struct(soc);

    sys_dev_t *sys_dev = &(sys_soc->devs[sys_soc->dev_cnt]);

    sys_soc->dev_cnt++;
    sys_dev->sys_soc = sys_soc;
    sys_dev->dev = dev;
    dev->sys_struct = sys_dev;
    if (!strcmp(dev->class_name, "idle_device"))
    {
        sys_soc->idle_device = sys_dev;
    }
    if (!strcmp(dev->class_name, "memory_space"))
    {
        sys_soc->memory_space[sys_soc->mem_cnt++] = sys_dev;
    }
    return sys_dev;
}

sys_cpu_t *system_register_cpu(conf_object_t * cpu, conf_object_t * soc)
{

    sys_soc_t *sys_soc = (sys_soc_t *) system_get_struct(soc);
    sys_cpu_t *sys_cpu = &(sys_soc->cpus[sys_soc->cpu_cnt]);

    sys_cpu->id = sys_soc->cpu_cnt;
    sys_soc->cpu_cnt++;
    sys_cpu->sys_soc = sys_soc;
    sys_cpu->cpu = cpu;
    cpu->sys_struct = sys_cpu;

    return sys_cpu;
}

static void system_config_cpu(sys_cpu_t * sys_cpu)
{
    conf_object_t *cpu = sys_cpu->cpu;

    if (!SKY_has_attribute(cpu, "freq_hz"))
    {
        attr_value_t attr = SKY_get_attribute(cpu, "freq_hz");

        sys_cpu->freq_hz = SKY_attr_uinteger(attr);
    } else
    {
        sys_cpu->freq_hz = DEFAULT_CPU_FREQ;
    }

    sys_cpu->core_info = SKY_get_iface(cpu, CORE_INFO_INTF_NAME);
    if (NULL == sys_cpu->core_info)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get iface:%s from %s.\n", CORE_INFO_INTF_NAME, cpu->objname);
    }
    sys_cpu->core_exec = (core_exec_intf *) SKY_get_iface(cpu, CORE_EXEC_INTF_NAME);
    if (NULL == sys_cpu->core_exec)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get iface:%s from %s.\n", CORE_EXEC_INTF_NAME, cpu->objname);
    }

    //whether it is x86 architecture
    char *arch_name = sys_cpu->core_info->get_architecture(cpu);

    sys_cpu->is_x86_arch = False;
    if (!strncmp(arch_name, "x86", strlen("x86")))
    {
        sys_cpu->is_x86_arch = True;
        sys_cpu->last_usec = 0;
    }

}

static void system_config_dev(sys_soc_t * sys_dev)
{

}

static void system_config_linker(sys_linker_t * sys_linker)
{

}

static void system_config_soc(sys_soc_t * sys_soc)
{
}

char **get_current_system_module_name()
{
    skyeye_system_t *system = system_get();
    static char *module_name[MAX_MODULE_NUM];
    sys_dev_t *sys_dev;
    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;
    sys_linker_t *sys_linker;
    conf_object_t *obj;

    int i, j, k = 0;
    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc = system->socs + i;
        obj = sys_soc->soc;
        //soc name
        module_name[k++] = obj->objname;

        for (j = 0; j < sys_soc->cpu_cnt; j++)
        {
            sys_cpu = sys_soc->cpus + j;
            obj = sys_cpu->cpu;
            //cpu name
            module_name[k++] = obj->objname;
        }

        for (j = 0; j < sys_soc->dev_cnt; j++)
        {
            sys_dev = sys_soc->devs + j;
            obj = sys_dev->dev;
            //device name
            module_name[k++] = obj->objname;
        }
    }

    for (j = 0; j < system->linker_cnt; j++)
    {
        sys_linker = system->linkers + j;
        obj = sys_linker->linker;
        //linker name
        module_name[k++] = obj->objname;
    }

    return module_name;
}

int system_config_conf_obj()
{
    skyeye_system_t *system = system_get();
    sys_dev_t *sys_dev;
    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;
    sys_linker_t *sys_linker;
    int i, j;

    //config socs
    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc = system->socs + i;
        config_conf_obj(sys_soc->soc);
        system_config_soc(sys_soc);
        //config devices
        for (j = 0; j < sys_soc->dev_cnt; j++)
        {
            sys_dev = sys_soc->devs + j;
            config_conf_obj(sys_dev->dev);
            system_config_dev(sys_dev);
        }
        //config cpus
        for (j = 0; j < sys_soc->cpu_cnt; j++)
        {
            sys_cpu = sys_soc->cpus + j;
            config_conf_obj(sys_cpu->cpu);
            system_config_cpu(sys_cpu);
        }
    }
    //config linkers
    for (j = 0; j < system->linker_cnt; j++)
    {
        sys_linker = system->linkers + j;
        config_conf_obj(sys_linker->linker);
        system_config_linker(sys_linker);
    }
    return True;
}

skyeye_system_t *system_get()
{
    return vir_system;
}

static uint64_t gcd(uint64_t a, uint64_t b)
{
    if (0 == b)
        return a;
    return gcd(b, a % b);
}

static uint64_t gcdn(uint64_t * digits, uint64_t length)
{
    if (1 == length)
        return digits[0];
    else
        return gcd(digits[length - 1], gcdn(digits, length - 1));
}

static time_tran_table[] = {
    1000000000,                         //S to NS
    1000000,                            //MS to NS
    1000,                               //US to NS
    1                                   //NS to NS
};

static uint64_t time_to_ns(uint64_t time, uint32_t type)
{
    return time * time_tran_table[type];

}

static uint64_t double_time_to_ns(double time, uint32_t type)
{

    return (uint64_t) (time * time_tran_table[type]);

}

static uint64_t get_soc_freqhz(sys_soc_t * soc)
{
    if (soc->cpu_cnt >= 0 && soc->cpus[0].freq_hz != 0)
    {
        return soc->cpus[0].freq_hz;
    }
    return DEFAULT_CPU_FREQ;
}

static void seek_max_time_slice(sys_soc_t * soc)
{
    uint32_t i;
    uint64_t time_cycle[MAX_TIMER];
    uint64_t timing, freq_hz;
    uint32_t time_type;

    for (i = 0; i < soc->timer_cnt; i++)
    {
        if (soc->timers[i] == NULL)
            continue;
        timing = time_to_ns(soc->timers[i]->time, soc->timers[i]->time_type);
        freq_hz = get_soc_freqhz(soc);
        timing = timing > MIN_TIME_ACCURACY(freq_hz) ? timing : MIN_TIME_ACCURACY(freq_hz);
        time_cycle[i] = ((float) freq_hz / (float) 1000000000) * (float) timing;
        soc->timers[i]->sched_cycle = time_cycle[i];
    }

    if (soc->timer_cnt != 0)
    {
        soc->io_cycle_count = gcdn(time_cycle, soc->timer_cnt);
    } else
    {
        soc->io_cycle_count = DEFAULT_IO_CYCLE;
    }

    return;
}

static void add_to_func_list(sys_soc_t * soc, sys_timer_t * timer)
{
    struct event *e;
    e = skyeye_mm(sizeof (struct event));
    e->timer = timer;
    e->target = timer->target;
    e->cycle_count = timer->sched_cycle;
    e->pass_count = 0;
    LIST_INSERT_HEAD(&(soc->handler_list), e, list_entry);
}

static void create_func_list(sys_soc_t * soc)
{
    struct event *e;
    int i;

    for (i = 0; i < soc->timer_cnt; i++)
    {
        e = skyeye_mm(sizeof (struct event));
        e->timer = soc->timers[i];
        e->target = soc->timers[i]->target;
        e->cycle_count = soc->timers[i]->sched_cycle;
        e->pass_count = 0;
        LIST_INSERT_HEAD(&(soc->handler_list), e, list_entry);
    }
}

bool_t system_del_timer_handler(time_handle_t handle)
{
    if (handle < 0 || handle >= vir_system->timer_cnt)
        return False;
    vir_system->timers[(uint32_t) handle].status = False;
    vir_system->timers[(uint32_t) handle].enable = False;
    vir_system->timers[(uint32_t) handle].target = NULL;
    return True;

}

bool_t system_disable_timer(time_handle_t handle)
{
    if (handle < 0 || handle >= vir_system->timer_cnt)
        return False;
    vir_system->timers[(uint32_t) handle].enable = False;
    return True;

}

bool_t system_enable_timer(time_handle_t handle)
{
    if (handle < 0 || handle >= vir_system->timer_cnt)
        return False;
    vir_system->timers[(uint32_t) handle].enable = True;
    return True;

}

time_handle_t _system_register_timer_handler(conf_object_t * obj, void *user_data, uint64_t time, time_sched_t func,
                                             uint32_t flags)
{
    int i = 0, cnt = 0, flag = 0;       //flag:Determine whether timers is the first to insert a linked list
    sys_timer_t *timer = NULL;

    for (i = 0; i < MAX_TIMER; i++)
    {
        if (vir_system->timers[i].target == NULL)
            break;
        if (!vir_system->timers[i].status)
        {                               //To determine whether the abandoned timers
            timer = &(vir_system->timers[i]);
            cnt = i;
            break;
        }
    }

    if (timer == NULL)
    {
        if (i == MAX_TIMER - 1)
        {
            printf("timer_cnt out of maximum range\n");
            return -1;
        }
        timer = &(vir_system->timers[vir_system->timer_cnt++]);
        cnt = vir_system->timer_cnt - 1;
        flag = 1;
    }

    timer->target = obj;
    timer->time = time;
    timer->time_type = (flags >> 1) & 0x3;
    timer->func = func;
    timer->user_data = user_data;
    timer->enable = True;
    timer->status = True;
    timer->sched_type = flags & 0x1;
    timer->be_sched_count = 0;
    if (vir_system->dispatch_ready)
    {
        sys_dev_t *sys_dev = obj->sys_struct;

        sys_dev->sys_soc->timers[cnt] = timer;
        if (flag == 1)
            sys_dev->sys_soc->timer_cnt = cnt + 1;
        seek_max_time_slice(sys_dev->sys_soc);
        if (flag == 1)
            add_to_func_list(sys_dev->sys_soc, timer);
    }

    return cnt;
}

static void dispatch_timers_to_soc(void)
{
    int i;
    sys_timer_t *timers = vir_system->timers;

    for (i = 0; i < vir_system->timer_cnt; i++)
    {
        sys_dev_t *dev = system_get_struct(timers[i].target);

        if (!dev)
        {
            skyeye_log(Info_log, __FUNCTION__, "%s not a device object\n", (timers[i].target)->objname);
            continue;
        }

        dev->sys_soc->timers[dev->sys_soc->timer_cnt++] = timers + i;
    }
    vir_system->dispatch_ready = True;
}

exception_t system_clock_calculation(void)
{
    int i;

    dispatch_timers_to_soc();
    for (i = 0; i < vir_system->soc_cnt; i++)
    {
        seek_max_time_slice(&(vir_system->socs[i]));
        create_func_list(&(vir_system->socs[i]));
    }

    return No_exp;
}

void system_cpu_cycle(sys_cpu_t * sys_cpu)
{

    sys_soc_t *soc = sys_cpu->sys_soc;
    struct event *tmp;
    int current_watch_count;
    uint32_t WatchPC;
    time_data_t data;
    int steps;

    //if synchronization disabled, every cycle exeute one or more instructions
    //if synchronizaton enabled, if the value of the syn_b bit in the syn_runtime is 1, exeute one or more instructions, otherwise, the current cycle doesn't do anything.
    if ((!vir_system->syn_enable) || (!(vir_system->syn_runtime & sys_cpu->syn_b)))
    {
        steps = sys_cpu->core_exec->run(sys_cpu->cpu);
        sys_cpu->step_sum += steps;

        if (vir_system->delay_syn)
        {
            if (!sys_cpu->is_x86_arch)
            {
                if (sys_cpu->step_sum >= sys_cpu->delay_syn_count)
                {

                    RW_WRLOCK(vir_system->lock);
                    vir_system->syn_enable = True;
                    vir_system->delay_syn = False;
                    RW_UNLOCK(vir_system->lock);
                }

            } else
            {
                uint64_t current_usec;

                current_usec = sys_cpu->core_info->get_usec(sys_cpu->cpu);
                if (current_usec >= vir_system->delay_syn_time_us)
                {
                    RW_WRLOCK(vir_system->lock);
                    vir_system->syn_enable = True;
                    vir_system->delay_syn = False;
                    RW_UNLOCK(vir_system->lock);
                }
            }

        }
        if (vir_system->syn_enable)
        {
            if (!sys_cpu->is_x86_arch)
            {
                sys_cpu->io_syn_pass_count += steps;
                //if io_cyn_pass_count is greater than or equal to io_syn_count, that means the current processor  reach the synchronization time
                if (sys_cpu->io_syn_pass_count >= sys_cpu->io_syn_count)
                {
                    RW_WRLOCK(vir_system->lock);
                    sys_cpu->io_syn_pass_count = 0;
                    //the value of the syn_b bit in the syn_runtime set to 1,indicates the current processor reach the synchronization time
                    vir_system->syn_runtime |= sys_cpu->syn_b;
                    //if syn is equal to syn_runtime, that means all processors reach the synchronization time, set syn_runtime to 0, start a new cycle push
                    if (vir_system->syn == vir_system->syn_runtime)
                    {
                        vir_system->syn_runtime = 0;
                    }
                    RW_UNLOCK(vir_system->lock);
                }
            } else
            {
                if (vir_system->all_soc_loaded)
                {
                    //未做任何操作
                }
#if 0
                uint64_t current_usec;

                current_usec = sys_cpu->core_info->get_usec(sys_cpu->cpu);
                sys_cpu->io_syn_pass_usec += (current_usec - sys_cpu->last_usec);
                sys_cpu->last_usec = current_usec;
                if (vir_system->syn_enable && (sys_cpu->io_syn_pass_usec >= vir_system->syn_time_us))
                {
                    sys_cpu->io_syn_pass_usec = sys_cpu->io_syn_pass_usec - vir_system->syn_time_us;
                    RW_WRLOCK(vir_system->lock);
                    vir_system->syn_runtime |= sys_cpu->syn_b;
                    if (vir_system->syn == vir_system->syn_runtime)
                    {
                        vir_system->syn_runtime = 0;
                    }
                    RW_UNLOCK(vir_system->lock);

                }
#endif
            }
        }

        LIST_FOREACH(tmp, &(soc->handler_list), list_entry)
        {
            if (tmp->timer->enable == True)
            {
                tmp->pass_count += steps;
                if (tmp->pass_count >= tmp->cycle_count)
                {
                    if (tmp->timer->sched_type == SCHED_NORMAL || tmp->timer->be_sched_count < SCHED_ONCE)
                    {
                        data.pass_cycle = tmp->pass_count - tmp->cycle_count;
                        data.user_data = tmp->timer->user_data;
                        tmp->timer->func(tmp->target, &data);
                        tmp->pass_count = tmp->pass_count - tmp->cycle_count;
                        tmp->timer->be_sched_count++;
                    }
                }
            }
        }

        for (current_watch_count = 0; current_watch_count < sys_cpu->pc_watch_count; current_watch_count++)
        {
            if (sys_cpu->pc_watch_info[current_watch_count]->addr == sys_cpu->core_info->get_pc(sys_cpu->cpu))
                if (sys_cpu->pc_watch_info[current_watch_count]->enable)
                {
                    sys_cpu->pc_watch_info[current_watch_count]->pc_watch_func(sys_cpu->
                                                                               pc_watch_info[current_watch_count]->
                                                                               pc_watch_data);
                }
        }
    } else
    {
    }

}

//set min synchronization time
void system_set_min_syn_time(double s, double delay)
{
    uint64_t freq_hz = DEFAULT_CPU_FREQ;
    uint64_t timing, delay_time;
    int i, j;
    conf_object_t *cpu;

    vir_system->synchronization_time = s;
    timing = double_time_to_ns(s, TIME_S);
    delay_time = double_time_to_ns(delay, TIME_S);

    uint32_t coef = (uint32_t) delay;

    for (i = 0; i < vir_system->soc_cnt; i++)
    {
        sys_soc_t *soc = vir_system->socs + i;

        for (j = 0; j < soc->cpu_cnt; j++)
        {
            sys_cpu_t *sys_cpu = &(soc->cpus[j]);

            if (sys_cpu->is_x86_arch)
            {
                continue;
            }
            freq_hz = sys_cpu->freq_hz;

            // MIN_TIME_ACCURACY(freq_hz): 最小时间周期数, 1ns执行的机器周期数目, 时间单位:(ns)
            // 设置同步时间, 同步时间不能小于最小时间周期数
            timing = timing > MIN_TIME_ACCURACY(freq_hz) ? timing : MIN_TIME_ACCURACY(freq_hz);
            sys_cpu->io_syn_count = ((float) freq_hz / (float) 1000000000) * (float) timing;
            sys_cpu->delay_syn_count = ((float) freq_hz / (float) 1000000000) * (float) delay_time;
            //sys_cpu->syn_b = 1 << (j * (i + 1));
            sys_cpu->syn_b = 1 << ((i * vir_system->soc_cnt) + j);
            vir_system->syn |= sys_cpu->syn_b;
        }
    }

    vir_system->syn_enable = True;
    vir_system->delay_syn = False;

    vir_system->syn_time_us = (uint64_t) (s * 1000000); //s to us
    vir_system->syn_time_us = (uint32_t) coef;
}

bool_t set_soc_delay_load(char *soc_name, uint32_t ms)
{
    conf_object_t *obj = get_conf_obj(soc_name);

    if (NULL == obj)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", soc_name);
        return False;
    }
    sys_soc_t *soc = (sys_soc_t *) (obj->sys_struct);

    if (ms < 0u)
        return False;
    soc->delay_load_ms = ms;
    return True;
}
