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
* @file skyeye_log.c
* @brief the log module of skyeye
* @author Michael.Kang blackfin.kang@gmail.com
* @version
* @date 2011-04-30
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <config.h>
#include "skyeye_options.h"
#include "skyeye_config.h"
#include "skyeye_log.h"
#include "skyeye_types.h"
#include "skyeye_mm.h"
#include "skyeye_class.h"
#include "skyeye_interface.h"
#include <time.h>
#include "skyeye_internal.h"
/* shenoubang */
#ifndef __WIN32__
#include <execinfo.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include "Ring_fifo.h"

/**
* @brief the welcome message of cli
*/
const char *Front_message =
    "SkyEye is an Open Source project under GPL. All rights of different parts or modules are reserved by their author. Any modification or redistributions of SkyEye should not remove or modify the annoucement of SkyEye copyright. \n\
Get more information about it, please visit the homepage http://www.skyeye.org.\n\
Type \"help\" to get command list. \n ";

#define LOG_ASSERT(cond) assert(cond)

/**
* @brief get the welcome message
*
* @return
*/
char *get_front_message()
{
    //printf("%s\n%s", PACKAGE_STRING, Front_message);
    return Front_message;
}

/**
* @brief By default, only warnning and some error displayed
*/

#define MAX_LOGFILE_LEN  128
static log_level_t current_log_level = Quiet_log;
static char *current_log_file = NULL;

static char logfile_dir[MAX_LOGFILE_LEN] = { '\0' };

exception_t set_logfile_dir(char *dir)
{
    if (strlen(dir) >= MAX_LOGFILE_LEN)
        return Excess_range_exp;
    sprintf(logfile_dir, "%s", dir);
    return No_exp;
}
char *get_logfile_dir(void)
{
    if (logfile_dir[0] == '\0')
        return NULL;
    return logfile_dir;
}

/**
* @brief the handler for log option
*
* @param this_option
* @param num_params
* @param params[]
*
* @return
*/
int do_log_option(skyeye_option_t * this_option, int num_params, const char *params[])
{
    char name[MAX_PARAM_NAME], value[MAX_PARAM_NAME];
    log_level_t level = current_log_level;
    int i;

    for (i = 0; i < num_params; i++)
    {
        if (split_param(params[i], name, value) < 0)
        {
            SKYEYE_ERR("log_info: Error: log has wrong parameter \"%s\".\n", name);
            continue;
        }
        if (!strncmp("level", name, strlen(name)))
        {
            sscanf(value, "%d", &level);
            if (level < Quiet_log || level >= MAX_LOG_LEVEL)
            {
                SKYEYE_ERR("log_info: Error log level %d\n", level);
            } else
                current_log_level = level;
            //break;
        }
        if (!strncmp("logfile", name, strlen(name)))
        {
            current_log_file = skyeye_mm_zero(strlen(value) + 1);
            memcpy(current_log_file, value, strlen(value) + 1);
        }
    }
    return 0;
}

/**
* @brief the exception string for various exception type
*/
static char *exp_str[] = {
    "No exception",
    "Malloc failed",
    "Can not open file",
    "Can not open DLL",
    "Invalid argument",
    "Invalid module",
    NULL
};

/**
* @brief get the exception string for the given exception
*
* @param exp
*
* @return
*/
char *get_exp_str(exception_t exp)
{
    const char *no_exp_str = "No such exception";
    int exp_str_len = 0;

    while (exp_str[exp_str_len++] != NULL) ;
    /* the last element is NULL in exp_str */
    if (exp >= exp_str_len - 1)
        return no_exp_str;
    return exp_str[exp];
}

void skyeye_info(char *format, ...)
{
    va_list args;

    va_start(args, format);
    skyeye_log(Info_log, NULL, format, args);
    va_end(args);
    return;
}

void skyeye_debug(char *format, ...)
{
    va_list args;

    va_start(args, format);
    skyeye_log(Debug_log, NULL, format, args);
    va_end(args);
    return;
}

void skyeye_error(char *format, ...)
{
    va_list args;

    va_start(args, format);
    skyeye_log(Error_log, NULL, format, args);
    va_end(args);
    return;
}

void skyeye_backtrace()
{
    int j, nptrs;

#define SIZE 100
    void *buffer[SIZE];
    char **strings;

    /* shenoubang add win32 2012-6-12 */
#ifndef __WIN32__
    nptrs = backtrace(buffer, SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
     * would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL)
    {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
        printf("%s\n", strings[j]);

    free(strings);
#endif
}

static int GetLogPath(char *path, unsigned size)
{
    char *env;
    char ConfigPath[256];
    char *dir;
    int ret;

    if (dir = get_logfile_dir())
    {
        snprintf(path, size, "%s/run.log", dir);
        return 0;
    }
#ifndef __MINGW32__
    env = getenv("HOME");
    if (env == NULL)
        return -1;
    snprintf(ConfigPath, 256, "%s/.skyeye", env);
    ret = access(ConfigPath, 0);
    if (ret < 0)
    {
        ret = mkdir(ConfigPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    snprintf(path, size, "%s/.skyeye/run.log", env);
#else
    env = get_bin_env_path("BIN");
    if (env == NULL)
        return -1;
    snprintf(path, size, "%s/run.log", env);
#endif
    return 0;
}
static void RemoveLineBreak(char *line)
{
    int len;

    len = strlen(line);
    if (line[len - 1] == '\n')
        line[len - 1] = '\0';
    return;
}

static void GetTime(char *TimeBuff, unsigned size)
{
    time_t tt = time(NULL);
    struct tm *t = localtime(&tt);

    sprintf(TimeBuff, "%d-%02d-%02d %02d:%02d:%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    return;
}
static void WriteLogToFile(char *log)
{
    char path[1024] = { '\0' };
    if (GetLogPath(path, 1024) < 0)
        return;
    FILE *fp = fopen(path, "a");

    fwrite(log, strlen(log), 1, fp);
    fflush(fp);
    fclose(fp);
    return;
}

static RING_FIFO *LOG_Fifo = NULL;
int New_Skyeye_log_fifo()
{
    if (LOG_Fifo == NULL)
    {
        LOG_Fifo = Ring_FIFO_create(2000, 1024);
        return 1;
    }
    return 0;
}

/**
* @brief log function of skyeye
*
* @param log_level
* @param func_name
* @param format
* @param ...
*/
void skyeye_log(log_level_t log_level, const char *func_name, const char *format, ...)
{
    static char OutPutInfo[1024] = {'\0'};
    char ErrorType[16] = {'\0'};
    char Time[64] = {'\0'};
    char ErrorString[1024] = {'\0'};
    va_list args;

    va_start(args, format);
    vsprintf(ErrorString, format, args);
    va_end(args);
    switch (log_level)
    {
        case Info_log:
        case Quiet_log:
        case Debug_log:
            snprintf(ErrorType, 16, "%s", "[INFO]");
            break;
        case Warning_log:
        case Critical_log:
            snprintf(ErrorType, 16, "%s", "[WARNING]");
            break;
        case Error_log:
            snprintf(ErrorType, 16, "%s", "[ERROR]");
            break;
        default:
            snprintf(ErrorType, 16, "%s", "[UNKNOWN]");
            break;
    }
    GetTime(Time, 64);
    snprintf(OutPutInfo, 1024, "[%s] %s [FUNCTION: %s] %s", Time, ErrorType, func_name, ErrorString);

    WriteLogToFile(OutPutInfo);

    if (LOG_Fifo == NULL)
    {
        if (New_Skyeye_log_fifo() == 0)
        {
            fprintf(stderr, "Function: new_skyeye_log_fifo() failed!\n");
            return;
        }
    }
    Ring_FIFO_write(LOG_Fifo, OutPutInfo);

    //printf("%s\n", OutPutInfo);
}

char *skyeye_get_next_log()
{
    char *str;

    if (LOG_Fifo == NULL)
    {
        return NULL;                    // Skyeye Log FIFO read error
    }
    if (!Ring_FIFO_read(LOG_Fifo, &str))
    {
        return NULL;                    // Skyeye log fifo is NULL
    }
    return str;
}

void skyeye_printf_in_color(COLOR_TYPE type, char *format, ...)
{
    static char buf[1024];

    memset(buf, '\0', 1024);
    va_list args;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    switch (type)
    {
        case BLACK:
            _debug_in_black("%s", buf);
            break;
        case BLUE:
            _debug_in_blue("%s", buf);
            break;
        case BROWN:
            _debug_in_brown("%s", buf);
            break;
        case CYAN:
            _debug_in_cyan("%s", buf);
            break;
        case DARK_GRAY:
            _debug_in_dark_gray("%s", buf);
            break;
        case GREEN:
            _debug_in_green("%s", buf);
            break;
        case LIGHT_BLUE:
            _debug_in_light_blue("%s", buf);
            break;
        case LIGHT_CYAN:
            _debug_in_light_cyan("%s", buf);
            break;
        case LIGHT_GRAY:
            _debug_in_light_gray("%s", buf);
            break;
        case LIGHT_GREEN:
            _debug_in_light_green("%s", buf);
            break;
        case LIGHT_PURPLE:
            _debug_in_light_purple("%s", buf);
            break;
        case LIGHT_RED:
            _debug_in_light_red("%s", buf);
            break;
        case PURPLE:
            _debug_in_purple("%s", buf);
            break;
        case RED:
            _debug_in_red("%s", buf);
            break;
        default:
            fprintf(stderr, "Not supported color type %d\n", type);
            break;
    }
}

static error_no_t skyeye_err_signal_no;
static char err_string[1024];

void set_err_info(error_no_t err, const char *err_str)
{
    skyeye_err_signal_no = err;
    int len = strlen(err_str);

    if (len < 1024)
        memcpy(err_string, err_str, len);
    else
        printf("the error log is too long!\n");

    return;
}

error_no_t get_err_no(void)
{
    return skyeye_err_signal_no;
}

char *get_err_str(void)
{
    return err_string;
}

char *get_log_file(void)
{
    return current_log_file;
}

/**
 * new interface
 * date: 2014-03-18
 */

#define DEFAULT_LOG_NAME "default_log_object"
void init_log_object(conf_object_t * obj)
{
    static conf_object_t *default_log_object = NULL;

    if (!default_log_object)
    {
        if (strcmp(obj->objname, DEFAULT_LOG_NAME) == 0)
        {
            return;
        }
        default_log_object = SKY_alloc_conf_object(DEFAULT_LOG_NAME, "stdlog");
        if (!default_log_object)
        {
            SKY_log_error(NULL, 0, "Can't load a standard log backend");
        }
    }

    /* configure log info */
    struct log_object *log;
    log = skyeye_mm_zero(sizeof (struct log_object));
    log->log_obj = default_log_object;
    if (log->log_obj)
    {
        log->log_intf = SKY_get_interface(log->log_obj, LOG_INTERFACE);
    }
    obj->log = log;
}

bool_t SKY_get_quiet(void)
{
    skyeye_config_t *config = get_current_config();

    return config->log.quiet;
}

bool_t SKY_get_verbose(void)
{
    skyeye_config_t *config = get_current_config();

    return !config->log.quiet;
}

void VT_log_message_fmt_va(int level, conf_object_t * obj, int grp, SKY_log_type_t typ, const char *fmt, va_list va)
{
    if (!obj)
    {
        skyeye_log(MAX_LOG_LEVEL - level, "Unknown", fmt, va);
        return;
    }
    if (!obj->log)
    {
        /* import level -> Critical_log etc */
        skyeye_log(MAX_LOG_LEVEL - level, obj->objname, fmt, va);
        return;
    }

    log_interface_t *intf = obj->log->log_intf;

    LOG_ASSERT(obj->log->log_obj && intf && intf->output);
    intf->output(obj->log->log_obj, obj, level, grp, typ, fmt, va);
}

void VT_log_message_fmt(int level, conf_object_t * obj, int grp, SKY_log_type_t typ, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    VT_log_message_fmt_va(level, obj, grp, typ, fmt, args);
    va_end(args);
}

void SKY_log_info(int level, conf_object_t * obj, int grp, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    VT_log_message_fmt_va(level, obj, grp, SKY_Log_Info, fmt, args);
    va_end(args);
}

void SKY_log_undefined(int level, conf_object_t * obj, int grp, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    VT_log_message_fmt_va(level, obj, grp, SKY_Log_Undefined, fmt, args);
    va_end(args);
}

void SKY_log_spec_violation(int level, conf_object_t * obj, int grp, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    VT_log_message_fmt_va(level, obj, grp, SKY_Log_Spec_Violation, fmt, args);
    va_end(args);
}

void SKY_log_unimplemented(int level, conf_object_t * obj, int grp, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    VT_log_message_fmt_va(level, obj, grp, SKY_Log_Unimplemented, fmt, args);
    va_end(args);
}

void SKY_log_error(conf_object_t * obj, int grp, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    VT_log_message_fmt_va(SKY_LOG_LEVEL_MAX, obj, grp, SKY_Log_Error, fmt, args);
    va_end(args);
}

unsigned SKY_log_level(const conf_object_t * obj)
{
    if (obj->log)
        return obj->log->log_level;
    else
        return 0;
}

void SKY_set_log_level(const conf_object_t * obj, unsigned level)
{
    if (level > SKY_LOG_LEVEL_MAX)
    {
        SKY_log_error(NULL, 0, "trying to set a error log level, log level range[1..4]");
        level = SKY_LOG_LEVEL_MAX;
    }
    if (obj->log)
        obj->log->log_level = level;
}

void SKY_set_quiet(bool_t quiet)
{
    skyeye_config_t *config = get_current_config();

    config->log.quiet = quiet;
}

void SKY_set_verbose(bool_t verbose)
{
    skyeye_config_t *config = get_current_config();

    config->log.quiet = !verbose;
}
