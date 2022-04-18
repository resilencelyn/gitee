#ifndef __DEFAULT_COMMANDS_H__
#define __DEFAULT_COMMANDS_H__
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <skyeye_api.h>
#include "skyeye_class.h"
#include <skyeye_cli.h>
#include "skyeye_readline_cli.h"
#include "skyeye_module.h"
#include "skyeye_options.h"
#include "sim_control.h"
#include "skyeye_mach.h"
#include "skyeye_pref.h"
#include "skyeye_core.h"
#include "json_conf.h"
#include "bank_defs.h"
#include "skyeye_mm.h"
#include "skyeye_addr_space.h"
#include <skyeye_core_intf.h>
#include <skyeye_exec.h>
#include <json_conf.h>
#include <skyeye_cell.h>
#include <skyeye_iface.h>
#include <skyeye_attribute.h>
#include <skyeye_loader.h>
#include "breakpoint.h"
#include "skyeye_internal.h"
#include "skyeye_interface.h"
#include "skyeye_types.h"
#include "skyeye_checkpoint.h"
#include "skyeye_module.h"
#include "skyeye_int_register.h"
#include "skyeye_fault_injection.h"
#include "skyeye_x86devs_fin.h"
#include "skyeye_x86devs_intf.h"
#include "skyeye_callback.h"
#include "skyeye_system.h"
#include "systemc_exec.h"
#include "skyeye_misc.h"
#include "mm_monitor.h"

#include "parse_symbol.h"
#ifdef __WIN32__
#include <windows.h>
#endif
#include <Python.h>

int com_run(char *arg);
int com_cont(char *arg);
int com_stop(char *arg);
int com_si(char *arg);
int com_view(char *arg);
int com_rename(char *arg);
int com_delete(char *arg);
int com_list(char *arg);
int com_stat(char *arg);
int com_start(char *arg);
int com_cd(char *arg);
int com_pwd(char *arg);
int com_quit(char *arg);
int com_list_modules(char *arg);
int com_list_options(char *arg);
int com_list_machines(char *arg);
int com_show_pref(char *arg);
int com_load_module(char *arg);
int com_load_conf(char *arg);
int com_show_map(char *arg);
int com_info(char *arg);
int com_x(char *arg);
exception_t run_command(char *command_str);

bool_t WIN_get_instr_process_state(char *machname);
int WIN_set_attr(char *objname, char *key, char *attr_type, char *value);

#define Error 0
#define Ok 1
#endif
