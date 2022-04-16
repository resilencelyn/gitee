#include <skyeye_module.h>

const char* skyeye_module = "zynq7000_cptimer";
extern void init_zynq7000_cptimer();

void module_init()
{
    init_zynq7000_cptimer();
}

void module_fini()
{
}
