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
 * @file sim_ctrl.c
 * @brief the control function set of the simulator
 * @author Michael.Kang blackfin.kang@gmail.com
 * @version 
 * @date 2011-04-30
 */

#include <stdlib.h>
#include <sim_control.h>
#include <Python.h>
#include "skyeye_types.h"
#include "skyeye_config.h"
#include "skyeye_pref.h"
#include "skyeye_module.h"
#include "skyeye_arch.h"
#include "skyeye_callback.h"
#include "skyeye_cell.h"
#include "skyeye_internal.h"
#include "skyeye_checkpoint.h"
#include "skyeye_mm.h"
#include "skyeye_command.h"
#include "skyeye_loader.h"
#include "skyeye_symbol.h"
#include "skyeye_log.h"
#include "skyeye_cli.h"
#include "skyeye_class.h"
#include "portable/portable.h"
#include "json_conf.h"
#include "skyeye_mach.h"
#include "skyeye_internal.h"
#include "skyeye_system.h"
#include "parse_symbol.h"
#ifdef __WIN32__
#include <tchar.h>
#include <windows.h>
#endif

/* FIXME, we should get it from prefix varaible after ./configure */
#ifndef SKYEYE_MODULE_DIR
const char *default_lib_dir = "/opt/skyeye/lib/skyeye/";
#else
const char *default_lib_dir = SKYEYE_MODULE_DIR;
#endif

static struct skyeye_sim_ctrl sim_ctrl;

#ifdef __MINGW32__
int setenv(const char *name, const char *value, int overwrite)
{
    int result = 0;

    if (overwrite || !getenv(name))
    {
        size_t length = strlen(name) + strlen(value) + 2;
        char string[1024];
        assert(length < 1024);
        snprintf(string, length, "%s=%s", name, value);
        result = putenv(string);
    }

    return result;
}
#endif

int cli(const char* prompt, const char* net_port)
{
    char new_path[1024];
    char cmdstring[512];
    char* skyeye_bin = SKYEYE_BIN;

#ifndef __MINGW32__
    setenv("SKYEYEBIN", skyeye_bin, 1);
#endif
    Py_Initialize();
    sprintf(new_path, "sys.path.append(\"%s\")\n", skyeye_bin);
    PyRun_SimpleString("import sys\n");
    PyRun_SimpleString(new_path);

    if(net_port != NULL)
    {
        sprintf(cmdstring,"import grpc_server\ngrpc_server.set_net_port(\"%s\")\n",net_port);
        PyRun_SimpleString(cmdstring);
    }
    PyRun_SimpleString("from skyeye_cli import *\nSkyEyeCli().cmdloop()\n");

    Py_Finalize();
    return 0;
}

int gui(const char* prompt)
{
    char new_path[1024];
    char* skyeye_bin = SKYEYE_BIN;

#ifndef __MINGW32__
    setenv("SKYEYEBIN", skyeye_bin, 1);
#endif
    Py_Initialize();
    PyRun_SimpleString("import sys, os, wx\n");
    sprintf(new_path, "sys.path.append(\"%s\")\n", skyeye_bin);
    PyRun_SimpleString(new_path);
    PyRun_SimpleString("import skyeye_gui\n");
    PyRun_SimpleString("app = wx.PySimpleApp()\n");
    PyRun_SimpleString("GUI = skyeye_gui.MainFrame(parent = None)\n");
    PyRun_SimpleString("GUI.Show()\n");
    PyRun_SimpleString("app.MainLoop()\n");
    Py_Finalize();

    return 0;
}

/**
 * @brief the default cell of the simulator
 */
static skyeye_cell_t *default_cell = NULL;

/**
 * @brief the flag of running or stop
 */

void SIM_init_command_line(void)
{
}

/**
 * @brief initialization of environment
 *
 * @param argv
 * @param handle_signals
 */
void SIM_init_environment(char **argv, bool_t handle_signals)
{
}

void SIM_main_loop(void)
{
}

/* we try to do init in batch mode */
static exception_t try_init()
{
    return No_exp;
}

//void SIM_cli();
cli_func_t global_cli = NULL;
cli_func_t global_gui = NULL;
cli_func_t global_silence = NULL;

#ifdef __WIN32__
wchar_t* c2w(const char *str)
{
    int length = strlen(str) + 1;
    wchar_t *t = (wchar_t*)malloc(sizeof(wchar_t) * length);

    memset(t, 0, length * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, str, strlen(str), t, length);

    return t;
}

void SIM_set_python_home()
{
    int i;
    char command[1024];
    char *skyeye_bin = get_bin_env_path("BIN");

    if (!skyeye_bin)
    {
        printf("SKYEYEBIN env var not found.");
        exit(0);
    }
    char python_home[1024];

    for (i = 0; i < strlen(skyeye_bin); i++)
    {
        if (skyeye_bin[i] == '\\')
        {
            skyeye_bin[i] = '/';
        }
    }
    sprintf(python_home, "%s%s", skyeye_bin, "/../../../lib/python35");
    Py_SetPythonHome(c2w(python_home));
}
#endif

void load_all_modules_func(char *module_dir)
{
    /*
     * loading all the modules in search directory
     */
    if (!module_dir)
#ifndef __WIN32__
    {
        module_dir = skyeye_strdup(default_lib_dir);
    }
#else
    {
        char lib_dir[1024] = { '\0' };
        char *skyeyebin = get_bin_env_path("BIN");

        if (strlen(skyeyebin) + strlen("/../lib/skyeye") + 1 > 1024)
        {
            printf("The lib dir name is too long\n");
            skyeye_log(Error_log, __FUNCTION__, "The lib dir name is too log\n");
            exit(0);
        }
        strcat(lib_dir, skyeyebin);
        strcat(lib_dir, "/../lib/skyeye");
        module_dir = skyeye_strdup(lib_dir);
    }
#endif
    SKY_load_all_modules(module_dir, NULL);
}
/**
 * @brief all the initilization of the simulator
 */
void SIM_init()
{
    sky_pref_t *pref;
    char *welcome_str = get_front_message();

    /*
     * initialization of load machine info.
     */
    init_sim_machine();
    
#ifdef __WIN32__
    SIM_set_python_home();
#endif
    atexit(SIM_fini);
    /*
     * initialization of callback data structure, it needs to 
     * be initialized at very beginning.
     */
    init_callback();
    SKY_hap_init();

    /*
     * initialization of system ctructure
     * be initialized at very beginning.
     */
    system_init();

    /*
     * initilize the data structure for command
     * register some default built-in command
     */
    init_command_list();

    /*
     * initialization of module manangement 
     */
    init_module_list();

    /*
     * initialization the timer scheduler 
     */
    init_thread_scheduler();
    //init_timer_scheduler();

    /*
     * initialization of common class module
     */
    init_common_class();

    /*
     * initialization of breakpoint, that depends on callback module.
     */
    init_bp();

    /*
     * initialization of breakpoint, that depends on callback module.
     */
    init_chp();


    /* 
     * get the current preference for simulator 
     */
    pref = get_skyeye_pref();
    register_cli(cli);
    register_gui(gui);

    pthread_t id;
    create_thread((void *) load_all_modules_func, pref->module_search_dir, &id);
    /* save the original termios */
    struct termios tmp;

    tcgetattr(0, &tmp);
    memcpy(&pref->saved_term, &tmp, sizeof (struct termios));

    /*
     * if we run simulator in GUI or external IDE, we do not need to
     * launch our CLI.
     */
    if (pref->gui_mode == True)
    {
        if (global_gui != NULL)
        {
            global_gui("SkyEye", NULL);
            exit(0);
        } else
            printf("No gui found\n");
    }
    if (pref->interactive_mode == True)
    {
        if (global_cli != NULL)
            global_cli("SkyEye", pref->net_port);
        else
            printf("No cli found\n");
    }
    if (pref->autoboot == True)
    {
        char cmdstr[256] = { '\0' };
#ifdef __WIN32__
        char lib_path[1024] = { '\0' };
        strcat(lib_path, get_ws_path());
        if (strlen(lib_path) > 0)
        {
            strcat(lib_path, "/win32/lib");
            SKY_load_all_modules(lib_path, NULL);
        }
#endif
        sprintf(cmdstr, "run-script %s", pref->script_file);
        RunCommand(cmdstr);
        SIM_run();
        while (1) ;
    }
    if (pref->autotest == True)
    {
        RunAutotest();
    }
    if (pref->autoload == True)
    {
        char cmdstr[256] = { '\0' };
#ifdef __WIN32__
        char lib_path[1024] = { '\0' };
        strcat(lib_path, get_ws_path());
        if (strlen(lib_path) > 0)
        {
            strcat(lib_path, "/win32/lib");
            SKY_load_all_modules(lib_path, NULL);
        }
#endif
        sprintf(cmdstr, "run-script %s", pref->script_file);
        RunCommand(cmdstr);
        while (1) ;
    }
}

/**
 * @brief launch the simlator
 */
exception_t SIM_start(void)
{
    if (sim_ctrl.SIM_started)
    {
        printf("SkyEye has been started\n");
        return Unknown_exp;
    }
    sim_ctrl.SIM_started = True;
    return No_exp;
}

/**
 * @brief reset the simlator
 */
void SIM_reset(void)
{
    /*set skyeye status to stop */
    sim_ctrl.SIM_running = False;

    free_core_symbol_info_list();

    /*stop all cell */
    stop_all_cell();

    /*destroy all thread */
    destroy_threads();

    /*destroy all objects */
    SKY_delete_all_obj();

    /*clear system struct */
    system_clear();

    /*init new thread fuction */
    init_threads();

    /*init new thread scheduler thread */
    init_thread_scheduler();

    /*init system struct */
    system_init();

    /*clear load machine info */
    sim_reset_machine();
}

/**
 * @brief quit the simlator
 */
void SIM_quit(void)
{
    /*set skyeye status to stop */
    //exit(0); //Fix me gpio stop destroy when add lock
    sim_ctrl.SIM_running = False;

    /*stop all cell */
    stop_all_cell();

    /*destroy all thread */
    destroy_threads();

    /*destroy all obects */
    SKY_delete_all_obj();

    /*clear system struct */
    system_clear();

    /*quit skyeye */
    exit(0);
}

void init_sim_machine(void)
{
    sim_ctrl.is_load_machine = UNLOADE_MACH;
    sim_ctrl.SIM_running = False;
    sim_ctrl.SIM_started = False;
}

void sim_reset_machine(void)
{
    sim_ctrl.is_load_machine = UNLOADE_MACH;
    sim_ctrl.SIM_running = False;
    sim_ctrl.SIM_started = False;
}

void sim_record_machine(void)
{
    sim_ctrl.is_load_machine = LOADED_MACH;
}

void sim_clear_machine(void)
{
    sim_ctrl.is_load_machine = LOADED_MACH;
}

int sim_find_machine(void)
{
    return sim_ctrl.is_load_machine;
}


/**
 * @brief all the cli
 */
void SIM_cli()
{
    skyeye_cli();
}

/**
 * @brief set the running state for the simulator
 */
int SIM_run()
{
    if (sim_find_machine() == LOADED_MACH)
    {
        sim_ctrl.SIM_running = True;
        start_all_cell();
        return 0;  /* ok */
    } else
    {
        sim_ctrl.SIM_running = False;
        return 1;  /* nok */
    }

}

/**
 * @brief continue the last stop state
 *
 * @param arch_instance
 */
void SIM_continue(generic_arch_t * arch_instance)
{
    //skyeye_continue();
    sim_ctrl.SIM_running = True;
    start_all_cell();
}

/**
 * @brief stop the simulator
 *
 * @param arch_instance
 */
int SIM_stop(generic_arch_t * arch_instance)
{
    //skyeye_pause();
    
    if (sim_find_machine() == LOADED_MACH)
    {
        sim_ctrl.SIM_running = False;
        stop_all_cell();
        return 0; /* ok */
    } else
    {
        return 1; /* nok */
    }
}

/**
 * @brief if the simulator is in running state
 *
 * @param arch_instance
 */

bool_t SIM_is_running()
{
    return sim_ctrl.SIM_running;
}

/**
 * @brief destructor of the simulator
 */
extern void skyeye_erase_map(void);
void SIM_fini()
{
    sky_pref_t *pref = get_skyeye_pref();

    /* unload all the module */
    SKY_unload_all_modules();
    /* free the memory */
#ifndef __MINGW32__
    skyeye_erase_map();
#endif
#ifdef IS_MM_TEST
    skyeye_save_mm_info();
    skyeye_save_mm_result();
#endif
    printf("exit.\n");

#ifndef __MINGW32__
    /* restore the environment */
    tcsetattr(0, TCSANOW, &pref->saved_term);
#else
    tcsetattr(0, TCSAFLUSH, &pref->saved_term);
#endif

    return;
}

exception_t space_obj_free(addr_space_t * addr_space);
void reset_arch(void);
void del_from_cell(void);

/* 
 * Reatarting will close the skyeye process with the 100
 * The skyeye_controler detects number of skyeye exiting is 100
 * skyeye_controler will restart skyeye process
 */
void SIM_restart(void)
{
    if (!sim_ctrl.SIM_started)
    {
        printf("SkyEye is very pure, you don't need to restart it!\n");
        return;
    }
#if 0
    skyeye_config_t *config = get_current_config();
    generic_arch_t *arch_instance = get_arch_instance("");

    if (config->mach == NULL)
    {
        return;
    }
    if (config->mach->mach_fini)
        config->mach->mach_fini(arch_instance, config->mach);

    SIM_stop(arch_instance);
    space_obj_free(config->mach->phys_mem);
    /* Call SIM_exit callback */
    exec_callback(SIM_exit_callback, arch_instance);
#endif
    sky_pref_t *pref = get_skyeye_pref();

    printf("Destroy threads.\n");
    //destroy_threads();
    /* unload all the module */
    SKY_unload_all_modules();

    reset_skyeye_config();
    /* reset mem map */
    reset_global_memmap();

    /* free exec from cell */
    del_from_cell();
    /* free the memory */
    skyeye_erase_map();
    /* reset arch */
    reset_arch();

    sim_reset_machine();

    if (!pref->module_search_dir)
#ifndef __MINGW32__
        pref->module_search_dir = skyeye_strdup(default_lib_dir);
#else
    {
        char lib_dir[1024] = { '\0' };
        char *skyeyebin = get_bin_env_path("BIN");

        if (strlen(skyeyebin) + strlen("/../lib/skyeye/") + 1 > 1024)
        {
            printf("The lib dir name is too long\n");
            exit(0);
        }
        strcat(lib_dir, skyeyebin);
        strcat(lib_dir, "/../lib/skyeye/");
        pref->module_search_dir = skyeye_strdup(lib_dir);
    }
#endif
    SKY_load_all_modules(pref->module_search_dir, NULL);
#ifndef __MINGW32__
    /* restore the environment */
    tcsetattr(0, TCSANOW, &pref->saved_term);
#else
    tcsetattr(0, TCSAFLUSH, &pref->saved_term);
#endif
    sim_ctrl.SIM_started = False;
}

#if 1
void register_cli(cli_func_t cli)
{
    global_cli = cli;
}

void register_gui(cli_func_t gui)
{
    global_gui = gui;
}

#endif
