#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "skyeye_types.h"
#include "skyeye_command.h"
#include "skyeye_arch.h"
#include "skyeye_module.h"
#include "skyeye_disassemble.h"
const char *skyeye_module = "disassemble";

extern char *disassemble_func(conf_object_t * core, generic_address_t addr);

/*
 * int com_disassemble(char* arg){
 * char** endptr;
 * generic_address_t addr;
 * generic_address_t count = 1;
 * int result;
 * int i = 0;
 * 
 * if(arg == NULL || *arg == '\0'){
 * generic_arch_t* arch_instance = get_arch_instance(NULL);
 * addr = arch_instance->get_pc();
 * }
 * else{
 * result = dis_getaddr(arg, &addr);
 * if(result == -1){
 * return 0;
 * }else if(addr == 0){
 * generic_arch_t* arch_instance = get_arch_instance(NULL);
 * addr = arch_instance->get_pc();
 * }
 * result = dis_getcount(arg, &count);
 * if(result == -1 || count == 0)
 * count = 1;
 * 
 * }
 * #if 0
 * addr = strtoul(arg, endptr, 16);
 * if(**endptr != '\0'){
 * printf("Not valid address format.\n");
 * return 1;
 * }
 * errno = 0;
 * addr = strtoul(arg, NULL, 16);
 * if(errno != 0){
 * if(errno == EINVAL)
 * printf("Not valid address format.\n");
 * else if(errno == ERANGE)
 * printf("Out of range for your disassemble address.\n");
 * else
 * printf("Some unknown error in your disassemble command.\n");
 * return 1;
 * }
 * #endif
 * for(i = 0; i < count; i++)
 * disassemble(addr + (i * INSN_LENGTH));
 * return 1;
 * }
 */

void module_init()
{
    register_disassemble_func(disassemble_func);
}

void module_fini()
{
}
