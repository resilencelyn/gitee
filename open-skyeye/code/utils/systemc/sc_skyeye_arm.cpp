#include "sc_skyeye_arm.h"
#include "systemc_exec.h"
#include "sim_control.h"

void sc_skyeye_arm::cpu_call()
{
    while (1)
    {
        exec_post_one_cycle();
    }

}
