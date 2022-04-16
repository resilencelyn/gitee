#include <skyeye_module.h>

const char *skyeye_module = "disassembler";

extern void clear_disassemble_buf();
void module_init()
{
    clear_disassemble_buf();
}

void module_fini()
{
}
