#include <stdio.h>
#include "skyeye_log.h"
#include "skyeye_disassemble.h"
#include "skyeye_types.h"

static disassemble_t disassemble_global = NULL;
exception_t register_disassemble_func(disassemble_t disassemble)
{
    disassemble_global = disassemble;
    return No_exp;
}

char *skyeye_disassemble(conf_object_t * core, int32_t addr)
{
    if (disassemble_global == NULL)
    {
        skyeye_log(Warning_log, __FUNCTION__, "No disassemble function is registered\n");
        return "NULL";
    }
    return disassemble_global(core, addr);
}
