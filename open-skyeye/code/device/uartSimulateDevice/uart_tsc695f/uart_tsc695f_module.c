#include "skyeye_module.h"

const char* skyeye_module = "uart_tsc695f";

extern void init_tsc695f_uart();

void module_init()
{
    init_tsc695f_uart();
}

void module_fini()
{
}
