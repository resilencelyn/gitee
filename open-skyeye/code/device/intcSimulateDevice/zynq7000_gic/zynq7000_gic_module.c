#include <skyeye_module.h>

const char* skyeye_module = "zynq7000_gic";
extern void init_zynq7000_gic();

void module_init()
{
    init_zynq7000_gic();
}

void module_fini()
{
}
