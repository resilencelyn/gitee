#include <skyeye_module.h>

const char* skyeye_module = "zynq7000_uart";
extern void init_zynq7000_uart();

void module_init()
{
    init_zynq7000_uart();
}

void module_fini()
{
}
