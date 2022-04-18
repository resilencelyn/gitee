
/*************************************************************************
	> File Name    : com_command.c
	> Author       : jiachao
	> Mail         : jiachao@digiproto.com
	> Created Time : 2020/11/17 11:34:34
************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <sim_control.h>
#include <skyeye_command.h>
#include <skyeye_mm.h>
#include <skyeye_cli.h>
#include "skyeye_core_intf.h"
#include <skyeye_iface.h>
#include <skyeye_core.h>
#include <skyeye_pref.h>
#include <json_conf.h>
#include "skyeye_readline_cli.h"
#include <libgen.h>

int com_run(arg)
    char *arg;
{
    if (!arg)
    {
        arg = "";
    }

    SIM_run();
    return 0;
}

/*
 * Continue running of the interrupted SkyEye
 */
int com_cont(arg)
    char *arg;
{
    if (!arg)
    {
        arg = "";
    }

    SIM_continue(NULL);
    return 0;
}

/*
 * stop running of  SkyEye
 */
int com_stop(arg)
    char *arg;
{
    if (!arg)
    {
        arg = "";
    }

    SIM_stop(NULL);

    return 0;
}

/*
 * stop running of  SkyEye
 */

int com_start(arg)
    char *arg;
{
    int flag = 0;

    SIM_start();

    return flag;
}

/*
 * step run of SkyEye
 */
int com_si(char *arg)
{
    int flag = 0;
    char **endptr;
    int steps;

    if (arg == NULL || *arg == '\0')
    {
        steps = 1;
    } else
    {
        errno = 0;
        steps = strtoul(arg, endptr, 10);
    }

    skyeye_stepi(steps);
    run_command("disassemble");

    return flag;
}

int com_x(char *arg)
{
    int flag = 0;
    char **endptr;
    char result[64];
    int addr, size;
    uint32 value;

    if (arg == NULL || *arg == '\0')
    {
        return Invarg_exp;
    } else
    {
        flag = get_parameter(result, arg, "addr");
        if (flag > 0)
        {
            addr = strtoul(result, NULL, 0);
        } else
        {
            printf("Format error: x addr=xxxx size=xxxx\n");
            return -1;
        }

        flag = get_parameter(result, arg, "size");

        if (flag > 0)
        {
            size = strtoul(result, NULL, 0);
        } else
        {
            printf("size default setting 1 word\n");
            size = 1;
        }
    }

    int i;

    for (i = 1; i <= size; i++)
    {
        bus_read(32, addr, &value);
        printf("0x%x:0x%x\t", addr, value);
        addr += 4;
        if (i % 5 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");

    return 0;
}

/* String to pass to system ().  This is for the LIST, VIEW and RENAME
 * commands. */
static char syscom[1024];

/* List the file(s) named in arg. */
int com_list(arg)
    char *arg;
{
    if (!arg)
    {
        arg = "";
    }

    sprintf(syscom, "ls -FClg %s", arg);
    return (system(syscom));
}

int com_view(arg)
    char *arg;
{
    if (!valid_argument("view", arg))
    {
        return 1;
    }
#if defined (__MSDOS__)
    /* more.com doesn't grok slashes in pathnames */
    sprintf(syscom, "less %s", arg);
#else
    sprintf(syscom, "more %s", arg);
#endif
    return (system(syscom));
}

int com_rename(arg)
    char *arg;
{
    too_dangerous("rename");
    return (1);
}

int com_stat(arg)
    char *arg;
{
    struct stat finfo;

    if (!valid_argument("stat", arg))
    {
        return (1);
    }

    if (stat(arg, &finfo) == -1)
    {
        perror(arg);
        return (1);
    }

    printf("Statistics for `%s':\n", arg);

    printf("%s has %d link%s, and is %d byte%s in length.\n",
           arg, finfo.st_nlink, (finfo.st_nlink == 1) ? "" : "s", finfo.st_size, (finfo.st_size == 1) ? "" : "s");
    printf("Inode Last Change at: %s", ctime(&finfo.st_ctime));
    printf("      Last access at: %s", ctime(&finfo.st_atime));
    printf("    Last modified at: %s", ctime(&finfo.st_mtime));

    return (0);
}

int com_delete(arg)
    char *arg;
{
    too_dangerous("delete");
    return (1);
}

/* Print out the current working directory. */
int com_pwd(ignore)
    char *ignore;
{
    char dir[1024], *s;

    s = getcwd(dir, sizeof (dir) - 1);
    if (s == 0)
    {
        printf("Error getting pwd: %s\n", dir);
        return 1;
    }

    printf("Current directory is %s\n", dir);
    return 0;
}

/* Change to the directory ARG. */
int com_cd(arg)
    char *arg;
{
    if (chdir(arg) == -1)
    {
        perror(arg);
        return 1;
    }

    com_pwd("");
    return (0);
}

/* The user wishes to quit using this program.  Just set DONE non-zero. */
int com_quit(arg)
    char *arg;
{
    SIM_quit();

    return 0;
}

int com_list_modules(char *arg)
{
    char *format = "%-20s\t%s\n";

    printf(format, "Module Name", "File Name");
    skyeye_module_t *list = get_module_list();

    while (list != NULL)
    {
        printf(format, list->module_name, list->filename);
        list = list->next;
    }

    return 0;
}

int com_list_options(char *arg)
{
    char *format = "%-20s\t%s\n";

    printf(format, "Option Name", "Description");
    skyeye_option_t *list = get_option_list();

    while (list != NULL)
    {
        printf(format, list->option_name, list->helper);
        list = list->next;
    }
    return 0;
}

int com_list_machines(char *arg)
{
    char *format = "%-20s\n";

    printf(format, "Machine Name");
    machine_config_t *list = get_mach_list();

    while (list != NULL)
    {
        //printf("option_name = %s\n", list->option_name);
        printf(format, list->machine_name);
        list = list->next;
    }
    return 0;
}

int com_show_pref(char *arg)
{
    char *format = "%-30s\t%s\n";
    char *int_format = "%-30s\t0x%x\n";
    sky_pref_t *pref = get_skyeye_pref();

    printf(format, "Module search directorys:", pref->module_search_dir);
    printf(int_format, "Boot address:", pref->start_address);
    printf(format, "Executable file:", pref->exec_file);
    printf(int_format, "Load base for executable file:", pref->exec_load_base);
    printf(int_format, "Load mask for executable file:", pref->exec_load_mask);
    printf(format, "SkyEye config file:", pref->conf_filename);
    printf(format, "Endian of exec file:", pref->endian == Big_endian ? "Big Endian" : "Little endian");
    return 0;
}

int com_show_map(char *arg)
{
    mem_bank_t *bank;
    mem_config_t *memmap = get_global_memmap();
    char *format1 = "%-30s\t%s\t%20s\n";

    printf(format1, "Start Addr", "Length", "Type");
    char *format2 = "0x%-30x\t0x%x\t%20s\n";

    for (bank = memmap->mem_banks; bank->len; bank++)
        printf(format2, bank->addr, bank->addr + bank->len, bank->type ? "memory" : "IO");
    return 0;
}

int com_load_module(char *arg)
{
    return 0;
}

int com_info(char *arg)
{
    char *info_target[] = { "registers", "breakpoint" };
    if (arg == NULL)
    {
        printf("Available options is %s, %s\n", info_target[0], info_target[1]);
        return 0;
    }
    return 0;
}

int com_restart(char *arg)
{
    SIM_restart();
    return 0;
}

int com_set_all(char *arg)
{
    char *info_target[] = { "reg", "addr" };
    char result[64];
    int i = 0;

    if (arg == NULL)
    {
        printf("Available options is %s, %s\n", info_target[0], info_target[1]);
        return 0;
    }
    for (i = 0; i < 2; i++)
    {
        if (!strncmp(arg, info_target[i], strlen(info_target[i])))
        {
            break;
        }
    }
    //generic_arch_t* arch_instance = get_arch_instance(NULL);
    skyeye_reg_intf *reg_intf = SIM_get_current_reg_intf();

    switch (i)
    {
        case 0:
            i = get_parameter(result, arg, "reg");
            char *reg_name[64];
            int reg_value;

            if (i > 0)
            {
                strcpy(reg_name, result);
                //printf("set reg = %s\t", reg_name);
            }

            i = get_parameter(result, arg, "value");
            if (i > 0)
            {
                reg_value = strtoul(result, NULL, 0);
                //printf("value = 0x%x\n", reg_value);
            }

            uint32 reg_id = reg_intf->get_regid_by_name(reg_intf->conf_obj, reg_name);

            reg_intf->set_regvalue_by_id(reg_intf->conf_obj, reg_value, reg_id);
            break;
        case 1:
            i = get_parameter(result, arg, "addr");
            int addr, value, size;

            if (i > 0)
            {
                addr = strtoul(result, NULL, 0);
                //printf("set addr = 0x%x\t", addr);
            } else
            {
                printf("Format error: set addr = xxxx value = xxxxx size = xxxx\n");
                return -1;
            }

            i = get_parameter(result, arg, "value");
            if (i > 0)
            {
                value = strtoul(result, NULL, 0);
                //printf("value = 0x%x\t", value);
            } else
            {
                printf("Format error: set addr = xxxx value = xxxxx size = xxxx\n");
                return -1;
            }

            i = get_parameter(result, arg, "size");
            if (i > 0)
            {
                size = strtoul(result, NULL, 0);
                //printf("size = 0x%x\n", size);
            } else
            {
                printf("default setting size = 4\n");
                size = 4;
            }
            switch (size)
            {
                case 1:
                    bus_write(8, addr, value);
                    break;
                case 2:
                    bus_write(16, addr, value);
                    break;
                case 4:
                    bus_write(32, addr, value);
                    break;
                default:
                    printf("size value should be 1 , 2, 4!\n");
                    break;
            }
            break;
        default:
            printf("set command parameter error\n");
            return -1;
    }
    return 0;
}

int com_load_conf(char *arg)
{
    int ret;
    sky_pref_t *pref = get_skyeye_pref();
    char *filename = basename(arg);
    skyeye_config_t *config = get_current_config();

    config->running_mode = GUIMODE;

    if (!strcmp(filename, JSON_CONF_NAME))
    {
        ret = json_conf(JSON_CONF_NAME, NULL);
        if (ret == -1)
        {
            printf("Can not parse the config file  %s correctly.\n ", arg);
        }
        return ret;
    }

    if ((ret = skyeye_read_config(arg)) != No_exp)
    {
        if (ret == Conf_format_exp)
        {
            printf("Can not parse the config file  %s correctly.\n ", arg);
        }
        if (ret == File_open_exp)
            printf("Can not open the config file %s\n", arg);
        else
            printf("Unknown error when read config from the file %s\n", arg);
    }

    pref->conf_filename = NULL;

    return 0;
}

int com_reset(char *arg)
{
    //generic_arch_t* arch_instance = get_arch_instance("");
    skyeye_config_t *config = get_current_config();

    /* get the current preference for simulator */
    sky_pref_t *pref = get_skyeye_pref();

    /* reset current arch_instanc */

    //arch_instance->reset();
    printf("reset not implemented.\n");
    /* reset all the values of mach */
    config->mach->mach_io_reset(NULL);
    generic_address_t pc = (config->start_address & pref->exec_load_mask) | pref->exec_load_base;

    skyeye_log(Info_log, __FUNCTION__, "Set PC to the address 0x%x\n", pc);
    //arch_instance->set_pc(pc);

    return 0;
}

int com_save_image(char *devname)
{
    conf_object_t *obj;
    skyeye_image_info *image_info;
    memory_space_intf *mm_space_intf;
    attr_value_t attr;
    uint32_t size, page_size, cnt, ret;
    time_t tm;
    struct tm *ptm;

    /* suffix: 20160714200856, filename: ${devname}.${suffix} */
    char suffix[15], filename[128], fullname[512];
    FILE *pfile;
    char *one_page;
    uint32_t page_count, i;
    attr_value_t save_path_attr;

    obj = (conf_object_t *) get_conf_obj(devname);

    image_info = (skyeye_image_info *) SKY_get_iface(obj, SKYEYE_IMAGE_INFO);
    mm_space_intf = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);
    attr = SKY_get_attribute(obj, "size");
    save_path_attr = SKY_get_attribute(obj, "save_path");

    if (image_info == NULL || mm_space_intf == NULL || attr.type == Val_Invalid)
    {
        printf("[%s:%s:%d]: image_info or mm_space_intf or get_attribute faild.\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    tm = time(NULL);
    ptm = localtime(&tm);

    strftime(suffix, sizeof (suffix), "%Y%m%d%H%M%S", ptm);
    snprintf(filename, sizeof (filename), "%s.%s", devname, suffix);
    snprintf(fullname, sizeof (fullname), "%s%s", SKY_attr_string(save_path_attr), filename);

    if ((pfile = fopen(fullname, "wb")) == NULL)
    {
        printf("[%s:%s:%d]: fopen error.\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    size = SKY_attr_uinteger(attr);
    page_count = image_info->get_page_count(obj);
    page_size = image_info->get_page_size(obj);

    printf("DEVNAME: %s\tIMAGE BANK_SIZE(PAGE_SIZE): 0x%x, PAGE_COUNT: %u\tACTUAL_TOTAL_SIZE: 0x%x\n",
           devname, page_size, page_count, size);

    for (i = 0; i < page_count; i++)
    {
        one_page = image_info->get_page_content(obj, i);

        cnt = 0;
        do
        {
            ret = fwrite(&one_page[cnt], 1, page_size - cnt, pfile);
            if (ret == 0)
            {
                printf("[%s:%s:%d]: fwrite error.\n", __FILE__, __FUNCTION__, __LINE__);
                return -1;
            }
            cnt += ret;
        } while (cnt < page_size);
    }

    fclose(pfile);

    printf("Save done!\n");

    return 0;
}
