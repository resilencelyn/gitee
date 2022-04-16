#include<skyeye_types.h>
#include "skyeye_mm.h"
#include <skyeye_types.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <skyeye_system.h>
#include <memory_space.h>
#include <skyeye_misc.h>
#include <skyeye_lock.h>
#include <skyeye_addr_space.h>
#include <sim_control.h>
extern init_memory_space();
extern init_memory_space_plug();

uint32_t common_class_var = 0;

#define ACTIVE 	 0X0
#define TYPE 	 0X4
#define NAME 	 0X8
#define AUTO 	 0Xc
#define DATA0 	 0X10
#define DATA1 	 0X14
#define CONFIRM  0X1c
#define EXP 	 0X20
#define VAR_ADDR 0X24

uint32_t idle_device_id_allocator = 0;
typedef struct
{
    conf_object_t *obj;
      LIST_HEAD(vm_point_list, vm_point) vm_point_list;
    char name[NAME_LEN];
    uint32_t name_cnt;
    vm_point_t *cur_vm;
    uint64_t cur_ms;
    uint32_t idle_device_id;
    uint32_t vm_point_id_allocator;
    RWLOCK_T dev_lock;
    time_handle_t *time_handle;
    double simulation_time;
} idle_device_t;

uint8_t type_bytes[] = { 4, 4, 8, 8, 8, 4 };
void insert_address_monitor(conf_object_t * opaque, addr_info_t * inf)
{
    idle_device_t *dev = opaque->obj;
    int cnt, i, j, byte_cnt;
    conf_object_t *memory_space;
    sys_dev_t *sys_dev;

    byte_cnt = type_bytes[inf->type];
    cnt = ((sys_dev_t *) (opaque->sys_struct))->sys_soc->mem_cnt;
    for (i = 0; i < cnt; i++)
    {
        sys_dev = ((sys_dev_t *) (opaque->sys_struct))->sys_soc->memory_space[i];
        memory_space = sys_dev->dev;
        addr_space_t *dev = (addr_space_t *) memory_space->obj;

        for (j = 0; j < byte_cnt; j++)
            insert_pgd_intem(dev->mm_monitor, inf->addr + j, inf);
    }
}

static exception_t idle_device_read(conf_object_t * opaque, generic_address_t offset, void *buf, size_t count)
{
    idle_device_t *dev = opaque->obj;

    return No_exp;
}

int insert_exp_to_vm(conf_object_t * opaque, uint32_t id, char *exp)
{
    char *items[100];
    char tmp[EXP_LEN];
    idle_device_t *dev = opaque->obj;
    vm_point_t *vm_point;

    RW_RDLOCK(dev->dev_lock);
    LIST_FOREACH(vm_point, &(dev->vm_point_list), list_entry)
    {
        if (vm_point->vm_point_id != id)
            continue;
        RW_WRLOCK(vm_point->vm_lock);
        if (vm_point->s)
        {
            strcpy(vm_point->exp, exp);
            strcpy(tmp, exp);
            if (get_items(items, tmp) >= 4)
            {
                decode_cmd(&(vm_point->addr_info), items);
            } else
            {
                vm_point->addr_info.exp_enable = False;
            }
        }
        RW_UNLOCK(vm_point->vm_lock);
    }
    RW_UNLOCK(dev->dev_lock);

}

int remove_exp_from_vm(conf_object_t * opaque, uint32_t id)
{
    idle_device_t *dev = opaque->obj;
    vm_point_t *vm_point;

    RW_RDLOCK(dev->dev_lock);
    LIST_FOREACH(vm_point, &(dev->vm_point_list), list_entry)
    {
        if (vm_point->vm_point_id != id)
            continue;
        RW_WRLOCK(vm_point->vm_lock);
        if (vm_point->s)
        {
            vm_point->addr_info.exp_enable = False;
        }
        RW_UNLOCK(vm_point->vm_lock);
    }
    RW_UNLOCK(dev->dev_lock);

}

void print_cur_var(vm_point_t * cur_vm)
{
    switch (cur_vm->type)
    {
        case MM_UINT32:
            printf("%s:%x\n", cur_vm->name, (uint32_t) cur_vm->cur_var);
            break;
        case MM_INT32:
            printf("%s:%d\n", cur_vm->name, (int32_t) cur_vm->cur_var);
            break;
        case MM_UINT64:
            printf("%s:%x\n", cur_vm->name, (uint64_t) cur_vm->cur_var);
            break;
        case MM_INT64:
            printf("%s:%d\n", cur_vm->name, (int64_t) cur_vm->cur_var);
            break;
        case MM_DOUBLE:
            printf("%s:%f\n", cur_vm->name, *(double *) (&cur_vm->cur_var));
            break;
        case MM_FLOAT:
            printf("%s:%f\n", cur_vm->name, *(float *) (&cur_vm->cur_var));
            break;
    }

}

uint32_t type_bit[] = {
    MM_BIT0 | MM_BIT1 | MM_BIT2 | MM_BIT3,      //UINT32 0
    MM_BIT0 | MM_BIT1 | MM_BIT2 | MM_BIT3,      //INT32  1
    MM_BIT0 | MM_BIT1 | MM_BIT2 | MM_BIT3 | MM_BIT4 | MM_BIT5 | MM_BIT6 | MM_BIT7,      //UINT64 2
    MM_BIT0 | MM_BIT1 | MM_BIT2 | MM_BIT3 | MM_BIT4 | MM_BIT5 | MM_BIT6 | MM_BIT7,      //INT64 3
    MM_BIT0 | MM_BIT1 | MM_BIT2 | MM_BIT3 | MM_BIT4 | MM_BIT5 | MM_BIT6 | MM_BIT7,      //DOUBLE 4
    MM_BIT0 | MM_BIT1 | MM_BIT2 | MM_BIT3,      //FLOAT
};

uint32_t get_cur_pc(vm_point_t * vm_point)
{
    sys_dev_t *sys_dev;
    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;

    sys_dev = (sys_dev_t *) (vm_point->idle_device->sys_struct);
    sys_soc = sys_dev->sys_soc;
    sys_cpu = sys_soc->cpus;            //get the fisrt cpu 
    conf_object_t *cpu = sys_cpu->cpu;
    core_info_intf *iface = (core_info_intf *) SKY_get_iface(cpu, CORE_INFO_INTF_NAME);

    if (iface)
    {
        return iface->get_pc(cpu);
    }
    return 0;
}

char *get_cur_arch(vm_point_t * vm_point)
{
    sys_dev_t *sys_dev;
    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;

    sys_dev = (sys_dev_t *) (vm_point->idle_device->sys_struct);
    sys_soc = sys_dev->sys_soc;
    sys_cpu = sys_soc->cpus;            //get the fisrt cpu 
    conf_object_t *cpu = sys_cpu->cpu;
    core_info_intf *iface = (core_info_intf *) SKY_get_iface(cpu, CORE_INFO_INTF_NAME);

    if (iface)
    {
        return iface->get_architecture(cpu);
    }
    return 0;
}

char *get_cur_binary(vm_point_t * vm_point)
{
    sys_dev_t *sys_dev;
    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;

    sys_dev = (sys_dev_t *) (vm_point->idle_device->sys_struct);
    sys_soc = sys_dev->sys_soc;
    return sys_soc->binary_path;

}

#ifdef __WIN32__
static bool_t execmd(char *cmd, char *result, unsigned size)
{
    char buffer[128];
    int len;
    FILE *pipe = _popen(cmd, "r");

    if (!pipe)
        return False;

    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe))
        {
            snprintf(result, size, "%s", buffer);
        }
    }
    len = strlen(result);
    if (result[len - 1] == '\n')
        result[len - 1] = 0;
    _pclose(pipe);
    return True;
}

#define CMD_LEN 1024
bool_t get_cur_source(char *archname, char *binary_path, uint32_t addr, char *source)
{
    char *bin = getenv("SKYEYEBIN");
    char cmd[CMD_LEN];
    int i;

    sprintf(cmd, "%s%s -e %s 0x%x\n", bin, "/../../../arch/tic6x/addr2line.exe", binary_path, addr);
    for (i = 0; i < strlen(cmd); i++)
    {
        if (cmd[i] == '\\')
        {
            cmd[i] = '/';
        }
    }
    return execmd(cmd, source, CMD_LEN);
}
#endif

void mm_callback(addr_info_t * addr_info, uint32_t addr, uint8_t val)
{
    uint64_t tmp = (uint64_t) val;
    vm_point_t *vm_point = (vm_point_t *) (addr_info->vm_point);

    addr_info->val_w |= tmp << (8 * (addr - addr_info->addr));
    addr_info->bit_w |= (1 << (addr - addr_info->addr));

    if (addr_info->bit_w == type_bit[addr_info->type])
    {
        RW_WRLOCK(vm_point->vm_lock);
        vm_point->cur_var = addr_info->val_w;
        RW_UNLOCK(vm_point->vm_lock);
        if (addr_info->exp_enable && verif_cmd(addr_info, vm_point->cur_var))
        {
            uint32_t cur_pc;
            char *arch;
            char *binary;
            char source[1024];
            int ret;

            SIM_stop(0);
            cur_pc = get_cur_pc(vm_point);
            arch = get_cur_arch(vm_point);
            binary = get_cur_binary(vm_point);
#ifdef __WIN32__
            ret = get_cur_source(arch, binary, cur_pc, source);
            skyeye_log(Info_log, __FUNCTION__, "SkyEye stoped,[%s] conditional hit\n", vm_point->exp);
            if (ret)
                skyeye_log(Info_log, __FUNCTION__, "The current code stops at [%s]\n", source);
#endif

        }
        //print_cur_var(vm_point);
        addr_info->bit_w = 0;
        addr_info->val_w = 0;
    }

}

static exception_t idle_device_write(conf_object_t * opaque, generic_address_t offset, void *buf, size_t count)
{
    idle_device_t *dev = opaque->obj;
    uint32_t val = *(uint32_t *) buf;

    switch (offset)
    {
        case ACTIVE:
            {
                vm_point_t *vm_ptr;

                dev->name[dev->name_cnt + 1] = 0;
                bool_t found = False;

                LIST_FOREACH(vm_ptr, &(dev->vm_point_list), list_entry)
                {
                    if (strcmp(vm_ptr->name, dev->name) == 0)
                    {
                        found = True;
                        dev->cur_vm = vm_ptr;
                        break;
                    }
                }
                if (!found)
                {
                    vm_ptr = skyeye_mm_zero(sizeof (vm_point_t));
                    RWLOCK_INIT(vm_ptr->vm_lock);
                    vm_ptr->var_head = 0;
                    vm_ptr->vm_point_id = (dev->idle_device_id << 2) | (dev->vm_point_id_allocator++);
                    vm_ptr->idle_device = opaque;
                    strcpy(vm_ptr->name, dev->name);
                    RW_WRLOCK(dev->dev_lock);
                    LIST_INSERT_HEAD(&(dev->vm_point_list), vm_ptr, list_entry);
                    RW_UNLOCK(dev->dev_lock);
                    dev->cur_vm = vm_ptr;
                }

                dev->cur_vm->confirmed = False;
                dev->name_cnt = 0;
                dev->cur_vm->exp_cnt = 0;
                vm_ptr->active = True;
                dev->cur_vm->cur_var = 0;
                break;
            }
        case TYPE:
            dev->cur_vm->type = (MMDataType_t) val;
            break;
        case NAME:
            dev->name[dev->name_cnt++] = (char) val;
            break;
        case AUTO:
            dev->cur_vm->s = (bool_t) val;
            break;
        case DATA0:
            dev->cur_vm->cur_var |= val;
            break;
        case DATA1:
            {
                uint64_t val64 = val;

                dev->cur_vm->cur_var |= val64 << 32;
                break;
            }
        case CONFIRM:
            //print_cur_var(dev->cur_vm);
            if (dev->cur_vm->s)
            {
                char *items[100];
                char exp[EXP_LEN];

                dev->cur_vm->addr_info.addr = dev->cur_vm->var_addr;
                dev->cur_vm->addr_info.type = dev->cur_vm->type;
                dev->cur_vm->addr_info.callback_ptr = mm_callback;
                dev->cur_vm->addr_info.vm_point = (void *) (dev->cur_vm);
                strncpy(exp, dev->cur_vm->exp, EXP_LEN);
                if (get_items(items, exp) >= 4)
                {
                    decode_cmd(&(dev->cur_vm->addr_info), items);
                } else
                {
                    dev->cur_vm->addr_info.exp_enable = False;
                }

                insert_address_monitor(opaque, &(dev->cur_vm->addr_info));

            }
            dev->cur_vm->confirmed = True;
            dev->cur_vm->exp[dev->cur_vm->exp_cnt] = 0;
            break;
        case EXP:
            dev->cur_vm->exp[dev->cur_vm->exp_cnt++] = (char) val;
            break;
        case VAR_ADDR:
            dev->cur_vm->var_addr = val;

            common_class_var = val;
            //printf("%s, %d, %x\n", __func__,__LINE__, val);
        default:
            return Not_found_exp;
    }
    return No_exp;
}

static void calc_timer_func(void *obj)
{
    idle_device_t *dev = ((conf_object_t *) obj)->obj;

    dev->simulation_time += 0.0001f;    // 1.0 s = 0.1 ms 0.0001 us
}

static conf_object_t *new_idle_device(char *obj_name)
{
    idle_device_t *dev = skyeye_mm_zero(sizeof (idle_device_t));

    dev->obj = new_conf_object(obj_name, dev);

    // imulation time accuracy: 100 us, 0.1 ms
    dev->simulation_time = 0.0f;
    dev->time_handle =
        system_register_timer_handler(dev->obj, 100, (time_sched_t) calc_timer_func, SCHED_US | SCHED_NORMAL);
    return dev->obj;
}
static exception_t free_idle_device(conf_object_t * opaque)
{
    idle_device_t *dev = opaque->obj;

    RWLOCK_DESTROY(dev->dev_lock);
    return No_exp;
}

void callback(void *obj, time_data_t * data)
{
    idle_device_t *dev = ((conf_object_t *) obj)->obj;

    dev->cur_ms++;
    vm_point_t *vm_ptr;

    RW_RDLOCK(dev->dev_lock);
    LIST_FOREACH(vm_ptr, &(dev->vm_point_list), list_entry)
    {
        RW_WRLOCK(vm_ptr->vm_lock);
        if (!vm_ptr->confirmed)
        {
            RW_UNLOCK(vm_ptr->vm_lock);
            continue;
        }
        if ((vm_ptr->var_head == VAR_CNT) || (vm_ptr->var_head == 0))
        {
            //prevent var_head overflow 
            vm_ptr->var_head = 0;
            //update virtual timestamp
            vm_ptr->ms_head = dev->cur_ms;
        }

        vm_ptr->int_var[vm_ptr->var_head] = vm_ptr->cur_var;

        vm_ptr->var_head++;
        RW_UNLOCK(vm_ptr->vm_lock);
    }
    RW_UNLOCK(dev->dev_lock);

}

static exception_t config_idle_device(conf_object_t * opaque)
{
    idle_device_t *dev = opaque->obj;

    LIST_INIT(&dev->vm_point_list);
    dev->name_cnt = 0;
    dev->idle_device_id = idle_device_id_allocator++;
    RWLOCK_INIT(dev->dev_lock);

    system_register_timer_handler(dev->obj, (uint32_t) 1, (time_sched_t) callback, SCHED_MS | SCHED_NORMAL);

    return No_exp;
}

uint32_t get_vms(conf_object_t * opaque, vm_t * vms, uint32_t limit)
{
    uint32_t cnt = 0;
    vm_t *vm_p;
    idle_device_t *dev = opaque->obj;
    vm_point_t *vm_ptr;

    RW_RDLOCK(dev->dev_lock);
    LIST_FOREACH(vm_ptr, &(dev->vm_point_list), list_entry)
    {
        vm_p = vms + cnt;
        cnt++;
        vm_p->id = vm_ptr->vm_point_id;
        vm_p->type = vm_ptr->type;
        vm_p->name = vm_ptr->name;
        if (cnt >= limit)
            break;
    }
    RW_UNLOCK(dev->dev_lock);
    return cnt;
}

uint32_t get_vars(conf_object_t * opaque, uint32_t id, var_t * vars, uint32_t limit)
{
    idle_device_t *dev = opaque->obj;
    uint32_t i;
    vm_point_t *vm_ptr;
    var_t *var_p;

    RW_RDLOCK(dev->dev_lock);
    LIST_FOREACH(vm_ptr, &(dev->vm_point_list), list_entry)
    {
        if (vm_ptr->vm_point_id != id)
            continue;
        RW_WRLOCK(vm_ptr->vm_lock);
        for (i = 0; i < vm_ptr->var_head; i++)
        {
            if (i > (limit - 1))
                break;

            var_p = vars + i;
            var_p->type = vm_ptr->type;
            var_p->t = vm_ptr->ms_head + i;
            switch (var_p->type)
            {
                case MM_UINT32:
                case MM_UINT64:
                    var_p->uint_v = vm_ptr->int_var[i];
                    break;
                case MM_INT32:
                    var_p->int_v = (int32_t) vm_ptr->int_var[i];
                    break;
                case MM_INT64:
                    var_p->int_v = vm_ptr->int_var[i];
                    break;
                case MM_DOUBLE:
                    var_p->double_v = *(double *) (vm_ptr->int_var + i);
                    break;
                case MM_FLOAT:
                    var_p->double_v = *(float *) (vm_ptr->int_var + i);
                    break;
                default:
                    break;
            }
        }
        vm_ptr->var_head -= i;
        RW_UNLOCK(vm_ptr->vm_lock);

        //jump out of LIST_FOREACH loop 
        break;

    }
    RW_UNLOCK(dev->dev_lock);
    //return the previous var_head 
    return i;
}

double get_curr_time(conf_object_t * opaque)
{
    idle_device_t *dev = opaque->obj;

    return dev->simulation_time;
}

void init_idle_device()
{
    static skyeye_class_t class_data = {
        .class_name = "idle_device",
        .class_desc = "idle_device",
        .new_instance = new_idle_device,
        .free_instance = free_idle_device,
        .config_instance = config_idle_device,
        .get_attr = NULL,
        .set_attr = NULL
    };
    conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);
    static const memory_space_intf io_memory = {
        .read = idle_device_read,
        .write = idle_device_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const var_monitor_intf var_monitor = {
        .get_vms = get_vms,
        .get_vars = get_vars,
        .insert_exp = insert_exp_to_vm,
        .remove_exp = remove_exp_from_vm,
    };
    SKY_register_iface(clss, VAR_MONITOR_INTF_NAME, &var_monitor);

    static const idle_api_intf_t idle_api = {
        .get_curr_time = get_curr_time,
    };
    SKY_register_iface(clss, IDLE_DEV_API_INTF_NAME, &idle_api);
}

exception_t init_common_class()
{
    init_memory_space();
    init_memory_space_plug();
    init_idle_device();
    return No_exp;
}
