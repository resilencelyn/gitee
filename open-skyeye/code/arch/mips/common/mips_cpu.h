#ifndef __MIPS_CPU_H__
#define __MIPS_CPU_H__

#include <skyeye_thread.h>
#include <skyeye_obj.h>
#include <skyeye_mach.h>
#include <skyeye_exec.h>
#include <skyeye_config.h>
#include "skyeye_types.h"
#include <skyeye_lock.h>
#include <skyeye_addr_space.h>

#include <stddef.h>
#include <stdio.h>
#include "emul.h"

#include <skyeye_system.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct MIPS_CPU_State_s
    {
        mips_core_t *core;
        uint32_t core_num;
        /* The core id that boot from
         */
        uint32_t boot_core_id;
    } MIPS_CPU_State;

    static inline mips_core_t *get_current_core()
    {
        skyeye_system_t *system = system_get();
          return (mips_core_t *) (system->socs[0].cpus[0].cpu->obj);
    }

    static inline MIPS_CPU_State *get_current_cpu()
    {
        mips_core_t *core = get_current_core();
        MIPS_CPU_State *cpu = (MIPS_CPU_State *) core->cpu;

        return cpu;
    }

/**
* @brief Get the core instance boot from
*
* @return
*/
    static inline MIPS_State *get_boot_core()
    {
        MIPS_CPU_State *cpu = get_current_cpu();

        return &cpu->core[cpu->boot_core_id];
    }

    void ls132r_step_once(conf_object_t * conf_obj);
    void gs32eb1_step_once(conf_object_t * conf_obj);

#define CURRENT_CORE get_current_core()
#endif
