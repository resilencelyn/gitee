#include <skyeye_module.h>

const char* skyeye_module = "mpc8247_memorycontroller";
extern void init_mpc8247_memorycontroller();

void module_init()
{
    init_mpc8247_memorycontroller();
}

void module_fini()
{
}
