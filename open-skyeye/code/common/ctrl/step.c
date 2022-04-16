#include <stdlib.h>

#include "skyeye_core.h"
#include "skyeye_callback.h"
#include "skyeye_command.h"
#include "sim_control.h"
#include "skyeye_log.h"

#include "skyeye_core_intf.h"
#include "skyeye_interface.h"
#include "skyeye_iface.h"
/*
 * That is an absolute value for step. Simulator should stop when hit this value 
 */
static int stopped_step = 0;

/**
 * @brief the handler of show step command
 *
 * @param arg
 *
 * @return 
 */
int com_show_step(char *cpuname)
{
    conf_object_t *core = get_conf_obj(cpuname);

    if (core == NULL)
        return 0;

    uint64_t step = skyeye_get_core_steps(core);

    printf("steps: %lld\n", step);

    return No_exp;
}

void skyeye_stepi(int steps)
{
    /* FIXME, that is not true for variable length of ISA, so get_next_pc should implemented for every core  */
    stopped_step = SIM_get_current_steps() + steps;
    skyeye_log(Debug_log, __FUNCTION__, "stopped_step=%d\n", stopped_step);
    SIM_continue(NULL);
    while (SIM_is_running()) ;
}

void skyeye_core_stepi(conf_object_t * core, uint64_t steps)
{
    core_exec_intf *core_exec = (core_exec_intf *) SKY_get_iface(core, CORE_EXEC_INTF_NAME);

    core_exec->stepi(core, steps);
    SIM_continue(NULL);
    while (SIM_is_running()) ;
}
