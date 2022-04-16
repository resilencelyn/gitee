#include <skyeye_module.h>
const char *skyeye_module = "gs32eb1_core";
extern void init_gs32eb1(void);
void module_init()
{
    init_gs32eb1();
}

void module_fini()
{
}
