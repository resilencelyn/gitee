/*************************************************************************
    > File Name    : skyeye_new_api.c
    > Author       : jiachao
    > Mail         : jiachao@digiproto.com
    > Created Time : 2021/3/24 14:19:08
************************************************************************/

#include "skyeye_new_api.h"

enum
{
    OUTPUT_2_LOG = 0,
    OUTPUT_2_PYTHON = 1,
    OUTPUT_2_LOG_PY = 2
} put_msg_range;

static char *msg[128] = {0};

#define output_msg(log_level, err_code, ret_msg, range, ...)                             \
    do                                                                                   \
    {                                                                                    \
        sprintf(msg, __VA_ARGS__);                                                       \
        if (range == OUTPUT_2_PYTHON || range == OUTPUT_2_LOG_PY)                        \
            ret_msg = getErrorMessage("%x %s %d %s", err_code, __func__, __LINE__, msg); \
        if (range == OUTPUT_2_LOG || range == OUTPUT_2_LOG_PY)                           \
            skyeye_log(log_level, __FUNCTION__, "s", msg);                               \
    } while (0)

char **getErrorMessage(char *format, ...)
{
    static char *retPtr[MAX_PARM_NUM];
    static char tmpStr[MAX_PARM_NUM][128];
    va_list args;
    int num, i = 0;
    char c;

    va_start(args, format);

    while ((c = *format++) > 0)
    {
        if (c == '%')
        {
            switch (c = *format++)
            {
            case 's':
                assert(i < MAX_PARM_NUM);
                retPtr[i++] = va_arg(args, char *);
                break;
            case 'd':
                num = va_arg(args, int);
                assert(i < MAX_PARM_NUM);
                sprintf(tmpStr[i], "%d", num);
                retPtr[i] = tmpStr[i];
                i++;
                break;
            case 'x':
                num = va_arg(args, int);
                assert(i < MAX_PARM_NUM);
                sprintf(tmpStr[i], "0x%x", num);
                retPtr[i] = tmpStr[i];
                i++;
                break;
            }
        }
    }
    retPtr[i] = NULL;
    return retPtr;
}

/*
 * 1.仿真控制相关API接口
 */

// SkyEye运行
SkyEyeAPIRetST skyeye_run(void)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (!SIM_run())
    {
        output_msg(Debug_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG,
                   "Open-SkyEye running!\n");
        ApiRet.result = API_OK;
    }
    else
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "No load machine %s!\n");
        ApiRet.result = API_ERROR;
    }

    return ApiRet;
}

// SkyEye暂停
SkyEyeAPIRetST skyeye_stop(void)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (!SIM_stop(NULL))
    {
        output_msg(Debug_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG,
                   "Open-SkyEye stoped!\n");
        ApiRet.result = API_OK;
    }
    else
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "No load machine!\n");
        ApiRet.result = API_ERROR;
    }

    return ApiRet;
}

// SkyEye退出
SkyEyeAPIRetST skyeye_quit(void)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    close_network_lic();
    output_msg(Debug_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG,
               "Open-SkyEye quit!\n");
    SIM_quit();
    ApiRet.result = API_OK;

    return ApiRet;
}

// SkyEye重置
SkyEyeAPIRetST skyeye_reset(void)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    output_msg(Debug_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG,
               "Open-SkyEye reset!\n");
    SIM_reset();
    ApiRet.result = API_OK;

    return ApiRet;
}

// SkyEye获取运行状态
SkyEyeAPIRetST skyeye_running_status(void)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    ApiRet.retIntValue = (unsigned int)SIM_is_running();
    ApiRet.result = API_OK;

    return ApiRet;
}

// SkyEye init-ok命令实现，创建线程来调度执行处理器指令
SkyEyeAPIRetST skyeye_prepare_running()
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    /* Call prepare_to_run to create cell to run instructions*/
    if (0 == (unsigned int)prepare_to_run())
    {
        output_msg(Debug_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG,
                   "Open-SkyEye Init OK!\n");
        ApiRet.result = API_OK;
    }
    else
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call prepare_to_run Failed!\n");
        ApiRet.result = API_ERROR;
    }

    return ApiRet;
}

/*
 * 2.快照功能API接口
 */

// SkyEye加载快照
SkyEyeAPIRetST skyeye_load_checkpoint(char *checkpoint_path)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 != save_chp(checkpoint_path))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call save_chp Failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;

    return ApiRet;
}

// SkyEye保存快照
SkyEyeAPIRetST skyeye_save_checkpoint(char *checkpoint_path)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 != load_chp(checkpoint_path))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call load_chp Failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 * 3.断点功能API接口
 */

// SkyEye创建断点
SkyEyeAPIRetST skyeye_create_breakpoint(char *cpuname, uint32_t addr)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);
    int ret;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME, cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ret = iface->insert_bp(core, addr);

    if (ret != 0)
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call %s insert_bp iface Failed!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye删除断点-通过ID
SkyEyeAPIRetST skyeye_delete_breakpoint_by_id(char *cpuname, int id)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);
    uint32_t address;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME, cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    address = iface->get_bp_addr_by_id(core, id);
    if (0 != iface->delete_bp(core, address))
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call %s delete_bp iface Failed!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye删除断点-通过地址
SkyEyeAPIRetST skyeye_delete_breakpoint_by_addr(char *cpuname, uint32_t address)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", cpuname, CORE_BREAKPOINT_OP_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    if (0 != iface->delete_bp(core, address))
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call %s delete_bp iface Failed!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye获取断点地址-通过ID
SkyEyeAPIRetST skyeye_get_breakpoint_address_by_id(char *cpuname, int id)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", cpuname, CORE_BREAKPOINT_OP_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = iface->get_bp_addr_by_id(core, id);
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye获取断点数量
SkyEyeAPIRetST skyeye_get_bp_numbers(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", cpuname, CORE_BREAKPOINT_OP_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = iface->get_bp_numbers(core);
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye检查断点是否触发
SkyEyeAPIRetST skyeye_check_bp_hit(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", cpuname, CORE_BREAKPOINT_OP_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = (unsigned int)iface->check_bp_trigger(core);
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye获取触发断点ID
SkyEyeAPIRetST skyeye_get_bp_hit_id(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", cpuname, CORE_BREAKPOINT_OP_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = (unsigned int)iface->get_bp_trigger(core);
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye清除断点
SkyEyeAPIRetST skyeye_clear_bp_hit(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);
    int ret;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Warning_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", cpuname, CORE_BREAKPOINT_OP_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    iface->clear_bp_trigger(core);
    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *3.SkyEye创建目标系统各模块API接口
 */

// SkyEye实例化模块,实际返回object对象指针
SkyEyeAPIRetST skyeye_add_pre_obj(char *objname, char *classname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *conf_obj = pre_conf_obj(objname, classname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (conf_obj == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.conf_obj = conf_obj;
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye创建mach
SkyEyeAPIRetST skyeye_create_mach(char *objname, char *classname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *conf_obj = pre_conf_obj(objname, classname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (conf_obj == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Create <mach>:%s object fail.", objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    system_register_soc(conf_obj);
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye创建linker设备
SkyEyeAPIRetST skyeye_create_linker(char *objname, char *classname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *conf_obj = pre_conf_obj(objname, classname);
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (conf_obj == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Create <linker>:%s object fail.", objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    system_register_linker(conf_obj);
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye创建cpu
SkyEyeAPIRetST skyeye_create_cpu(char *machname, char *objname, char *classname)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

#ifdef __WIN32__
    //检查cpu是否授权，返回0表示失败
    if (0 == check_cpu(classname))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "The Core:%s is not in the permit! Please Check License.\n", classname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
#endif

    conf_object_t *cpu = pre_conf_obj(objname, classname);
    if (cpu == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Create <cpu>:%s object fail.\n", objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    conf_object_t *mach_obj = get_conf_obj(machname);
    if (mach_obj == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s object\n", machname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    sys_cpu_t *sys_cpu = system_register_cpu(cpu, mach_obj);

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye创建device
SkyEyeAPIRetST skyeye_create_device(char *machname, char *objname, char *classname)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    conf_object_t *mach_obj = get_conf_obj(machname);
    conf_object_t *device = pre_conf_obj(objname, classname);
    reset_conf_obj(device);

    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Create <device>:%s object fail.\n", objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    if (strcmp(classname, "ram") == 0)
    {
        conf_object_t *core;
        attr_value_t *attr_after_making;
        sys_soc_t *sys_soc = mach_obj->sys_struct;

        core = sys_soc->cpus[0].cpu;
        core_info_intf *core_info = (core_info_intf *)SKY_get_iface(core, CORE_INFO_INTF_NAME);
        endian_t endianess = core_info->get_endian(core);

        attr_after_making = make_new_attr(Val_UInteger, (void *)endianess);
        set_conf_attr(device, "endian", attr_after_making);
    }

    system_register_dev(device, mach_obj);

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye虚拟系统所有模块初始化配置接口
SkyEyeAPIRetST skyeye_config_obj()
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == system_config_conf_obj())
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call system_config_conf_obj fail!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye模块初始化配置接口
SkyEyeAPIRetST skyeye_config_module(char *objname)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    conf_object_t *module = get_conf_obj(objname);

    if (module == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Create <module>:%s object fail.\n", objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    config_conf_obj(module);

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye获取soc数量
SkyEyeAPIRetST skyeye_get_soc_num()
{
    SkyEyeAPIRetST ApiRet;
    skyeye_system_t *system = system_get();

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retIntValue = system->soc_cnt;

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye获取指定SOC上的CPU数量
SkyEyeAPIRetST skyeye_get_cpu_num_by_soc_name(char *soc_name)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *soc = get_conf_obj(soc_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (soc == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s object\n", soc_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    sys_soc_t *sys_soc = (sys_soc_t *)soc->sys_struct;
    ApiRet.retIntValue = sys_soc->cpu_cnt;

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye获取整个虚拟系统所有模块对象名称
SkyEyeAPIRetST skyeye_get_current_system_module_name()
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.list = get_current_system_module_name();
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye添加设备在内存总线上的地址映射
SkyEyeAPIRetST skyeye_add_map(char *memory_space_name, char *device_name, uint64_t address, uint32_t length)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    //返回1表示成功，0表示失败
    if (0 == WIN_memory_space_add_map(memory_space_name, device_name, address, length))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_memory_space_add_map Failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye添加设备到内存空间组
SkyEyeAPIRetST skyeye_add_map_group(char *memory_space_name, char *device_name,
                                    uint64_t address, uint32_t length, uint32_t index)
{
    SkyEyeAPIRetST ApiRet;
    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    //返回1表示成功，0表示失败
    if (0 == WIN_memory_space_add_map_group(memory_space_name, device_name, address, length, index))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_memory_space_add_map_group Failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *4.SkyEye处理器相关API接口
 */
//获取PC地址-通过处理器名称
SkyEyeAPIRetST skyeye_get_pc_by_cpuname(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_info_intf *core_info = (core_info_intf *)SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (core_info == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", cpuname, CORE_INFO_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.retIntValue = core_info->get_pc(core);
    ApiRet.result = API_OK;
    return ApiRet;
}

//步进运行
SkyEyeAPIRetST skyeye_step_run(char *cpuname, const char *arg)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);
    int steps = 0;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    if (arg == NULL || *arg == '\0')
    {
        steps = 1;
    }
    else
    {
        steps = strtoul(arg, NULL, 10);
    }

    if (SIM_get_core_mode(core) == DYNCOM_RUN)
    {
        SIM_stop(NULL);
        // TODO(wshen): can't stop the simulator thread competely
#ifdef __WIN32__
        Sleep(50000); // sleep 50s
#else
        usleep(50000);
#endif
        SIM_set_core_mode(core, DYNCOM_RUN_SINGLE_STEP);
    }

    skyeye_core_stepi(core, steps);

    ApiRet.result = API_OK;
    return ApiRet;
}

//根据地址获取反汇编信息
SkyEyeAPIRetST skyeye_disassemble_by_addr(char *cpuname, uint32_t addr)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_info_intf *core_info = (core_info_intf *)SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (core_info == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", cpuname, CORE_INFO_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    char *tmpStrValue = core_info->disassemble(core, addr);

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

//获取CPU地址宽度
SkyEyeAPIRetST skyeye_get_cpu_address_width(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_info_intf *core_info = (core_info_intf *)SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (core_info == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", cpuname, CORE_INFO_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = core_info->get_address_width(core);
    ApiRet.result = API_OK;
    return ApiRet;
}

//根据获取处理器步长
SkyEyeAPIRetST skyeye_get_cpu_steps(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_info_intf *core_info = (core_info_intf *)SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (core_info == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", cpuname, CORE_INFO_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retLongValue = core_info->get_steps(core);
    ApiRet.result = API_OK;
    return ApiRet;
}

//设置CPU运行模式
SkyEyeAPIRetST skyeye_set_cpu_run_mode(char *cpuname, int mode)
{
    SkyEyeAPIRetST ApiRet;
    attr_value_t value;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    if (!SKY_has_attribute(core, "mode"))
    {
        value = SKY_make_attr_uinteger(mode);
        SKY_set_attribute(core, "mode", &value);
        ApiRet.result = API_OK;
        return ApiRet;
    }
    else
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "cpu_name: %s not have mode attribute!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
}

//获取CPU架构名称
SkyEyeAPIRetST skyeye_get_cpu_architecture(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_info_intf *core_info = (core_info_intf *)SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (core_info == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", cpuname, CORE_INFO_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    char *tmpStrValue = core_info->get_architecture(core);
    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

//获取CPU仿真时间
SkyEyeAPIRetST skyeye_get_simulation_time(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);
    double curr_time = 0.0f;
    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;
    sys_dev_t *idle_device;
    conf_object_t *idle_obj;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    sys_cpu = core->sys_struct;
    sys_soc = sys_cpu->sys_soc;
    idle_device = sys_soc->idle_device;
    idle_obj = idle_device->dev;

    idle_api_intf_t *iface = SKY_get_iface(idle_obj, IDLE_DEV_API_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", idle_obj->objname, IDLE_DEV_API_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    curr_time = iface->get_curr_time(idle_obj);

    ApiRet.retDoubleValue = curr_time;
    ApiRet.result = API_OK;
    return ApiRet;
}

//获取CPU仿真指令数
SkyEyeAPIRetST skyeye_get_simulation_insn_num(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    core_info_intf *core_info = (core_info_intf *)SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (core_info == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", cpuname, CORE_INFO_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    uint32_t cpu_freq = 10000000; //默认是10MHZ
    uint64_t insn_num = 0;

    core_info->get_cpu_run_status(core, &cpu_freq, &insn_num);

    ApiRet.retLongValue = insn_num;
    ApiRet.result = API_OK;
    return ApiRet;
}

//获取CPU主频
SkyEyeAPIRetST skyeye_get_cpu_freq(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core;
    sys_cpu_t *sys_cpu;

    core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    sys_cpu = core->sys_struct;

    ApiRet.retIntValue = sys_cpu->freq_hz;
    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *5.SkyEye内存读写API接口
 */

// SkyEye内存读
SkyEyeAPIRetST skyeye_memory_read(char *cpuname, uint32_t addr, int count)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);
    uint32_t value = 0;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memory_space_intf *iface = (memory_space_intf *)SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", cpuname, MEMORY_SPACE_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    exception_t ret = iface->read(core, addr, &value, count);

    if (ret != No_exp)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Read addr:%x Failed!\n", addr);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.retIntValue = value;
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye内存写
SkyEyeAPIRetST skyeye_memory_write(char *cpuname, uint32_t addr, char *val, int count)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);
    uint32_t value = 0;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memory_space_intf *iface = (memory_space_intf *)SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", cpuname, MEMORY_SPACE_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    exception_t ret = iface->write(core, addr, val, count);
    if (ret != No_exp)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Write addr:%x Failed!\n", addr);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye设备读
SkyEyeAPIRetST skyeye_device_read(char *device_name, uint32_t offset, int count)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(device_name);
    uint32_t data = 0;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get device_name: %s object!\n", device_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memory_space_intf *iface = (memory_space_intf *)SKY_get_iface(device, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", device_name, MEMORY_SPACE_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    exception_t ret = iface->read(device, offset, &data, count);
    if (ret != No_exp)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Read offset:%x Failed!\n", offset);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = data;
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye设备写
SkyEyeAPIRetST skyeye_device_write(char *device_name, uint32_t offset, char *buf, int count)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(device_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get device_name: %s object!\n", device_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memory_space_intf *iface = (memory_space_intf *)SKY_get_iface(device, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", device_name, MEMORY_SPACE_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    exception_t ret = iface->write(device, offset, buf, count);
    if (ret != No_exp)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Write offset:%x Failed!\n", offset);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye激励设备读
SkyEyeAPIRetST skyeye_inject_device_read(char *device_name, char *data)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(device_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get device_name: %s object!\n", device_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    data_inject_intf *iface = (data_inject_intf *)SKY_get_iface(device, DATA_INJECT_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", device_name, DATA_INJECT_INTF);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = iface->read(device, data);
    ApiRet.result = API_OK;
    return ApiRet;
}

// SkyEye激励设备写
SkyEyeAPIRetST skyeye_inject_device_write(char *device_name, char *data, int size)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(device_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get device_name: %s object!\n", device_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    data_inject_intf *iface = (data_inject_intf *)SKY_get_iface(device, DATA_INJECT_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface\n", device_name, DATA_INJECT_INTF);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    iface->write(device, data, size);
    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *6.SkyEye其他API接口
 */

SkyEyeAPIRetST skyeye_set_min_syn_time(double s, double delay)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    system_set_min_syn_time(s, delay);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_set_attr(char *objname, char *key, char *attr_type, char *value)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_set_attr(objname, key, attr_type, value))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_set_attr failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_connect_device(char *con_objname, char *iface_objname, char *name, uint32_t index)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_connect(con_objname, iface_objname, name, index))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_connect failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_class_list(void)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.list = get_class_list();
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_module_type(char *objname)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *obj = get_conf_obj(objname);

    if (obj == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get module_name: %s object!\n", objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = get_class_type(obj->class_name);

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_class_type(char *classname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = get_class_type(classname);
    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_interface_list(char *classname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.list = get_interface_list(classname);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_connect_list(char *classname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.list = get_connect_list(classname);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_attr_list(char *classname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.list = get_attr_list(classname);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_attr_info(char *classname, const char *attrname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.list = get_attr_info(classname, attrname);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_module_names(void)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = WIN_get_module_names();

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_device_module_type(char *classname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retIntValue = get_device_module_type(classname);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_module_get_value_by_name(char *modulename, char *key)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = SKY_module_get_value_by_name(modulename, key);

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_module_get_path_by_name(char *modulename)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = SKY_module_get_path_by_name(modulename);

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_load_module_fromdir(char *dir)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    SKY_load_all_modules(dir, NULL);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_next_logMsg()
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = skyeye_get_next_log();

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_list_dir(char *arg)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retIntValue = com_list(arg);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_list_modules(char *arg)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retIntValue = com_list_modules(arg);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_mm_info(char *args)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    mm_info_cmd(args);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_license_verify()
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retIntValue = license_verify();

    if (ApiRet.retIntValue != 3)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "License Check failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_read_license_info(char *filename)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = read_lic_info(filename);

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
        ApiRet.result = API_OK;
    }
    else
    {
        ApiRet.result = API_ERROR;
    }
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_cpuid()
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = get_cpuid();

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
        ApiRet.result = API_OK;
    }
    else
    {
        ApiRet.result = API_ERROR;
    }
    return ApiRet;
}

SkyEyeAPIRetST skyeye_check_usbKey_connect()
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == check_usb_lic_connect())
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "usbKey connect failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_set_script_path(char *path)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (-1 == WIN_set_script_path(path))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_set_script_path failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_work_full_path()
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = WIN_get_work_full_path();

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_work_path()
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = WIN_get_work_path();

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *7.SkyEye数据激励功能API接口
 */

SkyEyeAPIRetST skyeye_parse_symbol(char *cpuname, char *hex_file_name)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    parse_symbol_xml(cpuname, hex_file_name);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_debug_symbol()
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    parse_xml_output();

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_symbol_addr(char *cpu_name, char *sym_str)
{
    SkyEyeAPIRetST ApiRet;
    uint32_t global_addr = 0;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (SYM_OK != SkyEye_GetSymbol_Addr(cpu_name, sym_str, &global_addr))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Get Symobl:%s Addr Failed!\n", sym_str);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = global_addr;
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_symbol_value(char *cpu_name, char *sym_str, sym_type_t count)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retLongValue = SkyEye_GetSymbol_Value_By_Py(cpu_name, sym_str, count);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_float_symbol_value(char *cpu_name, char *sym_strvoid, sym_type_t count)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retFloatValue = SkyEye_GetFloatSymbol_Value(cpu_name, sym_strvoid, count);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_double_symbol_value(char *cpu_name, char *sym_strvoid, sym_type_t count)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retDoubleValue = SkyEye_GetDoubleSymbol_Value(cpu_name, sym_strvoid, count);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_set_symbol_value(char *cpu_name, char *sym_str, uint64_t value, sym_type_t count)
{
    SkyEyeAPIRetST ApiRet;
    int result = 0;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    result = SkyEye_SetSymbol_Value_By_Py(cpu_name, sym_str, value, count);

    if (SYM_OK != result)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Set Symobl:%s Value Failed!\n", sym_str);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = result;
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_set_float_symbol_value(char *cpu_name, char *sym_str, float value, sym_type_t count)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    int result = SkyEye_SetSymbol_Value(cpu_name, sym_str, &value, count);

    if (SYM_OK != result)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Set Float Symobl:%s Value Failed!\n", sym_str);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = result;
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_set_double_symbol_value(char *cpu_name, char *sym_str, double value, sym_type_t count)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    int result = SkyEye_SetSymbol_Value(cpu_name, sym_str, &value, count);

    if (SYM_OK != result)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Set Float Symobl:%s Value Failed!\n", sym_str);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = result;
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_func_addr(char *cpu_name, char *func_str)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retIntValue = SkyEye_GetFunc_Addr(cpu_name, func_str);

    if (ApiRet.retIntValue == 0)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Get Function Addr:%s Failed!\n", func_str);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_func_length(char *cpu_name, char *func_str)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    ApiRet.retIntValue = SkyEye_GetFunc_Length(cpu_name, func_str);

    if (ApiRet.retIntValue == 0)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Get Function Length:%s Failed!\n", func_str);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *8.SkyEye寄存器API接口
 */

SkyEyeAPIRetST skyeye_get_register_num(char *devicename)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(devicename);
    uint32_t reg_num;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   " Can't get device_name: %s object!\n", devicename);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    // First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(device, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        // if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(device, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                       "Can't get %s %s iface\n", devicename, SKYEYE_REG_INTF);
            ApiRet.result = API_ERROR;
            return ApiRet;
        }
        reg_num = iface->get_regnum(device);
    }
    else
    {
        reg_num = iface64->get_regnum(device);
    }

    ApiRet.retIntValue = reg_num;
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_regvalue_by_id(char *devicename, uint32_t id)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(devicename);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   " Can't get device_name: %s object!\n", devicename);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    // First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(device, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        // if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(device, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                       "Can't get %s %s iface\n", devicename, SKYEYE_REG_INTF);
            ApiRet.result = API_ERROR;
            return ApiRet;
        }
        ApiRet.retLongValue = iface->get_regvalue_by_id(device, id);
    }
    else
    {
        ApiRet.retLongValue = iface64->get_regvalue_by_id(device, id);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_regname_by_id(char *devicename, uint32_t id)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(devicename);
    char *tmpStrValue;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   " Can't get device_name: %s object!\n", devicename);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    // First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(device, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        // if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(device, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                       "Can't get %s %s iface\n", devicename, SKYEYE_REG_INTF);
            ApiRet.result = API_ERROR;
            return ApiRet;
        }
        tmpStrValue = iface->get_regname_by_id(device, id);
    }
    else
    {
        tmpStrValue = iface64->get_regname_by_id(device, id);
    }

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_regoffset_by_id(char *devicename, uint32_t id)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(devicename);
    char *tmpStrValue;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   " Can't get device_name: %s object!\n", devicename);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    // First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(device, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        // if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(device, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                       "Can't get %s %s iface\n", devicename, SKYEYE_REG_INTF);
            ApiRet.result = API_ERROR;
            return ApiRet;
        }

        if (iface->get_regoffset_by_id == NULL)
        {
            ApiRet.retIntValue = 0xffffffff;
            ApiRet.result = API_OK;
            return ApiRet;
        }
        ApiRet.retIntValue = iface->get_regoffset_by_id(device, id);
    }
    else
    {
        if (iface64->get_regoffset_by_id == NULL)
        {
            ApiRet.retIntValue = 0xffffffff;
            ApiRet.result = API_OK;
            return ApiRet;
        }

        ApiRet.retIntValue = iface64->get_regoffset_by_id(device, id);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_set_regvalue_by_id(char *devicename, uint64_t value, uint32_t id)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(devicename);
    char *tmpStrValue;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   " Can't get device_name: %s object!\n", devicename);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    // First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(device, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        // if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(device, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                       "Can't get %s %s iface\n", devicename, SKYEYE_REG_INTF);
            ApiRet.result = API_ERROR;
            return ApiRet;
        }

        iface->set_regvalue_by_id(device, (uint32_t)value, id);
    }
    else
    {
        iface64->set_regvalue_by_id(device, value, id);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_regid_by_name(char *devicename, char *name)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *device = get_conf_obj(devicename);
    char *tmpStrValue;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (device == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   " Can't get device_name: %s object!\n", devicename);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    // First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(device, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        // if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(device, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                       "Can't get %s %s iface\n", devicename, SKYEYE_REG_INTF);
            ApiRet.result = API_ERROR;
            return ApiRet;
        }

        ApiRet.retIntValue = iface->get_regid_by_name(device, name);
    }
    else
    {
        ApiRet.retIntValue = iface64->get_regid_by_name(device, name);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *9.SkyEye GDB调试功能API接口
 */

SkyEyeAPIRetST skyeye_create_remote_gdb(char *cpuname, int port, char *ip)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_create_remote_gdb(cpuname, port, ip))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_create_remote_gdb iface Failed\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_delete_remote_gdb(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_delete_remote_gdb(cpuname))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_delete_remote_gdb iface Failed\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_remote_gdb_check_link(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_remote_gdb_check_link(cpuname))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_remote_gdb_check_link iface Failed\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = 1;
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_remote_gdb_get_client_ip(char *cpuname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = WIN_remote_gdb_get_client_ip(cpuname);
    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}
/*
 *10.SkyEye加载功能API接口
 */

SkyEyeAPIRetST skyeye_load_file(char *cpuname, const char *filename, generic_address_t load_addr)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);
    int ret, errCode;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ret = load_file(core, filename, load_addr);
    if (ret == File_open_exp)
    {
        // error_message_data.json define errCode
        output_msg(Warning_log, 0x90000001, ApiRet.errMsg, OUTPUT_2_PYTHON, "%s", filename);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    else if (ret == Not_found_exp)
    {
        output_msg(Warning_log, 0x90000002, ApiRet.errMsg, OUTPUT_2_PYTHON, "%s %s", cpuname, MEMORY_SPACE_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    else if (ret == Excess_range_exp)
    {
        output_msg(Warning_log, 0x90000003, ApiRet.errMsg, OUTPUT_2_PYTHON, " ");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_load_binary(char *cpuname, char *elfname)
{
    SkyEyeAPIRetST ApiRet;
    generic_address_t start_addr, code_start_addr, code_end_addr;
    conf_object_t *core = get_conf_obj(cpuname);
    int errCode;
    exception_t exp;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    if (access(elfname, F_OK))
    {
        output_msg(Warning_log, 0x90000001, ApiRet.errMsg, OUTPUT_2_PYTHON, "%s", elfname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    if (is_elf(elfname))
    {
        exp = get_elf_start_addr(elfname, &start_addr, &code_start_addr, &code_end_addr);
    }
    else if (is_coff(elfname))
    {
        exp = get_coff_start_addr(elfname, &start_addr, &code_start_addr, &code_end_addr);
    }
    else if (is_hex(elfname))
    {
        exp = get_hex_start_addr(elfname, &start_addr, &code_start_addr, &code_end_addr);
    }
    else
    {
        exp = Invarg_exp;
    }

    if (exp != No_exp)
    {
        output_msg(Error_log, 0x90000004, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Get Binary Start Address Failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    if (!SKY_has_attribute(core, "load_mask"))
    {
        attr_value_t attr = SKY_get_attribute(core, "load_mask");
        uint32_t load_mask = 0;

        load_mask = SKY_attr_uinteger(attr);
        if (load_mask != 0)
        {
            start_addr = (start_addr & load_mask);
        }
    }

    if (!SKY_has_attribute(core, "load_addr"))
    {
        attr_value_t attr = SKY_get_attribute(core, "load_addr");
        uint32_t load_addr = 0;

        load_addr = SKY_attr_uinteger(attr);
        if (load_addr != 0)
        {
            start_addr = start_addr | load_addr;
        }
    }

    set_entry_to_cpu(core, start_addr);
    exp = SKY_load_file(core, NULL, elfname);

    if (exp != No_exp)
    {
        output_msg(Error_log, 0x90000005, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Load binary File Failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    SIM_set_pc(core, start_addr, code_start_addr, code_end_addr);

    sys_cpu_t *cpu = (sys_cpu_t *)(core->sys_struct);
    sys_soc_t *soc = cpu->sys_soc;

    sprintf(soc->binary_path, "%s", elfname);
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_load_bin_binary(char *cpuname, const char *filename, generic_address_t load_addr, int length, int start_pc)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *core = get_conf_obj(cpuname);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (core == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get cpu_name: %s object!\n", cpuname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet = skyeye_load_file(cpuname, filename, load_addr);
    if (ApiRet.result == API_OK)
    {
        int code_start_addr = load_addr;
        int code_end_addr = load_addr + length;

        SIM_set_pc(core, start_pc, code_start_addr, code_end_addr);
    }

    return ApiRet;
}

SkyEyeAPIRetST skyeye_new_load_file(char *memoryspace_name, const char *filename, generic_address_t load_addr)
{
    SkyEyeAPIRetST ApiRet;
    int ret, errCode, nread = 0;
    uint8_t data;
    FILE *f;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    conf_object_t *memory_space = get_conf_obj(memoryspace_name);

    if (memory_space == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get memory_space name: %s object!\n", memoryspace_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memory_space_intf *iface = (memory_space_intf *)SKY_get_iface(memory_space, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Get memory_space interface from %s error!!!\n", memory_space->objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    f = fopen(filename, "rb");
    if (f == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can not open file %s.\n", filename);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    /* read a char and write it to the memory */
    while (nread = fread(&data, 1, 1, f))
    {
        ret = iface->write(memory_space, load_addr, &data, 1);
        if (ret != No_exp)
        {
            /* error handler for address error */
            fclose(f);
            ApiRet.result = API_ERROR;
            return ApiRet;
        }
        load_addr++;
    }
    skyeye_log(Info_log, __FUNCTION__, "Load the file %s to the memory 0x%x\n", filename, load_addr);
    fclose(f);

    ApiRet.result = API_OK;
    return ApiRet;
}
/*
 *11.SkyEye故障注入功能API接口
 */

SkyEyeAPIRetST skyeye_set_fault(char *memory_space_name, uint32_t addr, uint32_t bit, uint32_t mode, uint32_t count)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *memory_space = get_conf_obj(memory_space_name);
    exception_t ret;
    uint32_t buf = 0;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (memory_space == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get memory_space_name: %s object!\n", memory_space_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memory_space_set_plug_intf *plug_iface = (memory_space_set_plug_intf *)SKY_get_iface(memory_space, MEMORY_SPACE_SET_PLUG_INTF_NAME);

    if (plug_iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", memory_space_name, MEMORY_SPACE_SET_PLUG_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ret = plug_iface->set_plug(memory_space, addr, bit, mode);

    memory_space_intf *iface = (memory_space_intf *)SKY_get_iface(memory_space, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", memory_space_name, MEMORY_SPACE_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ret = iface->read(memory_space, addr, &buf, count);
    if (ret != No_exp)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Read address:0x%x failed!\n", addr);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    if (mode == 0)
    {
        buf &= ~(0x1 << bit); // set bit : 0
    }
    else if (mode == 1)
    {
        buf |= (0x1 << bit); // set bit : 1
    }
    else if (mode == 2)
    { // reverse bit :if 0: set 1; if 1: set 0
        if (buf & (0x1 << bit))
        {
            buf &= ~(0x1 << bit);
        }
        else
        {
            buf |= (0x1 << bit);
        }
    }
    else
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can not set mode %d.\n", mode);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ret = iface->write(memory_space, addr, &buf, count);
    if (ret != No_exp)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Write address:0x%x failed!\n", addr);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_fault(char *memory_space_name, uint32_t addr)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    exception_t ret;
    conf_object_t *memory_space = get_conf_obj(memory_space_name);

    if (memory_space == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get memory_space_name: %s object!\n", memory_space_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memory_space_set_plug_intf *iface =
        (memory_space_set_plug_intf *)SKY_get_iface(memory_space, MEMORY_SPACE_SET_PLUG_INTF_NAME);
    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", memory_space_name, MEMORY_SPACE_SET_PLUG_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.fault_inject = iface->get_plug(memory_space, addr);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_clear_fault(char *memory_space_name, uint32_t addr, uint32_t bit, uint32_t mode)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *memory_space = get_conf_obj(memory_space_name);
    exception_t ret;
    uint32_t buf = 0;
    int errCode;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (memory_space == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get memory_space_name: %s object!\n", memory_space_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memory_space_set_plug_intf *plug_iface = (memory_space_set_plug_intf *)SKY_get_iface(memory_space, MEMORY_SPACE_SET_PLUG_INTF_NAME);

    if (plug_iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", memory_space_name, MEMORY_SPACE_SET_PLUG_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ret = plug_iface->clear_plug(memory_space, addr, bit, mode);

    if (ret != No_exp)
    {
        output_msg(Error_log, 0x90000006, ApiRet.errMsg, OUTPUT_2_LOG_PY, "clear_plug failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    memory_space_intf *iface = (memory_space_intf *)SKY_get_iface(memory_space, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", memory_space_name, MEMORY_SPACE_INTF_NAME);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ret = iface->read(memory_space, addr, &buf, 4);
    if (ret != No_exp)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Read address:0x%x failed!\n", addr);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    if (mode == 0)
    {
        buf |= (1 << bit);
    }
    else if (mode == 1)
    {
        buf &= ~(1 << bit);
        if ((buf >> 31) & 0x1)
            buf = 0;
    }
    else if (mode == 2)
    { // reverse bit :if 0: set 1; if 1: set 0
        if (buf & (0x1 << bit))
        {
            buf &= ~(0x1 << bit);
        }
        else
        {
            buf |= (0x1 << bit);
        }
    }
    else
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can not set mode %d.\n", mode);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ret = iface->write(memory_space, addr, &buf, 4);
    if (ret != No_exp)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Write address:0x%x failed!\n", addr);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *12.SkyEye代码覆盖率功能API接口
 */

SkyEyeAPIRetST skyeye_get_executed_pc_file(char *instr_process_name, char *filename)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (instr_process == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get instr_process_name: %s object!\n", instr_process_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    instr_process_intf *iface = (instr_process_intf *)SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", instr_process_name, INSTR_PROCESS_INTF);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    iface->get_dif_pc_cache(instr_process, filename);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_instr_process_device(char *machname, char *cpuname, char *classname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = get_instr_process_device(machname, cpuname, classname);

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_enable_cpu_codecov(char *machname)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_enable_cpu_codecov(machname))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_enable_cpu_codecov iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *13.SkyEye指令流记录功能API接口
 */

SkyEyeAPIRetST skyeye_get_pc_record_size(char *instr_process_name)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (instr_process == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get instr_process_name: %s object!\n", instr_process_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    instr_process_intf *iface = (instr_process_intf *)SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", instr_process_name, INSTR_PROCESS_INTF);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = iface->get_pc_record_size(instr_process);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_set_pc_record_size(char *instr_process_name, int size)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (instr_process == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get instr_process_name: %s object!\n", instr_process_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    instr_process_intf *iface = (instr_process_intf *)SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", instr_process_name, INSTR_PROCESS_INTF);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    iface->set_pc_record_size(instr_process, size);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_pc_record_nums(char *instr_process_name)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (instr_process == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get instr_process_name: %s object!\n", instr_process_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    instr_process_intf *iface = (instr_process_intf *)SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", instr_process_name, INSTR_PROCESS_INTF);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = iface->get_pc_record_nums(instr_process);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_pc_record_index(char *instr_process_name)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (instr_process == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get instr_process_name: %s object!\n", instr_process_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    instr_process_intf *iface = (instr_process_intf *)SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", instr_process_name, INSTR_PROCESS_INTF);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = iface->get_pc_record_index(instr_process);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_pc_record_overflow(char *instr_process_name)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (instr_process == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get instr_process_name: %s object!\n", instr_process_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    instr_process_intf *iface = (instr_process_intf *)SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", instr_process_name, INSTR_PROCESS_INTF);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = iface->get_pc_record_overflow(instr_process);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_pc_by_index(char *instr_process_name, int id)
{
    SkyEyeAPIRetST ApiRet;
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (instr_process == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get instr_process_name: %s object!\n", instr_process_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    instr_process_intf *iface = (instr_process_intf *)SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s %s iface!\n", instr_process_name, INSTR_PROCESS_INTF);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.retIntValue = iface->get_pc_by_index(instr_process, id);

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_open_instr_record(char *cpu_name, char *filename)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_open_instr_record(cpu_name, filename))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_open_instr_record iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_close_instr_record(char *cpu_name)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_close_instr_record(cpu_name))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_close_instr_record iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *14.SkyEye异常触发功能API接口
 */

SkyEyeAPIRetST skyeye_get_cpu_exception_num(char *cpu_name)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    ApiRet.retIntValue = SkyEye_GetCpuExceptionNum(cpu_name);

    if (0 == ApiRet.retIntValue)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call SkyEye_GetCpuExceptionNum iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_set_cpu_exception(char *cpu_name, int exception_id)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (No_exp != SkyEye_SetCpuException(cpu_name, exception_id))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call SkyEye_SetCpuException iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_set_ext_interrupt(char *intc_name, int interrupt_num)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (No_exp != SkyEye_SetExtInterrupt(intc_name, interrupt_num))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call SkyEye_SetExtInterrupt iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_cpu_exception_name_by_id(char *cpu_name, int exception_id)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    char *tmpStrValue = SkyEye_GetCpuExceptionNameById(cpu_name, exception_id);

    if (tmpStrValue != NULL)
    {
        strcpy(ApiRet.retStrValue, tmpStrValue);
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *15.SkyEye使能和禁用设备API接口
 */

SkyEyeAPIRetST skyeye_disable_device_work(char *device_name)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_disable_device_work(device_name))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_disable_device_work iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_enable_device_work(char *device_name)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0 == WIN_enable_device_work(device_name))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_enable_device_work iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

/*
 *16.设置和获取CPU PC API接口
 */

SkyEyeAPIRetST skyeye_set_pc(char *cpu_name, unsigned int pc)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    if (0xffffffff == WIN_setPC(cpu_name, pc))
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_setPC iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_get_pc(char *cpu_name)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    ApiRet.retIntValue = WIN_getPC(cpu_name);

    if (0xffffffff == ApiRet.retIntValue)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Call WIN_getPC iface failed!\n");
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    ApiRet.result = API_OK;
    return ApiRet;
}

SkyEyeAPIRetST skyeye_enable_debug(char *soc_name)
{
    SkyEyeAPIRetST ApiRet;
    sys_cpu_t *sys_cpu;
    sys_dev_t *sys_dev;
    conf_object_t *gdbserver_obj;
    char objname[MAX_OBJNAME];
    int i, j;

    skyeye_system_t *system = system_get();
    conf_object_t *soc = get_conf_obj(soc_name);

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));
    if (soc == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can't get %s object\n", soc_name);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    sys_soc_t *sys_soc = (sys_soc_t *)soc->sys_struct;
    for (i = 0; i < sys_soc->cpu_cnt; i++)
    {
        sys_cpu = &(sys_soc->cpus[i]);
        skyeye_debug_intf *iface = SKY_get_iface(sys_cpu->cpu, SKYEYE_DEBUG_INTF);
        if (iface)
        {
            iface->set_skyeye_debug(sys_cpu->cpu, 1); //使能Debug
            skyeye_log(Info_log, __FUNCTION__, "enable cpuname:%s debug\n", sys_cpu->cpu->objname);
        }
        get_strcat_objname(objname, sys_cpu->cpu->objname, "_gdbserver");
        gdbserver_obj = get_conf_obj(objname);
        if (gdbserver_obj)
        {
            skyeye_debug_intf *iface = SKY_get_iface(gdbserver_obj, SKYEYE_DEBUG_INTF);
            if (iface)
            {
                iface->set_skyeye_debug(gdbserver_obj, 1); //使能Debug
                skyeye_log(Info_log, __FUNCTION__, "enable %s debug\n", objname);
            }
        }
    }

    for (i = 0; i < sys_soc->dev_cnt; i++)
    {
        sys_dev = sys_soc->devs + i;
        skyeye_debug_intf *iface = SKY_get_iface(sys_dev->dev, SKYEYE_DEBUG_INTF);
        if (iface)
        {
            iface->set_skyeye_debug(sys_dev->dev, 1); //使能Debug
            skyeye_log(Info_log, __FUNCTION__, "enable devname:%s debug\n", sys_dev->dev->objname);
        }
    }

    ApiRet.result = API_OK;
    return ApiRet;
}

/*等待Term终端输出某个字符串 API接口*/
SkyEyeAPIRetST skyeye_term_wait_for_string(char *termname, const char *string)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    conf_object_t *term = get_conf_obj(termname);

    if (term == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can not get %s object.", termname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    skyeye_uart_intf *iface = SKY_get_iface(term, SKYEYE_UART_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can not get %s interface from %s", SKYEYE_UART_INTF, term->objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }

    iface->wait_string(term, string);
    ApiRet.result = API_OK;
    return ApiRet;
}

/*向Term终端写入某个字符串 API接口*/
SkyEyeAPIRetST skyeye_term_write(char *termname, const char *string)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    conf_object_t *term = get_conf_obj(termname);

    if (term == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can not get %s object.", termname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    skyeye_uart_intf *iface = SKY_get_iface(term, SKYEYE_UART_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can not get %s interface from %s", SKYEYE_UART_INTF, term->objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    iface->write_string(term, string);
    ApiRet.result = API_OK;
    return ApiRet;
}

/*当Term终端输出某个字符串则写入某个字符串*/
SkyEyeAPIRetST skyeye_term_wait_then_write(char *termname, const char *wait_string, const char *write_string)
{
    SkyEyeAPIRetST ApiRet;

    memset(&ApiRet, 0, sizeof(SkyEyeAPIRetST));

    conf_object_t *term = get_conf_obj(termname);

    if (term == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can not get %s object.", termname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    skyeye_uart_intf *iface = SKY_get_iface(term, SKYEYE_UART_INTF);

    if (iface == NULL)
    {
        output_msg(Error_log, 0xffffffff, ApiRet.errMsg, OUTPUT_2_LOG_PY,
                   "Can not get %s interface from %s", SKYEYE_UART_INTF, term->objname);
        ApiRet.result = API_ERROR;
        return ApiRet;
    }
    iface->wait_string(term, wait_string);
    iface->write_string(term, write_string);
    ApiRet.result = API_OK;
    return ApiRet;
}
