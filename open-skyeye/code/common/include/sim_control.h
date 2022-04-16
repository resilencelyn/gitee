#ifndef __COMMON_SKY_CONTROL_H__
#define __COMMON_SKY_CONTROL_H__
#include "skyeye_arch.h"

#ifdef __cplusplus
 extern "C" {
#endif 
//void SIM_init_command_line(void);
//void SIM_init_environment(char **argv, bool handle_signals);
//void SIM_init_simulators(init_prefs_t *init_prefs);

enum mach_status
{
    LOADED_MACH = 0,
    UNLOADE_MACH = 1
};

struct skyeye_sim_ctrl
{
    bool_t SIM_running;
    bool_t SIM_started;
    enum mach_status is_load_machine;
};
typedef struct skyeye_sim_ctrl * skyeye_sim_ctrl_t;

void init_sim_machine(void);
void sim_reset_machine(void);
void sim_record_machine(void);
void sim_clear_machine(void);
int sim_find_machine(void);

/*
 * initialization of simulator.
 */
void SIM_init(void);

/*
 * read and parse the specific config file and initilization the data.
 */
exception_t SIM_start(void);

/*
 * Launch the simulator.
 */
int SIM_run(void);


/*
 * reset the simulator.
 */
void SIM_reset(void);

/*
 * quit the simulator.
 */
void SIM_quit(void);




/*
 * Enters the main loop of SkyEye and never returns. It should only be called
 * When embedding SkyEye in another application that wishes to transfer full 
 * control of the simulations to SkyEye.
 */
void SIM_main_loop(void);

/*
 * Ask SkyEye to stop the simulation as soon as possible, displaying the supplied 
 * messages.
 */
void SIM_break_simulation(const char *msg);

/*
 * Run the simulation. if step is zero, the simulation will run forward until it
 * is stoppped.
 */
void SIM_continue(generic_arch_t* arch_instance);
/* 
 * stop a processor 
 */
int SIM_stop(generic_arch_t* arch_instance);
void SIM_restart(void);

bool_t SIM_is_running(void);

void SIM_fini(void);
void skyeye_loop(generic_arch_t *arch_instance);
void skyeye_stepi(int steps);
void skyeye_core_stepi(conf_object_t *core, uint64_t steps);
#ifdef __cplusplus
}
#endif 

#endif
