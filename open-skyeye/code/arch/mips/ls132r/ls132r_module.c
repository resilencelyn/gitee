#include <skyeye_module.h>
const char *skyeye_module = "ls132r_core";

extern void init_ls132r(void);

void module_init()
{
    init_ls132r();
}

void module_fini()
{
}
