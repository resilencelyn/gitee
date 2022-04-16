#ifndef __SKYEYE_COMMAND_H__
#define __SKYEYE_COMMAND_H__

#ifdef __cplusplus
extern "C" {
#endif

void skyeye_cli();
typedef int (*command_func_t)(char* arg);
exception_t add_command(char* command_name, command_func_t func, char* helper);
exception_t run_command(char* command_str);

#ifdef __cplusplus
}
#endif

#endif
