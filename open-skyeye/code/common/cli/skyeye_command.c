#include <stdlib.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_FILE_H
#  include <sys/file.h>
#endif
#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#if defined (HAVE_STRING_H)
#  include <string.h>
#else /* !HAVE_STRING_H */
#  include <strings.h>
#endif /* !HAVE_STRING_H */

#if 0
#ifdef READLINE_LIBRARY
#  include "readline.h"
#  include "history.h"
#else
#  include <readline/readline.h>
#  include <readline/history.h>
#endif
#endif

#include "default_command.h"
#include "skyeye_command.h"
/*
 * #include "skyeye_types.h"
 * #include <skyeye_cli.h>
 * #include "skyeye_readline_cli.h"
 * #include "default_command.h"
 * #include "skyeye_command.h"
 * #include "skyeye_mm.h"
 * #include "skyeye_obj.h"
 * #include "skyeye_pref.h"
 * #include "skyeye_class.h"
 * #include <skyeye_log.h>
 * #include <skyeye_dev_intf.h>
 * #include <skyeye_iface.h>
 */

/* **************************************************************** */
/*                                                                  */
/*                       predefined  commands                       */
/*                                                                  */
/* **************************************************************** */
int com_help(char *arg);
COMMAND default_cli_commands[] = {
    {"help", com_help, "List all the category for the commands.\n", NULL},
    {"?", com_help, "Synonym for `help'.\n", NULL},
    //{ "cd", com_cd, "Change to directory DIR" },
    //{ "delete", com_delete, "Delete FILE" },
    // { "list", com_list, "List files in DIR" },
    {"ls", com_list, "Synonym for `list'", NULL},
    // { "pwd", com_pwd, "Print the current working directory" },
    {"quit", com_quit, "Quit SkyEye ", NULL},
    {"q", com_quit, "Quit SkyEye ", NULL},
    // { "rename", com_rename, "Rename FILE to NEWNAME" },
    // { "stat", com_stat, "Print out statistics on FILE" },
    //  { "view", com_view, "View the contents of FILE" },
    {"run", com_run, "Start the simulator.", NULL},
    {"stop", com_stop, "Stop the running of simulator.", NULL},
    {"continue", com_cont, "Continue the running of interrupted simulator.", NULL},
    {"stepi", com_si, "step into .", NULL},
    {"start", com_start, "start simulator.", NULL},
    {"list-modules", com_list_modules, "List all the loaded module.", NULL},
    {"show-pref", com_show_pref, "Show the current preference for SkyEye.", NULL},
    {"show-map", com_show_map, "Show the current memory map for the machine.", NULL},
    {"list-options", com_list_options, "List all the available options for SkyEye.", NULL},
    {"list-machines", com_list_machines, "List all the supported machines for SkyEye.", NULL},
    {"load-conf", com_load_conf, "load a config file and parse it for SkyEye.", NULL},
    {"info", com_info, "show information for various objects. ", NULL},
    {"x", com_x, "display memory value at the address. ", NULL},
    {(char *) NULL, (icpfunc_t *) NULL, (char *) NULL, NULL}
};

static COMMAND *command_list;

void init_command_list()
{
    command_list = NULL;
    COMMAND *node;
    int index = 0;

    /* add all the predefined command into the list */
    node = &default_cli_commands[index];
    while (node->name)
    {
        /* FIXME, do we need to check the return of add_command? */
        add_command(node->name, node->func, node->doc);
        node = &default_cli_commands[index++];
    }
}

/* exception_t add_command(char* command_str, rl_icpfunc_t *func, char* help_str){ */
exception_t add_command(char *command_str, icpfunc_t * func, char *help_str)
{
    COMMAND *command = skyeye_mm_zero(sizeof (COMMAND));

    if (command == NULL)
    {
        return Malloc_exp;
    }
    command->name = skyeye_strdup(command_str);
    if (command->name == NULL)
    {
        skyeye_free(command);
        return Malloc_exp;
    }
    command->func = func;
    command->doc = skyeye_strdup(help_str);
    if (command->doc == NULL)
    {
        skyeye_free(command->name);
        skyeye_free(command);
        return Malloc_exp;
    }
    //command->next = NULL;

    /* we will insert the command node into the head of list */
    command->next = command_list;
    command_list = command;

    return No_exp;
}

void delete_command(char *command_str)
{
    COMMAND *node;

    node = command_list;
    if (command_str && node)
    {
        for (; !node; node = node->next)
        {
            if (!strncmp(node->name, command_str, strlen(node->name)))
            {
            }
        }
    }
}

/* Look up NAME as the name of a command, and return a pointer to that
 * command.  Return a NULL pointer if NAME isn't a command name. */
COMMAND *find_command(char *name)
{
    COMMAND *node;

    if (name == NULL)
        return ((COMMAND *) NULL);
    node = command_list;
    while (node)
    {
        if (!strncmp(name, node->name, strlen(node->name)))
        {
            return node;
        }
        node = node->next;
    }
    return ((COMMAND *) NULL);
}

exception_t run_command(char *command_str)
{
    COMMAND *command = find_command(command_str);

    if (command != NULL && command->func != NULL)
        (*command->func) (NULL);
    return No_exp;
}

/* Print out help for ARG, or for all of the commands if ARG is
 * not present. */
int com_help(char *arg)
{
    int printed = 0;
    COMMAND *node;

    node = command_list;

    /* Return the next name which partially matches from the command list. */
    while (node)
    {
        if (!*arg && (strcmp(arg, node->name) == 0))
        {
            printf("%s\t\t%s.\n", node->name, node->doc);
            printed++;
            break;
        }
        node = node->next;
    }
    if (!printed)
    {
        printf("No commands match '%s'.  Possibilties are:\n", arg);
        node = command_list;
        while (node)
        {
            printf("%s : %s\n", node->name, node->doc);
            node = node->next;
        }
    }
    return (0);
}

/*remote-gdb command */
int WIN_create_remote_gdb(char *cpuname, int port, char *ip)
{
    char objname[MAX_OBJNAME];

    if (strlen(cpuname) + strlen("_gdbserver") + 1 > MAX_OBJNAME)
    {
        return Error;
    }

    get_strcat_objname(objname, cpuname, "_gdbserver");
    conf_object_t *gdbserver_obj = pre_conf_obj(objname, "gdbserver");

    if (gdbserver_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not create cpuname:%s gdbserver\n", cpuname);
        return Error;
    }

    skyeye_gdbserver_intf *iface = (skyeye_gdbserver_intf *)SKY_get_iface(gdbserver_obj, SKYEYE_GDBSERVER_INTF_NAME);
    int ret = iface->GdbServerStart(gdbserver_obj, cpuname, port, ip);

    if (ret == GDB_SERVER_ERROR)
    {
        free_conf_obj(gdbserver_obj);
        return Error;
    }

    return Ok;
}

int WIN_remote_gdb_check_link(char *cpuname)
{
    char objname[MAX_OBJNAME];

    if (strlen(cpuname) + strlen("_gdbserver") + 1 > MAX_OBJNAME)
    {
        return Error;
    }

    get_strcat_objname(objname, cpuname, "_gdbserver");
    conf_object_t *obj = get_conf_obj(objname);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find obj [%s]\n", objname);
        return Error;
    }

    skyeye_gdbserver_intf *iface = (skyeye_gdbserver_intf *) SKY_get_iface(obj, SKYEYE_GDBSERVER_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", SKYEYE_GDBSERVER_INTF_NAME, objname);
        return Error;
    }

    int ret = iface->GdbServerIsConnected(obj);

    return ret;
}

char *WIN_remote_gdb_get_client_ip(char *cpuname)
{
    char objname[MAX_OBJNAME];

    if (strlen(cpuname) + strlen("_gdbserver") + 1 > MAX_OBJNAME)
    {
        return NULL;
    }

    get_strcat_objname(objname, cpuname, "_gdbserver");
    conf_object_t *obj = get_conf_obj(objname);

    if (obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find obj [%s]\n", objname);
        return NULL;
    }

    skyeye_gdbserver_intf *iface = (skyeye_gdbserver_intf *) SKY_get_iface(obj, SKYEYE_GDBSERVER_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", SKYEYE_GDBSERVER_INTF_NAME, objname);
        return NULL;
    }

    char *ip = iface->GdbServerGetClientIp(obj);

    return ip;
}

int WIN_delete_remote_gdb(char *cpuname)
{
    char objname[MAX_OBJNAME];

    if (strlen(cpuname) + strlen("_gdbserver") + 1 > MAX_OBJNAME)
    {
        return Error;
    }

    get_strcat_objname(objname, cpuname, "_gdbserver");
    conf_object_t *obj = get_conf_obj(objname);

    if (obj == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "Free obj [%s] fail\n", objname);
        return Error;
    }

    free_conf_obj(obj);
    return Ok;
}

void set_entry_to_cpu(conf_object_t * core, uint32_t entry_point)
{
    attr_value_t value;

    if (!SKY_has_attribute(core, "elf_entry_point"))
    {
        value = SKY_make_attr_uinteger(entry_point);
        SKY_set_attribute(core, "elf_entry_point", &value);
    }

    return;
}

/*load-binary command */
int32_t WIN_load_binary(char *cpuname, char *elfname)
{
    conf_object_t *core = get_conf_obj(cpuname);
    generic_address_t start_addr, code_start_addr, code_end_addr;
    exception_t exp;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return Error;
    }

    if (access(elfname, F_OK))
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not found file : %s.\n", elfname);
        return Error;
    }

    if (is_elf(elfname))
    {
        exp = get_elf_start_addr(elfname, &start_addr, &code_start_addr, &code_end_addr);
    } else if (is_coff(elfname))
    {
        exp = get_coff_start_addr(elfname, &start_addr, &code_start_addr, &code_end_addr);
    } else if (is_hex(elfname))
    {
        exp = get_hex_start_addr(elfname, &start_addr, &code_start_addr, &code_end_addr);
    } else
    {
        exp = Invarg_exp;
    }

    if (exp != No_exp)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get Binary Start Address Failed!\n");
        return exp;
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
        skyeye_log(Error_log, __FUNCTION__, "Load binary File Failed!\n");
        return exp;
    }

    SIM_set_pc(core, start_addr, code_start_addr, code_end_addr);

    sys_cpu_t *cpu = (sys_cpu_t *) (core->sys_struct);
    sys_soc_t *soc = cpu->sys_soc;

    sprintf(soc->binary_path, "%s", elfname);
    return No_exp;
}

/*load-bin-binary command */
int WIN_load_bin_binary(char *cpuname, const char *filename, generic_address_t load_addr, int length, int start_pc)
{
    conf_object_t *core = get_conf_obj(cpuname);
    int ret;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find obj [%s]\n", cpuname);
        return Error;
    }

    ret = load_file(core, filename, load_addr);
    if (ret == No_exp)
    {
        int code_start_addr = load_addr;
        int code_end_addr = load_addr + length;

        SIM_set_pc(core, start_pc, code_start_addr, code_end_addr);
        return Ok;
    }

    return Error;
}

/*load-file command */
int WIN_load_file(char *cpuname, const char *filename, generic_address_t load_addr)
{
    conf_object_t *core = get_conf_obj(cpuname);
    int ret;

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find obj [%s]\n", cpuname);
        return Error;
    }

    ret = load_file(core, filename, load_addr);
    if (ret != No_exp)
    {
        return Error;
    }

    return Ok;
}

/*parse-symbol command */
int32_t WIN_Parse_Symbol(char *cpuname, char *hex_file_name)
{
    generic_address_t start_addr;
    generic_address_t code_start_addr, code_end_addr;
    exception_t exp;

    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return Error;
    }

    parse_symbol_xml(cpuname, hex_file_name);

    // Debug: print all parse info
#if 0
    parse_xml_output();
#endif

    return 0;
}

static bool_t isLow = False;            //用来降低x86 cpu 时钟频率标志， ture为需要降低
void complete_init()
{
    skyeye_system_t *system = system_get();
    bool_t all_soc_loaded;
    sys_cpu_t *x86_cpu = NULL;
    int i, j;

    while (True)
    {
        while (!SIM_is_running())
        {
#ifdef __WIN32__
            Sleep(1000);
#else
            usleep(1000);
#endif
        }
        all_soc_loaded = True;
        for (i = 0; i < system->soc_cnt; i++)
        {
            sys_soc_t *soc = system->socs + i;

            if (soc == NULL)
            {
                skyeye_log(Error_log, __FUNCTION__, "Get soc[%d] failed.\n", i);
                continue;
            }

            if (!soc->delay_load_ms && !soc->loaded)
            {
                skyeye_cell_t *cell = create_cell();

                for (j = 0; j < soc->cpu_cnt; j++)
                {
                    sys_cpu_t *sys_cpu = &(soc->cpus[j]);

                    skyeye_exec_t *exec = create_exec();

                    exec->priv_data = (conf_object_t *) sys_cpu;
                    exec->run = system_cpu_cycle;
                    exec->stop = NULL;

                    add_to_cell(exec, cell);
                    if (sys_cpu->is_x86_arch)
                    {
                        x86_cpu = sys_cpu;
                    }
                }
                soc->loaded = True;
            } else if (soc->delay_load_ms)
            {
                all_soc_loaded = False;
                soc->delay_load_ms--;
            }
        }
        system->all_soc_loaded = all_soc_loaded;
        if (!system->all_soc_loaded)
        {
#ifdef __WIN32__
            Sleep(1000);                //sleep 1s
#else
            usleep(1000);
#endif
        } else
        {
            if (x86_cpu != NULL)
            {
                if (isLow)
                {
                    x86_cpu->core_info->set_x86_timer_low(x86_cpu->cpu, isLow);
                }
            }
            break;
        }
    }
    //call the init ok callback
    SKY_before_init_hap_occurred();
}


/*init-ok command */
int prepare_to_run(void)
{
    sky_pref_t *pref = get_skyeye_pref();
    int i, j;

    system_clock_calculation();
    skyeye_system_t *system = system_get();

    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc_t *soc = system->socs + i;
        sys_cpu_t *sys_cpu;

        skyeye_machine_intf *iface = (skyeye_machine_intf *) SKY_get_iface(soc->soc, MACHINE_INTF_NAME);

        if (iface && iface->update_boot_info)
            iface->update_boot_info(soc->soc);
    }

    //complete all socs
    pthread_t id;

    create_thread((void *) complete_init, NULL, &id);
    sim_record_machine();

    return 0;
}

/*slow-x86 command */
void slow_x86(char *cpu_name, uint32_t isLowFlag)
{
    isLow = isLowFlag;
    return;
}

char *get_instr_process_device(char *machname, char *cpuname, char *classname)
{
    skyeye_config_t *config = get_current_config();
    conf_object_t *soc = get_conf_obj(machname);
    conf_object_t *core = get_conf_obj(cpuname);
    conf_object_t *dev;
    char devName[100];
    int i;

    sys_soc_t *sys_soc = soc->sys_struct;
    for (i = 0; i < sys_soc->cpu_cnt; i++)
    {
        sys_cpu_t *sys_cpu = &(sys_soc->cpus[i]);
        if (sys_cpu->cpu == core)
        {
            sprintf(devName, "%s_instr_process_%d", sys_cpu->cpu->objname, i);
            dev = get_conf_obj(devName);
            if (dev)
            {
                return dev->objname;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Can't get %s object!\n", devName);
                return NULL;
            }
        }
    }

    skyeye_log(Error_log, __FUNCTION__, "Get cpuname:%s process device failed!\n", cpuname);
    return NULL;
}

bool_t WIN_get_instr_process_state(char *machname)
{
    conf_object_t *mach_obj = get_conf_obj(machname);
    sys_soc_t *sys_soc = mach_obj->sys_struct;

    return sys_soc->isEnableInstrProcessDevice;
}

/*enable-codecov command */
int WIN_enable_cpu_codecov(char *machname)
{
    conf_object_t *mach_obj = get_conf_obj(machname);
    sys_soc_t *sys_soc = mach_obj->sys_struct;
    conf_object_t *core, *device;
    int i;

    for (i = 0; i < sys_soc->cpu_cnt; i++)
    {
        core = sys_soc->cpus[i].cpu;
        char devName[50];

        sprintf(devName, "%s_instr_process_%d", core->objname, i);
        /*new instr_process device */
        device = pre_conf_obj(devName, "instr_process");
        if (device == NULL)
        {
            skyeye_log(Error_log, __func__, "Enable cpu codecov Failed!\n");
            return Error;
        }
        reset_conf_obj(device);
        system_register_dev(device, mach_obj);
        sys_soc->isEnableInstrProcessDevice = True;
        /*set instr_process device cpu attribute */
        if (!WIN_set_attr(devName, "cpu", "object", core->objname))
        {
            skyeye_log(Error_log, __func__, "Set instr_process cpu attribute Failed!\n");
            return Error;
        }
    }

    return Ok;
}

struct disable_device_info_t
{
    int num;
    conf_object_t *obj;
    conf_object_t *disable_obj;
    memory_space_intf *memory_space;
};

/*MAX Disable Device NUM: 50 MAX Disable Connect and Interface NUM:10 */
static struct disable_device_info_t disable_device_connect_info[50][10];
static struct disable_device_info_t disable_device_iface_info[50][10];
static struct disable_device_info_t disable_device_memory_info[50];
static int connect_cnt[50] = { 0 };
static int dev_cnt = 0;

void find_and_disable_device_connect(conf_object_t * device, conf_object_t * disable_device)
{
    conf_object_t *obj;
    char *port;
    int i;

    /*get device all connects */
    conf_class_t *dev_clss = SKY_get_class(device->class_name);
    const struct ConnectDescription *dev_connects = dev_clss->class_data.connect_list.connects;
    int count = dev_clss->class_data.connect_list.cnt;

    /*check all connects object is the disabled device object */
    for (i = 0; i < count; i++)
    {
        dev_connects[i].get(device, &obj, &port, 0);
        /* if disable device obj == connect obj */
        if (obj == disable_device)
        {
            int cnt = connect_cnt[dev_cnt];

            /* save disable obj and connect obj and connect num */
            disable_device_connect_info[dev_cnt][cnt].disable_obj = obj;
            disable_device_connect_info[dev_cnt][cnt].obj = device;
            disable_device_connect_info[dev_cnt][cnt].num = i;
            connect_cnt[dev_cnt]++;
            /* disable device this connect iface, and set NULL */
            dev_connects[i].set(device, NULL, NULL, 0);
        }
    }

    return;
}

void disable_device_memory_space(conf_object_t * memory, conf_object_t * device)
{
    addr_space_t *space = memory->obj;
    map_info_t *map;
    int i;

    for (i = 0; i < space->map_count; i++)
    {
        map = space->map_array[i];
        if (map->obj == device)
        {
            /*disable the device map */
            disable_device_memory_info[dev_cnt].disable_obj = device;
            disable_device_memory_info[dev_cnt].memory_space = map->memory_space;
            map->memory_space = NULL;
        }
    }
}

void disable_device_ifaces(conf_object_t * device)
{
    skyeye_system_t *system = system_get();
    sys_dev_t *sys_dev;
    sys_soc_t *sys_soc;
    sys_linker_t *sys_linker;
    int linker_cnt = system->linker_cnt;
    int i, j;

    /* check all soc to find the device to use the disabled device iface and set the iface to NULL */
    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc = system->socs + i;
        /* check all devices */
        for (j = 0; j < sys_soc->dev_cnt; j++)
        {
            sys_dev = sys_soc->devs + j;
            if (strcmp(sys_dev->dev->class_name, "memory_space") != 0)
            {
                find_and_disable_device_connect(sys_dev->dev, device);
            } else
            {
                /*disable device memory space iface */
                disable_device_memory_space(sys_dev->dev, device);
            }
        }
    }
    /* check linkers all connects and disable disabled device iface */
    for (j = 0; j < system->linker_cnt; j++)
    {
        sys_linker = system->linkers + j;
        find_and_disable_device_connect(sys_linker->linker, device);
    }

    return;
}

void disable_device_connects(conf_object_t * device)
{
    conf_class_t *clss = SKY_get_class(device->class_name);
    const struct ConnectDescription *connects = clss->class_data.connect_list.connects;
    int count = clss->class_data.connect_list.cnt;
    conf_object_t *obj;
    char *port;
    int i;

    for (i = 0; i < count; i++)
    {
        connects[i].get(device, &obj, &port, 0);
        disable_device_iface_info[dev_cnt][i].disable_obj = device;
        disable_device_iface_info[dev_cnt][i].obj = obj;
        if (obj != NULL)
        {
            connects[i].set(device, NULL, NULL, 0);
        } else
        {
            //do nothing
        }
    }
}

void enable_device_connects(conf_object_t * device)
{
    conf_class_t *clss = SKY_get_class(device->class_name);
    const struct ConnectDescription *connects = clss->class_data.connect_list.connects;
    int count = clss->class_data.connect_list.cnt;
    conf_object_t *obj;
    int i, j;

    for (i = 0; i < dev_cnt; i++)
    {
        if (disable_device_iface_info[i][0].disable_obj == device)
        {
            for (j = 0; j < count; j++)
            {
                obj = disable_device_iface_info[i][j].obj;
                if (obj != NULL)
                    connects[j].set(device, obj, NULL, 0);
                else
                {
                    //do nothing
                }
            }
        }
    }
}

void enable_device_memory_space(conf_object_t * memory, conf_object_t * device)
{
    addr_space_t *space = memory->obj;
    map_info_t *map;
    int i, j;

    for (i = 0; i < dev_cnt; i++)
    {
        if (disable_device_memory_info[i].disable_obj == device)
        {
            for (j = 0; j < space->map_count; j++)
            {
                map = space->map_array[j];
                if (map->obj == device)
                {
                    /*enable the device map */
                    map->memory_space = disable_device_memory_info[i].memory_space;
                }
            }
        }
    }
}

void find_and_enable_device_connect(conf_object_t * device, conf_object_t * enable_device)
{
    int i, j;

    for (i = 0; i < dev_cnt; i++)
    {
        if (disable_device_connect_info[i][0].disable_obj == enable_device)
        {
            for (j = 0; j < connect_cnt[i]; j++)
            {
                if (device == disable_device_connect_info[i][j].obj)
                {
                    conf_class_t *dev_clss = SKY_get_class(device->class_name);
                    const struct ConnectDescription *dev_connects = dev_clss->class_data.connect_list.connects;
                    int count = dev_clss->class_data.connect_list.cnt;
                    int num = disable_device_connect_info[i][j].num;

                    dev_connects[num].set(device, enable_device, NULL, 0);
                }
            }
        }
    }

    return;
}

void enable_device_ifaces(conf_object_t * device)
{
    skyeye_system_t *system = system_get();
    sys_dev_t *sys_dev;
    sys_soc_t *sys_soc;
    sys_linker_t *sys_linker;
    int linker_cnt = system->linker_cnt;
    int i, j;

    /* check all soc to find the device to use the enable device iface and set the iface to saved object */
    for (i = 0; i < system->soc_cnt; i++)
    {
        sys_soc = system->socs + i;
        /* check all devices */
        for (j = 0; j < sys_soc->dev_cnt; j++)
        {
            sys_dev = sys_soc->devs + j;
            if (strcmp(sys_dev->dev->class_name, "memory_space") != 0)
            {
                find_and_enable_device_connect(sys_dev->dev, device);
            } else
            {
                /*enable device memory space iface */
                enable_device_memory_space(sys_dev->dev, device);
            }
        }
    }
    /* check linkers all connects and enable disabled device iface */
    for (j = 0; j < system->linker_cnt; j++)
    {
        sys_linker = system->linkers + j;
        find_and_enable_device_connect(sys_linker->linker, device);
    }

    return;
}

/*disable-device-work command */
int WIN_disable_device_work(char *device_name)
{
    conf_object_t *device_obj = get_conf_obj(device_name);
    if (device_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", device_name);
        return Error;
    }

    /*disable device all ifaces */
    disable_device_ifaces(device_obj);
    /*disable device all connects */
    disable_device_connects(device_obj);
    dev_cnt++;

    return Ok;
}

/*enable-device-work command */
int WIN_enable_device_work(char *device_name)
{
    conf_object_t *device_obj = get_conf_obj(device_name);
    if (device_obj == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", device_name);
        return Error;
    }

    /*enable device all ifaces */
    enable_device_ifaces(device_obj);
    /*enable device all connects */
    enable_device_connects(device_obj);

    return Ok;
}

/*pre_translate command */
void do_pre_translate(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpuname);
        return;
    }

    core_info_intf *iface = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s.\n", CORE_INFO_INTF_NAME, cpuname);
        return;
    }

    return iface->do_pre_translate(core);
}

static char *skyeye_current_script_full_path = NULL;
static char *skyeye_current_script_path = NULL;

/*run-script command */
int WIN_set_script_path(char *path)
{
    if (path)
    {
        skyeye_current_script_full_path = skyeye_strdup(path);

        char *p;

        p = skyeye_current_script_full_path;
        while (*p)
        {
            if (*p == '\\')
                *p = '/';
            p++;
        }

        int length = strrchr(skyeye_current_script_full_path, '/') + 1 - skyeye_current_script_full_path;

        skyeye_current_script_path = skyeye_mm(length + 1);
        strncpy(skyeye_current_script_path, skyeye_current_script_full_path, length);
        skyeye_current_script_path[length] = '\0';
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Set current path error, path is NULL\n");
        return -1;
    }

    return 0;
}

char *WIN_get_work_full_path()
{
    if (skyeye_current_script_path)
    {
        return skyeye_current_script_full_path;
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Get current full path error\n");
        return NULL;
    }
}

char *WIN_get_work_path()
{
    if (skyeye_current_script_path)
    {
        return skyeye_current_script_path;
    } else
    {
        skyeye_log(Warning_log, __FUNCTION__, "Get current path error\n");
        return NULL;
    }
}

int record_count_id = 0;
int WIN_open_instr_record(char *cpu_name, char *filename)
{
    conf_object_t *core = get_conf_obj(cpu_name);
    conf_object_t *device;
    char devName[100] = {0};

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find obj [%s]\n", cpu_name);
        return Error;
    }

    sprintf(devName, "%s_instr_process%d", core->objname, record_count_id++);
    /*new instr_process device */
    device = pre_conf_obj(devName, "instr_process");
    if (device == NULL)
    {
        skyeye_log(Error_log, __func__, "Create %s Failed!\n", devName);
        return Error;
    }
    if (!WIN_set_attr(devName, "cpu", "object", core->objname))
    {
        skyeye_log(Error_log, __func__, "Set instr_process cpu attribute Failed!\n");
        return Error;
    }

    if (!WIN_set_attr(devName, "filename", "string", filename))
    {
        skyeye_log(Error_log, __func__, "Set instr_process recording instructions filename Failed!\n");
        return Error;
    }

    return Ok;
}

int WIN_close_instr_record(char *cpu_name)
{
    conf_object_t *core = get_conf_obj(cpu_name);
    conf_object_t *device;
    char devName[100] = {0};

    if (core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find obj [%s]\n", cpu_name);
        return Error;
    }

    if (record_count_id < 1)
    {
        skyeye_log(Error_log, __func__, "Please open-instr-record First!\n");
        return Error;
    }

    sprintf(devName, "%s_instr_process%d", core->objname, (record_count_id - 1));
    device = get_conf_obj(devName);

    if (device == NULL)
    {
        skyeye_log(Error_log, __func__, "Open %s Failed!\n", devName);
        return Error;
    }

    reset_conf_obj(device);

    return Ok;
}
