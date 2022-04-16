/* Copyright (C) 
 * 2013 - Michael.Kang blackfin.kang@gmail.com
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
* @file default_command.c
* @brief The implementation of default command
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-10-14
*/

#include "default_command.h"

char *SetSKYEYEBIN()
{
#ifndef __WIN32__
    setenv("SKYEYEBIN", SKYEYE_BIN, 1);
#endif
    return get_bin_env_path("BIN");
}

int RunCommand(char *cmdstr)
{
    char cmdstring[256];
    char new_path[1024];
    char *skyeye_bin;

    skyeye_bin = SetSKYEYEBIN();
    Py_Initialize();

    PyRun_SimpleString("import sys\n");
    sprintf(new_path, "sys.path.append(\"%s\")\n", skyeye_bin);
    PyRun_SimpleString(new_path);
    PyRun_SimpleString("from skyeye_cli import *\n");
    sprintf(cmdstring, "cmd = cli.GetCommand(\"run-command\")\n");
    PyRun_SimpleString(cmdstring);
    sprintf(cmdstring, "cmd = cli.GetCommand(\"run-command\")\n");
    sprintf(cmdstring, "cmd.run(\"%s\")", cmdstr);
    PyRun_SimpleString(cmdstring);
    Py_Finalize();
}

int RunAutotest()
{
    char cmdstring[256];
    char new_path[1024];
    char *skyeye_bin;

    skyeye_bin = SetSKYEYEBIN();
    Py_Initialize();

    PyRun_SimpleString("import sys\n");
    sprintf(new_path, "sys.path.append(\"%s\")\n", skyeye_bin);
    PyRun_SimpleString(new_path);
    PyRun_SimpleString("from auto_test import *\n");
    PyRun_SimpleString("cmd = skyeye_auto_test()\n");
    PyRun_SimpleString("cmd.test()\n");
    Py_Finalize();
}

int python_gui(void)
{
    char new_path[1024];
    char *env = SetSKYEYEBIN();

    Py_Initialize();
    sprintf(new_path, "sys.path.append(\"%s\")\n", env);
    PyRun_SimpleString("import sys\n");
    PyRun_SimpleString(new_path);
    PyRun_SimpleString("import cli_init\ncli_init.InitCommandList()\n");
    PyRun_SimpleString("from skyeye_gui import *\nmain_control()\n");
    Py_Finalize();
}

int python_cli()
{
    char new_path[1024];
    char *env = SetSKYEYEBIN();

    Py_Initialize();
    sprintf(new_path, "sys.path.append(\"%s\")\n", env);
    PyRun_SimpleString("import sys\n");
    PyRun_SimpleString(new_path);
    PyRun_SimpleString("from skyeye_cli import *\nSkyEyeCli([\"-gui\"]).cmdloop()\n");
    Py_Finalize();
}

char *get_bin_env_path(char *path_name)
{
    char *path = getenv("CONFIG_PATH");
    char *name = "\\config.txt";
    char config_path[1024] = { 0 };
    char str[100][1024] = { 0 };
    char ret[1024] = { "SKYEYEBIN" };
    FILE *fp;
    int i, j, count = 0;
    int len = strlen(path_name);

    if (path != NULL)
    {
        strcat(config_path, path);
        strcat(config_path, name);
        fp = fopen(config_path, "r");
        while (fgets(str[count], 1024, fp) != NULL)
        {
            count++;
        }
        for (i = 0; i < count; i++)
        {
            if (strncmp(str[i], path_name, strlen(path_name)) == 0 && str[i][len] == ':')
            {
                memset(ret, 0, sizeof (ret));
                for (j = len + 3; j < strlen(str[i]); j++)
                {
                    if (str[i][j] != '\n')
                    {
                        ret[j - len - 3] = str[i][j];
                    }
                }
            }
        }
        fclose(fp);
    }

    return getenv(ret);
}

char *get_ws_path()
{
    char new_path[1024] = { '\0' };
    char info[1024] = { '\0' };
    char path_info[1024] = { '\0' };
    char *env = SetSKYEYEBIN();
    char *ptr = path_info;
    FILE *fp;

    strcat(new_path, env);
    strcat(new_path, "/workspace.cfg");
    fp = fopen(new_path, "r");

    while (fgets(info, 1024, fp) != NULL)
    {
        if (strstr(info, "lastworkspace") != NULL)
        {
            if (info[strlen(info) - 1] == '\n')
            {
                info[strlen(info) - 1] = '\0';
            }
            strcpy(path_info, info + 16);
            break;
        }
    }
    fclose(fp);
    return ptr;
}

/*This function is allow WIN to call */
uint32 WIN_get_memory_value_by_cpuname(char *cpuname, char *addr)
{
    uint32 value, addr_value;
    exception_t ret;

    if (addr == NULL || addr == '\0')
    {
        return 0;
    } else
    {
        addr_value = strtoul(addr, NULL, 0);
    }

    conf_object_t *core = get_conf_obj(cpuname);
    addr_space_t *space = get_space_from_attr(core);

    if (space != NULL)
    {
        if (space->memory_space != NULL)
        {
            ret = space->memory_space->read(core, addr_value, &value, 4);
        } else
        {
            skyeye_log(Warning_log, __FUNCTION__, "Cant't get addr_space memory_space iface from %s\n", cpuname);
            return 0;
        }
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Cant't get addr_space attribute from %s\n", cpuname);
        return 0;
    }

    return value;
}

uint32_t WIN_get_pc_by_cpuname(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);

    return SIM_get_pc(core);
}

int WIN_stepi(char *cpuname, const char *arg)
{
    conf_object_t *core = get_conf_obj(cpuname);
    int steps, ret = 0;

    if (core == NULL)
    {
        return 0;
    }

    if (arg == NULL || *arg == '\0')
    {
        steps = 1;
    } else
    {
        steps = strtoul(arg, NULL, 10);
    }

    if (SIM_get_core_mode(core) == DYNCOM_RUN)
    {
        SIM_stop(NULL);
        //TODO(wshen): can't stop the simulator thread competely
#ifdef __WIN32__
        Sleep(50000);                   //sleep 50s
#else
        usleep(50000);
#endif
        SIM_set_core_mode(core, DYNCOM_RUN_SINGLE_STEP);
        printf("In %s, %s:Waiting for the quiescence of the System!\n", __func__, DYNCOM_RUN_SINGLE_STEP);
    }

    skyeye_core_stepi(core, steps);
    return ret;
}

int WIN_create_breakpoint(char *cpuname, uint32_t addr)
{
    conf_object_t *core = get_conf_obj(cpuname);
    int ret;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "can not get object from %s\n", cpuname);
        return Error;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME,
                   cpuname);
        return Error;
    }
    ret = iface->insert_bp(core, addr);

    if (ret != 0)
    {
        return Error;
    }

    return Ok;
}

int WIN_delete_breakpoint_by_id(char *cpuname, int id)
{
    conf_object_t *core = get_conf_obj(cpuname);
    uint32_t address;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "can not get object from %s\n", cpuname);
        return Error;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME,
                   cpuname);
        return Error;
    }

    address = iface->get_bp_addr_by_id(core, id);
    iface->delete_bp(core, address);

    return Ok;
}

int WIN_delete_breakpoint_by_addr(char *cpuname, uint32_t address)
{

    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "can not get object from %s\n", cpuname);
        return Error;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME,
                   cpuname);
        return Error;
    }

    iface->delete_bp(core, address);

    return Ok;
}

uint32_t WIN_get_breakpoint_address_by_id(char *cpuname, int id)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "can not get object from %s\n", cpuname);
        return Error;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME,
                   cpuname);
        return Error;
    }

    return iface->get_bp_addr_by_id(core, id);
}

int WIN_get_bp_numbers(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "can not get object from %s\n", cpuname);
        return Error;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME,
                   cpuname);
        return Error;
    }

    return iface->get_bp_numbers(core);
}

int WIN_check_bp_hit(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "can not get object from %s\n", cpuname);
        return Error;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME,
                   cpuname);
        return Error;
    }

    return iface->check_bp_trigger(core);
}

int WIN_get_bp_hit_id(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "can not get object from %s\n", cpuname);
        return Error;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME,
                   cpuname);
        return Error;
    }

    return iface->get_bp_trigger(core);
}

int WIN_clear_bp_hit(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);
    int ret;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "can not get object from %s\n", cpuname);
        return Error;
    }

    core_breakpoint_op_intf *iface = SKY_get_iface(core, CORE_BREAKPOINT_OP_INTF_NAME);

    if (!iface)
    {
        skyeye_log(Warning_log, __FUNCTION__, "can not get %s interface from %s\n", CORE_BREAKPOINT_OP_INTF_NAME,
                   cpuname);
        return Error;
    }

    ret = iface->clear_bp_trigger(core);
    if (ret != 0)
    {
        return Error;
    }

    return Ok;
}

int WIN_create_mach(char *objname, char *classname)
{
    skyeye_config_t *config = get_current_config();

    conf_object_t *conf_obj = pre_conf_obj(objname, classname);

    if (conf_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Create %s object fail.", objname);
        return Error;
    }

    system_register_soc(conf_obj);
    return Ok;
}

int WIN_create_linker(char *objname, char *classname)
{
    conf_object_t *conf_obj = pre_conf_obj(objname, classname);

    if (conf_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Create %s object fail.", objname);
        return Error;
    }

    system_register_linker(conf_obj);
    return Ok;
}

int WIN_create_cpu(char *machname, char *objname, char *classname)
{
#ifdef __WIN32__
    if (strstr(allow_cpu_list, classname) == NULL)
    {
        printf("The core is not in the permit\n");
        skyeye_log(Error_log, __FUNCTION__, "The %s is not in the permit.\n", objname);
        return Error;                   /* FIXME, error handler */
    }
#endif
    conf_object_t *cpu = pre_conf_obj(objname, classname);

    if (cpu == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", objname);
        return Error;                   /* FIXME, error handler */
    }

    core_update_memiface_cache *iface = SKY_get_iface(cpu, CORE_UPDATE_MEMIFACE_CACHE_NAME);

    if (iface != NULL)
    {
        iface->update_memiface_cache(cpu);
    }

    exception_t ret = reset_conf_obj(cpu);
    conf_object_t *mach_obj = get_conf_obj(machname);
    skyeye_machine_intf *mach_iface = (skyeye_machine_intf *) SKY_get_iface(mach_obj, MACHINE_INTF_NAME);

    addr_space_t *space = get_space_from_attr(cpu);

    if (space)
    {
        mach_iface->set_space(mach_obj, space);
        mach_iface->set_core(mach_obj, cpu);
    }
    sys_cpu_t *sys_cpu = system_register_cpu(cpu, mach_obj);

    return Ok;
}

int WIN_create_device(char *machname, char *objname, char *classname)
{
    skyeye_config_t *config = get_current_config();
    conf_object_t *mach_obj = get_conf_obj(machname);
    conf_object_t *device = pre_conf_obj(objname, classname);

    reset_conf_obj(device);
    conf_object_t *core;

    if (strcmp(classname, "ram") == 0)
    {
        attr_value_t *attr_after_making;
        sys_soc_t *sys_soc = mach_obj->sys_struct;

        core = sys_soc->cpus[0].cpu;
        core_info_intf *core_info = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);
        endian_t endianess = core_info->get_endian(core);

        attr_after_making = make_new_attr(Val_UInteger, (void *) endianess);
        set_conf_attr(device, "endian", attr_after_making);
    }

    system_register_dev(device, mach_obj);

    return Ok;
}

void *WIN_pre_conf_obj(char *objname, char *classname)
{
    conf_object_t *conf_obj = pre_conf_obj(objname, classname);

    if (conf_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", objname);
        return NULL;
    }

    return conf_obj;
}

int WIN_set_attr(char *objname, char *key, char *attr_type, char *value)
{
    skyeye_config_t *config = get_current_config();
    attr_value_t *attr_after_making = skyeye_mm_zero(sizeof (attr_value_t));
    uint32_t attr_integer, base;
    char parity_val;

    if (!strncmp(attr_type, "string", strlen("string")))
    {
        attr_after_making = make_new_attr(Val_String, value);
        if (!strncmp(key, "parity", strlen("parity")))
        {
            attr_after_making = make_new_attr(Val_Integer, value[0]);
        }
        if (!strncmp(key, "elf", strlen("elf")))
        {
            conf_object_t *soc = get_conf_obj(objname);

            register_soc_elfname(soc, value);
            return 0;
        }
        if (!strncmp(key, "brate", strlen("brate")))
        {
            return 0;
        }
    } else if (!strncmp(attr_type, "uinteger", strlen("uinteger")))
    {
        if (!strncmp(value, "0x", 2))
        {
            base = 16;
        } else
        {
            base = 10;
        }
        if (!strncmp(key, "brate", strlen("brate")))
        {
            return 0;
        }
        attr_integer = strtoll(value, NULL, base);
        attr_after_making = make_new_attr(Val_UInteger, (void *) attr_integer);
    } else if (!strncmp(attr_type, "integer", strlen("integer")))
    {
    } else if (!strncmp(attr_type, "floating", strlen("floating")))
    {
        attr_after_making = make_new_attr(Val_Floating, value);
    } else if (!strncmp(attr_type, "list", strlen("list")))
    {
    } else if (!strncmp(attr_type, "data", strlen("data")))
    {
    } else if (!strncmp(attr_type, "nil", strlen("nil")))
    {
    } else if (!strncmp(attr_type, "object", strlen("object")))
    {
        conf_object_t *obj = get_conf_obj(value);

        attr_after_making = make_new_attr(Val_Object, obj);
    } else if (!strncmp(attr_type, "dict", strlen("dict")))
    {
    } else if (!strncmp(attr_type, "boolean", strlen("boolean")))
    {
        if (!strncmp(value, "0x", 2))
        {
            base = 16;
        } else
        {
            base = 10;
        }
        attr_integer = strtoll(value, NULL, base);
        attr_after_making = make_new_attr(Val_Boolean, (void *) attr_integer);
    } else if (!strncmp(attr_type, "ptr", strlen("ptr")))
    {
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "Unsupport attr type: %s from %s\n", attr_type, objname);
        return 0;
    }

    conf_object_t *device = get_conf_obj(objname);

    set_conf_attr(device, key, attr_after_making);
    skyeye_free(attr_after_making);
    return 1;
}

int WIN_add_map(char *machname, char *devicename, generic_address_t base_addr, generic_address_t length)
{
    conf_object_t *mach_obj = get_conf_obj(machname);
    conf_object_t *device = get_conf_obj(devicename);
    memory_space_intf *io_memory = (memory_space_intf *) SKY_get_iface(device, MEMORY_SPACE_INTF_NAME);
    skyeye_machine_intf *iface = (skyeye_machine_intf *) SKY_get_iface(mach_obj, MACHINE_INTF_NAME);

    if (io_memory == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", MEMORY_SPACE_INTF_NAME, devicename);
        return -1;
    }

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", MACHINE_INTF_NAME, machname);
        return -1;
    }

    addr_space_t *space = iface->get_space(mach_obj);

    SKY_add_map(space, device, base_addr, length, 0x0, io_memory, 1, 1);

    return 1;
}

char *WIN_module_get_value_by_name(char *modulename, char *key)
{
    char *value;

    value = SKY_module_get_value_by_name(modulename, key);
    return value;
}

char *WIN_module_get_path_by_name(char *modulename)
{
    char *value;

    value = SKY_module_get_path_by_name(modulename);
    return value;
}

char *WIN_get_module_names(void)
{
    char buf[512] = { '\0' };
    char *module_name = NULL;
    int num = 1;
    skyeye_module_t *modules = get_module_list();
    module_name = skyeye_mm(sizeof (char) * 1);
    *module_name = '\0';

    while (modules != NULL)
    {
        sprintf(buf, "%s;", modules->module_name);
        num = num + strlen(buf);
        module_name = realloc(module_name, sizeof (char) * num);
        strcat(module_name, buf);

        modules = modules->next;
    }
    num = num + strlen("|");
    module_name = realloc(module_name, sizeof (char) * num);
    strcat(module_name, "|");
    return module_name;
}

void WIN_load_module_fromdir(char *dir)
{
    SKY_load_all_modules(dir, NULL);
}

void WIN_unload_all_modules(void)
{
    SKY_unload_all_modules();
}

uint32_t WIN_get_dev_register_num(char *devicename)
{
    conf_object_t *obj = get_conf_obj(devicename);
    uint32_t reg_num;

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", devicename);
        return 0;
    }
    //First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(obj, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        //if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(obj, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", SKYEYE_REG_INTF, devicename);
            return 0;
        }
        reg_num = iface->get_regnum(obj);
    } else
    {
        reg_num = iface64->get_regnum(obj);
    }

    return reg_num;
}

uint64_t WIN_get_regvalue_by_id(char *devicename, uint32_t id)
{
    conf_object_t *obj = get_conf_obj(devicename);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", devicename);
        return 0;
    }
    //First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(obj, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        //if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(obj, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", SKYEYE_REG_INTF, devicename);
            return 0;
        }

        return iface->get_regvalue_by_id(obj, id);
    } else
    {
        return iface64->get_regvalue_by_id(obj, id);
    }
}

uint32_t WIN_get_regoffset_by_id(char *devicename, uint32_t id)
{
    conf_object_t *obj = get_conf_obj(devicename);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", devicename);
        return 0;
    }

    skyeye_reg64_intf *iface64 = SKY_get_iface(obj, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        //if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(obj, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", SKYEYE_REG_INTF, devicename);
            return -1;
        }
        if (iface->get_regoffset_by_id == NULL)
        {
            return 0xffffffff;
        }

        return iface->get_regoffset_by_id(obj, id);
    } else
    {
        if (iface64->get_regoffset_by_id == NULL)
        {
            return 0xffffffff;
        }
        return iface64->get_regoffset_by_id(obj, id);
    }
}

char *WIN_get_regname_by_id(char *devicename, uint32_t id)
{
    conf_object_t *obj = get_conf_obj(devicename);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", devicename);
        return 0;
    }
    //First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(obj, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        //if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(obj, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", SKYEYE_REG_INTF, devicename);
            return NULL;
        }
        return iface->get_regname_by_id(obj, id);
    } else
    {
        return iface64->get_regname_by_id(obj, id);
    }
}

uint32_t WIN_set_regvalue_by_id(char *devicename, uint64_t value, uint32_t id)
{
    conf_object_t *obj = get_conf_obj(devicename);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", devicename);
        return -1;
    }
    //First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(obj, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        //if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(obj, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", SKYEYE_REG_INTF, devicename);
            return -1;
        }
        return iface->set_regvalue_by_id(obj, (uint32_t) value, id);
    } else
    {
        return iface64->set_regvalue_by_id(obj, value, id);
    }
}

uint32_t WIN_get_regid_by_name(char *devicename, char *name)
{
    conf_object_t *obj = get_conf_obj(devicename);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", devicename);
        return 0;
    }
    //First check if have SKYEYE_REG64_INTF
    skyeye_reg64_intf *iface64 = SKY_get_iface(obj, SKYEYE_REG64_INTF);

    if (iface64 == NULL)
    {
        //if no SKYEYE_REG64_INTF, check SKYEYE_REG_INTF
        skyeye_reg_intf *iface = SKY_get_iface(obj, SKYEYE_REG_INTF);

        if (iface == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", SKYEYE_REG_INTF, devicename);
            return -1;
        }
        return iface->get_regid_by_name(obj, name);
    } else
    {
        return iface64->get_regid_by_name(obj, name);
    }
}

char *WIN_disassemble(char *cpuname, uint32_t addr)
{
    conf_object_t *obj = get_conf_obj(cpuname);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return NULL;
    }

    core_info_intf *iface = SKY_get_iface(obj, CORE_INFO_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", CORE_INFO_INTF_NAME);
        return NULL;
    }
    return iface->disassemble(obj, addr);
}

char **get_class_list(void)
{
    return SKY_get_classname_list();
}

char *get_class_type(char *classname)
{
    conf_class_t *cls = SKY_get_class(classname);

    return cls->class_data.class_type;

}
int get_device_module_type(char *classname)
{
    conf_class_t *cls = SKY_get_class(classname);

    return cls->class_data.module_type.c_type;

}
char **get_interface_list(char *classname)
{
    conf_class_t *cls = SKY_get_class(classname);

    if (cls == NULL)
        return NULL;
    return class_get_iface_list(cls);
}

char **get_connect_list(char *classname)
{
    static char *cache[128];
    conf_class_t *cls = SKY_get_class(classname);

    if (cls == NULL)
        return NULL;
    return class_get_connect_list(cls);
}

const char **get_attr_list(char *classname)
{
    conf_class_t *cls = SKY_get_class(classname);

    if (cls == NULL)
        return NULL;
    return get_class_attributes(cls);
}

const char **get_attr_info(char *classname, const char *attrname)
{
    conf_class_t *cls = SKY_get_class(classname);

    if (cls == NULL)
        return NULL;
    return get_class_attrinfo(cls, attrname);
}

int connect_interface(char *pro, char *iface_name, char *rec, char *iface_attr_name)
{
    return 0;
}

int load_module_fromdir(char *dir)
{
    init_module_list();
    SKY_load_all_modules(dir, NULL);
    return 0;
}

int load_module_fromws(char *ws_dir)
{
#define MAX_LEN 256
#define MAX_DIR 64
    char module_dir[MAX_LEN];
    struct dirent *direntp;
    struct stat st;
    char path[MAX_LEN];
    char *dirs[MAX_DIR] = { NULL };
    int index = 0;
    DIR *dir = NULL;

    if (strlen(ws_dir) + 1 > MAX_LEN)
    {
        skyeye_log(Error_log, __FUNCTION__, "workspace_dir name is too long.\n");
        return Error;
    }

    sprintf(module_dir, "%s\\%s", ws_dir, "targets\\skyeye\\");

    dir = opendir(module_dir);
    if (dir == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not open dir:%s.\n", module_dir);
        return Error;
    }

    dirs[index] = skyeye_mm_zero(strlen(module_dir) + 1);
    if (strlen(module_dir) + 1 > MAX_LEN)
    {
        skyeye_log(Error_log, __FUNCTION__, "module_dir name is too long.\n");
        return Error;
    }

    sprintf(dirs[index], "%s", module_dir);
    index++;
    while ((direntp = readdir(dir)) != NULL)
    {
        if (strncmp(direntp->d_name, ".", 1) == 0)
        {
            continue;
        }

        if (strlen(module_dir) + strlen(direntp->d_name) + 1 > MAX_LEN)
        {
            skyeye_log(Error_log, __FUNCTION__, "dirname is too long.\n");
            return Error;
        }

        sprintf(path, "%s\\%s", module_dir, direntp->d_name);
        stat(path, &st);

        if (S_ISDIR(st.st_mode))
        {
            dirs[index] = skyeye_mm_zero(strlen(path) + 1);
            sprintf(dirs[index], "%s", path);
            index++;
        }

    }

    int i;

    for (i = 0; i < index; i++)
    {
        WIN_load_module_fromdir(dirs[i]);
        skyeye_free(dirs[i]);
    }

    return Ok;
}

int load_common_class()
{
    init_common_class();
    return 0;
}

void print_class_list()
{
    char **class_list;
    const char **attr_list;
    const char **attr_info_list;
    char **iface_list;
    int i, j, k;

    class_list = get_class_list();
    if (class_list == NULL)
    {
        printf("class_list == NULL");
        goto ret;
    }

    for (i = 0; class_list[i] != NULL; i++)
    {
        printf("\n******************************************************\n");
        printf("*%-13s+%5s%-33s*\n", "ClassName", " ", class_list[i]);
        printf("*+ + + + + + + + + + + + + + + + + + + + + + + + + + *\n");
        printf("*%-13s+%38s*\n", "InterfaceName", " ");
        iface_list = get_interface_list(class_list[i]);

        if (iface_list != NULL)
        {
            for (j = 0; iface_list[j] != NULL; j++)
                printf("*%-13s+%5s%-33s*\n", " ", " ", iface_list[j]);
        }
        printf("*+ + + + + + + + + + + + + + + + + + + + + + + + + + *\n");

        attr_list = get_attr_list(class_list[i]);
        if (attr_list != NULL)
        {
            for (j = 0; attr_list[j] != NULL; j++)
            {
                printf("*%-13s+%5s%-33s*\n", "AttributeName", " ", attr_list[j]);
                attr_info_list = get_attr_info(class_list[i], attr_list[j]);
                if (attr_info_list != NULL)
                {
                    printf("*%13s+%5s%-33s*\n", "Type", " ", attr_info_list[1]);
                    printf("*%13s+%5s%-33s*\n", "Des", " ", attr_info_list[2]);
                }
                printf("*+ + + + + + + + + + + + + + + + + + + + + + + + + + *\n");
            }
        }
    }
  ret:
    return;
}

int WIN_memory_read_char(char *cpuname, uint32_t addr)
{
    conf_object_t *core = get_conf_obj(cpuname);
    uint32_t val = 0xff;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return val;
    }

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (iface != NULL)
    {
        iface->read(core, addr, &val, 1);
    }

    return (int) (val & 0xff);
}

void WIN_memory_write_char(char *cpuname, uint32_t addr, char *val, int count)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return;
    }

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (iface != NULL)
    {
        iface->write(core, addr, val, count);
    }
}

int WIN_get_cpu_address_width(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return 0;
    }

    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (core_info == NULL)
    {
        return 0;
    }
    return core_info->get_address_width(core);
}

int WIN_write_device4(char *device, uint32_t offset, uint32_t data)
{
    conf_object_t *obj = get_conf_obj(device);

    if (device == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", device);
        return Error;
    }
    memory_space_intf *io_memory = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    io_memory->write(obj, offset, &data, 4);
    return Ok;
}

uint32_t WIN_read_device4(char *device, uint32_t offset)
{
    uint32_t data;
    conf_object_t *obj = get_conf_obj(device);

    if (device == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", device);
        return Error;
    }
    memory_space_intf *io_memory = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    io_memory->read(obj, offset, &data, 4);
    return data;
}

int WIN_write_device(char *device, uint32_t offset, char *buf, uint32_t size)
{
    int i;
    conf_object_t *obj = get_conf_obj(device);

    if (device == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", device);
        return Error;
    }
    memory_space_intf *io_memory = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    for (i = 0; i < size; i++)
        buf[i] = (buf[i]) & 0xff;
    io_memory->write(obj, offset, buf, size);
    return Ok;
}

int WIN_read_data_inject_device(char *device, char *data)
{
    int size;
    conf_object_t *obj = get_conf_obj(device);

    if (device == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", device);
        return 0;
    }
    data_inject_intf *data_inject = (data_inject_intf *) SKY_get_iface(obj, DATA_INJECT_INTF);

    if (data_inject != NULL)
    {
        size = data_inject->read(obj, data);
        return size;
    }
    return 0;
}

int WIN_write_data_inject_device(char *device, char *data, size_t size)
{
    conf_object_t *obj = get_conf_obj(device);

    if (device == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", device);
        return Error;
    }

    data_inject_intf *data_inject = (data_inject_intf *) SKY_get_iface(obj, DATA_INJECT_INTF);

    if (data_inject != NULL)
    {
        if (size > 0 && data != NULL)
        {
            size = data_inject->write(obj, data, size);
            return Ok;
        }
    }
    return Error;
}

uint32_t WIN_set_fj(char *mp, uint32_t addr, uint32_t bit, uint32_t mode, uint32_t count)
{
    exception_t ret;
    uint32_t buf = 0;
    conf_object_t *obj = get_conf_obj(mp);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", obj);
        return Error;
    }

    memory_space_plug_t *dev = obj->obj;
    memory_space_set_plug_intf *plug_iface =
        (memory_space_set_plug_intf *) SKY_get_iface(obj, MEMORY_SPACE_SET_PLUG_INTF_NAME);
    if (plug_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", MEMORY_SPACE_SET_PLUG_INTF_NAME);
        return Error;
    }
    ret = plug_iface->set_plug(obj, addr, bit, mode);

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", CORE_INFO_INTF_NAME);
        return Error;
    }
    ret = iface->read(obj, addr, &buf, count);

    if (mode == 0)
    {
        buf &= ~(0x1 << bit);           //set bit : 0
    } else if (mode == 1)
    {
        buf |= (0x1 << bit);            //set bit : 1
    } else if (mode == 2)
    {                                   //reverse bit :if 0: set 1; if 1: set 0
        if (buf & (0x1 << bit))
        {
            buf &= ~(0x1 << bit);
        } else
        {
            buf |= (0x1 << bit);
        }
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not set mode %d.\n", mode);
    }

    ret = iface->write(obj, addr, &buf, count);
    return Ok;
}

void *WIN_get_fj(char *mp, uint32_t addr, uint32_t bit, uint32_t mode)
{
    exception_t ret;
    conf_object_t *obj = get_conf_obj(mp);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", obj);
        return NULL;
    }

    memory_space_set_plug_intf *iface =
        (memory_space_set_plug_intf *) SKY_get_iface(obj, MEMORY_SPACE_SET_PLUG_INTF_NAME);
    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", MEMORY_SPACE_SET_PLUG_INTF_NAME);
        return NULL;
    }
    dev_fi_t *fj_l = iface->get_plug(obj, addr);

    if (fj_l == NULL)
        return NULL;
    return fj_l;
}

uint32_t WIN_clear_fj(char *mp, uint32_t addr, uint32_t bit, uint32_t mode)
{
    exception_t ret;
    conf_object_t *obj = get_conf_obj(mp);
    uint32_t buf;

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", obj);
        return Error;
    }
    memory_space_plug_t *dev = obj->obj;

    memory_space_set_plug_intf *plug_iface =
        (memory_space_set_plug_intf *) SKY_get_iface(obj, MEMORY_SPACE_SET_PLUG_INTF_NAME);
    if (plug_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", CORE_INFO_INTF_NAME);
        return Error;
    }
    ret = plug_iface->clear_plug(obj, addr, bit, mode);
    if (ret != No_exp)
        return Error;

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface for %s.\n", CORE_INFO_INTF_NAME);
        return Error;
    }
    ret = iface->read(obj, addr, &buf, 4);

    if (mode == 0)
    {
        buf |= (1 << bit);
    } else if (mode == 1)
    {
        buf &= ~(1 << bit);
        if ((buf >> 31) & 0x1)
            buf = 0;
    } else if (mode == 2)
    {                                   //reverse bit :if 0: set 1; if 1: set 0
        if (buf & (0x1 << bit))
        {
            buf &= ~(0x1 << bit);
        } else
        {
            buf |= (0x1 << bit);
        }
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not set mode %d.\n", mode);
    }

    ret = iface->write(obj, addr, &buf, 4);
    return Ok;
}

int WIN_connect(char *con_objname, char *iface_objname, char *name, uint32_t index)
{
    conf_object_t *con_obj = get_conf_obj(con_objname);

    if (con_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", con_objname);
        return Error;
    }

    conf_object_t *iface_obj = get_conf_obj(iface_objname);

    if (iface_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", iface_objname);
        return Error;
    }

    exception_t ret = skyeye_connect(con_obj, iface_obj, name, index);

    if (ret != No_exp)
    {
        skyeye_log(Error_log, __FUNCTION__, "Connect %s and %s fail.\n", con_objname, iface_objname);
        return Error;
    }

    return Ok;
}

int WIN_memory_space_add_map(char *memory_space_name, char *device_name, uint64_t address, uint32_t length)
{
    conf_object_t *memory_space = get_conf_obj(memory_space_name);

    if (memory_space == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", memory_space_name);
        return Error;
    }

    conf_object_t *device = get_conf_obj(device_name);

    if (device == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", device_name);
        return Error;
    }
    memory_space_add_map_intf *iface =
        (memory_space_add_map_intf *) SKY_get_iface(memory_space, MEMORY_SPACE_ADD_MAP_NAME);
    exception_t ret = iface->add_map(memory_space, device, address, length);

    if (ret != No_exp)
        return Error;

    return Ok;
}

int WIN_memory_space_add_map_group(char *memory_space_name, char *device_name, uint64_t address, uint32_t length,
                                   uint32_t index)
{
    conf_object_t *memory_space = get_conf_obj(memory_space_name);

    if (memory_space == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", memory_space_name);
        return Error;
    }
    conf_object_t *device = get_conf_obj(device_name);

    if (device == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", device_name);
        return Error;
    }
    memory_space_add_map_intf *iface =
        (memory_space_add_map_intf *) SKY_get_iface(memory_space, MEMORY_SPACE_ADD_MAP_NAME);
    exception_t ret = iface->add_map_group(memory_space, device, address, length, index);

    if (ret != No_exp)
        return Error;

    return Ok;
}

uint32_t x86_get_device_num(char *cpu)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return Error;
    }

    core_devices_list *iface = SKY_get_iface(obj, DEVICE_LIST_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpu);
        return Error;
    }
    return iface->get_devs_num(obj);
}

char *x86_get_device_name(char *cpu, int did)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return Error;
    }

    core_devices_list *iface = SKY_get_iface(obj, DEVICE_LIST_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpu);
        return Error;
    }

    return iface->get_devs_name(obj, did);
}

uint32_t x86_get_device_reg_num(char *cpu, int did)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return Error;
    }

    core_devices_list *iface = SKY_get_iface(obj, DEVICE_LIST_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpu);
        return Error;
    }

    return iface->get_devs_reg_num(obj, did);
}

char *x86_get_device_reg_name(char *cpu, int did, int rid)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return Error;
    }

    core_devices_list *iface = SKY_get_iface(obj, DEVICE_LIST_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpu);
        return Error;
    }
    return iface->get_devs_reg_name(obj, did, rid);
}

uint32_t x86_get_device_reg_value(char *cpu, int did, int rid)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return Error;
    }

    core_devices_list *iface = SKY_get_iface(obj, DEVICE_LIST_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpu);
        return Error;
    }
    return iface->get_devs_reg_val(obj, did, rid);
}

uint32_t x86_get_device_reg_large_value(char *cpu, int did, int rid, int offset)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return Error;
    }

    core_devices_list *iface = SKY_get_iface(obj, DEVICE_LIST_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpu);
        return Error;
    }
    return iface->get_devs_reg_large_val(obj, did, rid, offset);
}

uint32_t x86_get_device_reg_width(char *cpu, int did)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return Error;
    }

    core_devices_list *iface = SKY_get_iface(obj, DEVICE_LIST_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpu);
        return Error;
    }
    return iface->get_devs_reg_width(obj, did);
}

uint32_t x86_get_device_reg_addr(char *cpu, int did, int rid)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return Error;
    }

    core_devices_list *iface = SKY_get_iface(obj, DEVICE_LIST_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, cpu);
        return Error;
    }
    return iface->get_devs_reg_addr(obj, did, rid);
}

void x86_device_save_fin(char *cpu, int did, uint32_t addr, int bitnum, int mode)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return;
    }

    core_devices_fin *iface = SKY_get_iface(obj, DEVICE_FIN_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, cpu);
        return;
    }
    return iface->devs_save_fin(obj, did, addr, bitnum, mode);
}

void *x86_device_get_fin(char *cpu, int did)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return Error;
    }

    core_devices_fin *iface = SKY_get_iface(obj, DEVICE_FIN_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, cpu);
        return Error;
    }
    return (void *) (iface->devs_get_fin(obj, did));
}

void x86_device_clear_fin(char *cpu, int did, uint32_t addr, int bitnum, int mode)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return;
    }

    core_devices_fin *iface = SKY_get_iface(obj, DEVICE_FIN_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, cpu);
        return;
    }
    return iface->devs_clear_fin(obj, did, addr, bitnum, mode);
}

int x86_save_configure(char *cpuname, char *filename)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return Error;
    }

    x86_checkpoint_intf *iface = (x86_checkpoint_intf *) SKY_get_iface(core, X86_CHECKPOINT_INTF_NAME);
    exception_t ret = iface->save_configure(core, filename);

    if (ret != No_exp)
        return Error;

    return Ok;
}

int x86_load_configure(char *cpuname, char *filename)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return Error;
    }

    x86_checkpoint_intf *iface = (x86_checkpoint_intf *) SKY_get_iface(core, X86_CHECKPOINT_INTF_NAME);
    exception_t ret = iface->load_configure(core, filename);

    if (ret != No_exp)
        return Error;

    return Ok;
}

void x86_reset(char *cpu)
{
    conf_object_t *obj = get_conf_obj(cpu);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return;
    }

    x86_core_intf *iface = SKY_get_iface(obj, X86_CORE_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", X86_CORE_INTF_NAME, cpu);
        return;
    }
    return iface->x86_reset(obj);
}

int WIN_store_configure(char *filename)
{
    store_to_configure(filename);
    return Ok;
}

int WIN_recover_configure(char *filename)
{
    recover_from_configure(filename);
    return Ok;
}

int32_t WIN_image_get_page_size(char *imagename)
{
    conf_object_t *image = get_conf_obj(imagename);
    uint32_t page_size;

    if (image == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", imagename);
        return Error;
    }

    skyeye_image_info *iface = SKY_get_iface(image, SKYEYE_IMAGE_INFO);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, imagename);
        return Error;
    }
    page_size = iface->get_page_size(image);
    return page_size;
}

int32_t WIN_image_get_page_count(char *imagename)
{
    conf_object_t *image = get_conf_obj(imagename);
    uint32_t page_count;

    if (image == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", imagename);
        return Error;
    }

    skyeye_image_info *iface = SKY_get_iface(image, SKYEYE_IMAGE_INFO);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, imagename);
        return Error;
    }
    page_count = iface->get_page_count(image);
    return page_count;
}

int32_t WIN_image_get_page_index_by_id(char *imagename, uint32_t id)
{
    conf_object_t *image = get_conf_obj(imagename);

    if (image == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", imagename);
        return Error;
    }

    skyeye_image_info *iface = SKY_get_iface(image, SKYEYE_IMAGE_INFO);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, imagename);
        return Error;
    }
    return iface->get_page_index_by_id(image, id);
}

int32_t WIN_image_page_to_file(char *imagename, uint32_t index, char *filename)
{
    conf_object_t *image = get_conf_obj(imagename);
    uint32_t page_size, count;

    if (image == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", imagename);
        return Error;
    }

    skyeye_image_info *iface = SKY_get_iface(image, SKYEYE_IMAGE_INFO);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, imagename);
        return Error;
    }

    page_size = iface->get_page_size(image);
    char *page_base = iface->get_page_content(image, index);

    if (page_base == NULL)
    {
        return Error;
    }

    FILE *fp = fopen(filename, "wb+");

    if (fp == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Create %s file fail\n", filename);
        return Error;
    }

    count = fwrite(page_base, 1, page_size, fp);
    fclose(fp);
    if (count != page_size)
    {
        skyeye_log(Error_log, __FUNCTION__, "Write %d bytes to file %s, less than the correct amount.\n", count,
                   filename);
        return Error;
    }
    return Ok;
}

int32_t WIN_image_file_to_page(char *imagename, uint32_t index, char *filename)
{
    conf_object_t *image = get_conf_obj(imagename);
    uint32_t page_size, count;
    char *page_ptr = NULL;

    if (image == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", imagename);
        return Error;
    }

    skyeye_image_info *iface = SKY_get_iface(image, SKYEYE_IMAGE_INFO);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, imagename);
        return Error;
    }

    page_size = iface->get_page_size(image);
    page_ptr = skyeye_mm_zero(page_size + 1);
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "open %s file fail\n", filename);
        return Error;
    }
    count = fread(page_ptr, 1, page_size, fp);
    fclose(fp);
    if (count != page_size)
    {
        skyeye_log(Error_log, __FUNCTION__, "Read %d bytes from file %s, less than the correct amount.\n", count,
                   filename);
        skyeye_free(page_ptr);
        return Error;
    }

    iface->set_page_content(image, page_ptr, index);
    skyeye_free(page_ptr);
    return Ok;
}

int32_t WIN_image_clear_all_pages(char *imagename)
{
    conf_object_t *image = get_conf_obj(imagename);
    uint32_t page_size, count;
    char *page_ptr = NULL;

    if (image == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", imagename);
        return Error;
    }

    skyeye_image_info *iface = SKY_get_iface(image, SKYEYE_IMAGE_INFO);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", DEVICE_FIN_INTF_NAME, imagename);
        return Error;
    }

    iface->clear_all_pages(image);

    return Ok;
}

uint64_t WIN_get_cpu_steps(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);
    uint64_t steps;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return Error;
    }

    core_info_intf *core_info = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (core_info == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpuname);
        return Error;
    }

    steps = core_info->get_steps(core);
    return steps;
}

int WIN_set_cpu_mode(char *cpuname, int mode)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return Error;
    }

    attr_value_t value;

    if (!SKY_has_attribute(core, "mode"))
    {
        value = SKY_make_attr_uinteger(mode);
        SKY_set_attribute(core, "mode", &value);
    }

    return Ok;
}

int WIN_get_executed_pc_file(char *instr_process_name, char *fname)
{
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    if (instr_process == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", instr_process_name);
        return Error;
    }

    instr_process_intf *iface = (instr_process_intf *) SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", INSTR_PROCESS_INTF,
                   instr_process_name);
        return Error;
    }

    iface->get_dif_pc_cache(instr_process, fname);

    return Ok;
}

int WIN_get_pc_record_size(char *instr_process_name)
{
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    if (instr_process == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", instr_process_name);
        return Error;
    }

    instr_process_intf *iface = (instr_process_intf *) SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", INSTR_PROCESS_INTF,
                   instr_process_name);
        return Error;
    }

    return iface->get_pc_record_size(instr_process);
}

int WIN_set_pc_record_size(char *instr_process_name, int size)
{
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    if (instr_process == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", instr_process_name);
        return Error;
    }

    instr_process_intf *iface = (instr_process_intf *) SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", INSTR_PROCESS_INTF,
                   instr_process_name);
        return Error;
    }

    return iface->set_pc_record_size(instr_process, size);
}

int WIN_get_pc_record_nums(char *instr_process_name)
{
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    if (instr_process == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", instr_process_name);
        return Error;
    }

    instr_process_intf *iface = (instr_process_intf *) SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", INSTR_PROCESS_INTF,
                   instr_process_name);
        return Error;
    }

    return iface->get_pc_record_nums(instr_process);
}

int WIN_get_pc_record_index(char *instr_process_name)
{
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    if (instr_process == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", instr_process_name);
        return Error;
    }

    instr_process_intf *iface = (instr_process_intf *) SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", INSTR_PROCESS_INTF,
                   instr_process_name);
        return Error;
    }

    return iface->get_pc_record_index(instr_process);
}

int WIN_get_pc_record_overflow(char *instr_process_name)
{
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    if (instr_process == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", instr_process_name);
        return Error;
    }
    instr_process_intf *iface = (instr_process_intf *) SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", INSTR_PROCESS_INTF,
                   instr_process_name);
        return Error;
    }

    return iface->get_pc_record_overflow(instr_process);
}

int WIN_get_pc_by_index(char *instr_process_name, int n)
{
    conf_object_t *instr_process = get_conf_obj(instr_process_name);

    if (instr_process == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", instr_process_name);
        return Error;
    }

    instr_process_intf *iface = (instr_process_intf *) SKY_get_iface(instr_process, INSTR_PROCESS_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", INSTR_PROCESS_INTF,
                   instr_process_name);
        return Error;
    }

    return iface->get_pc_by_index(instr_process, n);
}

char *WIN_get_architecture(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return NULL;
    }

    core_info_intf *iface = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpuname);
        return Error;
    }

    return iface->get_architecture(core);
}

void mm_info_cmd(char *args)
{
#ifdef IS_MM_TEST
    if (!strcmp(args, "log"))
    {
        set_mm_save_log(1);
        printf("Set log save Ok!\n");
    } else if (!strcmp(args, "nolog"))
    {
        set_mm_save_log(0);
        printf("Cancel save Ok!\n");
    } else
    {
        mm_info_print(args);
    }
#else
    printf("mm_info command failure\n");
#endif
}

uint32_t get_pin_num(conf_object_t * obj)
{
    if (NULL == obj)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj.\n");
        return -1;
    }

    skyeye_pin_info_intf *iface = (skyeye_pin_info_intf *) SKY_get_iface(obj, SKYEYE_PIN_INFO_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", SKYEYE_PIN_INFO_INTF, obj->objname);
        return Error;
    }

    return iface->get_pin_num(obj);
}

int get_pin_type(conf_object_t * obj, uint32_t index)
{
    if (NULL == obj)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj.\n");
        return -1;
    }

    skyeye_pin_info_intf *iface = (skyeye_pin_info_intf *) SKY_get_iface(obj, SKYEYE_PIN_INFO_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", SKYEYE_PIN_INFO_INTF, obj->objname);
        return -1;
    }

    int type = -1;

    iface->get_pin_type(obj, index, &type);

    return type;
}

float read_pin_state(conf_object_t * obj, uint32_t index)
{
    if (NULL == obj)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj.\n");
        return -1.0;
    }

    skyeye_pin_info_intf *iface = (skyeye_pin_info_intf *) SKY_get_iface(obj, SKYEYE_PIN_INFO_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", SKYEYE_PIN_INFO_INTF, obj->objname);
        return -1.0;
    }

    float state = -1.0;

    iface->read_pin_state(obj, index, &state);

    return state;
}

exception_t write_pin_state(conf_object_t * obj, uint32_t index, float state)
{
    float val = state;

    if (NULL == obj)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj.\n");
        return -1;
    }

    skyeye_pin_info_intf *iface = (skyeye_pin_info_intf *) SKY_get_iface(obj, SKYEYE_PIN_INFO_INTF);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", SKYEYE_PIN_INFO_INTF, obj->objname);
        return -1;
    }

    return iface->write_pin_state(obj, index, &val);
}

char *WIN_get_next_log()
{
    return skyeye_get_next_log();
}

vm_t vm_n[256];

uint32_t WIN_get_vms(vm_py_t * vms, uint32_t limit)
{

    skyeye_system_t *system = system_get();
    uint32_t cnt = 0, i;
    conf_object_t *idle_obj;

    vms->vm_list = vm_n;

    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc_t *soc = system->socs + i;

        idle_obj = soc->idle_device->dev;
        var_monitor_intf *iface = SKY_get_iface(idle_obj, VAR_MONITOR_INTF_NAME);

        cnt += iface->get_vms(idle_obj, vms->vm_list, limit - cnt);

    }
    vms->count = cnt;
}

var_t var_n[1000];
uint32_t WIN_get_vars(var_py_t * vars, uint32_t id, uint32_t limit)
{
    skyeye_system_t *system = system_get();
    uint32_t cnt = 0, i;
    conf_object_t *idle_obj;

    vars->var_list = var_n;

    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc_t *soc = system->socs + i;

        idle_obj = soc->idle_device->dev;
        var_monitor_intf *iface = SKY_get_iface(idle_obj, VAR_MONITOR_INTF_NAME);

        cnt += iface->get_vars(idle_obj, id, vars->var_list, limit - cnt);

    }
    vars->count = cnt;
}

uint32_t WIN_insert_exp(uint32_t id, char *exp)
{
    skyeye_system_t *system = system_get();
    conf_object_t *idle_obj;
    uint32_t cnt = 0, i;

    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc_t *soc = system->socs + i;

        idle_obj = soc->idle_device->dev;
        var_monitor_intf *iface = SKY_get_iface(idle_obj, VAR_MONITOR_INTF_NAME);

        iface->insert_exp(idle_obj, id, exp);
    }
}

uint32_t WIN_remove_exp(uint32_t id)
{
    skyeye_system_t *system = system_get();
    conf_object_t *idle_obj;
    uint32_t cnt = 0, i;

    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc_t *soc = system->socs + i;

        idle_obj = soc->idle_device->dev;
        var_monitor_intf *iface = SKY_get_iface(idle_obj, VAR_MONITOR_INTF_NAME);

        iface->remove_exp(idle_obj, id);
    }
}

uint64_t get_simulation_insn_number(char *cpu_name, conf_object_t * conf_obj)
{
    conf_object_t *obj;
    core_info_intf *core_iface;

    if (cpu_name == NULL)
    {
        sys_soc_t *sys_soc;
        sys_dev_t *sys_dev;
        sys_cpu_t *sys_cpu;

        sys_dev = conf_obj->sys_struct;
        sys_soc = sys_dev->sys_soc;
        // Single core only
        sys_cpu = &sys_soc->cpus[0];

        obj = sys_cpu->cpu;
        core_iface = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);
    } else
    {
        obj = get_conf_obj(cpu_name);
        core_iface = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);
    }

    uint32_t cpu_freq = 10000000;       //Ä¬ÈÏÊÇ10MHZ
    uint64_t insn_num = 0;

    if (core_iface != NULL)
        core_iface->get_cpu_run_status(obj, &cpu_freq, &insn_num);

    return insn_num;
}

double WIN_get_simulation_time(char *cpu_name, conf_object_t * obj)
{
    double curr_time = 0.0f;
    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;
    sys_dev_t *sys_dev;
    sys_dev_t *idle_device;
    conf_object_t *idle_obj;
    idle_api_intf_t *iface;

    /* Query object bojdec */
    if (cpu_name == NULL)
    {
        sys_dev = obj->sys_struct;
        sys_soc = sys_dev->sys_soc;
    } else
    {
        // The following code can be used for single core cases
        conf_object_t *cpu_obj = get_conf_obj(cpu_name);

        sys_cpu = cpu_obj->sys_struct;
        sys_soc = sys_cpu->sys_soc;
    }

    idle_device = sys_soc->idle_device;
    idle_obj = idle_device->dev;

    iface = SKY_get_iface(idle_obj, IDLE_DEV_API_INTF_NAME);

    if (iface != NULL)
    {
        curr_time = iface->get_curr_time(idle_obj);
    }

    return curr_time;
}

uint32_t WIN_get_CPU_PC(char *cpu_name, conf_object_t * conf_obj)
{
    uint32_t curr_pc = 0;

    conf_object_t *obj;
    core_info_intf *core_iface;

    if (cpu_name == NULL)
    {
        sys_soc_t *sys_soc;
        sys_dev_t *sys_dev;
        sys_cpu_t *sys_cpu;

        sys_dev = conf_obj->sys_struct;
        sys_soc = sys_dev->sys_soc;
        // Single core only
        sys_cpu = &sys_soc->cpus[0];

        obj = sys_cpu->cpu;
        core_iface = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);
    } else
    {
        obj = get_conf_obj(cpu_name);
        core_iface = (core_info_intf *) SKY_get_iface(obj, CORE_INFO_INTF_NAME);
    }

    if (core_iface != NULL)
    {
        curr_pc = core_iface->get_pc(obj);
    }
    return curr_pc;
}

uint32_t WIN_read_address_bus(char *cpu_name, conf_object_t * conf_obj, uint32_t addr, void *value, uint32_t count)
{
    conf_object_t *obj;
    memory_space_intf *memory_space;

    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;
    sys_dev_t *sys_dev;

    if (cpu_name == NULL)
    {
        if (conf_obj == NULL)
        {
            skyeye_log(Warning_log, __FUNCTION__, "cpu_name and conf_obj is NULL\n");
            return 0;
        }
        sys_dev = conf_obj->sys_struct;
        sys_soc = sys_dev->sys_soc;
    } else
    {
        obj = get_conf_obj(cpu_name);
        sys_cpu = obj->sys_struct;
        sys_soc = sys_cpu->sys_soc;
    }

    obj = sys_soc->memory_space[0]->dev;
    memory_space = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    if (memory_space != NULL)
    {
        memory_space->read(obj, addr, value, count);
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "memory_space intf is NULL\n");
        return 0;
    }

    return 1;
}

uint32_t WIN_write_address_bus(char *cpu_name, conf_object_t * conf_obj, uint32_t addr, void *value, uint32_t count)
{
    conf_object_t *obj;
    memory_space_intf *memory_space;

    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;
    sys_dev_t *sys_dev;

    if (cpu_name == NULL)
    {
        if (conf_obj == NULL)
        {
            skyeye_log(Warning_log, __FUNCTION__, "cpu_name and conf_obj is NULL\n");
            return 0;
        }
        sys_dev = conf_obj->sys_struct;
        sys_soc = sys_dev->sys_soc;
    } else
    {
        obj = get_conf_obj(cpu_name);
        sys_cpu = obj->sys_struct;
        sys_soc = sys_cpu->sys_soc;
    }

    obj = sys_soc->memory_space[0]->dev;
    memory_space = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    if (memory_space != NULL)
    {
        memory_space->write(obj, addr, value, count);
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "memory_space intf is NULL\n");
        return 0;
    }
    return 1;
}

int32_t SkyEye_CreateTimer(char *devicename, void *user_data, uint64_t time, time_sched_t func, uint32_t flags)
{
    conf_object_t *obj;

    obj = get_conf_obj(devicename);
    return system_register_timer_handler_with_data(obj, user_data, time, func, flags);
}

bool_t SkyEye_DeleteTimer(int handle)
{
    return system_del_timer_handler(handle);
}

double SkyEye_GetRunTime(char *cpu_name, conf_object_t * obj)
{
    return WIN_get_simulation_time(cpu_name, obj);
}

uint32_t SkyEye_GetCurPC(char *cpu_name, conf_object_t * obj)
{
    return WIN_get_CPU_PC(cpu_name, obj);
}

bool_t SkyEye_IsCpuStopped(void)
{
    return !SIM_is_running();
}

char *SkyEye_GetPrjPath(void)
{
    return WIN_get_work_path();
}

void SkyEye_StartDebug(void)
{
    SIM_run();
}

void SkyEye_StopDebug(generic_arch_t * arch_instance)
{
    SIM_stop(arch_instance);
}

void SkyEye_ResetDebug(void)
{
    SIM_reset();
}

void SkyEye_ResetCpu(void)
{
    SIM_restart();
}

void SkyEye_SetDyncomWatchMemoryStatus(uint32_t addr, int status)
{
    skyeye_system_t *system = system_get();
    sys_soc_t *sys_soc;
    sys_cpu_t *sys_cpu;
    int i, j;

    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc = system->socs + i;
        for (j = 0; j < sys_soc->cpu_cnt; j++)
        {
            sys_cpu = sys_soc->cpus + j;
            conf_object_t *cpu = sys_cpu->cpu;
            core_info_intf *core_info = (core_info_intf *) SKY_get_iface(cpu, CORE_INFO_INTF_NAME);

            if (core_info->set_memory_watch_status != NULL)
                core_info->set_memory_watch_status(cpu, addr, 1);       //enable addr memory watch

        }
    }
}

int SkyEye_SetWatchOnMemory(conf_object_t * conf_obj,
                            char *ms_name,
                            int mm_type,
                            uint32_t m_addr,
                            uint32_t m_length,
                            OP_DataType_t data_type, cond_compare_t cond, void *value, memory_callback_t callback)
{
    int i;

    /*Enable addr memory watch */
    SkyEye_SetDyncomWatchMemoryStatus(m_addr, 1);

    for (i = 0; i <= m_length; i++)
        SkyEye_Memory_Monitor(conf_obj, ms_name, mm_type, m_addr + i * 4, data_type, cond, value, callback);
    return 1;
}

int SkyEye_UnWatchOnMemory(char *ms_name, int mm_type, uint32_t m_addr, uint32_t m_length)
{
    int i;

    /*Disable addr memory watch */
    SkyEye_SetDyncomWatchMemoryStatus(m_addr, 0);

    for (i = 0; i <= m_length; i++)
        SkyEye_Memory_Monitor_Remove(ms_name, mm_type, m_addr + i * 4);
    return 1;
}

int SkyEye_OutCovFile(char *instr_process_name, char *fname)
{
    return WIN_get_executed_pc_file(instr_process_name, fname);
}

uint8_t SkyEye_ReadByte(char *cpu_name, uint32_t mm_addr)
{
    conf_object_t *core = get_conf_obj(cpu_name);
    uint32_t val = 0xff;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu_name);
        return val;
    }

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (iface != NULL)
    {
        iface->read(core, mm_addr, &val, 1);
    }
    return (int) (val & 0xff);
}

uint32_t SkyEye_ReadDWord(char *cpu_name, uint32_t mm_addr)
{
    conf_object_t *core = get_conf_obj(cpu_name);
    uint32_t val = 0xffffffff;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu_name);
        return val;
    }

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (iface != NULL)
    {
        iface->read(core, mm_addr, &val, 4);
    }
    return val;
}

int SkyEye_WriteByte(char *cpu_name, uint32_t mm_addr, uint8_t value)
{
    conf_object_t *core = get_conf_obj(cpu_name);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu_name);
        return value;
    }

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (iface != NULL)
    {
        iface->write(core, mm_addr, &value, 1);
    }
    return 1;
}

int SkyEye_WriteDWord(char *cpu_name, uint32_t mm_addr, uint32_t value)
{
    conf_object_t *core = get_conf_obj(cpu_name);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu_name);
        return value;
    }

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(core, MEMORY_SPACE_INTF_NAME);

    if (iface != NULL)
    {
        iface->write(core, mm_addr, &value, 4);
    }
    return 1;
}

uint64_t SkyEye_GetCpuFreq(char *cpu_name, conf_object_t * conf_obj)
{
    conf_object_t *obj;
    sys_cpu_t *sys_cpu;

    obj = get_conf_obj(cpu_name);
    sys_cpu = obj->sys_struct;

    return sys_cpu->freq_hz;
}

static char *skyeye_log_full_path;
void SkyEye_ClearLog()
{
    skyeye_log_full_path = (char *) skyeye_mm_zero(64);
#ifdef __WIN32__
    skyeye_log_full_path = getenv("SKYEYEBIN");
    strcat(skyeye_log_full_path, "\\run.log");
#else
    skyeye_log_full_path = getenv("HOME");
    strcat(skyeye_log_full_path, "/.skyeye/run.log");
#endif
    remove(skyeye_log_full_path);
    skyeye_free(skyeye_log_full_path);
}

void SkyEye_ClearCov(char *fname)
{
    remove(fname);
}

void SkyEye_SetWatchOnPC(char *cpu_name, uint32_t pc_addr, PC_Watch_CallBack func, void *user_data)
{
    conf_object_t *obj;
    sys_cpu_t *sys_cpu;

    obj = get_conf_obj(cpu_name);
    sys_cpu = obj->sys_struct;

    if (sys_cpu->pc_watch_count < 1024)
    {
        sys_cpu->pc_watch_info[sys_cpu->pc_watch_count] = (pc_watch *) skyeye_mm_zero(sizeof (pc_watch));
        sys_cpu->pc_watch_info[sys_cpu->pc_watch_count]->addr = pc_addr;
        sys_cpu->pc_watch_info[sys_cpu->pc_watch_count]->pc_watch_func = func;
        sys_cpu->pc_watch_info[sys_cpu->pc_watch_count]->pc_watch_data = user_data;
        sys_cpu->pc_watch_info[sys_cpu->pc_watch_count]->enable = 1;

        sys_cpu->pc_watch_count++;
    } else
    {
        printf("In %s, Maximum monitoring quantity exceeded\n", __func__);
        return;
    }
}

void SkyEye_UnWatchOnPc(char *cpu_name, uint32_t pc_addr)
{
    int count;
    conf_object_t *obj;
    sys_cpu_t *sys_cpu;

    obj = get_conf_obj(cpu_name);
    sys_cpu = obj->sys_struct;

    for (count = 0; count < sys_cpu->pc_watch_count; count++)
    {
        if (pc_addr == sys_cpu->pc_watch_info[count]->addr)
        {
            sys_cpu->pc_watch_info[count]->enable = 0;
            skyeye_free(sys_cpu->pc_watch_info[sys_cpu->pc_watch_count]);
            break;
        }
    }
}

void SkyEye_RegisterSyncCallBack(Sync_CallBack func)
{
    SIM_stop(NULL);
    func();
    SIM_start();
}

int SkyEye_GetCpuExceptionNum(char *cpu_name)
{
    conf_object_t *core = get_conf_obj(cpu_name);
    int exception_num = 0;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get object Failed! cpu_name : %s !\n", cpu_name);
        return 0;
    }

    core_info_intf *iface = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't Get %s core_info_intf!\n", cpu_name);
        return 0;
    }

    if (iface->get_cpu_exception_num != NULL)
    {
        exception_num = iface->get_cpu_exception_num(core);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "cpu_name :%s Not implement get_cpu_exception_num iface!\n", cpu_name);
        return 0;
    }

    return exception_num;
}

exception_t SkyEye_SetCpuException(char *cpu_name, int exception_id)
{
    conf_object_t *core = get_conf_obj(cpu_name);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get object Failed! cpu_name : %s !\n", cpu_name);
        return Not_found_exp;
    }

    core_info_intf *iface = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't Get %s core_info_intf!\n", cpu_name);
        return Not_found_exp;
    }

    if (iface->set_cpu_exception_by_id != NULL)
    {
        iface->set_cpu_exception_by_id(core, exception_id);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "cpu_name :%s Not implement set_cpu_exception_by_id iface!\n", cpu_name);
        return Not_found_exp;
    }

    return No_exp;
}

exception_t SkyEye_SetExtInterrupt(char *intc_name, int interrupt_num)
{
    conf_object_t *intc = get_conf_obj(intc_name);

    if (intc == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get object Failed! intc : %s !\n", intc_name);
        return Not_found_exp;
    }

    general_signal_intf *iface = (general_signal_intf *) SKY_get_iface(intc, GENERAL_SIGNAL_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't Get %s general_signal_intf!\n", intc_name);
        return Not_found_exp;
    }

    if (iface->raise_signal != NULL)
    {
        iface->raise_signal(intc, interrupt_num);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "intc_name :%s Not implement raise_signal iface!\n", intc_name);
        return Not_found_exp;
    }

    return No_exp;
}

char *SkyEye_GetCpuExceptionNameById(char *cpu_name, int exception_id)
{
    conf_object_t *core = get_conf_obj(cpu_name);
    int exception_num = 0;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get object Failed! cpu_name : %s !\n", cpu_name);
        return NULL;
    }

    core_info_intf *iface = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't Get %s core_info_intf!\n", cpu_name);
        return NULL;
    }

    if (iface->get_cpu_exception_name_by_id != NULL)
    {
        return iface->get_cpu_exception_name_by_id(core, exception_id);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "cpu_name :%s Not implement get_cpu_exception_name_by_id iface!\n",
                   cpu_name);
        return NULL;
    }
}

int SkyEye_SetCpuRunMode(char *cpu_name, int run_mode)
{
    conf_object_t *core = get_conf_obj(cpu_name);
    attr_value_t attr;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get object Failed! cpu_name : %s !\n", cpu_name);
        return Error;
    }

    attr = SKY_make_attr_uinteger(run_mode);
    if (!SKY_has_attribute(core, "mode"))
    {
        SKY_set_attribute(core, "mode", &attr);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s not has mode attribute!\n", cpu_name);
        return Error;
    }

    skyeye_log(Info_log, __FUNCTION__, "Set %s run_mode :%d OK!\n", cpu_name, run_mode);
    return Ok;
}

int WIN_setPC(char *cpu_name, unsigned int pc)
{
    conf_object_t *core = get_conf_obj(cpu_name);
    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get object Failed! cpu_name : %s !\n", cpu_name);
        return Error;
    }

    core_info_intf *iface = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't Get %s core_info_intf!\n", cpu_name);
        return Error;
    }

    iface->set_pc(core, pc);
    return Ok;
}

uint32_t WIN_getPC(char *cpu_name)
{
    conf_object_t *core = get_conf_obj(cpu_name);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get object Failed! cpu_name : %s !\n", cpu_name);
        return 0xffffffff;
    }

    core_info_intf *iface = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't Get %s core_info_intf!\n", cpu_name);
        return 0xffffffff;
    }

    return iface->get_pc(core);
}
