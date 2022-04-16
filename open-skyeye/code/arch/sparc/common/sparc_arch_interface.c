/*
 * =====================================================================================
 *
 *       Filename:  sparc_arch_interface.c
 *
 *    Description:  Implements the SPARC architecture setup
 *
 *        Version:  1.0
 *        Created:  15/04/08 12:07:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  23/06/08 15:24:39 Aitor Viana Sanchez
 *-----------------------------------------------------------------------------*/
#include "skyeye_types.h"
#include "skyeye_config.h"
#include "skyeye_options.h"
#include "sparc_regformat.h"
#include "skyeye_exec.h"
#include "skyeye_arch.h"
#include <skyeye_core_intf.h>
#include <skyeye_mach.h>
#include "types.h"
#include "traps.h"
#include "sparc.h"
#include "fpu.h"
#include "iu.h"
#include "sparc_arch_interface.h"
/* shenoubang 2012-5-4 */
#include "bits.h"

void sparc_step_once(conf_object_t * conf_obj);

void sparc_init_state(conf_object_t * conf_obj)
{
    sparc_return_t status;
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    status = init_sparc_iu(sparc_state);
    status = init_sparc_fpu(sparc_state);

    /*  Init the FPU unit in case there is one  */
    //  init_sparc_FPU();

    if (status != SPARC_SUCCESS)
        SKYEYE_ERR("%s(): ISA init error\n", __func__);

    DBG("In %s, the above code not finished porting.\n", __FUNCTION__);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sparc_reset_state
 *  Description:  This function RESETs the SPARC architecture.
 * =====================================================================================
 */
void sparc_reset(conf_object_t * conf_obj)
{
}

void sparc_run(conf_object_t * obj)
{
    sparc_step_once(obj);
    return;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sparc_step_once
 *  Description:  This function performs step execution. The function handles
 *  the possible pending traps, and then call the Integer Unit to execute the
 *  next instruction
 * =====================================================================================
 */
void sparc_step_once(conf_object_t * conf_obj)
{
    sparc_state_t *sparc_state = conf_obj->obj;

    /*  Execute the next instruction    */
    /*  FIXME:  n_cycles must be used to update the system clock    */

    iu->iu_cycle_step(conf_obj);

    /*  Check for insterrupts   */
    iu->iu_trap_cycle(conf_obj);

}

char *sparc_get_regname_by_id(conf_object_t * conf_obj, uint32_t id)
{
    return sparc_regstr[id];
}

static int sparc_parse_cpu(const char *param[])
{
    /*  FIXME! to be implemented    */
    SKYEYE_DBG("%s()\n", __func__);

    return 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sparc_register_iu
 *  Description:  This function registers the SPARC Integer Unit module
 * =====================================================================================
 */
void sparc_register_iu(conf_object_t * conf_obj, iu_config_t * iu_)
{
    SKYEYE_DBG("%s: Registering the IU\n", __func__);

    if (!iu_)
    {
        SKYEYE_ERR("%s(): Error registering the IU\n", __func__);
        skyeye_exit(1);
    }
    iu = iu_;
    iu->iu_reset_state(conf_obj);
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    iu->iu_init_state(sparc_state);
}

endian_t sparc_get_endian(conf_object_t * obj)
{
    return Big_endian;
}

align_t sparc_get_alignment(conf_object_t * obj)
{
    return UnAlign;
}

#define REG_NUM 74
uint32 sparc_get_regnum(conf_object_t * conf_obj)
{
    return REG_NUM;
}

static uint32 sparc_get_regval_by_id(conf_object_t * conf_obj, int id)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    if (id < 32)
        return REG(id);
    else if (id < 64)
        return FPREG(id - 32);
    switch (id - 64)
    {
        case 0:
            return YREG;
        case 1:
            return PSRREG;
        case 2:
            return WIMREG;
        case 3:
            return TBRREG;
        case 4:
            return PCREG;
        case 5:
            return NPCREG;
        case 6:
            return FPSRREG;
        case 7:
            return CPSRREG;
        case 8:
            return CWP;
        default:
            printf("have not register id %d\n", id);
            return -1;
    }
    return 0;
}
