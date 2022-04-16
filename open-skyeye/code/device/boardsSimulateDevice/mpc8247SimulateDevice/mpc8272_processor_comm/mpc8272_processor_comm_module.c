#include <skyeye_module.h>

const char* skyeye_module = "mpc8272_processor_comm";
extern void init_mpc8272_processor_comm();

void module_init()
{
    init_mpc8272_processor_comm();
}

void module_fini()
{
}
