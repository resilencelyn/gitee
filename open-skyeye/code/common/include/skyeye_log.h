#ifndef __SKYEYE_LOG_H__
#define __SKYEYE_LOG_H__
#include "skyeye_types.h"
#include "skyeye_options.h"
#include "skyeye_obj.h"
#include "skyeye_class.h"
#include "skyeye_attr.h"
#include <stdio.h>
#include <assert.h>
#ifdef __cplusplus
 extern "C" {
#endif

#ifdef DEBUG
#define DBG(fmt, ...) do { fprintf(stderr, fmt, ## __VA_ARGS__); } while (0)
#else
#define DBG(fmt, ...) do { } while (0)
#endif
typedef enum{
	Quiet_log = 0, /* Only output the necessary information for the user */
	Info_log , /* Some addtional information to see how about the system */
	Debug_log, /* Debug information for programmer */
	Warning_log, /* Something warning information, maybe something wrong */
	Error_log, /* Something really error, but maybe still work or can be recover */
	Critical_log, /* Can not recover and the system should be terminated immediately */
	MAX_LOG_LEVEL,
}log_level_t;

typedef enum {
	RED,
	LIGHT_RED,
	BLACK,
	DARK_GRAY,
	BLUE,
	LIGHT_BLUE,
	GREEN,
	LIGHT_GREEN,
	CYAN,
	LIGHT_CYAN,
	PURPLE,
	LIGHT_PURPLE,
	BROWN,
	YELLOW,
	LIGHT_GRAY,
	WHITE
} COLOR_TYPE;

#define _debug_in_red(fmt,...)		printf("\033[0;31m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_light_red(fmt,...)	printf("\033[1;31m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_black(fmt,...)	printf("\033[0;30m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_dark_gray(fmt,...)	printf("\033[1;30m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_blue(fmt,...)		printf("\033[0;34m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_light_blue(fmt,...)	printf("\033[1;34m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_green(fmt,...)	printf("\033[0;32m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_light_green(fmt,...)	printf("\033[1;32m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_cyan(fmt,...)		printf("\033[0;36m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_light_cyan(fmt,...)	printf("\033[1;36m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_purple(fmt,...)	printf("\033[0;35m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_light_purple(fmt,...)	printf("\033[1;35m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_brown(fmt,...)	printf("\033[0;33m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_yellow(fmt,...)	printf("\033[1;33m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_light_gray(fmt,...)	printf("\033[0;37m"fmt"\033[0m", ## __VA_ARGS__)
#define _debug_in_white(fmt,...)	printf("\033[1;37m"fmt"\033[0m", ## __VA_ARGS__)

void skyeye_printf_in_color(COLOR_TYPE type, char *format, ...);
void skyeye_log_in_color(log_level_t log_level,const char* func_name, char* format, ...);
void skyeye_log(log_level_t log_level, const char* func_name, const char* format,...);
exception_t set_logfile_dir(char *dir);
char *get_logfile_dir();

char* get_front_message(void);

char* get_exp_str(exception_t exp);
void skyeye_info(char* format, ...);
void skyeye_debug(char* format, ...);
void skyeye_error(char* format, ...);

void skyeye_backtrace();

int
do_log_option (skyeye_option_t * this_option, int num_params,
	       const char *params[]);

char *skyeye_get_next_log();

/**
 * new log interface
 * date: 2014-03-18
 */

typedef enum {
	SKY_Log_Info,
	SKY_Log_Error,
	SKY_Log_Undefined,
	SKY_Log_Spec_Violation,
	SKY_Log_Unimplemented,
	SKY_Log_Num_Types,
}SKY_log_type_t;

#define SKY_LOG_LEVEL_MAX 4

typedef exception_t (*log_intf_output)(conf_object_t *log_obj, conf_object_t *obj,
		int level, int grp, SKY_log_type_t typ, const char *fmt, va_list args);
typedef exception_t (*log_intf_ctrl)(conf_object_t *log_obj, conf_object_t *obj,
		const char *attr_name, attr_value_t value);

typedef struct log_interface {
	log_intf_output output;
	log_intf_ctrl   ctrl;
}log_interface_t;

#define LOG_INTERFACE "log_interface"

struct log_object {
	conf_object_t *log_obj;
	log_interface_t *log_intf;
	int log_level;
	int group_mask;
	int type_mask;
};

void init_log_object(conf_object_t *obj);
void init_log_groups(conf_class_t *cls);

bool_t SKY_get_quiet(void);
bool_t SKY_get_verbose(void);

/**
 * level should be between 1 and 4:
 * 1. important messages that are always printed
 * 2. "high-level" information messages
 * 3. standard debug messages
 * 4. detailed information, such as register accesses
 */
#define FMT_CHECK(a, b) __attribute__((format(printf,a,b)))
void VT_log_message_fmt(int level, conf_object_t *obj, int grp, SKY_log_type_t typ,
                        const char *fmt, ...) FMT_CHECK(5, 6);
void VT_log_message_fmt_va(int level, conf_object_t *obj, int grp, SKY_log_type_t typ,
                           const char *fmt, va_list va);
void SKY_log_info(int level, conf_object_t *obj, int grp,
		const char *fmt, ...) FMT_CHECK(4, 5);
void SKY_log_undefined(int level, conf_object_t *obj, int grp,
		const char *fmt, ...) FMT_CHECK(4, 5);
void SKY_log_spec_violation(int level, conf_object_t *obj, int grp,
		const char *fmt, ...) FMT_CHECK(4, 5);
void SKY_log_unimplemented(int level, conf_object_t *obj, int grp,
		const char *fmt, ...) FMT_CHECK(4, 5);
void SKY_log_error(conf_object_t *obj, int grp,
		const char *fmt, ...) FMT_CHECK(3, 4);

/**
 * the level must be in the range 0..4 inclusive
 * higher values mean more detailed logging
 */
unsigned SKY_log_level(const conf_object_t *obj);
void SKY_set_log_level(const conf_object_t *obj, unsigned level);

void SKY_log_register_groups(conf_class_t *cls, const char *const *names);
const char* SKY_log_get_group(conf_class_t *cls, int idx);
void SKY_set_quiet(bool_t quiet);
void SKY_set_verbose(bool_t verbose);

#ifdef __cplusplus
}
#endif

#endif
